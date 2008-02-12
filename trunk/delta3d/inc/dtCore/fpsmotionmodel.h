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
//////////////////////////////////////////////////////////////////////

#include <dtCore/motionmodel.h>
#include <dtCore/inputdevice.h>
#include <dtUtil/functor.h>

#include <osg/Vec3>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class IntersectVisitor;
}
/// @endcond

namespace dtCore
{
   class Axis;
   class AxisToAxis;
   class ButtonAxisToAxis;
   class ButtonsToAxis;
   class Keyboard;
   class LogicalAxis;
   class LogicalInputDevice;
   class Mouse;
   class Scene;
   class Isector;

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
         FPSMotionModel(   Keyboard* keyboard = 0,
                           Mouse* mouse = 0,
                           float maxWalkSpeed = 5.0f,
                           float maxTurnSpeed = 100.0f,
                           float maxSidestepSpeed = 5.0f,
                           float height = 2.0f,
                           float maxStepUpDist = 1.0f);

      protected:

         /**
          * Destructor.
          */
         virtual ~FPSMotionModel();

         /**
          * This method can be overriden in subclasses to produce
          * desired mouse behavior
          * @param deltaTime The time change
          */
         virtual void UpdateMouse(const double deltaTime);

         /**
         * This method can be overriden in subclasses to produce
         * desired translation behavior
         * Note: Any collision detection/response and other physical
         * constraints should be applied here
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
         dtCore::RefPtr<Mouse> GetMouse() { return mMouse; }

         /**
         * Returns the keyboard
         *
         * @return the keyboard
         */
         dtCore::RefPtr<Keyboard> GetKeyboard() { return mKeyboard; }

         ///internal class, used by FPSMotionModel for InputDevice listening
         /** Helper class used to call the supplied functor when an axis value
           * changes.  Used only by the FPSMotionModel.
           */
         class DT_CORE_EXPORT FPSAxisListener :  public dtCore::AxisListener
         {
         public:
            typedef dtUtil::Functor<bool, TYPELIST_2(double,double)> SetFunctor;

            FPSAxisListener(const SetFunctor& setFunc);

            virtual ~FPSAxisListener() {}; 

            virtual bool AxisStateChanged(const Axis* axis,
                                          double oldState, 
                                          double newState, 
                                          double delta);

         private:
            SetFunctor mSetFunctor;
         };


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
         void SetFallingHeight( float fallingHeight );

         ///Get the distance above terrain at which point we're falling.
         float GetFallingHeight() const;
         
         /**
          * Message handler callback.
          *
          * @param data the message data
          */
         virtual void OnMessage(MessageData *data);

         /**
         * Sets having to use the mouse buttons to move the camera
         * @param use True to use
         */
         void SetUseMouseButtons(bool use) { mUseMouseButtons = use; }


         /**
          * Inverts the mouse movement in pitch 
          */
         void InvertMouse(bool b){ mInvertMouse = b;}

         /**
          * @return whether or not the mouse movement in pitch has been inverted
          */
         bool GetInvertMouse() {return mInvertMouse;}

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
          * The left/right mouse movement.
          */
         AxisToAxis* mLeftRightMouseMovement;
         
         /**
          * The up/down mouse movement.
          */
         AxisToAxis* mUpDownMouseMovement;
         
         /**
          * The arrow key up/down mapping.
          */
         ButtonsToAxis* mArrowKeysUpDownMapping;
         
         /**
          * The arrow key left/right mapping.
          */
         ButtonsToAxis* mArrowKeysLeftRightMapping;

         /**
          * The arrow key up/down mapping.
          */
         ButtonsToAxis* mArrowKeysUpDownMappingUpperCase;

         /**
          * The arrow key left/right mapping.
          */
         ButtonsToAxis* mArrowKeysLeftRightMappingUpperCase;
         
         /**
          * The a/d key left/right mapping.
          */
         ButtonsToAxis* mADKeysLeftRightMapping;
         
         /**
          * The default walk forward/backward axis.
          */
         LogicalAxis* mDefaultWalkForwardBackwardAxis;
         
         /**
          * The default turn left/right axis.
          */
         LogicalAxis* mDefaultTurnLeftRightAxis;
         
		   /**
          * The default look up/down axis.
          */
         LogicalAxis* mDefaultLookUpDownAxis;
         
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
          * The axis that looks up or down.
          */
         Axis* mLookUpDownAxis;
         
         /**
          * The axis that sidesteps the target left or right.
          */
         Axis* mSidestepLeftRightAxis;
         
         FPSAxisListener *mSidestepListener;       ///<Side step Axis listener
         FPSAxisListener *mForwardBackwardListener;///<Forward/back Axis listener
         FPSAxisListener *mLookLeftRightListener;  ///<LeftRight Axis listener
         FPSAxisListener *mLookUpDownListener;     ///<Up/Down Axis listener

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

         dtCore::RefPtr<Mouse>    mMouse;
         dtCore::RefPtr<Keyboard> mKeyboard;

         dtCore::RefPtr<dtCore::Isector> mIsector; ///<used for ground clamping

         bool OnForwardBackwardChanged(double newState, double delta);
         bool OnSidestepChanged(double newState, double delta);
         bool OnLookLeftRightChanged(double newState, double delta);
         bool OnLookUpDownChanged(double newState, double delta);

         float mForwardBackCtrl; ///<control value for forward/back movement (-1.0, 1.0)
         float mSidestepCtrl;    ///<control value for sidestep movement (-1.0, 1.0)
         float mLookLeftRightCtrl;///<control value for Left/Right rotation (-1.0, 1.0)
         float mLookUpDownCtrl;   ///<control value for up/down rotation (-1.0, 1.0)

         bool mUseMouseButtons;

         ///private method used to ground clamp or adjust the falling velocity/position
         void AdjustElevation(osg::Vec3 &xyz, double deltaFrameTime);
   };
}


#endif // DELTA_FPSMOTIONMODEL
