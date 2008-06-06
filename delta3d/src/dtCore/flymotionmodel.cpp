// flymotionmodel.cpp: Implementation of the FlyMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/flymotionmodel.h>

#include <osg/Vec3>
#include <osg/Matrix>

#include <dtCore/system.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
//#include <dtCore/transform.h>
//#include <dtCore/transformable.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h>
#include <dtCore/keyboard.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(FlyMotionModel)

/**
 * Constructor.
 *
 * @param keyboard the keyboard instance, or 0 to
 * avoid creating default input mappings
 * @param mouse the mouse instance, or 0 to avoid
 * creating default input mappings
 * @param useSimTimeForSpeed true if the motion model should use the 
 * simulation time, which can be scaled, for motion or false if it   
 * should use the real time.
 * @param requireMouseDown true if the mouse button must be held
 * down for mouse motions to control the camera. (Default is true.)
 * @param resetMouseCursor true if the mouse cursor should reset to
 * the center of the scene each frame. (Default is false.)
 */
FlyMotionModel::FlyMotionModel(Keyboard *keyboard,
                               Mouse *mouse,                             
                               BehaviorOptions options)
   : MotionModel("FlyMotionModel"),
     mLeftButtonUpDownMapping(NULL),
     mLeftButtonLeftRightMapping(NULL),
     mRightButtonUpDownMapping(NULL),
     mRightButtonLeftRightMapping(NULL),
     mArrowKeysUpDownMapping(NULL),
     mArrowKeysLeftRightMapping(NULL),
     mWSKeysUpDownMapping(NULL),
     mADKeysLeftRightMapping(NULL),
     mQEKeysUpDownMapping(NULL),
     mDefaultFlyForwardBackwardAxis(NULL),
     mDefaultFlyLeftRightAxis(NULL),
     mDefaultFlyUpDownAxis(NULL),
     mDefaultTurnLeftRightAxis(NULL),
     mDefaultTurnUpDownAxis(NULL),
     mFlyForwardBackwardAxis(NULL),
     mFlyLeftRightAxis(NULL),
     mFlyUpDownAxis(NULL),
     mTurnLeftRightAxis(NULL),
     mTurnUpDownAxis(NULL),
     mMaximumFlySpeed(100.0f),
     mMaximumTurnSpeed(90.0f),   
     mMouse(mouse),
     mOptions(options)
{

   RegisterInstance(this);
   
   if(keyboard != NULL && mouse != NULL)
   {
      SetDefaultMappings(keyboard, mouse);
   }
   
   AddSender(&System::GetInstance());
}

/**
 * Destructor.
 */
