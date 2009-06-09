// stageobjectmotionmodel.cpp: Implementation of the STAGEObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtEditQt/stageobjectmotionmodel.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>

#include <osg/Math>
#include <osg/Vec3>
#include <osg/Matrix>

IMPLEMENT_MANAGEMENT_LAYER(STAGEObjectMotionModel)

////////////////////////////////////////////////////////////////////////////////
STAGEObjectMotionModel::STAGEObjectMotionModel(dtCore::View* view)
   : dtCore::ObjectMotionModel(view)
{
   ResetGetMouseLineFunc();
   ResetObjectToScreenFunc();
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnLeftMousePressed(void)
{
   mLeftMouse = true;
   mSnap = true;
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnLeftMouseReleased(void)
{
   mLeftMouse = false;
   mSnap = false;
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnRightMousePressed(void)
{
   mRightMouse = true;
   mSnap = true;
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnRightMouseReleased(void)
{
   mRightMouse = false;
   mSnap = false;
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnMessage(MessageData *data)
{
   // This function is overritten to do nothing.
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::ResetGetMouseLineFunc(void)
{
   mGetMouseLineFunc = dtDAL::MakeFunctor(
      *this, &STAGEObjectMotionModel::DefaultGetMouseLineFunc);
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::SetGetMouseLineFunc(GetMouseLineFunc func)
{
   mGetMouseLineFunc = func;
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::ResetObjectToScreenFunc(void)
{
   mObjectToScreenFunc = dtDAL::MakeFunctorRet(
      *this, &STAGEObjectMotionModel::DefaultGetObjectScreenCoordinates);
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::SetObjectToScreenFunc(ObjectToScreenFunc func)
{
   mObjectToScreenFunc = func;
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::SetTarget(dtCore::Transformable* target)
{
   // This does nothing and shoule not be used.
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::AddTarget(dtDAL::TransformableActorProxy* target)
{
   // Skip if there is no target to add.
   if (!target)
   {
      return;
   }

   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      // Skip if the target is already added.
      if (target == targetProxy)
      {
         return;
      }
   }

   mTargets.push_back(target);

   // Make sure our primary target it set to the first target.
   if (mTargets.size() > 0)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(0);

      if (targetProxy)
      {
         dtCore::Transformable* target = NULL;
         targetProxy->GetActor(target);
         ObjectMotionModel::SetTarget(target);
      }
   }
   else
   {
      ObjectMotionModel::SetTarget(NULL);
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::RemoveTarget(dtDAL::TransformableActorProxy* target)
{
   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      // Once we find the target, remove it.
      if (target == targetProxy)
      {
         mTargets.erase(mTargets.begin() + subIndex);
         break;
      }
   }

   // Make sure our primary target it set to the first target.
   if (mTargets.size() > 0)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(0);

      if (targetProxy)
      {
         dtCore::Transformable* target = NULL;
         targetProxy->GetActor(target);
         ObjectMotionModel::SetTarget(target);
      }
   }
   else
   {
      ObjectMotionModel::SetTarget(NULL);
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::ClearTargets(void)
{
   mTargets.clear();
   ObjectMotionModel::SetTarget(NULL);
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
{
   mGetMouseLineFunc(mousePos, start, end);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 STAGEObjectMotionModel::GetObjectScreenCoordinates(osg::Vec3 objectPos)
{
   return mObjectToScreenFunc(objectPos);
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::DefaultGetMouseLineFunc(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
{
   ObjectMotionModel::GetMouseLine(mousePos, start, end);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 STAGEObjectMotionModel::DefaultGetObjectScreenCoordinates(osg::Vec3 objectPos)
{
   return ObjectMotionModel::GetObjectScreenCoordinates(objectPos);
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnTranslate(osg::Vec3 delta)
{
   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      if (targetProxy)
      {
         osg::Vec3 pos = targetProxy->GetTranslation();
         pos += delta;
         targetProxy->SetTranslation(pos);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnRotate(float delta, osg::Vec3 axis)
{
   osg::Vec3 center;
   dtCore::Transformable* target = ObjectMotionModel::GetTarget();
   if (target)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);
      center = transform.GetTranslation();
   }

   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      if (targetProxy)
      {
         dtCore::Transformable* target = NULL;
         targetProxy->GetActor(target);

         if (target)
         {
            osg::Matrix rotation;
            osg::Matrix matrix;
            dtCore::Transform transform;
            target->GetTransform(transform);
            transform.GetRotation(matrix);
            osg::Vec3 targetCenter = transform.GetTranslation();
            osg::Vec3 offset = targetCenter - center;

            rotation = matrix.rotate(delta, axis);
            matrix *= rotation;
            osg::Vec3 newPos = (offset * rotation) + center;

            targetProxy->SetRotationFromMatrix(matrix);
            targetProxy->SetTranslation(newPos);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnScale(osg::Vec3 delta)
{
   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtDAL::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      if (targetProxy)
      {
         dtDAL::ActorProperty* prop = targetProxy->GetProperty("Scale");
         dtDAL::Vec3ActorProperty* scaleProp = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);

         if (scaleProp)
         {
            osg::Vec3 oldScale = scaleProp->GetValue();
            osg::Vec3 newScale = oldScale + delta;

            // It causes problems when the scale is negative.
            if (newScale.x() < 0.0f)
            {
               newScale.x() = 0.0f;
            }
            if (newScale.y() < 0.0f)
            {
               newScale.y() = 0.0f;
            }
            if (newScale.z() < 0.0f)
            {
               newScale.z() = 0.0f;
            }

            scaleProp->SetValue(newScale);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
