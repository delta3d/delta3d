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

#ifndef DELTA_FPSMOTIONMODEL
#define DELTA_FPSMOTIONMODEL

// fpsmotionmodel.h: Declaration of the FPSMotionModel class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtCore/motionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>
#include <osg/Vec3>

////////////////////////////////////////////////////////////////////////////////

namespace dtCore
{
   class Axis;
   class LogicalAxis;
   class FPSAxisHandler;

   /**
    * A motion model used for typical First Person Shooter motion.
    */
   class DT_CORE_EXPORT FPSMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(FPSMotionModel)

   public:
      /**
       * Constructor.
       *
       * @param keyboard the keyboard instance, or 0 to
       * avoid creating default input mappings
       * @param mouse the mouse instance, or 0 to avoid
       * creating default input mappings
       * @param maxWalkSpeed the maximum walking speed
       * @param maxTurnSpeed the maximum turning speed
       * @param maxSidestepSpeed the maximum sidestep speed
       * @param height the height above the terrain at which we stand
       * @param maxStepUpDist the maximum distance we can step up to higher terrain
       */
      FPSMotionModel(Keyboard* keyboard = 0,
                     Mouse* mouse = 0,
                     float maxWalkSpeed = 5.0f,
                     float maxTurnSpeed = 1.5f,
                     float maxSidestepSpeed = 5.0f,
                     float height = 2.0f,
                     float maxStepUpDist = 1.0f,
                     bool useWASD = true,
                     bool useArrowKeys = true);

   protected:
      /**
       * Destructor.
       */
      virtual ~FPSMotionModel();

      /**
       * This method can be overwritten in subclasses to produce
       * desired rotation behavior
       *
       * @param deltaTime The time change
       */
      virtual void UpdateMouse(const double deltaTime);

      /**
       * This method can be overwritten in subclasses to produce
       * desired translation behavior
       * Note: Any collision detection/response and other physical
       * constraints should be applied here
       *
       * @param deltaTime The time change
       */
      virtual void PerformTranslation(const double deltaTime);

      /**
       * Returns the factor [-1.0, 1.0] to side-step by
       *
       * @return the current sidestep factor
       */
      float GetSidestepFactor() const { return mSidestepCtrl; }

      /**
       * Returns the factor [-1.0, 1.0] to walk by
       *
       * @return the current walk factor
       */
      float GetForwardBackFactor() const { return mForwardBackCtrl; }

      /**
       * Returns the mouse
       *
       * @return the mouse
       */
      Mouse* GetMouse() const { return mMouse.get(); }

      /**
       * Returns the keyboard
       *
       * @return the keyboard
       */
      Keyboard* GetKeyboard() const { return mKeyboard.get(); }



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
       * Enables or disables this motion model.
       *
       * @param enabled true to enable this motion model, false
       * to disable it
       */
      virtual void SetEnabled(bool enabled);

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
       * Sets the axis looks down (for negative values)
       * or up (for positive values).
       *
       * @param lookUpDownAxis the new look up/down axis
       */
      void SetLookUpDownAxis(Axis* lookUpDownAxis);

      /**
       * Returns the axis that looks down (for negative values)
       * or up (for positive values).
       *
       * @return the current look up/down axis
       */
      Axis* GetLookUpDownAxis();

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
       * Set the height distance at which point we're falling.  If the MotionModel
       * elevation is higher than fallingHeight plus the height of terrain, than
       * we'll let gravity take over. (defaults to 1.0 meter)
       * @param fallingHeight: The distance above terrain at which point we're falling (meters)
       */
      void SetFallingHeight(float fallingHeight);

      ///Get the distance above terrain at which point we're falling.
      float GetFallingHeight() const;

