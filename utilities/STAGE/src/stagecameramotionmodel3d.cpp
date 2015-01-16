// stagecameramotionmodel3d.cpp: Implementation of the STAGECameraMotionModel3D class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/stageprefix.h>
#include <dtEditQt/stagecameramotionmodel3d.h>
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>

#include <dtCore/system.h>
#include <dtCore/view.h>
#include <dtCore/transformable.h>

//dev
//#include <iostream>
//#include <osg/io_utils>

namespace dtEditQt
{
   IMPLEMENT_MANAGEMENT_LAYER(STAGECameraMotionModel3D)

   ////////////////////////////////////////////////////////////////////////////////
   STAGECameraMotionModel3D::STAGECameraMotionModel3D(const std::string& name)
      : STAGECameraMotionModel(name)
      , mCameraMode(NOTHING)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   STAGECameraMotionModel3D::~STAGECameraMotionModel3D()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel3D::BeginCameraMode(QMouseEvent* e)
   {
      if (STAGECameraMotionModel::BeginCameraMode(e))
      {
         if (!mViewport) return false;

         bool bBeginNewCamera = false;
         if (mCameraMode == NOTHING) bBeginNewCamera = true;

         if ((mLeftMouse && mRightMouse) || mMiddleMouse)
         {
            mCameraMode = CAMERA_TRANSLATE;
         }
         else if (mLeftMouse)
         {
            mCameraMode = CAMERA_NAVIGATE;
         }
         else if (mRightMouse)
         {
            mCameraMode = CAMERA_LOOK;
         }
         else
         {
            mCameraMode = NOTHING;
            return false;
         }

         if (bBeginNewCamera)
         {
            mViewport->trapMouseCursor();

            return true;
         }

         return false;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel3D::EndCameraMode(QMouseEvent* e)
   {
      if (STAGECameraMotionModel::EndCameraMode(e))
      {
         if (!mViewport) return false;

         if (!mLeftMouse && mRightMouse)
         {
            mCameraMode = CAMERA_LOOK;
            return false;
         }
         else if (mLeftMouse && !mRightMouse)
         {
            mCameraMode = CAMERA_NAVIGATE;
            return false;
         }
         else
         {
            mCameraMode = NOTHING;
            mViewport->releaseMouseCursor();

            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel3D::OnMouseMoved(float dx, float dy)
   {
      if (STAGECameraMotionModel::OnMouseMoved(dx, dy))
      {
         // If we are not in a camera mode or there is no viewport, bail.
         if (mCameraMode == NOTHING || !mViewport)
         {
            return true;
         }

         // Navigate the camera.
         if (mCameraMode == CAMERA_NAVIGATE)
         {
            mCamera->yaw(-dx / 10.0f);

            //Move along the view direction, however, ignore the z-axis.  This way
            //we can look at the ground but move parallel to it.
            osg::Vec3 viewDir = mCamera->getViewDir() * -dy * mViewport->getMouseSensitivity();
            viewDir[2] = 0.0f;
            mCamera->move(viewDir);
         }
         else if (mCameraMode == CAMERA_LOOK)
         {
            mCamera->pitch(-dy * 0.1f);
            mCamera->yaw(-dx * 0.1f);
         }
         else if (mCameraMode == CAMERA_TRANSLATE)
         {
            mCamera->move(mCamera->getUpDir() * -dy * mViewport->getMouseSensitivity());
            mCamera->move(mCamera->getRightDir() * dx * mViewport->getMouseSensitivity());
         }

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel3D::WheelEvent(int delta)
   {
      if (STAGECameraMotionModel::WheelEvent(delta))
      {
         return true;
      }

      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
