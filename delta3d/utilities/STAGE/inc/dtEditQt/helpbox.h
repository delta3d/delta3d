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

#include <dtEditQt/helpxmlreader.h>

#include <dtCore/refptr.h>

#include <QtGui/QDialog>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QTabWidget>

namespace dtEditQt
{
   /**
   * @class DocumentText
   * @brief This is the help document text window,
   *        overloaded to provide functionality for hyperlinks.
   */
   class DocumentText : public QTextEdit
   {
      Q_OBJECT

   public:

      /**
      * Constructs a document text viewer.
      *
      * @param[in]  parent  The parent widget.
      */
      DocumentText(QWidget* parent);

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
   };

   /**
    * @class HelpBox
    * @brief This class generates the Editor's about box.
    */
   class HelpBox : public QDialog
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

   public slots:

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

   private:

      /**
      * Loads a specified page.
      *
      * @param[in]  page  The name of the page to open.
      */
      void openPage(const QString& page);

      /**
      * Creates a document text edit.
      *
      * @param[in]  filename  The name of the document file to load.
      */
      QTextEdit* createDocument(const QString& filename);

      /**
      * Sets up the table of contents.
      */
      void setupContents();
      void setupContents(const std::vector<HelpXMLReader::SectionInfo*>& sections, QTreeWidgetItem* parent);

      /**
      * Finds a table of contents entry that matches the given page.
      *
      * @param[in]  page    The page to find.
      * @param[in]  parent  The parent widget.
      *
      * @return     The tree widget item or NULL if not found.
      */
      QTreeWidgetItem* findContents(const QString& page);
      QTreeWidgetItem* findContents(const QString& page, QTreeWidgetItem* parent);

      QString      mResourcePrefix;
      QTreeWidget* mContentList;
      QTabWidget*  mDocumentTabs;
      HelpXMLReader* mDocument;
   };

} // namespace dtEditQt

#endif // DELTA_HELP_BOX
