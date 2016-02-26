// stageobjectmotionmodel.cpp: Implementation of the ToolObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include "toolobjectmotionmodel.h"

#include <dtActors/linkedpointsactorproxy.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/transformableactorproxy.h>

#include <osg/Math>
#include <osg/Matrix>
#include <osg/Vec3>

IMPLEMENT_MANAGEMENT_LAYER(ToolObjectMotionModel)

////////////////////////////////////////////////////////////////////////////////
ToolObjectMotionModel::ToolObjectMotionModel(dtCore::View* view)
   : dtCore::ObjectMotionModel(view)
   , mPointIndex(-1)
   , mActiveActor(NULL)
{
   ResetGetMouseLineFunc();
   ResetObjectToScreenFunc();
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::SetPointIndex(int index)
{
   mPointIndex = index;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::SetActiveActor(dtActors::LinkedPointsActor* actor)
{
   mActiveActor = actor;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnLeftMousePressed(void)
{
   mLeftMouse = true;
   mSnap = true;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnLeftMouseReleased(void)
{
   mLeftMouse = false;
   mSnap = false;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnRightMousePressed(void)
{
   mRightMouse = true;
   mSnap = true;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnRightMouseReleased(void)
{
   mRightMouse = false;
   mSnap = false;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   // This function is overritten to do nothing.
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::ResetGetMouseLineFunc(void)
{
   mGetMouseLineFunc = dtUtil::MakeFunctor(
      &ToolObjectMotionModel::DefaultGetMouseLineFunc, this);
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::SetGetMouseLineFunc(GetMouseLineFunc func)
{
   mGetMouseLineFunc = func;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::ResetObjectToScreenFunc(void)
{
   mObjectToScreenFunc = dtUtil::MakeFunctor(
      &ToolObjectMotionModel::DefaultGetObjectScreenCoordinates, this);
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::SetObjectToScreenFunc(ObjectToScreenFunc func)
{
   mObjectToScreenFunc = func;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
{
   mGetMouseLineFunc(mousePos, start, end);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 ToolObjectMotionModel::GetObjectScreenCoordinates(const osg::Vec3& objectPos)
{
   return mObjectToScreenFunc(objectPos);
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::DefaultGetMouseLineFunc(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
{
   ObjectMotionModel::GetMouseLine(mousePos, start, end);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 ToolObjectMotionModel::DefaultGetObjectScreenCoordinates(osg::Vec3 objectPos)
{
   return ObjectMotionModel::GetObjectScreenCoordinates(objectPos);
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnTranslate(const osg::Vec3& delta)
{
   dtCore::Transformable* target = GetTarget();

   if (target && mPointIndex >= 0 && mActiveActor)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);

      mActiveActor->SetPointPosition(mPointIndex, transform.GetTranslation() + delta);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::OnRotate(float delta, const osg::Vec3& axis)
{
   dtCore::Transformable* target = GetTarget();

   if (target && mPointIndex >= 0 && mActiveActor)
   {
      osg::Matrix matrix;
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.GetRotation(matrix);
      matrix *= matrix.rotate(delta, axis);
      transform.SetRotation(matrix);

      osg::Vec3 hpr;
      transform.GetRotation(hpr);

      mActiveActor->SetPointRotation(mPointIndex, hpr);
   }
}

////////////////////////////////////////////////////////////////////////////////
