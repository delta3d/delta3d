// ufomotionmodel.cpp: Implementation of the UFOMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/ufomotionmodel.h>

#include <dtCore/inputdevice.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(UFOMotionModel)

/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 */
UFOMotionModel::UFOMotionModel(Keyboard* keyboard,
                               Mouse* mouse)
   : MotionModel("UFOMotionModel")
   , mFlyForwardBackwardAxis(NULL)
   , mFlyLeftRightAxis(NULL)
   , mFlyUpDownAxis(NULL)
   , mTurnLeftRightAxis(NULL)
   , mMaximumFlySpeed(100.0f)
   , mMaximumTurnSpeed(90.0f)
{
   RegisterInstance(this);

   if (keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &UFOMotionModel::OnSystem);
}

/**
 * Destructor.
 */
UFOMotionModel::~UFOMotionModel()
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
void UFOMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if (!mDefaultInputDevice.valid())
   {
      mDefaultInputDevice = new LogicalInputDevice;

      Axis* leftButtonUpAndDown = mDefaultInputDevice->AddAxis(
         "left mouse button up/down",
         mLeftButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(Mouse::LeftButton),
            mouse->GetAxis(1)
         )
      );

      Axis* leftButtonLeftAndRight = mDefaultInputDevice->AddAxis(
         "left mouse button left/right",
         mLeftButtonLeftRightMapping = new ButtonAxisToAxis(
            mouse->GetButton(Mouse::LeftButton),
            mouse->GetAxis(0)
         )
      );

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

      Axis* wsKeysUpAndDown = mDefaultInputDevice->AddAxis(
         "w/s keys up/down",
         mWSKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton('s'),
            keyboard->GetButton('w')
         )
      );

      Axis* adKeysLeftAndRight = mDefaultInputDevice->AddAxis(
         "a/d keys left/right",
         mADKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton('a'),
            keyboard->GetButton('d')
         )
      );

      AxesToAxis* defaultFlyForwardBackwardMapping = new AxesToAxis(arrowKeysUpAndDown, leftButtonUpAndDown);
      AxesToAxis* defaultFlyLeftRightMapping = new AxesToAxis(arrowKeysLeftAndRight, leftButtonLeftAndRight);
      AxesToAxis* defaultFlyUpDownMapping = new AxesToAxis(wsKeysUpAndDown, rightButtonUpAndDown);
      AxesToAxis* defaultTurnLeftRightMapping = new AxesToAxis(adKeysLeftAndRight, rightButtonLeftAndRight);

      mDefaultFlyForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default fly forward/backward", defaultFlyForwardBackwardMapping);

      mDefaultFlyLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default fly left/right", defaultFlyLeftRightMapping);

      mDefaultFlyUpDownAxis = mDefaultInputDevice->AddAxis(
         "default fly up/down", defaultFlyUpDownMapping);

      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right", defaultTurnLeftRightMapping);

      mMiscAxisMappingList.push_back(defaultFlyForwardBackwardMapping);
      mMiscAxisMappingList.push_back(defaultFlyLeftRightMapping);
      mMiscAxisMappingList.push_back(defaultFlyUpDownMapping);
      mMiscAxisMappingList.push_back(defaultTurnLeftRightMapping);
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

      mADKeysLeftRightMapping->SetSourceButtons(
         keyboard->GetButton('a'),
         keyboard->GetButton('d')
      );
   }

   SetFlyForwardBackwardAxis(mDefaultFlyForwardBackwardAxis);

   SetFlyLeftRightAxis(mDefaultFlyLeftRightAxis);

   SetFlyUpDownAxis(mDefaultFlyUpDownAxis);

   SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis);
}

/**
 * Sets the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @param flyForwardBackwardAxis the new forward/backward axis
 */
void UFOMotionModel::SetFlyForwardBackwardAxis(Axis* flyForwardBackwardAxis)
{
   mFlyForwardBackwardAxis = flyForwardBackwardAxis;
}

/**
 * Returns the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @return the current forward/backward axis
 */
