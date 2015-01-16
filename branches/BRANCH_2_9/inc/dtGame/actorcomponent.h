/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2009, MOVES Institute
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
* @author Choco (forum handle)
* @author David Guthrie
* @author Curtiss Murphy
*/

#ifndef DTGAME_ACTOR_COMPONENT_H
#define DTGAME_ACTOR_COMPONENT_H

#include <dtGame/export.h>
#include <dtCore/actorcomponent.h>

namespace dtGame
{
   class TickMessage;

   /**
    * Abstract interface class for ActorComponent. ActorComponent can be retrieved from 
    * ActorComponentContainer by type. ActorComponent have to register to receive any GameActor messages.
    * ActorComponent implementing this interface have to provide a value
    * for ACType to identify the ActorComponent type.
    * When the ActorComponent is added to a ActorComponentContainer, it receives a
    * reference to the ActorComponentContainer through the method SetOwner().
    *
    * Note - Actor Components can reference each other, but you should not check for another
    * actor component until the OnEnteredWorld() method. This gives each actor component a chance
    * to initialize and get properties from the map regardless of order.
    */
   class DT_GAME_EXPORT GameActorComponent : public dtCore::ActorComponent
   {

   public:

      typedef dtCore::ActorComponent BaseClass;

      static const dtCore::ActorComponent::ACType TYPE;

      /**
       * Constructor.
       * @param type Type string to identify component class
       */
      GameActorComponent(ACType type);

   protected:

      virtual ~GameActorComponent();

      /**
       * Registers for tick local or tick remote depending on the actor state.
       */
      void RegisterForTick();

      /**
       * Unregisters for tick local or tick remote depending on the actor state.
       */
      void UnregisterForTick();

      /**
       * Default update method. Override to execute stuff for
       * each physics step. Call RegisterForTicks() to let this get called.
       */
      virtual void OnTickLocal(const TickMessage& /*tickMessage*/) {};
      virtual void OnTickRemote(const TickMessage& /*tickMessage*/) {};
   };

}

#endif // DTGAME_ACTOR_COMPONENT_H
