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
#include <prefix/dtdirectorqtprefix.h>
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
#include <dtDirectorQt/clipboard.h>

#include <dtDirector/node.h>
#include <dtDirector/nodemanager.h>
#include <dtDirector/nodetype.h>
#include <dtDirector/groupnode.h>
#include <dtDirectorNodes/referencescriptaction.h>

#include <QtCore/QMimeData>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtCore/QMimeData>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QKeyEvent>

#include <dtCore/actoridactorproperty.h>

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
      , mGroupSelectionAction(NULL)
      , mTranslationItem(NULL)
   {
      mPropertyEditor->SetScene(this);

      setBackgroundBrush(Qt::lightGray);

      setSceneRect(0, 0, 10000, 10000);

      mMacroSelectionAction = new QAction("Create Macro with Selection", this);
      mGroupSelectionAction = new QAction("Create Group Around Selection", this);

      connect(mMacroSelectionAction, SIGNAL(triggered()), this, SLOT(OnCreateMacro()));
      connect(mGroupSelectionAction, SIGNAL(triggered()), this, SLOT(OnCreateGroupForSelection()));
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::SetView(EditorView* view)
   {
      mView = view;
   }

   //////////////////////////////////////////////////////////////////////////
   EditorView* EditorScene::GetView() const
   {
      return mView;
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
   void EditorScene::SetGraph(DirectorGraph* graph, bool rememberHistory)
   {
      // If we are currently within a valid graph, save it in history.
      if (rememberHistory && mGraph.valid() && mGraph.get() != graph)
      {
         AddGraphHistory(mGraph.get());
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

      ShowNodes(mGraph.get(), false);

      mEditor->Refresh();
      CenterAll();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::ShowNodes(DirectorGraph* graph, bool imported)
   {
      ID id = graph->GetID();
      id.index = -1;

      // Show the nodes within each imported script first.
      int count = (int)graph->GetDirector()->GetImportedScriptList().size();
      for (int index = 0; index < count; ++index)
      {
         Director* imported = graph->GetDirector()->GetImportedScriptList()[index];
         if (imported)
         {
            DirectorGraph* importedGraph = NULL;
            if (mGraph == mGraph->GetDirector()->GetGraphRoot())
            {
               importedGraph = imported->GetGraphRoot();
            }
            else
            {
               importedGraph = imported->GetGraph(id);
            }

            if (importedGraph)
            {
               ShowNodes(importedGraph, true);
            }
         }
      }

      // Create all nodes in the graph.
      count = (int)graph->GetEventNodes().size();
      for (int index = 0; index < count; index++)
      {
         Node* node = graph->GetEventNodes()[index].get();
         ActionItem* item = new ActionItem(node, node->IsReadOnly(), imported, mTranslationItem, this);
         if (item)
         {
            item->setZValue(10.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)graph->GetActionNodes().size();
      for (int index = 0; index < count; index++)
      {
         Node* node = graph->GetActionNodes()[index].get();
         NodeItem* item = NULL;

         // Special case for reference script nodes.
         if (IS_A(node, ReferenceScriptAction*))
         {
            item = new ScriptItem(node, node->IsReadOnly(), imported, mTranslationItem, this);
            item->setZValue(10.0f);
         }
         // Special case for group frame nodes.
         else if (IS_A(node, GroupNode*))
         {
            item = new GroupItem(node, node->IsReadOnly(), imported, mTranslationItem, this);
            item->setZValue(0.0f);
         }
         else
         {
            item = new ActionItem(node, node->IsReadOnly(), imported, mTranslationItem, this);
            item->setZValue(10.0f);
         }

         if (item)
         {
            mNodes.push_back(item);
         }
      }

      count = (int)graph->GetValueNodes().size();
      for (int index = 0; index < count; index++)
      {
         Node* node = graph->GetValueNodes()[index].get();
         ValueItem* item = new ValueItem(node, node->IsReadOnly(), imported, mTranslationItem, this);
         if (item)
         {
            item->setZValue(20.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)graph->GetSubGraphs().size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* macro = graph->GetSubGraphs()[index].get();

         if (macro)
         {
            ID id = macro->GetID();
            id.index = -1;

            // If the macro already exists, bail.
            if (GetGraphItem(id))
            {
               continue;
            }

            DirectorGraph* parentGraph = mGraph->GetGraph(id);
            if (parentGraph)
            {
               MacroItem* item = new MacroItem(parentGraph, parentGraph->IsReadOnly(), imported, mTranslationItem, this);
               if (item)
               {
                  item->setZValue(10.0f);
                  mNodes.push_back(item);
               }
            }
         }
      }
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

      // Don't snap imported nodes, because they are non-movable.
      if ((item->GetNode() && (item->GetNode()->IsImported() || item->GetNode()->IsReadOnly())) ||
         (item->GetMacro() && (item->GetMacro()->IsImported() || item->GetMacro()->IsReadOnly())))
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

            bool inBounds = false;
            // Only align with nodes that are within range of the current item.
            if (itemBounds.left() - snapRange <= nodeBounds.right() &&
               itemBounds.right() + snapRange >= nodeBounds.left() &&
               itemBounds.top() - snapRange <= nodeBounds.bottom() &&
               itemBounds.bottom() + snapRange >= nodeBounds.top())
            {
               inBounds = true;
               // Snap align with the top of this node.
               mSnapTargetsY.push_back(nodeItem->GetPosition().y());

               // Snap align with the bottom of this node.
               mSnapTargetsY.push_back(nodeItem->GetPosition().y() + nodeItem->GetNodeHeight() - item->GetNodeHeight());

               // Snap align with the left side of this node.
               mSnapTargetsX.push_back(nodeItem->GetPosition().x());

               // Snap align with the right side of this node.
               mSnapTargetsX.push_back(nodeItem->GetPosition().x() + nodeItem->GetNodeWidth() - item->GetNodeWidth());
            }

            // If the nodes are not near each-other, then check if they are
            // directly connected by links.
            if (!inBounds)
            {
               int valueCount = (int)item->GetValues().size();
               for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
               {
                  ValueData& data = item->GetValues()[valueIndex];

                  int testCount = (int)data.link->GetLinks().size();
                  for (int testIndex = 0; testIndex < testCount; ++testIndex)
                  {
                     if (nodeItem->HasNode(data.link->GetLinks()[testIndex]))
                     {
                        inBounds = true;
                        break;
                     }
                  }

                  if (inBounds)
                  {
                     break;
                  }
               }

               if (!inBounds)
               {
                  int valueCount = (int)nodeItem->GetValues().size();
                  for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
                  {
                     ValueData& data = nodeItem->GetValues()[valueIndex];

                     int testCount = (int)data.link->GetLinks().size();
                     for (int testIndex = 0; testIndex < testCount; ++testIndex)
                     {
                        if (item->HasNode(data.link->GetLinks()[testIndex]))
                        {
                           inBounds = true;
                           break;
                        }
                     }

                     if (inBounds)
                     {
                        break;
                     }
                  }
               }

               if (!inBounds)
               {
                  int inputCount = (int)item->GetInputs().size();
                  for (int inputIndex = 0; inputIndex < inputCount; ++inputIndex)
                  {
                     InputData& inData = item->GetInputs()[inputIndex];

                     int testCount = (int)inData.link->GetLinks().size();
                     for (int testIndex = 0; testIndex < testCount; ++testIndex)
                     {
                        if (nodeItem->HasNode(inData.link->GetLinks()[testIndex]->GetOwner()))
                        {
                           inBounds = true;
                           break;
                        }
                     }

                     if (inBounds)
                     {
                        break;
                     }
                  }
               }

               if (!inBounds)
               {
                  int outputCount = (int)item->GetOutputs().size();
                  for (int outputIndex = 0; outputIndex < outputCount; ++outputIndex)
                  {
                     OutputData& outData = item->GetOutputs()[outputIndex];

                     int testCount = (int)outData.link->GetLinks().size();
                     for (int testIndex = 0; testIndex < testCount; ++testIndex)
                     {
                        if (nodeItem->HasNode(outData.link->GetLinks()[testIndex]->GetOwner()))
                        {
                           inBounds = true;
                           break;
                        }
                     }

                     if (inBounds)
                     {
                        break;
                     }
                  }
               }
            }

            if (!inBounds)
            {
               continue;
            }

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
            // All non-value nodes will also snap along the inputs
            // and outputs of each node.
            else
            {
               if (itemBounds.left() > nodeBounds.right())
               {
                  int inputCount = (int)item->GetInputs().size();
                  for (int inputIndex = 0; inputIndex < inputCount; ++inputIndex)
                  {
                     InputData& inData = item->GetInputs()[inputIndex];

                     float offset = inData.linkGraphic->scenePos().y() - item->scenePos().y();

                     int outputCount = (int)nodeItem->GetOutputs().size();
                     for (int outputIndex = 0; outputIndex < outputCount; ++outputIndex)
                     {
                        OutputData& outData = nodeItem->GetOutputs()[outputIndex];

                        float y = outData.linkGraphic->scenePos().y() - nodeItem->scenePos().y();
                        y += nodeItem->GetPosition().y();
                        y -= offset;
                        mSnapTargetsY.push_back(y);
                     }
                  }
               }
               else if (itemBounds.right() < nodeBounds.left())
               {
                  int outputCount = (int)item->GetOutputs().size();
                  for (int outputIndex = 0; outputIndex < outputCount; ++outputIndex)
                  {
                     OutputData& outData = item->GetOutputs()[outputIndex];

                     float offset = outData.linkGraphic->scenePos().y() - item->scenePos().y();

                     int inputCount = (int)nodeItem->GetInputs().size();
                     for (int inputIndex = 0; inputIndex < inputCount; ++inputIndex)
                     {
                        InputData& inData = nodeItem->GetInputs()[inputIndex];

                        float y = inData.linkGraphic->scenePos().y() - nodeItem->scenePos().y();
                        y += nodeItem->GetPosition().y();
                        y -= offset;
                        mSnapTargetsY.push_back(y);
                     }
                  }
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

      // Find the best snap position.
      float nearest = 10.0f;
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

      nearest = 10.0f;
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
   NodeItem* EditorScene::GetNodeItem(const dtDirector::ID& id, bool exactMatch)
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
   NodeItem* EditorScene::GetNodeItem(Node* node, bool exactMatch)
   {
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item)
         {
            if (!exactMatch)
            {
               if (item->HasNode(node)) return item;
            }
            else
            {
               if (item->GetNode() == node) return item;
            }
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   MacroItem* EditorScene::GetGraphItem(const dtDirector::ID& id)
   {
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item && item->GetMacro() &&
            item->GetMacro()->GetID() == id)
         {
            return dynamic_cast<MacroItem*>(item);
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
   void EditorScene::AddSelected(dtCore::PropertyContainer* container)
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
   void EditorScene::RemoveSelected(dtCore::PropertyContainer* container)
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
      dtCore::RefPtr<Node> node = NodeManager::GetInstance().CreateNode(name, category, mGraph.get());
      if (node.valid())
      {
         node->OnFinishedLoading();
         if (mGraph->GetDirector() && mGraph->GetDirector()->HasStarted())
         {
            node->OnStart();
         }

         node->SetPosition(osg::Vec2(x, y));

         // Now refresh the all editors that view the same graph.
         int count = mEditor->GetGraphTabs()->count();
         for (int index = 0; index < count; index++)
         {
            EditorView* view = dynamic_cast<EditorView*>(mEditor->GetGraphTabs()->widget(index));
            if (view && view->GetScene())
            {
               if (view->GetScene()->GetGraph() == mGraph.get())
               {
                  // First remember the position of the translation node.
                  QPointF trans = view->GetScene()->GetTranslationItem()->pos();
                  view->GetScene()->SetGraph(mGraph.get());
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
   Node* EditorScene::CreateNodeItem(const std::string& name, const std::string& category, float x, float y)
   {
      // If the node name is empty, it means we are creating a macro.
      if (name.empty())
      {
         mMenuPos.setX(x);
         mMenuPos.setY(y);
         CreateMacro(category);

         mEditor->RefreshGraph(mGraph.get());
         mEditor->Refresh();
      }
      else
      {
         Node* node = CreateNode(name, category, x, y);
         if (node)
         {
            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, node->GetID(), mGraph->GetID());
            event->SetDescription("Creation of Node \'" + name + "\'.");
            mEditor->GetUndoManager()->AddEvent(event);
         }

         return node;
      }

      return NULL;
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
         event->SetDescription("Creation of Node \'" + name + "\'.");
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

      mEditor->GetUndoManager()->BeginMultipleEvents("Creation of Macro Node with Selection.");

      DirectorGraph* parent = mGraph.get();
      DirectorGraph* graph = CreateMacro("");
      if (graph)
      {
         if (!selection.empty())
         {
            // If there are any nodes selected, cut them.
            mEditor->on_action_Cut_triggered();
         }

         // Switch the current graph to the new macro.
         SetGraph(graph);

         if (!selection.empty())
         {
            // If there are nodes selected, move them into the macro.
            mEditor->PasteNodes(true);
         }
      }

      mEditor->GetUndoManager()->EndMultipleEvents();
      mEditor->RefreshGraph(parent);
      mEditor->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateCustomEditedMacro(QAction* action)
   {
      CreateMacro(action->statusTip().toStdString());

      mEditor->RefreshGraph(mGraph.get());
      mEditor->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateActorsFromSelection()
   {
      dtCore::ActorPtrVector proxies = mEditor->GetActorSelection();

      int count = (int)proxies.size();
      for (int index = 0; index < count; ++index)
      {
         dtDirector::Node* node = CreateNode("Actor", "General", mMenuPos.x() + (index * 55), mMenuPos.y());

         if (node)
         {
            dtCore::ActorIDActorProperty* prop = dynamic_cast<dtCore::ActorIDActorProperty*>(node->GetProperty("Value"));

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
      std::vector<dtCore::RefPtr<dtCore::PropertyContainer> > selection = GetSelection();
      if (selection.empty()) return;

      GroupNode* groupNode = dynamic_cast<GroupNode*>(CreateNode("Group Box", "Core", 0.0f, 0.0f));
      if (groupNode)
      {
         QList<NodeItem*> nodeItems;

         int count = (int)selection.size();
         for (int index = 0; index < count; index++)
         {
            dtDirector::ID id;
            Node* node = dynamic_cast<Node*>(selection[index].get());
            if (node)
            {
               id = node->GetID();
            }
            else
            {
               DirectorGraph* macro = dynamic_cast<DirectorGraph*>(selection[index].get());
               if (macro)
               {
                  id = macro->GetID();
               }
            }

            if (!id.id.ToString().empty())
            {
               NodeItem* item = GetNodeItem(id, true);
               if (item) nodeItems.push_back(item);
            }
         }

         mEditor->GetUndoManager()->BeginMultipleEvents("Creation of a new Group Box.");

         // Retrieve the group item that was created for this node.
         GroupItem* groupItem = dynamic_cast<GroupItem*>(GetNodeItem(groupNode->GetID(), true));
         if (groupItem)
         {
            groupItem->SizeToFit(nodeItems);
         }

         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, groupNode->GetID(), mGraph->GetID());
         mEditor->GetUndoManager()->AddEvent(event);

         mEditor->GetUndoManager()->EndMultipleEvents();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::keyPressEvent(QKeyEvent* event)
   {
      if (event->key() == Qt::Key_Backspace)
      {
         if (event->modifiers() == Qt::ShiftModifier)
         {
            GraphHistoryForward();
         }
         else
         {
            GraphHistoryBack();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
   {
      // Back button should switch graphs to its parent if able.
      if (event->button() == Qt::XButton1 ||
          event->button() == Qt::XButton2)
      {
         return;
      }

      mDragging = false;
      mHasDragged = false;
      mBandSelecting = false;
      mHoldingAlt = (event->modifiers() & Qt::AltModifier);
      mHoldingControl = (event->modifiers() & Qt::ControlModifier);
      mHoldingShift = (event->modifiers() & Qt::ShiftModifier);

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

         if (count > 0)
         {
            for (int index = 0; index < count; ++index)
            {
               QGraphicsItem* item = itemList[index];
               GroupInnerRectItem* innerItem = dynamic_cast<GroupInnerRectItem*>(item);

               if (innerItem)
               {
                  canDrag = true;
                  break;
               }

               LinkItem* linkItem = dynamic_cast<LinkItem*>(item);
               ResizeItem* sizeItem = dynamic_cast<ResizeItem*>(item);
               NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);
               if (linkItem || sizeItem || nodeItem)
               {
                  canDrag = false;
                  break;
               }

               //QGraphicsPathItem* pathItem = dynamic_cast<QGraphicsPathItem*>(item);
               //if (!pathItem)
               //{
               //   canDrag = false;
               //}
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
         if (count > 0)
         {
            if (mHoldingAlt)
            {
               for (int index = 0; index < count; ++index)
               {
                  QGraphicsItem* item = itemList[index];
                  if (item && item->sceneBoundingRect().contains(event->scenePos()))
                  {
                     CopiedNodeBeginDrag(event);
                     return;
                  }
               }
            }

            // If we get here, it means we are not drag-copying but instead are moving nodes.
            for (int index = 0; index < count; index++)
            {
               NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
               if (item) item->BeginMoveEvent();
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
   {
      // Back button should switch graphs to its parent if able.
      if (event->button() == Qt::XButton1)
      {
         GraphHistoryBack();
         return;
      }
      else if (event->button() == Qt::XButton2)
      {
         GraphHistoryForward();
         return;
      }

      QGraphicsScene::mouseReleaseEvent(event);

      // If you have pressed and released the right mouse on an empty
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
            std::string undoDescription = "Movement of a Node.";
            if (count == 1)
            {
               NodeItem* item = dynamic_cast<NodeItem*>(itemList[0]);
               if (item && item->GetNode())
               {
                  undoDescription = "Movement of Node \'" +
                     item->GetNode()->GetTypeName() + "\'";
               }
               else if (item && item->GetMacro())
               {
                  if (item->GetMacro()->GetEditor().empty())
                  {
                     undoDescription = "Movement of Macro Node \'" +
                        item->GetMacro()->GetName() + "\'";
                  }
                  else
                  {
                     undoDescription = "Movement of \'" +
                        item->GetMacro()->GetEditor() + "\' Macro Node \'" +
                        item->GetMacro()->GetName() + "\'";
                  }
               }
            }
            else
            {
               undoDescription = "Movement of multiple Nodes.";
            }
            mEditor->GetUndoManager()->BeginMultipleEvents(undoDescription);

            osg::Vec2 offset;

            for (int index = 0; index < count; index++)
            {
               NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
               if (item)
               {
                  osg::Vec2 oldPos = item->GetPosition();
                  osg::Vec2 newPos = GetSmartSnapPosition(item);

                  osg::Vec2 testOffset = newPos - oldPos;

                  if (testOffset.length2() > 0.0f &&
                     (offset.length2() == 0.0f ||
                      testOffset.length2() < offset.length2()))
                  {
                     offset = testOffset;
                  }
               }
            }

            for (int index = 0; index < count; index++)
            {
               NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
               if (item)
               {
                  osg::Vec2 position = item->GetPosition() + offset;
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
      const QMimeData *mime = event->mimeData();
      if (mime->hasFormat("data"))
      {
         event->acceptProposedAction();
         return;
      }
      else if (mime->hasFormat("DragCopyEvent"))
      {
         // Only allow this item to be dropped if the mouse has moved a significant amount away from it's origin.
         QByteArray itemData = mime->data("DragCopyEvent");
         QDataStream dataStream(&itemData, QIODevice::ReadOnly);
         QPoint hotspot; // The original position of the mouse when this copy was started.
         QPoint oldMousePos;
         dataStream >> hotspot >> oldMousePos;

         QPoint mousePos = event->scenePos().toPoint();
         QPoint move = mousePos - oldMousePos;
         if (move.manhattanLength() <= 75)
         {
            event->ignore();
            return;
         }

         event->acceptProposedAction();
         return;
      }

      event->ignore();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorScene::dropEvent(QGraphicsSceneDragDropEvent* event)
   {
      const QMimeData *mime = event->mimeData();
      if (mime->hasFormat("data"))
      {
         QByteArray itemData = mime->data("data");
         QDataStream dataStream(&itemData, QIODevice::ReadOnly);

         QString name; //name of the Node type
         QString category; //category of the Node
         QString refName; //referenced value name.
         QPoint hotspot; //the dragging hotspot position (distance from the left corner)

         dataStream >> name >> category >> refName >> hotspot;

         QPointF pos = event->scenePos() - mTranslationItem->scenePos() - hotspot;

         if (!name.isEmpty() && !refName.isEmpty())
         {
            mEditor->GetUndoManager()->BeginMultipleEvents("Creation of Node \'" + name.toStdString() + "\'.");
         }

         Node* item = CreateNodeItem(name.toStdString(), category.toStdString(), pos.x(), pos.y());

         if (!refName.isEmpty())
         {
            if (item)
            {
               item->SetString(refName.toStdString(), "Reference");
               dtCore::RefPtr<UndoPropertyEvent> event =
                  new UndoPropertyEvent(mEditor, mGraph->GetID(), "Reference", "", refName.toStdString());
               mEditor->GetUndoManager()->AddEvent(event.get());
               mEditor->RefreshNodeItem(item);
            }

            mEditor->GetUndoManager()->EndMultipleEvents();
         }
         return;
      }
      else if (mime->hasFormat("DragCopyEvent"))
      {
         QByteArray itemData = mime->data("DragCopyEvent");
         QDataStream dataStream(&itemData, QIODevice::ReadOnly);
         QPoint hotspot; // The original position of the mouse when this copy was started.
         QPoint oldMousePos;
         dataStream >> hotspot >> oldMousePos;

         mEditor->on_action_Copy_triggered();

         QPointF pos = event->scenePos() - mTranslationItem->scenePos() - hotspot;
         Clipboard& clipboard = Clipboard::GetInstance();

         std::vector<dtCore::PropertyContainer*> newSelection;
         newSelection = clipboard.PasteObjects(mGraph.get(), mEditor->GetUndoManager(), osg::Vec2(pos.x(), pos.y()));

         mEditor->RefreshGraph(mGraph.get());

         int count = (int)newSelection.size();
         for (int index = 0; index < count; ++index)
         {
            Node* node = dynamic_cast<Node*>(newSelection[index]);
            if (node)
            {
               NodeItem* item = GetNodeItem(node->GetID(), true);
               if (item) item->setSelected(true);
            }
            else
            {
               DirectorGraph* graph = dynamic_cast<DirectorGraph*>(newSelection[index]);
               if (graph)
               {
                  MacroItem* item = GetGraphItem(graph->GetID());
                  if (item) item->setSelected(true);
               }
            }
         }
         return;
      }

      event->ignore();
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
         std::vector<std::string> toolList = mEditor->GetRegisteredToolList(mEditor->GetDirector());
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
   DirectorGraph* EditorScene::CreateMacro(const std::string& editorName)
   {
      QString macroName = QInputDialog::getText(NULL, "Name Macro", "Enter the name of the new macro:");
      if (!macroName.isEmpty())
      {
         DirectorGraph* graph = mGraph->AddGraph();
         if (graph)
         {
            graph->SetName(macroName.toStdString());
            graph->SetPosition(osg::Vec2(mMenuPos.x(), mMenuPos.y()));
            graph->SetEditor(editorName);

            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, graph->GetID(), mGraph->GetID());
            if (editorName.empty())
            {
               event->SetDescription("Creation of Macro Node \'" + graph->GetName() + "\'.");
            }
            else
            {
               event->SetDescription("Creation of \'" + editorName + "\' Macro Node \'" + graph->GetName() + "\'.");
            }
            mEditor->GetUndoManager()->AddEvent(event);
         }
         return graph;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::CopiedNodeBeginDrag(QGraphicsSceneMouseEvent* event)
   {
      QRectF iconBounds;
      QPoint topLeftPos;

      bool first = true;
      QList<QGraphicsItem*> itemList = selectedItems();
      int count = (int)itemList.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
         if (item)
         {
            QRectF itemBounds = item->sceneBoundingRect();
            if (first)
            {
               iconBounds = itemBounds;
               topLeftPos = item->scenePos().toPoint();
               first = false;
            }
            else
            {
               if (iconBounds.left() > itemBounds.left())
               {
                  iconBounds.setLeft(itemBounds.left());
               }
               if (iconBounds.right() < itemBounds.right())
               {
                  iconBounds.setRight(itemBounds.right());
               }
               if (iconBounds.top() > itemBounds.top())
               {
                  iconBounds.setTop(itemBounds.top());
               }
               if (iconBounds.bottom() < itemBounds.bottom())
               {
                  iconBounds.setBottom(itemBounds.bottom());
               }

               if (topLeftPos.x() > item->scenePos().toPoint().x())
               {
                  topLeftPos.setX(item->scenePos().toPoint().x());
               }
               if (topLeftPos.y() > item->scenePos().toPoint().y())
               {
                  topLeftPos.setY(item->scenePos().toPoint().y());
               }
            }
         }
      }

      QImage icon(iconBounds.width() + LINK_LENGTH * 2, iconBounds.height() + LINK_LENGTH * 2, QImage::Format_ARGB32_Premultiplied);
      icon.fill(qRgba(0, 0, 0, 0));

      QPainter painter;
      painter.begin(&icon);

      QStyleOptionGraphicsItem* options = new QStyleOptionGraphicsItem();

      for (int index = 0; index < count; index++)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(itemList[index]);
         if (item)
         {
            //shift to the right to account for some negative geometry
            QRectF itemSceneBound = item->sceneBoundingRect();
            QRectF itemBound = item->boundingRect();
            QPoint offset = QPoint(
               (itemSceneBound.left() - iconBounds.left()) - itemBound.left() + LINK_LENGTH,
               (itemSceneBound.top() - iconBounds.top()) - itemBound.top() + LINK_LENGTH);

            painter.translate(offset);
            PaintItemChildren(&painter, item, options);
            painter.translate(-offset);

            // Draw all link connections.
            int outputCount = (int)item->GetOutputs().size();
            for (int outputIndex = 0; outputIndex < outputCount; ++outputIndex)
            {
               OutputData& data = item->GetOutputs()[outputIndex];
               int connectionCount = (int)data.link->GetLinks().size();
               for (int connectionIndex = 0; connectionIndex < connectionCount; ++connectionIndex)
               {
                  InputLink* link = data.link->GetLinks()[connectionIndex];
                  if (link)
                  {
                     NodeItem* linkedItem = GetNodeItem(link->GetOwner(), false);
                     if (linkedItem && linkedItem->isSelected())
                     {
                        QGraphicsPathItem* connectionGraphic = data.linkConnectors[connectionIndex];
                        if (connectionGraphic)
                        {
                           //shift to the right to account for some negative geometry
                           itemSceneBound = connectionGraphic->sceneBoundingRect();
                           itemBound = connectionGraphic->boundingRect();
                           int penWidth = connectionGraphic->pen().width();
                           offset = QPoint(
                              (itemSceneBound.left() - iconBounds.left()) - itemBound.left() + LINK_LENGTH - penWidth / 2,
                              (itemSceneBound.top() - iconBounds.top()) - itemBound.top() + LINK_LENGTH - penWidth / 2);

                           painter.translate(offset);
                           PaintItemChildren(&painter, connectionGraphic, options);
                           painter.translate(-offset);
                        }
                     }
                  }
               }
            }

            int valueCount = (int)item->GetValues().size();
            for (int valueIndex = 0; valueIndex < valueCount; ++valueIndex)
            {
               ValueData& data = item->GetValues()[valueIndex];
               int connectionCount = (int)data.link->GetLinks().size();
               for (int connectionIndex = 0; connectionIndex < connectionCount; ++connectionIndex)
               {
                  ValueNode* link = data.link->GetLinks()[connectionIndex];
                  if (link)
                  {
                     NodeItem* linkedItem = GetNodeItem(link, false);
                     if (linkedItem && linkedItem->isSelected())
                     {
                        QGraphicsPathItem* connectionGraphic = data.linkConnectors[connectionIndex];
                        if (connectionGraphic)
                        {
                           //shift to the right to account for some negative geometry
                           itemSceneBound = connectionGraphic->sceneBoundingRect();
                           itemBound = connectionGraphic->boundingRect();
                           int penWidth = connectionGraphic->pen().width();
                           offset = QPoint(
                              (itemSceneBound.left() - iconBounds.left()) - itemBound.left() + LINK_LENGTH - penWidth / 2,
                              (itemSceneBound.top() - iconBounds.top()) - itemBound.top() + LINK_LENGTH - penWidth / 2);

                           painter.translate(offset);
                           PaintItemChildren(&painter, connectionGraphic, options);
                           painter.translate(-offset);
                        }
                     }
                  }
               }
            }
         }
      }

      painter.end();

      QDrag* drag = new QDrag(event->widget());
      QMimeData* mime = new QMimeData;
      drag->setMimeData(mime);

      float zoom = GetView()->GetZoomScale();

      icon = icon.scaled(icon.width() * zoom, icon.height() * zoom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      QPixmap pix = QPixmap::fromImage(icon);
      drag->setPixmap(pix);

      QPoint mousePos = event->scenePos().toPoint();
      QPoint hotspot(mousePos - iconBounds.topLeft().toPoint());
      hotspot += QPoint(LINK_LENGTH, LINK_LENGTH);
      hotspot *= zoom;
      drag->setHotSpot(hotspot);

      QByteArray itemData;
      QDataStream dataStream(&itemData, QIODevice::WriteOnly);

      hotspot = QPoint(event->scenePos().toPoint() - topLeftPos);
      dataStream << hotspot << mousePos;

      mime->setData("DragCopyEvent", itemData);
      drag->exec(Qt::CopyAction, Qt::CopyAction);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::PaintItemChildren(QPainter* painter, QGraphicsItem* item, QStyleOptionGraphicsItem* options)
   {
      float scale = item->scale();
      painter->setOpacity(item->opacity() * 0.75f);
      painter->scale(scale, scale);
      item->paint(painter, options);

      int count = item->children().count();
      for (int index = 0; index < count; ++index)
      {
         QGraphicsItem* child = item->children()[index];
         if (child)
         {
            painter->translate(child->pos());

            PaintItemChildren(painter, child, options);

            painter->translate(-child->pos());
         }
      }

      // Undo the previous scale amount.
      scale = 1.0f / scale;
      painter->scale(scale, scale);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::AddGraphHistory(dtDirector::DirectorGraph* graph)
   {
      if (!graph)
      {
         return;
      }

      GraphHistoryData data;
      data.graph = graph;
      data.pos = mTranslationItem->pos();
      data.zoom = GetView()->GetZoomScale();

      mPrevHistory.push_back(data);
      mNextHistory.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::GraphHistoryBack()
   {
      if (!mPrevHistory.empty())
      {
         // Retrieve a previous data that is valid.
         GraphHistoryData data;
         while (!data.graph.valid())
         {
            // Bail out if we ran out of history.
            if (mPrevHistory.empty())
            {
               return;
            }

            data = mPrevHistory.back();
            mPrevHistory.pop_back();
         }

         if (data.graph.valid())
         {
            // If we found a valid graph to go back to, then
            // push our current graph into the forward history.
            if (mGraph.valid())
            {
               GraphHistoryData data;
               data.graph = mGraph;
               data.pos = mTranslationItem->pos();
               data.zoom = GetView()->GetZoomScale();

               mNextHistory.push_back(data);
            }

            dtCore::ObserverPtr<dtDirector::DirectorGraph> oldGraph = mGraph.get();

            // Apply the graph change.
            SetGraph(data.graph.get(), false);
            mTranslationItem->setPos(data.pos);
            GetView()->SetZoomScale(data.zoom);

            // Now auto select the old graph in case it belongs within this graph.
            if (oldGraph.valid())
            {
               MacroItem* macroItem = GetGraphItem(oldGraph->GetID());
               if (macroItem)
               {
                  macroItem->setSelected(true);
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::GraphHistoryForward()
   {
      if (!mNextHistory.empty())
      {
         // Retrieve a forward data that is valid.
         GraphHistoryData data;
         while (!data.graph.valid())
         {
            // Bail out if we ran out of history.
            if (mNextHistory.empty())
            {
               return;
            }

            data = mNextHistory.back();
            mNextHistory.pop_back();
         }

         if (data.graph.valid())
         {
            // If we found a valid graph to go forward to, then
            // push our current graph into the back history.
            if (mGraph.valid())
            {
               GraphHistoryData data;
               data.graph = mGraph.get();
               data.pos = mTranslationItem->pos();
               data.zoom = GetView()->GetZoomScale();

               mPrevHistory.push_back(data);
            }

            dtCore::ObserverPtr<dtDirector::DirectorGraph> oldGraph = mGraph.get();

            // Apply the graph change.
            SetGraph(data.graph.get(), false);
            mTranslationItem->setPos(data.pos);
            GetView()->SetZoomScale(data.zoom);

            // Now auto select the old graph in case it belongs within this graph.
            if (oldGraph.valid())
            {
               MacroItem* macroItem = GetGraphItem(oldGraph->GetID());
               if (macroItem)
               {
                  macroItem->setSelected(true);
               }
            }
         }
      }
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
