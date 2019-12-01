// objectmotionmodel.cpp: Implementation of the ObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/objectmotionmodel.h>

#include <osg/Math>
#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/CullFace>
#include <osg/PolygonMode>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osgViewer/GraphicsWindow>

#include <dtCore/system.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/batchisector.h>

#include <iostream>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(ObjectMotionModel)

const int ARROW_NODE_MASK = 0x0f000000;
const float SENSITIVITY = 0.1f;

//////////////////////////////////////////////////////////////////////////
ObjectMotionModel::ObjectMotionModel(dtCore::View* view)
   : MotionModel("ObjectMotionModel")
   , mScale(1.0f)
   , mVisible(false)
   , mInteractionEnabled(true)
   , mCurScale(0.0f)
   , mView(NULL)
   , mSceneNode(NULL)
   , mMouse(NULL)
   , mCamera(NULL)
   , mMaxDistance(-1)
   , mCoordinateSpace(LOCAL_SPACE)
   , mMotionType(MOTION_TYPE_MAX)
   , mHoverArrow(ARROW_TYPE_MAX)
   , mLeftMouse(false)
   , mRightMouse(false)
   , mMouseDown(false)
   , mMouseLocked(false)
   , mOriginAngle(0.0f)
   , mAutoScale(true)
   , mAllowScaleGizmo(false)
   , mSnap(false)
   , mSnapTranslationEnabled(false)
   , mSnapRotationEnabled(false)
   , mSnapScaleEnabled(false)
{
   mSnapRotation = osg::DegreesToRadians(45.0f);
   mSnapTranslation = 1.0f;
   mSnapScale = 1;

   RegisterInstance(this);

   SetView(view);

   // Create our three axis.
   InitArrows();

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &ObjectMotionModel::OnSystem);
}

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::~ObjectMotionModel()
{
   SetEnabled(false);

   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetTarget(Transformable* target)
{
   MotionModel::SetTarget(target);

   UpdateVisibility();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetView(dtCore::View* view)
{
   // First remove this model from the current view scene.
   bool bWasEnabled = IsEnabled();
   SetEnabled(false);

   mView = view;

   if (view)
   {
      if (view->GetScene())
      {
         SetSceneNode(view->GetScene()->GetSceneNode());
      }

      mMouse  = view->GetMouse();
      SetCamera(view->GetCamera());

      SetEnabled(bWasEnabled);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetSceneNode(osg::Group* sceneNode)
{
   bool bWasEnabled = IsEnabled();
   SetEnabled(false);

   mSceneNode = sceneNode;

   SetEnabled(bWasEnabled);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetCamera(dtCore::Camera* camera)
{
   mCamera = camera;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetEnabled(bool enabled)
{
   if (IsEnabled() != enabled)
   {
      MotionModel::SetEnabled(enabled);

      UpdateVisibility();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetInteractionEnabled(bool enabled)
{
   mInteractionEnabled = enabled;

   // If we are turning interaction off, make sure we clear all
   // interaction data.
   if (!mInteractionEnabled)
   {
      HighlightWidgets(NULL);
      mMouseDown     = false;
      mMouseLocked   = false;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetScaleEnabled(bool enabled)
{
   if (mAllowScaleGizmo != enabled)
   {
      mAllowScaleGizmo = enabled;

      // Only allow the scale gizmos if we are in local space.
      if (mCoordinateSpace == WORLD_SPACE)
      {
         enabled = false;
      }

      if (mAllowScaleGizmo)
      {
         mTargetTransform->addChild(mScaleTransform->GetOSGNode());
      }
      else
      {
         mTargetTransform->removeChild(mScaleTransform->GetOSGNode());
      }

      if (enabled)
      {
         for (int arrow = 0; arrow < ARROW_TYPE_MAX; arrow++)
         {
            mTargetTransform->addChild(mArrows[arrow].scaleTransform->GetOSGNode());
         }
      }
      else
      {
         for (int arrow = 0; arrow < ARROW_TYPE_MAX; arrow++)
         {
            mTargetTransform->removeChild(mArrows[arrow].scaleTransform->GetOSGNode());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetScale(float scale)
{
   mScale = scale;
   UpdateWidgets();
}

////////////////////////////////////////////////////////////////////////////////
float ObjectMotionModel::GetAutoScaleSize(void)
{
   if (mAutoScale)
   {
      if (mCamera)
      {
         const osg::Camera* camera = mCamera->GetOSGCamera();

         double left, right, bottom, top, near, far;
         if (camera->getProjectionMatrixAsOrtho(left, right, bottom, top, near, far))
         {
            if (camera->getViewport())
            {
               int width = int(camera->getViewport()->width());
               if (width > 0)
               {
                  double windowSize = right - left;
                  float zoomFactor = width / windowSize;

                  if (zoomFactor != 0.0f)
                  {
                     return GetScale() / zoomFactor;
                  }
               }
            }
         }
         else
         {
            // Perspective views are scaled by the distance to the camera.
            return GetScale() * GetCameraDistanceToTarget();
         }
      }
   }

   return GetScale();
}

////////////////////////////////////////////////////////////////////////////////
float ObjectMotionModel::GetCameraDistanceToTarget(void)
{
   if (!mCamera)
   {
      return 0.0f;
   }

   dtCore::Transform transform;
   mCamera->GetTransform(transform);
   osg::Vec3 camPos = transform.GetTranslation();

   dtCore::Transformable* target = GetTarget();
   if (!target)
   {
      return 0.0f;
   }

   dtCore::Transform targetTransform;
   target->GetTransform(targetTransform);

   osg::Vec3 VecToTarget = targetTransform.GetTranslation() - camPos;
   return VecToTarget.length();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetAutoScaleEnabled(bool enabled)
{
   if (mTargetTransform.valid())
   {
      //mTargetTransform->setAutoScaleToScreen(enabled);

      mAutoScale = enabled;
   }
}

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::CoordinateSpace ObjectMotionModel::GetCoordinateSpace(void)
{
   return mCoordinateSpace;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetMaxObjectDistanceFromCamera(float distance)
{
   mMaxDistance = distance;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetCoordinateSpace(CoordinateSpace coordinateSpace)
{
   if (mCoordinateSpace != coordinateSpace)
   {
      mCoordinateSpace = coordinateSpace;

      bool enabled = mAllowScaleGizmo;

      // Only allow the scale gizmos if we are in local space.
      if (mCoordinateSpace == WORLD_SPACE)
      {
         enabled = false;
      }

      if (mAllowScaleGizmo)
      {
         mTargetTransform->addChild(mScaleTransform->GetOSGNode());
      }
      else
      {
         mTargetTransform->removeChild(mScaleTransform->GetOSGNode());
      }

      if (enabled)
      {
         for (int arrow = 0; arrow < ARROW_TYPE_MAX; arrow++)
         {
            mTargetTransform->addChild(mArrows[arrow].scaleTransform->GetOSGNode());
         }
      }
      else
      {
         for (int arrow = 0; arrow < ARROW_TYPE_MAX; arrow++)
         {
            mTargetTransform->removeChild(mArrows[arrow].scaleTransform->GetOSGNode());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetSnapTranslation(float increment)
{
   mSnapTranslation = increment;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetSnapRotation(float degrees)
{
   mSnapRotation = osg::DegreesToRadians(degrees);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetSnapScale(float increment)
{
   mSnapScale = increment;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetSnapEnabled(bool translation, bool rotation, bool scale)
{
   mSnapTranslationEnabled = translation;
   mSnapRotationEnabled = rotation;
   mSnapScaleEnabled = scale;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 ObjectMotionModel::GetMousePosition(void)
{
   return mMousePos;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnLeftMousePressed(void)
{
   mLeftMouse = true;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnLeftMouseReleased(void)
{
   mLeftMouse = false;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnRightMousePressed(void)
{
   mRightMouse = true;
   mSnap = true;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnRightMouseReleased(void)
{
   mRightMouse = false;
   mSnap = false;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (str == dtCore::System::MESSAGE_FRAME_SYNCH)
   {
      // If we have a Delta3D mouse, then set our mouse position with it.
      if (mMouse)
      {
         bool bLeftMouse = mMouse->GetButtonState(dtCore::Mouse::LeftButton);
         bool bRightMouse= mMouse->GetButtonState(dtCore::Mouse::RightButton);

         if (mLeftMouse != bLeftMouse)
         {
            if (bLeftMouse) OnLeftMousePressed();
            else            OnLeftMouseReleased();
         }

         if (mRightMouse != bRightMouse)
         {
            if (bRightMouse) OnRightMousePressed();
            else             OnRightMouseReleased();
         }

         osg::Vec2 mousePos = mMouse->GetPosition();
         Update(mousePos);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::MotionType ObjectMotionModel::Update(const osg::Vec2& pos)
{
   if (mMousePos != pos)
   {
      mMousePos = pos;

      if (mVisible && mTargetTransform.valid())
      {
         if (mInteractionEnabled)
         {
            // Now update the widget highlights based on the new mouse position.
            // When the mouse is released, deselect our current arrow.
            if (!mLeftMouse && !mRightMouse)
            {
               if (mMouseLocked)
               {
                  mAngleGeode->removeDrawable(mAngleDrawable.get());
                  mAngleOriginGeode->removeDrawable(mAngleOriginDrawable.get());

                  //callbacks
                  switch(mMotionType)
                  {
                     case MOTION_TYPE_PLANE_TRANSLATION:
                     case MOTION_TYPE_TRANSLATION: OnTranslateEnd(); break;
                     case MOTION_TYPE_ROTATION: OnRotateEnd(); break;
                     case MOTION_TYPE_SCALE: OnScaleEnd(); break;
                     default: break;
                  }
               }

               mMouseDown     = false;
               mMouseLocked   = false;

               // Test for arrow hovering.
               HighlightWidgets(MousePick());
            }
            else
            {
               // First mouse click.
               if (!mMouseDown)
               {
                  mMouseDown = true;

                  // Do a collision test to see if the mouse collides with any of
                  // the motion arrows.
                  if (HighlightWidgets(MousePick()))
                  {
                     mMouseLocked = true;

                     // Get the offset mouse position.
                     dtCore::Transformable* target = GetTarget();
                     if (target)
                     {
                        dtCore::Transform transform;
                        target->GetTransform(transform);
                        osg::Vec2 objectPos = GetObjectScreenCoordinates(transform.GetTranslation());
                        mMouseOrigin = GetMousePosition();
                        mMouseOffset = objectPos - mMouseOrigin;
                        mOriginalTargetPos = transform.GetTranslation();
                        mOriginAngle = 0.0f;

                        //callbacks
                        switch(mMotionType)
                        {
                           case MOTION_TYPE_PLANE_TRANSLATION:
                           case MOTION_TYPE_TRANSLATION: OnTranslateBegin(); break;
                           case MOTION_TYPE_ROTATION: OnRotateBegin(); break;
                           case MOTION_TYPE_SCALE: OnScaleBegin(); break;
                           default: break;
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

                  case MOTION_TYPE_PLANE_TRANSLATION:
                     UpdatePlanarTranslation();
                     break;

                  default:
                     break;
                  }
               }
            }
         }

         UpdateWidgets();
      }
   }

   return mMotionType;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateWidgets(void)
{
   if (mVisible && mTargetTransform.valid())
   {
      // Update the position of the arrows to the position of the target.
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

      osg::Quat q;
      transform.GetRotation(q);
      mTargetTransform->setRotation(q);
      mTargetTransform->setPosition(position);

      float scale = GetAutoScaleSize();
      if (mCurScale != scale)
      {
         mTargetTransform->setScale(scale);
         mCurScale = scale;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::InitArrows(void)
{
   mTargetTransform = new osg::AutoTransform();
   mTargetTransform->setAutoRotateMode(osg::AutoTransform::NO_ROTATION);

   osg::Group* groupNode = mTargetTransform->asGroup();
   if (groupNode)
   {
      // Make this group render top most.
      osg::StateSet* stateSet = groupNode->getOrCreateStateSet();
      if (stateSet)
      {
         //stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
         stateSet->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
         stateSet->setMode(GL_BLEND, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
         stateSet->setRenderBinDetails(98, "RenderBin");

         osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
         stateSet->setAttribute(polygonMode, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

         osg::Depth* depth = new osg::Depth(osg::Depth::ALWAYS);
         stateSet->setAttribute(depth, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

         osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
         stateSet->setAttribute(blend, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
      }
   }

   const float ringRadius             = 0.08f;
   const float ringVisibleThickness   = 0.004f;
   const float ringSelectionThickness = 0.01f;

   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      // Create all our objects and nodes.
      mArrows[arrowIndex].translationTransform   = new dtCore::Transformable();
      mArrows[arrowIndex].rotationTransform      = new dtCore::Transformable();
      mArrows[arrowIndex].scaleTransform         = new dtCore::Transformable();
      mArrows[arrowIndex].planarTransform        = new dtCore::Transformable();
      mArrows[arrowIndex].arrowGeode             = new osg::Geode();
      mArrows[arrowIndex].rotationGeode          = new osg::Geode();
      mArrows[arrowIndex].rotationSelectionGeode = new osg::Geode();
      mArrows[arrowIndex].scaleGeode             = new osg::Geode();
      mArrows[arrowIndex].planarGeode            = new osg::Geode();
      mArrows[arrowIndex].planarSelectionGeode   = new osg::Geode();

      osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.09f), 0.008f, 0.14f);
      osg::Cone*     cone     = new osg::Cone(osg::Vec3(0.0f, 0.0f, 0.165f), 0.015f, 0.02f);
      osg::Box*      box      = new osg::Box(osg::Vec3(0.0f, 0.0f, 0.2f), 0.015f, 0.015f, 0.015f);

      osg::TriangleMesh* ring = GenerateRing(ringRadius - ringVisibleThickness, ringRadius + ringVisibleThickness, 80);
      osg::TriangleMesh* selectionRing = GenerateRing(ringRadius - ringSelectionThickness, ringRadius + ringSelectionThickness, 80);

      osg::TriangleMesh* plane = GeneratePlane(0.035f, 0.04f);
      osg::TriangleMesh* selectionPlane = GeneratePlane(0.03f, 0.045f);

      mArrows[arrowIndex].arrowCylinder = new osg::ShapeDrawable(cylinder);
      mArrows[arrowIndex].arrowCone     = new osg::ShapeDrawable(cone);
      mArrows[arrowIndex].scaleBox      = new osg::ShapeDrawable(box);
      mArrows[arrowIndex].arrowCylinder->setUseDisplayList(false);
      mArrows[arrowIndex].arrowCone->setUseDisplayList(false);
      mArrows[arrowIndex].scaleBox->setUseDisplayList(false);

      mArrows[arrowIndex].rotationRing  = new osg::ShapeDrawable(ring);
      mArrows[arrowIndex].rotationRing->setUseDisplayList(false);
      osg::ShapeDrawable* rotationSelectionRing = new osg::ShapeDrawable(selectionRing);
      rotationSelectionRing->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
      rotationSelectionRing->setUseDisplayList(false);

      mArrows[arrowIndex].planarGeom    = new osg::ShapeDrawable(plane);
      mArrows[arrowIndex].planarGeom->setUseDisplayList(false);
      osg::ShapeDrawable* planarSelectionGeom = new osg::ShapeDrawable(selectionPlane);
      planarSelectionGeom->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
      planarSelectionGeom->setUseDisplayList(false);

      // Now set up their Hierarchy.
      mTargetTransform->addChild(mArrows[arrowIndex].translationTransform->GetOSGNode());
      mTargetTransform->addChild(mArrows[arrowIndex].rotationTransform->GetOSGNode());
      mTargetTransform->addChild(mArrows[arrowIndex].planarTransform->GetOSGNode());

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

      osg::Group* scaleGroup = mArrows[arrowIndex].scaleTransform->GetOSGNode()->asGroup();
      if (scaleGroup)
      {
         scaleGroup->addChild(mArrows[arrowIndex].scaleGeode.get());
      }

      osg::Group* planarGroup = mArrows[arrowIndex].planarTransform->GetOSGNode()->asGroup();
      if (planarGroup)
      {
         planarGroup->addChild(mArrows[arrowIndex].planarGeode.get());
         planarGroup->addChild(mArrows[arrowIndex].planarSelectionGeode.get());
      }

      mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCylinder.get());
      mArrows[arrowIndex].arrowGeode->addDrawable(mArrows[arrowIndex].arrowCone.get());
      mArrows[arrowIndex].arrowGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].rotationGeode->addDrawable(mArrows[arrowIndex].rotationRing.get());
      mArrows[arrowIndex].rotationGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].rotationSelectionGeode->addDrawable(rotationSelectionRing);
      mArrows[arrowIndex].rotationSelectionGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].scaleGeode->addDrawable(mArrows[arrowIndex].scaleBox.get());
      mArrows[arrowIndex].scaleGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].planarGeode->addDrawable(mArrows[arrowIndex].planarGeom.get());
      mArrows[arrowIndex].planarGeode->setNodeMask(ARROW_NODE_MASK);

      mArrows[arrowIndex].planarSelectionGeode->addDrawable(planarSelectionGeom);
      mArrows[arrowIndex].planarSelectionGeode->setNodeMask(ARROW_NODE_MASK);
   }

   mScaleTransform        = new dtCore::Transformable();
   mScaleGeode            = new osg::Geode();
   osg::Sphere* scaleOrb  = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.012f);
   mScaleOrb              = new osg::ShapeDrawable(scaleOrb);
   mScaleOrb->setUseDisplayList(false);
   mScaleGeode->addDrawable(mScaleOrb.get());
   mScaleGeode->setNodeMask(ARROW_NODE_MASK);
   osg::Group* scaleGroup = mScaleTransform->GetOSGNode()->asGroup();
   if (scaleGroup)
   {
      scaleGroup->addChild(mScaleGeode.get());
   }
   //mTargetTransform->addChild(mScaleTransform->GetOSGNode());

   mAngleTransform = new dtCore::Transformable();
   mAngleGeode     = new osg::Geode();
   mAngleCylinder  = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, ringRadius * 0.5f), 0.001f, ringRadius);
   mAngleDrawable  = new osg::ShapeDrawable(mAngleCylinder.get());

   mAngleGeode->setNodeMask(ARROW_NODE_MASK);
   mAngleTransform->GetOSGNode()->asGroup()->addChild(mAngleGeode.get());
   mTargetTransform->addChild(mAngleTransform->GetOSGNode());

   mAngleOriginTransform = new dtCore::Transformable();
   mAngleOriginGeode     = new osg::Geode();
   mAngleOriginCylinder  = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, ringRadius * 0.5f), 0.001f, ringRadius);
   mAngleOriginDrawable  = new osg::ShapeDrawable(mAngleOriginCylinder.get());
   mAngleOriginGeode->setNodeMask(ARROW_NODE_MASK);

   mAngleOriginTransform->GetOSGNode()->asGroup()->addChild(mAngleOriginGeode.get());
   mTargetTransform->addChild(mAngleOriginTransform->GetOSGNode());

   UpdateVisibility();

   // Now position and rotate our axes.
   dtCore::Transform transformX;
   transformX.SetTranslation(0.0f, 0.0f, 0.0f);
   transformX.Set(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
   mArrows[ARROW_TYPE_RIGHT].translationTransform->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].rotationTransform->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].scaleTransform->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].planarTransform->SetTransform(transformX);
   mArrows[ARROW_TYPE_RIGHT].arrowCylinderColor = osg::Vec4(1.0f, 0.0f, 0.0f, 0.6f);
   mArrows[ARROW_TYPE_RIGHT].arrowConeColor = osg::Vec4(1.0f, 0.3f, 0.3f, 0.6f);
   mArrows[ARROW_TYPE_RIGHT].planarColor = osg::Vec4(1.0f, 1.0f, 0.0f, 0.6f);

   dtCore::Transform transformY;
   transformY.SetTranslation(0.0f, 0.0f, 0.0f);
   transformY.Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
   mArrows[ARROW_TYPE_AT].translationTransform->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].rotationTransform->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].scaleTransform->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].planarTransform->SetTransform(transformY);
   mArrows[ARROW_TYPE_AT].arrowCylinderColor = osg::Vec4(0.0f, 1.0f, 0.0f, 0.6f);
   mArrows[ARROW_TYPE_AT].arrowConeColor = osg::Vec4(0.3f, 1.0f, 0.3f, 0.6f);
   mArrows[ARROW_TYPE_AT].planarColor = osg::Vec4(0.0f, 1.0f, 1.0f, 0.6f);

   dtCore::Transform transformZ;
   transformZ.SetTranslation(0.0f, 0.0f, 0.0f);
   transformZ.Set(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
   mArrows[ARROW_TYPE_UP].translationTransform->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].rotationTransform->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].scaleTransform->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].planarTransform->SetTransform(transformZ);
   mArrows[ARROW_TYPE_UP].arrowCylinderColor = osg::Vec4(0.0f, 0.0f, 1.0f, 0.6f);
   mArrows[ARROW_TYPE_UP].arrowConeColor = osg::Vec4(0.3f, 0.3f, 1.0f, 0.6f);
   mArrows[ARROW_TYPE_UP].planarColor = osg::Vec4(1.0f, 0.0f, 1.0f, 0.6f);

   SetArrowHighlight(ARROW_TYPE_MAX);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateVisibility()
{
   bool visible = IsEnabled() && GetTarget();
   if (mVisible != visible)
   {
      // Make sure our arrows are in the scene.
      if (mSceneNode && mTargetTransform.valid())
      {
         if (visible)
         {
            mSceneNode->addChild(mTargetTransform.get());
         }
         else
         {
            mSceneNode->removeChild(mTargetTransform.get());
         }

         mVisible = visible;
      }
   }

   UpdateWidgets();
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

   osg::Vec3 minVertex = osg::Vec3(minRadius, 0.0f, 0.0f);
   osg::Vec3 maxVertex = osg::Vec3(maxRadius, 0.0f, 0.0f);
   osg::Vec3 midVertex = osg::Vec3(minRadius + ((maxRadius - minRadius) / 2), 0.0f, 0.0f);
   osg::Vec3 lowVertex = osg::Vec3(minRadius * 0.9f, 0.0f, 0.0f);
   osg::Vec3 axis = osg::Vec3(0.0f, 0.0f, 1.0f);

   osg::Matrix rotationMatrix;
   for (int segmentIndex = 0; segmentIndex < segments; segmentIndex++)
   {
      float angle = segmentAngle * segmentIndex;
      rotationMatrix = rotationMatrix.rotate(angle, axis);

      osg::Vec3 minV = minVertex;
      osg::Vec3 maxV = maxVertex;
      if (segmentIndex % (segments / 4) == 0)
      {
         maxV = midVertex;
         minV = lowVertex;
      }

      osg::Vec3 vert = minV * rotationMatrix;
      vertices->push_back(vert);
      indices->push_back((int)vertices->size()-1);

      // Once we get to our second segment, we need to start connecting
      // with our previous segments.
      if (segmentIndex > 0)
      {
         indices->push_back((int)vertices->size()-1);
         indices->push_back((int)vertices->size()-2);
      }

      vert = maxV * rotationMatrix;
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

   // Now make new faces in the opposite direction so it is double sided.
   for (int index = indices->size()-1; index >= 0; index--)
   {
      indices->push_back((*indices)[index]);
   }

   mesh->setVertices(vertices);
   mesh->setIndices(indices);

   return mesh;
}


////////////////////////////////////////////////////////////////////////////////
osg::TriangleMesh* ObjectMotionModel::GeneratePlane(float inner, float outer)
{
   osg::Vec3Array* vertices = new osg::Vec3Array();
   osg::IntArray* indices   = new osg::IntArray();

   osg::TriangleMesh* mesh = new osg::TriangleMesh();

   vertices->push_back(osg::Vec3(0.0f, 0.0f, inner));
   vertices->push_back(osg::Vec3(0.0f, 0.0f, outer));
   vertices->push_back(osg::Vec3(0.0f, outer, outer));
   vertices->push_back(osg::Vec3(0.0f, outer, 0.0f));
   vertices->push_back(osg::Vec3(0.0f, inner, 0.0f));
   vertices->push_back(osg::Vec3(0.0f, inner, inner));

   indices->push_back(5);
   indices->push_back(0);
   indices->push_back(1);

   indices->push_back(5);
   indices->push_back(1);
   indices->push_back(2);

   indices->push_back(5);
   indices->push_back(2);
   indices->push_back(3);

   indices->push_back(5);
   indices->push_back(3);
   indices->push_back(4);

   // Now make new faces in the opposite direction so it is double sided.
   for (int index = indices->size()-1; index >= 0; index--)
   {
      indices->push_back((*indices)[index]);
   }

   mesh->setVertices(vertices);
   mesh->setIndices(indices);

   return mesh;
}


////////////////////////////////////////////////////////////////////////////////
dtCore::DeltaDrawable* ObjectMotionModel::MousePick(void)
{
   if (!mView)
   {
      return NULL;
   }

   if (!mCamera)
   {
      return NULL;
   }

   osg::Vec3 startPoint, endPoint;
   GetMouseLine(GetMousePosition(), endPoint, startPoint);

   // Can't do anything if we don't have a valid mouse line.
   if (startPoint != endPoint)
   {
      dtCore::RefPtr<dtCore::BatchIsector> isector = new dtCore::BatchIsector(mView->GetScene());
      isector->EnableAndGetISector(0).SetSectorAsLineSegment(startPoint, endPoint);

      if (isector->Update())
      {
         // Check for planar translation.
         const dtCore::BatchIsector::HitList& hitlist = isector->GetSingleISector(0).GetHitList();
         for (dtCore::BatchIsector::HitList::const_reverse_iterator hitItr = hitlist.rbegin();
            hitItr != hitlist.rend();
            ++hitItr)
         {
            for (osg::NodePath::const_reverse_iterator nodeItr = hitItr->getNodePath().rbegin();
               nodeItr != hitItr->getNodePath().rend();
               ++nodeItr)
            {
               osg::Node* node = (*nodeItr);
               if (node == mScaleTransform->GetOSGNode())
               {
                  return mScaleTransform.get();
               }

               for (int ArrowIndex = 0; ArrowIndex < ARROW_TYPE_MAX; ++ArrowIndex)
               {
                  if (node == mArrows[ArrowIndex].planarTransform->GetOSGNode())
                  {
                     return mArrows[ArrowIndex].planarTransform.get();
                  }
               }
            }
         }

         // Check for rotation.
         for (dtCore::BatchIsector::HitList::const_reverse_iterator hitItr = hitlist.rbegin();
            hitItr != hitlist.rend();
            ++hitItr)
         {
            for (osg::NodePath::const_reverse_iterator nodeItr = hitItr->getNodePath().rbegin();
                  nodeItr != hitItr->getNodePath().rend();
                  ++nodeItr)
            {
               osg::Node* node = (*nodeItr);
               for (int ArrowIndex = 0; ArrowIndex < ARROW_TYPE_MAX; ++ArrowIndex)
               {
                  if (node == mArrows[ArrowIndex].rotationTransform->GetOSGNode())
                  {
                     return mArrows[ArrowIndex].rotationTransform.get();
                  }
               }
            }
         }

         // If we are not trying to rotate, then check the rest of the motion types.
         for (dtCore::BatchIsector::HitList::const_reverse_iterator hitItr = hitlist.rbegin();
            hitItr != hitlist.rend();
            ++hitItr)
         {
            for (osg::NodePath::const_reverse_iterator nodeItr = hitItr->getNodePath().rbegin();
               nodeItr != hitItr->getNodePath().rend();
               ++nodeItr)
            {
               osg::Node* node = (*nodeItr);
               if (node == mScaleTransform->GetOSGNode())
               {
                  return mScaleTransform.get();
               }

               for (int ArrowIndex = 0; ArrowIndex < ARROW_TYPE_MAX; ++ArrowIndex)
               {
                  if (node == mArrows[ArrowIndex].translationTransform->GetOSGNode())
                  {
                     return mArrows[ArrowIndex].translationTransform.get();
                  }
                  else if (node == mArrows[ArrowIndex].scaleTransform->GetOSGNode())
                  {
                     return mArrows[ArrowIndex].scaleTransform.get();
                  }
               }
            }
         }
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::GetMouseLine(const osg::Vec2& mousePos, osg::Vec3& start, osg::Vec3& end)
{
   if (!mCamera)
   {
      return;
   }

   float distance = GetCameraDistanceToTarget();

   dtCore::Transform transform;
   mCamera->GetTransform(transform);

   const osg::Camera* camera = mCamera->GetOSGCamera();

   double left, right, bottom, top, near, far;
   if (camera->getProjectionMatrixAsOrtho(left, right, bottom, top, near, far))
   {
      double xDif = right - left;
      double yDif = top - bottom;

      osg::Vec3 rightAxis, upAxis, forwardAxis;
      transform.GetOrientation(rightAxis, upAxis, forwardAxis);
      osg::Vec3 center = transform.GetTranslation();
      center += rightAxis * ((xDif * 0.5f) * mousePos.x());
      center += upAxis * ((yDif * 0.5f) * mousePos.y());

      osg::Vec3 dir = forwardAxis * 45000.0f;
      start = center - dir;
      end = center + dir;
   }
   else
   {
      osg::Matrix matrix = camera->getViewMatrix() * camera->getProjectionMatrix();
      const osg::Matrix inverse = osg::Matrix::inverse(matrix);

      start = transform.GetTranslation();

      osg::Vec3 dir = (osg::Vec3(mousePos.x(), mousePos.y(), 0.0f) * inverse) - start;
      dir.normalize();
      end = start + (dir * (distance * 1.5f));
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 ObjectMotionModel::GetObjectScreenCoordinates(const osg::Vec3& objectPos)
{
   if (!mCamera)
   {
      return osg::Vec2();
   }

   dtCore::Transform transform;
   mCamera->GetTransform(transform);

   const osg::Camera* camera = mCamera->GetOSGCamera();

   double left, right, bottom, top, near, far;
   if (camera->getProjectionMatrixAsOrtho(left, right, bottom, top, near, far))
   {
      double xDif = (right - left) * 0.5f;
      double yDif = (top - bottom) * 0.5f;

      osg::Vec3 rightAxis, upAxis, forwardAxis;
      transform.GetOrientation(rightAxis, upAxis, forwardAxis);

      osg::Vec3 offset = transform.GetTranslation() - objectPos;
      osg::Vec2 center;
      center.x() = xDif + (rightAxis * offset);
      center.y() = yDif + (upAxis * offset);

      if (xDif == 0.0f && yDif == 0.0f)
      {
         return osg::Vec2();
      }

      center.x() = 1.0f - (center.x() / xDif);
      center.y() = 1.0f - (center.y() / yDif);

      return center;
   }
   else
   {
      osg::Matrix matrix = camera->getViewMatrix() * camera->getProjectionMatrix();

      osg::Vec3 screenPos = objectPos * matrix;
      return osg::Vec2(screenPos.x(), screenPos.y());
   }
}

////////////////////////////////////////////////////////////////////////////////
bool ObjectMotionModel::HighlightWidgets(dtCore::DeltaDrawable* drawable)
{
   if (drawable)
   {
      if (drawable == mScaleTransform.get())
      {
         mMotionType = MOTION_TYPE_SCALE;
         SetArrowHighlight(ARROW_TYPE_ALL);
         return true;
      }

      for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
      {
         if (drawable == mArrows[arrowIndex].translationTransform.get())
         {
            mMotionType = MOTION_TYPE_TRANSLATION;
            SetArrowHighlight((ArrowType)arrowIndex);
            return true;
         }
         else if (drawable == mArrows[arrowIndex].rotationTransform.get())
         {
            mMotionType = MOTION_TYPE_ROTATION;
            SetArrowHighlight((ArrowType)arrowIndex);
            return true;
         }
         else if (drawable == mArrows[arrowIndex].scaleTransform.get())
         {
            mMotionType = MOTION_TYPE_SCALE;
            SetArrowHighlight((ArrowType)arrowIndex);
            return true;
         }
         else if (drawable == mArrows[arrowIndex].planarTransform.get())
         {
            mMotionType = MOTION_TYPE_PLANE_TRANSLATION;
            SetArrowHighlight((ArrowType)arrowIndex);
            return true;
         }
      }
   }

   mMotionType = MOTION_TYPE_MAX;
   SetArrowHighlight(ARROW_TYPE_MAX);
   return false;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::SetArrowHighlight(ArrowType arrowType)
{
   mHoverArrow = arrowType;

   float brightness = 1.0f;
   //if (arrowType != ARROW_TYPE_MAX)
   //{
   //   brightness = 0.5f;
   //}

   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      osg::Vec4 color = mArrows[arrowIndex].arrowCylinderColor;
      color.r() *= 0.5f;
      color.g() *= 0.5f;
      color.b() *= 0.5f;
      color.a() *= brightness;

      mArrows[arrowIndex].rotationRing->setColor(color);
      mArrows[arrowIndex].arrowCylinder->setColor(color);
      mArrows[arrowIndex].scaleBox->setColor(color);

      color = mArrows[arrowIndex].arrowConeColor;
      color.r() *= 0.5f;
      color.g() *= 0.5f;
      color.b() *= 0.5f;
      color.a() *= brightness;

      mArrows[arrowIndex].arrowCone->setColor(color);

      color = mArrows[arrowIndex].planarColor;
      color.r() *= 0.5f;
      color.g() *= 0.5f;
      color.b() *= 0.5f;
      color.a() *= brightness;

      mArrows[arrowIndex].planarGeom->setColor(color);

      //osg::Vec4 highlightColor = mArrows[arrowIndex].arrowCylinderColor;
      //highlightColor.r() *= 20.0f;
      //highlightColor.g() *= 20.0f;
      //highlightColor.b() *= 20.0f;

      if (arrowIndex == mHoverArrow)
      {
         if (mMotionType == MOTION_TYPE_ROTATION)
         {
            mArrows[arrowIndex].rotationRing->setColor(mArrows[arrowIndex].arrowCylinderColor);

            if (mHoverArrow == (ArrowType)arrowIndex)
            {
               mAngleDrawable->setColor(mArrows[arrowIndex].arrowCylinderColor);
               mAngleOriginDrawable->setColor(mArrows[arrowIndex].arrowCylinderColor);
            }
         }
         else if (mMotionType == MOTION_TYPE_TRANSLATION)
         {
            mArrows[arrowIndex].arrowCylinder->setColor(mArrows[arrowIndex].arrowCylinderColor);
            mArrows[arrowIndex].arrowCone->setColor(mArrows[arrowIndex].arrowCylinderColor);
         }
         else if (mMotionType == MOTION_TYPE_SCALE)
         {
            mArrows[arrowIndex].scaleBox->setColor(mArrows[arrowIndex].arrowCylinderColor);
         }
         else if (mMotionType == MOTION_TYPE_PLANE_TRANSLATION)
         {
            mArrows[arrowIndex].planarGeom->setColor(mArrows[arrowIndex].planarColor);
         }
      }
   }

   mScaleOrb->setColor(osg::Vec4(0.75f, 0.75f, 0.75f, 0.6f));
   if (mHoverArrow == ARROW_TYPE_ALL)
   {
      if (mMotionType == MOTION_TYPE_SCALE)
      {
         mScaleOrb->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.6f));
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateTranslation(void)
{
   osg::Vec3 plane1;
   osg::Vec3 plane2;
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

   if (!mCamera)
   {
      return;
   }

   dtCore::Transform camTransform;
   mCamera->GetTransform(camTransform);
   camTransform.GetOrientation(camRight, camUp, camAt);
   camPos = camTransform.GetTranslation();

   if (mCoordinateSpace == LOCAL_SPACE)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.GetOrientation(targetRight, targetUp, targetAt);
      targetPos = mOriginalTargetPos;
      //targetPos = transform.GetTranslation();
   }
   else // World Space
   {
      dtCore::Transform transform;
      transform.GetOrientation(targetRight, targetUp, targetAt);
      target->GetTransform(transform);
      targetPos = transform.GetTranslation();
   }

   switch (mHoverArrow)
   {
   case ARROW_TYPE_AT:
      {
         axis = targetAt;
         plane1 = targetUp;
         plane2 = targetRight;
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis = targetRight;
         plane1 = targetUp;
         plane2 = targetAt;
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis = targetUp;
         plane1 = targetRight;
         plane2 = targetAt;
         break;
      }
   default:
      {
         return;
      }
   }

   // Get the mouse vector.
   osg::Vec3 mouseStart, mouseEnd;
   GetMouseLine(GetMousePosition() + mMouseOffset, mouseStart, mouseEnd);
   osg::Vec3 mouse = mouseEnd - mouseStart;

   osg::Vec3 vecToTarget = targetPos - mouseStart;
   vecToTarget.normalize();
   float fDot = fabs(vecToTarget * axis);
   if (fDot < 0.95f)
   {
      plane1.normalize();
      plane2.normalize();

      float fDot1 = fabs(vecToTarget * plane1);
      float fDot2 = fabs(vecToTarget * plane2);

      // Find the best plane.
      if (fDot1 >= fDot2)
      {
         plane = &plane1;
      }
      else
      {
         plane = &plane2;
      }
   }

   if (plane)
   {
      // Calculate the mouse collision in the 3D space relative to the plane
      // of the camera and the desired axis of the object.
      float fStartOffset   = mouseStart * (*plane);
      float fDistMod       = mouse      * (*plane);
      float fPlaneOffset   = targetPos  * (*plane);

      // Check if the mouse vector does not intersect the plane.
      if (fDistMod == 0.0f) return;
      if (camAt * (*plane) > 0 && mouse * (*plane) < 0) return;
      else if (camAt * (*plane) <= 0 && mouse * (*plane) > 0) return;

      float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

      // Find the projected point of collision on the plane.
      osg::Vec3 projection = (mouse * fDistance) + mouseStart;
      osg::Vec3 vector     = projection - targetPos;

      // Find the translation vector.
      fDistance = axis * vector;

      osg::Vec3 desiredPos = targetPos + (axis * fDistance);

      dtCore::Transform transform;
      target->GetTransform(transform);
      osg::Vec3 actualPos = transform.GetTranslation();

      // Clamp the desired position so it does not leave the camera range.
      if (mMaxDistance > 0)
      {
         // First test the current desired distance.
         float curDist = (camPos - desiredPos).length2();
         if (curDist > (mMaxDistance*mMaxDistance))
         {
            // If we are moving it too far, find a new position that
            // is within the bounds.
            desiredPos = actualPos;
         }
      }

      // Snap
      if (mSnap && mSnapTranslationEnabled)
      {
         // Find the closest grid point to snap to.
         targetPos += axis * fDistance;

         for (int index = 0; index < 3; index++)
         {
            float trans = targetPos[index];

            int mul = int(floorf((trans / mSnapTranslation) + 0.5f));
            trans = mSnapTranslation * mul;

            targetPos[index] = trans;
         }

         desiredPos = targetPos;
      }

      osg::Vec3 offset = desiredPos - actualPos;
      OnTranslate(offset);
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

   osg::Vec2 mousePos = GetMousePosition();

   mMouseOrigin = mousePos;

   osg::Vec3 camUp;
   osg::Vec3 camRight;
   osg::Vec3 camAt;
   //osg::Vec3 camPos;

   if (!mCamera)
   {
      return;
   }

   dtCore::Transform camTransform;
   mCamera->GetTransform(camTransform);
   camTransform.GetOrientation(camRight, camUp, camAt);
   //camPos = camTransform.GetTranslation();

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

   switch (mHoverArrow)
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
   osg::Vec3 mouseStart, mouseEnd;
   GetMouseLine(GetMousePosition(), mouseStart, mouseEnd);
   osg::Vec3 mouse = mouseEnd - mouseStart;

   // Calculate the mouse collision in the 3D space relative to the plane
   // of the camera and the desired axis of the object.
   float fStartOffset   = mouseStart * axis;
   float fDistMod       = mouse      * axis;
   float fPlaneOffset   = targetPos  * axis;

   // Check if the mouse vector does not intersect the plane.
   if (fDistMod == 0.0f) { return; }
   if (camAt * axis > 0 && mouse * axis < 0) { return; }
   else if (camAt * axis <= 0 && mouse * axis > 0) { return; }

   float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

   // Find the projected point of collision on the plane.
   osg::Vec3 projection = (mouse * fDistance) + mouseStart;
   osg::Vec3 vector     = projection - targetPos;
   vector.normalize();

   float fDotAngle = upAxis * vector;
   float fDirection = rightAxis * vector;

   // Debug
   //std::cout << "fDistance: " << fDistance << " fDistMod: " << fDistMod << std::endl;

   // The first update should not cause a rotation to happen
   // unless we're in snap mode, Instead, it should set the
   // current mouse position to be the current angle.
   if (mMouseOffset.x() != 0.0f || mMouseOffset.y() != 0.0f)
   {
      mMouseOffset.set(0.0f, 0.0f);

      mOriginAngle = acos(fDotAngle);
      if (fDirection < 0.0f)
      {
         mOriginAngle = -mOriginAngle;
      }

      // Snap angle to fixed degree angles.
      if (mSnap && mSnapRotationEnabled)
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

      if (mHoverArrow == ARROW_TYPE_UP)
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

      // Snap angle to fixed degree angles.
      if (mSnap && mSnapRotationEnabled)
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
         OnRotate(angle, axis);

         osg::Matrix matrix = matrix.identity();
         if (mHoverArrow == ARROW_TYPE_UP)
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
   osg::Vec3 plane1;
   osg::Vec3 plane2;
   osg::Vec3 axis;
   osg::Vec3 scaleAxis;
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

   if (!mCamera)
   {
      return;
   }

   dtCore::Transform camTransform;
   mCamera->GetTransform(camTransform);
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

   switch (mHoverArrow)
   {
   case ARROW_TYPE_AT:
      {
         axis = targetAt;
         scaleAxis = osg::Vec3(0.0f, 1.0f, 0.0f);
         plane1 = targetUp;
         plane2 = targetRight;
         //plane1 = targetUp ^ axis;
         //plane2 = targetRight ^ axis;
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis = targetRight;
         scaleAxis = osg::Vec3(1.0f, 0.0f, 0.0f);
         plane1 = targetUp;
         plane2 = targetAt;
         //plane1 = targetUp ^ axis;
         //plane2 = axis ^ targetAt;
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis = targetUp;
         scaleAxis = osg::Vec3(0.0f, 0.0f, 1.0f);
         plane1 = targetAt;
         plane2 = targetRight;
         //plane1 = axis ^ targetRight;
         //plane2 = targetAt ^ axis;
         break;
      }

   case ARROW_TYPE_ALL:
      {
         axis = camRight;
         scaleAxis = osg::Vec3(1.0f, 1.0f, 1.0f);
         plane1 = camUp ^ axis;
         plane2 = axis ^ camAt;
         break;
      }

   default:
      {
         return;
      }
   }

   // Get the mouse vector.
   osg::Vec3 mouseStart, mouseEnd;
   GetMouseLine(GetMousePosition() + mMouseOffset, mouseStart, mouseEnd);
   osg::Vec3 mouse = mouseEnd - mouseStart;

   osg::Vec3 vecToTarget = targetPos - mouseStart;
   vecToTarget.normalize();
   float fDot = fabs(vecToTarget * axis);
   if (fDot < 0.95f)
   {
      plane1.normalize();
      plane2.normalize();

      float fDot1 = fabs(vecToTarget * plane1);
      float fDot2 = fabs(vecToTarget * plane2);

      // Find the best plane.
      if (fDot1 >= fDot2)
      {
         plane = &plane1;
      }
      else
      {
         plane = &plane2;
      }
   }

   if (plane)
   {
      // Calculate the mouse collision in the 3D space relative to the plane
      // of the camera and the desired axis of the object.
      float fStartOffset   = mouseStart * (*plane);
      float fDistMod       = mouse      * (*plane);
      float fPlaneOffset   = targetPos  * (*plane);

      // Check if the mouse vector does not intersect the plane.
      if (fDistMod == 0.0f) { return; }
      if (camAt * (*plane) > 0 && mouse * (*plane) < 0) { return; }
      else if (camAt * (*plane) <= 0 && mouse * (*plane) > 0) { return; }

      float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

      // Find the projected point of collision on the plane.
      osg::Vec3 projection = (mouse * fDistance) + mouseStart;
      osg::Vec3 vector     = projection - targetPos;

      // Find the translation vector.
      fDistance = axis * vector;
      fDistance *= SENSITIVITY;

      // Snap
      if (mSnap && mSnapScaleEnabled)
      {
         if (fDistance != 0.0f)
         {
            for (int index = 0; index < 3; index++)
            {
               float len = scaleAxis[index] * fDistance;

               int mul = int(floorf((len / mSnapScale) + 0.5f));
               scaleAxis[index] = (mSnapScale * mul) / fDistance;
            }
         }
      }

      if (scaleAxis.length() >= 0.0001f)
      {
         osg::Vec2 objectPos = GetObjectScreenCoordinates(targetPos);
         mMouseOffset = objectPos - GetMousePosition();

         OnScale(scaleAxis * fDistance);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdatePlanarTranslation(void)
{
   osg::Vec3 plane;
   osg::Vec3 axis1;
   osg::Vec3 axis2;

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

   if (!mCamera)
   {
      return;
   }

   dtCore::Transform camTransform;
   mCamera->GetTransform(camTransform);
   camTransform.GetOrientation(camRight, camUp, camAt);
   camPos = camTransform.GetTranslation();

   if (mCoordinateSpace == LOCAL_SPACE)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.GetOrientation(targetRight, targetUp, targetAt);
      targetPos = mOriginalTargetPos;
      //targetPos = transform.GetTranslation();
   }
   else // World Space
   {
      dtCore::Transform transform;
      transform.GetOrientation(targetRight, targetUp, targetAt);
      target->GetTransform(transform);
      targetPos = transform.GetTranslation();
   }

   switch (mHoverArrow)
   {
   case ARROW_TYPE_AT:
      {
         axis1 = targetAt;
         axis2 = targetUp;
         plane = targetRight;
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis1 = targetRight;
         axis2 = targetAt;
         plane = targetUp;
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis1 = targetUp;
         axis2 = targetRight;
         plane = targetAt;
         break;
      }
   default:
      {
         return;
      }
   }

   // Get the mouse vector.
   osg::Vec3 mouseStart, mouseEnd;
   GetMouseLine(GetMousePosition() + mMouseOffset, mouseStart, mouseEnd);
   osg::Vec3 mouse = mouseEnd - mouseStart;

   // Calculate the mouse collision in the 3D space relative to the plane
   // of the camera and the desired axis of the object.
   float fStartOffset   = mouseStart * plane;
   float fDistMod       = mouse      * plane;
   float fPlaneOffset   = targetPos  * plane;

   // Check if the mouse vector does not intersect the plane.
   if (fDistMod == 0.0f) return;
   if (camAt * plane > 0 && mouse * plane < 0) return;
   else if (camAt * plane <= 0 && mouse * plane > 0) return;

   float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

   // Find the projected point of collision on the plane.
   osg::Vec3 projection = (mouse * fDistance) + mouseStart;
   osg::Vec3 vector     = projection - targetPos;

   // Find the translation vector.
   osg::Vec3 desiredPos = targetPos + (axis1 * (axis1 * vector)) + (axis2 * (axis2 * vector));

   dtCore::Transform transform;
   target->GetTransform(transform);
   osg::Vec3 actualPos = transform.GetTranslation();

   // Clamp the desired position so it does not leave the camera range.
   if (mMaxDistance > 0)
   {
      // First test the current desired distance.
      float curDist = (camPos - desiredPos).length2();
      if (curDist > (mMaxDistance*mMaxDistance))
      {
         // If we are moving it too far, find a new position that
         // is within the bounds.
         desiredPos = actualPos;
      }
   }

   // Snap
   if (mSnap && mSnapTranslationEnabled)
   {
      // Find the closest grid point to snap to.
      targetPos += (axis1 * (axis1 * vector)) + (axis2 * (axis2 * vector));

      for (int index = 0; index < 3; index++)
      {
         float trans = targetPos[index];

         int mul = int(floorf((trans / mSnapTranslation) + 0.5f));
         trans = mSnapTranslation * mul;

         targetPos[index] = trans;
      }

      desiredPos = targetPos;
   }

   osg::Vec3 offset = desiredPos - actualPos;
   OnTranslate(offset);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnTranslate(const osg::Vec3& delta)
{
   dtCore::Transformable* target = GetTarget();

   if (target)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.Move(delta);
      target->SetTransform(transform);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnRotate(float delta, const osg::Vec3& axis)
{
   dtCore::Transformable* target = GetTarget();

   if (target)
   {
      osg::Matrix matrix;
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.GetRotation(matrix);
      matrix *= matrix.rotate(delta, axis);
      transform.SetRotation(matrix);
      target->SetTransform(transform);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnScale(const osg::Vec3& /*delta*/)
{
}

////////////////////////////////////////////////////////////////////////////////
