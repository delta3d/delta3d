// walkmotionmodel.cpp: Implementation of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/walkmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>

#include <osg/Vec3>
#include <osgUtil/IntersectVisitor>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(WalkMotionModel)

/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 */
WalkMotionModel::WalkMotionModel(Keyboard* keyboard,
                                 Mouse* mouse)
   : MotionModel("WalkMotionModel"),
     mWalkForwardBackwardAxis(0),
     mTurnLeftRightAxis(0),
     mSidestepLeftRightAxis(0),
     mMaximumWalkSpeed(10.0f),
     mMaximumTurnSpeed(90.0f),
     mMaximumSidestepSpeed(5.0f),
     mHeightAboveTerrain(2.0f),
     mMaximumStepUpDistance(1.0f),
     mDownwardSpeed(0.0f)
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
WalkMotionModel::~WalkMotionModel()
{
   RemoveSender(System::Instance());
   
   DeregisterInstance(this);
}

/**
 * Sets the active Scene, which is used for ground following.
 *
 * @param scene the active scene
 */
void WalkMotionModel::SetScene(Scene* scene)
{
   mScene = scene;
}

/**
 * Returns the active Scene.
 *
 * @return the active Scene
 */
Scene* WalkMotionModel::GetScene()
{
   return mScene.get();
}

/**
 * Sets the input axes to a set of default mappings for mouse
 * and keyboard.
 *
 * @param keyboard the keyboard instance
 * @param mouse the mouse instance
 */
void WalkMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
{
   if(mDefaultInputDevice.get() == 0)
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
      
      Axis* adKeysLeftAndRight = mDefaultInputDevice->AddAxis(
         "a/d keys left/right",
         mADKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton(Producer::Key_A),
            keyboard->GetButton(Producer::Key_D)
         )
      );
            
      mDefaultWalkForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default walk forward/backward",
         new AxesToAxis(arrowKeysUpAndDown, leftButtonUpAndDown)
      );
         
      mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default turn left/right",
         new AxesToAxis(arrowKeysLeftAndRight, leftButtonLeftAndRight)
      );
         
      mDefaultSidestepLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default sidestep left/right",
         new AxesToAxis(adKeysLeftAndRight, rightButtonLeftAndRight)
      );
   }
   else
   {
      mLeftButtonUpDownMapping->SetSourceButton(mouse->GetButton(Mouse::LeftButton));
      mLeftButtonUpDownMapping->SetSourceAxis(mouse->GetAxis(1));
      
      mLeftButtonLeftRightMapping->SetSourceButton(mouse->GetButton(Mouse::LeftButton));
      mLeftButtonLeftRightMapping->SetSourceAxis(mouse->GetAxis(0));
      
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
      
      mADKeysLeftRightMapping->SetSourceButtons(
         keyboard->GetButton(Producer::Key_A),
         keyboard->GetButton(Producer::Key_D)
      );
   }
   
   SetWalkForwardBackwardAxis(mDefaultWalkForwardBackwardAxis);
      
   SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis);
         
   SetSidestepLeftRightAxis(mDefaultSidestepLeftRightAxis);
}
         
/**
 * Sets the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @param walkForwardBackwardAxis the new forward/backward axis
 */
void WalkMotionModel::SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis)
{
   mWalkForwardBackwardAxis = walkForwardBackwardAxis;
}

/**
 * Returns the axis that moves the target forwards (for positive
 * values) or backwards (for negative values).
 *
 * @return the current forward/backward axis
 */
Axis* WalkMotionModel::GetWalkForwardBackwardAxis()
{
   return mWalkForwardBackwardAxis;
}

/**
 * Sets the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @param turnLeftRightAxis the new turn left/right axis
 */
void WalkMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
{
   mTurnLeftRightAxis = turnLeftRightAxis;  
}

/**
 * Returns the axis that turns the target left (for negative values)
 * or right (for positive values).
 *
 * @return the current turn left/right axis
 */
Axis* WalkMotionModel::GetTurnLeftRightAxis()
{
   return mTurnLeftRightAxis;
}

/**
 * Sets the axis that sidesteps the target left (for negative values)
 * or right (for positive values).
 *
 * @param sidestepLeftRightAxis the new sidestep left/right axis
 */
void WalkMotionModel::SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis)
{
   mSidestepLeftRightAxis = sidestepLeftRightAxis;
}

