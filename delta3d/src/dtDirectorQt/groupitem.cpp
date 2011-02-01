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

#include <dtDirectorQt/groupitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/resizeitem.h>
#include <dtDirectorQt/lockitem.h>

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/director.h>
#include <dtDirector/groupnode.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   GroupItem::GroupItem(Node* node, QGraphicsItem* parent, EditorScene* scene, bool canResize /*= true*/)
       : NodeItem(node, parent, scene)
       , mLocker(NULL)
       , mResizer(NULL)
       , mCanResize(canResize)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::Draw()
   {
      NodeItem::Draw();

      mLoading = true;

      dtCore::RefPtr<dtDirector::GroupNode> groupNode = dynamic_cast<dtDirector::GroupNode*>(mNode.get());
      if (groupNode.valid())
      {

         mNodeWidth = groupNode->GetSize().x();
         mNodeHeight = groupNode->GetSize().y();

         DrawPolygonTop();
         DrawPolygonRightFlat();
         DrawPolygonBottomFlat();
         DrawPolygonLeftFlat();

         setPolygon(mPolygon);

         // Draw the locker.
         if (!mLocker)
         {
            mLocker = new LockItem(this, this, mScene);
            mLocker->Init();
         }

         mLocker->setPos(0, 0);

         if (mCanResize)
         {
            // Draw the resizer.
            if (!mResizer)
            {
               mResizer = new ResizeItem(this, this, mScene);
               mResizer->Init();
            }

            mResizer->setPos(mNodeWidth, mNodeHeight);
         }

         SetComment(mNode->GetComment());

         SetDefaultPen();
         setBrush(GetNodeColor());
      }

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::BeginMoveEvent()
   {
      NodeItem::BeginMoveEvent();

      mMovingNodes.clear();
      if (mLocker->IsLocked())
      {
         // Find all nodes that are placed on top of this group and have them
         // selected as well so they will move along with the group.
         QList<QGraphicsItem*> movingItems = mScene->items(scenePos().x(), scenePos().y(), GetNodeWidth(), GetNodeHeight(), Qt::ContainsItemShape);

         int count = (int)movingItems.size();
         for (int index = 0; index < count; index++)
         {
            NodeItem* item = dynamic_cast<NodeItem*>(movingItems[index]);
            if (item && !item->isSelected() && item != this)
            {
               mMovingNodes.push_back(item);
            }
         }

         mScene->BeginBatchSelection();
         count = (int)mMovingNodes.size();
         for (int index = 0; index < count; index++)
         {
            NodeItem* item = mMovingNodes[index];
            if (item)
            {
               item->setSelected(true);
               item->BeginMoveEvent();
            }
         }
         mScene->EndBatchSelection();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::EndMoveEvent()
   {
      NodeItem::EndMoveEvent();

      mScene->BeginBatchSelection();
      osg::Vec2 pos = GetPosition();
      int count = (int)mMovingNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mMovingNodes[index];
         if (item)
         {
            item->setSelected(false);
         }
      }
      mMovingNodes.clear();
      mScene->EndBatchSelection();
      //mScene->Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupItem::childItemChange(QGraphicsItem* changedItem, GraphicsItemChange change, const QVariant &value)
   {
      if (mLoading) return;

      // Moving the resize widget will resize the group frame.
      if (changedItem && changedItem == mResizer)
      {
         if (change == QGraphicsItem::ItemPositionChange)
         {
            QPointF newPos = value.toPointF();

            //bool clamped = false;
            if (newPos.x() < MIN_NODE_WIDTH)
            {
               newPos.setX(MIN_NODE_WIDTH);
               //clamped = true;
            }
            if (newPos.y() < MIN_NODE_HEIGHT)
            {
               newPos.setY(MIN_NODE_HEIGHT);
               //clamped = true;
            }

            //if (clamped)
            //{
            //   mResizer->setPos(newPos);
            //   return;
            //}

            dtCore::RefPtr<dtDirector::GroupNode> groupNode = dynamic_cast<dtDirector::GroupNode*>(mNode.get());
            if (groupNode.valid())
            {
               groupNode->SetSize(osg::Vec2(newPos.x(), newPos.y()));
               Draw();
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupItem::SizeToFit(const QList<NodeItem*>& nodeItems)
   {
      // Don't resize if there is nothing to fit it to.
      if (nodeItems.empty()) return;

      dtCore::RefPtr<dtDirector::GroupNode> groupNode = dynamic_cast<dtDirector::GroupNode*>(mNode.get());
      if (!groupNode.valid()) return;

      osg::Vec2 minPos = GetPosition() + groupNode->GetSize();
      osg::Vec2 maxPos = GetPosition();

      int count = (int)nodeItems.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = nodeItems[index];
         if (item && item != this)
         {
            QRectF rect = item->boundingRect();

            osg::Vec2 offset = osg::Vec2(rect.x(), rect.y());
            osg::Vec2 topLeft = item->GetPosition() + offset;
            osg::Vec2 botRight= osg::Vec2(topLeft.x() + rect.width(), topLeft.y() + rect.height());

            if (minPos.x() > topLeft.x()) minPos.x() = topLeft.x();
            if (minPos.y() > topLeft.y()) minPos.y() = topLeft.y();
            if (maxPos.x() < botRight.x()) maxPos.x() = botRight.x();
            if (maxPos.y() < botRight.y()) maxPos.y() = botRight.y();
         }
      }

      // Add a border.
      minPos.x() -= AUTO_BORDER_SIZE;
      minPos.y() -= AUTO_BORDER_SIZE;
      maxPos.x() += AUTO_BORDER_SIZE;
      maxPos.y() += AUTO_BORDER_SIZE;

      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents();
      dtDAL::ActorProperty* prop = groupNode->GetProperty("Position");
      if (prop)
      {
         std::string oldValue = prop->ToString();
         groupNode->SetPosition(minPos);
         std::string newValue = prop->ToString();

         // Ignore the property if the node did not move.
         if (newValue != oldValue)
         {
            // Notify the undo manager of the property changes.
            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), groupNode->GetID(), prop->GetName(), oldValue, newValue);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
         }
      }

      prop = groupNode->GetProperty("Size");
      if (prop)
      {
         std::string oldValue = prop->ToString();
         groupNode->SetSize(maxPos - minPos);
         std::string newValue = prop->ToString();

         // Ignore the property if the node did not move.
         if (newValue != oldValue)
         {
            // Notify the undo manager of the property changes.
            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), groupNode->GetID(), prop->GetName(), oldValue, newValue);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
         }
      }
      mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();

      // Now resize the frame.
      Draw();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupItem::SizeToFit()
   {
      QList<QGraphicsItem*> fittingItems = mScene->items(scenePos().x(), scenePos().y(), GetNodeWidth(), GetNodeHeight(), Qt::ContainsItemShape);
      QList<NodeItem*> nodeItems;

      int count = (int)fittingItems.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(fittingItems[index]);
         if (item && item != this)
         {
            nodeItems.push_back(item);
         }
      }

      SizeToFit(nodeItems);
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      if (isSelected())
      {
         QMenu menu;

         QAction* sizeToFitAction = menu.addAction("Size to Fit Contents");
         connect(sizeToFitAction, SIGNAL(triggered()), this, SLOT(SizeToFit()));

         menu.addSeparator();
         menu.addAction(mScene->GetMacroSelectionAction());
         menu.addSeparator();
         menu.addAction(mScene->GetEditor()->GetCutAction());
         menu.addAction(mScene->GetEditor()->GetCopyAction());
         menu.addSeparator();
         menu.addAction(mScene->GetEditor()->GetDeleteAction());
         menu.exec(event->screenPos());
      }
      else
      {
         event->ignore();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   QVariant GroupItem::itemChange(GraphicsItemChange change, const QVariant &value)
   {
      if (!mNode.valid()) return value;

      if (change == QGraphicsItem::ItemPositionHasChanged)
      {
         QPointF newPos = value.toPointF();

         mNode->SetPosition(osg::Vec2(newPos.x(), newPos.y()));

         if (!mLoading)
         {
            ConnectLinks(true);
         }
      }
      else if (change == QGraphicsItem::ItemSelectedHasChanged)
      {
         if (isSelected())
         {
            setZValue(zValue() + 1.0f);
            mScene->AddSelected(mNode.get());
         }
         else
         {
            setZValue(zValue() - 1.0f);
            mScene->RemoveSelected(mNode.get());
         }
      }

      return value;
   }
}

//////////////////////////////////////////////////////////////////////////
