/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2006 MOVES Institute
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Bradley Anderegg and Chris Darken 08/21/2006
*/

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/collisionmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>


namespace dtCore
{

   CollisionMotionModel::FPSAxisListener::FPSAxisListener(const SetFunctor& setFunc):
mSetFunctor(setFunc)
{
}

///When the axis changes, just call the functor with the new values
bool CollisionMotionModel::FPSAxisListener::AxisStateChanged(const Axis* axis,
                                                             double oldState, 
                                                             double newState, 
                                                             double delta)
{
   return mSetFunctor(newState, delta);
}

IMPLEMENT_MANAGEMENT_LAYER(CollisionMotionModel);

/**
* Constructor.
*
* @param keyboard the keyboard instance, or 0 to
* avoid creating default input mappings
* @param mouse the mouse instance, or 0 to avoid
* creating default input mappings
*/
CollisionMotionModel::CollisionMotionModel(float pHeight, float pRadius, float k, float theta, dtCore::Scene* pScene, Keyboard* keyboard, Mouse* mouse)
      : MotionModel("CollisionMotionModel")
      , mLeftRightMouseMovement(NULL)
      , mUpDownMouseMovement(NULL)
      , mArrowKeysUpDownMapping(NULL)
      , mArrowKeysLeftRightMapping(NULL)
      , mADKeysLeftRightMapping(NULL)
      , mDefaultWalkForwardBackwardAxis(NULL)
      , mDefaultTurnLeftRightAxis(NULL)
      , mDefaultLookUpDownAxis(NULL)
      , mDefaultSidestepLeftRightAxis(NULL)
      , mWalkForwardBackwardAxis(NULL)
      , mTurnLeftRightAxis(NULL)
      , mLookUpDownAxis(NULL)
      , mSidestepLeftRightAxis(NULL)
      , mSidestepListener(NULL)
      , mForwardBackwardListener(NULL)
      , mLookLeftRightListener(NULL)
      , mLookUpDownListener(NULL)
      , mMaximumWalkSpeed(3.0f)
      , mMaximumTurnSpeed(10000.0f)
      , mMouse(mouse)
      , mKeyboard(keyboard)
      , mCollider(pHeight, pRadius, k, theta, pScene)
      , mForwardBackCtrl(0.0f)
      , mSidestepCtrl(0.0f)
      , mLookLeftRightCtrl(0.0f)
      , mLookUpDownCtrl(0.0f)
      , mUseMouseButtons(true)
      , mCanJump(true)
{

   //setup some axis listeners with functors 
   FPSAxisListener::SetFunctor fbFunc(this, &CollisionMotionModel::OnForwardBackwardChanged);
   FPSAxisListener::SetFunctor sideStepFunc(this, &CollisionMotionModel::OnSidestepChanged);  
   FPSAxisListener::SetFunctor lookLeftRightFunc(this, &CollisionMotionModel::OnLookLeftRightChanged);
   FPSAxisListener::SetFunctor lookUpDownFunc(this, &CollisionMotionModel::OnLookUpDownChanged);

   mLookUpDownListener = new FPSAxisListener( lookUpDownFunc );
   mLookLeftRightListener = new FPSAxisListener( lookLeftRightFunc );
   mSidestepListener = new FPSAxisListener( sideStepFunc );
   mForwardBackwardListener = new FPSAxisListener( fbFunc );

   if(keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   mMouse = mouse;

   AddSender(&System::GetInstance());

   RegisterInstance(this);
}

/**
* Destructor.
*/
CollisionMotionModel::~CollisionMotionModel()
{
   DeregisterInstance(this);
   RemoveSender(&System::GetInstance());

   mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
   mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
   mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
   mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);

   delete mLookUpDownListener;
   delete mLookLeftRightListener;
   delete mSidestepListener;
   delete mForwardBackwardListener;
}

/**
* Enables or disables this motion model.
*
* @param enabled true to enable this motion model, false
* to disable it
*/
void CollisionMotionModel::SetEnabled(bool enabled)
{
   if(enabled && !MotionModel::IsEnabled())
   {
      mMouse->SetPosition(0.0f,0.0f);

      mLookUpDownAxis->AddAxisListener(mLookUpDownListener);
      mTurnLeftRightAxis->AddAxisListener(mLookLeftRightListener);
      mSidestepLeftRightAxis->AddAxisListener(mSidestepListener);
      mWalkForwardBackwardAxis->AddAxisListener(mForwardBackwardListener);
   }
   else
   {
      mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
      mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
      mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
      mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
   }

   MotionModel::SetEnabled(enabled);
}

/**
* Sets the input axes to a set of default mappings for mouse
* and keyboard.
*
* @param keyboard the keyboard instance
* @param mouse the mouse instance
*/
void CollisionMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if(!mDefaultInputDevice.valid())
   {
      mDefaultInputDevice = new LogicalInputDevice("FPSLogicalInputDevice");

      Axis* leftRightMouseMovement = mDefaultInputDevice->AddAxis(
         "left/right mouse movement",
         mLeftRightMouseMovement = new AxisToAxis(mouse->GetAxis(0)));

      Axis* upDownMouseMovement = mDefaultInputDevice->AddAxis(
         "up/down mouse movement",
         mUpDownMouseMovement = new AxisToAxis(mouse->GetAxis(1)));

      Axis* arrowKeysUpAndDown = mDefaultInputDevice->AddAxis(
         "arrow keys up/down",
         mArrowKeysUpDownMapping = new ButtonsToAxis(
         keyboard->GetButton(Producer::Key_S),
         keyboard->GetButton(Producer::Key_W)
         )
         );
      Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
         "arrow keys left/right",
         mArrowKeysLeftRightMapping = new ButtonsToAxis(
         keyboard->GetButton(Producer::Key_A),
         keyboard->GetButton(Producer::Key_D)
         )
         );

      mDefaultWalkForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default walk forward/backward",
         new AxesToAxis(arrowKeysUpAndDown)
         );

      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right",
         new AxesToAxis(leftRightMouseMovement)
         );

      mDefaultLookUpDownAxis = mDefaultInputDevice->AddAxis(
         "default look up/down",
         new AxesToAxis(upDownMouseMovement)
         );

      mDefaultSidestepLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default sidestep left/right",
         new AxesToAxis(arrowKeysLeftAndRight)
         );
   }

   SetWalkForwardBackwardAxis(mDefaultWalkForwardBackwardAxis);

   SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis);

   SetLookUpDownAxis(mDefaultLookUpDownAxis);

   SetSidestepLeftRightAxis(mDefaultSidestepLeftRightAxis);
}

/**
* Sets the axis that moves the target forwards (for positive
* values) or backwards (for negative values).
*
* @param walkForwardBackwardAxis the new forward/backward axis
*/
void CollisionMotionModel::SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis)
{
   if (mWalkForwardBackwardAxis) 
   {
      mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
   }

   mWalkForwardBackwardAxis = walkForwardBackwardAxis;

   mWalkForwardBackwardAxis->AddAxisListener(mForwardBackwardListener);
}

/**
* Returns the axis that moves the target forwards (for positive
* values) or backwards (for negative values).
*
* @return the current forward/backward axis
*/
Axis* CollisionMotionModel::GetWalkForwardBackwardAxis()
{
   return mWalkForwardBackwardAxis;
}

/**
* Sets the axis that turns the target left (for negative values)
* or right (for positive values).
*
* @param turnLeftRightAxis the new turn left/right axis
*/
void CollisionMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   if (mTurnLeftRightAxis)
   {
      mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
   }

   mTurnLeftRightAxis = turnLeftRightAxis;  

   mTurnLeftRightAxis->AddAxisListener(mLookLeftRightListener);
}

/**
* Returns the axis that turns the target left (for negative values)
* or right (for positive values).
*
* @return the current turn left/right axis
*/
Axis* CollisionMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis;
}

/**
* Sets the axis that looks down (for negative values)
* or up (for positive values).
*
* @param lookUpDownAxis the new look up/down axis
*/
void CollisionMotionModel::SetLookUpDownAxis(Axis* lookUpDownAxis)
{
   if (mLookUpDownAxis != NULL)
   {
      mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
   }

   mLookUpDownAxis = lookUpDownAxis;  

   mLookUpDownAxis->AddAxisListener(mLookUpDownListener);
}

/**
* Returns the axis that looks down (for negative values)
* or up (for positive values).
*
* @return the current look up/down axis
*/
Axis* CollisionMotionModel::GetLookUpDownAxis()
{
   return mLookUpDownAxis;
}

/**
* Sets the axis that sidesteps the target left (for negative values)
* or right (for positive values).
*
* @param sidestepLeftRightAxis the new sidestep left/right axis
*/
void CollisionMotionModel::SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis)
{
   if (mSidestepLeftRightAxis != NULL)
   {
      mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
   }

   mSidestepLeftRightAxis = sidestepLeftRightAxis;

   mSidestepLeftRightAxis->AddAxisListener(mSidestepListener);
}

/**
* Returns the axis that sidesteps the target left (for negative values)
* or right (for positive values).
*
* @return the current sidestep left/right axis
*/
Axis* CollisionMotionModel::GetSidestepLeftRightAxis()
{
   return mSidestepLeftRightAxis;
}

/**
* Sets the maximum walk speed (meters per second).
*
* @param maximumWalkSpeed the new maximum walk speed
*/
void CollisionMotionModel::SetMaximumWalkSpeed(float maximumWalkSpeed)
{
   mMaximumWalkSpeed = maximumWalkSpeed;
}

