// flymotionmodel.cpp: Implementation of the FlyMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/flymotionmodel.h>

#include <osg/Vec3>
#include <osg/Matrix>

#include <dtCore/system.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/keyboard.h>
#include <dtCore/transform.h>

#include <dtUtil/bits.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(FlyMotionModel)

//////////////////////////////////////////////////////////////////////////
FlyMotionModel::FlyMotionModel(Keyboard* keyboard, Mouse* mouse, unsigned int options)
   : MotionModel("FlyMotionModel")
   , mRightButtonUpDownMapping(NULL)
   , mRightButtonLeftRightMapping(NULL)
   , mArrowKeysUpDownMapping(NULL)
   , mArrowKeysLeftRightMapping(NULL)
   , mWSKeysUpDownMapping(NULL)
   , mWSKeysUpDownMappingCaps(NULL)
   , mADKeysLeftRightMapping(NULL)
   , mADKeysLeftRightMappingCaps(NULL)
   , mQEKeysUpDownMapping(NULL)
   , mQEKeysUpDownMappingCaps(NULL)
   , mDefaultFlyForwardBackwardAxis(NULL)
   , mDefaultFlyLeftRightAxis(NULL)
   , mDefaultFlyUpDownAxis(NULL)
   , mDefaultTurnLeftRightAxis(NULL)
   , mDefaultTurnUpDownAxis(NULL)
   , mFlyForwardBackwardAxis(NULL)
   , mFlyLeftRightAxis(NULL)
   , mFlyUpDownAxis(NULL)
   , mTurnLeftRightAxis(NULL)
   , mTurnUpDownAxis(NULL)
   , mMaximumFlySpeed(40.0f)
   , mMaximumTurnSpeed(60.0f)
   , mMouse(mouse)
   , mOptions(options)
{

   RegisterInstance(this);

   if (keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &FlyMotionModel::OnSystem);
}

/**
 * Destructor.
 */
FlyMotionModel::~FlyMotionModel()
{
   DeregisterInstance(this);
}

/**
 * Sets the input axes to a set of default mappings for mouse
 * and keyboard.
 *
 * @param keyboard the keyboard instance
 * @param mouse the mouse instance
 */
void FlyMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if (!mDefaultInputDevice.valid())
   {
      mDefaultInputDevice = new LogicalInputDevice;

      Axis* leftButtonUpAndDown = NULL;
      Axis* leftButtonLeftAndRight = NULL;

      if (HasOption(OPTION_REQUIRE_MOUSE_DOWN))
      {
         leftButtonUpAndDown = mDefaultInputDevice->AddAxis(
            "left mouse button up/down",
            mLeftButtonUpDownMapping = new ButtonAxisToAxis(
               mouse->GetButton(Mouse::LeftButton),
               mouse->GetAxis(1))
         );

         leftButtonLeftAndRight = mDefaultInputDevice->AddAxis(
            "left mouse button left/right",
            mLeftButtonLeftRightMapping = new ButtonAxisToAxis(
               mouse->GetButton(Mouse::LeftButton),
               mouse->GetAxis(0)
            )
         );
      }
      else
      {
         AxisToAxis* leftButtonUpDownMapping = new AxisToAxis(mouse->GetAxis(1));
         AxisToAxis* leftButtonLeftRightMapping = new AxisToAxis(mouse->GetAxis(0));

         leftButtonUpAndDown = mDefaultInputDevice->AddAxis(
            "left mouse movement up/down", leftButtonUpDownMapping);

         leftButtonLeftAndRight = mDefaultInputDevice->AddAxis(
            "left mouse movement left/right", leftButtonLeftRightMapping);

         mMiscAxisMappingList.push_back(leftButtonUpDownMapping);
         mMiscAxisMappingList.push_back(leftButtonLeftRightMapping);
      }

      Axis* rightButtonUpAndDown = mDefaultInputDevice->AddAxis(
         "right mouse button up/down",
         mRightButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(Mouse::RightButton),
            mouse->GetAxis(1)
         )
      );

      Axis* rightButtonLeftAndRight = mDefaultInputDevice->AddAxis(
         "right mouse button left/right",
         mRightButtonLeftRightMapping = new ButtonAxisToAxis(
            mouse->GetButton(Mouse::RightButton),
            mouse->GetAxis(0)
         )
      );

      if (HasOption(OPTION_USE_CURSOR_KEYS))
      {
         Axis* arrowKeysUpAndDown = mDefaultInputDevice->AddAxis(
            "arrow keys up/down",
            mArrowKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Down),
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Up)
            )
            );

         Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
            "arrow keys left/right",
            mArrowKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Left),
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Right)
            )
            );

         AxesToAxis* defaultTurnLeftRightMapping = new AxesToAxis(arrowKeysLeftAndRight, leftButtonLeftAndRight);
         AxesToAxis* defaultTurnUpDownMapping = new AxesToAxis(arrowKeysUpAndDown, leftButtonUpAndDown);

         mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
            "default turn left/right", defaultTurnLeftRightMapping);

         mDefaultTurnUpDownAxis = mDefaultInputDevice->AddAxis(
            "default turn up/down", defaultTurnUpDownMapping);

         mMiscAxisMappingList.push_back(defaultTurnLeftRightMapping);
         mMiscAxisMappingList.push_back(defaultTurnUpDownMapping);
      }
      else
      {
         AxisToAxis* defaultTurnLeftRightMapping = new AxisToAxis(mouse->GetAxis(0));
         AxisToAxis* defaultTurnUpDownMapping = new AxisToAxis(mouse->GetAxis(1));

         mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
            "default turn left/right", defaultTurnLeftRightMapping);

         mDefaultTurnUpDownAxis = mDefaultInputDevice->AddAxis(
            "default turn up/down", defaultTurnUpDownMapping);

         mMiscAxisMappingList.push_back(defaultTurnLeftRightMapping);
         mMiscAxisMappingList.push_back(defaultTurnUpDownMapping);
      }

      Axis* wsKeysUpAndDown = mDefaultInputDevice->AddAxis(
         "w/s keys stafe forward/back",
         mWSKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton('s'),
            keyboard->GetButton('w')
         )
      );

      Axis* wsKeysUpAndDownCaps = mDefaultInputDevice->AddAxis(
         "w/s keys stafe forward/back",
         mWSKeysUpDownMappingCaps = new ButtonsToAxis(
            keyboard->GetButton('S'),
            keyboard->GetButton('W')
         )
      );

      Axis* adKeysStrafeLeftAndRight = mDefaultInputDevice->AddAxis(
         "a/d keys strafe left/right",
         mADKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton('a'),
            keyboard->GetButton('d')
         )
      );

      Axis* adKeysStrafeLeftAndRightCaps = mDefaultInputDevice->AddAxis(
         "a/d keys strafe left/right",
         mADKeysLeftRightMappingCaps = new ButtonsToAxis(
            keyboard->GetButton('A'),
            keyboard->GetButton('D')
         )
      );

      Axis* qeKeysFlyUpAndDown = mDefaultInputDevice->AddAxis(
         "q/e keys fly up/down",
         mQEKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton('q'),
            keyboard->GetButton('e')
         )
      );

      Axis* qeKeysFlyUpAndDownCaps = mDefaultInputDevice->AddAxis(
         "q/e keys fly up/down",
         mQEKeysUpDownMappingCaps = new ButtonsToAxis(
            keyboard->GetButton('Q'),
            keyboard->GetButton('E')
         )
      );

      dtCore::AxesToAxis* axesMapping = new AxesToAxis(wsKeysUpAndDown, wsKeysUpAndDownCaps);
      axesMapping->AddSourceAxis(rightButtonUpAndDown);
      mDefaultFlyForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default fly forward/backward", axesMapping);

      mMiscAxisMappingList.push_back(axesMapping);

      axesMapping = new AxesToAxis(adKeysStrafeLeftAndRight, adKeysStrafeLeftAndRightCaps);
      axesMapping->AddSourceAxis(rightButtonLeftAndRight);
      mDefaultFlyLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default fly left/right", axesMapping);

      mMiscAxisMappingList.push_back(axesMapping);

      AxesToAxis* defaultFlyUpDownMapping = new AxesToAxis(qeKeysFlyUpAndDown, qeKeysFlyUpAndDownCaps);

      mDefaultFlyUpDownAxis = mDefaultInputDevice->AddAxis(
         "default fly up/down", defaultFlyUpDownMapping);

      mMiscAxisMappingList.push_back(defaultFlyUpDownMapping);
   }
   else
   {
      mLeftButtonUpDownMapping->SetSourceButton(mouse->GetButton(Mouse::LeftButton));
      mLeftButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));

      mLeftButtonLeftRightMapping->SetSourceButton(mouse->GetButton(Mouse::LeftButton));
      mLeftButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));

      mRightButtonUpDownMapping->SetSourceButton(mouse->GetButton(Mouse::RightButton));
      mRightButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));

      mRightButtonLeftRightMapping->SetSourceButton(mouse->GetButton(Mouse::RightButton));
      mRightButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));

      mArrowKeysUpDownMapping->SetSourceButtons(
         keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Down),
         keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Up)
      );

      mArrowKeysLeftRightMapping->SetSourceButtons(
         keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Left),
         keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Right)
      );

      mWSKeysUpDownMapping->SetSourceButtons(
         keyboard->GetButton('s'),
         keyboard->GetButton('w')
      );

      mWSKeysUpDownMappingCaps->SetSourceButtons(
         keyboard->GetButton('S'),
         keyboard->GetButton('W')
      );

      mADKeysLeftRightMapping->SetSourceButtons(
         keyboard->GetButton('a'),
         keyboard->GetButton('d')
      );

      mADKeysLeftRightMappingCaps->SetSourceButtons(
         keyboard->GetButton('A'),
         keyboard->GetButton('D')
      );

      mQEKeysUpDownMapping->SetSourceButtons(
         keyboard->GetButton('q'),
         keyboard->GetButton('e')
      );

      mQEKeysUpDownMappingCaps->SetSourceButtons(
         keyboard->GetButton('Q'),
         keyboard->GetButton('E')
      );
   }

   SetFlyForwardBackwardAxis(mDefaultFlyForwardBackwardAxis);

   SetFlyLeftRightAxis(mDefaultFlyLeftRightAxis);

   SetFlyUpDownAxis(mDefaultFlyUpDownAxis);

   SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis);

   SetTurnUpDownAxis(mDefaultTurnUpDownAxis);
}

