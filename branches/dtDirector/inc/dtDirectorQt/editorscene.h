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

#ifndef DIRECTORQT_EDITOR_SCENE
#define DIRECTORQT_EDITOR_SCENE

#include <dtDirectorQt/export.h>
#include <dtDirectorQt/propertyeditor.h>

#include <dtDirector/director.h>

#include <dtCore/refptr.h>

#include <QtGui/QGraphicsScene>

#include <vector>


class QGraphicsSceneMouseEvent;

namespace dtDirector
{
   class DirectorEditor;
   class EditorView;
   class GraphTabs;
   class DirectorGraph;
   class NodeItem;
   class MacroItem;

   /**
   * @class EditorScene
   */
   class EditorScene : public QGraphicsScene
   {
      Q_OBJECT

   public:

      /**
      * Constructor.
      *
      * @param[in]  propEditor  The Property Editor.
      * @param[in]  graphTabs   The Graph Tabs widget.
      * @param[in]  parent      The parent widget.
      */
      EditorScene(PropertyEditor* propEditor, GraphTabs* graphTabs, QWidget* parent = 0);

      /**
       * Sets the current view.
       */
      void SetView(EditorView* view);

      /**
       * Accessors to the editor.
       */
      void SetEditor(DirectorEditor* editor);
      DirectorEditor* GetEditor();

      /**
       * Sets the currently viewed director graph.
       *
       * @param[in]  graph  The Graph to view.
       */
      void SetGraph(DirectorGraph* graph);

      /**
       * Retrieves the current graph.
       *
       * @return  The current graph.
       */
      DirectorGraph* GetGraph() {return mGraph;}

      /**
       * Retrieves the background item.
       */
      QGraphicsRectItem* GetTranslationItem() {return mTranslationItem;};

      /**
       * Refreshes the graph.
       */
      void Refresh();

      /**
       * Retrieves a node item given the nodes ID.
       *
       * @param[in]  id          The ID of the node.
       * @param[in]  exactMatch  True to only search for exact nodes with that ID.
       *                         False to find any graph nodes that have a node with that ID.
       *
       * @return     A pointer to the NodeItem with the matching ID,
       *             or NULL if not found.
       */
      NodeItem* GetNodeItem(const dtCore::UniqueId& id, bool exactMatch = false);

      /**
       * Retrieves a graph item for a graph.
       *
       * @param[in]  id  The ID of the graph.
       *
       * @return     The graph item that contains this graph.
       */
      MacroItem* GetGraphItem(const dtCore::UniqueId& id);

      /**
       * Adds an item to the selected list.
       *
       * @param[in]  container  The item to add.
       */
      void AddSelected(dtDAL::PropertyContainer* container);

      /**
       * Removes an item from the selected list.
       *
       * @param[in]  container  The item to remove.
       */
      void RemoveSelected(dtDAL::PropertyContainer* container);

      /**
       * Deletes a node from the list.
       *
       * @param[in]  node  The node to delete.
       */
      void DeleteNode(NodeItem* node);

      /**
       * Retrieves whether there are nodes selected.
       *
       * @return   True if there is at least one node selected.
       */
      bool HasSelection();

      /**
       * Refreshes the property editor.
       */
      void RefreshProperties();

   signals:

   public slots:

   protected:
      
      /**
       * Mouse button events.
       *
       * @param[in]  event  The mouse event.
       */
      void mousePressEvent(QGraphicsSceneMouseEvent* event);
      void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

      /**
       * Mouse movement event.
       *
       * @param[in]  event  The mouse event.
       */
      void mouseMoveEvent(QGraphicsSceneMouseEvent* event);  

   private:

      DirectorEditor*            mEditor;
      EditorView*                mView;

      PropertyEditor*            mPropertyEditor;
      GraphTabs*                 mGraphTabs;

      dtDirector::DirectorGraph* mGraph;

      std::vector<NodeItem*>     mNodes;

      bool     mDragging;
      QPointF  mDragOrigin;
      bool     mHasDragged;

      QGraphicsRectItem* mTranslationItem;
      
      PropertyEditor::PropertyContainerRefPtrVector mSelected;
   };
} // namespace dtDirector

#endif // DIRECTORQT_EDITOR_SCENE
