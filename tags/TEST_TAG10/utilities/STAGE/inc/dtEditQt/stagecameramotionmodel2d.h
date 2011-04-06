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
 * Author: Jeffrey P Houde
 */

#ifndef STAGE_CAMERAMOTIONMODEL2D
#define STAGE_CAMERAMOTIONMODEL2D

// stagecameramotionmodel2d.h: Declaration of the STAGECameraMotionModel2D class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtEditQt/stagecameramotionmodel.h>

////////////////////////////////////////////////////////////////////////////////

namespace dtEditQt
{
   /**
   * This motion model should be inherited to provide 
   */
   class STAGECameraMotionModel2D : public STAGECameraMotionModel
   {
      DECLARE_MANAGEMENT_LAYER(STAGECameraMotionModel2D)

      enum CameraModeEnum
      {
         CAMERA_PAN,
         CAMERA_ZOOM,
         NOTHING,
      };

   public:

      /**
      * Constructor.
      *
      * @param[in]  name  The instance name
      */
      STAGECameraMotionModel2D(const std::string& name = "STAGECameraMotionModel2D");

   protected:

      /**
      * Deconstructor.
      */
      virtual ~STAGECameraMotionModel2D();

   public:

      /**
      * Event handler to enable the camera mode.
      *
      * @param[in]  e  The mouse event.
      */
      virtual bool BeginCameraMode(QMouseEvent* e);

      /**
      * Event handler to disable the camera mode.
      *
      * @param[in]  e  The mouse event.
      */
      virtual bool EndCameraMode(QMouseEvent* e);

      /**
      * Event handler when the mouse position has moved.
      *
      * @param[in]  dx      Delta X motion.
      * @param[in]  dy      Delta Y motion.
      *
      * @return     Returns true if manual motion is enabled.
      */
      virtual bool OnMouseMoved(float dx, float dy);

      /**
      * Event handler when the mouse wheel is scrolled.
      *
      * @param[in]  delta   The scroll delta.
      */
      virtual bool WheelEvent(int delta);

   protected:

   private:

      CameraModeEnum mCameraMode;
      osg::Vec3      mZoomToPosition;
   };
} // namespace dtEditQt

////////////////////////////////////////////////////////////////////////////////

#endif // STAGE_CAMERAMOTIONMODEL2D
