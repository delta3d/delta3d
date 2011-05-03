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

#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undocreateevent.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/scriptitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/macroitem.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/groupitem.h>

#include <dtDirector/nodemanager.h>
#include <dtDirector/nodetype.h>
#include <dtDirector/groupnode.h>
#include <dtDirectorNodes/referencescriptaction.h>

#include <QtCore/QMimeData>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>

#include <dtDAL/actoridactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   EditorScene::EditorScene(PropertyEditor* propEditor, GraphTabs* graphTabs, QWidget* parent)
      : QGraphicsScene(parent)
      , mEditor(NULL)
      , mView(NULL)
      , mPropertyEditor(propEditor)
      , mGraphTabs(graphTabs)
      , mGraph(NULL)
      , mDragging(false)
      , mHasDragged(false)
      , mBandSelecting(false)
      , mBatchSelecting(false)
      , mHoldingAlt(false)
      , mHoldingControl(false)
      , mHoldingShift(false)
      , mMacroSelectionAction(NULL)
      , mTranslationItem(NULL)
   {
      mPropertyEditor->SetScene(this);

      setBackgroundBrush(Qt::lightGray);

      setSceneRect(0, 0, 10000, 10000);

      mMacroSelectionAction = new QAction("Create Macro with Selection", this);

      connect(mMacroSelectionAction, SIGNAL(triggered()),
         this, SLOT(OnCreateMacro()));
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::SetView(EditorView* view)
   {
      mView = view;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::SetEditor(DirectorEditor* editor)
   {
      mEditor = editor;

      mSelected.clear();
      mSelected.push_back(mEditor->GetDirector());
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorEditor* EditorScene::GetEditor()
   {
      return mEditor;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::SetGraph(dtDirector::DirectorGraph* graph)
   {
      // If we are selecting the same graph, ignore.
      if (graph == mGraph)
      {
         return;
      }

      // First clear the current items.
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* node = mNodes[index];
         if (node) delete node;
      }
      mNodes.clear();

      clear();

      // Also clear the current selection.
      mSelected.clear();
      mSelected.push_back(mEditor->GetDirector());

      // The translation item is the parent class for all other items.
      // This simulates the translation of the view by moving all children
      // nodes with it.  When the user translates the view, this item is
      // actually being translated instead.
      mTranslationItem = new QGraphicsRectItem(NULL, this);

      mGraph = graph;

      if (!mGraph) return;

      // Create all nodes in the graph.
      count = (int)mGraph->GetEventNodes().size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->GetEventNodes()[index].get();
         ActionItem* item = new ActionItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(10.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->GetActionNodes().size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->GetActionNodes()[index].get();
         NodeItem* item = NULL;

         // Special case for reference script nodes.
         if (IS_A(node, ReferenceScriptAction*))
         {
            item = new ScriptItem(node, mTranslationItem, this);
            item->setZValue(10.0f);
         }
         // Special case for group frame nodes.
         else if (IS_A(node, GroupNode*))
         {
            item = new GroupItem(node, mTranslationItem, this);
            item->setZValue(0.0f);
         }
         else
         {
            item = new ActionItem(node, mTranslationItem, this);
            item->setZValue(10.0f);
         }

         if (item)
         {
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->GetValueNodes().size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->GetValueNodes()[index].get();
         ValueItem* item = new ValueItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(20.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->GetSubGraphs().size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* graph = mGraph->GetSubGraphs()[index].get();
         MacroItem* item = new MacroItem(graph, mTranslationItem, this);
         if (item)
         {
            item->setZValue(10.0f);
            mNodes.push_back(item);
         }
      }

      mEditor->Refresh();
      CenterAll();
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 EditorScene::GetSmartSnapPosition(NodeItem* item)
   {
      if (!item) return osg::Vec2();

      osg::Vec2 position = item->GetPosition();

      // If snap to grid is disabled, then don't snap.
      if (!mEditor->GetSnapGridAction()->isChecked())
      {
         return position;
      }

      bool isValue = false;
      if (dynamic_cast<ValueItem*>(item))
      {
         isValue = true;
      }

      // Find all of the smart snap positions first.
      std::vector<float> mSnapTargetsX;
      std::vector<float> mSnapTargetsY;

      float snapRange = 150.0f;

      QList<QGraphicsItem*> nodes = items();
      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         // Ignore nodes that are currently selected.
         if (nodes[index]->isSelected())
         {
            continue;
         }

         NodeItem* nodeItem = dynamic_cast<NodeItem*>(nodes[index]);

         if (nodeItem && nodeItem != item)
         {
            QRectF itemBounds = QRectF(item->GetPosition().x(), item->GetPosition().y(), item->GetNodeWidth(), item->GetNodeHeight());
            QRectF nodeBounds = QRectF(nodeItem->GetPosition().x(), nodeItem->GetPosition().y(), nodeItem->GetNodeWidth(), nodeItem->GetNodeHeight());

            // Only align with nodes that are within range of the current item.
            if (itemBounds.left() - snapRange <= nodeBounds.right() &&
               itemBounds.right() + snapRange >= nodeBounds.left() &&
               itemBounds.top() - snapRange <= nodeBounds.bottom() &&
               itemBounds.bottom() + snapRange >= nodeBounds.top())
            {
               // Snap align with the top of this node.
               mSnapTargetsY.push_back(nodeItem->GetPosition().y());

               // Snap align with the bottom of this node.
               mSnapTargetsY.push_back(nodeItem->GetPosition().y() + nodeItem->GetNodeHeight() - item->GetNodeHeight());

               // Snap align with the left side of this node.
               mSnapTargetsX.push_back(nodeItem->GetPosition().x());

               // Snap align with the right side of this node.
               mSnapTargetsX.push_back(nodeItem->GetPosition().x() + nodeItem->GetNodeWidth() - item->GetNodeWidth());

               // If the snapping node is a value, snap this node to all
               // value link positions as well.
               if (isValue)
               {
                  int valueCount = (int)nodeItem->GetValues().size();
                  for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
                  {
                     ValueData& data = nodeItem->GetValues()[valueIndex];

                     float x = data.linkGraphic->scenePos().x();
                     x -= mTranslationItem->pos().x();
                     x -= item->GetNodeWidth()/2;
                     mSnapTargetsX.push_back(x);
                  }
               }

               // If the test node is a value node, snap align the moving
               // node's value links with this value.
               if (dynamic_cast<ValueItem*>(nodeItem))
               {
                  int valueCount = (int)item->GetValues().size();
                  for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
                  {
                     ValueData& data = item->GetValues()[valueIndex];

                     float offset = data.linkGraphic->scenePos().x() - item->scenePos().x();

                     float x = nodeItem->GetNodeWidth()/2 + nodeItem->GetPosition().x();
                     x -= offset;
                     mSnapTargetsX.push_back(x);
                  }
               }
            }
         }
      }

      // Find the best snap position.
      float nearest = 5.0f;
      osg::Vec2 newPos = position;
      count = (int)mSnapTargetsY.size();
      for (int index = 0; index < count; index++)
      {
         float y = mSnapTargetsY[index];
         if (fabs(y - position.y()) < nearest)
         {
            nearest = fabs(y - position.y());
            newPos.y() = y;
         }
      }

      nearest = 5.0f;
      count = (int)mSnapTargetsX.size();
      for (int index = 0; index < count; index++)
      {
         float x = mSnapTargetsX[index];
         if (fabs(x - position.x()) < nearest)
         {
            nearest = fabs(x - position.x());
            newPos.x() = x;
         }
      }

      return newPos;
   }

   //////////////////////////////////////////////////////////////////////////
   QPointF EditorScene::GetCenter(const QList<QGraphicsItem*>& nodes)
   {
      QPointF topLeft;
      QPointF botRight;

      int count = (int)nodes.size();

      if (count) topLeft = botRight = nodes[0]->pos();
      for (int index = 0; index < count; index++)
      {
         QPointF pos = nodes[index]->pos();
         QRectF bounds = nodes[index]->boundingRect();

         if (topLeft.x() > pos.x()) topLeft.setX(pos.x());
         if (topLeft.y() > pos.y()) topLeft.setY(pos.y());
         if (botRight.x() < pos.x() + bounds.width()) botRight.setX(pos.x() + bounds.width());
         if (botRight.y() < pos.y() + bounds.height()) botRight.setY(pos.y() + bounds.height());
      }

      QPointF center = (topLeft + botRight) / 2;
      return center;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::CenterAll()
   {
      QPointF topLeft;
      QPointF botRight;

      CenterOn(GetCenter(items()));
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::CenterSelection()
   {
      QPointF topLeft;
      QPointF botRight;

      CenterOn(GetCenter(selectedItems()));
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::CenterOn(const QPointF& pos)
   {
      QPointF center = mView->mapToScene(mView->width()/2, mView->height()/2);

      QPointF trans = -pos + center;
      mTranslationItem->setPos(trans);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::Refresh()
   {
      // Re-draw all nodes.
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item)
         {
            item->Draw();
         }
      }

      // Re-connect all nodes.
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item)
         {
            item->ConnectLinks();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   NodeItem* EditorScene::GetNodeItem(const dtCore::UniqueId& id, bool exactMatch)
   {
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item)
         {
            if (!exactMatch)
            {
               if (item->HasID(id)) return item;
            }
            else
            {
               if (item->GetID() == id) return item;
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   MacroItem* EditorScene::GetGraphItem(const dtCore::UniqueId& id)
   {
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];

         MacroItem* macro = dynamic_cast<MacroItem*>(item);
         if (macro && macro->GetGraph() &&
            macro->GetGraph()->GetID() == id)
         {
            return macro;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::BeginBatchSelection()
   {
      mBatchSelecting = true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::EndBatchSelection()
   {
      if (mBatchSelecting)
      {
         mEditor->Refresh();
      }

      mBatchSelecting = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::AddSelected(dtDAL::PropertyContainer* container)
   {
      // If we have the director container in the list, remove it.
      if (mSelected.size() == 1 && mSelected[0].get() == mEditor->GetDirector())
      {
         mSelected.clear();
      }

      mSelected.push_back(container);

      // Update the property editor.
      mPropertyEditor->HandlePropertyContainersSelected(mSelected);

      if (!mBandSelecting && !mBatchSelecting) mEditor->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::RemoveSelected(dtDAL::PropertyContainer* container)
   {
      int count = (int)mSelected.size();
      for (int index = 0; index < count; index++)
      {
         if (mSelected[index].get() == container)
         {
            mSelected.erase(mSelected.begin() + index);
            break;
         }
      }

      // If we have removed our last selected item, add the director.
      if (mSelected.empty())
      {
         mSelected.push_back(mEditor->GetDirector());
      }

      // Update the property editor.
      mPropertyEditor->HandlePropertyContainersSelected(mSelected);

      if (!mBandSelecting && !mBatchSelecting) mEditor->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   Node* EditorScene::CreateNode(const std::string& name, const std::string& category, float x, float y)
   {
      dtCore::RefPtr<Node> node = NodeManager::GetInstance().CreateNode(name, category, mGraph);
      if (node.valid())
      {
         node->SetPosition(osg::Vec2(x, y));

         // Now refresh the all editors that view the same graph.
         int count = mEditor->GetGraphTabs()->count();
         for (int index = 0; index < count; index++)
         {
            EditorView* view = dynamic_cast<EditorView*>(mEditor->GetGraphTabs()->widget(index));
            if (view && view->GetScene())
            {
               if (view->GetScene()->GetGraph() == mGraph)
               {
                  // First remember the position of the translation node.
                  QPointF trans = view->GetScene()->GetTranslationItem()->pos();
                  view->GetScene()->SetGraph(mGraph);
                  view->GetScene()->GetTranslationItem()->setPos(trans);
               }
            }
         }
         mEditor->Refresh();

         // Auto select the new node.
         NodeItem* item = GetNodeItem(node->GetID());
         if (item) item->setSelected(true);
      }

      return node.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorScene::CreateNodeItem(const std::string& name, const std::string& category, float x, float y)
   {
      // If the node name is empty, it means we are creating a macro.
      if (name.empty())
      {
         mEditor->GetUndoManager()->BeginMultipleEvents();
         mMenuPos.setX(x);
         mMenuPos.setY(y);
         DirectorGraph* graph = CreateMacro();
         if (graph)
         {
            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mEditor, graph->GetID(), "Name", "Macro", graph->GetName());
            mEditor->GetUndoManager()->AddEvent(event.get());

            if (!category.empty())
            {
               graph->SetEditor(category);

               event = new UndoPropertyEvent(mEditor, graph->GetID(), "Custom Editor", "", category);
               mEditor->GetUndoManager()->AddEvent(event.get());
            }
         }
         mEditor->GetUndoManager()->EndMultipleEvents();

         mEditor->RefreshGraph(mGraph);
         mEditor->Refresh();
      }
      else
      {
         Node* node = CreateNode(name, category, x, y);
         if (node)
         {
            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, node->GetID(), mGraph->GetID());
            mEditor->GetUndoManager()->AddEvent(event);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::DeleteNode(NodeItem* node)
   {
      if (!node) return;

      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mNodes[index] == node)
         {
            node->setSelected(false);
            mNodes.erase(mNodes.begin() + index);
            delete node;
            break;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool EditorScene::HasSelection()
   {
      return !selectedItems().isEmpty();
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::RefreshProperties()
   {
      mPropertyEditor->HandlePropertyContainersSelected(mSelected);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateNodeEvent(QAction* action)
   {
      if (!action) return;

      std::string name = action->text().toStdString();
      std::string category = action->statusTip().toStdString();
      if (name.empty() || category.empty()) return;

      Node* node = CreateNode(name, category, mMenuPos.x(), mMenuPos.y());
      if (node)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, node->GetID(), mGraph->GetID());
         mEditor->GetUndoManager()->AddEvent(event);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateMacro()
   {
      // Modify the menu position to the top left item selection
      QList<QGraphicsItem*> selection = selectedItems();
      if (!selection.empty())
      {
         // Set the graph position to the top left most item.
         mMenuPos = selection[0]->pos();

         int count = (int)selection.size();
         for (int index = 0; index < count; index++)
         {
            if (selection[index]->pos().x() < mMenuPos.x())
            {
               mMenuPos.setX(selection[index]->pos().x());
            }
            if (selection[index]->pos().y() < mMenuPos.y())
            {
               mMenuPos.setY(selection[index]->pos().y());
            }
         }
      }

      DirectorGraph* parent = mGraph;

      mEditor->GetUndoManager()->BeginMultipleEvents();
      DirectorGraph* graph = CreateMacro();
      if (graph)
      {
         // If there are any nodes selected, cut them.
         mEditor->on_action_Cut_triggered();

         // Switch the current graph to the new macro.
         SetGraph(graph);

         // If there are nodes selected, move them into the macro.
         mEditor->PasteNodes(true);
      }
      mEditor->GetUndoManager()->EndMultipleEvents();

      mEditor->RefreshGraph(parent);
      mEditor->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateCustomEditedMacro(QAction* action)
   {
      mEditor->GetUndoManager()->BeginMultipleEvents();
      DirectorGraph* graph = CreateMacro();
      if (graph)
      {
         // Assign the editor to the graph.
         graph->SetEditor(action->statusTip().toStdString());

         dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mEditor, graph->GetID(), "Custom Editor", "", action->statusTip().toStdString());
         mEditor->GetUndoManager()->AddEvent(event.get());

         event = new UndoPropertyEvent(mEditor, graph->GetID(), "Name", "Macro", graph->GetName());
         mEditor->GetUndoManager()->AddEvent(event.get());
      }

      mEditor->GetUndoManager()->EndMultipleEvents();

      mEditor->RefreshGraph(mGraph);
      mEditor->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateActorsFromSelection()
   {
      std::vector<dtDAL::BaseActorObject*> proxies = mEditor->GetActorSelection();

      int count = (int)proxies.size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::Node* node = CreateNode("Actor", "General", mMenuPos.x() + (index * 55), mMenuPos.y());

         if (node)
         {
            dtDAL::ActorIDActorProperty* prop = dynamic_cast<dtDAL::ActorIDActorProperty*>(node->GetProperty("Value"));

            if (prop)
            {
               prop->SetValue(proxies[index]->GetId());
            }
         }
      }

      Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateGroupForSelection()
   {
      std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> > selection = GetSelection();
      if (selection.empty()) return;

      GroupNode* groupNode = dynamic_cast<GroupNode*>(CreateNode("Group Box", "Core", 0.0f, 0.0f));
      if (groupNode)
      {
         QList<NodeItem*> nodeItems;

         int count = (int)selection.size();
         for (int index = 0; index < count; index++)
         {
            Node* node = dynamic_cast<Node*>(selection[index].get());
            if (node)
            {
               NodeItem* item = mPropertyEditor->GetScene()->GetNodeItem(node->GetID(), true);
               if (item) nodeItems.push_back(item);
            }
         }

         // Retrieve the group item that was created for this node.
         GroupItem* groupItem = dynamic_cast<GroupItem*>(mPropertyEditor->GetScene()->GetNodeItem(groupNode->GetID(), true));
         if (groupItem)
         {
            groupItem->SizeToFit(nodeItems);
         }
      }

      dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, groupNode->GetID(), mGraph->GetID());
      mEditor->GetUndoManager()->AddEvent(event);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
   {
      mDragging = false;
      mHasDragged = false;
      mBandSelecting = false;
      mHoldingAlt = (event->modifiers() == Qt::AltModifier);
      mHoldingControl = (event->modifiers() == Qt::ControlModifier);
      mHoldingShift = (event->modifiers() == Qt::ShiftModifier);

      if (event->modifiers() == Qt::ShiftModifier ||
         event->button() == Qt::RightButton)
      {
         mBandSelecting = true;
         mView->setDragMode(QGraphicsView::RubberBandDrag);
         mMenuPos = event->screenPos();
      }
      else
      {
         QList<QGraphicsItem*> itemList = items(event->scenePos());
         bool canDrag = true;
         int count = itemList.count();
         for (int index = 0; index < count; index++)
         {
            // Ignore all path items because they are connection links.
            QGraphicsPathItem* pathItem = dynamic_cast<QGraphicsPathItem*>(itemList[index]);
            if (!pathItem)
            {
               canDrag = false;
               break;
            }
         }

         if (canDrag)
         {
            mDragging = true;
            mDragOrigin = event->scenePos();

            // This is unusual, but I have to set it to scroll
            // drag mode so it does not un-select all selected
            // nodes.
            mView->setDragMode(QGraphicsView::ScrollHandDrag);
         }
      }

      // If we are not mouse clicking on a node...
      QGraphicsScene::mousePressEvent(event);

      // If we are dragging the view, turn off the scroll hand
      // drag mode because I am performing a custom drag instead.
      if (mDragging)
      {
         mView->setDragMode(QGraphicsView::NoDrag);
      }
      else
      {
         QList<QGraphicsItem*> itemList = selectedItems();
         int count = (int)itemList.size();
         for (int index = 0; index < count; index++)
         {
            NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
            if (item) item->BeginMoveEvent();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
   {
      QGraphicsScene::mouseReleaseEvent(event);

      // If you have pressed and released the mouse on an empty
      // area and did not drag, then it should clear the current
      // selection instead.
      if (mDragging)
      {
         if (!mHasDragged)
         {
            clearSelection();
            mSelected.clear();
            RemoveSelected(NULL);
         }
      }
      else if (mHasDragged)
      {
         QList<QGraphicsItem*> itemList = selectedItems();
         int count = (int)itemList.size();

         if (count > 0)
         {
            mEditor->GetUndoManager()->BeginMultipleEvents();

            for (int index = 0; index < count; index++)
            {
               NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
               if (item)
               {
                  osg::Vec2 position = GetSmartSnapPosition(item);
                  item->setPos(position.x(), position.y());
                  item->EndMoveEvent();
               }
            }

            mEditor->GetUndoManager()->EndMultipleEvents();
         }
      }

      if (mBandSelecting)
      {
         mBandSelecting = false;
         mEditor->Refresh();
      }

      mView->setDragMode(QGraphicsView::NoDrag);
      mDragging = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
   {
      QGraphicsScene::mouseMoveEvent(event);

      mHasDragged = true;

      // If we are dragging the view.
      if (mDragging)
      {
         QPointF mousePos = event->scenePos();
         QPointF inc = mDragOrigin - mousePos;
         mDragOrigin = mousePos;

         // Translate the background item to simulate the entire view moving.
         if (mTranslationItem)
         {
            mTranslationItem->setPos(mTranslationItem->pos() - inc);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorScene::dropEvent(QGraphicsSceneDragDropEvent* event)
   {
      const QMimeData *mime = event->mimeData();
      if (!mime->hasFormat("data"))
      {
         event->ignore();
         return;
      }

      QByteArray itemData = mime->data("data");
      QDataStream dataStream(&itemData, QIODevice::ReadOnly);

      QString name; //name of the Node type
      QString category; //category of the Node
      QPoint hotspot; //the dragging hotspot position (distance from the left corner)

      dataStream >> name >> category >> hotspot;

      QPointF pos = event->scenePos() - mTranslationItem->scenePos() - hotspot;

      CreateNodeItem(name.toStdString(), category.toStdString(), pos.x(), pos.y());
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      // If we are rubber band selecting, we don't want a context menu.
      if (!mDragging && mMenuPos != event->screenPos())
      {
         event->accept();
         return;
      }

      QGraphicsScene::contextMenuEvent(event);

      // If none of the child nodes have accepted this event, then
      // produce the default menu.
      if (!event->isAccepted())
      {
         QMenu menu;

         // If this is a child graph, show the goto parent option.
         if (mGraph->GetParent())
         {
            menu.addAction(mEditor->GetParentAction());
            menu.addSeparator();
         }

         QAction* createMacroAction = menu.addAction("Create Macro");
         connect(createMacroAction, SIGNAL(triggered()), this, SLOT(OnCreateMacro()));

         // Add custom macro editing tools
         std::vector<std::string> toolList = mEditor->GetRegisteredToolList();
         if (!toolList.empty())
         {
            QMenu* toolMenu = menu.addMenu("Custom Editor Macro's");
            if (toolMenu)
            {
               connect(toolMenu, SIGNAL(triggered(QAction*)), this, SLOT(OnCreateCustomEditedMacro(QAction*)));

               int count = (int)toolList.size();
               for (int index = 0; index < count; ++index)
               {
                  QAction* macroAction = toolMenu->addAction(QString("\'") + toolList[index].c_str() + "\' Macro");
                  if (macroAction)
                  {
                     macroAction->setStatusTip(toolList[index].c_str());
                     macroAction->setToolTip(QString("Creates a macro that is edited by the custom \'") + toolList[index].c_str() + "\' Editor.");
                  }
               }
            }
         }

         // If we have selected actors in STAGE, add an option to create values for those actors.
         if (mEditor->GetActorSelection().size() > 0)
         {
            QAction* createActorSelection = menu.addAction("Create Actor Nodes Using Selection");
            connect(createActorSelection, SIGNAL(triggered()), this, SLOT(OnCreateActorsFromSelection()));
         }

         // Add the undo and redo options.
         menu.addSeparator();
         menu.addAction(mEditor->GetUndoAction());
         menu.addAction(mEditor->GetRedoAction());
         menu.addSeparator();
         menu.addAction(mEditor->GetPasteAction());
         menu.addSeparator();
         menu.addAction(mEditor->GetRefreshAction());

         // Execute the menu.
         mMenuPos = event->scenePos();
         mMenuPos -= mTranslationItem->scenePos();
         menu.exec(event->screenPos());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorGraph* EditorScene::CreateMacro()
   {
      QString macroName = QInputDialog::getText(NULL, "Name Macro", "Enter the name of the new macro:");
      if (!macroName.isEmpty())
      {
         DirectorGraph* graph = mGraph->AddGraph();
         if (graph)
         {
            graph->SetName(macroName.toStdString());
            graph->SetPosition(osg::Vec2(mMenuPos.x(), mMenuPos.y()));

            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, graph->GetID(), mGraph->GetID());
            mEditor->GetUndoManager()->AddEvent(event);
         }
         return graph;
      }

      return NULL;
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
