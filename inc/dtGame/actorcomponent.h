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
*/

#ifndef actorcomponent_h__
#define actorcomponent_h__

#include <dtGame/export.h>
#include <osg/Referenced>
#include <dtUtil/refstring.h>

namespace dtGame
{
   class GameActor;
   class ActorComponentBase;
   class TickMessage;

   /**
    * Abstract interface class for ActorComponent. ActorComponent can be retrieved from 
    * ActorComponentBase by type. ActorComponent have to register to receive any GameActor messages.
    * ActorComponent implementing this interface have to provide a value
    * for ACType to identify the ActorComponent type.
    * When the ActorComponent is added to a ActorComponentBase, it receives a
    * reference to the ActorComponentBase through the method SetOwner().
    */
   class DT_GAME_EXPORT ActorComponent : public osg::Referenced
   {

   public:

      /**
       *  String used to identify component.
       */
      typedef dtUtil::RefString ACType;

      /**
       * Constructor.
       * @param type Type string to identify component class
       */
      ActorComponent(const ACType& type);

      /**
       * Get type string of this component
       * @return The type string of this ActorComponent
       */
      const ACType& GetType() const;

      /** 
       * If component base actor is already in a running game, this
       * method is called immediately after adding the component to the actor.
       * If the base actor is not yet in the game, this method is called when the actor
       * enters the game.
       * @param actor The GameActor this ActorComponent has been added to
       */ 
      virtual void OnAddedToActor(dtGame::GameActor& actor);

      /** 
       * This method is called when owner is removed from game 
       * or when component is removed from actor.
       * @param actor The GameActor this ActorComponent has just been removed from
       */
      virtual void OnRemovedFromActor(dtGame::GameActor& actor);


      /**
       * Get ComponentBase this component is a part of
       * @return The ActorComponentBase that contains this ActorComponent
       */
      ActorComponentBase* GetOwner() const;

      /**
       * Get ComponentBase this component is a part of. Cast to given pointer type.
       * Usage example:
       * @code 
       * MyComponentBaseActor* owner;
       * mycomponent->GetOwner(owner);
       * @endcode
       */
      template <typename TActorPtr>
      void GetOwner(TActorPtr& actorType) const
      {
         actorType = static_cast<TActorPtr>(mOwner);
      }

      /**
       * Set the ComponentBase that this component is a part of.
       * Don't call this! Should only be called by ComponentBase.
       */
      virtual void SetOwner(ActorComponentBase* owner);

      /**
       * Let GameManager call the OnTickLocal method on each tick.
       * This method can only be called when the OnAddedToActor method
       * was already called.
       */
      void RegisterForTicks();

      /**
       * Unregister from game tick messages
       */
      void UnregisterForTicks();

      /**
       * Default update method. Override to execute stuff for
       * each physics step. Call RegisterForTicks() to let this get called.
       */
      virtual void OnTickLocal(const TickMessage& tickMessage);

      /**
       * Overwrite this to add ActorProperty to owner's proxy
       */
      virtual void BuildPropertyMap();

   protected:

      virtual ~ActorComponent();

   private: 

      /** The ComponentBase this component is a part of */
      ActorComponentBase* mOwner;

      /** type string of component */
      const ACType mType;

   };
}
#endif // actorcomponent_h__
