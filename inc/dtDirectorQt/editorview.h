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

#ifndef DIRECTORQT_EDITOR_VIEW
#define DIRECTORQT_EDITOR_VIEW

#include <vector>

#include <dtDirectorQt/export.h>

#include <QtGui/QGraphicsView>

#include <QtCore/QTimer>

class QWheelEvent;

namespace dtDirector
{
   class EditorScene;

   /**
    * @class EditorView
    */
   class EditorView: public QGraphicsView
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  scene   The scene.
       * @param[in]  parent  The parent.
       */
      EditorView(EditorScene* scene, QWidget* parent = 0);

      /**
       * Retrieves the graphic scene.
       */
      EditorScene* GetScene() {return mScene;}

      /**
       * Retrieves the current zoom scale.
       */
      float GetZoomScale() const {return mCurrentScale;}

      void SetZoomScale(float zoom);

   protected:

      /**
       * Event handler when the mouse wheel is scrolled.
       *
       * @param[in]  event  The wheel event.
       */
      void wheelEvent(QWheelEvent* event);

   public slots:

      /**
       * Timer interval callback.
       */
      void TimerTick();

   private:

      EditorScene* mScene;

      float mMinScale;
      float mMaxScale;
      float mCurrentScale;
      float mGoalScale;

      QTimer  mTimer;
      QPointF mFocusPos;
   };
} // namespace dtDirector

#endif // DIRECTORQT_EDITOR_VIEW
