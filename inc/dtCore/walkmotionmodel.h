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

#ifndef DELTA_WALKMOTIONMODEL
#define DELTA_WALKMOTIONMODEL

// walkmotionmodel.h: Declaration of the WalkMotionModel class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/motionmodel.h>

namespace dtCore
{
   //forward declaration
   class Scene;
   class Keyboard;
   class Mouse;
   class LogicalInputDevice;
   class ButtonAxisToAxis;
   class Axis;
   class AxisMapping;
   class ButtonsToAxis;
   class LogicalAxis;

   /**
    * A motion model that simulates the action of walking or driving.
    */
   class DT_CORE_EXPORT WalkMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(WalkMotionModel)

      public:

         /**
          * Constructor.
          *
          * @param keyboard the keyboard instance, or 0 to
          * avoid creating default input mappings
          * @param mouse the mouse instance, or 0 to avoid
          * creating default input mappings
          */
         WalkMotionModel(Keyboard* keyboard = NULL,
                         Mouse* mouse = NULL);

      protected:

         /**
          * Destructor.
          */
         virtual ~WalkMotionModel();

      public:

         /**
          * Sets the active Scene, which is used for ground following.
          *
          * @param scene the active scene
          */
         void SetScene(Scene* scene);

         /**
          * Returns the active Scene.
          *
          * @return the active Scene
          */
         Scene* GetScene();

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
          * @param walkForwardBackwardAxis the new forward/backward axis
          */
         void SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis);

         /**
          * Returns the axis that moves the target forwards (for positive
          * values) or backwards (for negative values).
          *
          * @return the current forward/backward axis
          */
         Axis* GetWalkForwardBackwardAxis();

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
          * Sets the axis that sidesteps the target left (for negative values)
          * or right (for positive values).
          *
          * @param sidestepLeftRightAxis the new sidestep left/right axis
          */
         void SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis);

         /**
          * Returns the axis that sidesteps the target left (for negative values)
          * or right (for positive values).
          *
          * @return the current sidestep left/right axis
          */
         Axis* GetSidestepLeftRightAxis();

         /**
          * Sets the maximum walk speed (meters per second).
          *
          * @param maximumWalkSpeed the new maximum walk speed
          */
         void SetMaximumWalkSpeed(float maximumWalkSpeed);

         /**
          * Returns the maximum walk speed (meters per second).
          *
          * @return the current maximum walk speed
          */
         float GetMaximumWalkSpeed();

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
          * Sets the maximum sidestep speed (meters per second).
          *
          * @param maximumSidestepSpeed the new maximum sidestep speed
          */
         void SetMaximumSidestepSpeed(float maximumSidestepSpeed);

         /**
          * Returns the maximum sidestep speed (meters per second).
          *
          * @return the current maximum sidestep speed
          */
         float GetMaximumSidestepSpeed();

         /**
          * Sets the height to maintain above the terrain (meters).
          *
          * @param heightAboveTerrain the height to maintain above the
          * terrain
          */
         void SetHeightAboveTerrain(float heightAboveTerrain);

         /**
          * Returns the height to maintain above the terrain (meters).
          *
          * @return the height to maintain above the terrain
          */
         float GetHeightAboveTerrain();

         /**
          * Sets the maximum step-up distance.  When clamping to the ground, the
          * maximum step-up distance determines whether to rise to a new level
          * (as when the model climbs a staircase) or to stay at the current level
          * (as when the model passes under a roof).  The default is 1.0.
          *
          * @param maximumStepUpDistance the new maximum step-up distance
          */
         void SetMaximumStepUpDistance(float maximumStepUpDistance);

         /**
          * Returns the current maximum step-up distance.
          *
          * @return the maximum step-up distance
          */
         float GetMaximumStepUpDistance();

         /**
          * Message handler callback.
          *
          * @param data the message data
          */
         virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;


      private:

         /**
          * A reference to the Scene, used for ground following.
          */
         RefPtr<Scene> mScene;

         /**
          * The default input device.
          */
         RefPtr<LogicalInputDevice> mDefaultInputDevice;

         /**
          * The left button up/down mapping.
          */
         dtCore::RefPtr<ButtonAxisToAxis> mLeftButtonUpDownMapping;

         /**
          * The left button right/left mapping.
          */
         dtCore::RefPtr<ButtonAxisToAxis> mLeftButtonLeftRightMapping;

         /**
          * The right button left/right mapping.
          */
         dtCore::RefPtr<ButtonAxisToAxis> mRightButtonLeftRightMapping;

         /**
          * The arrow key up/down mapping.
          */
         dtCore::RefPtr<ButtonsToAxis> mArrowKeysUpDownMapping;

         /**
          * The arrow key left/right mapping.
          */
         dtCore::RefPtr<ButtonsToAxis> mArrowKeysLeftRightMapping;

         /**
          * The a/d key left/right mapping.
          */
         dtCore::RefPtr<ButtonsToAxis> mADKeysLeftRightMapping;

         /**
          * A list of ref pointers to misc axis mappings created in this class.
          */
         std::vector<dtCore::RefPtr<AxisMapping> > mMiscAxisMappingList;

         /**
          * The default walk forward/backward axis.
          */
         LogicalAxis* mDefaultWalkForwardBackwardAxis;

         /**
          * The default turn left/right axis.
          */
         LogicalAxis* mDefaultTurnLeftRightAxis;

         /**
          * The default sidestep left/right axis.
          */
         LogicalAxis* mDefaultSidestepLeftRightAxis;

         /**
          * The axis that moves the target forwards or backwards.
          */
         Axis* mWalkForwardBackwardAxis;

         /**
          * The axis that turns the target to the left or right.
          */
         Axis* mTurnLeftRightAxis;

         /**
          * The axis that sidesteps the target left or right.
          */
         Axis* mSidestepLeftRightAxis;

         /**
          * The maximum walk speed (meters per second).
          */
         float mMaximumWalkSpeed;

         /**
          * The maximum turn speed (degrees per second).
          */
         float mMaximumTurnSpeed;

         /**
          * The maximum sidestep speed (meters per second).
          */
         float mMaximumSidestepSpeed;

         /**
          * The height to maintain above terrain (meters).
          */
         float mHeightAboveTerrain;

         /**
          * The maximum step-up distance (meters).
          */
         float mMaximumStepUpDistance;

         /**
          * The current downward speed.
          */
         float mDownwardSpeed;
   };
}

#endif // DELTA_WALKMOTIONMODEL
