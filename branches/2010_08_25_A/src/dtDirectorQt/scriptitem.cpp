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

#include <dtDirectorQt/scriptitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ScriptItem::ScriptItem(Node* node, QGraphicsItem* parent, EditorScene* scene)
      : ActionItem(node, parent, scene)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ScriptItem::Draw()
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

         QLinearGradient linearGradient(0,0,0,mNodeHeight);

         setPen(QPen(mColorDarken, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

         if (mNode->IsEnabled())
         {
            if (mNode->GetNodeLogging())
            {
               setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            }

            QColor color = Qt::darkGreen;
            color.setAlphaF(0.80f);
            linearGradient.setColorAt(1.0, color);

            color = Qt::green;
            color.setAlphaF(0.80f);
            linearGradient.setColorAt(0.0, color);
         }
         else
         {
            QColor color = Qt::darkGreen;
            color.setAlphaF(0.25f);
            linearGradient.setColorAt(1.0, color);

            color = Qt::green;
            color.setAlphaF(0.25f);
            linearGradient.setColorAt(0.0, color);
         }

         // If we are in replay mode and if this node is the current node,
         // make sure we highlight it with a special color.
         if (mScene->GetEditor()->GetReplayMode() &&
            mScene->GetEditor()->GetReplayNode().nodeID == mNode->GetID())
         {
            setPen(QPen(Qt::yellow, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         }

         setBrush(linearGradient);
         setPolygon(mPolygon);

         SetComment(mNode->GetComment());
      }

      mLoading = false;
   }
}

//////////////////////////////////////////////////////////////////////////