/**
 * Sets the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @param flyForwardBackwardAxis the new forward/backward axis
 */
void FlyMotionModel::SetFlyForwardBackwardAxis(Axis* flyForwardBackwardAxis)
{
   mFlyForwardBackwardAxis = flyForwardBackwardAxis;
}

/**
 * Returns the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @return the current forward/backward axis
 */
Axis* FlyMotionModel::GetFlyForwardBackwardAxis()
{
   return mFlyForwardBackwardAxis;
}

const Axis* FlyMotionModel::GetFlyForwardBackwardAxis() const
{
   return mFlyForwardBackwardAxis;
}

////////////////////////////////////////////////////////////////////////////
void FlyMotionModel::SetFlyLeftRightAxis(Axis* flyLeftRightAxis)
{
   mFlyLeftRightAxis = flyLeftRightAxis;
}

////////////////////////////////////////////////////////////////////////////
Axis* FlyMotionModel::GetFlyLeftRightAxis()
{
   return mFlyLeftRightAxis;
}

////////////////////////////////////////////////////////////////////////////
const Axis* FlyMotionModel::GetFlyLeftRightAxis() const
{
   return mFlyLeftRightAxis;
}

////////////////////////////////////////////////////////////////////////////
void FlyMotionModel::SetFlyUpDownAxis(Axis* flyUpDownAxis)
{
   mFlyUpDownAxis = flyUpDownAxis;
}

