// walkmotionmodel.cpp: Implementation of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/fpsmotionmodel.h>


#include <dtCore/system.h>
#include <dtCore/transform.h>

#include <dtUtil/matrixutil.h>
#include <dtUtil/functor.h>

#include <osg/Quat>
#include <osgViewer/View>

#include <cmath>
#include <iostream>

namespace dtCore
{

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_MANAGEMENT_LAYER(FPSMotionModel)

/**
* Helper class used to call the supplied functor when an axis value
* changes.  Used only by the FPSMotionModel.
*/
class DT_CORE_EXPORT FPSAxisHandler :  public dtCore::AxisHandler
{
public:
   typedef dtUtil::Functor<bool, TYPELIST_2(double,double)> SetFunctor;

   FPSAxisHandler(const SetFunctor& setFunc)
   : mSetFunctor(setFunc)
   {
   }

   virtual ~FPSAxisHandler() {};

///When the axis changes, just call the functor with the new values
   virtual bool HandleAxisStateChanged(const Axis* axis,
      double oldState,
      double newState,
      double delta)
   {
      return mSetFunctor(newState, delta);
   }

private:
   SetFunctor mSetFunctor;
};




////////////////////////////////////////////////////////////////////////////////
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
   , mSidestepHandler(NULL)
   , mForwardBackwardHandler(NULL)
   , mLookLeftRightHandler(NULL)
   , mLookUpDownHandler(NULL)
   , mMaximumWalkSpeed(maxWalkSpeed)
   , mMaximumTurnSpeed(maxTurnSpeed)
   , mMaximumSidestepSpeed(maxSidestepSpeed)
   , mHeightAboveTerrain(height)
   , mMaximumStepUpDistance(maxStepUpDist)
   , mFallingHeight(1.0f)
   , mFallingVec(0.0f, 0.0f, 0.0f)
   , mFalling(false)
   , mInvertMouse(false)
   , mUseWASD(useWASD)
   , mUseArrowKeys(useArrowKeys)
   , mOperateWhenUnfocused(false)
   , mShouldRecenterMouse(true)
   , mForwardBackCtrl(0.0f)
   , mSidestepCtrl(0.0f)
   , mLookLeftRightCtrl(0.0f)
   , mLookUpDownCtrl(0.0f)
   , mUseMouseButtons(false) // default behavior is NOT to require mouse down to look
{
   RegisterInstance(this);

   //setup some axis handlers with functors
   FPSAxisHandler::SetFunctor fbFunc(this, &FPSMotionModel::OnForwardBackwardChanged);
   FPSAxisHandler::SetFunctor sideStepFunc(this, &FPSMotionModel::OnSidestepChanged);
   FPSAxisHandler::SetFunctor lookLeftRightFunc(this, &FPSMotionModel::OnLookLeftRightChanged);
   FPSAxisHandler::SetFunctor lookUpDownFunc(this, &FPSMotionModel::OnLookUpDownChanged);

   mLookUpDownHandler      = new FPSAxisHandler(lookUpDownFunc);
   mLookLeftRightHandler   = new FPSAxisHandler(lookLeftRightFunc);
   mSidestepHandler        = new FPSAxisHandler(sideStepFunc);
   mForwardBackwardHandler = new FPSAxisHandler(fbFunc);

   if (keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &FPSMotionModel::OnSystem);

   mMouse    = mouse;
   mKeyboard = keyboard;
}

////////////////////////////////////////////////////////////////////////////////
FPSMotionModel::~FPSMotionModel()
{

   if (mLookUpDownAxis.get())
   {
      mLookUpDownAxis->RemoveAxisHandler(mLookUpDownHandler);
   }

   if (mTurnLeftRightAxis.get())
   {
      mTurnLeftRightAxis->RemoveAxisHandler(mLookLeftRightHandler);
   }

   if (mSidestepLeftRightAxis.get())
   {
      mSidestepLeftRightAxis->RemoveAxisHandler(mSidestepHandler);
   }

   if (mWalkForwardBackwardAxis.get())
   {
      mWalkForwardBackwardAxis->RemoveAxisHandler(mForwardBackwardHandler);
   }

   delete mForwardBackwardHandler;
   delete mSidestepHandler;
   delete mLookLeftRightHandler;
   delete mLookUpDownHandler;

   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetScene(Scene* scene)
{
   mScene = scene;
}

////////////////////////////////////////////////////////////////////////////////
Scene* FPSMotionModel::GetScene()
{
   return mScene.get();
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetEnabled(bool enabled)
{
   if (enabled && !MotionModel::IsEnabled())
   {
      if (mShouldRecenterMouse)
      {
         ResetMousePosition();
      }

      mForwardBackCtrl = 0.0f;
      mSidestepCtrl = 0.0f;
      mLookLeftRightCtrl = 0.0f;
      mLookUpDownCtrl = 0.0f;

      if (mLookUpDownAxis.valid())
      {
         mLookUpDownAxis->SetState(0.0f);
         mLookUpDownAxis->AddAxisHandler(mLookUpDownHandler);
      }
      if (mTurnLeftRightAxis.valid())
      {
         mTurnLeftRightAxis->SetState(0.0f);
         mTurnLeftRightAxis->AddAxisHandler(mLookLeftRightHandler);
      }
      if (mSidestepLeftRightAxis.valid())
      {
         mSidestepLeftRightAxis->AddAxisHandler(mSidestepHandler);
      }
      if (mWalkForwardBackwardAxis.valid())
      {
         mWalkForwardBackwardAxis->AddAxisHandler(mForwardBackwardHandler);
      }
   }
   if (!enabled && MotionModel::IsEnabled())
   {
      if (mLookUpDownAxis.valid())
      {
         mLookUpDownAxis->RemoveAxisHandler(mLookUpDownHandler);
      }
      if (mTurnLeftRightAxis.valid())
      {
         mTurnLeftRightAxis->RemoveAxisHandler(mLookLeftRightHandler);
      }
      if (mSidestepLeftRightAxis.valid())
      {
         mSidestepLeftRightAxis->RemoveAxisHandler(mSidestepHandler);
      }
      if (mWalkForwardBackwardAxis.valid())
      {
         mWalkForwardBackwardAxis->RemoveAxisHandler(mForwardBackwardHandler);
      }
   }

   MotionModel::SetEnabled(enabled);
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if (!mDefaultInputDevice.valid()) //if (mDefaultInputDevice.get() == 0)
   {
      mDefaultInputDevice = new LogicalInputDevice("FPSLogicalInputDevice");

      AxisToAxis* leftRightMouseMapping = new AxisToAxis(mouse->GetAxis(0));
      AxisToAxis* upDownMouseMapping = new AxisToAxis(mouse->GetAxis(1));

      AxesToAxis* forwardBack = new AxesToAxis();
      AxesToAxis* leftRight = new AxesToAxis();

      Axis* leftRightMouseMovement = mDefaultInputDevice->AddAxis(
         "left/right mouse movement",
          leftRightMouseMapping);

      Axis* upDownMouseMovement = mDefaultInputDevice->AddAxis(
         "up/down mouse movement",
          upDownMouseMapping);

      mAxisMappingList.push_back(leftRightMouseMapping);
      mAxisMappingList.push_back(upDownMouseMapping);
      mAxisMappingList.push_back(forwardBack);
      mAxisMappingList.push_back(leftRight);

      if (mUseWASD)
      {
         ButtonsToAxis* forwardBackMapping1 = new ButtonsToAxis(keyboard->GetButton('s'), keyboard->GetButton('w'));
         ButtonsToAxis* forwardBackMapping2 = new ButtonsToAxis(keyboard->GetButton('S'), keyboard->GetButton('S'));
         ButtonsToAxis* sideStepMapping1 = new ButtonsToAxis(keyboard->GetButton('a'), keyboard->GetButton('d'));
         ButtonsToAxis* sideStepMapping2 = new ButtonsToAxis(keyboard->GetButton('A'), keyboard->GetButton('D'));

         Axis* forwardAndBackAxis1 = mDefaultInputDevice->AddAxis("s/w", forwardBackMapping1);
         Axis* forwardAndBackAxis2 = mDefaultInputDevice->AddAxis("S/W", forwardBackMapping2);
         Axis* sideStepAxis1 = mDefaultInputDevice->AddAxis("a/d", sideStepMapping1);
         Axis* sideStepAxis2 = mDefaultInputDevice->AddAxis("A/D", sideStepMapping2);

         forwardBack->AddSourceAxis(forwardAndBackAxis1);
         forwardBack->AddSourceAxis(forwardAndBackAxis2);
         leftRight->AddSourceAxis(sideStepAxis1);
         leftRight->AddSourceAxis(sideStepAxis2);

         // Since we allocated the memory here, we'll be responsible for it with ref pointers
         mAxisMappingList.push_back(forwardBackMapping1);
         mAxisMappingList.push_back(forwardBackMapping2);
         mAxisMappingList.push_back(sideStepMapping1);
         mAxisMappingList.push_back(sideStepMapping2);
         mAxisMappingList.push_back(forwardBack);
         mAxisMappingList.push_back(leftRight);
      }

      if (mUseArrowKeys)
      {
         ButtonsToAxis* arrowKeysUpAndDownMapping = new ButtonsToAxis(
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Down),
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Up)
            );

         ButtonsToAxis* arrowKeysLeftAndRightMapping = new ButtonsToAxis(
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Left),
            keyboard->GetButton(osgGA::GUIEventAdapter::KEY_Right)
            );

         Axis* arrowKeysUpAndDown = mDefaultInputDevice->AddAxis(
            "arrow keys up/down", arrowKeysUpAndDownMapping);

         Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
            "arrow keys left/right", arrowKeysLeftAndRightMapping);

         // Since we allocated the memory here, we'll be responsible for it with ref pointers
         mAxisMappingList.push_back(arrowKeysUpAndDownMapping);
         mAxisMappingList.push_back(arrowKeysLeftAndRightMapping);

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

      AxesToAxis* defaultLeftRightMouseMapping = new AxesToAxis(leftRightMouseMovement);
      AxesToAxis* defaultUpDownMouseMapping = new AxesToAxis(upDownMouseMovement);

      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right", defaultLeftRightMouseMapping);

      mDefaultLookUpDownAxis = mDefaultInputDevice->AddAxis(
         "default look up/down", defaultUpDownMouseMapping);

      // Since we allocated the memory here, we'll be responsible for it with ref pointers
      mAxisMappingList.push_back(defaultLeftRightMouseMapping);
      mAxisMappingList.push_back(defaultUpDownMouseMapping);
   }