      /**
       * Message handler callback.
       *
       * @param data the message data
       */
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal);

      /**
       * Sets having to use the mouse buttons to move the camera
       * @param use True to use
       */
      void SetUseMouseButtons(bool use) { mUseMouseButtons = use; }

      /**
       * Inverts the mouse movement in pitch
       */
      void InvertMouse(bool b) { mInvertMouse = b; }

      /**
       * @return whether or not the mouse movement in pitch has been inverted
       */
      bool GetInvertMouse() { return mInvertMouse; }

      /**
       * Sets whether we should recenter the mouse on an update or not
       */
      void SetRecenterMouse(bool b) { mShouldRecenterMouse = b; }

      /**
       * @return whether or not the mouse gets recentered on updates
       */
      bool GetRecenterMouse() { return mShouldRecenterMouse; }

      /**
       * @return whether the WASD keys are used for movement.
       */
      bool GetUseWASD() const {return mUseWASD;}

      /**
       * Sets whether the WASD keys are used for movement.
       */
      void SetUseWASD(bool enable) {mUseWASD = enable;}

      /**
       * @return whether the Arrow keys are used for movement.
       */
      bool GetUseArrowKeys() const {return mUseArrowKeys;}

      /**
       * Sets whether the Arrow keys are used for movement.
       */
      void SetUseArrowKeys(bool enable) {mUseArrowKeys = enable;}

      /**
       * Sets whether this motion model should operate even when unfocused.
       * Defaults to false (no).
       * @param operate True if motion model should operate even when unfocused
       */
      void ShouldOperateWhenUnfocused(bool operate);

      /**
       * Checks that we are enabled, have a target, mouse has focus, etc... In other
       * words, is this motion model currently doing it's job? Useful for subclasses...
       * @return whether or not this is currently processing mouse/keyboard movement.
       */
      bool IsCurrentlyActive();

   private:
      class FPSMotionModelDebugger; // private inner class used for debugging
      FPSMotionModelDebugger* mpDebugger; // private inner object used for debugging

      /**
       * A reference to the Scene, used for ground following.
       */
      RefPtr<Scene> mScene;

      /**
       * The default input device.
       */
      RefPtr<LogicalInputDevice> mDefaultInputDevice;

      /**
       * The default walk forward/backward axis.
       */
      dtCore::RefPtr<LogicalAxis> mDefaultWalkForwardBackwardAxis;

      /**
       * The default turn left/right axis.
       */
      dtCore::RefPtr<LogicalAxis> mDefaultTurnLeftRightAxis;

      /**
       * The default look up/down axis.
       */
      dtCore::RefPtr<LogicalAxis> mDefaultLookUpDownAxis;

      /**
       * The default sidestep left/right axis.
       */
      dtCore::RefPtr<LogicalAxis> mDefaultSidestepLeftRightAxis;

      /**
       * The axis that moves the target forwards or backwards.
       */
      dtCore::RefPtr<Axis> mWalkForwardBackwardAxis;

      /**
       * The axis that turns the target to the left or right.
       */
      dtCore::RefPtr<Axis> mTurnLeftRightAxis;

      /**
       * The axis that looks up or down.
       */
      dtCore::RefPtr<Axis> mLookUpDownAxis;

      /**
       * The axis that sidesteps the target left or right.
       */
      dtCore::RefPtr<Axis> mSidestepLeftRightAxis;

      /**
       * A list of ref pointers to misc axis mappings created in this class.
       */
      std::vector<dtCore::RefPtr<AxisMapping> > mAxisMappingList;

      FPSAxisHandler* mSidestepHandler;       ///<Side step Axis handler
      FPSAxisHandler* mForwardBackwardHandler;///<Forward/back Axis handler
      FPSAxisHandler* mLookLeftRightHandler;  ///<LeftRight Axis handler
      FPSAxisHandler* mLookUpDownHandler;     ///<Up/Down Axis handler

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
       * The height at which the motion model will "fall" and have gravity take over
       */
      float mFallingHeight;

      /**
       * The current downward speed.
       */
      osg::Vec3 mFallingVec;

      bool mFalling; ///<are we currently falling?

      bool mInvertMouse; //invert the nouse
      bool mUseWASD, mUseArrowKeys;

      bool mOperateWhenUnfocused; // should motion model operate when unfocused? Defaults to false
      bool mShouldRecenterMouse;

      dtCore::RefPtr<Mouse>    mMouse;
      dtCore::RefPtr<Keyboard> mKeyboard;

      bool OnForwardBackwardChanged(double newState, double delta);
      bool OnSidestepChanged(double newState, double delta);
      bool OnLookLeftRightChanged(double newState, double delta);
      bool OnLookUpDownChanged(double newState, double delta);
      bool HasHeadingChanged(const osg::Vec2& diff);
      void ResetMousePosition();

      float mForwardBackCtrl; ///<control value for forward/back movement (-1.0, 1.0)
      float mSidestepCtrl;    ///<control value for sidestep movement (-1.0, 1.0)
      float mLookLeftRightCtrl;///<control value for Left/Right rotation (-1.0, 1.0)
      float mLookUpDownCtrl;   ///<control value for up/down rotation (-1.0, 1.0)

      bool mUseMouseButtons;

      ///private method used to ground clamp or adjust the falling velocity/position
      void AdjustElevation(osg::Vec3& xyz, double deltaFrameTime);
   };
} // namespace dtCore

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_FPSMOTIONMODEL
