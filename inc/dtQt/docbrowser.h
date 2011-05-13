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

#ifndef DELTA_DOC_BROWSER
#define DELTA_DOC_BROWSER

#include <vector>

#include <dtQt/export.h>
#include <dtQt/docbrowserxmlreader.h>

#include <dtCore/refptr.h>

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QTabWidget>

class QAction;

namespace dtQt
{
   class DocBrowser;

   /**
   * @class DocBrowserTabs
   * @brief This is the help document tab display window,
   *        overloaded to provide functionality for adding and removing tabs.
   */
   class DocBrowserTabs : public QTabWidget
   {
      Q_OBJECT

   public:

      /**
      * Constructs a document tabs viewer.
      *
      * @param[in]  parent      The parent widget.
      * @param[in]  docBrowser  The document browser that owns this widget.
      */
      DocBrowserTabs(QWidget* parent, DocBrowser* docBrowser);

   protected:

      /**
      * Event handler when the mouse button is clicked.
      *
      * @param[in]  e  The mouse event.
      */
      void mousePressEvent(QMouseEvent* e);

      /**
      * Event handler when the mouse button is released.
      *
      * @param[in]  e  The mouse event.
      */
      void mouseReleaseEvent(QMouseEvent *e);

      /**
      * Event handler when the mouse button is double clicked.
      *
      * @param[in]  e  The mouse event.
      */
      void mouseDoubleClickEvent(QMouseEvent *e);


      DocBrowser* mDocBrowser;
   };

   /**
   * @class DocBrowserText
   * @brief This is the help document text window,
   *        overloaded to provide functionality for hyperlinks.
   */
   class DocBrowserText : public QTextEdit
   {
      Q_OBJECT

      struct DocHistory
      {
         QString page;
         QString anchor;
      };

   public:

      /**
      * Constructs a document text viewer.
      *
      * @param[in]  parent  The parent widget.
      */
      DocBrowserText(QWidget* parent);

      /**
      * Adds a new page to the page history and sets it as the
      * current.
      *
      * @param[in]  page    The page.
      * @param[in]  anchor  The anchor.
      */
      void SetPage(const QString& page, const QString& anchor);

      /**
      * Retrieves the data of the current page.
      *
      * @param[out]  page    The current page.
      * @param[out]  anchor  The current anchor.
      */
      bool GetPage(QString& page, QString& anchor);

      /**
      * Retrieves the previous page data from the page history and
      * makes it the current.
      *
      * @param[out]  page    The previous page.
      * @param[out]  anchor  The previous anchor.
      *
      * @return      Returns false if there is no previous page.
      */
      bool PrevPage(QString& page, QString& anchor);

      /**
      * Retrieves the next page data from the page history
      * and makes it the current.
      *
      * @param[out]  page    The next page.
      * @param[out]  anchor  The next anchor.
      *
      * @return      Returns false if there is no next page.
      */
      bool NextPage(QString& page, QString& anchor);

      /**
      * Retrieves whether there is a previous or next page.
      */
      bool IsPrevPageValid();
      bool IsNextPageValid();

   signals:

      /**
      * Signal when a hyper link has been clicked on.
      *
      * @param[in]  link  The location of the link.
      */
      void OnHyperlinkClicked(const QString& link);

   protected:
      
      /**
      * Event handler when the mouse is moved.
      */
      virtual void mouseMoveEvent(QMouseEvent *e);

      /**
      * Event handler when the mouse is pressed.
      */
      virtual void mousePressEvent(QMouseEvent *e);

   private:

      bool mWasOverHyperlink;

      std::vector<DocHistory> mHistory;
      int                     mCurrentPage;
   };

   /**
    * @class DocBrowser
    * @brief This class generates the Editor's about box.
    */
   class DT_QT_EXPORT DocBrowser : public QMainWindow
   {
      Q_OBJECT

   public:
      /**
       * Constructs a new help box.
       *
       * @param[in]  filename  The name of the help file to load.
       * @param[in]  parent    The parent widget.
       */
      DocBrowser(const QString& filename, QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~DocBrowser();

      /**
      * Retrieves the home page.
      */
      std::string GetHome() {return mDocument->GetHome();}

      /**
      * Loads the Using Help page.
      *
      * @param[in]  newPage  True to create a new tab page.
      */
      void OpenUsingHelpPage(bool newPage = false);
      
      /**
      * Loads a specified page.
      *
      * @param[in]  page     The name of the page to open.
      * @param[in]  anchor   The anchor to scroll to.
      * @param[in]  newTab   True to create a new tab for this page.
      * @param[in]  SetPage  True if we want to set this page in the browser history.
      */
      void OpenPage(const QString& page, const QString& anchor, bool newTab = false, bool SetPage = true);

   public slots:

      /**
      * Event handler when the back button is pressed.
      */
      void OnPrevButton();

      /**
      * Event handler when the next button is pressed.
      */
      void OnNextButton();

      /**
      * Event handler when the home button is pressed.
      */
      void OnHomeButton();

      /**
      * Event handler when a hyper link is clicked on.
      *
      * @param[in]  link  The location of the link.
      */
      void OnHyperlinkClicked(const QString& link);

      /**
      * Event handler when the current item in the contents list is changed.
      *
      * @param[in]  current   The new current item.
      * @param[in]  previous  The previous item.
      */
      void OnContentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

      /**
      * Event handler when the current document tab has changed.
      *
      * @param[in]  index  The index of the new tab.
      */
      void OnDocumentTabChanged(int index);

      /**
      * Event handler when the current document tab has closed.
      *
      * @param[in]  index  The index of the closed tab.
      */
      void OnDocumentTabClosed(int index);

      /**
      * Event handler when the document window scroll position has changed.
      *
      * @param[in]  value  The new scroll position.
      */
      void OnDocumentScrolled(int value);

   private:

      /**
      * Sets up the table of contents.
      */
      void SetupContents();
      void SetupContents(const std::vector<DocBrowserXMLReader::SectionInfo*>& sections, QTreeWidgetItem* parent);

      /**
      * Finds a table of contents entry that matches the given page.
      *
      * @param[in]  page    The page to find.
      * @param[in]  anchor  The anchor of the page.
      * @param[in]  parent  The parent widget.
      *
      * @return     The tree widget item or NULL if not found.
      */
      QTreeWidgetItem* FindTOC(const QString& page, const QString& anchor = "", bool anchorMustMatch = false);
      QTreeWidgetItem* FindTOC(const QString& page, const QString& anchor, QTreeWidgetItem* parent, bool anchorMustMatch);

      QString              mResourcePrefix;
      QTreeWidget*         mContentList;
      DocBrowserTabs*      mDocumentTabs;
      DocBrowserXMLReader* mDocument;

      QAction*             mPrevPageAction;
      QAction*             mNextPageAction;
      QAction*             mHomeAction;
   };

} // namespace dtQt

#endif // DELTA_DOC_BROWSER
