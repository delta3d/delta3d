/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
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
 * Bradley Anderegg and Chris Darken 08/21/2006
 */

#ifndef __DELTA_collisionmotionmodel_H__
#define __DELTA_collisionmotionmodel_H__

#include <dtCore/motionmodel.h>
#include <dtCore/fpscollider.h>
#include <dtCore/transformable.h>
#include <dtCore/inputdevice.h>
#include <dtCore/export.h>

#include <dtUtil/functor.h>


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
   * Collision Motion Model uses ode collision meshes to allow typical FPS Camera interaction with the environment
   */
   class DT_CORE_EXPORT CollisionMotionModel : public MotionModel
   {

   public:
      enum eMode{WALKING = 0, FALLING, SLIDING};

   public:

      /**
      * Constructor.
      * @param height, the height of the character and camera, in meters      
      * @param radius, the width of our character  
      * @param k the distance from the bottom of the knees to the ground, this represents the maximum step up height
      * @param theta the collision amount to maintain below the ground (note: this should be less then half of k,
      *        something small like 0.1 is recommended)
      * @param Scene is used to get the gravity and the ode space
      * @param keyboard the keyboard instance, or 0 to
      * avoid creating default input mappings
      * @param mouse the mouse instance, or 0 to avoid
      * creating default input mappings
      */
      CollisionMotionModel(float pHeight, float pRadius, float k, float theta, dtCore::Scene* pScene, Keyboard* keyboard, Mouse* mouse);

   protected:

      /**
      * Destructor.
      */
      virtual ~CollisionMotionModel();


      ///internal class, used by CollisionMotionModel for InputDevice listening
      /** Helper class used to call the supplied functor when an axis value
      * changes.  Used only by the CollisionMotionModel.
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
      * Message handler callback.
      *
      * @param data the message data
      */
      virtual void OnMessage(MessageData *data);


      FPSCollider& GetFPSCollider();


   private:

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
      

      dtCore::RefPtr<Mouse> mMouse;
      dtCore::RefPtr<Keyboard> mKeyboard;
      FPSCollider mCollider;

      bool OnForwardBackwardChanged(double newState, double delta);
      bool OnSidestepChanged(double newState, double delta);
      bool OnLookLeftRightChanged(double newState, double delta);
      bool OnLookUpDownChanged(double newState, double delta);

      float mForwardBackCtrl; ///<control value for forward/back movement (-1.0, 1.0)
      float mSidestepCtrl;    ///<control value for sidestep movement (-1.0, 1.0)
      float mLookLeftRightCtrl;///<control value for Left/Right rotation (-1.0, 1.0)
      float mLookUpDownCtrl;   ///<control value for up/down rotation (-1.0, 1.0)

   };
};


#endif // __DELTA_collisionmotionmodel_H__
