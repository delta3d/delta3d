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
#include <dtCore/transform.h>
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
   , mScene(NULL)
   , mKeyboard(NULL)
   , mMouse(NULL)
   , mScale(1.0f)
   , mCoordinateSpace(LOCAL_SPACE)
   , mMotionType(MOTION_TYPE_MAX)
   , mHoverArrow(ARROW_TYPE_MAX)
   , mCurrentArrow(ARROW_TYPE_MAX)
   , mMouseDown(false)
   , mMouseLocked(false)
   , mOriginAngle(0.0f)
   , mAutoScale(true)
{
   mSnapRotation = osg::DegreesToRadians(45.0f);
   mSnapTranslation = 1;

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
         mScene->RemoveDrawable(mArrows[arrowIndex].translationTransform.get());
         mScene->RemoveDrawable(mArrows[arrowIndex].rotationTransform.get());
      }
   }

   RemoveSender(&System::GetInstance());

   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetView(dtCore::View* view)
{
   // First remove this model from the current view scene.
   bool bWasEnabled = false;
   if (IsEnabled())
   {
      bWasEnabled = true;
      SetEnabled(false);
   }

   mView = view;

   if (mView)
   {
      mScene = mView->GetScene();
      mKeyboard = mView->GetKeyboard();
      mMouse = mView->GetMouse();

      if (bWasEnabled)
      {
         SetEnabled(true);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetEnabled(bool enabled)
{
   if (IsEnabled() != enabled)
   {
      MotionModel::SetEnabled(enabled);

      // Make sure our arrows are in the scene.
      if (mScene && mTargetTransform.valid())
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
float ObjectMotionModel::GetAutoScaleSize(void)
{
   dtCore::Camera* camera = NULL;
   if (mView)
   {
      camera = mView->GetCamera();
   }

   if (!camera)
   {
      return GetScale();
   }

   dtCore::Transform camTransform;
   camera->GetTransform(camTransform);
   osg::Vec3 camPos = camTransform.GetTranslation();

   dtCore::Transformable* target = GetTarget();
   if (!target)
   {
      return GetScale();
   }

   dtCore::Transform targetTransform;
   target->GetTransform(targetTransform);

   osg::Vec3 VecToTarget = targetTransform.GetTranslation() - camPos;
   return GetScale() * VecToTarget.length();
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
void ObjectMotionModel::SetSnapTranslation(int increment)
{
   mSnapTranslation = increment;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetSnapRotation(float degrees)
{
   mSnapRotation = osg::DegreesToRadians(degrees);
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

         float scale = GetAutoScaleSize();
         transform.Rescale(osg::Vec3(scale, scale, scale));
         transform.SetTranslation(position);

        mTargetTransform->SetTransform(transform);
      }

      // Update mouse input events.
      {
         // Check for mouse press.
         bool bLeftMouse = mMouse->GetButtonState(dtCore::Mouse::LeftButton);
         bool bRightMouse= mMouse->GetButtonState(dtCore::Mouse::RightButton);
         mSnap = bRightMouse;

         // When the mouse is released, deselect our current arrow.
         if (!bLeftMouse && !bRightMouse)
         {
            if (mMouseLocked)
            {
               mAngleGeode->removeDrawable(mAngleDrawable.get());
               mAngleOriginGeode->removeDrawable(mAngleOriginDrawable.get());
            }

            mCurrentArrow  = ARROW_TYPE_MAX;
            mMouseDown     = false;
            mMouseLocked   = false;

            // Test for arrow hovering.
            if (mView && !mMouseDown)
            {
               dtCore::DeltaDrawable* arrow = mView->GetMousePickedObject(ARROW_NODE_MASK);

               mHoverArrow = ARROW_TYPE_MAX;
               mMotionType = MOTION_TYPE_MAX;

               if (arrow)
               {
                  for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
                  {
                     if (arrow == mArrows[arrowIndex].translationTransform.get())
                     {
                        mHoverArrow = (ArrowType)arrowIndex;
                        mMotionType = MOTION_TYPE_TRANSLATION;
                        break;
                     }
                     else if (arrow == mArrows[arrowIndex].rotationTransform.get())
                     {
                        mHoverArrow = (ArrowType)arrowIndex;
                        mMotionType = MOTION_TYPE_ROTATION;
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
                        if (arrow == mArrows[arrowIndex].translationTransform.get() ||
                           arrow == mArrows[arrowIndex].rotationTransform.get())
                        {
                           mCurrentArrow = (ArrowType)arrowIndex;
                           mMouseLocked = true;

                           if (arrow == mArrows[arrowIndex].translationTransform.get())
                           {
                              mMotionType = MOTION_TYPE_TRANSLATION;
                           }
                           else if (arrow == mArrows[arrowIndex].rotationTransform.get())
                           {
                              mMotionType = MOTION_TYPE_ROTATION;
                           }

                           // Get the offset mouse position.
                           dtCore::Transformable* target = GetTarget();
                           if (target)
                           {
                              dtCore::Transform transform;
                              target->GetTransform(transform);
                              osg::Vec2 objectPos = ObjectToScreenCoords(transform.GetTranslation());
                              mMouseOrigin = mMouse->GetPosition();
                              mMouseOffset = objectPos - mMouseOrigin;
                              mOriginAngle = 0.0f;
                           }

                           SetArrowHighlight(mCurrentArrow);

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
         stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
         stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
         stateSet->setRenderBinDetails(98, "RenderBin");

         //osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
         //stateSet->setAttribute(polygonMode, osg::StateAttribute::ON);

         osg::Depth* depth = new osg::Depth(osg::Depth::ALWAYS);
         stateSet->setAttribute(depth, osg::StateAttribute::ON);

         osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
         stateSet->setAttribute(blend, osg::StateAttribute::ON);
      }
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
      }

      if (groupNode)
      {
         groupNode->addChild(wireNode);
      }
   }

   float ringLength    = 0.09f;
   float ringVisibleThickness = 0.002f;
   float ringSelectionThickness = 0.02f;
   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      // Create all our objects and nodes.
      mArrows[arrowIndex].translationTransform   = new dtCore::Transformable();
      mArrows[arrowIndex].rotationTransform      = new dtCore::Transformable();
      mArrows[arrowIndex].arrowGeode             = new osg::Geode();
      mArrows[arrowIndex].rotationGeode          = new osg::Geode();
      mArrows[arrowIndex].rotationSelectionGeode = new osg::Geode();

      osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.06f), 0.01f, 0.1f);
      osg::Cone*     cone     = new osg::Cone(osg::Vec3(0.0f, 0.0f, 0.115f), 0.013f, 0.03f);

//      osg::Cylinder* ring     = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 0.07f, 0.001f);
      osg::TriangleMesh* ring = GenerateRing(ringLength - ringVisibleThickness, ringLength + ringVisibleThickness, 40);
      osg::TriangleMesh* selectionRing = GenerateRing(ringLength - ringSelectionThickness, ringLength + ringSelectionThickness, 40);

      mArrows[arrowIndex].arrowCylinder = new osg::ShapeDrawable(cylinder);
      mArrows[arrowIndex].arrowCone     = new osg::ShapeDrawable(cone);

      mArrows[arrowIndex].rotationRing  = new osg::ShapeDrawable(ring);
      osg::ShapeDrawable* rotationSelectionRing = new osg::ShapeDrawable(selectionRing);
      rotationSelectionRing->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

      // Now set up their Hierarchy.
      mTargetTransform->AddChild(mArrows[arrowIndex].translationTransform.get());
      mTargetTransform->AddChild(mArrows[arrowIndex].rotationTransform.get());

      if (wireNode)
      {
         wireNode->addChild(mArrows[arrowIndex].translationTransform->GetOSGNode());
         wireNode->addChild(mArrows[arrowIndex].rotationTransform->GetOSGNode());
      }

      osg::Group* arrowGroup = mArrows[arrowIndex].translationTransform->GetOSGNode()->asGroup();
      if (arrowGroup)
      {
         arrowGroup->addChild(mArrows[arrowIndex].arrowGeode.get());
      }

      osg::Group* rotationGroup = mArrows[arrowIndex].rotationTransform->GetOSGNode()->asGroup();
      if (rotationGroup)
      {
         rotationGroup->addChild(mArrows[arrowIndex].rotationGeode.get());
         rotationGroup->addChild(mArrows[arrowIndex].rotationSelectionGeode.get());
      }

      mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCylinder.get());
      mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCone.get());

      mArrows[arrowIndex].arrowGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].rotationGeode->addDrawable(mArrows[arrowIndex].rotationRing.get());
      mArrows[arrowIndex].rotationGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].rotationSelectionGeode->addDrawable(rotationSelectionRing);
      mArrows[arrowIndex].rotationSelectionGeode->setNodeMask(ARROW_NODE_MASK);
   }

   mAngleTransform = new dtCore::Transformable();
   mAngleGeode = new osg::Geode();
   mAngleCylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, ringLength * 0.5f), 0.001f, ringLength);
   mAngleDrawable = new osg::ShapeDrawable(mAngleCylinder.get());

   mAngleTransform->GetOSGNode()->asGroup()->addChild(mAngleGeode.get());
   mTargetTransform->AddChild(mAngleTransform.get());

   mAngleOriginTransform = new dtCore::Transformable();
   mAngleOriginGeode = new osg::Geode();
   mAngleOriginCylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, ringLength * 0.5f), 0.001f, ringLength);
   mAngleOriginDrawable = new osg::ShapeDrawable(mAngleOriginCylinder.get());

   mAngleOriginTransform->GetOSGNode()->asGroup()->addChild(mAngleOriginGeode.get());
   mTargetTransform->AddChild(mAngleOriginTransform.get());

   if (IsEnabled() && mScene)
   {
      mScene->AddDrawable(mTargetTransform.get());
   }

   // Now position and rotate our axes.
   dtCore::Transform transformX;
   transformX.SetTranslation(0.0f, 0.0f, 0.0f);
   transformX.Set(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
   mArrows[ARROW_TYPE_RIGHT].translationTransform->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].rotationTransform->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].arrowCylinderColor = osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f);
   mArrows[ARROW_TYPE_RIGHT].arrowConeColor = osg::Vec4(1.0f, 0.3f, 0.3f, 0.5f);

   dtCore::Transform transformY;
   transformY.SetTranslation(0.0f, 0.0f, 0.0f);
   transformY.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
   mArrows[ARROW_TYPE_AT].translationTransform->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].rotationTransform->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].arrowCylinderColor = osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f);
   mArrows[ARROW_TYPE_AT].arrowConeColor = osg::Vec4(0.3f, 1.0f, 0.3f, 0.5f);

   dtCore::Transform transformZ;
   transformZ.SetTranslation(0.0f, 0.0f, 0.0f);
   mArrows[ARROW_TYPE_UP].translationTransform->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].rotationTransform->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].arrowCylinderColor = osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f);
   mArrows[ARROW_TYPE_UP].arrowConeColor = osg::Vec4(0.3f, 0.3f, 1.0f, 0.5f);
}