   SetWalkForwardBackwardAxis(mDefaultWalkForwardBackwardAxis.get());

   SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis.get());

   SetLookUpDownAxis(mDefaultLookUpDownAxis.get());

   SetSidestepLeftRightAxis(mDefaultSidestepLeftRightAxis.get());
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis)
{
   if (mWalkForwardBackwardAxis.valid())
   {
      mWalkForwardBackwardAxis->RemoveAxisHandler(mForwardBackwardHandler);
   }

   mWalkForwardBackwardAxis = walkForwardBackwardAxis;

   if (mWalkForwardBackwardAxis.valid())
   {
      mWalkForwardBackwardAxis->AddAxisHandler(mForwardBackwardHandler);
   }
}

////////////////////////////////////////////////////////////////////////////////
Axis* FPSMotionModel::GetWalkForwardBackwardAxis()
{
   return mWalkForwardBackwardAxis.get();
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   if (mTurnLeftRightAxis.valid())
   {
      mTurnLeftRightAxis->RemoveAxisHandler(mLookLeftRightHandler);
   }

   mTurnLeftRightAxis = turnLeftRightAxis;

   if (mTurnLeftRightAxis.valid())
   {
      mTurnLeftRightAxis->AddAxisHandler(mLookLeftRightHandler);
   }
}

