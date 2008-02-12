// walkmotionmodel.cpp: Implementation of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/fpsmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>//due to include of scene.h
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/keyboard.h>//due to include of scene.h

#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>
#include <dtUtil/mathdefines.h>

#include <osg/Quat>
#include <dtUtil/matrixutil.h>
#include <cmath>

namespace dtCore
{

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_MANAGEMENT_LAYER(FPSMotionModel)


FPSMotionModel::FPSAxisListener::FPSAxisListener(const SetFunctor& setFunc):
mSetFunctor(setFunc)
{
}

///When the axis changes, just call the functor with the new values
bool FPSMotionModel::FPSAxisListener::AxisStateChanged(const Axis* axis,
                                                       double oldState, 
                                                       double newState, 
                                                       double delta)
{
   return mSetFunctor(newState, delta);
}


/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 */
FPSMotionModel::FPSMotionModel(  Keyboard* keyboard,
                                 Mouse* mouse,
                                 float maxWalkSpeed,
                                 float maxTurnSpeed,
                                 float maxSidestepSpeed,
                                 float height,
                                 float maxStepUpDist)
   : MotionModel("FPSMotionModel")
   , mWalkForwardBackwardAxis(0)
   , mTurnLeftRightAxis(0)
   , mLookUpDownAxis(0)
   , mSidestepLeftRightAxis(0)
   , mSidestepListener(0)
   , mForwardBackwardListener(0)
   , mLookLeftRightListener(0)
   , mLookUpDownListener(0)
   , mMaximumWalkSpeed(maxWalkSpeed)
   , mMaximumTurnSpeed(maxTurnSpeed)
   , mMaximumSidestepSpeed(maxSidestepSpeed)
   , mHeightAboveTerrain(height)
   , mMaximumStepUpDistance(maxStepUpDist)
   , mFallingHeight(1.f)
   , mFallingVec(0.f, 0.f, 0.f)
   , mFalling(false)
   , mInvertMouse(false)
   , mForwardBackCtrl(0.f)
   , mSidestepCtrl(0.f)
   , mLookLeftRightCtrl(0.f)
   , mLookUpDownCtrl(0.f)
   , mUseMouseButtons(false) // default behavior is NOT to require mouse down to look
{
   RegisterInstance(this);
   
   //setup some axis listeners with functors 
   FPSAxisListener::SetFunctor fbFunc(this, &FPSMotionModel::OnForwardBackwardChanged);
   FPSAxisListener::SetFunctor sideStepFunc(this, &FPSMotionModel::OnSidestepChanged);  
   FPSAxisListener::SetFunctor lookLeftRightFunc(this, &FPSMotionModel::OnLookLeftRightChanged);
   FPSAxisListener::SetFunctor lookUpDownFunc(this, &FPSMotionModel::OnLookUpDownChanged);

   mLookUpDownListener      = new FPSAxisListener( lookUpDownFunc );
   mLookLeftRightListener   = new FPSAxisListener( lookLeftRightFunc );
   mSidestepListener        = new FPSAxisListener( sideStepFunc );
   mForwardBackwardListener = new FPSAxisListener( fbFunc );

   if(keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }
   
   AddSender(&System::GetInstance());
   
   mMouse    = mouse;
   mKeyboard = keyboard;

   mIsector = new Isector();
   mIsector->SetDirection( osg::Vec3(0.f, 0.f, -1.f) );
   mIsector->SetLength(1000.f);
}

/**
 * Destructor.
 */
FPSMotionModel::~FPSMotionModel()
{
   RemoveSender(&System::GetInstance());

   mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
   mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
   mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
   mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);

   delete mLookUpDownListener;
   delete mLookLeftRightListener;
   delete mSidestepListener;
   delete mForwardBackwardListener;

   mIsector->SetScene(0);

   DeregisterInstance(this);
}

/**
 * Sets the active Scene, which is used for ground following.
 *
 * @param scene the active scene
 */
void FPSMotionModel::SetScene(Scene *scene)
{
   mScene = scene;
   mIsector->SetScene( mScene.get() );
}

/**
 * Returns the active Scene.
 *
 * @return the active Scene
 */
Scene* FPSMotionModel::GetScene()
{
   return mScene.get();
}

