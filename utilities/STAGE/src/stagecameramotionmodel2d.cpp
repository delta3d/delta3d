// stagecameramotionmodel2d.cpp: Implementation of the STAGECameraMotionModel2D class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/stageprefix.h>
#include <dtEditQt/stagecameramotionmodel2d.h>
#include <dtEditQt/editorviewport.h>

#include <dtCore/system.h>
#include <dtCore/view.h>
#include <dtCore/transformableactorproxy.h>

namespace dtEditQt
{
   IMPLEMENT_MANAGEMENT_LAYER(STAGECameraMotionModel2D)

   ////////////////////////////////////////////////////////////////////////////////
   STAGECameraMotionModel2D::STAGECameraMotionModel2D(const std::string& name)
      : STAGECameraMotionModel(name)
      , mCameraMode(NOTHING)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   STAGECameraMotionModel2D::~STAGECameraMotionModel2D()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel2D::BeginCameraMode(QMouseEvent* e)
   {
      if (STAGECameraMotionModel::BeginCameraMode(e))
      {
         if (mLeftMouse)
         {
            mCameraMode = CAMERA_PAN;
         }
         else if (mRightMouse)
         {
            mCameraMode = CAMERA_ZOOM;
            mZoomToPosition = mCamera->getPosition();
         }
         else
         {
            return false;
         }

         if (mViewport)
         {
            mViewport->trapMouseCursor();
         }

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel2D::EndCameraMode(QMouseEvent* e)
   {
      if (STAGECameraMotionModel::EndCameraMode(e))
      {
         mCameraMode = NOTHING;

         if (mViewport)
         {
            mViewport->releaseMouseCursor();
         }

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel2D::OnMouseMoved(float dx, float dy)
   {
      if (STAGECameraMotionModel::OnMouseMoved(dx, dy))
      {
         // If we're not in any camera modes or we are missing a viewport, bail.
         if (mCameraMode == NOTHING || !mViewport)
         {
            return true;
         }

         // Pan the camera.
         if (mCameraMode == CAMERA_PAN)
         {
            // Calculate the translation amount.
            float xAmount = (-dx * mViewport->getMouseSensitivity()) / mCamera->getZoom();
            float yAmount = ( dy * mViewport->getMouseSensitivity()) / mCamera->getZoom();

            mCamera->move(mCamera->getRightDir() * xAmount);
            mCamera->move(mCamera->getUpDir() * yAmount);
         }
         else if (mCameraMode == CAMERA_ZOOM)
         {
            osg::Vec3 moveVec = mZoomToPosition - mCamera->getPosition();

            moveVec.normalize();
            if (dy <= -1.0f)
            {
               mCamera->zoom(1.0f + 0.1f * mViewport->getMouseSensitivity());
            }
            else if (dy >= 1.0f)
            {
               mCamera->zoom(1.0f - 0.1f * mViewport->getMouseSensitivity());
            }
         }

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel2D::WheelEvent(int delta)
   {
      if (STAGECameraMotionModel::WheelEvent(delta))
      {
         if (mViewport)
         {
            if (delta > 0)
            {
               mCamera->zoom(1.0f + 0.3f * mViewport->getMouseSensitivity());
            }
            else
            {
               mCamera->zoom(1.0f - 0.3f * mViewport->getMouseSensitivity());
            }

            return true;
         }
      }

      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
