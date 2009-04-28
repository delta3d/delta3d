// walkmotionmodel.cpp: Implementation of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/fpsmotionmodel.h>

#include <dtCore/logicalinputdevice.h>

#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>

#include <osg/Quat>
#include <osgViewer/View>

#include <cmath>
#include <iostream>

namespace dtCore
{

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_MANAGEMENT_LAYER(FPSMotionModel)


FPSMotionModel::FPSAxisListener::FPSAxisListener(const SetFunctor& setFunc)
   : mSetFunctor(setFunc)
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
FPSMotionModel::FPSMotionModel(Keyboard* keyboard,
                               Mouse* mouse,
                               float maxWalkSpeed,
                               float maxTurnSpeed,
                               float maxSidestepSpeed,
                               float height,
                               float maxStepUpDist,
                               bool useWASD,
                               bool useArrowKeys)
   : MotionModel("FPSMotionModel")
   , mpDebugger(NULL)
   , mWalkForwardBackwardAxis(NULL)
   , mTurnLeftRightAxis(NULL)
   , mLookUpDownAxis(NULL)
   , mSidestepLeftRightAxis(NULL)
   , mSidestepListener(NULL)
   , mForwardBackwardListener(NULL)
   , mLookLeftRightListener(NULL)
   , mLookUpDownListener(NULL)
   , mMaximumWalkSpeed(maxWalkSpeed)
   , mMaximumTurnSpeed(maxTurnSpeed)
   , mMaximumSidestepSpeed(maxSidestepSpeed)
   , mHeightAboveTerrain(height)
   , mMaximumStepUpDistance(maxStepUpDist)
   , mFallingHeight(1.f)
   , mFallingVec(0.f, 0.f, 0.f)
   , mFalling(false)
   , mInvertMouse(false)
   , mUseWASD(useWASD)
   , mUseArrowKeys(useArrowKeys)
   , mOperateWhenUnfocused(false)
   , mShouldResetMouse(true)
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

   mLookUpDownListener      = new FPSAxisListener(lookUpDownFunc);
   mLookLeftRightListener   = new FPSAxisListener(lookLeftRightFunc);
   mSidestepListener        = new FPSAxisListener(sideStepFunc);
   mForwardBackwardListener = new FPSAxisListener(fbFunc);