/**
* Enables or disables this motion model.
*
* @param enabled true to enable this motion model, false
* to disable it
*/
void FPSMotionModel::SetEnabled(bool enabled)
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
void FPSMotionModel::SetDefaultMappings(Keyboard *keyboard, Mouse *mouse)
{
   if(!mDefaultInputDevice.valid()) //if(mDefaultInputDevice.get() == 0)
   {
      mDefaultInputDevice = new LogicalInputDevice("FPSLogicalInputDevice");

      Axis *leftRightMouseMovement = mDefaultInputDevice->AddAxis(
         "left/right mouse movement",
          new AxisToAxis(mouse->GetAxis(0)));

      Axis *upDownMouseMovement = mDefaultInputDevice->AddAxis(
         "up/down mouse movement",
          new AxisToAxis(mouse->GetAxis(1)));

      Axis *forwardAndBackAxis1 = mDefaultInputDevice->AddAxis(
         "s/w",
          new ButtonsToAxis( keyboard->GetButton('s'), keyboard->GetButton('w') )
      );

      Axis *forwardAndBackAxis2 = mDefaultInputDevice->AddAxis(
         "S/W",
          new ButtonsToAxis( keyboard->GetButton('S'), keyboard->GetButton('W') )
       );

      Axis *sideStepAxis1 = mDefaultInputDevice->AddAxis(
         "a/d",
          new ButtonsToAxis( keyboard->GetButton('a'), keyboard->GetButton('d') )
      );

      Axis *sideStepAxis2 = mDefaultInputDevice->AddAxis(
         "A/D",
          new ButtonsToAxis( keyboard->GetButton('A'), keyboard->GetButton('D') )
      );


      mDefaultWalkForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default walk forward/backward",
         new AxesToAxis(forwardAndBackAxis1, forwardAndBackAxis2)
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
         new AxesToAxis(sideStepAxis1, sideStepAxis2)
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
void FPSMotionModel::SetWalkForwardBackwardAxis(Axis *walkForwardBackwardAxis)
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
Axis* FPSMotionModel::GetWalkForwardBackwardAxis()
{
   return mWalkForwardBackwardAxis;
}

/**
 * Sets the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @param turnLeftRightAxis the new turn left/right axis
 */
void FPSMotionModel::SetTurnLeftRightAxis(Axis *turnLeftRightAxis)
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
Axis* FPSMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis;
}

/**
 * Sets the axis that looks down (for negative values)
 * or up (for positive values).
 *
 * @param lookUpDownAxis the new look up/down axis
 */
void FPSMotionModel::SetLookUpDownAxis(Axis *lookUpDownAxis)
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
Axis* FPSMotionModel::GetLookUpDownAxis()
{
   return mLookUpDownAxis;
}

/**
 * Sets the axis that sidesteps the target left (for negative values)
 * or right (for positive values).
 *
 * @param sidestepLeftRightAxis the new sidestep left/right axis
 */
void FPSMotionModel::SetSidestepLeftRightAxis(Axis *sidestepLeftRightAxis)
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
Axis* FPSMotionModel::GetSidestepLeftRightAxis()
{
   return mSidestepLeftRightAxis;
}

/**
 * Sets the maximum walk speed (meters per second).
 *
 * @param maximumWalkSpeed the new maximum walk speed
 */
void FPSMotionModel::SetMaximumWalkSpeed(float maximumWalkSpeed)
{
   mMaximumWalkSpeed = maximumWalkSpeed;
}

/**
 * Returns the maximum walk speed (meters per second).
 *
 * @return the current maximum walk speed
 */
float FPSMotionModel::GetMaximumWalkSpeed()
{
   return mMaximumWalkSpeed;
}

/**
 * Sets the maximum turn speed (degrees per second).
 *
 * @param maximumTurnSpeed the new maximum turn speed
 */
void FPSMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
   mMaximumTurnSpeed = maximumTurnSpeed;
}

/**
 * Returns the maximum turn speed (degrees per second).
 *
 * @return the current maximum turn speed
 */
float FPSMotionModel::GetMaximumTurnSpeed()
{
   return mMaximumTurnSpeed;
}

/**
 * Sets the maximum sidestep speed (meters per second).
 *
 * @param maximumSidestepSpeed the new maximum sidestep speed
 */
