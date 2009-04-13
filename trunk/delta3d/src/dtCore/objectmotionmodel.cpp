// objectmotionmodel.cpp: Implementation of the ObjectMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/objectmotionmodel.h>

#include <osg/Math>
#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/CullFace>
#include <osg/PolygonMode>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osgUtil/LineSegmentIntersector>

#include <dtCore/system.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/keyboard.h>
#include <dtCore/transform.h>
#include <dtUtil/bits.h>
#include <dtCore/deltawin.h>
#include <dtCore/isector.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(ObjectMotionModel)

const int ARROW_NODE_MASK = 0x0f000000;
const float SENSITIVITY = 5.0f;

//////////////////////////////////////////////////////////////////////////
ObjectMotionModel::ObjectMotionModel(dtCore::View* view)
   : MotionModel("ObjectMotionModel")
   , mView(NULL)
   , mSceneNode(NULL)
   , mCamera(NULL)
   , mMouse(NULL)
   , mScale(1.0f)
   , mCoordinateSpace(LOCAL_SPACE)
   , mMotionType(MOTION_TYPE_MAX)
   , mHoverArrow(ARROW_TYPE_MAX)
   , mLeftMouse(false)
   , mRightMouse(false)
   , mMouseDown(false)
   , mMouseLocked(false)
   , mSnap(false)
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
   SetEnabled(false);

   RemoveSender(&System::GetInstance());

   DeregisterInstance(this);
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

      // Make sure our arrows are in the scene.
      if (mSceneNode && mTargetTransform.valid())
      {
         if (IsEnabled())
         {
            mSceneNode->addChild(mTargetTransform->GetOSGNode());
         }
         else
         {
            mSceneNode->removeChild(mTargetTransform->GetOSGNode());
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
   if (mAutoScale)
   {
      return GetScale() * GetCameraDistanceToTarget();
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
   mSnap = mRightMouse;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnRightMouseReleased(void)
{
   mRightMouse = false;
   mSnap = mRightMouse;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnMessage(MessageData *data)
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

////////////////////////////////////////////////////////////////////////////////
ObjectMotionModel::MotionType ObjectMotionModel::Update(osg::Vec2 pos)
{
   mMousePos = pos;

   if (GetTarget() && IsEnabled() && mTargetTransform.valid())
   {
      // Now update the widget highlights based on the new mouse position.
      // When the mouse is released, deselect our current arrow.
      if (!mLeftMouse && !mRightMouse)
      {
         if (mMouseLocked)
         {
            mAngleGeode->removeDrawable(mAngleDrawable.get());
            mAngleOriginGeode->removeDrawable(mAngleOriginDrawable.get());
         }

         mMouseDown     = false;
         mMouseLocked   = false;

         // Test for arrow hovering.
         UpdateWidgets();
         if (HighlightWidgets(MousePick()))
         {
            return mMotionType;
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
            UpdateWidgets();
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
                  mOriginAngle = 0.0f;
               }
               return mMotionType;
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
            UpdateWidgets();
            return mMotionType;
         }
      }
   }

   return mMotionType;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::UpdateWidgets(void)
{
   if (GetTarget() && IsEnabled() && mTargetTransform.valid())
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

      float scale = GetAutoScaleSize();
      transform.Rescale(osg::Vec3(scale, scale, scale));
      transform.SetTranslation(position);

      mTargetTransform->SetTransform(transform);
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

         osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
         stateSet->setAttribute(polygonMode, osg::StateAttribute::ON);

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

         osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
         stateSet->setAttribute(polygonMode, osg::StateAttribute::ON);

         osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL);
         stateSet->setAttribute(depth, osg::StateAttribute::ON);
      }

      if (groupNode)
      {
         groupNode->addChild(wireNode);
      }
   }

   float ringRadius             = 0.08f;
   float ringVisibleThickness   = 0.003f;
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
      osg::TriangleMesh* ring = GenerateRing(ringRadius - ringVisibleThickness, ringRadius + ringVisibleThickness, 40);
      osg::TriangleMesh* selectionRing = GenerateRing(ringRadius - ringSelectionThickness, ringRadius + ringSelectionThickness, 40);

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
   mAngleCylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, ringRadius * 0.5f), 0.001f, ringRadius);
   mAngleDrawable = new osg::ShapeDrawable(mAngleCylinder.get());

   mAngleTransform->GetOSGNode()->asGroup()->addChild(mAngleGeode.get());
   mTargetTransform->AddChild(mAngleTransform.get());

   mAngleOriginTransform = new dtCore::Transformable();
   mAngleOriginGeode = new osg::Geode();
   mAngleOriginCylinder = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, ringRadius * 0.5f), 0.001f, ringRadius);
   mAngleOriginDrawable = new osg::ShapeDrawable(mAngleOriginCylinder.get());

   mAngleOriginTransform->GetOSGNode()->asGroup()->addChild(mAngleOriginGeode.get());
   mTargetTransform->AddChild(mAngleOriginTransform.get());

   if (IsEnabled() && mSceneNode)
   {
      mSceneNode->addChild(mTargetTransform->GetOSGNode());
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

   SetArrowHighlight(ARROW_TYPE_MAX);
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
osg::Vec3 ObjectMotionModel::GetMouseVector(osg::Vec2 mousePos)
{
   if (!mCamera)
   {
      return osg::Vec3();
   }

   const osg::Camera* camera = mCamera->GetOSGCamera();
   osg::Matrix matrix = camera->getViewMatrix() * camera->getProjectionMatrix();

   const osg::Matrix inverse = osg::Matrix::inverse(matrix);
   osg::Vec3 startVertex = osg::Vec3(mousePos.x(), mousePos.y(), 0.0f) * inverse;

   dtCore::Transform transform;
   mCamera->GetTransform(transform);
   return startVertex - transform.GetTranslation();
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
   GetMouseLine(GetMousePosition(), startPoint, endPoint);

   osg::ref_ptr<osgUtil::LineSegmentIntersector> picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, startPoint, endPoint);

   osgUtil::IntersectionVisitor iv(picker.get());
   iv.setTraversalMask(ARROW_NODE_MASK);

   mTargetTransform->GetOSGNode()->accept(iv);

   osgUtil::LineSegmentIntersector::Intersections hitList;
   if (picker->containsIntersections())
   {
      hitList = picker->getIntersections();

      for (std::multiset<osgUtil::LineSegmentIntersector::Intersection>::const_iterator hitItr = hitList.begin();
         hitItr != hitList.end();
         ++hitItr)
      {
         for (osg::NodePath::const_reverse_iterator nodeItr = hitItr->nodePath.rbegin();
            nodeItr != hitItr->nodePath.rend();
            ++nodeItr)
         {
            osg::Node* node = (*nodeItr);
            for (int ArrowIndex = 0; ArrowIndex < ARROW_TYPE_MAX; ArrowIndex++)
            {
               if (node == mArrows[ArrowIndex].translationTransform->GetOSGNode())
               {
                  return mArrows[ArrowIndex].translationTransform.get();
               }
               else if (node == mArrows[ArrowIndex].rotationTransform->GetOSGNode())
               {
                  return mArrows[ArrowIndex].rotationTransform.get();
               }
            }
         }
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
{
   osg::Vec3 mouseVec = GetMouseVector(mousePos);
   mouseVec.normalize();

   dtCore::Transform transform;
   mCamera->GetTransform(transform);
   start = transform.GetTranslation();
   end = start + (mouseVec * (GetCameraDistanceToTarget() * 1.5f));
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 ObjectMotionModel::GetObjectScreenCoordinates(osg::Vec3 objectPos)
{
   if (!mCamera)
   {
      return osg::Vec2();
   }

   const osg::Camera* camera = mCamera->GetOSGCamera();
   osg::Matrix matrix = camera->getViewMatrix() * camera->getProjectionMatrix();

   osg::Vec3 screenPos = objectPos * matrix;
   return osg::Vec2(screenPos.x(), screenPos.y());
}

////////////////////////////////////////////////////////////////////////////////
bool ObjectMotionModel::HighlightWidgets(dtCore::DeltaDrawable* drawable)
{
   if (drawable)
   {
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

   for (int arrowIndex = 0; arrowIndex < ARROW_TYPE_MAX; arrowIndex++)
   {
      osg::Vec4 color = mArrows[arrowIndex].arrowCylinderColor;
      color.r() = color.r() * 0.5f;
      color.g() = color.g() * 0.5f;
      color.b() = color.b() * 0.5f;
      color.a() = color.a();

      mArrows[arrowIndex].rotationRing->setColor(color);
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

            if (mHoverArrow == (ArrowType)arrowIndex)
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
   osg::Vec3 axis;
   osg::Vec3* plane = NULL;

   osg::Vec3 targetUp;
   osg::Vec3 targetRight;
   osg::Vec3 targetAt;
   osg::Vec3 targetPos;

   osg::Vec3 camUp;
   osg::Vec3 camRight;
   osg::Vec3 camAt;
   //osg::Vec3 camPos;

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
   //camPos = camTransform.GetTranslation();

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
         plane1 = targetUp ^ axis;
         plane2 = targetRight ^ axis;
         break;
      }

   case ARROW_TYPE_RIGHT:
      {
         axis = targetRight;
         plane1 = targetUp ^ axis;
         plane2 = axis ^ targetAt;
         break;
      }

   case ARROW_TYPE_UP:
      {
         axis = targetUp;
         plane1 = axis ^ targetRight;
         plane2 = targetAt ^ axis;
         break;
      }
   default:
      {
         return;
      }
   }

   plane1.normalize();
   plane2.normalize();

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

   if (plane)
   {
      // Get the mouse vector.
      osg::Vec3 mouseStart, mouseEnd;
      GetMouseLine(GetMousePosition() + mMouseOffset, mouseStart, mouseEnd);
      osg::Vec3 mouse = mouseEnd - mouseStart;

      // Calculate the mouse collision in the 3D space relative to the plane
      // of the camera and the desired axis of the object.
      float fStartOffset   = mouseStart * (*plane);
      float fDistMod       = mouse      * (*plane);
      float fPlaneOffset   = targetPos  * (*plane);

      float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

      // Find the projected point of collision on the plane.
      osg::Vec3 projection = (mouse * fDistance) + mouseStart;
      osg::Vec3 vector     = projection - targetPos;

      // Find the translation vector.
      fDistance = axis * vector;

      // Snap
      if (mSnap)
      {
         fDistance = (int)fDistance / mSnapTranslation;
      }

      targetPos += axis * fDistance;

      OnTranslate(axis * fDistance);
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

   float fDistance      = (fPlaneOffset - fStartOffset) / fDistMod;

   // Find the projected point of collision on the plane.
   osg::Vec3 projection = (mouse * fDistance) + mouseStart;
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
   // TODO: Not currenlty implemented as there may be issues with scaling transformables.
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnTranslate(osg::Vec3 delta)
{
   dtCore::Transformable* target = GetTarget();
   
   if (target)
   {
      dtCore::Transform transform;
      target->GetTransform(transform);
      transform.SetTranslation(transform.GetTranslation() + delta);
      target->SetTransform(transform);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectMotionModel::OnRotate(float delta, osg::Vec3 axis)
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
void ObjectMotionModel::OnScale(osg::Vec3 delta)
{
}

////////////////////////////////////////////////////////////////////////////////