////////////////////////////////////////////////////////////////////////////
Axis* FlyMotionModel::GetFlyUpDownAxis()
{
   return mFlyUpDownAxis;
}

////////////////////////////////////////////////////////////////////////////
const Axis* FlyMotionModel::GetFlyUpDownAxis() const
{
   return mFlyUpDownAxis;
}

////////////////////////////////////////////////////////////////////////////

/**
 * Sets the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @param turnLeftRightAxis the new turn left/right axis
 */
void FlyMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   mTurnLeftRightAxis = turnLeftRightAxis;
}

/**
 * Returns the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @return the current turn left/right axis
 */
Axis* FlyMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis;
}

const Axis* FlyMotionModel::GetTurnLeftRightAxis() const
{
   return mTurnLeftRightAxis;
}

/**
 * Sets the axis that turns the target up (for positive values)
 * or down (for negative values).
 *
 * @param turnUpDownAxis the new turn up/down axis
 */
void FlyMotionModel::SetTurnUpDownAxis(Axis* turnUpDownAxis)
{
   mTurnUpDownAxis = turnUpDownAxis;
}

/**
 * Returns the axis that turns the target up (for positive values)
 * or down (for negative values).
 *
 * @return the current turn up/down axis
 */
Axis* FlyMotionModel::GetTurnUpDownAxis()
{
   return mTurnUpDownAxis;
}

const Axis* FlyMotionModel::GetTurnUpDownAxis() const
{
   return mTurnUpDownAxis;
}

/**
 * Sets the maximum fly speed (meters per second).
 *
 * @param maximumFlySpeed the new maximum fly speed
 */
void FlyMotionModel::SetMaximumFlySpeed(float maximumFlySpeed)
{
   mMaximumFlySpeed = maximumFlySpeed;
}

/**
 * Returns the maximum fly speed (meters per second).
 *
 * @return the current maximum fly speed
 */
float FlyMotionModel::GetMaximumFlySpeed() const
{
   return mMaximumFlySpeed;
}

/**
 * Sets the maximum turn speed (degrees per second).
 *
 * @param maximumTurnSpeed the new maximum turn speed
 */
void FlyMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
   mMaximumTurnSpeed = maximumTurnSpeed;
}

/**
 * Returns the maximum turn speed (degrees per second).
 *
 * @return the current maximum turn speed
 */
float FlyMotionModel::GetMaximumTurnSpeed() const
{
   return mMaximumTurnSpeed;
}

void FlyMotionModel::SetUseSimTimeForSpeed(bool useSimTimeForSpeed)
{
   if (useSimTimeForSpeed)
   {
      mOptions = dtUtil::Bits::Add(mOptions, OPTION_USE_SIMTIME_FOR_SPEED);
   }
   else
   {
      mOptions = dtUtil::Bits::Remove(mOptions, OPTION_USE_SIMTIME_FOR_SPEED);
   }
}

/**
 * Message handler callback.
 *
 * @param data the message data
 */
void FlyMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (GetTarget() != NULL &&
      IsEnabled() &&
      (str == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL/*MESSAGE_PRE_FRAME*/) &&
      // don't move if paused & using simtime for speed (since simtime will be 0 if paused)
      (!HasOption(OPTION_USE_SIMTIME_FOR_SPEED) || !System::GetInstance().GetPause()))
   {
      // Get the time change (sim time or real time)
      double delta = GetTimeDelta(deltaSim, deltaReal);

      Transform transform;

      GetTarget()->GetTransform(transform);

      osg::Vec3 xyz, hpr;

      transform.Get(xyz, hpr);

      // rotation

      bool rotationChanged = false;
      hpr = Rotate(hpr, delta, rotationChanged);
      if (rotationChanged)
      {
         transform.SetRotation(hpr);
      }

      // translation

      bool translationChanged = false;
      xyz = Translate(xyz, delta, translationChanged);
      if (translationChanged)
      {
         transform.SetTranslation(xyz);
      }

      // finalize changes

      if (rotationChanged || translationChanged)
      {
         GetTarget()->SetTransform(transform);
      }
   }
}

