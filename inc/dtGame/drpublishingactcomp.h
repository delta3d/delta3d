/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * @author Curtiss Murphy
 */
#ifndef DR_PUBLISHING_ACT_COMP_H_
#define DR_PUBLISHING_ACT_COMP_H_

#include <dtGame/export.h>

#include <dtGame/actorcomponentbase.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/motioninterface.h>
#include <dtUtil/getsetmacros.h>
#include <dtGame/gameactor.h>
#include <dtGame/deadreckoninghelper.h>


namespace dtGame
{
   /**
    * A game actor component that has special logic for the when, how, and what to 
    * publish for local entities that are using dead reckoning. This actor component
    * expects your actor to also have a dead reckoning actor component (aka helper).
    */
   class DT_GAME_EXPORT DRPublishingActComp : public dtGame::ActorComponent, public dtCore::MotionInterface
   {

   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      // set the type of the actor component
      static const ActorComponent::ACType TYPE;

      /**
       * TODO This should be a default and the code should allow changing this value for runtime, and possibly
       *      allow setting the global default in the config xml.
       */
      static const float TIME_BETWEEN_UPDATES;


      /// Constructor - pass in false if you are using the DR Publisher ONLY for heartbeats, without a DR Helper to do the Dead Reckoning
      DRPublishingActComp(bool requiresDRHelper = true);

      ////////////////////////////////////////////////////////////////
      // Basic Actor Component Behaviors
      ////////////////////////////////////////////////////////////////

      virtual void OnAddedToActor(dtGame::GameActor& actor);
      virtual void OnRemovedFromActor(dtGame::GameActor& actor);

      /// Called when the parent actor enters the "world".
      virtual void OnEnteredWorld();

      /// Called when the parent actor leaves the "world".
      virtual void OnRemovedFromWorld();

      /** add actor component properties to game actor for configuring in STAGE */
      virtual void BuildPropertyMap();

      ////////////////////////////////////////////////////////////////
      // DR Publishing Act Comp behaviors
      ////////////////////////////////////////////////////////////////

      /** 
       * Causes an actor update (publish) at the next opportunity. Call this if DR data changed and 
       * you want to force a pos/vel update. Note, happens when the actor component gets the tick call, not immediatly
       */
      void ForceUpdateAtNextOpportunity();

      /** 
       * Causes a full actor update at the next opportunity. Call this if non-DR data changed and 
       * you want to force a full update. Note, happens when the actor component gets the tick call, not immediatly
       */
      void ForceFullUpdateAtNextOpportunity();

      /// Called once we decided to publish - does a quick calc on acceleration. 
      virtual void SetLastKnownValuesBeforePublish(const osg::Vec3& pos, const osg::Vec3& rot);

      void SetSecsSinceLastUpdateSent(float secsSinceLastUpdateSent);
      float GetSecsSinceLastUpdateSent() const;

      /////////////////////////////////////////////////////////////////
      /// The next 3 values are not recommended for general use. They can be expensive and are probably not needed.
      /////////////////////////////////////////////////////////////////

      /// Allows you to use the change in velocity as a test for publish. Off by default because it can be expensive.
      void SetUseVelocityInDRUpdateDecision(bool);
      /// Allows you to use the change in velocity as a test for publish. Off by default because it can be expensive.
      bool GetUseVelocityInDRUpdateDecision() const;

      /// Set - The magnitude of the change in velocity that will cause an update to happen.
      void SetVelocityMagnitudeUpdateThreshold(float);
      /// Get - The magnitude of the change in velocity that will cause an update to happen.
      float GetVelocityMagnitudeUpdateThreshold() const;

      /// Set - Threshold of velocity dotted with last known velocity that will cause an update to happen.
      void SetVelocityDotProductUpdateThreshold(float);
      /// Get - Threshold of velocity dotted with last known velocity that will cause an update to happen.
      float GetVelocityDotProductUpdateThreshold() const;


