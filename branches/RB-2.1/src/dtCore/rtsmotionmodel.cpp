#include <dtCore/rtsmotionmodel.h>

#include <dtCore/camera.h>
#include <dtCore/axistoaxistransformation.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtUtil/matrixutil.h>


////////////////////////////////////////////////////////////////////////////////
using namespace dtCore;

/**
 * Constructor.
 */
RTSMotionModel::RTSMotionModel( dtCore::Keyboard *keyboard
                              , dtCore::Mouse *mouse
                              , bool useSimTimeForSpeed )
   : dtCore::OrbitMotionModel(keyboard, mouse)
   , mTerrainDrawable(NULL)
   , mUseSimTimeForSpeed(useSimTimeForSpeed)
{
   AddSender(&dtCore::System::GetInstance());

   // set some parameters
   SetDistance(20.0f);

   // azimuth (left/right angle)
   SetAzimuthAxis(GetDefaultLogicalInputDevice()->AddAxis(
      "orbit vertical",
      new dtCore::ButtonAxisToAxis(
         keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Alt_L),
         mouse->GetAxis(0))
      ));
   // elevation (up/down angle)
   SetElevationAxis(GetDefaultLogicalInputDevice()->AddAxis(
      "orbit horizontal",
      new dtCore::ButtonAxisToAxis(
         keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Alt_L),
         mouse->GetAxis(1))
      ));

   SetDistanceAxis(GetDefaultLogicalInputDevice()->AddAxis(
      "mouse wheel camera zoom",
      new dtCore::AxisToAxis(
         mouse->GetAxis(2),
         0.05f,
         0.0f)));

   SetLeftRightTranslationAxis(GetDefaultLogicalInputDevice()->AddAxis(
      "translate left/right",
      new dtCore::AxesToAxis(
         GetDefaultLogicalInputDevice()->AddAxis(
            "left/right of screen translates left/right",
            new dtCore::AxisToAxisTransformation(
               mouse->GetAxis(0),
               new dtUtil::EdgeStepFilter(-0.9f, 0.9f))),
         GetDefaultLogicalInputDevice()->AddAxis(
            "a/d keys translate left/right",
            new dtCore::ButtonsToAxis(
               keyboard->GetButton('a'),
               keyboard->GetButton('d')))
      )));

   SetUpDownTranslationAxis(GetDefaultLogicalInputDevice()->AddAxis(
      "translate forward/back",
      new dtCore::AxesToAxis(
         GetDefaultLogicalInputDevice()->AddAxis(
            "top/bottom of screen translates forward/back",
            new dtCore::AxisToAxisTransformation(
               mouse->GetAxis(1),
               new dtUtil::EdgeStepFilter(-0.9f, 0.9f))),
         GetDefaultLogicalInputDevice()->AddAxis(
            "w/s keys translate forward/back",
            new dtCore::ButtonsToAxis(
               keyboard->GetButton('s'),
               keyboard->GetButton('w')))
      )));
}


bool RTSMotionModel::AxisStateChanged(const dtCore::Axis *axis, double oldState, double newState, double delta)
{
   if (!IsEnabled())
   {
      return false;
   }

   //printf("RTSMotionModel::AxisStateChanged(axis=%p, oldState=%g, newState=%g, delta=%g)\n", axis, oldState, newState, delta);
   bool rval = dtCore::OrbitMotionModel::AxisStateChanged(axis, oldState, newState, delta);

   // do value correction here

   // correct the pitch
   if (GetTarget())
   {
      dtCore::Transform transform;

      // get the current orientation
      osg::Vec3 hpr;
      GetTarget()->GetTransform(transform);
      transform.GetRotation(hpr);

      // clamp pitch
      if (hpr[1] > 0.0f)
      {
         hpr[1] = 0.0f;
      }

      // set rotation
      transform.SetRotation(hpr);
      GetTarget()->SetTransform(transform);
   }

   return rval;
}


void RTSMotionModel::OnMessage(MessageData *data)
{
   dtCore::Transformable *target = GetTarget();

   if (  target == 0
      || !IsEnabled()
      || (data->message != "preframe"
         //&& (mUseSimTimeForSpeed || data->message != "pause")
         )
      )
   {
      return;
   }

   //
   // Get the time change (sim time or real time)
   double delta;
   double *timeChange = (double *)data->userData;

   // paused and !useSimTime
   if (data->message == "pause")
   {
      // 0 is real time when paused
      delta = *timeChange;
   }
   else if (mUseSimTimeForSpeed)
   {
      // 0 is sim time
      delta = timeChange[0]; 
   }
   else
   {
      // 1 is real time
      delta = timeChange[1]; 
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

   {
      osg::Vec3 hpr_trans = hpr;
      osg::Vec3 translation
      (
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
   xyz[2] = 1.0f;

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
