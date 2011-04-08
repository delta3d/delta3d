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
#include <QtGui/QPixmap>
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
      mDocBrowser->OpenPage("", "", true);
      mDocBrowser->OnHyperlinkClicked(mDocBrowser->GetHome().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   DocBrowserText::DocBrowserText(QWidget* parent)
      : QTextEdit(parent)
      , mWasOverHyperlink(false)
      , mCurrentPage(-1)
   {
      setMouseTracking(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserText::SetPage(const QString& page, const QString& anchor)
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
   bool DocBrowserText::GetPage(QString& page, QString& anchor)
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
   bool DocBrowserText::PrevPage(QString& page, QString& anchor)
   {
      if (IsPrevPageValid())
      {
         mCurrentPage--;
         return GetPage(page, anchor);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DocBrowserText::NextPage(QString& page, QString& anchor)
   {
      if (IsNextPageValid())
      {
         mCurrentPage++;
         return GetPage(page, anchor);
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

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowserText::mouseMoveEvent(QMouseEvent *e)
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
   void DocBrowserText::mousePressEvent(QMouseEvent *e)
   {
      QTextEdit::mousePressEvent(e);

      QString hyperlink = anchorAt(e->pos());
      if (!hyperlink.isEmpty())
      {
         QApplication::restoreOverrideCursor();
         mWasOverHyperlink = false;

         emit OnHyperlinkClicked(hyperlink);
      }
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
      documentSplit->setStretchFactor(0, 1);
      documentSplit->setStretchFactor(1, 2);

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
      OpenPage("", "", true);
      OnHyperlinkClicked(mDocument->GetHome().c_str());
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
      OpenPage("", "", newPage);

      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         doc->SetPage(USING_HELP_TAG, "");
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
   void DocBrowser::OpenPage(const QString& page, const QString& anchor, bool newTab, bool SetPage)
   {
      // Create a new page if we are forcing a new page or
      // if we don't have any pages yet.
      if (mDocumentTabs->count() < 1 || newTab)
      {
         DocBrowserText* doc = new DocBrowserText(this);

         doc->setTextInteractionFlags(Qt::TextBrowserInteraction);

         connect(doc, SIGNAL(OnHyperlinkClicked(const QString&)),
            this, SLOT(OnHyperlinkClicked(const QString&)));

         //QScrollBar* scrollBar = doc->verticalScrollBar();
         //if (scrollBar)
         //{
         //   connect(scrollBar, SIGNAL(valueChanged(int)),
         //      this, SLOT(OnDocumentScrolled(int)));
         //}

         int index = mDocumentTabs->addTab(doc, "");
         mDocumentTabs->setCurrentIndex(index);
      }

      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc && !page.isEmpty())
      {
         mDocumentTabs->setTabText(mDocumentTabs->currentIndex(), page);

         if (SetPage)
         {
            doc->SetPage(page, anchor);
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
   void DocBrowser::OnPrevButton()
   {
      DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
      if (doc)
      {
         if (doc->IsPrevPageValid())
         {
            QString page, anchor;
            doc->PrevPage(page, anchor);
            OpenPage(page, anchor, false, false);

            // Search the table of contents for a page that references the same link location.
            QTreeWidgetItem* item = FindTOC(page, anchor);
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
            QString page, anchor;
            doc->NextPage(page, anchor);
            OpenPage(page, anchor, false, false);

            // Search the table of contents for a page that references the same link location.
            QTreeWidgetItem* item = FindTOC(page, anchor);
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
      OnHyperlinkClicked(mDocument->GetHome().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnHyperlinkClicked(const QString& link)
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
         QTreeWidgetItem* item = FindTOC(page, anchor);
         if (item)
         {
            // If the page is listed in the table of contents, select that item.
            mContentList->blockSignals(true);
            mContentList->setCurrentItem(item);
            mContentList->blockSignals(false);
         }

         // If the contents doesn't list this page, then just open it.
         OpenPage(page, anchor);
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
         //OpenPage(current->statusTip(0));
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
            QString anchor;
            if (doc->GetPage(page, anchor) && !page.isEmpty())
            {
               // Search the table of contents for a page that references the same link location.
               QTreeWidgetItem* item = FindTOC(page, anchor);
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
            disconnect(doc, SIGNAL(OnHyperlinkClicked(const QString&)),
               this, SLOT(OnHyperlinkClicked(const QString&)));

            //QScrollBar* scrollBar = doc->verticalScrollBar();
            //if (scrollBar)
            //{
            //   disconnect(scrollBar, SIGNAL(valueChanged(int)),
            //      this, SLOT(OnDocumentScrolled(int)));
            //}

            mDocumentTabs->removeTab(index);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DocBrowser::OnDocumentScrolled(int value)
   {
      //// Find all anchors in the document up to our current position.
      //DocBrowserText* doc = dynamic_cast<DocBrowserText*>(mDocumentTabs->currentWidget());
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
      //            QTreeWidgetItem* item = FindTOC(mDocumentTabs->tabText(mDocumentTabs->currentIndex()), mAnchor, true);
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
   QTreeWidgetItem* DocBrowser::FindTOC(const QString& page, const QString& anchor, bool anchorMustMatch)
   {
      for (int index = 0; index < mContentList->topLevelItemCount(); index++)
      {
         QTreeWidgetItem* item = FindTOC(page, anchor, mContentList->topLevelItem(index), anchorMustMatch);
         if (item) return item;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QTreeWidgetItem* DocBrowser::FindTOC(const QString& page, const QString& anchor, QTreeWidgetItem* parent, bool anchorMustMatch)
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
               QTreeWidgetItem* found = FindTOC(page, anchor, child, true);
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
            QTreeWidgetItem* found = FindTOC(page, anchor, child, anchorMustMatch);

            if (found) return found;
         }
      }

      return NULL;
   }

} // namespace dtQt
