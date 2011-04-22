// flymotionmodel.h: Declaration of the FlyMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_FLY_MOTION_MODEL
#define DELTA_FLY_MOTION_MODEL

#include "keyboard.h"
#include "mouse.h"
#include "logicalinputdevice.h"
#include "motionmodel.h"
#include "inputdevice.h"
#include "system.h"

namespace dtCore
{
   /**
    * A motion model that simulates the action of flying.
    */
   class FlyMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(FlyMotionModel)


      public:

         /**
          * Constructor.
          *
          * @param keyboard the keyboard instance, or NULL to
          * avoid creating default input mappings
          * @param mouse the mouse instance, or NULL to avoid
          * creating default input mappings
          */
         FlyMotionModel(Keyboard* keyboard = NULL,
                        Mouse* mouse = NULL);

         /**
          * Destructor.
          */
         virtual ~FlyMotionModel();
         
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
          * Sets the axis that turns the target up (for positive values)
          * or down (for negative values).
          *
          * @param turnUpDownAxis the new turn up/down axis
          */
         void SetTurnUpDownAxis(Axis* turnUpDownAxis);
         
         /**
          * Returns the axis that turns the target up (for positive values)
          * or down (for negative values).
          *
          * @return the current turn up/down axis
          */
         Axis* GetTurnUpDownAxis();
         
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
          * The default fly forward/backward axis.
          */
         LogicalAxis* mDefaultFlyForwardBackwardAxis;
         
         /**
          * The default turn left/right axis.
          */
         LogicalAxis* mDefaultTurnLeftRightAxis;
         
         /**
          * The default turn up/down axis.
          */
         LogicalAxis* mDefaultTurnUpDownAxis;
         
         /**
          * The axis that moves the target forwards or backwards.
          */
         Axis* mFlyForwardBackwardAxis;
         
         /**
          * The axis that turns the target left or right.
          */
         Axis* mTurnLeftRightAxis;
         
         /**
          * The axis that turns the target up or down.
          */
         Axis* mTurnUpDownAxis;
         
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

#endif // DELTA_FLY_MOTION_MODEL