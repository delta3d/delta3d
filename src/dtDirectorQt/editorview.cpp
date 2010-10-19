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

#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>

#include <QtGui/QMouseEvent>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   EditorView::EditorView(EditorScene* scene, QWidget* parent)
      : QGraphicsView(scene, parent)
      , mScene(scene)
      , mMinScale(0.01f)
      , mMaxScale(1.5f)
      , mCurrentScale(1.0f)
   {
      setObjectName("Graph Tab");
      setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

      setTransformationAnchor(QGraphicsView::AnchorViewCenter);

      // Always hide the scroll bars.
      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorView::wheelEvent(QWheelEvent* event)
   {
      // Delta is in eighths of degrees
      int numberOfDegrees = event->delta() / 8;

      // Typical mice move in 15 degree steps
      float numberOfSteps = (float)numberOfDegrees / 15.0f;

      float inc = (numberOfSteps * 0.1f) + 1.0f;

      // Clamp the scale to the min and max range.
      if (mCurrentScale * inc > mMaxScale)
      {
         inc = mMaxScale / mCurrentScale;
      }
      else if (mCurrentScale * inc < mMinScale)
      {
         inc = mMinScale / mCurrentScale;
      }

      mCurrentScale *= inc;
      scale(inc, inc);

      // Translate the view towards the mouse cursor.
      //if (numberOfSteps > 0.0f)
      //{
      //   QPointF centerPos(width()/2, height()/2);
      //   QPointF mousePos = event->pos();

      //   QPointF translation = mousePos - centerPos;
      //   centerPos = mapToScene(centerPos.x(), centerPos.y());
      //   centerOn(centerPos);

      //   translation = mapToScene(translation.x(), translation.y());
      //   //translation *= 0.2f;
      //   mScene->GetTranslationItem()->setPos(
      //      mScene->GetTranslationItem()->pos() - translation);
      //}
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
