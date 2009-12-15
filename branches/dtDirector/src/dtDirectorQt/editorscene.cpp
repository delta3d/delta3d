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
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/macroitem.h>

#include <dtDirector/nodemanager.h>
#include <dtDirector/nodetype.h>

#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QMenu>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   EditorScene::EditorScene(PropertyEditor* propEditor, GraphTabs* graphTabs, QWidget* parent)
      : QGraphicsScene(parent)
      , mPropertyEditor(propEditor)
      , mGraphTabs(graphTabs)
      , mView(NULL)
      , mGraph(NULL)
      , mDragging(false)
      , mHasDragged(false)
      , mTranslationItem(NULL)
   {
      mPropertyEditor->SetScene(this);

      setBackgroundBrush(Qt::lightGray);
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
      count = (int)mGraph->mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->mEventNodes[index].get();
         ActionItem* item = new ActionItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(0.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->mActionNodes[index].get();
         ActionItem* item = new ActionItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(0.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->mValueNodes[index].get();
         ValueItem* item = new ValueItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(10.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraph* graph = mGraph->mSubGraphs[index].get();
         MacroItem* item = new MacroItem(graph, mTranslationItem, this);
         if (item)
         {
            item->setZValue(0.0f);
            mNodes.push_back(item);
         }
      }

      mEditor->Refresh();
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

      mEditor->RefreshButtonStates();
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

      mEditor->RefreshButtonStates();
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
   void EditorScene::OnMenuEvent(QAction* action)
   {
      if (!action) return;
      
      if (action->text() == "Create Macro")
      {
         DirectorGraph* graph = mGraph->AddGraph();
         if (graph)
         {
            graph->SetPosition(osg::Vec2(mMenuPos.x(), mMenuPos.y()));
            mEditor->RefreshGraph(mGraph);
            mEditor->Refresh();

            dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, graph->GetID(), mGraph->GetID());
            mEditor->GetUndoManager()->AddEvent(event);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::OnCreateNodeEvent(QAction* action)
   {
      if (!action) return;

      std::string name = action->text().toStdString();
      std::string category = action->statusTip().toStdString();

      Node* node = CreateNode(name, category, mMenuPos.x(), mMenuPos.y());
      if (node)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(mEditor, node->GetID(), mGraph->GetID());
         mEditor->GetUndoManager()->AddEvent(event);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
   {
      mDragging = false;
      mHasDragged = false;
      bool bMultiSelect = false;
      if (event->modifiers() == Qt::ShiftModifier ||
         event->button() == Qt::RightButton)
      {
         bMultiSelect = true;
         mView->setDragMode(QGraphicsView::RubberBandDrag);
         mMenuPos = event->screenPos();
      }
      else if(!itemAt(event->scenePos()))
      {
         mDragging = true;
         mDragOrigin = event->scenePos();

         // This is unusual, but I have to set it to scroll
         // drag mode so it does not un-select all selected
         // nodes.
         mView->setDragMode(QGraphicsView::ScrollHandDrag);
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
               if (item) item->EndMoveEvent();
            }

            mEditor->GetUndoManager()->EndMultipleEvents();
         }
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
         if (mGraph->mParent)
         {
            menu.addAction(mEditor->GetParentAction());
            menu.addSeparator();
         }

         QMenu* nodeMenu = new QMenu("Create Node");

         if (nodeMenu)
         {
            std::map<std::string, QMenu*> folders;

            // Get the list of available nodes to create.
            std::vector<const NodeType*> nodes;
            NodeManager::GetInstance().GetNodeTypes(nodes);

            int count = (int)nodes.size();
            for (int index = 0; index < count; index++)
            {
               const NodeType* node = nodes[index];
               if (node)
               {
                  // Special case, the parent graph does not get the link
                  // nodes, because they have no effect.
                  if (!mGraph->mParent && node->GetFolder() == "Links")
                  {
                     continue;
                  }

                  // Find the folder.
                  if (folders.find(node->GetFolder()) == folders.end())
                  {
                     //QMenu* folder = nodeMenu->addMenu(node->GetFolder().c_str());
                     folders[node->GetFolder()] = new QMenu(node->GetFolder().c_str());
                  }

                  QMenu* folder = folders[node->GetFolder()];
                  if (folder)
                  {
                     QAction* action = folder->addAction(node->GetName().c_str());
                     if (action)
                     {
                        action->setStatusTip(node->GetCategory().c_str());
                        action->setToolTip(node->GetDescription().c_str());
                     }
                  }
               }
            }

            std::map<std::string, QMenu*>::iterator i = folders.begin();
            for (i = folders.begin(); i != folders.end(); i++)
            {
               QMenu* folder = i->second;
               if (i->first == "Links")
               {
                  menu.addMenu(folder);

                  connect(folder, SIGNAL(triggered(QAction*)),
                     this, SLOT(OnCreateNodeEvent(QAction*)));
               }
               else
               {
                  nodeMenu->addMenu(folder);
               }
            }

            menu.addMenu(nodeMenu);
            QAction* createMacroAction = menu.addAction("Create Macro");

            connect(nodeMenu, SIGNAL(triggered(QAction*)),
               this, SLOT(OnCreateNodeEvent(QAction*)));
         }

         // Add the undo and redo options.
         menu.addSeparator();
         menu.addAction(mEditor->GetUndoAction());
         menu.addAction(mEditor->GetRedoAction());
         menu.addSeparator();
         menu.addAction(mEditor->GetPasteAction());
         menu.addSeparator();
         menu.addAction(mEditor->GetRefreshAction());

         connect(&menu, SIGNAL(triggered(QAction*)),
            this, SLOT(OnMenuEvent(QAction*)));

         // Execute the menu.
         mMenuPos = event->scenePos();
         mMenuPos -= mTranslationItem->scenePos();
         menu.exec(event->screenPos());
      }
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
