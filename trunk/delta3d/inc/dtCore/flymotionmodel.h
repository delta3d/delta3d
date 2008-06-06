/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
*/

#ifndef DELTA_FLYMOTIONMODEL
#define DELTA_FLYMOTIONMODEL

// flymotionmodel.h: Declaration of the FlyMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/motionmodel.h>

namespace dtCore
{
   class Axis;
   class ButtonAxisToAxis;
   class ButtonsToAxis;
   class Keyboard;
   class LogicalAxis;
   class LogicalInputDevice;
   class Mouse;
   
   /**
    * A motion model that simulates the action of flying.
    */
   class DT_CORE_EXPORT FlyMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(FlyMotionModel)

      public:

         enum BehaviorOptions
         {       
            OPTION_NONE                  = 0x0,
            OPTION_USE_SIMTIME_FOR_SPEED = 0x1,
            OPTION_REQUIRE_MOUSE_DOWN    = 0x2,
            OPTION_RESET_MOUSE_CURSOR    = 0x4,
            OPTION_USE_CURSOR_KEYS       = 0x8,
            OPTION_DEFAULT               = OPTION_USE_CURSOR_KEYS | OPTION_REQUIRE_MOUSE_DOWN

            /** OPTION_USE_SIMTIME_FOR_SPEED
            * Indicates whether the fly motion model will use Sim Time
            * or not (aka Real Time) for speed of movement and turn rotation
            */

            /** OPTION_REQUIRE_MOUSE_DOWN
            * Indicates whether the fly motion model will require the mouse
            * button(s) depressed in order to control turn rotation (defaults
            * to true)
            */

            /** OPTION_RESET_MOUSE_CURSOR
            * Indicates whether the fly motion model will reset the mouse
            * cursor coordinates to the center of the screen each frame
            * (defaults to false)
            */

            /** OPTION_USE_CURSOR_KEYS
            * Indicates whether the fly motion model will move the camera
            * in response to pressing the cursor keys.
            * (defaults to true)
            */
         };

         /**
          * Constructor.
          *
          * @param keyboard the keyboard instance, or NULL to
          * avoid creating default input mappings
          * @param mouse the mouse instance, or NULL to avoid
          * creating default input mappings
          * @param useSimTimeForSpeed true if the motion model should use the 
          * simulation time, which can be scaled, for motion or false if it 
          * should use the real time.
          * @param requireMouseDown true if the mouse button must be held
          * down for mouse motions to control the camera. (Default is true.)
          * @param resetMouseCursor true if the mouse cursor should reset to
          * the center of the scene each frame. (Default is false.)
          */
         FlyMotionModel(Keyboard* keyboard = NULL,
                        Mouse* mouse = NULL,                        
                        BehaviorOptions options = OPTION_DEFAULT);

      protected:

         /**
          * Destructor.
          */
         virtual ~FlyMotionModel();

      public:

         /**
         * Returns whether the motion model has been created with a 
         * specific behavior option
         *
         * @param option the option to query         
         */
         bool HasOption(BehaviorOptions option) { return (mOptions & option) > 0; }
         
         /**
          * Sets the input axes to a set of default mappings for mouse
          * and keyboard.
          *
          * @param keyboard the keyboard instance
          * @param mouse the mouse instance
          */
         void SetDefaultMappings(Keyboard *keyboard, Mouse *mouse);
         
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
          * Sets the axis that moves the target left (for positive
          * values) or right (for negative values).
          *
          * @param flyLeftRightAxis the new left/right axis
          */
         void SetFlyLeftRightAxis(Axis* flyLeftRightAxis);
         
         /**
          * Returns the axis that moves the target left (for positive
          * values) or right (for negative values).
          *
          * @return the current left/right axis
          */
         Axis* GetFlyLeftRightAxis();

         /**
         * Sets the axis that moves the target target-relative up
         * (for positive values) or target-relative down (for
         * negative values).
         *
         * @param flyUpDownAxis the new up/down axis
         */
         void SetFlyUpDownAxis(Axis* flyUpDownAxis);

         /**
         * Returns the axis that moves the target target-relative up
         * (for positive values) or target-relative down (for negative
         * values).
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
          * Sets whether we use sim time or not (aka real time)
          * for the speed of movement and rotation.
          */
         void SetUseSimTimeForSpeed(bool useSimTimeForSpeed);
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
         RefPtr<LogicalInputDevice> mDefaultInputDevice;
         
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
          * The w/s key forward/backward mapping.
          */
         ButtonsToAxis* mWSKeysUpDownMapping;
         
         /**
          * The a/d key strafe left/right mapping.
          */
         ButtonsToAxis* mADKeysLeftRightMapping;

         /**
         * The q/e key fly up/down mapping.
         */
         ButtonsToAxis* mQEKeysUpDownMapping;

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
          * The default turn up/down axis.
          */
         LogicalAxis* mDefaultTurnUpDownAxis;
         
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

         Mouse *mMouse;

         /**
         * The bitwise combination of the types of behavior this
         * motion model will enforce
         */
         BehaviorOptions mOptions;
   };
}


#endif // DELTA_FLYMOTIONMODEL
