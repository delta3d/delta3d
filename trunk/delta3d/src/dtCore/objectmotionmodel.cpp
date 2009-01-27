// objectmotionmodel.cpp: Implementation of the ObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/objectmotionmodel.h>

#include <osg/Math>
#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/PolygonMode>
#include <osg/Depth>
#include <osg/BlendFunc>

#include <dtCore/system.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/keyboard.h>
#include <dtUtil/bits.h>
#include <dtCore/deltawin.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(ObjectMotionModel)

const int ARROW_NODE_MASK = 0x0f000000;
const float SENSITIVITY = 5.0f;

//////////////////////////////////////////////////////////////////////////
ObjectMotionModel::ObjectMotionModel(dtCore::View* view)
   : MotionModel("ObjectMotionModel")
   , mView(NULL)
   , mKeyboard(NULL)
   , mMouse(NULL)
   , mScale(1.0f)
   , mClearNode(NULL)
   , mCoordinateSpace(LOCAL_SPACE)
   , mMotionType(MOTION_TYPE_TRANSLATION)
   , mHoverArrow(ARROW_TYPE_MAX)
   , mCurrentArrow(ARROW_TYPE_MAX)
   , mMouseDown(false)
   , mMouseLocked(false)
{
   RegisterInstance(this);

   SetView(view);

   // Create our three axis.
   InitArrows();

   AddSender(&System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::~ObjectMotionModel()
{
   if (mScene)
   {
      for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
      {
         mScene->RemoveDrawable(mArrows[arrowIndex].transformable.get());
      }
   }

   RemoveSender(&System::GetInstance());

   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetView(dtCore::View* view)
{
   mView = view;

   if (mView)
   {
      mScene = mView->GetScene();
      mKeyboard = mView->GetKeyboard();
      mMouse = mView->GetMouse();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetEnabled(bool enabled)
{
   if (IsEnabled() != enabled)
   {
      MotionModel::SetEnabled(enabled);

      // Make sure our arrows are in the scene.
      if (mScene)
      {
         if (IsEnabled())
         {
            mScene->AddDrawable(mTargetTransform.get());
         }
         else
         {
            mScene->RemoveDrawable(mTargetTransform.get());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetScale(float scale)
{
   mScale = scale;
}

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::MotionType ObjectMotionModel::GetMotionType(void)
{
   return mMotionType;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetMotionType(ObjectMotionModel::MotionType motionType)
{
   mMotionType = motionType;

   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      mArrows[arrowIndex].arrowGeode->removeDrawable(mArrows[arrowIndex].arrowCylinder.get());
      mArrows[arrowIndex].arrowGeode->removeDrawable(mArrows[arrowIndex].arrowCone.get());
      mArrows[arrowIndex].arrowGeode->removeDrawable(mArrows[arrowIndex].rotationRing.get());
   }

   if (mMotionType == MOTION_TYPE_ROTATION)
   {
      for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
      {
         mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].rotationRing.get());
      }
   }
   else
   {
      for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
      {
         mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCylinder.get());
         mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCone.get());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::CoordinateSpace ObjectMotionModel::GetCoordinateSpace(void)
{
   return mCoordinateSpace;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetCoordinateSpace(CoordinateSpace coordinateSpace)
{
   mCoordinateSpace = coordinateSpace;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnMessage(MessageData *data)
{
   if (GetTarget() && IsEnabled() && mTargetTransform.valid())
   {
      // Update the position of the arrows to the position of the target.
      {
         dtCore::Transformable* target = GetTarget();
         dtCore::Transform transform;
         osg::Vec3 position;

         if (mCoordinateSpace == LOCAL_SPACE)
         {
            target->GetTransform(transform);
            position = transform.GetTranslation();
         }
         else
         {
            // We only need the targets position for a world space setting.
            dtCore::Transform targetTransform;
            target->GetTransform(targetTransform);
            position = targetTransform.GetTranslation();
         }

         transform.Rescale(osg::Vec3(mScale, mScale, mScale));
         transform.SetTranslation(position);

        mTargetTransform->SetTransform(transform);
      }

      // Update mouse input events.
      {
         // Check for mouse press.
         bool bLeftMouse = mMouse->GetButtonState(dtCore::Mouse::LeftButton);
         bool bRightMouse= mMouse->GetButtonState(dtCore::Mouse::RightButton);

         // When the mouse is released, deselect our current arrow.
         if (!bLeftMouse && !bRightMouse)
         {
            mCurrentArrow  = ARROW_TYPE_MAX;
            mMouseDown     = false;
            mMouseLocked   = false;

            // Test for arrow hovering.
            if (mView && !mMouseDown)
            {
               dtCore::DeltaDrawable* arrow = mView->GetMousePickedObject(ARROW_NODE_MASK);

               mHoverArrow = ARROW_TYPE_MAX;

               if (arrow)
               {
                  for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
                  {
                     if (arrow == mArrows[arrowIndex].transformable.get())
                     {
                        mHoverArrow = (ArrowType)arrowIndex;
                        break;
                     }
                  }
               }

               SetArrowHighlight(mHoverArrow);
            }
         }
         else
         {
            // First mouse click.
            if (!mMouseDown)
            {
               mMouseDown = true;

               // Do a collision test to see if the mouse collides with any of
               // the motion arrows.
               if (mView)
               {
                  dtCore::DeltaDrawable* arrow = mView->GetMousePickedObject(ARROW_NODE_MASK);

                  if (arrow)
                  {
                     for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
                     {
                        // If we clicked on a motion arrow, lock it to the mouse.
                        if (arrow == mArrows[arrowIndex].transformable.get())
                        {
                           mCurrentArrow = (ArrowType)arrowIndex;
                           mMouseLocked = true;

                           // Get the offset mouse position.
                           dtCore::Transformable* target = GetTarget();
                           if (target)
                           {
                              dtCore::Transform transform;
                              target->GetTransform(transform);
                              osg::Vec2 objectPos = ObjectToScreenCoords(transform.GetTranslation());
                              mMouseOrigin = mMouse->GetPosition();
                              mMouseOffset = objectPos - mMouseOrigin;
                           }

                           break;
                        }
                     }
                  }
               }
            }
            // If we currently have a motion arrow locked to the mouse.
            else if (mMouseLocked)
            {
               switch (mMotionType)
               {
               case MOTION_TYPE_TRANSLATION:
                  UpdateTranslation();
                  break;

               case MOTION_TYPE_ROTATION:
                  UpdateRotation();
                  break;

               case MOTION_TYPE_SCALE:
                  UpdateScale();
                  break;
               }
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::InitArrows(void)
{
   mTargetTransform = new dtCore::Transformable();
   osg::Group* groupNode = mTargetTransform->GetOSGNode()->asGroup();
   if (groupNode)
   {
      // Make this group render top most.
      osg::StateSet* stateSet = groupNode->getOrCreateStateSet();
      if (stateSet)
      {
         //stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
         stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
         stateSet->setRenderBinDetails(98, "RenderBin");

         //osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
         //stateSet->setAttribute(polygonMode, osg::StateAttribute::ON);

         osg::Depth* depth = new osg::Depth(osg::Depth::ALWAYS);
         stateSet->setAttribute(depth, osg::StateAttribute::ON);

         osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
         stateSet->setAttribute(blend, osg::StateAttribute::ON);
      }

      //mClearNode = new osg::ClearNode();
      //if (mClearNode)
      //{
      //   mClearNode->setClearMask(GL_DEPTH_BUFFER_BIT);
      //   mClearNode->setRequiresClear(true);

      //   groupNode->addChild(mClearNode);

      //   //// Make this group render top most.
      //   //osg::StateSet* stateSet = mClearNode->getOrCreateStateSet();
      //   //if (stateSet)
      //   //{
      //   //   //stateSet->setRenderBinDetails(99, "DepthRenderBin", osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);

      //   //}
      //}
   }

   osg::Group* wireNode = new osg::Group();
   if (wireNode)
   {
      // Make this group render top most.
      osg::StateSet* stateSet = wireNode->getOrCreateStateSet();
      if (stateSet)
      {
         //stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
         stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
         stateSet->setRenderBinDetails(99, "RenderBin");

         //osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
         //stateSet->setAttribute(polygonMode, osg::StateAttribute::ON);

         osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL);
         stateSet->setAttribute(depth, osg::StateAttribute::ON);

         osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
         stateSet->setAttribute(blend, osg::StateAttribute::ON);
      }

      if (groupNode)
      {
         groupNode->addChild(wireNode);
      }
   }

   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      // Create all our objects and nodes.
      mArrows[arrowIndex].transformable   = new dtCore::Transformable();
      mArrows[arrowIndex].arrowGeode      = new osg::Geode();

      osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.06f), 0.01f, 0.1f);
      osg::Cone*     cone     = new osg::Cone(osg::Vec3(0.0f, 0.0f, 0.115f), 0.013f, 0.03f);

      osg::Cylinder* ring     = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 0.1f, 0.005f);

      mArrows[arrowIndex].arrowCylinder = new osg::ShapeDrawable(cylinder);
      mArrows[arrowIndex].arrowCone     = new osg::ShapeDrawable(cone);

      mArrows[arrowIndex].rotationRing  = new osg::ShapeDrawable(ring);

      // Now set up their Hierarchy.
      if (mClearNode)
      {
         mClearNode->addChild(mArrows[arrowIndex].transformable->GetOSGNode());
      }
      else
      {
         mTargetTransform->AddChild(mArrows[arrowIndex].transformable.get());
      }

      if (wireNode)
      {
         wireNode->addChild(mArrows[arrowIndex].transformable->GetOSGNode());
      }

      osg::Group* arrowGroup = mArrows[arrowIndex].transformable->GetOSGNode()->asGroup();
      if (arrowGroup)
      {
         arrowGroup->addChild(mArrows[arrowIndex].arrowGeode.get());
      }

      if (mMotionType == MOTION_TYPE_ROTATION)
      {
         mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].rotationRing.get());
      }
      else
      {
         mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCylinder.get());
         mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCone.get());
      }

      mArrows[arrowIndex].arrowGeode->setNodeMask(ARROW_NODE_MASK);
   }

   if (IsEnabled() && mScene)
   {
      mScene->AddDrawable(mTargetTransform.get());
   }

   // Now position and rotate our axes.
   dtCore::Transform transformX;
   transformX.SetTranslation(0.0f, 0.0f, 0.0f);
   transformX.Set(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
   mArrows[ARROW_TYPE_RIGHT].transformable->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].arrowCylinderColor = osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f);
   mArrows[ARROW_TYPE_RIGHT].arrowConeColor = osg::Vec4(1.0f, 0.3f, 0.3f, 0.5f);

   dtCore::Transform transformY;
   transformY.SetTranslation(0.0f, 0.0f, 0.0f);
   transformY.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
   mArrows[ARROW_TYPE_AT].transformable->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].arrowCylinderColor = osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f);
   mArrows[ARROW_TYPE_AT].arrowConeColor = osg::Vec4(0.3f, 1.0f, 0.3f, 0.5f);

   dtCore::Transform transformZ;
   transformZ.SetTranslation(0.0f, 0.0f, 0.0f);
   mArrows[ARROW_TYPE_UP].transformable->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].arrowCylinderColor = osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f);
   mArrows[ARROW_TYPE_UP].arrowConeColor = osg::Vec4(0.3f, 0.3f, 1.0f, 0.5f);
}


////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ObjectMotionModel::GetMouseVector(osg::Vec2 mousePos)
{
   const osg::Camera* camera = mView->GetCamera()->GetOSGCamera();
   osg::Matrix matrix = camera->getViewMatrix() * camera->getProjectionMatrix();

   const osg::Matrix inverse = osg::Matrix::inverse(matrix);
   osg::Vec3 startVertex = osg::Vec3(mousePos.x(), mousePos.y(), 0.0f) * inverse;

   dtCore::Transform transform;
   mView->GetCamera()->GetTransform(transform);
   return startVertex - transform.GetTranslation();
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 ObjectMotionModel::ObjectToScreenCoords(osg::Vec3 objectPosition)
{
   const osg::Camera* camera = mView->GetCamera()->GetOSGCamera();
   osg::Matrix matrix = camera->getViewMatrix() * camera->getProjectionMatrix();

   osg::Vec3 screenPos = objectPosition * matrix;
   return osg::Vec2(screenPos.x(), screenPos.y());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetArrowHighlight(ArrowType arrowType)
{
   mHoverArrow = arrowType;

   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      float alpha = 0.5f;
      if (arrowType == (ArrowType)arrowIndex)
      {
         alpha = 1.0f;
      }

      osg::Vec4 color = mArrows[arrowIndex].arrowCylinderColor;
      color.r() = color.r() * alpha;
      color.g() = color.g() * alpha;
      color.b() = color.b() * alpha;
      color.a() = color.a() * alpha;

      if (mMotionType == MOTION_TYPE_ROTATION)
      {
         mArrows[arrowIndex].rotationRing->setColor(color);
      }
      else
      {
         mArrows[arrowIndex].arrowCylinder->setColor(color);

         color = mArrows[arrowIndex].arrowConeColor;
         color.r() = color.r() * alpha;
         color.g() = color.g() * alpha;
         color.b() = color.b() * alpha;
         color.a() = color.a() * alpha;
         mArrows[arrowIndex].arrowCone->setColor(color);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateTranslation(void)
{
   osg::Vec3 plane1;
   osg::Vec3 plane2;
   osg::Vec3 plane3;
   osg::Vec3 axis;
   osg::Vec3* plane = NULL;

   osg::Vec3 targetUp;
   osg::Vec3 targetRight;
   osg::Vec3 targetAt;
   osg::Vec3 targetPos;

   osg::Vec3 camUp;
   osg::Vec3 camRight;
   osg::Vec3 camAt;
   osg::Vec3 camPos;

   dtCore::Transformable* target = GetTarget();
   if (!target)
   {
      return;
   }

   dtCore::Camera* camera = NULL;
   if (mView)
   {
      camera = mView->GetCamera();
   }

   if (!camera)
   {
      return;
   }

   dtCore::Transform camTransform;
   camera->GetTransform(camTransform);
   camTransform.GetOrientation(camRight, camUp, camAt);
   camPos = camTransform.GetTranslation();

   if (mCoordinateSpace == LOCAL_SPACE)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.GetOrientation(targetRight, targetUp, targetAt);
      targetPos = transform.GetTranslation();
   }
   else // World Space
   {
      dtCore::Transform transform;
      transform.GetOrientation(targetRight, targetUp, targetAt);
      target->GetTransform(transform);
      targetPos = transform.GetTranslation();
   }

   switch (mCurrentArrow)
   {
   case ARROW_TYPE_AT:
      {
         axis = targetAt;
         plane1 = axis ^ camUp;
         plane2 = axis ^ camRight;
         plane3 = axis ^ camAt;
         axis.normalize();
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis = targetRight;
         plane1 = axis ^ camUp;
         plane2 = axis ^ camRight;
         plane3 = axis ^ camAt;
         axis.normalize();
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis = targetUp;
         plane1 = axis ^ camUp;
         plane2 = axis ^ camRight;
         plane3 = axis ^ camAt;
         axis.normalize();
         break;
      }
   default:
      {
         return;
      }
   }

   // Find the best plane.
   float fBest;
   float fTest;

   if (fBest = fabs(camAt * plane1) > osg::DegreesToRadians(5.0f))
   {
      plane = &plane1;
   }

   fTest = fabs(camAt * plane2);
   if (fTest > fBest)
   {
      fBest = fTest;
      if (fBest > osg::DegreesToRadians(5.0f))
      {
         plane = &plane2;
      }
   }

   fTest = fabs(camAt * plane3);
   if (fTest > fBest)
   {
      fBest = fTest;
      if (fBest > osg::DegreesToRadians(5.0f))
      {
         plane = &plane3;
      }
   }

   if (plane)
   {
      // Get the mouse vector.
      osg::Vec3 mouse = GetMouseVector(mMouse->GetPosition() + mMouseOffset);

      // Calculate the mouse collision in the 3D space relative to the plane
      // of the camera and the desired axis of the object.
      float fStartOffset   = camPos *     (*plane);
      float fDistMod       = mouse *      (*plane);
      float fPlaneOffset   = targetPos *  (*plane);

      float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

      // Find the projected point of collision on the plane.
      osg::Vec3 projection = (mouse * fDistance) + camPos;
      osg::Vec3 vector     = projection - targetPos;

      // Find the translation vector.
      fDistance = axis * vector;

      targetPos += axis * fDistance;

      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.SetTranslation(targetPos);
      target->SetTransform(transform);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateRotation(void)
{
   dtCore::Transformable* target = GetTarget();
   if (!target)
   {
      return;
   }

   osg::Vec2 mousePos = mMouse->GetPosition();

   float rotationX = (mMouseOrigin.x() - mousePos.x()) * SENSITIVITY;
   float rotationY = (mMouseOrigin.y() - mousePos.y()) * SENSITIVITY;

   mMouseOrigin = mousePos;

   osg::Vec3 camUp;
   osg::Vec3 camRight;
   osg::Vec3 camAt;

   dtCore::Camera* camera = NULL;
   if (mView)
   {
      camera = mView->GetCamera();
   }

   if (!camera)
   {
      return;
   }

   dtCore::Transform camTransform;
   camera->GetTransform(camTransform);
   camTransform.GetOrientation(camRight, camUp, camAt);

   osg::Vec3 targetUp;
   osg::Vec3 targetRight;
   osg::Vec3 targetAt;
   dtCore::Transform transform;

   if (mCoordinateSpace == LOCAL_SPACE)
   {
      target->GetTransform(transform);
   }

   transform.GetOrientation(targetRight, targetUp, targetAt);

   osg::Vec3 axis;

   switch (mCurrentArrow)
   {
   case ARROW_TYPE_AT:
      {
         axis = targetAt;
         axis.normalize();
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis = targetRight;
         axis.normalize();
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis = targetUp;
         axis.normalize();
         break;
      }
   default:
      {
         return;
      }
   }

   //float angle = rotationX;
   float angle = 0.0f;
   if (camUp * axis > 0.0f)
   {
      angle = rotationX;
   }
   else
   {
      angle = -rotationX;
   }

   if (camRight * axis > 0.0f)
   {
      angle += rotationY;
   }
   else
   {
      angle -= rotationY;
   }

   if (angle != 0)
   {
      osg::Matrix matrix;
      target->GetTransform(transform);
      transform.GetRotation(matrix);
      matrix *= matrix.rotate(angle, axis);
      transform.SetRotation(matrix);
      target->SetTransform(transform);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateScale(void)
{
}

////////////////////////////////////////////////////////////////////////////////
