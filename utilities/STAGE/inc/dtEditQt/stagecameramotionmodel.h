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

#ifndef STAGE_CAMERAMOTIONMODEL
#define STAGE_CAMERAMOTIONMODEL

// stagecameramotionmodel.h: Declaration of the STAGECameraMotionModel class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtCore/motionmodel.h>

#include <dtEditQt/stagecamera.h>
#include <dtEditQt/export.h>

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>

////////////////////////////////////////////////////////////////////////////////

namespace dtEditQt
{
   class EditorViewport;

   /**
   * This motion model should be inherited to provide
   */
   class DT_EDITQT_EXPORT STAGECameraMotionModel : public dtCore::MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(STAGECameraMotionModel)

   public:

      /**
      * Constructor.
      *
      * @param[in]  name    The instance name.
      */
      STAGECameraMotionModel(const std::string& name = "STAGECameraMotionModel");

   protected:

      /**
      * Deconstructor.
      */
      virtual ~STAGECameraMotionModel();

   public:

      /**
      * Sets the camera for STAGE.
      */
      virtual void SetCamera(StageCamera* camera);

      /**
       *	Retrieves the camera assigned to this model.
       */
      const StageCamera* GetCamera() const;
      StageCamera* GetCamera();

      /**
      * Sets the current viewport.
      */
      virtual void SetViewport(EditorViewport* viewport);

      /**
      * Message handler callback.
      *
      * @param[in] data  The message data
      */
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      /**
      * Event handler when the left mouse button is pressed.
      *
      * @return  Returns true if manual motion is enabled.
      */
      virtual bool OnLeftMousePressed(void);

      /**
      * Event handler when the left mouse button is released.
      *
      * @return  Returns true if manual motion is enabled.
      */
      virtual bool OnLeftMouseReleased(void);

      /**
      * Event handler when the right mouse button is pressed.
      *
      * @return  Returns true if manual motion is enabled.
      */
      virtual bool OnRightMousePressed(void);

      /**
      * Event handler when the right mouse button is released.
      *
      * @return  Returns true if manual motion is enabled.
      */
      virtual bool OnRightMouseReleased(void);

      /**
      * Event handler when the middle mouse button is pressed.
      *
      * @return  Returns true if manual motion is enabled.
      */
      virtual bool OnMiddleMousePressed(void);

      /**
      * Event handler when the middle mouse button is released.
      *
      * @return  Returns true if manual motion is enabled.
      */
      virtual bool OnMiddleMouseReleased(void);

      /**
      * Event handler to enable the camera mode.
      *
      * @param[in]  e  The mouse event.
      *
      * @return     Returns true to confirm the camera should be activated.
      *              If false is returned, the camera mode will not be enabled.
      */
      virtual bool BeginCameraMode(QMouseEvent* e);

      /**
      * Event handler to disable the camera mode.
      *
      * @param[in]  e  The mouse event.
      *
      * @return     Returns true to confirm the camera should be disabled.
      *              If false is returned, the camera mode will not be
      *              turned off.
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

      bool  mLeftMouse;
      bool  mRightMouse;
      bool  mMiddleMouse;

      dtCore::RefPtr<StageCamera> mCamera;
      EditorViewport*             mViewport;

   private:
   };
} // namespace dtEditQt

////////////////////////////////////////////////////////////////////////////////

#endif // STAGE_CAMERAMOTIONMODEL
