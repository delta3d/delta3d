// walkmotionmodel.cpp: Implementation of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/fpsmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/inputdevice.h>
#include <dtCore/system.h>

using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(FPSMotionModel)

/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 */
FPSMotionModel::FPSMotionModel(  Keyboard* keyboard,
                                 Mouse* mouse) : MotionModel("FPSMotionModel"),
   mWalkForwardBackwardAxis(0),
   mTurnLeftRightAxis(0),
   mLookUpDownAxis(0),
   mSidestepLeftRightAxis(0),
   mMaximumWalkSpeed(10.0f),
   mMaximumTurnSpeed(1440.0f),
   mMaximumSidestepSpeed(10.0f),
   mHeightAboveTerrain(2.0f),
   mMaximumStepUpDistance(1.0f),
   mDownwardSpeed(10.0f)
{
   RegisterInstance(this);
   
   if(keyboard != 0 && mouse != 0)
   {
      SetDefaultMappings(keyboard, mouse);
   }
   
   AddSender(System::GetSystem());
   
   mMouse = mouse;
}

/**
 * Destructor.
 */
FPSMotionModel::~FPSMotionModel()
{
   RemoveSender(System::GetSystem());
   
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
   if(enabled)
   {
      mMouse->SetPosition(0.0f,0.0f);
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
   if(mDefaultInputDevice.get() == 0)
   {
      mDefaultInputDevice = new LogicalInputDevice;
      
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
void FPSMotionModel::SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis)
{
   mWalkForwardBackwardAxis = walkForwardBackwardAxis;
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
void FPSMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   mTurnLeftRightAxis = turnLeftRightAxis;  
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
void FPSMotionModel::SetLookUpDownAxis(Axis* lookUpDownAxis)
{
   mLookUpDownAxis = lookUpDownAxis;  
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
void FPSMotionModel::SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis)
{
   mSidestepLeftRightAxis = sidestepLeftRightAxis;
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
         
/**
 * Message handler callback.
 *
 * @param data the message data
 */
void FPSMotionModel::OnMessage(MessageData *data)
{
   if(GetTarget() != 0 &&
      IsEnabled() && 
      data->message == "preframe")
   {
      double deltaFrameTime = *reinterpret_cast<double*>(data->userData);

      Transform transform;

      GetTarget()->GetTransform(&transform);

      osg::Vec3 xyz, hpr;
      
      transform.GetRotation(hpr);

      if(mTurnLeftRightAxis->GetState() != 0)
      {
         hpr[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * deltaFrameTime);
         mMouse->SetPosition(0.0f,0.0f); //keeps cursor at center of screen
      }
      if(mLookUpDownAxis->GetState() != 0)
      {
         hpr[1] += float(mLookUpDownAxis->GetState() * mMaximumTurnSpeed * deltaFrameTime);
         mLookUpDownAxis->SetState(0.0f);//necessary to stop camera drifting down
         mMouse->SetPosition(0.0f,0.0f);//keeps cursor at center of screen
      }

      float temp_hpr = hpr[1]; //save current y axis value to be set later
      hpr[1] = 0.0f; //set to 0 to stop camera translating above terrain

      hpr[2] = 0.0f;
      transform.SetRotation(hpr);

      osg::Vec3 translation;

      if(mWalkForwardBackwardAxis != 0)
      {
         translation[1] = float(mWalkForwardBackwardAxis->GetState() * mMaximumWalkSpeed * deltaFrameTime);
      }

      if(mSidestepLeftRightAxis != 0)
      {
         translation[0] = float(mSidestepLeftRightAxis->GetState() * mMaximumSidestepSpeed * deltaFrameTime);
      }

      transform.GetTranslation(xyz);
      
      osg::Matrix mat;
      transform.GetRotation(mat);
      translation = translation * mat;

      //patch - stops camera from translating off terrain while looking up
      hpr[1] = temp_hpr;
      transform.SetRotation(hpr);

      xyz += translation;

      if(mScene.valid())
      {
         osgUtil::IntersectVisitor iv;

         osg::Vec3 start(
            xyz[0],
            xyz[1],
            xyz[2] + mMaximumStepUpDistance - mHeightAboveTerrain
            );

         osg::Vec3 end(
            xyz[0], 
            xyz[1], 
            xyz[2] - 10000.0f
            );

         osg::LineSegment* seg = new osg::LineSegment(start, end);

         iv.addLineSegment(seg);

         mScene->GetSceneNode()->accept(iv);

         float height = 0.0f;

         if(iv.hits())
         {
            height = iv.getHitList(seg)[0].getWorldIntersectPoint()[2];
         }

         height += mHeightAboveTerrain;

         if(xyz[2] <= height)
         {
            xyz[2] = height;

            mDownwardSpeed = 0.0f;
         }
         else if(xyz[2] > height)
         {
            xyz[2] -= float(mDownwardSpeed * deltaFrameTime);

            mDownwardSpeed += float(9.8 * deltaFrameTime);
         }
      }

      transform.SetTranslation(xyz);
      GetTarget()->SetTransform(&transform); 
   }
}