      /// Set - Accum Acceleration is computed each frame inside ComputeCurrentVel. Override that if you want to set this.
      void SetAccumulatedAcceleration(const osg::Vec3& newValue);
      /// Get - Accum Acceleration is computed each frame inside ComputeCurrentVel. Override that if you want to set this.
      osg::Vec3 GetAccumulatedAcceleration() const; 

      //////////////////////////////////////////////////////////////
      // PROPERTY MACROS
      // Not all of these are actual actor properties, but they all use the property macros
      //////////////////////////////////////////////////////////////

      /**
       * The current velocity is computed using a moving average of the
       * change in position over time.  The frame count passed in is used to
       * to decide about how many frames the velocity will be average across.
       * This may help 'smooth' out some vehicles. The default is 1, so no effect
       */
      DT_DECLARE_ACCESSOR(int, VelocityAverageFrameCount);

      /**
       * Velocity clamping.  If the magnitude of the velocity is less than this
       * it will just make it 0 for publishing.
       */
      DT_DECLARE_ACCESSOR(float, VelocityClampMagnitude);

      /// The max number of times per second an update may be sent if it exceeds the dead reckoning tolerances
      DT_DECLARE_ACCESSOR(float, MaxUpdateSendRate);

      /**
       * When publishing updates with dead reckoning - do we send velocity & accel? Allows tight
       * control for actors (ex for motion model stuff). If false, linear velocity & accel will be zero.
       * NOTE - This is not an actor property
       */
      DT_DECLARE_ACCESSOR(bool, PublishLinearVelocity);
      bool IsPublishLinearVelocity() const { return GetPublishLinearVelocity(); }

      /**
       * When publishing updates with dead reckoning - do we send angular velocity? Allows tight
       * control for actors (Ex for motion model stuff). If false, angular velocity will be zero. 
       * NOTE - This is not an actor property
       */
      DT_DECLARE_ACCESSOR(bool, PublishAngularVelocity);
      bool IsPublishAngularVelocity() const { return GetPublishAngularVelocity(); }


      /**
       * Assigns a source for things like velocity, angular velocity, etc, so that this doesn't have to calculate them.
       */
      DT_DECLARE_ACCESSOR(dtCore::RefPtr<dtCore::VelocityInterface>, VelocitySource);
      DT_DECLARE_ACCESSOR(dtCore::RefPtr<dtCore::AccelerationInterface>, AccelSource);
      DT_DECLARE_ACCESSOR(dtCore::RefPtr<dtCore::AngularVelocityInterface>, AngVelSource);

      /**
       * Sets the current velocity. This is calculated each frame and is different
       * from the LastKnownVelocityVector (which is the last published DR value).
       * By default, this is taken care of for you, but you can call this directly 
       * if you want more specific control and it will bypass the calc'ed value. 
       * @param vec the velocity vector to copy
       */
      void SetVelocity(const osg::Vec3& vec);
      /** 
        * Gets the CURRENT velocity. This is different from LastKnownVelocity.  
        * @see #SetVelocity()
        * @return the most recently calculated instantaneous velocity.
        */
      virtual osg::Vec3 GetVelocity() const;

      /**
       * Sets the CURRENT acceleration. This is calculated each frame and is different 
       * from the LastKnownAcceleration (which is the last published DR value). 
       * By default, this is taken care of for you, but you can call this directly 
       * if you want more specific control and it will bypass the calc'ed value.
       * @param vec the new value
       */
      void SetAcceleration(const osg::Vec3& vec);

      /**
       * Gets the CURRENT acceleration. This is different from the LastKnownAcceleration.
       * @see #SetAcceleration()
       * @return the most recently calculated instantaneous acceleration
       */
      virtual osg::Vec3 GetAcceleration() const;

