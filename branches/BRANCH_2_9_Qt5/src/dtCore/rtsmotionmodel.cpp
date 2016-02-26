#include <prefix/dtcoreprefix.h>
#include <dtCore/rtsmotionmodel.h>

#include <dtCore/axistoaxistransformation.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtUtil/matrixutil.h>
#include <osg/Node>

#include <algorithm> //needed for std::max and std::min in VS2013

////////////////////////////////////////////////////////////////////////////////
namespace dtCore
{

   /**
    * Constructor.
    */
   RTSMotionModel::RTSMotionModel( dtCore::Keyboard *keyboard
         , dtCore::Mouse *mouse
         , bool useSimTimeForSpeed
         , bool useMouseScrolling )
   : dtCore::OrbitMotionModel(keyboard, mouse)
   , mTerrainDrawable(NULL)
   , mUseSimTimeForSpeed(useSimTimeForSpeed)
   , mShouldTranslate(true)
   , mMinGroundHeight(1.0f)
   {
      dtCore::System::GetInstance().TickSignal.connect_slot(this, &RTSMotionModel::OnSystem);

      // set some parameters
      SetDistance(20.0f);

      // JPH: This makes sure the camera does not rotate towards the sky.
      SetElevationMaxLimit(0.0f);

      dtCore::ButtonAxisToAxis* azimuthMapping = new dtCore::ButtonAxisToAxis(
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Alt_L), mouse->GetAxis(0));

