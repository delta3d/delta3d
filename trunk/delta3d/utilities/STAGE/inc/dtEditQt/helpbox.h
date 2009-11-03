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

#ifndef DELTA_HELP_BOX
#define DELTA_HELP_BOX

#include <vector>

#include <dtEditQt/helpxmlreader.h>

#include <dtCore/refptr.h>

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QTabWidget>

class QAction;

namespace dtEditQt
{
   class HelpBox;

   /**
   * @class DocumentTabs
   * @brief This is the help document tab display window,
   *        overloaded to provide functionality for adding and removing tabs.
   */
   class DocumentTabs : public QTabWidget
   {
      Q_OBJECT

   public:

      /**
      * Constructs a document tabs viewer.
      *
      * @param[in]  parent   The parent widget.
      * @param[in]  helpbox  The help box that owns this widget.
      */
      DocumentTabs(QWidget* parent, HelpBox* helpBox);

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


      HelpBox* mHelpBox;
   };

   /**
   * @class DocumentText
   * @brief This is the help document text window,
   *        overloaded to provide functionality for hyperlinks.
   */
   class DocumentText : public QTextEdit
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
      DocumentText(QWidget* parent);

      /**
      * Adds a new page to the page history and sets it as the
      * current.
      *
      * @param[in]  page    The page.
      * @param[in]  anchor  The anchor.
      */
      void setPage(const QString& page, const QString& anchor);

      /**
      * Retrieves the data of the current page.
      *
      * @param[out]  page    The current page.
      * @param[out]  anchor  The current anchor.
      */
      bool getPage(QString& page, QString& anchor);

      /**
      * Retrieves the previous page data from the page history and
      * makes it the current.
      *
      * @param[out]  page    The previous page.
      * @param[out]  anchor  The previous anchor.
      *
      * @return      Returns false if there is no previous page.
      */
      bool prevPage(QString& page, QString& anchor);

      /**
      * Retrieves the next page data from the page history
      * and makes it the current.
      *
      * @param[out]  page    The next page.
      * @param[out]  anchor  The next anchor.
      *
      * @return      Returns false if there is no next page.
      */
      bool nextPage(QString& page, QString& anchor);

      /**
      * Retrieves whether there is a previous or next page.
      */
      bool isPrevPage();
      bool isNextPage();

   signals:

      /**
      * Signal when a hyper link has been clicked on.
      *
      * @param[in]  link  The location of the link.
      */
      void hyperlinkClicked(const QString& link);

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
    * @class HelpBox
    * @brief This class generates the Editor's about box.
    */
   class HelpBox : public QMainWindow
   {
      Q_OBJECT

   public:
      /**
       * Constructs a new help box.
       *
       * @param[in]  filename  The name of the help file to load.
       * @param[in]  parent    The parent widget.
       */
      HelpBox(const QString& filename, QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~HelpBox();

      /**
      * Retrieves the home page.
      */
      std::string getHome() {return mDocument->getHome();}

      /**
      * Loads the Using Help page.
      *
      * @param[in]  newPage  True to create a new tab page.
      */
      void openUsingHelpPage(bool newPage = false);
      
      /**
      * Loads a specified page.
      *
      * @param[in]  page     The name of the page to open.
      * @param[in]  anchor   The anchor to scroll to.
      * @param[in]  newTab   True to create a new tab for this page.
      * @param[in]  setPage  True if we want to set this page in the browser history.
      */
      void openPage(const QString& page, const QString& anchor, bool newTab = false, bool setPage = true);

   public slots:

      /**
      * Event handler when the back button is pressed.
      */
      void onPrevButton();

      /**
      * Event handler when the next button is pressed.
      */
      void onNextButton();

      /**
      * Event handler when the home button is pressed.
      */
      void onHomeButton();

      /**
      * Event handler when a hyper link is clicked on.
      *
      * @param[in]  link  The location of the link.
      */
      void onHyperlinkClicked(const QString& link);

      /**
      * Event handler when the current item in the contents list is changed.
      *
      * @param[in]  current   The new current item.
      * @param[in]  previous  The previous item.
      */
      void onContentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

      /**
      * Event handler when the current document tab has changed.
      *
      * @param[in]  index  The index of the new tab.
      */
      void onDocumentTabChanged(int index);

      /**
      * Event handler when the current document tab has closed.
      *
      * @param[in]  index  The index of the closed tab.
      */
      void onDocumentTabClosed(int index);

      /**
      * Event handler when the document window scroll position has changed.
      *
      * @param[in]  value  The new scroll position.
      */
      void onDocumentScrolled(int value);

   private:

      /**
      * Sets up the table of contents.
      */
      void setupContents();
      void setupContents(const std::vector<HelpXMLReader::SectionInfo*>& sections, QTreeWidgetItem* parent);

      /**
      * Finds a table of contents entry that matches the given page.
      *
      * @param[in]  page    The page to find.
      * @param[in]  anchor  The anchor of the page.
      * @param[in]  parent  The parent widget.
      *
      * @return     The tree widget item or NULL if not found.
      */
      QTreeWidgetItem* findTOC(const QString& page, const QString& anchor = "", bool anchorMustMatch = false);
      QTreeWidgetItem* findTOC(const QString& page, const QString& anchor, QTreeWidgetItem* parent, bool anchorMustMatch);

      QString        mResourcePrefix;
      QTreeWidget*   mContentList;
      DocumentTabs*  mDocumentTabs;
      HelpXMLReader* mDocument;

      QAction*    mPrevPageAction;
      QAction*    mNextPageAction;
      QAction*    mHomeAction;
   };

} // namespace dtEditQt

#endif // DELTA_HELP_BOX