double FlyMotionModel::GetTimeDelta(double dtSim, double dtReal) const
{
   double delta = dtReal;
   if (HasOption(OPTION_USE_SIMTIME_FOR_SPEED) && !System::GetInstance().GetPause() )
   {
      delta = dtSim;
   }

   return delta;
}

osg::Vec3 FlyMotionModel::Rotate(const osg::Vec3& hpr, double delta, bool& changed) const
{
   osg::Vec3 out = hpr;

   changed = false;

   if (mTurnLeftRightAxis != NULL && mTurnLeftRightAxis->GetState() != 0.0)
   {
      out[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * delta);
      changed = true;
   }

   if (mTurnUpDownAxis != NULL && mTurnUpDownAxis->GetState() != 0.0)
   {
      float rotateTo = out[1] + float(mTurnUpDownAxis->GetState() * mMaximumTurnSpeed * delta);

      if (rotateTo < -89.5f)
      {
         out[1] = -89.5f;
      }
      else if (rotateTo > 89.5f)
      {
         out[1] = 89.5f;
      }
      else
      {
         out[1] = rotateTo;
      }
      changed = true;
   }


   if (HasOption(OPTION_RESET_MOUSE_CURSOR))
   {
      // fix to avoid camera drift
      mTurnUpDownAxis->SetState(0.0f); // necessary to stop camera drifting down
      mTurnLeftRightAxis->SetState(0.0f); // necessary to stop camera drifting left

      mMouse->SetPosition(0.0f,0.0f); // keeps cursor at center of screen
   }

   // allow for one degree of error.  Assigning it to 1 or -1 as opposed
   // to 0.0, keeps the camera from jerking when it corrects.
   // One degree of being off isn't very noticeable, and it keeps the accumulated error
   // of getting and setting the transform down.
   if (out[2] > 1.0 )
   {
      out[2] = 1.0f;
      changed = true;
   }
   else if (out[2] < -1.0)
   {
      out[2] = -1.0f;
      changed = true;
   }

   if (!changed)
   {
      return hpr;
   }

   return out;
}

osg::Vec3 FlyMotionModel::Translate(const osg::Vec3& xyz, double delta, bool& changed) const
{
   osg::Vec3 translation;

   changed = false;

   if (mFlyForwardBackwardAxis != NULL && mFlyForwardBackwardAxis->GetState() != 0.0)
   {
      translation[1] = float(mFlyForwardBackwardAxis->GetState() * mMaximumFlySpeed * delta);
      changed = true;
   }

   if (mFlyLeftRightAxis != NULL && mFlyLeftRightAxis->GetState() != 0.0)
   {
      translation[0] = float(mFlyLeftRightAxis->GetState() * mMaximumFlySpeed * delta);
      changed = true;
   }

   if (mFlyUpDownAxis != NULL && mFlyUpDownAxis->GetState() != 0.0)
   {
      translation[2] = float(mFlyUpDownAxis->GetState() * mMaximumFlySpeed * delta);
      changed = true;
   }

   if (!changed)
   {
      return xyz;
   }

   // rotate
   {
      Transform transform;
      GetTarget()->GetTransform(transform);
      osg::Matrix mat;

      transform.GetRotation(mat);

      translation = osg::Matrix::transform3x3(translation, mat);
   }

   osg::Vec3 out = xyz + translation;

   return out;
}

//////////////////////////////////////////////////////////////////////////
bool FlyMotionModel::HasOption(unsigned int option) const
{
   return dtUtil::Bits::Has(mOptions, option);
}
