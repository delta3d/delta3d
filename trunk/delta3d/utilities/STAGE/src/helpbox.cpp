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
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QTextEdit>
#include <QtGui/QTabWidget>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

const QString USING_HELP_TAG = "Using Help!";

namespace dtEditQt
{
   ////////////////////////////////////////////////////////////////////////////////
   DocumentText::DocumentText(QWidget* parent)
      : QTextEdit(parent)
      , mWasOverHyperlink(false)
      , mCurrentPage(-1)
   {
      setMouseTracking(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocumentText::setPage(const QString& page, const QString& anchor)
   {
      // First clear all history that is beyond our current page.
      // This removes all next pages if we have backed up any.
      if (mCurrentPage > -1 && mCurrentPage < (int)mHistory.size() - 1)
      {
         mHistory.erase(mHistory.begin() + mCurrentPage + 1, mHistory.end());
      }

      DocHistory data;
      data.page = page;
      data.anchor = anchor;

      mHistory.push_back(data);
      mCurrentPage = (int)mHistory.size() - 1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocumentText::getPage(QString& page, QString& anchor)
   {
      if (mCurrentPage > -1 && mCurrentPage < (int)mHistory.size())
      {
         page = mHistory[mCurrentPage].page;
         anchor = mHistory[mCurrentPage].anchor;
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocumentText::prevPage(QString& page, QString& anchor)
   {
      if (isPrevPage())
      {
         mCurrentPage--;
         return getPage(page, anchor);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocumentText::nextPage(QString& page, QString& anchor)
   {
      if (isNextPage())
      {
         mCurrentPage++;
         return getPage(page, anchor);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocumentText::isPrevPage()
   {
      if (mCurrentPage > 0) return true;
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocumentText::isNextPage()
   {
      if (mCurrentPage > -1 && mCurrentPage < (int)mHistory.size() - 1) return true;
      return false;
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

      // Setup the UI
      QVBoxLayout* mainVLay      = new QVBoxLayout(this);
      QToolBar*    toolbar       = new QToolBar(this);
      QHBoxLayout* okButtonLay   = new QHBoxLayout();
      QSplitter*   documentSplit = new QSplitter(Qt::Horizontal);

      mPrevPageAction            = new QAction(tr("Back"), this);
      mNextPageAction            = new QAction(tr("Forward"), this);
      mHomeAction                = new QAction(tr("Home"), this);

      QPushButton* ok            = new QPushButton("OK", this);

      mContentList               = new QTreeWidget(documentSplit);
      mDocumentTabs              = new QTabWidget(documentSplit);

      // Toolbar
      toolbar->setObjectName("Toolbar");
      toolbar->setWindowTitle(tr("Toolbar"));

      mPrevPageAction->setShortcut(tr("Backspace"));
      mPrevPageAction->setStatusTip(tr("Returns to a previous page"));
      toolbar->addAction(mPrevPageAction);

      mNextPageAction->setStatusTip(tr("Returns to the next page"));
      toolbar->addAction(mNextPageAction);

      toolbar->addSeparator();

      mHomeAction->setStatusTip(tr("Returns to the home page"));
      toolbar->addAction(mHomeAction);

      // Document splitter.
      documentSplit->setStretchFactor(0, 1);
      documentSplit->setStretchFactor(1, 2);

      // Document tabs.
      mDocumentTabs->setTabsClosable(true);
      mDocumentTabs->setMovable(true);
      mDocumentTabs->setTabShape(QTabWidget::Rounded);

      // Ok button.
      okButtonLay->addStretch(1);
      okButtonLay->addWidget(ok);
      okButtonLay->addStretch(1);

      // Main layout.
      mainVLay->addWidget(toolbar);
      mainVLay->addWidget(documentSplit);
      mainVLay->addLayout(okButtonLay);

      // Setup our table of contents.
      setupContents();

      // Connect slots.
      connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
      connect(mContentList, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
         this, SLOT(onContentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
      connect(mDocumentTabs, SIGNAL(currentChanged(int)),
         this, SLOT(onDocumentTabChanged(int)));
      connect(mDocumentTabs, SIGNAL(tabCloseRequested(int)),
         this, SLOT(onDocumentTabClosed(int)));

      connect(mPrevPageAction, SIGNAL(triggered()),
         this, SLOT(onPrevButton()));
      connect(mNextPageAction, SIGNAL(triggered()),
         this, SLOT(onNextButton()));
      connect(mHomeAction, SIGNAL(triggered()),
         this, SLOT(onHomeButton()));

      // Open our home page to start with.
      openUsingHelpPage(true);
      openPage("", "", true);
      onHyperlinkClicked(mDocument->getHome().c_str());
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
   void HelpBox::onPrevButton()
   {
      DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         if (doc->isPrevPage())
         {
            QString page, anchor;
            doc->prevPage(page, anchor);
            openPage(page, anchor, false, false);

            // Search the table of contents for a page that references the same link location.
            QTreeWidgetItem* item = findTOC(page, anchor);
            if (item)
            {
               // If the page is listed in the table of contents, select that item.
               mContentList->blockSignals(true);
               mContentList->setCurrentItem(item);
               mContentList->blockSignals(false);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onNextButton()
   {
      DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         if (doc->isNextPage())
         {
            QString page, anchor;
            doc->nextPage(page, anchor);
            openPage(page, anchor, false, false);

            // Search the table of contents for a page that references the same link location.
            QTreeWidgetItem* item = findTOC(page, anchor);
            if (item)
            {
               // If the page is listed in the table of contents, select that item.
               mContentList->blockSignals(true);
               mContentList->setCurrentItem(item);
               mContentList->blockSignals(false);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onHomeButton()
   {
      // Set the page to the home page.
      onHyperlinkClicked(mDocument->getHome().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onHyperlinkClicked(const QString& link)
   {
      // First parse the link to make sure we are not selecting a web page.
      if (link.contains("http:", Qt::CaseInsensitive))
      {
         // TODO: Open a web browser for this link.
         return;
      }

      // Check if this link contains a bookmark.
      QString page = link;
      QString anchor = "";
      int bookmark = page.indexOf("#");
      if (bookmark > -1)
      {
         // Extract the page and anchor separately.
         anchor = page;
         anchor.remove(0, bookmark + 1);
         page.chop(page.length() - bookmark);
      }

      // If the link doesn't contain a page, but does have an anchor,
      // use the current page.
      if (page.isEmpty() && !anchor.isEmpty())
      {
         page = mDocumentTabs->tabText(mDocumentTabs->currentIndex());
      }

      if (!page.isEmpty())
      {
         // Search the table of contents for a page that references the same link location.
         QTreeWidgetItem* item = findTOC(page, anchor);
         if (item)
         {
            // If the page is listed in the table of contents, select that item.
            mContentList->blockSignals(true);
            mContentList->setCurrentItem(item);
            mContentList->blockSignals(false);
         }

         // If the contents doesn't list this page, then just open it.
         openPage(page, anchor);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onContentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
   {
      if (!current->statusTip(0).isEmpty())
      {
         // If we are clicking our special Using Help button...
         if (current->statusTip(0) == USING_HELP_TAG)
         {
            // Open a special page.
            openUsingHelpPage();
            return;
         }

         onHyperlinkClicked(current->statusTip(0));
         //openPage(current->statusTip(0));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onDocumentTabChanged(int index)
   {
      if (index < mDocumentTabs->count())
      {
         DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->widget(index));
         if (doc)
         {
            QString page;
            QString anchor;
            if (doc->getPage(page, anchor) && !page.isEmpty())
            {
               // Search the table of contents for a page that references the same link location.
               QTreeWidgetItem* item = findTOC(page, anchor);
               if (item)
               {
                  // If the page is listed in the table of contents, select that item.
                  mContentList->blockSignals(true);
                  mContentList->setCurrentItem(item);
                  mContentList->blockSignals(false);
                  return;
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onDocumentTabClosed(int index)
   {
      if (index < mDocumentTabs->count())
      {
         DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->widget(index));
         if (doc)
         {
            disconnect(doc, SIGNAL(hyperlinkClicked(const QString&)),
               this, SLOT(onHyperlinkClicked(const QString&)));

            //QScrollBar* scrollBar = doc->verticalScrollBar();
            //if (scrollBar)
            //{
            //   disconnect(scrollBar, SIGNAL(valueChanged(int)),
            //      this, SLOT(onDocumentScrolled(int)));
            //}

            mDocumentTabs->removeTab(index);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::onDocumentScrolled(int value)
   {
      //// Find all anchors in the document up to our current position.
      //DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->currentWidget());
      //if (doc)
      //{
      //   // Find all anchors.
      //   QString tag = "<a name=\"";
      //   //if (doc->find(tag, QTextDocument::FindBackward))
      //   {
      //      QString text = doc->toHtml();
      //      if (!text.isEmpty())
      //      {
      //         // Get the anchor name.
      //         QString anchor = text;
      //         int loc = -1;
      //         while (loc = text.indexOf(tag, loc, Qt::CaseInsensitive) > -1)
      //         {
      //            // make sure this anchor is above the current scroll position.
      //            doc->

      //            anchor.remove(0, loc + tag.length());

      //            // Now chop it off at the end quote.
      //            int endQuote = anchor.indexOf("\"");
      //            if (endQuote > -1)
      //            {
      //               anchor.chop(anchor.length() - endQuote);
      //            }
      //         }

      //         {
      //            // Now find this anchor in our contents.
      //            QTreeWidgetItem* item = findTOC(mDocumentTabs->tabText(mDocumentTabs->currentIndex()), mAnchor, true);
      //            if (item)
      //            {
      //               // If the page is listed in the table of contents, select that item.
      //               mContentList->blockSignals(true);
      //               mContentList->setCurrentItem(item);
      //               mContentList->blockSignals(false);
      //            }
      //         }
      //      }
      //   }
      //}
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::mouseDoubleClickEvent(QMouseEvent *e)
   {
      // Create a new page and set it to the home page.
      openPage("", "", true);
      onHyperlinkClicked(mDocument->getHome().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::openUsingHelpPage(bool newPage)
   {
      openPage("", "", newPage);

      DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         doc->setPage(USING_HELP_TAG, "");
         mDocumentTabs->setTabText(mDocumentTabs->currentIndex(), USING_HELP_TAG);

         doc->setHtml(
	         "<p align=\"left\">"
		         "<font color=\"DarkBlue\"><h2>"
                  "Using Help!"
		         "</h2></font>"
	         "</p>"
            "<p align=\"center\">"
		         //"<table align=\"center\" cellpadding=\"2\" cellspacing=\"10\" border=\"0\" width=\"80%\">"
			         "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;To navigate the help library, "
			         "use the Table of Contents menu to the left of this window or click "
			         "on any links displayed in this view area.  You may also manage "
			         "multiple help pages by double clicking an empty space on the tab "
                  "bar above to create new tabs."
		         //"</table>"
            "</p>"
         );
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::openPage(const QString& page, const QString& anchor, bool newTab, bool setPage)
   {
      // Create a new page if we are forcing a new page or
      // if we don't have any pages yet.
      if (mDocumentTabs->count() < 1 || newTab)
      {
         DocumentText* doc = new DocumentText(this);

         doc->setTextInteractionFlags(Qt::TextBrowserInteraction);

         connect(doc, SIGNAL(hyperlinkClicked(const QString&)),
            this, SLOT(onHyperlinkClicked(const QString&)));

         //QScrollBar* scrollBar = doc->verticalScrollBar();
         //if (scrollBar)
         //{
         //   connect(scrollBar, SIGNAL(valueChanged(int)),
         //      this, SLOT(onDocumentScrolled(int)));
         //}

         int index = mDocumentTabs->addTab(doc, "");
         mDocumentTabs->setCurrentIndex(index);
      }

      DocumentText* doc = dynamic_cast<DocumentText*>(mDocumentTabs->currentWidget());
      if (doc && !page.isEmpty())
      {
         mDocumentTabs->setTabText(mDocumentTabs->currentIndex(), page);

         if (setPage)
         {
            doc->setPage(page, anchor);
         }

         QFile file(mResourcePrefix + page);
         if (!file.open(QFile::ReadOnly | QFile::Text))
         {
            doc->setPlainText("Could not locate file.");
         }
         else
         {
            QTextStream in(&file);
            QString data = in.readAll();

            // Prepend the resource prefix to all images referenced.
            data.replace("<img src=\"", "<img src=\"" + mResourcePrefix, Qt::CaseInsensitive);

            doc->setHtml(data);

            // Scroll to an anchor if needed.
            if (!anchor.isEmpty())
            {
               doc->scrollToAnchor(anchor);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void HelpBox::setupContents()
   {
      if (!mContentList) return;
      if (!mDocument) return;

      QTreeWidgetItem* header = new QTreeWidgetItem(NULL);
      header->setText(0, "Table of Contents");
      mContentList->setHeaderItem(header);

      QTreeWidgetItem* usingHelp = new QTreeWidgetItem(NULL);
      usingHelp->setText(0, "Using Help");
      usingHelp->setStatusTip(0, USING_HELP_TAG);
      mContentList->addTopLevelItem(usingHelp);

      // start by adding our top level items.
      std::vector<HelpXMLReader::SectionInfo*> sections = mDocument->getSections();
      for (int index = 0; index < (int)sections.size(); index++)
      {
         HelpXMLReader::SectionInfo* section = sections[index];
         if (section)
         {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(NULL);
            treeItem->setText(0, section->title.c_str());
            treeItem->setStatusTip(0, section->link.c_str());

            mContentList->addTopLevelItem(treeItem);
            setupContents(section->children, treeItem);

            if (mDocument->getHome() == section->link)
            {
               mContentList->setCurrentItem(treeItem);
            }
         }
      }

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
   QTreeWidgetItem* HelpBox::findTOC(const QString& page, const QString& anchor, bool anchorMustMatch)
   {
      for (int index = 0; index < mContentList->topLevelItemCount(); index++)
      {
         QTreeWidgetItem* item = findTOC(page, anchor, mContentList->topLevelItem(index), anchorMustMatch);
         if (item) return item;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTreeWidgetItem* HelpBox::findTOC(const QString& page, const QString& anchor, QTreeWidgetItem* parent, bool anchorMustMatch)
   {
      if (!parent) return NULL;

      QString link = parent->statusTip(0);

      // Check if this link contains an anchor.
      QString TOCPage = link;
      QString TOCAnchor = "";
      int bookmark = TOCPage.indexOf("#");
      if (bookmark > -1)
      {
         // Extract the page and anchor separately.
         TOCAnchor = TOCPage;
         TOCAnchor.remove(0, bookmark + 1);
         TOCPage.chop(TOCPage.length() - bookmark);
      }

      if (TOCPage == page)
      {
         // If both the page and anchor match exactly, return this result.
         if (TOCAnchor == anchor)
         {
            return parent;
         }

         // If the anchor of the TOC does not match, check the children first.
         for (int index = 0; index < parent->childCount(); index++)
         {
            QTreeWidgetItem* child = parent->child(index);
            if (child)
            {
               QTreeWidgetItem* found = findTOC(page, anchor, child, true);
               if (found) return found;
            }
         }

         // If we aren't enforcing an anchor match, then return this result
         // as it is close enough.
         if (!anchorMustMatch)
         {
            // return the closest page match, ignoring the anchor.
            return parent;
         }
      }

      // If we have not found a matching page yet, check the children.
      for (int index = 0; index < parent->childCount(); index++)
      {
         QTreeWidgetItem* child = parent->child(index);
         if (child)
         {
            QTreeWidgetItem* found = findTOC(page, anchor, child, anchorMustMatch);

            if (found) return found;
         }
      }

      return NULL;
   }

} // namespace dtEditQt
