/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Jeff Houde
 */
#include <prefix/dtstageprefix-src.h>
#include "dtEditQt/helpbox.h"
#include "dtEditQt/uiresources.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QTextEdit>
#include <QtGui/QTabWidget>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

namespace dtEditQt
{
   ////////////////////////////////////////////////////////////////////////////////
   DocumentText::DocumentText(QWidget* parent)
      : QTextEdit(parent)
      , mWasOverHyperlink(false)
   {
      setMouseTracking(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocumentText::mouseMoveEvent(QMouseEvent *e)
   {
      QTextEdit::mouseMoveEvent(e);

      QString hyperlink = anchorAt(e->pos());
      if (!hyperlink.isEmpty())
      {
         if (!mWasOverHyperlink)
         {
            QApplication::setOverrideCursor(Qt::PointingHandCursor);
            mWasOverHyperlink = true;
         }
      }
      else if (mWasOverHyperlink)
      {
         QApplication::restoreOverrideCursor();
         mWasOverHyperlink = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocumentText::mousePressEvent(QMouseEvent *e)
   {
      QTextEdit::mousePressEvent(e);

      QString hyperlink = anchorAt(e->pos());
      if (!hyperlink.isEmpty())
      {
         QApplication::restoreOverrideCursor();
         mWasOverHyperlink = false;

         emit hyperlinkClicked(hyperlink);
      }
   }


   //////////////////////////////////////////////////////////////////////////////
   HelpBox::HelpBox(const QString& filename, QWidget* parent)
      : QDialog(parent)
      , mContentList(NULL)
      , mDocumentTabs(NULL)
      , mDocument(NULL)
   {
      // Attempt to load the template file.
      mDocument = new HelpXMLReader();
      mDocument->ReadXML(filename.toStdString());

      setWindowTitle(mDocument->getTitle().c_str());

      mResourcePrefix = mDocument->getResource().c_str();

      QVBoxLayout* mainVLay      = new QVBoxLayout(this);
      QHBoxLayout* okButtonLay   = new QHBoxLayout();
      QSplitter*   documentSplit = new QSplitter(Qt::Horizontal);

      QPushButton* ok            = new QPushButton("OK", this);

      mContentList               = new QTreeWidget(documentSplit);
      mDocumentTabs              = new QTabWidget(documentSplit);

      documentSplit->setStretchFactor(0, 1);
      documentSplit->setStretchFactor(1, 2);

      mDocumentTabs->setMovable(true);
      mDocumentTabs->addTab(createDocument(mDocument->getHome().c_str()), mDocument->getHome().c_str());
      setupContents();

      okButtonLay->addStretch(1);
      okButtonLay->addWidget(ok);
      okButtonLay->addStretch(1);

      mainVLay->addWidget(documentSplit);
      mainVLay->addLayout(okButtonLay);

      connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
      connect(mContentList, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
         this, SLOT(onContentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   HelpBox::~HelpBox()
   {
      if (mDocument)
      {
         delete mDocument;
         mDocument = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onHyperlinkClicked(const QString& link)
   {
      // Search the table of contents for a page that references the same link location.
      QTreeWidgetItem* item = findContents(link);
      if (item)
      {
         // If the page is listed in the table of contents, select that item.
         mContentList->setCurrentItem(item);
         return;
      }

      // If the contents doesn't list this page, then just open it.
      openPage(link);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onContentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
   {
      openPage(current->statusTip(0));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::openPage(const QString& page)
   {
      int index = mDocumentTabs->currentIndex();
      QTextEdit* newDoc = createDocument(page);

      if (index < mDocumentTabs->count())
      {
         disconnect(mDocumentTabs->currentWidget(),
            SIGNAL(hyperlinkClicked(const QString&)),
            this, SLOT(onHyperlinkClicked(const QString&)));

         mDocumentTabs->removeTab(index);
         mDocumentTabs->insertTab(index, newDoc, page);
         mDocumentTabs->setCurrentIndex(index);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTextEdit* HelpBox::createDocument(const QString& filename)
   {
      DocumentText* textEdit = new DocumentText(this);

      textEdit->setTextInteractionFlags(Qt::TextBrowserInteraction);

      // Load the document page.
      QFile file(mResourcePrefix + filename);
      if (!file.open(QFile::ReadOnly | QFile::Text))
      {
         textEdit->setPlainText("Could not locate file.");
      }
      else
      {
         QTextStream in(&file);
         QString data = in.readAll();

         // Prepend the resource prefix to all images referenced.
         data.replace("<img src=\"", "<img src=\"" + mResourcePrefix, Qt::CaseInsensitive);

         textEdit->setHtml(data);
      }

      connect(textEdit, SIGNAL(hyperlinkClicked(const QString&)),
         this, SLOT(onHyperlinkClicked(const QString&)));

      return textEdit;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::setupContents()
   {
      if (!mContentList) return;
      if (!mDocument) return;

      QTreeWidgetItem* root = new QTreeWidgetItem(NULL);
      root->setText(0, "Table of Contents");

      mContentList->addTopLevelItem(root);

      std::vector<HelpXMLReader::SectionInfo*> sections = mDocument->getSections();
      setupContents(sections, root);

      mContentList->expandAll();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::setupContents(const std::vector<HelpXMLReader::SectionInfo*>& sections, QTreeWidgetItem* parent)
   {
      if (!parent) return;

      for (int index = 0; index < (int)sections.size(); index++)
      {
         HelpXMLReader::SectionInfo* section = sections[index];
         if (section)
         {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(parent);
            treeItem->setText(0, section->title.c_str());
            treeItem->setStatusTip(0, section->link.c_str());

            setupContents(section->children, treeItem);

            if (mDocument->getHome() == section->link)
            {
               mContentList->setCurrentItem(treeItem);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTreeWidgetItem* HelpBox::findContents(const QString& page)
   {
      return findContents(page, mContentList->topLevelItem(0));
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTreeWidgetItem* HelpBox::findContents(const QString& page, QTreeWidgetItem* parent)
   {
      if (!parent) return NULL;

      for (int index = 0; index < parent->childCount(); index++)
      {
         QTreeWidgetItem* child = parent->child(index);
         if (child)
         {
            if (child->statusTip(0) == page)
            {
               return child;
            }

            QTreeWidgetItem* found = findContents(page, child);

            if (found) return found;
         }
      }

      return NULL;
   }

} // namespace dtEditQt
