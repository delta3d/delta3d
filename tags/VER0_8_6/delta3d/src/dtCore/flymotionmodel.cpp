// flymotionmodel.cpp: Implementation of the FlyMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/flymotionmodel.h"
#include "dtCore/scene.h"

using namespace dtCore;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(FlyMotionModel)


/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or NULL to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or NULL to avoid
 * creating default input mappings
 */
FlyMotionModel::FlyMotionModel(Keyboard* keyboard,
                               Mouse* mouse)
   : MotionModel("FlyMotionModel"),
     mFlyForwardBackwardAxis(NULL),
     mTurnLeftRightAxis(NULL),
     mTurnUpDownAxis(NULL),
     mMaximumFlySpeed(100.0f),
     mMaximumTurnSpeed(90.0f)
{
   RegisterInstance(this);
   
   if(keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }
   
   AddSender(System::GetSystem());
}

/**
 * Destructor.
 */
FlyMotionModel::~FlyMotionModel()
{
   RemoveSender(System::GetSystem());
   
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
   if(mDefaultInputDevice.get() == NULL)
   {
      mDefaultInputDevice = new LogicalInputDevice;
      
      Axis* leftButtonUpAndDown = mDefaultInputDevice->AddAxis(
         "left mouse button up/down",
         mLeftButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(LeftButton),
            mouse->GetAxis(1)
         )
      );
      
      Axis* leftButtonLeftAndRight = mDefaultInputDevice->AddAxis(
         "left mouse button left/right",
         mLeftButtonLeftRightMapping = new ButtonAxisToAxis(
            mouse->GetButton(LeftButton),
            mouse->GetAxis(0)
         )
      );
   
      Axis* rightButtonUpAndDown = mDefaultInputDevice->AddAxis(
         "right mouse button up/down",
         mRightButtonUpDownMapping = new ButtonAxisToAxis(
            mouse->GetButton(RightButton),
            mouse->GetAxis(1)
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
      
      mDefaultFlyForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default fly forward/backward",
         new AxesToAxis(wsKeysUpAndDown, rightButtonUpAndDown)
      );
      
      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right",
         new AxesToAxis(arrowKeysLeftAndRight, leftButtonLeftAndRight)
      );
         
      mDefaultTurnUpDownAxis = mDefaultInputDevice->AddAxis(
         "default turn up/down",
         new AxesToAxis(arrowKeysUpAndDown, leftButtonUpAndDown)
      );
   }
   else
   {
      mLeftButtonUpDownMapping->SetSourceButton(mouse->GetButton(LeftButton));
      mLeftButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
      mLeftButtonLeftRightMapping->SetSourceButton(mouse->GetButton(LeftButton));
      mLeftButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
      
      mRightButtonUpDownMapping->SetSourceButton(mouse->GetButton(RightButton));
      mRightButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
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
   }
   
   SetFlyForwardBackwardAxis(mDefaultFlyForwardBackwardAxis);
      
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
float FlyMotionModel::GetMaximumFlySpeed()
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
float FlyMotionModel::GetMaximumTurnSpeed()
{
   return mMaximumTurnSpeed;
}

/**
 * Message handler callback.
 *
 * @param data the message data
 */
void FlyMotionModel::OnMessage(MessageData *data)
{
   if(GetTarget() != NULL &&
      IsEnabled() && 
      data->message == "preframe")
   {
      double dtCore = *(double*)data->userData;
      
      Transform transform;
      
      GetTarget()->GetTransform(&transform);
      
      sgVec3 xyz, hpr;
      
      transform.Get(xyz, hpr);
      
      if(mTurnLeftRightAxis != NULL)
      {
         hpr[0] -= 
            (float)(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * dtCore);
      }
      
      if(mTurnUpDownAxis != NULL)
      {
         hpr[1] +=
            (float)(mTurnUpDownAxis->GetState() * mMaximumTurnSpeed * dtCore);
      }
      
      hpr[2] = 0.0f;
      
      transform.SetRotation(hpr);
      
      sgVec3 translation = {0, 0, 0};
      
      if(mFlyForwardBackwardAxis != NULL)
      {
         translation[1] = 
            (float)(mFlyForwardBackwardAxis->GetState() * mMaximumFlySpeed * dtCore);
      }
      
      sgMat4 mat;
      
      transform.GetRotation(mat);
      
      sgXformVec3(translation, mat);
      
      sgAddVec3(xyz, translation);
      
      transform.SetTranslation(xyz);
      
      GetTarget()->SetTransform(&transform);  
   }
}