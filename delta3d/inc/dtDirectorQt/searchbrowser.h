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

#ifndef DIRECTORQT_SEARCH_BROWSER
#define DIRECTORQT_SEARCH_BROWSER

#include <dtDirectorQt/export.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidgetItem>

#include <stack>

class QGroupBox;
class QTreeWidget;
class QLineEdit;
class QToolButton;

namespace dtDirector
{
   class DirectorEditor;
   class Node;

   class DT_DIRECTOR_QT_EXPORT SearchMacroItem : public QTreeWidgetItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  graph     The graph.
       * @param[in]  descText  The description text to display.
       */
      SearchMacroItem(DirectorGraph* graph, const QString& descText);

      /**
       * Retrieves the node data.
       */
      DirectorGraph* GetGraph() {return mGraph;}

   private:

      DirectorGraph*  mGraph;
   };

   class DT_DIRECTOR_QT_EXPORT SearchItem : public QTreeWidgetItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  node      The node.
       * @param[in]  descText  The description text to display.
       */
      SearchItem(Node* node, const QString& descText);

      /**
       * Retrieves the node data.
       */
      Node* GetNode() {return mNode;}

   private:

      Node*  mNode;
   };

   /**
    * @class SearchBrowser
    *
    * @brief Browser tree for searching.
    */
   class DT_DIRECTOR_QT_EXPORT SearchBrowser : public QDockWidget
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  parent    This editor parent.
       */
      SearchBrowser(QMainWindow* parent);

      /**
      * Sets the Search Browser's DirectorEditor
      *
      * @param[in]  editor  The director editor that owns this widget.
      */
      void SetDirectorEditor(DirectorEditor* editor);

      /**
       * Focuses the cursor on the search field.
       */
      void FocusSearch();

      /**
       * Retrieves whether the search field has focus already.
       */
      bool HasSearchFocus() const;

   public slots:

      /**
       * Builds the Graph list with all nodes found.
       */
      void OnSearch();

      /**
       * Event handler when the current item is double clicked.
       *
       * @param[in]  item    The item.
       * @param[in]  column  The column.
       */
      void OnFocusItem(QTreeWidgetItem* item, int column);

   private:

      /**
       * Performs a recursive search within the given graph.
       *
       * @param[in]  graph                  The graph to search.
       * @param[in]  searchText             The search text.
       * @param[in]  searchImportedScripts  True to search nodes in imported scripts as well.
       * @param[in]  parent                 The parent.
       *
       * @return     True if nodes were found.
       */
      bool SearchGraph(DirectorGraph* graph, const QString& searchText, bool searchImportedScripts = false, QTreeWidgetItem* parent = NULL);

      /**
       * Tests whether the given test matches with a property of the given node.
       *
       * @param[in]   node        The node to test.
       * @param[in]   searchText  The text to search for.
       * @param[out]  outDesc     The found property data.
       *
       * @return     True if the node is valid for the given search.
       */
      bool TestItem(dtCore::PropertyContainer* container, const QString& searchText, QString& outDesc);

      DirectorEditor* mEditor;

      QGroupBox*     mGroupBox;
      QLineEdit*     mSearchEdit;
      QToolButton*   mSearchButton;
      QTreeWidget*   mNodeTree;
   };
}

#endif // DIRECTORQT_SEARCH_BROWSER