Axis* UFOMotionModel::GetFlyForwardBackwardAxis()
{
   return mFlyForwardBackwardAxis;
}

/**
 * Sets the axis that moves the target left (for negative
 * values) or right (for positive values).
 *
 * @param flyLeftRightAxis the new left/right axis
 */
void UFOMotionModel::SetFlyLeftRightAxis(Axis* flyLeftRightAxis)
{
   mFlyLeftRightAxis = flyLeftRightAxis;
}

/**
 * Returns the axis that moves the target left (for negative
 * values) or right (for positive values).
 *
 * @return the current left/right axis
 */
Axis* UFOMotionModel::GetFlyLeftRightAxis()
{
   return mFlyLeftRightAxis;
}

/**
 * Sets the axis that moves the target up (for positive
 * values) or down (for negative values).
 *
 * @param flyUpDownAxis the new up/down axis
 */
void UFOMotionModel::SetFlyUpDownAxis(Axis* flyUpDownAxis)
{
   mFlyUpDownAxis = flyUpDownAxis;
}

/**
 * Returns the axis that moves the target up (for positive
 * values) or down (for negative values).
 *
 * @return the current up/down axis
 */
Axis* UFOMotionModel::GetFlyUpDownAxis()
{
   return mFlyUpDownAxis;
}

/**
 * Sets the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @param turnLeftRightAxis the new turn left/right axis
 */
void UFOMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   mTurnLeftRightAxis = turnLeftRightAxis;
}

/**
 * Returns the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @return the current turn left/right axis
 */
Axis* UFOMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis;
}

/**
 * Sets the maximum fly speed (meters per second).
 *
 * @param maximumFlySpeed the new maximum fly speed
 */
void UFOMotionModel::SetMaximumFlySpeed(float maximumFlySpeed)
{
   mMaximumFlySpeed = maximumFlySpeed;
}

/**
 * Returns the maximum fly speed (meters per second).
 *
 * @return the current maximum fly speed
 */
float UFOMotionModel::GetMaximumFlySpeed()
{
   return mMaximumFlySpeed;
}

/**
 * Sets the maximum turn speed (degrees per second).
 *
 * @param maximumTurnSpeed the new maximum turn speed
 */
void UFOMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
   mMaximumTurnSpeed = maximumTurnSpeed;
}

/**
 * Returns the maximum turn speed (degrees per second).
 *
 * @return the current maximum turn speed
 */
float UFOMotionModel::GetMaximumTurnSpeed()
{
   return mMaximumTurnSpeed;
}

/**
 * Message handler callback.
 *
 * @param data the message data
 */
void UFOMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (GetTarget() != 0 &&
      IsEnabled() &&
      str == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL/*MESSAGE_PRE_FRAME*/)
   {

      Transform transform;

      GetTarget()->GetTransform(transform);

      osg::Vec3 xyz, hpr;

      transform.Get(xyz, hpr);

      if (mTurnLeftRightAxis != 0)
      {
         hpr[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * deltaSim);
      }

      hpr[1] = 0.0f;
      hpr[2] = 0.0f;

      transform.SetRotation(hpr);

      osg::Vec3 translation;

      if (mFlyForwardBackwardAxis != 0)
      {
         translation[1] =
            (float)(mFlyForwardBackwardAxis->GetState() * mMaximumFlySpeed * deltaSim);
      }

      if (mFlyLeftRightAxis != 0)
      {
         translation[0] =
            (float)(mFlyLeftRightAxis->GetState() * mMaximumFlySpeed * deltaSim);
      }

      if (mFlyUpDownAxis != 0)
      {
         translation[2] =
            (float)(mFlyUpDownAxis->GetState() * mMaximumFlySpeed * deltaSim);
      }

      osg::Matrix mat;

      transform.GetRotation(mat);

      translation = translation * mat;

      xyz += translation;

      transform.SetTranslation(xyz);

      GetTarget()->SetTransform(transform);
   }
}

} // namespace dtCore
