/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Jeff P. Houde
 */

#include <prefix/dtqtprefix.h>
#include <dtQt/docbrowser.h>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QtGui/QTabWidget>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

const QString USING_HELP_TAG = "Using Help!";

namespace dtQt
{
   ////////////////////////////////////////////////////////////////////////////////
   DocBrowserTabs::DocBrowserTabs(QWidget* parent, DocBrowser* docBrowser)
      : QTabWidget(parent)
      , mDocBrowser(docBrowser)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserTabs::mousePressEvent(QMouseEvent *e)
   {
      QTabWidget::mousePressEvent(e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserTabs::mouseReleaseEvent(QMouseEvent *e)
   {
      QTabWidget::mouseReleaseEvent(e);

      if (e->button() == Qt::MidButton)
      {
         // TODO: Find the tab that you are mousing over.
         //mDocBrowser->OnDocumentTabClosed(currentIndex());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserTabs::mouseDoubleClickEvent(QMouseEvent *e)
   {
      QTabWidget::mouseDoubleClickEvent(e);

      // Create a new page and set it to the home page.
      mDocBrowser->OpenPage("", true);
      mDocBrowser->OnHyperlinkClicked(mDocBrowser->GetHome());
   }

   ////////////////////////////////////////////////////////////////////////////////
   DocBrowserText::DocBrowserText(QWidget* parent)
      : QWebView(parent)
      , mWasOverHyperlink(false)
      , mCurrentPage(-1)
   {
      setMouseTracking(true);
      page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserText::SetPage(const QString& page)
   {
      // First clear all history that is beyond our current page.
      // This removes all next pages if we have backed up any.
      if (mCurrentPage > -1 && mCurrentPage < (int)mHistory.size() - 1)
      {
         mHistory.erase(mHistory.begin() + mCurrentPage + 1, mHistory.end());
      }

      DocHistory data;
      data.page = page;

      mHistory.push_back(data);
      mCurrentPage = (int)mHistory.size() - 1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocBrowserText::GetPage(QString& page)
   {
      if (mCurrentPage > -1 && mCurrentPage < (int)mHistory.size())
      {
         page = mHistory[mCurrentPage].page;
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocBrowserText::PrevPage(QString& page)
   {
      if (IsPrevPageValid())
      {
         mCurrentPage--;
         return GetPage(page);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocBrowserText::NextPage(QString& page)
   {
      if (IsNextPageValid())
      {
         mCurrentPage++;
         return GetPage(page);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocBrowserText::IsPrevPageValid()
   {
      if (mCurrentPage > 0) return true;
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocBrowserText::IsNextPageValid()
   {
      if (mCurrentPage > -1 && mCurrentPage < (int)mHistory.size() - 1) return true;
      return false;
   }


   //////////////////////////////////////////////////////////////////////////////
   DocBrowser::DocBrowser(const QString& filename, QWidget* parent)
      : QMainWindow(parent, Qt::Window)
      , mContentList(NULL)
      , mDocumentTabs(NULL)
      , mDocument(NULL)
   {
      // Set the default size of the window.
      resize(900, 600);

      // Attempt to load the template file.
      mDocument = new DocBrowserXMLReader();
      mDocument->ReadXML(filename.toStdString());

      setWindowTitle(mDocument->getTitle().c_str());

      mResourcePrefix = mDocument->getResource().c_str();

      // Setup the UI
      QToolBar*    toolbar       = new QToolBar(this);
      QSplitter*   documentSplit = new QSplitter(Qt::Horizontal);

      mPrevPageAction            = new QAction(tr("Back"), this);
      mNextPageAction            = new QAction(tr("Forward"), this);
      mHomeAction                = new QAction(tr("Home"), this);

      mContentList               = new QTreeWidget(documentSplit);
      mDocumentTabs              = new DocBrowserTabs(documentSplit, this);

      // Toolbar
      addToolBar(toolbar);
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
      //documentSplit->setStretchFactor(0, 1);
      //documentSplit->setStretchFactor(1, 2);

      // Document tabs.
      mDocumentTabs->setTabsClosable(true);
      mDocumentTabs->setMovable(true);
      mDocumentTabs->setTabShape(QTabWidget::Rounded);

      // Main layout.
      setCentralWidget(documentSplit);

      // Setup our table of contents.
      SetupContents();

      // Connect slots.
      connect(mContentList, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
         this, SLOT(OnContentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
      connect(mDocumentTabs, SIGNAL(currentChanged(int)),
         this, SLOT(OnDocumentTabChanged(int)));
      connect(mDocumentTabs, SIGNAL(tabCloseRequested(int)),
         this, SLOT(OnDocumentTabClosed(int)));

      connect(mPrevPageAction, SIGNAL(triggered()),
         this, SLOT(OnPrevButton()));
      connect(mNextPageAction, SIGNAL(triggered()),
         this, SLOT(OnNextButton()));
      connect(mHomeAction, SIGNAL(triggered()),
         this, SLOT(OnHomeButton()));

      // Open our using help page, and also our home page.
      OpenUsingHelpPage(true);
      OpenPage("", true);
      OnHyperlinkClicked(GetHome());
   }

   ////////////////////////////////////////////////////////////////////////////////
   DocBrowser::~DocBrowser()
   {
      if (mDocument)
      {
         delete mDocument;
         mDocument = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OpenUsingHelpPage(bool newPage)
   {
      OpenPage("", newPage);

      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         doc->SetPage(USING_HELP_TAG);
         mDocumentTabs->setTabText(mDocumentTabs->currentIndex(), USING_HELP_TAG);

         doc->setHtml(
            "<p align=\"left\">"
               "<font color=\"DarkBlue\"><h2>"
                  "Using Help!"
               "</h2></font>"
            "</p>"
            "<p align=\"center\">"
               "<table align=\"center\" cellpadding=\"2\" cellspacing=\"10\" border=\"0\" width=\"80%\">"
                  "<tr><td>"
                     "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;To navigate the help library, "
                     "use the Table of Contents menu to the left of this window.  You "
                     "may also manage multiple help pages by double clicking an empty "
                     "space on the tab bar above to create new tabs."
                  "</td></tr>"
               "</table>"
            "</p>"
         );
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OpenPage(const QString& page, bool newTab, bool setPage)
   {
      // Create a new page if we are forcing a new page or
      // if we don't have any pages yet.
      if (mDocumentTabs->count() < 1 || newTab)
      {
         DocBrowserText* doc = new DocBrowserText(this);

         connect(doc, SIGNAL(linkClicked(const QUrl&)),
            this, SLOT(OnHyperlinkClicked(const QUrl&)));
         connect(doc, SIGNAL(loadFinished(bool)),
            this, SLOT(OnPageLoadFinished(bool)));

         int index = mDocumentTabs->addTab(doc, "");
         mDocumentTabs->setCurrentIndex(index);
      }

      // Now set up the current tab and page
      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc && !page.isEmpty())
      {
         if (setPage)
         {
            doc->SetPage(page);
         }

         doc->load("qrc:/" + mResourcePrefix + page);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnPrevButton()
   {
      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         if (doc->IsPrevPageValid())
         {
            QString page;
            doc->PrevPage(page);
            OpenPage(page, false, false);

            // Search the table of contents for a page that references the same link location.
            QTreeWidgetItem* item = FindTOC(page);
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
   void DocBrowser::OnNextButton()
   {
      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         if (doc->IsNextPageValid())
         {
            QString page;
            doc->NextPage(page);
            OpenPage(page, false, false);

            // Search the table of contents for a page that references the same link location.
            QTreeWidgetItem* item = FindTOC(page);
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
   void DocBrowser::OnHomeButton()
   {
      // Set the page to the home page.
      OnHyperlinkClicked(GetHome());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnHyperlinkClicked(const QUrl& link)
   {
      // Make sure the link isn't empty
      if (!link.isEmpty())
      {
         // If this is a local resource (starts with qrc:/ then strip that part out the relative path part
         QString page = link.toString();
         if (page.startsWith("qrc:/"))
         {
            page.remove(0, 5 + mResourcePrefix.length());
         }

         // Search the table of contents for a page that references the same link location.
         QTreeWidgetItem* item = FindTOC(page);
         if (item)
         {
            // If the page is listed in the table of contents, select that item.
            mContentList->blockSignals(true);
            mContentList->setCurrentItem(item);
            mContentList->blockSignals(false);
         }

         // If the contents doesn't list this page, then just open it.
         OpenPage(page);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnPageLoadFinished(bool success)
   {
      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (success && doc)
      {
         if (!doc->title().isEmpty())
         {
            mDocumentTabs->setTabText(mDocumentTabs->currentIndex(), doc->title());
         }
      }
      else
      {
         mDocumentTabs->setTabText(mDocumentTabs->currentIndex(), "Failed to load help page...");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnContentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
   {
      if (!current->statusTip(0).isEmpty())
      {
         // If we are clicking our special Using Help button...
         if (current->statusTip(0) == USING_HELP_TAG)
         {
            // Open a special page.
            OpenUsingHelpPage();
            return;
         }

         OnHyperlinkClicked(current->statusTip(0));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnDocumentTabChanged(int index)
   {
      if (index < mDocumentTabs->count())
      {
         DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->widget(index));
         if (doc)
         {
            QString page;
            if (doc->GetPage(page) && !page.isEmpty())
            {
               // Search the table of contents for a page that references the same link location.
               QTreeWidgetItem* item = FindTOC(page);
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
   void DocBrowser::OnDocumentTabClosed(int index)
   {
      if (index < mDocumentTabs->count())
      {
         DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->widget(index));
         if (doc)
         {
            disconnect(doc, SIGNAL(linkClicked(const QUrl&)),
               this, SLOT(OnHyperlinkClicked(const QUrl&)));

            mDocumentTabs->removeTab(index);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::SetupContents()
   {
      if (!mContentList) return;
      if (!mDocument) return;

      QTreeWidgetItem* header = new QTreeWidgetItem();
      header->setText(0, "Table of Contents");
      mContentList->setHeaderItem(header);

      QTreeWidgetItem* usingHelp = new QTreeWidgetItem();
      usingHelp->setText(0, "Using Help");
      usingHelp->setStatusTip(0, USING_HELP_TAG);
      mContentList->addTopLevelItem(usingHelp);

      // start by adding our top level items.
      std::vector<DocBrowserXMLReader::SectionInfo*> sections = mDocument->getSections();
      for (int index = 0; index < (int)sections.size(); index++)
      {
         DocBrowserXMLReader::SectionInfo* section = sections[index];
         if (section)
         {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem();
            treeItem->setText(0, section->title.c_str());
            treeItem->setStatusTip(0, section->link.c_str());

            mContentList->addTopLevelItem(treeItem);
            SetupContents(section->children, treeItem);

            if (section->expanded)
            {
               mContentList->expandItem(treeItem);
            }

            if (mDocument->GetHome() == section->link)
            {
               mContentList->setCurrentItem(treeItem);
            }
         }
      }

      // Resize the splitter so the tree fits nicely
      QSplitter* splitter = dynamic_cast<QSplitter*>(mContentList->parent());
      if (splitter != NULL)
      {
         mContentList->resizeColumnToContents(0);
         int treeWidth = mContentList->columnWidth(0) + mContentList->verticalScrollBar()->height();
         QList<int> sizes;
         sizes.push_back(treeWidth);
         sizes.push_back(width() - splitter->handleWidth() - treeWidth);
         splitter->setSizes(sizes);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::SetupContents(const std::vector<DocBrowserXMLReader::SectionInfo*>& sections, QTreeWidgetItem* parent)
   {
      if (!parent) return;

      for (int index = 0; index < (int)sections.size(); index++)
      {
         DocBrowserXMLReader::SectionInfo* section = sections[index];
         if (section)
         {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(parent);
            treeItem->setText(0, section->title.c_str());
            treeItem->setStatusTip(0, section->link.c_str());

            SetupContents(section->children, treeItem);

            if (section->expanded)
            {
               mContentList->expandItem(treeItem);
            }

            if (mDocument->GetHome() == section->link)
            {
               mContentList->setCurrentItem(treeItem);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTreeWidgetItem* DocBrowser::FindTOC(const QString& page)
   {
      for (int index = 0; index < mContentList->topLevelItemCount(); index++)
      {
         QTreeWidgetItem* item = FindTOC(page, mContentList->topLevelItem(index));
         if (item) return item;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTreeWidgetItem* DocBrowser::FindTOC(const QString& page, QTreeWidgetItem* parent)
   {
      if (!parent) return NULL;

      // If this is it, we match
      if (parent->statusTip(0) == page)
      {
         return parent;
      }

      // If this isn't it, then check our children
      for (int index = 0; index < parent->childCount(); index++)
      {
         QTreeWidgetItem* child = parent->child(index);
         if (child)
         {
            QTreeWidgetItem* found = FindTOC(page, child);
            if (found) return found;
         }
      }

      return NULL;
   }

} // namespace dtQt
