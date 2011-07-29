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
#include <dtDirectorQt/resizeitem.h>
#include <dtDirectorQt/groupitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtCore/datatype.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ResizeItem::ResizeItem(GroupItem* nodeItem, QGraphicsItem* parent, EditorScene* scene, ResizeType type)
      : QGraphicsPolygonItem(parent, scene)
      , mScene(scene)
      , mNodeItem(nodeItem)
      , mType(type)
   {
      setZValue(100.0f);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ResizeItem::Init()
   {
      setFlag(QGraphicsItem::ItemIsMovable, true);
      //setFlag(QGraphicsItem::ItemIsSelectable, true);
      setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);

#if(QT_VERSION >= 0x00040600)
      setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif

      setAcceptHoverEvents(true);

      const float size = 5.0f;
      float x = 0;
      float y = 0;

      if (mType == RESIZE_TOP_LEFT ||
         mType == RESIZE_TOP ||
         mType == RESIZE_TOP_RIGHT)
      {
         y -= size;
      }

      if (mType == RESIZE_BOT_LEFT ||
         mType == RESIZE_BOT_RIGHT ||
         mType == RESIZE_BOT)
      {
         y += size;
      }

      if (mType == RESIZE_TOP_LEFT ||
         mType == RESIZE_LEFT ||
         mType == RESIZE_BOT_LEFT)
      {
         x -= size;
      }

      if (mType == RESIZE_TOP_RIGHT ||
         mType == RESIZE_BOT_RIGHT ||
         mType == RESIZE_RIGHT)
      {
         x += size;
      }

      QPolygonF poly;
      poly << QPointF(x - size, y - size)
           << QPointF(x + size, y - size)
           << QPointF(x + size, y + size)
           << QPointF(x - size, y + size);
      setPolygon(poly);

      SetHighlight(false);
   }

   //////////////////////////////////////////////////////////////////////////
   QPointF ResizeItem::GetFramePosition()
   {
      float x = mNodeItem->GetPosition().x();
      float y = mNodeItem->GetPosition().y();

      float w = mNodeItem->GetNodeWidth();
      float h = mNodeItem->GetNodeHeight();

      if (mType == RESIZE_TOP ||
         mType == RESIZE_BOT)
      {
         x += w * 0.5f;
      }

      if (mType == RESIZE_LEFT ||
         mType == RESIZE_RIGHT)
      {
         y += h * 0.5f;
      }

      if (mType == RESIZE_BOT_LEFT ||
         mType == RESIZE_BOT_RIGHT ||
         mType == RESIZE_BOT)
      {
         y += h;
      }

      if (mType == RESIZE_TOP_RIGHT ||
         mType == RESIZE_BOT_RIGHT ||
         mType == RESIZE_RIGHT)
      {
         x += w;
      }

      return QPointF(x, y);
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::SetHighlight(bool enable)
   {
      setPen(Qt::NoPen);
      setBrush(QBrush( enable ? Qt::yellow : QColor(0, 0, 0, 75) ));
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(true);
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
   {
      SetHighlight(false);
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      QGraphicsPolygonItem::mousePressEvent(mouseEvent);

      if (!mouseEvent) return;

      if (mScene)
      {
         mScene->clearSelection();
      }

      if (mNodeItem)
      {
         mNodeItem->OnPreSizing();
      }

      mouseEvent->accept();
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      QGraphicsPolygonItem::mouseReleaseEvent(mouseEvent);

      if (!mouseEvent) return;

      if (mNodeItem)
      {
         mNodeItem->OnPostSizing();
      }

      mouseEvent->accept();
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      QGraphicsPolygonItem::mouseMoveEvent(mouseEvent);

      if (!mouseEvent) return;

      mouseEvent->accept();
   }

   //////////////////////////////////////////////////////////////////////////
   QVariant ResizeItem::itemChange(GraphicsItemChange change, const QVariant &value)
   {
      if (change == QGraphicsItem::ItemPositionChange && mNodeItem)
      {
         if (!mNodeItem->IsLoading())
         {
            mNodeItem->childItemChange(this, change, value);

            return GetFramePosition();
         }
      }

      return value;
   }
}

//////////////////////////////////////////////////////////////////////////
