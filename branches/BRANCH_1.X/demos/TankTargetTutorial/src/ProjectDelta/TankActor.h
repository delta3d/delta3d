/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
* @author Curtiss Murphy
*/
#ifndef __GM_TUTORIAL_TANK_ACTOR__
#define __GM_TUTORIAL_TANK_ACTOR__

#include "export.h"
#include <dtCore/refptr.h>
#include <dtActors/gamemeshactor.h>
#include <dtCore/particlesystem.h>
#include <dtCore/isector.h>

/**
 * This class is the actor that represents a hover tank of sorts.  It knows how to 
 * respond to keyboard presses (I, J, K, L to steer). It also listens for 
 * "ToggleEngine" & "SpeedBoost" GameEvents.  This actor correctly handles 
 * TickLocal versus TickRemote.
 */
class TUTORIAL_TANK_EXPORT TankActor : public dtActors::GameMeshActor
{
   public:
      // Constructs the tank actor.
      TankActor(dtGame::GameActorProxy &proxy);

      /**
       * This method is an invokable for when a local object receives a tick.
       * @param tickMessage A message containing tick related information.
       */
      virtual void TickLocal(const dtGame::Message &tickMessage);

      /**
       * This method is an invokable for when a remote object receives a tick.
       * @param tickMessage A message containing tick related information.
       */
      virtual void TickRemote(const dtGame::Message &tickMessage);
      

      // TUTORIAL - OVERRIDE TankActor::ProcessMessage() here 
      /**
       * Generic handler (Invokable) for messages. Overridden from base class.
       * This is the default invokable on GameActorProxy.
       */
      virtual void ProcessMessage(const dtGame::Message &message);


      /**
       * Sets the velocity of our tank actor.  
       * @param velocity The new velocity.
       * @note This may cause an actor update message
       */
      void SetVelocity(float velocity);
      
      // @return The actor's current velocity.
      float GetVelocity() const { return mVelocity; }

      /**
       * Sets the turn rate of our tank actor.  
       * @param rate The new turn rate in degrees per second.
       * @note This may cause an actor update message
       */
      void SetTurnRate(float rate);

      // @return The actor's current turn rate.
      float GetTurnRate() const { return mTurnRate; }

      // Called when the actor has been added to the game manager.
      // You can respond to OnEnteredWorld on either the proxy or actor or both.
      virtual void OnEnteredWorld();

   protected:
      virtual ~TankActor() { } ;

   private:
      // do our internal velocity/turn calculations based on keyboard status
      // only relevant for Local mode (ie, not when remote)
      void ComputeVelocityAndTurn(float deltaSimTime);

      // calculate new position based on turn rate and velocity
      // relevant in both local and remote
      void MoveTheTank(float deltaSimTime);

      void CheckForNewTarget();

      // Utility method to fire a target changed message using mCurrentTargetId
      void FireTargetChangedMessage();

      // private vars
      dtCore::RefPtr<dtCore::ParticleSystem> mDust;
      float mVelocity;
      float mTurnRate;
      bool mIsEngineRunning;
      float mLastReportedVelocity;
      dtCore::RefPtr<dtCore::Isector> mIsector;
      dtCore::UniqueId mNoTargetId;
      dtCore::UniqueId mCurrentTargetId;
      dtCore::Transform mOriginalPosition;   
};

/**
 * Our proxy class for the hover tank actor.  The proxy contains properties,
 * invokables, and hover tank actor. 
 */
class TUTORIAL_TANK_EXPORT TankActorProxy : public dtActors::GameMeshActorProxy
{
   public:
      // Constructs the proxy.
      TankActorProxy();
      
      // Creates the properties that are custom to the hover tank proxy.
      virtual void BuildPropertyMap();
      
   protected:
      virtual ~TankActorProxy() { };

      // Creates an instance of our hover tank actor 
      virtual void CreateActor();

      // Called when this proxy is added to the game manager (ie, the "world")
      // You can respond to OnEnteredWorld on either the proxy or actor or both.
      virtual void OnEnteredWorld();
};

#endif
