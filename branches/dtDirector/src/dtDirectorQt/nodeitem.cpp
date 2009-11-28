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

#include <dtDirectorQt/nodeitem.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   NodeItem::NodeItem(Node* node, QGraphicsItem *parent, QGraphicsScene *scene)
       : QGraphicsPolygonItem(parent, scene)
       , mNode(node)
       , mTitle(NULL)
       , mComment(NULL)
       , mContextMenu(NULL)
       , mNodeWidth(MIN_NODE_WIDTH)
       , mNodeHeight(MIN_NODE_HEIGHT)
   {
      setFlag(QGraphicsItem::ItemIsMovable, true);
      setFlag(QGraphicsItem::ItemIsSelectable, true);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonTop()
   {
      mPolygon.clear();

      mPolygon << QPointF(0, 0) << QPointF(mNodeWidth, 0);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonRightRound()
   {
      QPointF point(0, -mNodeHeight / 2);
      QPointF center(mNodeWidth, mNodeHeight / 2);

      int points = mNodeHeight / 5;
      float angle = DegToRad(180 / points);
      for (int index = 0; index <= points; index++)
      {
         mPolygon << RotatePoint(point, angle * index) + center;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonRightFlat()
   {
      mPolygon << QPointF(mNodeWidth, mNodeHeight);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonBottomRound()
   {
      QPointF point(mNodeWidth / 2, 0);
      QPointF center(mNodeWidth / 2, mNodeHeight);

      int points = mNodeWidth / 5;
      float angle = DegToRad(180 / points);
      for (int index = 0; index <= points; index++)
      {
         mPolygon << RotatePoint(point, angle * index) + center;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonBottomFlat()
   {
      mPolygon << QPointF(0, mNodeHeight);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonLeftRound()
   {
      QPointF point(0, mNodeHeight / 2);
      QPointF center(0, mNodeHeight / 2);

      int points = mNodeHeight / 5;
      float angle = DegToRad(180 / points);
      for (int index = 0; index <= points; index++)
      {
         mPolygon << RotatePoint(point, angle * index) + center;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonLeftFlat()
   {
      mPolygon << QPointF(0, 0);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawTitle(const std::string text)
   {
      if (!mTitle)
      {
         mTitleBG = new QGraphicsRectItem(this, scene());
         mTitle = new QGraphicsTextItem(mTitleBG, scene());

         mTitleBG->setBrush(Qt::gray);
         mTitleBG->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
         mTitleBG->setOpacity(0.5f);

         mTitle->setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
      }

      mTitle->setPlainText(text.c_str());

      // Create the title background.
      QRectF bounds = mTitle->boundingRect();

      // Clamp the bounds to our min and max.
      if (bounds.width() > MAX_NODE_WIDTH) bounds.setWidth(MAX_NODE_WIDTH);
      if (bounds.width() < MIN_NODE_WIDTH) bounds.setWidth(MIN_NODE_WIDTH);

      mTitleBG->setRect(bounds);

      // Resize the width of the node if it is not wide enough already.
      if (bounds.width() > mNodeWidth)
      {
         mNodeWidth = bounds.width();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //int NodeItem::DrawInputs()
   //{
   //   if (!mNode.valid()) return 0;

   //   int height = (LINK_SPACING + LINK_SIZE) * mNode->GetInputLinks().size();

   //   if (height < MIN_NODE_HEIGHT) height = MIN_NODE_HEIGHT;
   //   return height;
   //}

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawOutputs()
   {
      QRectF bounds;

      if (!mNode.valid()) return;

      int count = (int)mNode->GetOutputLinks().size();
      for (int index = 0; index < count; index++)
      {

      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //int NodeItem::CalculateOutputHeight()
   //{
   //   if (!mNode.valid()) return 0;

   //   int height = (LINK_SPACING + LINK_SIZE) * mNode->GetOutputLinks().size();

   //   if (height < MIN_NODE_HEIGHT) height = MIN_NODE_HEIGHT;
   //   return height;
   //}

   ////////////////////////////////////////////////////////////////////////////
   //int NodeItem::CalculateHeight()
   //{
   //   int inputHeight = DrawInputs();
   //   int outputHeight = CalculateOutputHeight();

   //   if (inputHeight > outputHeight) return inputHeight;
   //   else return outputHeight;
   //}

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      if (mContextMenu)
      {
         scene()->clearSelection();
         setSelected(true);
         mContextMenu->exec(event->screenPos());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
   {
       if (change == QGraphicsItem::ItemPositionChange)
       {
          QPointF newPos = value.toPointF();
          mNode->SetPosition(osg::Vec2(newPos.x(), newPos.y()));
       }

       return value;
   }
}

//////////////////////////////////////////////////////////////////////////
