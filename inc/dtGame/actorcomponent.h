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

#ifndef actorcomponent_h__
#define actorcomponent_h__

#include <dtGame/export.h>
#include <dtDAL/propertycontainer.h>
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
    *
    * Note - Actor Components can reference each other, but you should not check for another
    * actor component until the OnEnteredWorld() method. This gives each actor component a chance
    * to initialize and get properties from the map regardless of order.
    */
   class DT_GAME_EXPORT ActorComponent : public dtDAL::PropertyContainer
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
       * Called when this ActorComponent gets added to an GameActor.  Overwrite 
       * to perform any custom initialization.
       * @param actor The GameActor this ActorComponent has been added to.
       */ 
      virtual void OnAddedToActor(dtGame::GameActor& actor) {};

      /** 
       * Called when this ActorComponent is removed from the parent actor.
       * @param actor The GameActor this ActorComponent has just been removed from
       */
      virtual void OnRemovedFromActor(dtGame::GameActor& actor) {};

      /** 
       * Called when the parent actor enters the "world".
       */
      virtual void OnEnteredWorld() {};

      /** 
       * Called when the parent actor leaves the "world".
       */
      virtual void OnRemovedFromWorld() {};

      /**
       * Overwrite this to add ActorProperties to this PropertyContainer.
       * @see PropertyContainer::AddProperty()
       */
      virtual void BuildPropertyMap() {};


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
       * This method builds the property map, sets the initialized state, etc. 
       * For now, this is called from the AddComponent on the ActorComponentBase. 
       * In the future, this should be called from library initialization behavior, like with Actors
       */
      virtual void Init();


   protected:

      virtual ~ActorComponent();

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
      virtual void OnTickLocal(const TickMessage& tickMessage) {};

   private: 

      /** The ComponentBase this component is a part of */
      ActorComponentBase* mOwner;

      /** type string of component */
      const ACType mType;

      /// Have we built our property maps, etc.
      bool mInitialized;

   };
}
#endif // actorcomponent_h__
