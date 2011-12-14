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
   class Node;
   struct ID;

   /**
   * @class EditorScene
   */
   class DT_DIRECTOR_QT_EXPORT EditorScene : public QGraphicsScene
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
      EditorView* GetView() const;

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
      void SetGraph(DirectorGraph* graph, bool rememberHistory = true);

      /**
       *	Shows the nodes within a given graph.
       *
       * @param[in]  graph     The graph to show.
       * @param[in]  imported  Whether this graph is imported.
       */
      void ShowNodes(DirectorGraph* graph, bool imported);

      /**
       * Retrieves the current graph.
       *
       * @return  The current graph.
       */
      DirectorGraph* GetGraph() {return mGraph.get();}

      /**
       * Retrieves the background item.
       */
      QGraphicsRectItem* GetTranslationItem() {return mTranslationItem;};

      /**
       * Calculates a snapped position.
       *
       * @param[in]  item  The item.
       *
       * Return      A smart position.
       */
      osg::Vec2 GetSmartSnapPosition(NodeItem* item);

      /**
       * Calculates the center of a list of node items.
       *
       * @param[in]  nodes  List of node items.
       *
       * @return     The center of all nodes.
       */
      QPointF GetCenter(const QList<QGraphicsItem*>& nodes);

      /**
       * Retrieves whether the control or shift modifiers are being held.
       */
      bool IsHoldingAlt() const { return mHoldingAlt; }
      bool IsHoldingControl() const { return mHoldingControl; }
      bool IsHoldingShift() const { return mHoldingShift; }

      /**
       * Focuses the view on the center of all node items.
       */
      void CenterAll();

      /**
       * Focuses the view on the current selection.
       */
      void CenterSelection();

      /**
       * Centers the view on a given position.
       */
      void CenterOn(const QPointF& pos);

      /**
       * Refreshes the graph.
       */
      void Refresh();

      /**
       * Retrieves the Create Macro with Selection Action.
       */
      QAction* GetMacroSelectionAction() {return mMacroSelectionAction;}

      /**
       * Retrieves the Create Group Around Selection Action.
       */
      QAction* GetGroupSelectionAction() {return mGroupSelectionAction;}

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
      NodeItem* GetNodeItem(const dtDirector::ID& id, bool exactMatch = false);
      NodeItem* GetNodeItem(Node* node, bool exactMatch = false);

      /**
       * Retrieves a graph item for a graph.
       *
       * @param[in]  id  The ID of the graph.
       *
       * @return     The graph item that contains this graph.
       */
      MacroItem* GetGraphItem(const dtDirector::ID& id);

      /**
       * Batch selection.  To be used when we are purposely selecting multiple
       * nodes in a single operation.
       */
      void BeginBatchSelection();
      void EndBatchSelection();

      /**
       * Adds an item to the selected list.
       *
       * @param[in]  container  The item to add.
       */
      void AddSelected(dtCore::PropertyContainer* container);

      /**
       * Removes an item from the selected list.
       *
       * @param[in]  container  The item to remove.
       */
      void RemoveSelected(dtCore::PropertyContainer* container);

      /**
       * Creates a new node.
       *
       * @param[in]  name      The name of the node.
       * @param[in]  category  The category of the node.
       * @param[in[  x, y      Starting UI coordinates to spawn the node.
       *
       * @return     A pointer to the new node.  NULL if node could
       *             not be created.
       */
      Node* CreateNode(const std::string& name, const std::string& category, float x, float y);

      /**
       * Creates a new node item.  Providing a blank name will cause a
       * macro node to be created instead using the category as the custom
       * editor name.
       *
       * @param[in]  name      The name of the node or blank to create macro.
       * @param[in]  category  The category of the node or custom editor for a macro.
       * @param[in[  x, y      Starting UI coordinates to spawn the node.
       */
      Node* CreateNodeItem(const std::string& name, const std::string& category, float x, float y);

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
       * Retrieves the current selection.
       */
      std::vector<dtCore::RefPtr<dtCore::PropertyContainer> >& GetSelection() {return mSelected;}

      /**
       * Refreshes the property editor.
       */
      void RefreshProperties();

   signals:

   public slots:

      /**
       * Event handler when a node creation menu item is triggered.
       */
      void OnCreateNodeEvent(QAction* action);

      /**
       * Event handler to create a macro.
       */
      void OnCreateMacro();

      /**
       * Event handler when a custom edited macro node item is triggered.
       */
      void OnCreateCustomEditedMacro(QAction* action);

      /**
       * Event handler to create actor nodes for the current selected actors in STAGE.
       */
      void OnCreateActorsFromSelection();

      /**
       * Event handler to create a group frame around the current selection.
       */
      void OnCreateGroupForSelection();

   protected:

      /**
       *	Key press events.
       *
       * @param[in]  event  The keyboard event.
       */
      void keyPressEvent(QKeyEvent* event);

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

      /**
       * Drag event moved on this scene.
       *
       * @param[in]  event  The drag event.
       */
      virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event);

      /**
       * Drag event dropped on this scene.
       *
       * @param[in]  event  The drag event.
       */
      virtual void dropEvent(QGraphicsSceneDragDropEvent* event);

      /**
       * Event handler when the context menu event has triggered.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      /**
       * Creates a new macro node.
       */
      DirectorGraph* CreateMacro(const std::string& editorName);

      /**
       *	Begins dragging of copied nodes.
       */
      void CopiedNodeBeginDrag(QGraphicsSceneMouseEvent* event);

      void PaintItemChildren(QPainter* painter, QGraphicsItem* item, QStyleOptionGraphicsItem* options);

      /**
       *	Adds a graph to the previous history data.
       */
      void AddGraphHistory(dtDirector::DirectorGraph* graph);

      /**
       *	Attempts to go back in graph history.
       */
      void GraphHistoryBack();

      /**
       *	Attempts to go forward in graph history.
       */
      void GraphHistoryForward();

   private:
      DirectorEditor*            mEditor;
      EditorView*                mView;

      PropertyEditor*            mPropertyEditor;
      GraphTabs*                 mGraphTabs;

      dtCore::ObserverPtr<dtDirector::DirectorGraph> mGraph;

      struct GraphHistoryData
      {
         dtCore::ObserverPtr<dtDirector::DirectorGraph> graph;
         QPointF pos;
         float zoom;
      };

      std::vector<GraphHistoryData> mPrevHistory;
      std::vector<GraphHistoryData> mNextHistory;

      std::vector<NodeItem*>     mNodes;

      bool     mDragging;
      QPointF  mDragOrigin;
      bool     mHasDragged;
      bool     mBandSelecting;
      bool     mBatchSelecting;
      bool     mHoldingAlt;
      bool     mHoldingControl;
      bool     mHoldingShift;

      QAction* mMacroSelectionAction;
      QAction* mGroupSelectionAction;

      QPointF            mMenuPos;
      QGraphicsRectItem* mTranslationItem;

      PropertyEditor::PropertyContainerRefPtrVector mSelected;
   };
} // namespace dtDirector

#endif // DIRECTORQT_EDITOR_SCENE