      dtCore::ButtonAxisToAxis* elevationMapping = new dtCore::ButtonAxisToAxis(
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Alt_L), mouse->GetAxis(1));

      mMiscAxisMappingList.push_back(azimuthMapping);
      mMiscAxisMappingList.push_back(elevationMapping);

      // azimuth (left/right angle)
      SetAzimuthAxis(GetDefaultLogicalInputDevice()->AddAxis(
            "orbit vertical",azimuthMapping));

      // elevation (up/down angle)
      SetElevationAxis(GetDefaultLogicalInputDevice()->AddAxis(
            "orbit horizontal", elevationMapping));

      dtCore::DeltaButtonsToAxis* distanceButtonMapping = new dtCore::DeltaButtonsToAxis(
            keyboard->GetButton('q'), keyboard->GetButton('e'), -0.05f, 0.05f);

      dtCore::AxisToAxis* distanceMouseMapping = new dtCore::AxisToAxis(mouse->GetAxis(2), 0.05f, 0.0f);

      dtCore::AxesToAxis* distanceMapping =  new dtCore::AxesToAxis(
            GetDefaultLogicalInputDevice()->AddAxis("mouse wheel camera zoom", distanceMouseMapping),
            GetDefaultLogicalInputDevice()->AddAxis("q/e keys zoom in/out", distanceButtonMapping));

      mMiscAxisMappingList.push_back(distanceButtonMapping);
      mMiscAxisMappingList.push_back(distanceMouseMapping);
      mMiscAxisMappingList.push_back(distanceMapping);

      SetDistanceAxis(GetDefaultLogicalInputDevice()->AddAxis(
            "translate left/right", distanceMapping));

      if (useMouseScrolling)
      {
         dtCore::ButtonsToAxis* leftRightButtonMapping = new dtCore::ButtonsToAxis(
               keyboard->GetButton('a'),
               keyboard->GetButton('d'));

         dtUtil::EdgeStepFilter* filter = new dtUtil::EdgeStepFilter(-0.9f, 0.9f);

         dtCore::AxisToAxisTransformation* leftRightAxisTransformationMapping = new dtCore::AxisToAxisTransformation(
               mouse->GetAxis(0), filter);

         dtCore::AxesToAxis* leftRightTranslationMapping = new dtCore::AxesToAxis(
               GetDefaultLogicalInputDevice()->AddAxis("left/right of screen translates left/right", leftRightAxisTransformationMapping),
               GetDefaultLogicalInputDevice()->AddAxis("a/d keys translate left/right", leftRightButtonMapping));

         mMiscAxisMappingList.push_back(leftRightButtonMapping);
         mMiscAxisMappingList.push_back(leftRightAxisTransformationMapping);
         mMiscAxisMappingList.push_back(leftRightTranslationMapping);

         SetLeftRightTranslationAxis(GetDefaultLogicalInputDevice()->AddAxis(
               "translate left/right", leftRightTranslationMapping));

         dtCore::ButtonsToAxis* forwardBackButtonMapping = new dtCore::ButtonsToAxis(
               keyboard->GetButton('s'), keyboard->GetButton('w'));

         dtUtil::EdgeStepFilter* upDownFilter = new dtUtil::EdgeStepFilter(-0.9f, 0.9f);

         dtCore::AxisToAxisTransformation* forwardBackwardAxisTransformMapping = new dtCore::AxisToAxisTransformation(
               mouse->GetAxis(1), upDownFilter);

         dtCore::AxesToAxis* forwardBackTranslationMapping = new dtCore::AxesToAxis(
               GetDefaultLogicalInputDevice()->AddAxis("top/bottom of screen translates forward/back", forwardBackwardAxisTransformMapping),
               GetDefaultLogicalInputDevice()->AddAxis("w/s keys translate forward/back", forwardBackButtonMapping));

         mMiscAxisMappingList.push_back(forwardBackButtonMapping);
         mMiscAxisMappingList.push_back(forwardBackwardAxisTransformMapping);
         mMiscAxisMappingList.push_back(forwardBackTranslationMapping);

         SetUpDownTranslationAxis(GetDefaultLogicalInputDevice()->AddAxis(
               "translate forward/back", forwardBackTranslationMapping));
      }
      else
      {
         dtCore::ButtonsToAxis* leftRightMapping = new dtCore::ButtonsToAxis(
               keyboard->GetButton('a'), keyboard->GetButton('d'));

         SetLeftRightTranslationAxis(GetDefaultLogicalInputDevice()->AddAxis(
               "a/d keys translate left/right", leftRightMapping));

         dtCore::ButtonsToAxis* forwardBackMapping = new dtCore::ButtonsToAxis(
               keyboard->GetButton('s'), keyboard->GetButton('w'));

         SetUpDownTranslationAxis(GetDefaultLogicalInputDevice()->AddAxis(
               "w/s keys translate forward/back", forwardBackMapping));

         mMiscAxisMappingList.push_back(leftRightMapping);
         mMiscAxisMappingList.push_back(forwardBackMapping);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RTSMotionModel::~RTSMotionModel()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////

   bool RTSMotionModel::HandleAxisStateChanged(const dtCore::Axis *axis, double oldState, double newState, double delta)
   {
      if (!IsEnabled())
      {
         return false;
      }

      //printf("RTSMotionModel::HandleAxisStateChanged(axis=%p, oldState=%g, newState=%g, delta=%g)\n", axis, oldState, newState, delta);
      bool rval = dtCore::OrbitMotionModel::HandleAxisStateChanged(axis, oldState, newState, delta);

      // do value correction here

      // correct the pitch
      // JPH: Removed this algorithm here because it is already handled
      // JPH: by the Orbit Motion Model by adjusting the elevation max limit.
      // JPH: It also works better because it doesn't force the camera to
      // JPH: translate down (Z axis) when you try to go beyond the limit.
      //if (GetTarget())
      //{
      //   dtCore::Transform transform;

      //   // get the current orientation
      //   osg::Vec3 hpr;
      //   GetTarget()->GetTransform(transform);
      //   transform.GetRotation(hpr);

      //   // clamp pitch
      //   if (hpr[1] > 0.0f)
      //   {
      //      hpr[1] = 0.0f;
      //   }

      //   // set rotation
      //   transform.SetRotation(hpr);
      //   GetTarget()->SetTransform(transform);
      //}

      return rval;
   }

   ////////////////////////////////////////////////////////////////////////////////

   void RTSMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

   {
      dtCore::Transformable *target = GetTarget();

      if (  target == NULL
            || !IsEnabled()
            || (str != dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL/*MESSAGE_PRE_FRAME*/
                  // Note - someone commented out the paused stuff below - this model does NOTHING while paused.
                  //&& (mUseSimTimeForSpeed || str != "pause")
            )
      )
      {
         return;
      }

      //
      // Get the time change (sim time or real time)
      double delta = deltaReal;

      // if we want the sim time and aren't paused, then use it.
      if (mUseSimTimeForSpeed && str != dtCore::System::MESSAGE_PAUSE)
      {
         delta = deltaSim;
      }

      // get the current position and orientation
      osg::Vec3 xyz, hpr;
      float height;
      {
         dtCore::Transform transform;
         GetTarget()->GetTransform(transform);
         transform.Get(xyz, hpr);
         height = xyz[2];
      }

      // let's work in terms of the focal point instead
      xyz = GetFocalPoint();

      if (mShouldTranslate)
      {
         osg::Vec3 hpr_trans = hpr;
         osg::Vec3 translation(
               float(GetLeftRightTranslationAxis()->GetState()),
               float(GetUpDownTranslationAxis()->GetState()),
               0.0f
         );

         // actually, let's not let pitch be a factor
         hpr_trans[1] = 0.0f;

         osg::Matrix mat;

         dtUtil::MatrixUtil::HprToMatrix(mat, hpr_trans);

         translation = osg::Matrix::transform3x3(translation, mat);

         // scale
         //const float speed = GetLinearRate();
         //const float speed = 0.05f * height + 0.5f;
         const float speed = 1.0f * height + 10.0f;
         translation *= delta;
         //translation *= GetDistance();
         translation *= speed;

         // apply
         xyz += translation;
      }

      // clamp z to 1 meter off the ground
      xyz[2] = std::max(mMinGroundHeight, xyz[2]);

      // clamp position to bounding terrain sphere
      if (mTerrainDrawable.valid())
      {
         const osg::Node *node = mTerrainDrawable->GetOSGNode();

         osg::BoundingSphere bs = node->getBound();

         osg::Vec3 terrain_center = bs.center();
         float     radius         = bs.radius();

         if ((xyz - terrain_center).length() > radius)
         {
            osg::Vec3 dir = (xyz - terrain_center);
            dir.normalize();
            xyz = terrain_center + dir * radius;
         }
      }

      // set back
      SetFocalPoint(xyz);
   }

   ////////////////////////////////////////////////////////////////////////////////
}
