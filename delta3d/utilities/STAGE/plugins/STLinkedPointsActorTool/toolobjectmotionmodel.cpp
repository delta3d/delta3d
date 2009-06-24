// stageobjectmotionmodel.cpp: Implementation of the ToolObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include "toolobjectmotionmodel.h"

#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>

#include <osg/Math>
#include <osg/Vec3>
#include <osg/Matrix>

IMPLEMENT_MANAGEMENT_LAYER(ToolObjectMotionModel)

////////////////////////////////////////////////////////////////////////////////
ToolObjectMotionModel::ToolObjectMotionModel(dtCore::View* view)
   : dtCore::ObjectMotionModel(view)
{
   ResetGetMouseLineFunc();
   ResetObjectToScreenFunc();
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
void ToolObjectMotionModel::OnMessage(MessageData *data)
{
   // This function is overritten to do nothing.
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::ResetGetMouseLineFunc(void)
{
   mGetMouseLineFunc = dtDAL::MakeFunctor(
      *this, &ToolObjectMotionModel::DefaultGetMouseLineFunc);
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::SetGetMouseLineFunc(GetMouseLineFunc func)
{
   mGetMouseLineFunc = func;
}

////////////////////////////////////////////////////////////////////////////////
void ToolObjectMotionModel::ResetObjectToScreenFunc(void)
{
   mObjectToScreenFunc = dtDAL::MakeFunctorRet(
      *this, &ToolObjectMotionModel::DefaultGetObjectScreenCoordinates);
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
osg::Vec2 ToolObjectMotionModel::GetObjectScreenCoordinates(osg::Vec3 objectPos)
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