/**
* Returns the maximum walk speed (meters per second).
*
* @return the current maximum walk speed
*/
float CollisionMotionModel::GetMaximumWalkSpeed()
{
   return mMaximumWalkSpeed;
}

/**
* Sets the maximum turn speed (degrees per second).
*
* @param maximumTurnSpeed the new maximum turn speed
*/
void CollisionMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
   mMaximumTurnSpeed = maximumTurnSpeed;
}

/**
* Returns the maximum turn speed (degrees per second).
*
* @return the current maximum turn speed
*/
float CollisionMotionModel::GetMaximumTurnSpeed()
{
   return mMaximumTurnSpeed;
}


bool CollisionMotionModel::OnForwardBackwardChanged(double newState, double delta)
{
   mForwardBackCtrl = newState;
   return true;
}

bool CollisionMotionModel::OnSidestepChanged(double newState, double delta)
{
   mSidestepCtrl = newState;
   return true;
}

bool CollisionMotionModel::OnLookLeftRightChanged(double newState, double delta)
{
   mLookLeftRightCtrl = newState;
   return true;
}

bool CollisionMotionModel::OnLookUpDownChanged(double newState, double delta)
{
   mLookUpDownCtrl = newState;
   return true;
}

FPSCollider& CollisionMotionModel::GetFPSCollider()
{
   return mCollider;
}

/**
* Message handler callback.
*
* @param data the message data
*/
void CollisionMotionModel::OnMessage(MessageData *data)
{    

   if(GetTarget() != NULL &&
      IsEnabled() && 
      data->message == "preframe")
   {
      const double deltaFrameTime = static_cast<const double*>(data->userData)[1];

      Transform transform;
      GetTarget()->GetTransform(transform);

      osg::Vec3 xyz, hpr;
      float newH = 0.0f, newP = 0.0f;
      osg::Vec3 newXYZ;

      transform.GetRotation(hpr);

      transform.GetTranslation(xyz);

      newH = hpr[0];
      newP = hpr[1];

      if(mUseMouseButtons)
      {
         if(mMouse->GetButtonState(Mouse::LeftButton)) 
         {
            //calculate our new heading
            newH -= mLookLeftRightCtrl * mMaximumTurnSpeed * deltaFrameTime;
            //calculate our new pitch
            newP += mLookUpDownCtrl * mMaximumTurnSpeed * deltaFrameTime;
            dtUtil::Clamp(newP, -89.9f, 89.9f); //stay away from 90.0 as it causes funky gimbal lock
            mLookUpDownAxis->SetState(0.0f);//necessary to stop camera drifting down
         }
      }
      else
      {
         //calculate our new heading
         newH -= mLookLeftRightCtrl * mMaximumTurnSpeed * deltaFrameTime;
         //calculate our new pitch
         newP += mLookUpDownCtrl * mMaximumTurnSpeed * deltaFrameTime;
         dtUtil::Clamp(newP, -89.9f, 89.9f); //stay away from 90.0 as it causes funky gimbal lock
         mLookUpDownAxis->SetState(0.0f);//necessary to stop camera drifting down
      }

      //calculate x/y delta
      osg::Vec3 translation;
      translation[0] = mSidestepCtrl * mMaximumWalkSpeed;
      translation[1] = mForwardBackCtrl * mMaximumWalkSpeed;

      //transform our x/y delta by our new heading
      osg::Matrix mat;
      mat.makeRotate(osg::DegreesToRadians(newH), osg::Vec3(0.0f, 0.0f, 1.0f) );
      translation = translation * mat;  
      if(translation.length() > mMaximumWalkSpeed)
      {
         translation.normalize();
         translation.set(translation[0] * mMaximumWalkSpeed, translation[1] * mMaximumWalkSpeed, translation[2] * mMaximumWalkSpeed);
      }

      newXYZ = mCollider.Update(xyz, translation, deltaFrameTime, mCanJump ? mKeyboard->GetKeyState(Producer::Key_space) : false);

      transform.SetTranslation(newXYZ);
      if(mUseMouseButtons)
      {
         if(mMouse->GetButtonState(Mouse::LeftButton)) 
            transform.SetRotation(newH, newP, 0.0f);
      }
      else
      {
         transform.SetRotation(newH, newP, 0.0f);
      }   
      
      GetTarget()->SetTransform(transform); 

      //if are requiring mouse buttons and the left mouse button is pressed OR we aren't requiring mouse buttons
      if((mUseMouseButtons && mMouse->GetButtonState(Mouse::LeftButton)) || !mUseMouseButtons) 
      {
         mMouse->SetPosition(0.0f,0.0f);//keeps cursor at center of screen
      }
   }
}
}
