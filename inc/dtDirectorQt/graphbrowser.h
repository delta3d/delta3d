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

#ifndef DIRECTORQT_GRAPH_BROWSER
#define DIRECTORQT_GRAPH_BROWSER

#include <dtDirectorQt/export.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidgetItem>

class QGroupBox;
class QTreeWidget;

namespace dtDirector
{
   class DirectorEditor;

   class DT_DIRECTOR_QT_EXPORT GraphItem : public QTreeWidgetItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  graph   The graph.
       * @param[in]  parent  The parent.
       */
      GraphItem(DirectorGraph* graph, QTreeWidgetItem* parent);

      /**
       * Retrieves the graph.
       */
      DirectorGraph* GetGraph() {return mGraph;}

   private:

      DirectorGraph* mGraph;
   };

   /**
    * @class GraphBrowser
    *
    * @brief Browser tree for viewing Director Graphs.
    */
   class DT_DIRECTOR_QT_EXPORT GraphBrowser : public QDockWidget
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  parent    This editor parent.
       */
      GraphBrowser(QMainWindow* parent);

      /**
      * Sets the Graph Browser's DirectorEditor
      *
      * @param[in]  editor  The director editor that owns this widget.
      */
      void SetDirectorEditor(DirectorEditor* editor);

      /**
       * Builds the Graph list.
       *
       * @param[in]  selected  The currently selected graph.
       */
      void BuildGraphList(DirectorGraph* selected = NULL);

      /**
       * Selects a Graph item.
       *
       * @param[in]  selected  The currently selected graph.
       */
      void SelectGraph(DirectorGraph* selected);

   public slots:
      
      /**
       * Event handler when the current item has changed.
       *
       * @param[in]  current   The new current item.
       * @param[in]  previous  The previous item.
       */
      void OnItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

   private:

      /**
       * Recursively adds a graph item and all its children to the tree.
       *
       * @param[in]  graph     The graph to add.
       * @param[in]  parent    The parent item.
       * @param[in]  selected  The currently selected graph item.
       *
       * @return     Returns true if the selected item was found.
       */
      bool AddGraphItem(DirectorGraph* graph, GraphItem* parent, DirectorGraph* selected);

      /**
       * Recursively selects a graph item.
       *
       * @param[in]  parent    The parent.
       * @param[in]  selected  The selected item to find.
       */
      bool SelectGraph(QTreeWidgetItem* parent, DirectorGraph* selected);

      DirectorEditor* mEditor;

      QGroupBox*     mGroupBox;
      QTreeWidget*   mGraphTree;
   };
}

#endif // DIRECTORQT_GRAPH_BROWSER