   if (keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   AddSender(&System::GetInstance());

   mMouse    = mouse;
   mKeyboard = keyboard;

   mIsector = new Isector();
   mIsector->SetDirection(osg::Vec3(0.f, 0.f, -1.f));
   mIsector->SetLength(1000.f);
}

/**
 * Destructor.
 */
FPSMotionModel::~FPSMotionModel()
{
   RemoveSender(&System::GetInstance());

   if (mLookUpDownAxis.get())
   {
      mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
      delete mLookUpDownListener;
   }
   if (mTurnLeftRightAxis.get())
   {
      mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
      delete mLookLeftRightListener;
   }
   if (mSidestepLeftRightAxis.get())
   {
      mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
      delete mSidestepListener;
   }
   if (mWalkForwardBackwardAxis.get())
   {
      mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
      delete mForwardBackwardListener;
   }

   mIsector->SetScene(0);

   DeregisterInstance(this);
}

/**
 * Sets the active Scene, which is used for ground following.
 *
 * @param scene the active scene
 */
void FPSMotionModel::SetScene(Scene* scene)
{
   mScene = scene;
   mIsector->SetScene(mScene.get());
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
* Returns the isector used for collision testing.
*
* @return the active Scene
*/
dtCore::Isector* FPSMotionModel::GetISector() const
{
   return mIsector.get();
}

/**
* Enables or disables this motion model.
*
* @param enabled true to enable this motion model, false
* to disable it
*/
void FPSMotionModel::SetEnabled(bool enabled)
{
   if (enabled && !MotionModel::IsEnabled())
   {
      mMouse->SetPosition(0.0f,0.0f);

      if (mLookUpDownAxis.valid())
      {
         mLookUpDownAxis->AddAxisListener(mLookUpDownListener);
      }
      if (mTurnLeftRightAxis.valid())
      {
         mTurnLeftRightAxis->AddAxisListener(mLookLeftRightListener);
      }
      if (mSidestepLeftRightAxis.valid())
      {
         mSidestepLeftRightAxis->AddAxisListener(mSidestepListener);
      }
      if (mWalkForwardBackwardAxis.valid())
      {
         mWalkForwardBackwardAxis->AddAxisListener(mForwardBackwardListener);
      }
   }
   if (!enabled && MotionModel::IsEnabled())
   {
      if (mLookUpDownAxis.valid())
      {
         mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
      }
      if (mTurnLeftRightAxis.valid())
      {
         mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
      }
      if (mSidestepLeftRightAxis.valid())
      {
         mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
      }
      if (mWalkForwardBackwardAxis.valid())
      {
         mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
      }
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
void FPSMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if (!mDefaultInputDevice.valid()) //if (mDefaultInputDevice.get() == 0)
   {
      mDefaultInputDevice = new LogicalInputDevice("FPSLogicalInputDevice");

      Axis* leftRightMouseMovement = mDefaultInputDevice->AddAxis(
         "left/right mouse movement",
          new AxisToAxis(mouse->GetAxis(0)));

      Axis* upDownMouseMovement = mDefaultInputDevice->AddAxis(
         "up/down mouse movement",
          new AxisToAxis(mouse->GetAxis(1)));


      AxesToAxis* forwardBack = new AxesToAxis();
      AxesToAxis* leftRight = new AxesToAxis();

      if (mUseWASD)
      {
         Axis* forwardAndBackAxis1 = mDefaultInputDevice->AddAxis(
            "s/w",
            new ButtonsToAxis(keyboard->GetButton('s'), keyboard->GetButton('w'))
         );

         Axis* forwardAndBackAxis2 = mDefaultInputDevice->AddAxis(
            "S/W",
            new ButtonsToAxis(keyboard->GetButton('S'), keyboard->GetButton('W'))
         );

         Axis* sideStepAxis1 = mDefaultInputDevice->AddAxis(
            "a/d",
            new ButtonsToAxis(keyboard->GetButton('a'), keyboard->GetButton('d'))
         );

         Axis* sideStepAxis2 = mDefaultInputDevice->AddAxis(
            "A/D",
            new ButtonsToAxis(keyboard->GetButton('A'), keyboard->GetButton('D'))
         );

         forwardBack->AddSourceAxis(forwardAndBackAxis1);
         forwardBack->AddSourceAxis(forwardAndBackAxis2);
         leftRight->AddSourceAxis(sideStepAxis1);
         leftRight->AddSourceAxis(sideStepAxis2);
      }

      if (mUseArrowKeys)
      {

         Axis* arrowKeysUpAndDown = mDefaultInputDevice->AddAxis(
            "arrow keys up/down",
            new ButtonsToAxis(
               keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Down),
               keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Up)
            )
         );

         Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
            "arrow keys left/right",
            new ButtonsToAxis(
               keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Left),
               keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Right)
            )
         );

         forwardBack->AddSourceAxis(arrowKeysUpAndDown);
         leftRight->AddSourceAxis(arrowKeysLeftAndRight);
      }

      mDefaultWalkForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default walk forward/backward",
         forwardBack
      );

      mDefaultSidestepLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default sidestep left/right",
         leftRight
      );

      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right",
         new AxesToAxis(leftRightMouseMovement)
      );

      mDefaultLookUpDownAxis = mDefaultInputDevice->AddAxis(
         "default look up/down",
         new AxesToAxis(upDownMouseMovement)
      );

   }

   SetWalkForwardBackwardAxis(mDefaultWalkForwardBackwardAxis.get());

   SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis.get());

   SetLookUpDownAxis(mDefaultLookUpDownAxis.get());

   SetSidestepLeftRightAxis(mDefaultSidestepLeftRightAxis.get());
}

/**
 * Sets the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @param walkForwardBackwardAxis the new forward/backward axis
 */
