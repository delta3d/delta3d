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

#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/linkitem.h>

#include <dtDirector/valuenode.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <dtDAL/datatype.h>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ValueItem::ValueItem(Node* node, QGraphicsItem* parent, EditorScene* scene)
       : NodeItem(node, parent, scene)
       , mValueLink(NULL)
   {
      mLoading = true;

      SetTitle(mNode->GetName());
      DrawTitle();

      DrawPolygonTop();
      DrawPolygonRightFlat();
      DrawPolygonBottomRound();
      DrawPolygonLeftFlat();

      ValueNode *valueNode = dynamic_cast<ValueNode *>(mNode.get());
      if (valueNode)
      {
         setPen(QPen(GetDarkColorForType(valueNode->GetType().GetTypeId()), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         QRadialGradient radialGradient(mNodeWidth/2, mNodeHeight, mNodeWidth, mNodeWidth/2, mNodeHeight);
         radialGradient.setColorAt(0.0, GetColorForType(valueNode->GetType().GetTypeId()));
         radialGradient.setColorAt(1.0, GetDarkColorForType(valueNode->GetType().GetTypeId()));
         setBrush(radialGradient);

         mValueLink = new ValueNodeLinkItem(this, this, scene);
         if (mValueLink)
         {
            QPolygonF poly;
            poly << QPointF(-LINK_SIZE/2, 0.0f) << QPointF(LINK_SIZE/2, 0.0f) <<
               QPointF(LINK_SIZE/2, -LINK_LENGTH + LINK_SIZE/2) <<
               QPointF(0, -LINK_LENGTH) <<
               QPointF(-LINK_SIZE/2, -LINK_LENGTH + LINK_SIZE/2);

            mValueLink->setPolygon(poly);
            mValueLink->setPos(mNodeWidth / 2, -1.0f);

            mValueLink->SetType(valueNode->GetType().GetTypeId());
            mValueLink->setPen(QPen(GetDarkColorForType(mValueLink->GetType()), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            mValueLink->setBrush(GetColorForType(mValueLink->GetType()));
         }
      }
      setPolygon(mPolygon);

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::SetTitle(const std::string text)
   {
      if (!mTitle)
      {
         mTitle = new QGraphicsTextItem(this, scene());
         mTitle->setTextWidth(VALUE_TITLE_LENGTH);
         mTitle->setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
      }

      mTitle->setPlainText(text.c_str());

      // Create the title background.
      QRectF bounds = mTitle->boundingRect();
      mTextHeight = bounds.height();

      if (mNodeHeight < mTextHeight) mNodeHeight = mTextHeight;

      // Clamp the bounds to our min and max.
      if (bounds.width() > MAX_NODE_WIDTH - 2) bounds.setWidth(MAX_NODE_WIDTH - 2);
      if (bounds.width() < MIN_NODE_WIDTH - 2) bounds.setWidth(MIN_NODE_WIDTH - 2);

      // Resize the width of the node if it is not wide enough already.
      if (bounds.width() + 2 > mNodeWidth)
      {
         mNodeWidth = bounds.width();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueItem::ConnectLinks(bool fullConnect)
   {
      NodeItem::ConnectLinks(fullConnect);

      if (fullConnect && mNode.valid())
      {
         ValueNode* valueNode = dynamic_cast<ValueNode*>(mNode.get());
         if (valueNode)
         {
            int count = (int)valueNode->GetLinks().size();
            for (int index = 0; index < count; index++)
            {
               ValueLink* link = valueNode->GetLinks()[index];
               if (!link) continue;

               NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
               if (!item) continue;

               int valueCount = (int)item->GetValues().size();
               for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
               {
                  ValueData& value = item->GetValues()[valueIndex];
                  if (value.link)
                  {
                     int linkCount = (int)value.link->GetLinks().size();
                     value.ResizeLinks(linkCount, mScene);
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        if (value.link->GetLinks()[linkIndex] == mNode.get())
                        {
                           NodeItem::ConnectLinks(value, this, linkIndex);
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