/**
 * Returns the axis that sidesteps the target left (for negative values)
 * or right (for positive values).
 *
 * @return the current sidestep left/right axis
 */
Axis* WalkMotionModel::GetSidestepLeftRightAxis()
{
   return mSidestepLeftRightAxis;
}

/**
 * Sets the maximum walk speed (meters per second).
 *
 * @param maximumWalkSpeed the new maximum walk speed
 */
void WalkMotionModel::SetMaximumWalkSpeed(float maximumWalkSpeed)
{
   mMaximumWalkSpeed = maximumWalkSpeed;
}

/**
 * Returns the maximum walk speed (meters per second).
 *
 * @return the current maximum walk speed
 */
float WalkMotionModel::GetMaximumWalkSpeed()
{
   return mMaximumWalkSpeed;
}

/**
 * Sets the maximum turn speed (degrees per second).
 *
 * @param maximumTurnSpeed the new maximum turn speed
 */
void WalkMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
{
   mMaximumTurnSpeed = maximumTurnSpeed;
}

/**
 * Returns the maximum turn speed (degrees per second).
 *
 * @return the current maximum turn speed
 */
float WalkMotionModel::GetMaximumTurnSpeed()
{
   return mMaximumTurnSpeed;
}

/**
 * Sets the maximum sidestep speed (meters per second).
 *
 * @param maximumSidestepSpeed the new maximum sidestep speed
 */
void WalkMotionModel::SetMaximumSidestepSpeed(float maximumSidestepSpeed)
{
   mMaximumSidestepSpeed = maximumSidestepSpeed;
}

/**
 * Returns the maximum sidestep speed (meters per second).
 *
 * @return the current maximum sidestep speed
 */
float WalkMotionModel::GetMaximumSidestepSpeed()
{
   return mMaximumSidestepSpeed;
}

/**
 * Sets the height to maintain above the terrain (meters).
 *
 * @param heightAboveTerrain the height to maintain above the
 * terrain
 */
void WalkMotionModel::SetHeightAboveTerrain(float heightAboveTerrain)
{
   mHeightAboveTerrain = heightAboveTerrain;
}

/**
 * Returns the height to maintain above the terrain (meters).
 *
 * @return the height to maintain above the terrain
 */
float WalkMotionModel::GetHeightAboveTerrain()
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
void WalkMotionModel::SetMaximumStepUpDistance(float maximumStepUpDistance)
{
   mMaximumStepUpDistance = maximumStepUpDistance;
}

/**
 * Returns the current maximum step-up distance.
 *
 * @return the maximum step-up distance
 */
float WalkMotionModel::GetMaximumStepUpDistance()
{
   return mMaximumStepUpDistance;
}
         
/**
 * Message handler callback.
 *
 * @param data the message data
 */
void WalkMotionModel::OnMessage(MessageData *data)
{
   if(GetTarget() != 0 &&
      IsEnabled() && 
      data->message == "preframe")
   {
      double dtCore = *static_cast<double*>(data->userData);
      
      Transform transform;
      
      GetTarget()->GetTransform(&transform);
      
      osg::Vec3 xyz, hpr, scale;
      
      transform.Get(xyz, hpr, scale);
      
      if(mTurnLeftRightAxis != 0)
      {
         hpr[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * dtCore);
      }
      
      hpr[1] = 0.0f;
      hpr[2] = 0.0f;
      
      transform.SetRotation(hpr);
      
      osg::Vec3 translation(0, 0, 0);
      
      if(mWalkForwardBackwardAxis != 0)
      {
         translation[1] = float(mWalkForwardBackwardAxis->GetState() * mMaximumWalkSpeed * dtCore);
      }
      
      if(mSidestepLeftRightAxis != 0)
      {
         translation[0] = float(mSidestepLeftRightAxis->GetState() * mMaximumSidestepSpeed * dtCore);
      }
      
      osg::Matrix mat;
      
      transform.GetRotation(mat);
      
      //sgXformVec3(translation, mat);
      translation = osg::Matrix::transform3x3(translation, mat);
      
      //sgAddVec3(xyz, translation);
      xyz += translation;
      
      if(mScene.get() != 0)
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
            xyz[2] -= float(mDownwardSpeed * dtCore);
            
            mDownwardSpeed += float(9.8 * dtCore);
         }
      }
      
      transform.SetTranslation(xyz);
      
      GetTarget()->SetTransform(&transform);  
   }
}

}