FlyMotionModel::~FlyMotionModel()
{
   RemoveSender(&System::GetInstance());
   
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
   if(!mDefaultInputDevice.valid())
   {
      mDefaultInputDevice = new LogicalInputDevice;

      Axis *leftButtonUpAndDown, *leftButtonLeftAndRight;

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
         leftButtonUpAndDown = mDefaultInputDevice->AddAxis(
            "left mouse movement up/down",
            new AxisToAxis(mouse->GetAxis(1))
         );

         leftButtonLeftAndRight = mDefaultInputDevice->AddAxis(
            "left mouse movement left/right",
            new AxisToAxis(mouse->GetAxis(0))
         );
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
         mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
            "default turn left/right",
            new AxisToAxis(mouse->GetAxis(0))
            );

         mDefaultTurnUpDownAxis = mDefaultInputDevice->AddAxis(
            "default turn up/down",
            new AxisToAxis(mouse->GetAxis(1))
            );
      }
      
      Axis* wsKeysUpAndDown = mDefaultInputDevice->AddAxis(
         "w/s keys stafe forward/back",
         mWSKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton('s'),
            keyboard->GetButton('w')
         )
      );
      
      Axis* adKeysStrafeLeftAndRight = mDefaultInputDevice->AddAxis(
         "a/d keys strafe left/right",
         mADKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton('a'),
            keyboard->GetButton('d')
         )
      );

      Axis* qeKeysFlyUpAndDown = mDefaultInputDevice->AddAxis(
         "a/d keys fly up/down",
         mQEKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton('q'),
            keyboard->GetButton('e')
         )
      );

      mDefaultFlyForwardBackwardAxis = mDefaultInputDevice->AddAxis(
         "default fly forward/backward",
         new AxesToAxis(wsKeysUpAndDown, rightButtonUpAndDown)
      );
      
      mDefaultFlyLeftRightAxis = mDefaultInputDevice->AddAxis(
         "default fly left/right",
         new AxesToAxis(adKeysStrafeLeftAndRight, rightButtonLeftAndRight)
      );

      mDefaultFlyUpDownAxis = mDefaultInputDevice->AddAxis(
         "default fly up/down",
         new AxesToAxis(qeKeysFlyUpAndDown, 0) // not sure what to map for right 2nd parameter (?)
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

      mQEKeysUpDownMapping->SetSourceButtons(
         keyboard->GetButton('q'),
         keyboard->GetButton('e')
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

void FlyMotionModel::SetUseSimTimeForSpeed(bool useSimTimeForSpeed)
{
   unsigned int newOptions = (unsigned int)mOptions | (unsigned int)OPTION_USE_SIMTIME_FOR_SPEED;
   mOptions = (BehaviorOptions)newOptions; 
}

/**
 * Message handler callback.
 *
 * @param data the message data
 */
void FlyMotionModel::OnMessage(MessageData *data)
{
   if(GetTarget() != 0 &&
      IsEnabled() && 
      (data->message == "preframe" || 
        (!HasOption(OPTION_USE_SIMTIME_FOR_SPEED) && data->message == "pause")))
   {
      // Get the time change (sim time or real time)
      double delta;
      double* timeChange = (double*)data->userData;
      if (data->message == "pause") // paused and !useSimTime
         delta = *timeChange; // 0 is real time when paused
      else if (HasOption(OPTION_USE_SIMTIME_FOR_SPEED)) 
         delta = timeChange[0]; // 0 is sim time
      else
         delta = timeChange[1]; // 1 is real time

      Transform transform;
      
      GetTarget()->GetTransform(transform);
      
      osg::Vec3 xyz, hpr;
      
      transform.Get(xyz, hpr);

      // rotation

      if(mTurnLeftRightAxis != 0)
      {
         hpr[0] -= float(mTurnLeftRightAxis->GetState() * mMaximumTurnSpeed * delta);
      }
      
      if(mTurnUpDownAxis != 0)
      {
         float rotateTo = hpr[1] + float(mTurnUpDownAxis->GetState() * mMaximumTurnSpeed * delta);

         if( rotateTo < -89.5f )
         {
            hpr[1] = -89.5f;
         }
         else if( rotateTo > 89.5f )
         {
            hpr[1] = 89.5f;
         }
         else
         {
            hpr[1] = rotateTo;
         }
      }
      
      hpr[2] = 0.0f;
      
      transform.SetRotation(hpr);

      if (HasOption(OPTION_RESET_MOUSE_CURSOR))
      {       
         // fix to avoid camera drift
         mTurnUpDownAxis->SetState(0.0f); // necessary to stop camera drifting down
         mTurnLeftRightAxis->SetState(0.0f); // necessary to stop camera drifting left                

         mMouse->SetPosition(0.0f,0.0f); // keeps cursor at center of screen
      }

      // translation

      osg::Vec3 translation;
      
      if(mFlyForwardBackwardAxis != 0)
      {
         translation[1] = float(mFlyForwardBackwardAxis->GetState() * mMaximumFlySpeed * delta);
      }
      
      if(mFlyLeftRightAxis != 0)
      {
         translation[0] = float(mFlyLeftRightAxis->GetState() * mMaximumFlySpeed * delta);
      }

      if(mFlyUpDownAxis != 0)
      {
         translation[2] = float(mFlyUpDownAxis->GetState() * mMaximumFlySpeed * delta);
      }

      osg::Matrix mat;
      
      transform.GetRotation(mat);
      
      translation = osg::Matrix::transform3x3(translation, mat);
      
      xyz += translation;
      
      transform.SetTranslation(xyz);

      // finalize changes

      GetTarget()->SetTransform(transform);  
   }
}
