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

   protected:
      virtual ~TankActor() { } ;

   private:
      // private vars
      float mVelocity;
      float mTurnRate;
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

};

#endif
