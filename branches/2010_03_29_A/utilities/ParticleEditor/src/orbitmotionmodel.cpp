#include <orbitmotionmodel.h>

#include <dtUtil/matrixutil.h>
#include <dtCore/transform.h>

//////////////////////////////////////////////////////////////////////////////////
OrbitMotionModel::OrbitMotionModel(dtCore::Mouse* mouse, dtCore::Transformable* target)
: mpCompassTransform(NULL)
{
   mMouse = mouse;
   mTarget = target;

   mMouse->AddMouseListener(this);

   mAzimuth = 0.0f;
   mElevation = 22.5f;
   mDistance = 50.0f;

   mCenter.set(0, 0, 0);

   UpdateTargetTransform();
}

//////////////////////////////////////////////////////////////////////////////////
OrbitMotionModel::~OrbitMotionModel()
{
}

//////////////////////////////////////////////////////////////////////////////////
bool OrbitMotionModel::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   mouse->GetPosition(mLastX, mLastY);
   return true;
}

//////////////////////////////////////////////////////////////////////////////////
bool OrbitMotionModel::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   float dX = x - mLastX, dY = y - mLastY;

   if (mouse->GetButtonState(dtCore::Mouse::LeftButton))
   {
      mAzimuth -= dX*90;
      mElevation -= dY*90;

      if (mElevation < -90.0f) mElevation = -90.0f;
      else if (mElevation > 90.0f) mElevation = 90.0f;
   }

   if (mouse->GetButtonState(dtCore::Mouse::MiddleButton))
   {
      mDistance -= dY*20.0f;

      if (mDistance < 1.0f) mDistance = 1.0f;
   }

   if (mouse->GetButtonState(dtCore::Mouse::RightButton))
   {
      dtCore::Transform transform;

      mTarget->GetTransform(transform);

      osg::Matrix mat;

      transform.Get(mat);

      osg::Vec3 offset (-dX*mDistance*0.25f, 0, -dY*mDistance*0.25f);

      //sgXformVec3(offset, mat);
      osg::Matrix::transform3x3(offset, mat);

      //sgAddVec3(mCenter, offset);
      mCenter += offset;
   }

   UpdateTargetTransform();

   mLastX = x;
   mLastY = y;

   return true;
}

//////////////////////////////////////////////////////////////////////////////////
bool OrbitMotionModel::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
{
   mDistance -= delta*5;

   if (mDistance < 1.0f) mDistance = 1.0f;

   UpdateTargetTransform();
   return true;
}

//////////////////////////////////////////////////////////////////////////////////
void OrbitMotionModel::UpdateTargetTransform()
{
   dtCore::Transform transform;

   transform.SetTranslation(
      mCenter[0] + mDistance * cosf(osg::DegreesToRadians(mElevation)) * sinf(osg::DegreesToRadians(mAzimuth)),
      mCenter[1] + mDistance * cosf(osg::DegreesToRadians(mElevation)) * -cosf(osg::DegreesToRadians(mAzimuth)),
      mCenter[2] + mDistance * sinf(osg::DegreesToRadians(mElevation))
      );

   transform.SetRotation(
      mAzimuth,
      -mElevation,
      0.0f
      );

   mTarget->SetTransform(transform);

   osg::Matrix mat;

   transform.Get(mat);

   osg::Vec3 vec (-0.3, 1, -0.225);

   //sgXformPnt3(vec, mat);
   dtUtil::MatrixUtil::TransformVec3(vec, mat);

   osg::Matrix osgMat;

   osgMat.makeTranslate(vec[0], vec[1], vec[2]);

   if(mpCompassTransform != NULL)
   {
      mpCompassTransform->setMatrix(osgMat);
   }
}

//////////////////////////////////////////////////////////////////////////////////
