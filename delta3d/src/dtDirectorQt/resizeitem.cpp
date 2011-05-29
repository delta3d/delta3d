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
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDAL/datatype.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ResizeItem::ResizeItem(NodeItem* nodeItem, QGraphicsItem* parent, EditorScene* scene)
      : QGraphicsPolygonItem(parent, scene)
      , mScene(scene)
      , mNodeItem(nodeItem)
   {
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

      const float size = 15;
      QPolygonF poly;
      poly << QPointF(0, -size)
         << QPointF(0, 0)
         << QPointF(-size, 0);
      setPolygon(poly);

      SetHighlight(false);
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

      dtDAL::ActorProperty* prop = mNodeItem->GetNode()->GetProperty("Size");
      if (prop) mOldSize = prop->ToString();

      mouseEvent->accept();
   }

   //////////////////////////////////////////////////////////////////////////
   void ResizeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
   {
      QGraphicsPolygonItem::mouseReleaseEvent(mouseEvent);

      if (!mouseEvent) return;

      //if (mNodeItem)
      //{
      //   mNodeItem->setSelected(true);
      //}

      dtDAL::ActorProperty* prop = mNodeItem->GetNode()->GetProperty("Size");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value != mOldSize)
         {
            // Notify the undo manager of the property changes.
            dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), mNodeItem->GetID(), prop->GetName(), mOldSize, value);
            mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
         }
      }

      //QPointF mousePos = mouseEvent->scenePos();
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
      if (mNodeItem)
      {
         mNodeItem->childItemChange(this, change, value);
      }

      return value;
   }
}

//////////////////////////////////////////////////////////////////////////
