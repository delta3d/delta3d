// stagecameramotionmodel3d.cpp: Implementation of the STAGERTSCameraMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include "stagertscameramotionmodel.h"
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>

#include <dtCore/axistoaxistransformation.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/system.h>
#include <dtCore/view.h>
#include <dtCore/transformable.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/transform.h>
#include <dtCore/camera.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>

IMPLEMENT_MANAGEMENT_LAYER(STAGERTSCameraMotionModel)

////////////////////////////////////////////////////////////////////////////////
STAGERTSCameraMotionModel::STAGERTSCameraMotionModel(dtCore::Mouse* mouse, const std::string& name)
   : dtEditQt::STAGECameraMotionModel(name)
   , mDistance(100.0f)
   , mCurrentDistance(100.0f)
   , mLinearRate(1.0f)
   , mMinCameraHeight(-5.0f)
   , mTransitionSpeed(2.0f)
   , mMouse(mouse)
{
}

////////////////////////////////////////////////////////////////////////////////
STAGERTSCameraMotionModel::~STAGERTSCameraMotionModel()
{
}

////////////////////////////////////////////////////////////////////////////////
void STAGERTSCameraMotionModel::SetCamera(dtEditQt::StageCamera* camera)
{
   STAGECameraMotionModel::SetCamera(camera);

   if (camera)
   {
      dtCore::Transform transform;
      camera->getDeltaCamera()->GetTransform(transform);

      osg::Vec3 xyz, hpr;
      transform.Get(xyz, hpr);
      osg::Vec3 right, up, forward;
      transform.GetOrientation(right, up, forward);

      // We need to calculate a focal point that is equidistant in front of
      // the camera as it is from the ground height (zero).
      mDistance = 0.0f;
      if (forward.z() != 1.0f)
      {
         mDistance = (xyz.z() - mMinCameraHeight) / (1.0f - forward.z());
      }

      mCurrentDistance = mDistance;

      mFocalOffset = -forward;
      mFocalPoint = xyz - (mFocalOffset * mDistance);
      mCurrentTranslation = mFocalPoint;
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGERTSCameraMotionModel::SetGroundHeight(float height)
{
   mMinCameraHeight = height;
   SetCamera(GetCamera());
}

////////////////////////////////////////////////////////////////////////////////

float STAGERTSCameraMotionModel::GetGroundHeight() const
{
   return mMinCameraHeight;
}

////////////////////////////////////////////////////////////////////////////////
void STAGERTSCameraMotionModel::OnSystem(const dtUtil::RefString& str, double delta, double deltaReal)

{
   if(str != dtCore::System::MESSAGE_PRE_FRAME &&
      str != dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
   {
      return;
   }

   if (mCamera.valid() && IsEnabled())
   {
      dtCore::Transform transform;
      mCamera->getDeltaCamera()->GetTransform(transform);

      // Interpolate the distance.
      float travel = mDistance - mCurrentDistance;
      if (dtUtil::Abs(travel) > 0.001f)
      {
         double alpha = (delta * mTransitionSpeed);
         if (alpha > 1.0f) alpha = 1.0f;

         mCurrentDistance += travel * alpha;
      }
      else
      {
         mCurrentDistance = mDistance;
      }

      // Interpolate the focal point.
      osg::Vec3 targetToGoal = mFocalPoint - mCurrentTranslation;
      if (targetToGoal.length() > 0.001f)
      {
         double alpha = (delta * mTransitionSpeed);
         if (alpha > 1.0f) alpha = 1.0f;

         mCurrentTranslation += (targetToGoal * alpha);
      }
      else
      {
         mCurrentTranslation = mFocalPoint;
      }

      transform.SetTranslation(mCurrentTranslation + (mFocalOffset * mCurrentDistance));
      mCamera->getDeltaCamera()->SetTransform(transform);
      mCamera->update();
      mViewport->GetObjectMotionModel()->UpdateWidgets();
   }
}

////////////////////////////////////////////////////////////////////////////////
bool STAGERTSCameraMotionModel::BeginCameraMode(QMouseEvent* e)
{
   if (dtEditQt::STAGECameraMotionModel::BeginCameraMode(e))
   {
      if (!mViewport) return false;

      mViewport->trapMouseCursor();

      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool STAGERTSCameraMotionModel::EndCameraMode(QMouseEvent* e)
{
   if (dtEditQt::STAGECameraMotionModel::EndCameraMode(e))
   {
      if (!mViewport) return false;

      // If we are still holding down a mouse button, don't end the camera yet.
      if (mLeftMouse || mRightMouse)
      {
         return false;
      }

      mViewport->releaseMouseCursor();

      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool STAGERTSCameraMotionModel::OnMouseMoved(float dx, float dy)
{
   if (dtEditQt::STAGECameraMotionModel::OnMouseMoved(dx, dy))
   {
      // Bail if we don't have a viewport.
      if (!mViewport)
      {
         return true;
      }

      // Both or middle mouse buttons translate the camera along the x/y plane.
      if (mMiddleMouse || (mLeftMouse && mRightMouse))
      {
         dtCore::Transform transform;
         mCamera->getDeltaCamera()->GetTransform(transform);

         // Reset the pitch so the translation is only on the x/y plane.
         osg::Vec3 hpr;
         transform.GetRotation(hpr);
         hpr[1] = 0.0f;
         transform.SetRotation(hpr);

         osg::Vec3 right, up, forward;
         transform.GetOrientation(right, up, forward);

         osg::Vec3 translation = right * dx * mDistance * mLinearRate * mViewport->getMouseSensitivity() * 0.01f;
         translation -= forward * dy * mDistance * mLinearRate * mViewport->getMouseSensitivity() * 0.01f;

         mFocalPoint += translation;
         mCurrentTranslation += translation;
      }
      // Any single mouse button will rotate the camera view.
      else if (mLeftMouse || mRightMouse)
      {
         dtCore::Transform transform;
         mCamera->getDeltaCamera()->GetTransform(transform);

         // Reset the pitch so the translation is only on the x/y plane.
         osg::Vec3 hpr;
         transform.GetRotation(hpr);
         hpr[1] -= dy * 0.2f;// * mViewport->getMouseSensitivity();
         hpr[0] -= dx * 0.2f;// * mViewport->getMouseSensitivity();

         // Clamp the elevation so it does not go beyond its' limits.
         if (hpr[1] < -89.9f)    hpr[1] = -89.9f;
         else if (hpr[1] > 89.0f) hpr[1] = 89.0f;

         transform.SetRotation(hpr);
         mCamera->getDeltaCamera()->SetTransform(transform);

         osg::Vec3 right, up, forward;
         transform.GetOrientation(right, up, forward);

         mFocalOffset = -forward;
      }

      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool STAGERTSCameraMotionModel::WheelEvent(int delta)
{
   if (dtEditQt::STAGECameraMotionModel::WheelEvent(delta))
   {
      float distDelta = -float(delta * mDistance * mLinearRate * mViewport->getMouseSensitivity() * 0.002f);

      osg::Vec3 translation (0.0f, -distDelta, 0.0f);

      // Translate towards the mouse position when zooming in.
      if (distDelta < 0.0f)
      {
         osg::Vec2 mousePos = mMouse->GetPosition();
         translation.x() -= distDelta * mousePos.x() * 2.0f;
         translation.z() -= distDelta * mousePos.y() * 2.0f;
      }

      dtCore::Transform transform;
      mCamera->getDeltaCamera()->GetTransform(transform);

      osg::Vec3 hpr;
      transform.GetRotation(hpr);

      osg::Matrix mat;
      dtUtil::MatrixUtil::HprToMatrix(mat, hpr);
      translation = osg::Matrix::transform3x3(translation, mat);

      // Move the focal point and make sure it does not go below our min threshold.
      mFocalPoint += translation;
      if (mFocalPoint.z() < mMinCameraHeight) mFocalPoint.z() = mMinCameraHeight;

      mDistance = (mFocalPoint.z() - mMinCameraHeight);
      dtUtil::ClampMin(mDistance, 1.0f);

      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
