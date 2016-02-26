// stageobjectmotionmodel.cpp: Implementation of the STAGEObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/stageprefix.h>
#include <dtEditQt/stageobjectmotionmodel.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/transformableactorproxy.h>
#include <dtCore/vectoractorproperties.h>

#include <osg/Math>
#include <osg/Vec3>
#include <osg/Matrix>


namespace dtEditQt {


IMPLEMENT_MANAGEMENT_LAYER(STAGEObjectMotionModel)

////////////////////////////////////////////////////////////////////////////////
STAGEObjectMotionModel::STAGEObjectMotionModel(dtCore::View* view)
   : dtCore::ObjectMotionModel(view)
{
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
//void STAGEObjectMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

//{
//   // This function is overwritten to do nothing.
//}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::AddTarget(dtCore::TransformableActorProxy* target)
{
   // Skip if there is no target to add.
   if (!target)
   {
      return;
   }

   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtCore::TransformableActorProxy* targetProxy = GetTarget(subIndex);

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
      dtCore::TransformableActorProxy* targetProxy = GetTarget(0);

      if (targetProxy)
      {
         dtCore::Transformable* target = NULL;
         targetProxy->GetDrawable(target);
         ObjectMotionModel::SetTarget(target);
      }
   }
   else
   {
      ObjectMotionModel::SetTarget(NULL);
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::RemoveTarget(dtCore::TransformableActorProxy* target)
{
   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtCore::TransformableActorProxy* targetProxy = GetTarget(subIndex);

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
      dtCore::TransformableActorProxy* targetProxy = GetTarget(0);

      if (targetProxy)
      {
         dtCore::Transformable* target = NULL;
         targetProxy->GetDrawable(target);
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
void STAGEObjectMotionModel::OnTranslate(const osg::Vec3& delta)
{
   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtCore::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      if (targetProxy)
      {
         osg::Vec3 pos = targetProxy->GetTranslation();
         pos += delta;
         targetProxy->SetTranslation(pos);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnRotate(float delta, const osg::Vec3& axis)
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
      dtCore::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      if (targetProxy)
      {
         dtCore::Transformable* target = NULL;
         targetProxy->GetDrawable(target);

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

            transform.SetTranslation(newPos);
            transform.SetRotation(matrix);
            target->SetTransform(transform);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void STAGEObjectMotionModel::OnScale(const osg::Vec3& delta)
{
   for (int subIndex = 0; subIndex < GetNumTargets(); subIndex++)
   {
      dtCore::TransformableActorProxy* targetProxy = GetTarget(subIndex);

      if (targetProxy)
      {
         dtCore::ActorProperty* prop = targetProxy->GetProperty("Scale");
         dtCore::Vec3ActorProperty* scaleProp = dynamic_cast<dtCore::Vec3ActorProperty*>(prop);

         if (scaleProp)
         {
            osg::Vec3 oldScale = scaleProp->GetValue();
            osg::Vec3 newScale = oldScale + delta;

            // It causes problems when the scale is negative.
            if (newScale.x() < 0.01f)
            {
               newScale.x() = 0.01f;
            }
            if (newScale.y() < 0.01f)
            {
               newScale.y() = 0.01f;
            }
            if (newScale.z() < 0.01f)
            {
               newScale.z() = 0.01f;
            }

            scaleProp->SetValue(newScale);
         }
      }
   }
}

}//namespace dtEditQt

////////////////////////////////////////////////////////////////////////////////