      /**
       * Sets the CURRENT acceleration. This is different from LastKnownAngulageVelocity. 
       * Unlike vel and accel, this value is NOT calculated for you. If you can get this from
       * your physics engine or know it for some other reason, then you should set it.
       * @param vec the new value
       */
      void SetAngularVelocity(const osg::Vec3& vec);
      /**
       * Sets the CURRENT acceleration. This is different from LastKnownAngulageVelocity. 
       * @see #SetAngularVelocity()
       * @return the current angular velocity that was set at the start of the frame.
       */
      virtual osg::Vec3 GetAngularVelocity() const;

      /// Set - The threshold for the translation from last update before deciding to publish 
      void SetMaxTranslationError(float distance);
      /// Get - The threshold for the translation from last update before deciding to publish 
      float GetMaxTranslationError() const;

      /// Set - threshold for rot (degrees) from last update before deciding to publish 
      void SetMaxRotationError(float rotation);
      /// Set - threshold for rot (degrees) from last update before deciding to publish 
      float GetMaxRotationError() const;

      /// Getter for the time until next update value.  Used for testing mostly
      float GetTimeUntilNextFullUpdate() const;
      /// Sets Full Update counter back to heart beat time. Pass true to distribute the updates.
      void ResetFullUpdateTimer(bool doRandomOffset = false);

      /// The Dead Reckoning Helper is part of the actor and is a requirement to use this actor comp.
      dtGame::DeadReckoningActorComponent* GetDeadReckoningActorComponent();
      void SetDeadReckoningActorComponent(dtGame::DeadReckoningActorComponent* drHelper);
      /// Since GetDeadReckoningActorComponent returns a reference, you MUST call this method first.
      bool IsDeadReckoningActorComponentValid() const;

   protected:

       virtual ~DRPublishingActComp();

       /// Called during tick. Returns true if changes in pos/rot/vel/accel require a network update 
       virtual bool ShouldForceUpdate(const osg::Vec3& pos, const osg::Vec3& rot);

       /// Used by ShouldForceUpdate. 
       float GetPercentageChangeDifference(float startValue, float newValue) const;

       /**
        * Calculates and assigns the current velocity using a moving average.
        * @see SetVelocityAverageFrameCount
        */
       virtual void CalculateCurrentVelocity(float deltaTime, const osg::Vec3& pos, const osg::Vec3& rot);

       /// react to game tick messages - Note, we check for publish in 'remote' even for local actors.
       virtual void UpdatePublishingData(const dtGame::TickMessage& tickMessage);

   private:
       dtCore::RefPtr<Invokable> mTickInvokable;
      /// Do we insist a DR helper is set? If yes, log error if missing. Default is true on construction
      bool mRequiresDRHelper; 
      float mTimeUntilNextFullUpdate;

      // Current values - not published or directly set-able
      osg::Vec3 mVelocity;
      osg::Vec3 mAcceleration;
      osg::Vec3 mAngularVelocity;

      osg::Vec3 mLastPos;
      osg::Vec3 mAccumulatedLinearVelocity;
      osg::Vec3 mAccumulatedAcceleration; 
      osg::Vec3 mAccelerationCalculatedForLastPublish; // Used in SetLastKnownValuesBeforePublish()
      //int mVelocityAverageFrameCount; - part of the property macro

      ///////////////////////////////////////////////////
      // sending out dead reckoning
      float mSecsSinceLastUpdateSent;
      //float mMaxUpdateSendRate; - part of the property macro
      float mVelocityMagThreshold;
      float mVelocityDotThreshold;

      bool mForceUpdateNextChance;

      bool mUseVelocityInDRUpdateDecision;

      //members for sending updates on a local actor.
      float mMaxRotationError;
      float mMaxRotationError2;
      float mMaxTranslationError;
      float mMaxTranslationError2;

      // Note - the helper will eventually be a act comp on the actor and we can look it up
      // Note that the LastKnown values are stored on the DR helper, but we hold the CURRENT
      // values because they are part of publishing. 
      dtCore::ObserverPtr<dtGame::DeadReckoningActorComponent> mDeadReckoningActorComponent;
   };
} // namespace dtGame

#endif // DR_PUBLISHING_ACT_COMP_H_
