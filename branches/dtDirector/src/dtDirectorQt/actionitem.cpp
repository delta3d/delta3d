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

#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/directoreditor.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ActionItem::ActionItem(Node* node, QGraphicsItem* parent, EditorScene* scene)
       : NodeItem(node, parent, scene)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ActionItem::Draw()
   {
      NodeItem::Draw();

      mLoading = true;

      if (mNode.valid())
      {
         SetTitle(mNode->GetName());
         if (mNode->InputsExposed())  DrawInputs();
         if (mNode->ValuesExposed())  SetupValues();
         if (mNode->OutputsExposed()) DrawOutputs();

         // Now draw the node.
         DrawTitle();
         DrawValues();

         DrawPolygonTop();
         if (mNode->OutputsExposed()) DrawPolygonRightFlat();
         else                         DrawPolygonRightRound();
         DrawPolygonBottomFlat();
         if (mNode->InputsExposed()) DrawPolygonLeftFlat();
         else                        DrawPolygonLeftRound();

         DrawDividers();

         int size = mNodeWidth;
         if (size < mNodeHeight) size = mNodeHeight;

         QRadialGradient radialGradient(mNodeWidth/2, mNodeHeight/2, size, mNodeWidth/2, mNodeHeight/2);
         if (!mNode->GetDisabled())
         {
            setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            QColor color = Qt::darkCyan;
            color.setAlphaF(0.80f);
            radialGradient.setColorAt(0.0, color);

            color = Qt::cyan;
            color.setAlphaF(0.80f);
            radialGradient.setColorAt(1.0, color);
         }
         else
         {
            setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            QColor color = Qt::darkCyan;
            color.setAlphaF(0.25f);
            radialGradient.setColorAt(0.0, color);

            color = Qt::cyan;
            color.setAlphaF(0.25f);
            radialGradient.setColorAt(1.0, color);
         }

         setBrush(radialGradient);
         setPolygon(mPolygon);
      }

      mLoading = false;
   }
}

//////////////////////////////////////////////////////////////////////////
