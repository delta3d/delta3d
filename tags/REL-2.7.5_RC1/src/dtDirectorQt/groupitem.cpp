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
#include <dtDirectorQt/groupitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/editorview.h>

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/director.h>
#include <dtDirector/groupnode.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <dtUtil/mathdefines.h>

#include <osg/Vec2>

#define AUTO_BORDER_SIZE 40
#define BORDER_THICKNESS 15

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   GroupItem::GroupItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent, EditorScene* scene, bool inPalette)
       : NodeItem(node, readOnly, imported, parent, scene)
       , mInnerRect(NULL)
       , mInPalette(inPalette)
   {
      if (!imported)
      {
         for (int index = 0; index < ResizeItem::RESIZE_COUNT; ++index)
         {
            mResizer[index] = new ResizeItem(this, parent, mScene, (ResizeItem::ResizeType)index);
            mResizer[index]->Init();
         }
      }
      else
      {
         for (int index = 0; index < ResizeItem::RESIZE_COUNT; ++index)
         {
            mResizer[index] = NULL;
         }
      }

      setFlag(QGraphicsItem::ItemIsMovable, !imported);
      setFlag(QGraphicsItem::ItemIsSelectable, true);
   }

   //////////////////////////////////////////////////////////////////////////
   GroupItem::~GroupItem()
   {
      for (int index = 0; index < ResizeItem::RESIZE_COUNT; ++index)
      {
         if (mResizer[index])
         {
            delete mResizer[index];
            mResizer[index] = NULL;
         }
      }
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

         if (mInPalette)
         {
            mPolygon << QPointF(BORDER_THICKNESS, BORDER_THICKNESS);
            mPolygon << QPointF(mNodeWidth - BORDER_THICKNESS, BORDER_THICKNESS);
            mPolygon << QPointF(mNodeWidth - BORDER_THICKNESS, mNodeHeight - BORDER_THICKNESS);
            mPolygon << QPointF(BORDER_THICKNESS, mNodeHeight - BORDER_THICKNESS);
            mPolygon << QPointF(BORDER_THICKNESS, BORDER_THICKNESS);
            mPolygon << QPointF(0, 0);
         }
         else
         {
            mPolygon << QPointF(BORDER_THICKNESS, BORDER_THICKNESS);
            mPolygon << QPointF(BORDER_THICKNESS, mNodeHeight - BORDER_THICKNESS);
            mPolygon << QPointF(mNodeWidth - BORDER_THICKNESS, mNodeHeight - BORDER_THICKNESS);
            mPolygon << QPointF(mNodeWidth - BORDER_THICKNESS, BORDER_THICKNESS);
            mPolygon << QPointF(BORDER_THICKNESS, BORDER_THICKNESS);
            mPolygon << QPointF(0, 0);
            DrawInnerRect();
         }

         setPolygon(mPolygon);

         DrawResizers();

         SetComment(mNode->GetComment());

         SetDefaultPen();
         setBrush(GetNodeColor());
      }

      mLoading = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupItem::DrawInnerRect()
   {
      if (!mInnerRect)
      {
         mInnerRect = new GroupInnerRectItem(this, mScene);
      }

      mInnerRect->setRect(BORDER_THICKNESS, BORDER_THICKNESS, mNodeWidth - BORDER_THICKNESS * 2, mNodeHeight - BORDER_THICKNESS * 2);
      mInnerRect->setPen(QColor(0,0,0,0));

      QColor innerColor = GetNodeColor();
      innerColor.setAlphaF(0.15f);
      mInnerRect->setBrush(innerColor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupItem::DrawResizers()
   {
      for (int index = 0; index < ResizeItem::RESIZE_COUNT; ++index)
      {
         if (mResizer[index])
         {
            mResizer[index]->setPos(mResizer[index]->GetFramePosition());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::DrawComment()
   {
      float scale = 1.0f;
      if (mScene)
      {
         scale = 1.0f / mScene->GetView()->GetZoomScale();
      }
      scale = scale * 1.5;

      mComment->setScale(scale);
      mComment->setTextWidth(dtUtil::Max(100, mNodeWidth));

      // Create the title background.
      QRectF commentBounds = mComment->boundingRect();
      QRectF nodeBounds = boundingRect();

      mComment->setPos(nodeBounds.x(), commentBounds.height() * -scale);
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::BeginMoveEvent()
   {
      NodeItem::BeginMoveEvent();

      UpdateGroupedItems();

      mScene->BeginBatchSelection();
      for (int i = 0; i < mGroupedItems.size(); i++)
      {
         mGroupedItems[i]->setSelected(true);
         mGroupedItems[i]->BeginMoveEvent();
      }
      mScene->EndBatchSelection();
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::EndMoveEvent()
   {
      NodeItem::EndMoveEvent();

      mScene->BeginBatchSelection();
      for (int i = 0; i < mGroupedItems.size(); i++)
      {
         mGroupedItems[i]->setSelected(false);
      }
      mScene->EndBatchSelection();

      mScene->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::UpdateGroupedItems()
   {
      // Update the internal grouped-items-vector and the items' group-information.
      mGroupedItems.clear();
      QList<QGraphicsItem*> items = mScene->items(scenePos().x(), scenePos().y(), GetNodeWidth(), GetNodeHeight(), Qt::ContainsItemShape);
      for (int i = 0; i < items.size(); i++)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(items[i]);
         if (item && item != this)
         {
            mGroupedItems.push_back(item);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupItem::childItemChange(QGraphicsItem* changedItem, GraphicsItemChange change, const QVariant &value)
   {
      if (change == QGraphicsItem::ItemPositionChange)
      {
         // Moving the resize widget will resize the group frame.
         ResizeItem* resizer = dynamic_cast<ResizeItem*>(changedItem);
         dtDirector::GroupNode* groupNode = dynamic_cast<dtDirector::GroupNode*>(mNode.get());
         if (resizer && groupNode)
         {
            ResizeItem::ResizeType type = resizer->GetType();

            QPointF newMin   = QPointF(GetPosition().x(), GetPosition().y());
            QPointF newMax   = newMin + QPointF(groupNode->GetSize().x(), groupNode->GetSize().y());
            QPointF sizerPos = value.toPointF();

            // Top
            if (type == ResizeItem::RESIZE_TOP_LEFT ||
               type == ResizeItem::RESIZE_TOP ||
               type == ResizeItem::RESIZE_TOP_RIGHT)
            {
               newMin.setY(sizerPos.y());

               if (newMax.y() - newMin.y() < MIN_NODE_HEIGHT)
               {
                  newMin.setY(newMax.y() - MIN_NODE_HEIGHT);
               }
            }

            // Bottom
            if (type == ResizeItem::RESIZE_BOT_LEFT ||
               type == ResizeItem::RESIZE_BOT ||
               type == ResizeItem::RESIZE_BOT_RIGHT)
            {
               newMax.setY(sizerPos.y());

               if (newMax.y() - newMin.y() < MIN_NODE_HEIGHT)
               {
                  newMax.setY(newMin.y() + MIN_NODE_HEIGHT);
               }
            }

            // Left
            if (type == ResizeItem::RESIZE_TOP_LEFT ||
               type == ResizeItem::RESIZE_LEFT ||
               type == ResizeItem::RESIZE_BOT_LEFT)
            {
               newMin.setX(sizerPos.x());

               if (newMax.x() - newMin.x() < MIN_NODE_WIDTH)
               {
                  newMin.setX(newMax.x() - MIN_NODE_WIDTH);
               }
            }

            // Right
            if (type == ResizeItem::RESIZE_TOP_RIGHT ||
                type == ResizeItem::RESIZE_RIGHT ||
                type == ResizeItem::RESIZE_BOT_RIGHT)
            {
               newMax.setX(sizerPos.x());

               if (newMax.x() - newMin.x() < MIN_NODE_WIDTH)
               {
                  newMax.setX(newMin.x() + MIN_NODE_WIDTH);
               }
            }

            setPos(newMin);
            groupNode->SetSize(osg::Vec2(newMax.x() - newMin.x(), newMax.y() - newMin.y()));

            Draw();
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

      osg::Vec2 firstPos = nodeItems[0]->GetPosition();

      osg::Vec2 minPos = firstPos;
      osg::Vec2 maxPos = firstPos;

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

      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents("Resizing of Node \'Group Box\'.");
      dtCore::ActorProperty* prop = groupNode->GetProperty("Position");
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
      UpdateGroupedItems();
      SizeToFit(mGroupedItems);
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::OnPreSizing()
   {
      dtCore::ActorProperty* prop = mNode->GetProperty("Position");
      if (prop)
      {
         mOldPos = prop->ToString();
      }

      prop = mNode->GetProperty("Size");
      if (prop)
      {
         mOldSize = prop->ToString();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::OnPostSizing()
   {
      mScene->GetEditor()->GetUndoManager()->BeginMultipleEvents("Resizing of Node \'Group Box\'.");
      dtCore::ActorProperty* prop = mNode->GetProperty("Position");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value != mOldPos)
         {
            // Notify the undo manager of the property changes.
            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), GetID(), prop->GetName(), mOldPos, value);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
         }
      }

      prop = mNode->GetProperty("Size");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value != mOldSize)
         {
            // Notify the undo manager of the property changes.
            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), GetID(), prop->GetName(), mOldSize, value);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
         }
      }
      mScene->GetEditor()->GetUndoManager()->EndMultipleEvents();
   }

   //////////////////////////////////////////////////////////////////////////
   bool GroupItem::IsPointInInnerRect(const QPointF& point) const
   {
      if (mInnerRect)
      {
         QRectF rect = mInnerRect->rect();
         rect.setRect(mInnerRect->scenePos().x(), mInnerRect->scenePos().y(), rect.width(), rect.height());
         return rect.contains(point);
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void GroupItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);

      QMenu menu;

      QAction* sizeToFitAction = menu.addAction("Size to Fit Contents");
      connect(sizeToFitAction, SIGNAL(triggered()), this, SLOT(SizeToFit()));

      if (IsImported() || IsReadOnly())
      {
         sizeToFitAction->setEnabled(false);
      }

      menu.addSeparator();
      menu.addAction(mScene->GetMacroSelectionAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetDeleteAction());
      menu.exec(event->screenPos());
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

            Draw();
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

   //////////////////////////////////////////////////////////////////////////
   QColor GroupItem::GetNodeColor() const
   {
      QColor color = NodeItem::GetNodeColor();
      color.setAlphaF(0.35f);
      return color;
   }
}

//////////////////////////////////////////////////////////////////////////