void FPSMotionModel::SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis)
{
   if (mWalkForwardBackwardAxis.valid())
   {
      mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
   }

   mWalkForwardBackwardAxis = walkForwardBackwardAxis;

   if (mWalkForwardBackwardAxis.valid())
   {
      mWalkForwardBackwardAxis->AddAxisListener(mForwardBackwardListener);
   }
}

/**
 * Returns the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @return the current forward/backward axis
 */
Axis* FPSMotionModel::GetWalkForwardBackwardAxis()
{
   return mWalkForwardBackwardAxis.get();
}

/**
 * Sets the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @param turnLeftRightAxis the new turn left/right axis
 */
void FPSMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   if (mTurnLeftRightAxis.valid())
   {
      mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
   }

   mTurnLeftRightAxis = turnLeftRightAxis;

   if (mTurnLeftRightAxis.valid())
   {
      mTurnLeftRightAxis->AddAxisListener(mLookLeftRightListener);
   }
}

/**
 * Returns the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @return the current turn left/right axis
 */
Axis* FPSMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis.get();
}

/**
 * Sets the axis that looks down (for negative values)
 * or up (for positive values).
 *
 * @param lookUpDownAxis the new look up/down axis
 */
void FPSMotionModel::SetLookUpDownAxis(Axis* lookUpDownAxis)
{
   if (mLookUpDownAxis.valid())
   {
      mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
   }

   mLookUpDownAxis = lookUpDownAxis;

   if (mLookUpDownAxis.valid())
   {
      mLookUpDownAxis->AddAxisListener(mLookUpDownListener);
   }
}

/**
 * Returns the axis that looks down (for negative values)
 * or up (for positive values).
 *
 * @return the current look up/down axis
 */
Axis* FPSMotionModel::GetLookUpDownAxis()
{
   return mLookUpDownAxis.get();
}

/**
 * Sets the axis that sidesteps the target left (for negative values)
 * or right (for positive values).
 *
 * @param sidestepLeftRightAxis the new sidestep left/right axis
 */
void FPSMotionModel::SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis)
{
   if (mSidestepLeftRightAxis.valid())
   {
      mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
   }

   mSidestepLeftRightAxis = sidestepLeftRightAxis;

   if (mSidestepLeftRightAxis.valid())
   {
      mSidestepLeftRightAxis->AddAxisListener(mSidestepListener);
   }
}

/**
 * Returns the axis that sidesteps the target left (for negative values)
 * or right (for positive values).
 *
 * @return the current sidestep left/right axis
 */
