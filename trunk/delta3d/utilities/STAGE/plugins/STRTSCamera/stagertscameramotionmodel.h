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

#ifndef STAGE_RTSCAMERAMOTIONMODEL
#define STAGE_RTSCAMERAMOTIONMODEL

// stagecameramotionmodel3d.h: Declaration of the STAGERTSCameraMotionModel class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtEditQt/stagecameramotionmodel.h>

////////////////////////////////////////////////////////////////////////////////

namespace dtCore
{
   class Mouse;
   class Axis;
}

/**
* RTS Motion for the STAGE Camera.
*/
class STAGERTSCameraMotionModel : public dtEditQt::STAGECameraMotionModel
{
   DECLARE_MANAGEMENT_LAYER(STAGERTSCameraMotionModel)

public:

   /**
   * Constructor.
   *
   * @param[in]  mouse  The mouse device.
   * @param[in]  name   The instance name.
   */
   STAGERTSCameraMotionModel(dtCore::Mouse* mouse, const std::string& name = "STAGERTSCameraMotionModel");

protected:

   /**
   * Deconstructor.
   */
   virtual ~STAGERTSCameraMotionModel();

public:

   /**
   * Sets the camera for STAGE.
   */
   virtual void SetCamera(dtEditQt::StageCamera* camera);

   /**
    *	Sets the ground height.
    *
    * @param[in]  height  The height of the ground.
    */
   void SetGroundHeight(float height);

   /**
    *	Gets the ground height.
    */
   float GetGroundHeight() const;

   /**
   * Message handler callback.
   *
   * @param[in] data  The message data
   */
   virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

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

   float mDistance;
   float mCurrentDistance;

   float mLinearRate;

   osg::Vec3 mFocalPoint;
   osg::Vec3 mFocalOffset;
   osg::Vec3 mCurrentTranslation;

   float    mMinCameraHeight;
   float    mTransitionSpeed;

   dtCore::Mouse* mMouse;
};

////////////////////////////////////////////////////////////////////////////////

#endif // STAGE_RTSCAMERAMOTIONMODEL
