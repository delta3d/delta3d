// ufomotionmodel.h: Declaration of the UFOMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_UFO_MOTION_MODEL
#define DELTA_UFO_MOTION_MODEL

#include "keyboard.h"
#include "mouse.h"
#include "logicalinputdevice.h"
#include "motionmodel.h"
#include "inputdevice.h"
#include "system.h"

namespace dtCore
{
   /**
    * A motion model that simulates the action of flying in a UFO.
    */
   class DT_EXPORT UFOMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(UFOMotionModel)


      public:

         /**
          * Constructor.
          *
          * @param keyboard the keyboard instance, or NULL to
          * avoid creating default input mappings
          * @param mouse the mouse instance, or NULL to avoid
          * creating default input mappings
          */
         UFOMotionModel(Keyboard* keyboard = NULL,
                        Mouse* mouse = NULL);

         /**
          * Destructor.
          */
         virtual ~UFOMotionModel();
         
         /**
          * Sets the input axes to a set of default mappings for mouse
          * and keyboard.
          *
          * @param keyboard the keyboard instance
          * @param mouse the mouse instance
          */
         void SetDefaultMappings(Keyboard* keyboard, Mouse* mouse);
         
         /**
          * Sets the axis that moves the target forwards (for positive
          * values) or backwards (for negative values).
          *
          * @param flyForwardBackwardAxis the new forward/backward axis
          */
         void SetFlyForwardBackwardAxis(Axis* flyForwardBackwardAxis);
         
         /**
          * Returns the axis that moves the target forwards (for positive
          * values) or backwards (for negative values).
          *
          * @return the current forward/backward axis
          */
         Axis* GetFlyForwardBackwardAxis();
   
         /**
          * Sets the axis that moves the target left (for negative
          * values) or right (for positive values).
          *
          * @param flyLeftRightAxis the new left/right axis
          */
         void SetFlyLeftRightAxis(Axis* flyLeftRightAxis);
         
         /**
          * Returns the axis that moves the target left (for negative
          * values) or right (for positive values).
          *
          * @return the current left/right axis
          */
         Axis* GetFlyLeftRightAxis();
         
         /**
          * Sets the axis that moves the target up (for positive
          * values) or down (for negative values).
          *
          * @param flyUpDownAxis the new up/down axis
          */
         void SetFlyUpDownAxis(Axis* flyUpDownAxis);
         
         /**
          * Returns the axis that moves the target up (for positive
          * values) or down (for negative values).
          *
          * @return the current up/down axis
          */
         Axis* GetFlyUpDownAxis();
         
         /**
          * Sets the axis that turns the target left (for negative values)
          * or right (for positive values).
          *
          * @param turnLeftRightAxis the new turn left/right axis
          */
         void SetTurnLeftRightAxis(Axis* turnLeftRightAxis);
         
         /**
          * Returns the axis that turns the target left (for negative values)
          * or right (for positive values).
          *
          * @return the current turn left/right axis
          */
         Axis* GetTurnLeftRightAxis();
         
         /**
          * Sets the maximum fly speed (meters per second).
          *
          * @param maximumFlySpeed the new maximum fly speed
          */
         void SetMaximumFlySpeed(float maximumFlySpeed);
         
         /**
          * Returns the maximum fly speed (meters per second).
          *
          * @return the current maximum fly speed
          */
         float GetMaximumFlySpeed();
   
         /**
          * Sets the maximum turn speed (degrees per second).
          *
          * @param maximumTurnSpeed the new maximum turn speed
          */
         void SetMaximumTurnSpeed(float maximumTurnSpeed);
         
         /**
          * Returns the maximum turn speed (degrees per second).
          *
          * @return the current maximum turn speed
          */
         float GetMaximumTurnSpeed();
         
         /**
          * Message handler callback.
          *
          * @param data the message data
          */
         virtual void OnMessage(MessageData *data);
         
         
      private:
      
         /**
          * The default input device.
          */
         osg::ref_ptr<LogicalInputDevice> mDefaultInputDevice;
         
         /**
          * The left button up/down mapping.
          */
         ButtonAxisToAxis* mLeftButtonUpDownMapping;
         
         /**
          * The left button right/left mapping.
          */
         ButtonAxisToAxis* mLeftButtonLeftRightMapping;
         
         /**
          * The right button up/down mapping.
          */
         ButtonAxisToAxis* mRightButtonUpDownMapping;
         
         /**
          * The right button left/right mapping.
          */
         ButtonAxisToAxis* mRightButtonLeftRightMapping;
         
         /**
          * The arrow key up/down mapping.
          */
         ButtonsToAxis* mArrowKeysUpDownMapping;
         
         /**
          * The arrow key left/right mapping.
          */
         ButtonsToAxis* mArrowKeysLeftRightMapping;
         
         /**
          * The w/s key up/down mapping.
          */
         ButtonsToAxis* mWSKeysUpDownMapping;
         
         /**
          * The a/d key left/right mapping.
          */
         ButtonsToAxis* mADKeysLeftRightMapping;
         
         /**
          * The default fly forward/backward axis.
          */
         LogicalAxis* mDefaultFlyForwardBackwardAxis;
         
         /**
          * The default fly left/right axis.
          */
         LogicalAxis* mDefaultFlyLeftRightAxis;
         
         /**
          * The default fly up/down axis.
          */
         LogicalAxis* mDefaultFlyUpDownAxis;
         
         /**
          * The default turn left/right axis.
          */
         LogicalAxis* mDefaultTurnLeftRightAxis;
         
         /**
          * The axis that moves the target forwards or backwards.
          */
         Axis* mFlyForwardBackwardAxis;
         
         /**
          * The axis that moves the target left or right.
          */
         Axis* mFlyLeftRightAxis;
         
         /**
          * The axis that moves the target up or down.
          */
         Axis* mFlyUpDownAxis;
         
         /**
          * The axis that turns the target left or right.
          */
         Axis* mTurnLeftRightAxis;
         
         /**
          * The maximum fly speed (meters per second).
          */
         float mMaximumFlySpeed;
         
         /**
          * The maximum turn speed (degrees per second).
          */
         float mMaximumTurnSpeed;
   };
};

#endif // DELTA_UFO_MOTION_MODEL