Axis* FPSMotionModel::GetSidestepLeftRightAxis()
{
   return mSidestepLeftRightAxis.get();
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
void FPSMotionModel::OnMessage(MessageData* data)
{
   //if (IsCurrentlyActive() && data->message == dtCore::System::MESSAGE_EVENT_TRAVERSAL)
   if (IsCurrentlyActive() && mShouldResetMouse)
   {
      if (GetTurnLeftRightAxis())
      {
         GetTurnLeftRightAxis()->SetState(0.0f);
      }
      if (GetLookUpDownAxis())
      {
         GetLookUpDownAxis()->SetState(0.0f);
      }
      mShouldResetMouse = false;
   }
   else if (IsCurrentlyActive() && data->message == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
   {
      // use the simulated change in time, not the real time change
      // see dtCore::System for the difference.
      // The ideal solution would be to use options, like FlyMotionModel
      double deltaFrameTime = static_cast<const double*>(data->userData)[0];

      // clamp frame time to be no more then 3 fps
      const double MAX_FRAME_TIME = 1.0 / 3.0;
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
bool FPSMotionModel::IsCurrentlyActive()
{
   bool result = false;

   result = GetTarget() != NULL && IsEnabled() &&
      (mOperateWhenUnfocused || mMouse->GetHasFocus());

   // Flag the mouse to be reset if the motion model is not currently active.
   if (!result)
   {
      mShouldResetMouse = true;
   }

   return result;
}

/////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::ShouldOperateWhenUnfocused(bool operate)
{
   mOperateWhenUnfocused = operate;
}

/////////////////////////////////////////////////////////////////////////////
class FPSMotionModel::FPSMotionModelDebugger
{
public:
   FPSMotionModelDebugger()
      : mNumFrames(0)
      , mFramesWithNonZeroMouse(0)
      , mTotalTime(0.0f)
   {
      // nada
   }

   void Update(const double deltaTime, bool mouse_has_moved)
   {
      ++mNumFrames;
      if (mouse_has_moved) { ++mFramesWithNonZeroMouse; }
      mTotalTime += deltaTime;

      if (mTotalTime > 0.5f)
      {
         std::cout << "Num Frames: " << mNumFrames << ", Frames with mouse input: " << mFramesWithNonZeroMouse << std::endl;

         mTotalTime = 0.0f;
         mFramesWithNonZeroMouse = 0;
         mNumFrames = 0;
      }
   }

private:
   int mNumFrames;
   int mFramesWithNonZeroMouse;
   float mTotalTime;
};

/////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::UpdateMouse(const double deltaTime)
{
   //NOTE: This code has been commented out because it does not work with window resizing
   //if the window is resized then the mouse position which was set to (0.1, 0.0) will come
   //back differently and you will never regain mouse movement.  Is there a better way we can do this? -Anderegg

   //if (!mShouldResetMouse)
   //{
   //   if (GetTurnLeftRightAxis())
   //   {
   //      diff.x() = GetTurnLeftRightAxis()->GetState();
   //   }
   //   if (GetLookUpDownAxis())
   //   {
   //      diff.y() = GetLookUpDownAxis()->GetState();
   //   }
   //}
   //// If the mouse needs to be reset (eg. the motion model has just become active)
   //// then we need to reset the mouse to ensure the camera does not pop to a different angle.
   //else if (IsCurrentlyActive())
   //{
   //   // This is a really ugly hack that I hate!
   //   // Since the OSG mouse does not update its position from 0,0 until the
   //   // mouse first enters a valid viewport area, there is no way
   //   // to tell if the mouse is currently on the screen.
   //   // If the mouse is not in a valid camera view, setting its position
   //   // does nothing.  Therefore, I set the position to a value
   //   // other than its default position 0,0 and then check the position to be sure
   //   // the mouse was actually moved.
   //   mMouse->SetPosition(0.1f,0.0f);

   //   osg::Vec2 mousePosition = mMouse->GetPosition();

   //   // If the mouse has moved to my custom position, that means
   //   // the mouse is within a valid camera view area.
   //   // It is now that I need to reset the mouse so
   //   // the view camera does not pop to another angle during a focus change.
   //   if (mousePosition.x() == 0.1f && mousePosition.y() == 0.0f)
   //   {
   //      mShouldResetMouse = false;
   //      mMouse->SetPosition(0.0f, 0.0f);
   //   }
   //}

   if (GetTurnLeftRightAxis())
   {
      mMouseMove.x() += GetTurnLeftRightAxis()->GetState();
      GetTurnLeftRightAxis()->SetState(0.0f);
   }
   if (GetLookUpDownAxis())
   {
      mMouseMove.y() += GetLookUpDownAxis()->GetState();
      GetLookUpDownAxis()->SetState(0.0f);
   }

   const bool calc_new_heading_pitch = !mUseMouseButtons || mMouse->GetButtonState(Mouse::LeftButton);
   const bool mouse_has_moved = HasMouseMoved(mMouseMove);

   // Once a new mouse movement has been triggered, we no longer need to reset the mouse.
   if (mpDebugger)
   {
      mpDebugger->Update(deltaTime, mouse_has_moved);
   }


   if (calc_new_heading_pitch && mouse_has_moved)
   {
      Transform transform;
      GetTarget()->GetTransform(transform);

      osg::Matrix rot;
      transform.GetRotation(rot);
      float deltaZ = mMouseMove[0] * mMaximumTurnSpeed;
      float deltaX = mMouseMove[1] * mMaximumTurnSpeed;

      osg::Vec3 upVector      = dtUtil::MatrixUtil::GetRow3(rot, 2);
      osg::Vec3 forwardVector = dtUtil::MatrixUtil::GetRow3(rot, 1);
      osg::Vec3 rightVector   = dtUtil::MatrixUtil::GetRow3(rot, 0);

      if (mInvertMouse)
      {
         deltaX = -deltaX;
      }

      osg::Quat rotateZ, rotateX;
      rotateZ.makeRotate(-deltaZ, upVector);
      rotateX.makeRotate(+deltaX, rightVector); //we must revert the x axis delta

      forwardVector = rotateZ * forwardVector;
      forwardVector = rotateX * forwardVector;

      //TODO- use the normalized opposite of the scene's gravity vector
      upVector = osg::Vec3(0.0f, 0.0f, 1.0f);

      rightVector = forwardVector ^ upVector;
      upVector    = rightVector   ^ forwardVector;

      rightVector.normalize();
      forwardVector.normalize();
      upVector.normalize();

      dtUtil::MatrixUtil::SetRow(rot, rightVector,   0);
      dtUtil::MatrixUtil::SetRow(rot, forwardVector, 1);
      dtUtil::MatrixUtil::SetRow(rot, upVector,      2);

      // apply changes (new orientation)
      transform.SetRotation(rot);
      GetTarget()->SetTransform(transform);

      ResetMousePosition();
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
   if (mScene.valid())
   {
      // ground clamp if required
      AdjustElevation(newXYZ, deltaTime);
   }

   // apply changes (new position)
   transform.SetTranslation(newXYZ);
   GetTarget()->SetTransform(transform);
}

///Update the MotionModel's elevation by either ground clamping, or "falling"
void FPSMotionModel::AdjustElevation(osg::Vec3& xyz, double deltaFrameTime)
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
   if ((xyz[2]-targetHeight) > mFallingHeight)
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

bool FPSMotionModel::HasMouseMoved(const osg::Vec2& diff)
{
   //this is overly complicated and rather annoying however it fixes the 'drifting' bug
   //the use case is resizing the window with the motion model active and the camera will
   //drift indefinitely.  This is caused by setting the mouse position to (0.0, 0.0) which
   //does not work in the case that the window width or height is an odd number.  When the
   //window width or height is odd then the center of the screen is not an even number of pixels.
   //The result is that the mouse will get a difference of 1/width or 1/height for whichever is odd
   //it will then recenter the mouse again and we drift.  This code here uses an accumulated mouse movement
   //so the user has to move the mouse at least one pixel to get the motion model to update.

   dtCore::View* viewPtr = mMouse->GetView();

   int width = 2;
   int height = 2;

   if (viewPtr &&
      viewPtr->GetOsgViewerView() &&
      viewPtr->GetOsgViewerView()->getCamera() &&
      viewPtr->GetOsgViewerView()->getCamera()->getGraphicsContext() &&
      viewPtr->GetOsgViewerView()->getCamera()->getGraphicsContext()->getTraits())
   {
      width = viewPtr->GetOsgViewerView()->getCamera()->getGraphicsContext()->getTraits()->width;
      height = viewPtr->GetOsgViewerView()->getCamera()->getGraphicsContext()->getTraits()->height;
   }
   else
   {
      //in the odd case that there is no view I'm not sure it matters what we do here....
      return true;
   }

   //this will ensure we do not move the camera unless the mouse has moved at least one pixel from the center
   float diffWidth = 1.0f / float(width - 1);
   float diffHeight = 1.0f / float(height - 1);

   return (std::abs(diff[0]) > diffWidth || std::abs(diff[1]) > diffHeight);
}

void FPSMotionModel::ResetMousePosition()
{
   mMouseMove.set(0.0f, 0.0f);
   mMouse->SetPosition(0.0f,0.0f); // keeps cursor at center of screen
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

void FPSMotionModel::SetFallingHeight(float fallingHeight)
{
   mFallingHeight = fallingHeight;
}
/////////////////////////////////////////////////////////////////////////////

}