////////////////////////////////////////////////////////////////////////////////
Axis* FPSMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis.get();
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetLookUpDownAxis(Axis* lookUpDownAxis)
{
   if (mLookUpDownAxis.valid())
   {
      mLookUpDownAxis->RemoveAxisHandler(mLookUpDownHandler);
   }

   mLookUpDownAxis = lookUpDownAxis;

   if (mLookUpDownAxis.valid())
   {
      mLookUpDownAxis->AddAxisHandler(mLookUpDownHandler);
   }
}

////////////////////////////////////////////////////////////////////////////////
Axis* FPSMotionModel::GetLookUpDownAxis()
{
   return mLookUpDownAxis.get();
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis)
{
   if (mSidestepLeftRightAxis.valid())
   {
      mSidestepLeftRightAxis->RemoveAxisHandler(mSidestepHandler);
   }

   mSidestepLeftRightAxis = sidestepLeftRightAxis;

   if (mSidestepLeftRightAxis.valid())
   {
      mSidestepLeftRightAxis->AddAxisHandler(mSidestepHandler);
   }
}

////////////////////////////////////////////////////////////////////////////////
Axis* FPSMotionModel::GetSidestepLeftRightAxis()
{
   return mSidestepLeftRightAxis.get();
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetMaximumWalkSpeed(float maximumWalkSpeed)
{
   mMaximumWalkSpeed = maximumWalkSpeed;
}

////////////////////////////////////////////////////////////////////////////////
float FPSMotionModel::GetMaximumWalkSpeed()
{
   return mMaximumWalkSpeed;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
   mMaximumTurnSpeed = maximumTurnSpeed;
}

////////////////////////////////////////////////////////////////////////////////
float FPSMotionModel::GetMaximumTurnSpeed()
{
   return mMaximumTurnSpeed;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetMaximumSidestepSpeed(float maximumSidestepSpeed)
{
   mMaximumSidestepSpeed = maximumSidestepSpeed;
}

////////////////////////////////////////////////////////////////////////////////
float FPSMotionModel::GetMaximumSidestepSpeed()
{
   return mMaximumSidestepSpeed;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetHeightAboveTerrain(float heightAboveTerrain)
{
   mHeightAboveTerrain = heightAboveTerrain;
}

////////////////////////////////////////////////////////////////////////////////
float FPSMotionModel::GetHeightAboveTerrain()
{
   return mHeightAboveTerrain;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetMaximumStepUpDistance(float maximumStepUpDistance)
{
/**
 * Sets the maximum step-up distance.  When clamping to the ground, the
 * maximum step-up distance determines whether to rise to a new level
 * (as when the model climbs a staircase) or to stay at the current level
 * (as when the model passes under a roof).  The default is 1.0.
 */
   mMaximumStepUpDistance = maximumStepUpDistance;
}

////////////////////////////////////////////////////////////////////////////////
float FPSMotionModel::GetMaximumStepUpDistance()
{
   return mMaximumStepUpDistance;
}

////////////////////////////////////////////////////////////////////////////////
float FPSMotionModel::GetFallingHeight() const
{
   return mFallingHeight;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{

   if (str == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
   {
      if (!IsCurrentlyActive()) {return;}

      // use the simulated change in time, not the real time change
      // see dtCore::System for the difference.
      // The ideal solution would be to use options, like FlyMotionModel
      double deltaFrameTime = deltaSim;

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

////////////////////////////////////////////////////////////////////////////////
bool FPSMotionModel::IsCurrentlyActive()
{
   bool result = false;

   result = GetTarget() != NULL && IsEnabled() &&
      (mOperateWhenUnfocused || mMouse->GetHasFocus());



   return result;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::ShouldOperateWhenUnfocused(bool operate)
{
   mOperateWhenUnfocused = operate;
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
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

   osg::Vec2 rotationMovement;
   if (mLookLeftRightCtrl != 0.0f && GetTurnLeftRightAxis())
   {
      rotationMovement.x() += GetTurnLeftRightAxis()->GetState();
      if (mShouldRecenterMouse)
      {
         GetTurnLeftRightAxis()->SetState(0.0f);
      }
   }
   if (mLookUpDownCtrl != 0.0f && GetLookUpDownAxis())
   {
      rotationMovement.y() += GetLookUpDownAxis()->GetState();
      if (mShouldRecenterMouse)
      {
         GetLookUpDownAxis()->SetState(0.0f);
      }
   }

   const bool calc_new_heading_pitch = !mUseMouseButtons || mMouse->GetButtonState(Mouse::LeftButton);
   const bool heading_changed = HasHeadingChanged(rotationMovement);

   // Once a new mouse movement has been triggered, we no longer need to reset the mouse.
   if (mpDebugger)
   {
      mpDebugger->Update(deltaTime, heading_changed);
   }

   if (calc_new_heading_pitch && heading_changed)
   {
      Transform transform;
      GetTarget()->GetTransform(transform);

      osg::Matrix rot;
      transform.GetRotation(rot);
      float deltaZ = rotationMovement[0] * mMaximumTurnSpeed;
      float deltaX = rotationMovement[1] * mMaximumTurnSpeed;

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

      // Reset the mouse position if necessary
      if (mShouldRecenterMouse)
      {
         ResetMousePosition();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::AdjustElevation(osg::Vec3& xyz, double deltaFrameTime)
{
   //Update the MotionModel's elevation by either ground clamping, or "falling"

   osg::Vec3 start(
      xyz[0],
      xyz[1],
      xyz[2] + mMaximumStepUpDistance - mHeightAboveTerrain
   );

   float heightOfTerrain = 0.0f;
   if (!mScene->GetHeightOfTerrain(heightOfTerrain, start.x(), start.y(), start.z(), start.z() - 10000))
   {
      // no intersection - lets just stay at the same elevation
      heightOfTerrain = xyz[2] - mHeightAboveTerrain;
   }

   // add in the offset distance off the height of terrain
   const float targetHeight = heightOfTerrain + mHeightAboveTerrain;

   // if we're too high off the terrain, then let gravity take over
   if ((xyz[2] - targetHeight) > mFallingHeight)
   {
      mFalling = true;
   }

   if (mFalling)
   {
      // adjust the position based on the gravity vector

      osg::Vec3 gravityVec(0.0,0.0,-9.8);

      mFallingVec += gravityVec * deltaFrameTime;

      // modify our position using the falling vector
      xyz += mFallingVec * deltaFrameTime;

      // make sure didn't fall below the terrain
      if (xyz[2] <= targetHeight)
      {
         // stop falling
         mFallingVec.set(0.0f, 0.0f, 0.0f);
         xyz[2] = targetHeight;
         mFalling = false;
      }
   }
   else // otherwise, lets clamp to the terrain
   {
      mFallingVec.set(0.0f, 0.0f, 0.0f);
      xyz[2] = targetHeight;
   }
}

////////////////////////////////////////////////////////////////////////////////
bool FPSMotionModel::HasHeadingChanged(const osg::Vec2& diff)
{
   // this is overly complicated and rather annoying however it fixes the 'drifting' bug
   // the use case is resizing the window with the motion model active and the camera will
   // drift indefinitely.  This is caused by setting the mouse position to (0.0, 0.0) which
   // does not work in the case that the window width or height is an odd number.  When the
   // window width or height is odd then the center of the screen is not an even number of pixels.
   // The result is that the mouse will get a difference of 1/width or 1/height for whichever is odd
   // it will then recenter the mouse again and we drift.  This code here uses an accumulated mouse movement
   // so the user has to move the mouse at least one pixel to get the motion model to update.

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

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::ResetMousePosition()
{
   mMouse->SetPosition(0.0f, 0.0f); // keeps cursor at center of screen
}

////////////////////////////////////////////////////////////////////////////////
bool FPSMotionModel::OnForwardBackwardChanged(double newState, double delta)
{
   mForwardBackCtrl = newState;
   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool FPSMotionModel::OnSidestepChanged(double newState, double delta)
{
   mSidestepCtrl = newState;
   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool FPSMotionModel::OnLookLeftRightChanged(double newState, double delta)
{
   mLookLeftRightCtrl = newState;
   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool FPSMotionModel::OnLookUpDownChanged(double newState, double delta)
{
   mLookUpDownCtrl = newState;
   return true;
}

////////////////////////////////////////////////////////////////////////////////
void FPSMotionModel::SetFallingHeight(float fallingHeight)
{
   mFallingHeight = fallingHeight;
}

////////////////////////////////////////////////////////////////////////////////
}