////////////////////////////////////////////////////////////////////////////////
osg::TriangleMesh* ObjectMotionModel::GenerateRing(float minRadius, float maxRadius, int segments)
{
   osg::Vec3Array* vertices = new osg::Vec3Array();
   osg::IntArray* indices   = new osg::IntArray();

   osg::TriangleMesh* mesh = new osg::TriangleMesh();

   // 4 segments minimum.
   segments = segments < 4? 4: segments;

   // Determine how many degrees each segment will be.
   float segmentAngle = osg::DegreesToRadians(360.0f) / segments;

   // Now iterate through each segment.
   int vertexIndex = 0;

   osg::Vec3 minVertex = osg::Vec3(minRadius, 0.0f, 0.0f);
   osg::Vec3 maxVertex = osg::Vec3(maxRadius, 0.0f, 0.0f);
   osg::Vec3 axis = osg::Vec3(0.0f, 0.0f, 1.0f);

   osg::Matrix rotationMatrix;
   for (int segmentIndex = 0; segmentIndex < segments; segmentIndex++)
   {
      rotationMatrix = rotationMatrix.rotate(segmentAngle * segmentIndex, axis);

      osg::Vec3 vert = minVertex * rotationMatrix;
      vertices->push_back(vert);
      indices->push_back((int)vertices->size()-1);

      // Once we get to our second segment, we need to start connecting
      // with our previous segments.
      if (segmentIndex > 0)
      {
         indices->push_back((int)vertices->size()-1);
         indices->push_back((int)vertices->size()-2);
      }

      vert = maxVertex * rotationMatrix;
      vertices->push_back(vert);
      indices->push_back((int)vertices->size()-1);

      // Once we get to our second segment, we need to start
      // our next triangle from the current two points.
      if (segmentIndex > 0)
      {
         indices->push_back((int)vertices->size()-2);
         indices->push_back((int)vertices->size()-1);
      }
   }

   // Now connect the ends of the strip together.
   indices->push_back(0);
   indices->push_back(0);
   indices->push_back((int)vertices->size()-1);
   indices->push_back(1);

   mesh->setVertices(vertices);
   mesh->setIndices(indices);

   return mesh;
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
      osg::Vec4 color = mArrows[arrowIndex].arrowCylinderColor;
      color.r() = color.r() * 0.5f;
      color.g() = color.g() * 0.5f;
      color.b() = color.b() * 0.5f;
      color.a() = color.a();

      mArrows[arrowIndex].rotationRing->setColor(color);
      //mAngleDrawable->setColor(color);
      //mAngleOriginDrawable->setColor(color);

      mArrows[arrowIndex].arrowCylinder->setColor(color);

      color = mArrows[arrowIndex].arrowConeColor;
      color.r() = color.r() * 0.5f;
      color.g() = color.g() * 0.5f;
      color.b() = color.b() * 0.5f;
      color.a() = color.a();
      mArrows[arrowIndex].arrowCone->setColor(color);

      osg::Vec4 highlightColor = mArrows[arrowIndex].arrowCylinderColor;

      if (arrowIndex == mHoverArrow)
      {
         if (mMotionType == MOTION_TYPE_ROTATION)
         {
            mArrows[arrowIndex].rotationRing->setColor(highlightColor);

            if (mCurrentArrow == (ArrowType)arrowIndex)
            {
               mAngleDrawable->setColor(highlightColor);
               mAngleOriginDrawable->setColor(highlightColor);
            }
         }
         else if (mMotionType == MOTION_TYPE_TRANSLATION)
         {
            mArrows[arrowIndex].arrowCylinder->setColor(highlightColor);
            mArrows[arrowIndex].arrowCone->setColor(mArrows[arrowIndex].arrowConeColor);
         }
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

      // Snap
      if (mSnap)
      {
         fDistance = (int)fDistance / mSnapTranslation;
      }

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
   osg::Vec3 camPos;

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

   osg::Vec3 targetUp;
   osg::Vec3 targetRight;
   osg::Vec3 targetAt;
   osg::Vec3 targetPos;
   dtCore::Transform targetTransform;
   dtCore::Transform transform;

   target->GetTransform(targetTransform);

   if (mCoordinateSpace == LOCAL_SPACE)
   {
      transform = targetTransform;
   }

   transform.GetOrientation(targetRight, targetUp, targetAt);
   targetPos = targetTransform.GetTranslation();

   osg::Vec3 axis;
   osg::Vec3 upAxis;
   osg::Vec3 rightAxis;

   switch (mCurrentArrow)
   {
   case ARROW_TYPE_AT:
      {
         axis = targetAt;
         upAxis = targetUp;
         rightAxis = targetRight;
         axis.normalize();
         upAxis.normalize();
         rightAxis.normalize();
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis = targetRight;
         upAxis = targetUp;
         rightAxis = -targetAt;
         axis.normalize();
         upAxis.normalize();
         rightAxis.normalize();
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis = targetUp;
         upAxis = -targetAt;
         rightAxis = targetRight;
         axis.normalize();
         upAxis.normalize();
         rightAxis.normalize();
         break;
      }
   default:
      {
         return;
      }
   }

   // Calculate the angle
   float angle = 0.0f;

   // Get the mouse vector.
   osg::Vec3 mouse = GetMouseVector(mMouse->GetPosition());

   // Calculate the mouse collision in the 3D space relative to the plane
   // of the camera and the desired axis of the object.
   float fStartOffset   = camPos *    axis;
   float fDistMod       = mouse *     axis;
   float fPlaneOffset   = targetPos * axis;

   float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

   // Find the projected point of collision on the plane.
   osg::Vec3 projection = (mouse * fDistance) + camPos;
   osg::Vec3 vector     = projection - targetPos;
   vector.normalize();

   float fDotAngle = upAxis * vector;
   float fDirection = rightAxis * vector;

   // The first update should not cause a rotation to happen,
   // Instead, it should set the current mouse position to be
   // the current angle.
   if (mMouseOffset.x() != 0.0f || mMouseOffset.y() != 0.0f)
   {
      mMouseOffset.set(0.0f, 0.0f);

      mOriginAngle = acos(fDotAngle);
      if (fDirection < 0.0f)
      {
         mOriginAngle = -mOriginAngle;
      }

      // Snap angle to 45 degree angles.
      if (mSnap)
      {
         float snaps = ceilf((mOriginAngle - 0.5f) / mSnapRotation);
         mOriginAngle = (mSnapRotation * snaps);
      }

      mAngleGeode->removeDrawable(mAngleDrawable.get());
      mAngleGeode->addDrawable(mAngleDrawable.get());
      mAngleOriginGeode->removeDrawable(mAngleOriginDrawable.get());
      mAngleOriginGeode->addDrawable(mAngleOriginDrawable.get());

      osg::Matrix matrix;

      if (mCoordinateSpace == LOCAL_SPACE)
      {
         target->GetTransform(transform);
         transform.GetRotation(matrix);
      }

      if (mCurrentArrow == ARROW_TYPE_UP)
      {
         matrix *= matrix.rotate(osg::DegreesToRadians(90.0f), rightAxis);
      }
      matrix *= matrix.rotate(mOriginAngle, axis);

      transform.SetRotation(matrix);
      transform.SetTranslation(osg::Vec3(0,0,0));

      float scale = GetAutoScaleSize();
      transform.Rescale(osg::Vec3(scale, scale, scale));
      transform.SetTranslation(targetPos);
      mAngleTransform->SetTransform(transform);
      mAngleOriginTransform->SetTransform(transform);
   }
   else
   {
      angle = acos(fDotAngle);

      if (fDirection < 0.0f)
      {
         angle = -angle;
      }

      // Snap angle to 45 degree angles.
      if (mSnap)
      {
         float snaps = ceilf((angle - 0.5f) / mSnapRotation);
         angle = (mSnapRotation * snaps);
      }

      if (mCoordinateSpace == WORLD_SPACE)
      {
         float originAngle = mOriginAngle;
         mOriginAngle = angle;
         angle -= originAngle;
      }
      else
      {
         angle -= mOriginAngle;
      }

      if (angle != 0)
      {
         // Rotate the actual object.
         osg::Matrix matrix;
         target->GetTransform(transform);
         transform.GetRotation(matrix);
         matrix *= matrix.rotate(angle, axis);
         transform.SetRotation(matrix);
         target->SetTransform(transform);

         matrix = matrix.identity();
         if (mCurrentArrow == ARROW_TYPE_UP)
         {
            matrix *= matrix.rotate(osg::DegreesToRadians(90.0f), rightAxis);
         }
         matrix *= matrix.rotate(mOriginAngle, axis);

         float scale = GetAutoScaleSize();

         // Rotate the compass angle.
         if (mCoordinateSpace == WORLD_SPACE)
         {
            transform.SetRotation(matrix);
            transform.SetTranslation(osg::Vec3(0,0,0));
            transform.Rescale(osg::Vec3(scale, scale, scale));
            transform.SetTranslation(targetPos);
            mAngleTransform->SetTransform(transform);
         }
         else
         {
            mAngleOriginTransform->GetTransform(transform);
            transform.GetRotation(matrix);
            matrix *= matrix.rotate(-angle, axis);
            transform.SetRotation(matrix);
            transform.SetTranslation(osg::Vec3(0,0,0));
            transform.Rescale(osg::Vec3(scale, scale, scale));
            transform.SetTranslation(targetPos);
            mAngleOriginTransform->SetTransform(transform);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateScale(void)
{
   // TODO: Not currenlty implemented as there may be issues with scaling transformables.
}

////////////////////////////////////////////////////////////////////////////////
