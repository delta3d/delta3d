// ufomotionmodel.cpp: Implementation of the UFOMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/ufomotionmodel.h>

#include <dtCore/inputdevice.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/motionmodel.h>
#include <dtCore/mouse.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>

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
   : MotionModel("UFOMotionModel"),
     mFlyForwardBackwardAxis(0),
     mFlyLeftRightAxis(0),
     mFlyUpDownAxis(0),
     mTurnLeftRightAxis(0),
     mMaximumFlySpeed(100.0f),
     mMaximumTurnSpeed(90.0f)
{
   RegisterInstance(this);
   
   if(keyboard != 0 && mouse != 0)
   {
      SetDefaultMappings(keyboard, mouse);
   }
   
   AddSender(System::Instance());
}

/**
 * Destructor.
 */
UFOMotionModel::~UFOMotionModel()
{
   RemoveSender(System::Instance());
   
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
   if(!mDefaultInputDevice.valid())
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
            keyboard->GetButton(Producer::Key_Down),
            keyboard->GetButton(Producer::Key_Up)
         )
      );
         
      Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
         "arrow keys left/right",
         mArrowKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton(Producer::Key_Left),
            keyboard->GetButton(Producer::Key_Right)
         )
      );
      
      Axis* wsKeysUpAndDown = mDefaultInputDevice->AddAxis(
         "w/s keys up/down",
         mWSKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton(Producer::Key_S),
            keyboard->GetButton(Producer::Key_W)
         )
      );
      
      Axis* adKeysLeftAndRight = mDefaultInputDevice->AddAxis(
         "a/d keys left/right",
         mADKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton(Producer::Key_A),
            keyboard->GetButton(Producer::Key_D)
         )
      );
      
      mDefaultFlyForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default fly forward/backward",
         new AxesToAxis(arrowKeysUpAndDown, leftButtonUpAndDown)
      );
      
      mDefaultFlyLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default fly left/right",
         new AxesToAxis(arrowKeysLeftAndRight, leftButtonLeftAndRight)
      );
         
      mDefaultFlyUpDownAxis = mDefaultInputDevice->AddAxis(
         "default fly up/down",
         new AxesToAxis(wsKeysUpAndDown, rightButtonUpAndDown)
      );
      
      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right",
         new AxesToAxis(adKeysLeftAndRight, rightButtonLeftAndRight)
      );
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
         keyboard->GetButton(Producer::Key_Down),
         keyboard->GetButton(Producer::Key_Up)
      );
      
      mArrowKeysLeftRightMapping->SetSourceButtons(
         keyboard->GetButton(Producer::Key_Left),
         keyboard->GetButton(Producer::Key_Right)
      );
      
      mWSKeysUpDownMapping->SetSourceButtons(
         keyboard->GetButton(Producer::Key_S),
         keyboard->GetButton(Producer::Key_W)
      );
      
      mADKeysLeftRightMapping->SetSourceButtons(
         keyboard->GetButton(Producer::Key_A),
         keyboard->GetButton(Producer::Key_D)
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
void UFOMotionModel::OnMessage(MessageData *data)
{
   if(GetTarget() != 0 &&
      IsEnabled() && 
      data->message == "preframe")
   {
      const double dtCore = *static_cast<const double*>(data->userData);
      
      Transform transform;
      
      GetTarget()->GetTransform(transform);
      
      osg::Vec3 xyz, hpr, scale;
      
      transform.Get(xyz, hpr, scale);
      
      if(mTurnLeftRightAxis != 0)
      {
         hpr[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * dtCore);
      }
      
      hpr[1] = 0.0f;
      hpr[2] = 0.0f;
      
      transform.SetRotation(hpr);
      
      osg::Vec3 translation;
      
      if(mFlyForwardBackwardAxis != 0)
      {
         translation[1] = 
            (float)(mFlyForwardBackwardAxis->GetState() * mMaximumFlySpeed * dtCore);
      }
      
      if(mFlyLeftRightAxis != 0)
      {
         translation[0] = 
            (float)(mFlyLeftRightAxis->GetState() * mMaximumFlySpeed * dtCore);
      }
      
      if(mFlyUpDownAxis != 0)
      {
         translation[2] = 
            (float)(mFlyUpDownAxis->GetState() * mMaximumFlySpeed * dtCore);
      }
      
      osg::Matrix mat;
      
      transform.GetRotation(mat);
      
      translation = translation * mat;
      
      xyz += translation;
      
      transform.SetTranslation(xyz);
      
      GetTarget()->SetTransform(transform);  
   }
}

}