void FPSMotionModel::SetMaximumSidestepSpeed(float maximumSidestepSpeed)
{
   mMaximumSidestepSpeed = maximumSidestepSpeed;
}

/**
 * Returns the maximum sidestep speed (meters per second).
 *
 * @return the current maximum sidestep speed
 */
float FPSMotionModel::GetMaximumSidestepSpeed()
{
   return mMaximumSidestepSpeed;
}

/**
 * Sets the height to maintain above the terrain (meters).
 *
 * @param heightAboveTerrain the height to maintain above the
 * terrain
 */
void FPSMotionModel::SetHeightAboveTerrain(float heightAboveTerrain)
{
   mHeightAboveTerrain = heightAboveTerrain;
}

/**
 * Returns the height to maintain above the terrain (meters).
 *
 * @return the height to maintain above the terrain
 */
float FPSMotionModel::GetHeightAboveTerrain()
{
   return mHeightAboveTerrain;
}

/**
 * Sets the maximum step-up distance.  When clamping to the ground, the
 * maximum step-up distance determines whether to rise to a new level
 * (as when the model climbs a staircase) or to stay at the current level
 * (as when the model passes under a roof).  The default is 1.0.
 *
 * @param maximumStepUpDistance the new maximum step-up distance
 */
void FPSMotionModel::SetMaximumStepUpDistance(float maximumStepUpDistance)
{
   mMaximumStepUpDistance = maximumStepUpDistance;
}

/**
 * Returns the current maximum step-up distance.
 *
 * @return the maximum step-up distance
 */
float FPSMotionModel::GetMaximumStepUpDistance()
{
   return mMaximumStepUpDistance;
}
         

float FPSMotionModel::GetFallingHeight() const
{
   return mFallingHeight;
}

/**
 * Message handler callback.
 *
 * @param data the message data
 */
void FPSMotionModel::OnMessage(MessageData *data)
{
   if(GetTarget() != NULL &&
      IsEnabled() && mMouse->GetHasFocus() &&
      data->message == "preframe")
   {
      // use the real change in time, not the simulated time change
      // see dtCore::System for the difference.
      double deltaFrameTime = static_cast<const double*>(data->userData)[1];

      // clamp frame time to be no less then 20 fps
      const double MAX_FRAME_TIME = 1.0 / 20.0;
      if (deltaFrameTime > MAX_FRAME_TIME)
      {
         deltaFrameTime = MAX_FRAME_TIME;
      }

      // read mouse state to perform rotations (and reset mouse state)
      UpdateMouse(deltaFrameTime);

      // perform translations
      PerformTranslation(deltaFrameTime);
   }
}

/////////////////////////////////////////////////////////////////////////////

void FPSMotionModel::UpdateMouse(const double deltaTime)
{
   const bool calc_new_heading_pitch = !mUseMouseButtons || mMouse->GetButtonState(Mouse::LeftButton);
   const bool mouse_has_moved = (std::abs(mLookLeftRightCtrl) > 0.0f || std::abs(mLookUpDownCtrl) > 0.0f);

   if(calc_new_heading_pitch && mouse_has_moved)
   {
      Transform transform;
      GetTarget()->GetTransform(transform);

      osg::Matrix rot;
      transform.GetRotation(rot);
      float deltaZ = mLookLeftRightCtrl * mMaximumTurnSpeed * deltaTime;
      float deltaX = mLookUpDownCtrl * mMaximumTurnSpeed * deltaTime;

      osg::Vec3 upVector = dtUtil::MatrixUtil::GetRow3(rot, 2);
      osg::Vec3 forwardVector = dtUtil::MatrixUtil::GetRow3(rot, 1);
      osg::Vec3 rightVector = dtUtil::MatrixUtil::GetRow3(rot, 0);
      
      if(mInvertMouse)
      {
         deltaX = -deltaX;
      }

      osg::Quat rotateZ, rotateX;
      rotateZ.makeRotate(-deltaZ, upVector);
      rotateX.makeRotate(deltaX, rightVector); //we must revert the x axis delta

      forwardVector = rotateZ * forwardVector;
      forwardVector = rotateX * forwardVector;

      //TODO- use the normalized opposite of the scene's gravity vector
      upVector = osg::Vec3(0.0f, 0.0f, 1.0f);
       
      rightVector = forwardVector ^ upVector;
      upVector = rightVector ^ forwardVector;

      rightVector.normalize();
      forwardVector.normalize();
      upVector.normalize();

      dtUtil::MatrixUtil::SetRow(rot, rightVector, 0);
      dtUtil::MatrixUtil::SetRow(rot, forwardVector, 1);
      dtUtil::MatrixUtil::SetRow(rot, upVector, 2);

      // apply changes (new orientation)
      transform.SetRotation(rot);
      GetTarget()->SetTransform(transform);

      mMouse->SetPosition(0.0f,0.0f); // keeps cursor at center of screen
   }
}

