/* -*-c++-*-
 * TutorialLibrary - TankActor (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
#include <dtCore/transform.h>
#include <osgSim/DOFTransform>

/**
 * This class is the actor that represents a hover tank of sorts.  It knows how to
 * respond to keyboard presses (I, J, K, L to steer). It also listens for
 * "ToggleEngine" & "SpeedBoost" GameEvents.  This actor correctly handles
 * TickLocal versus TickRemote.
 */
class TUTORIAL_TANK_EXPORT TankActor : public dtActors::GameMeshDrawable
{
   public:

      // Constant identifier for our game event handler method.
      static const std::string EVENT_HANDLER_NAME;

      // Constructs the tank actor.
      TankActor(dtGame::GameActorProxy& parent);

      /**
       * This method is an invokable for when a local object receives a tick.
       * @param tickMessage A message containing tick related information.
       */
      virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

      /**
       * This method is an invokable for when a remote object receives a tick.
       * @param tickMessage A message containing tick related information.
       */
      virtual void OnTickRemote(const dtGame::TickMessage& tickMessage);

      /**
       * Generic handler (Invokable) for messages. Overridden from base class.
       * This is the default invokable on GameActorProxy.
       */
      virtual void ProcessMessage(const dtGame::Message& message);

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

      /**
       * This is a method called by the game event invokable when a game
       * event message is passed through the game manager.
       * @param msg The message containing the game event.
       */
      void HandleGameEvent(const dtGame::Message& msg);

      // Called when the actor has been added to the game manager.
      // You can respond to OnEnteredWorld on either the proxy or actor or both.
      virtual void OnEnteredWorld();

   protected:
      virtual ~TankActor() {}

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
      dtCore::RefPtr<dtCore::ParticleSystem> mCannonShot;

      float mVelocity;
      float mAddOnVelocity;
      float mTurnRate;
      bool  mIsEngineRunning;
      float mLastReportedVelocity;
      dtCore::RefPtr<dtCore::Isector> mIsector;
      dtCore::UniqueId mNoTargetId;
      dtCore::UniqueId mCurrentTargetId;
      dtCore::Transform mOriginalPosition;

      bool mPropertiesUpdated;

      dtCore::RefPtr<osgSim::DOFTransform> mDOFTran;
};

/**
 * Our proxy class for the hover tank actor.  The proxy contains properties,
 * invokables, and hover tank actor.
 */
class TUTORIAL_TANK_EXPORT TankActorProxy : public dtActors::GameMeshActor
{
public:
   // Constructs the proxy.
   TankActorProxy();

   // Creates the properties that are custom to the hover tank proxy.
   virtual void BuildPropertyMap();

protected:
   virtual ~TankActorProxy() {}

   // Creates an instance of our hover tank actor
   virtual void CreateDrawable();

   // Called when this proxy is added to the game manager (ie, the "world")
   // You can respond to OnEnteredWorld on either the proxy or actor or both.
   virtual void OnEnteredWorld();
};

#endif // __GM_TUTORIAL_TANK_ACTOR__