void FPSMotionModel::PerformTranslation(const double deltaTime)
{
   Transform transform;
   osg::Vec3 xyz, newXYZ;
   osg::Matrix rot;

   // query initial status (to change from)
   GetTarget()->GetTransform(transform);
   transform.GetRotation(rot);
   transform.GetTranslation(xyz);

   osg::Vec3 forwardVector = dtUtil::MatrixUtil::GetRow3(rot, 1);
   osg::Vec3 transForward = forwardVector * mForwardBackCtrl * deltaTime;

   osg::Vec3 rightVector = dtUtil::MatrixUtil::GetRow3(rot, 0);
   osg::Vec3 transRight = rightVector * mSidestepCtrl * deltaTime;

   // calculate x/y delta
   osg::Vec3 translation(transForward + transRight);
   translation.normalize();   
   translation *= mMaximumWalkSpeed;

   // integration step
   newXYZ = xyz + translation * deltaTime;

   // apply collision detection/response
   if(mScene.valid())
   {         
      // ground clamp if required
      AdjustElevation(newXYZ, deltaTime);
   }

   // apply changes (new position)
   transform.SetTranslation(newXYZ);
   GetTarget()->SetTransform(transform);
}

///Update the MotionModel's elevation by either ground clamping, or "falling"
void FPSMotionModel::AdjustElevation(osg::Vec3 &xyz, double deltaFrameTime)
{
   mIsector->Reset();

   osg::Vec3 start(
      xyz[0],
      xyz[1],
      xyz[2] + mMaximumStepUpDistance - mHeightAboveTerrain
   );

   mIsector->SetStartPosition(start);

   float hot = 0.0f;

   if (mIsector->Update() == true)
   {
      osg::Vec3 hitPt;
      mIsector->GetHitPoint(hitPt, 0);
      hot = hitPt[2];
   }
   else
   {
      //no intersection - lets just stay at the same elevation
      hot = xyz[2]-mHeightAboveTerrain;
   }

   //add in the offset distance off the height of terrain
   const float targetHeight = hot + mHeightAboveTerrain;

   //if we're too high off the terrain, then let gravity take over
   if( (xyz[2]-targetHeight) > mFallingHeight)
   {
      mFalling = true;
   }

   if (mFalling)
   {
      //adjust the position based on the gravity vector

      osg::Vec3 gravityVec;
      mScene->GetGravity(gravityVec);

      mFallingVec += gravityVec * deltaFrameTime;

      //modify our position using the falling vector
      xyz += mFallingVec * deltaFrameTime;    

      //make sure didn't fall below the terrain
      if (xyz[2] <= targetHeight)
      {
         //stop falling
         mFallingVec.set(0.f, 0.f, 0.f);
         xyz[2] = targetHeight;
         mFalling = false;
      }
   }

   //otherwise, lets clamp to the terrain
   else 
   {
      mFallingVec.set(0.f, 0.f, 0.f);
      xyz[2] = targetHeight;
   }
}

bool FPSMotionModel::OnForwardBackwardChanged(double newState, double delta)
{
   mForwardBackCtrl = newState;
   return true;
}

bool FPSMotionModel::OnSidestepChanged(double newState, double delta)
{
   mSidestepCtrl = newState;
   return true;
}

bool FPSMotionModel::OnLookLeftRightChanged(double newState, double delta)
{
   mLookLeftRightCtrl = newState;
   return true;
}

bool FPSMotionModel::OnLookUpDownChanged(double newState, double delta)
{
   mLookUpDownCtrl = newState;
   return true;
}

/////////////////////////////////////////////////////////////////////////////

}
