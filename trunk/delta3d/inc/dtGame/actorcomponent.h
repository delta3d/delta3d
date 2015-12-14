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
#include <dtCore/baseactorobject.h>
#include <dtCore/actortype.h>
#include <dtCore/sigslot.h>
#include <dtUtil/refstring.h>
#include <dtUtil/getsetmacros.h>
#include <typeinfo>  //for bad_cast

namespace dtGame
{
   class GameActor;
   class ActorComponentContainer;
   class MapMessage;
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
   class DT_GAME_EXPORT ActorComponent : public dtCore::BaseActorObject, public sigslot::has_slots<>
   {

   public:

      typedef dtCore::BaseActorObject BaseClass;

      /**
       *  String used to identify component.
       */
      typedef dtCore::ActorTypePtr ACType;

      ///b All derived actor component types must pass this or a descendant as a parent type.
      static const ACType BaseActorComponentType;

      /**
       * Constructor.
       * @param type Type string to identify component class
       */
      ActorComponent(ACType type);

      /**
       * Get type string of this component
       * @return The type string of this ActorComponent
       */
      ACType GetType() const;

      /** 
       * Called when this ActorComponent gets added to an GameActor.  Overwrite 
       * to perform any custom initialization.
       * @param actor The Actor this ActorComponent has been added to.
       */ 
      DEPRECATE_FUNC virtual void OnAddedToActor(dtGame::GameActor& /*drawable*/) {};

      /** 
       * Called when this ActorComponent is removed from the parent actor.
       * @param actor The Actor this ActorComponent has just been removed from
       */
      DEPRECATE_FUNC virtual void OnRemovedFromActor(dtGame::GameActor& /*drawable*/) {};

      /** 
       * Called when this ActorComponent gets added to an GameActor.  Overwrite
       * to perform any custom initialization.
       * @param actor The Actor this ActorComponent has been added to.
       */
      virtual void OnAddedToActor(dtCore::BaseActorObject& /*actor*/) {};

      /**
       * Called when this ActorComponent is removed from the parent actor.
       * @param actor The Actor this ActorComponent has just been removed from
       */
      virtual void OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/) {};


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
       * @return The ActorComponentContainer that contains this ActorComponent
       */
      ActorComponentContainer* GetOwner() const;

      /**
       * Get ComponentBase this component is a part of. Cast to given pointer type.
       * Usage example:
       * @code 
       * MyComponentBaseActor* owner;
       * mycomponent->GetOwner(owner);
       * @endcode
       */
      template <typename TOwnerPtr>
      void GetOwner(TOwnerPtr& ownerPtr) const
      {
         if (mOwner == NULL)
         {
            ownerPtr = NULL;
         }
         else
         {   
            ownerPtr = dynamic_cast<TOwnerPtr>(mOwner);
            if (ownerPtr == NULL)
            {
               throw std::bad_cast();
            }
         }
      }

      template <typename TOwner>
      TOwner* GetOwner() const
      {
         TOwner* owner = NULL;
         GetOwner(owner);
         return owner;
      }

      /**
       * Set the ComponentBase that this component is a part of.
       * Don't call this! Should only be called by ComponentBase.
       */
      virtual void SetOwner(ActorComponentContainer* owner);

      /** 
       * This method builds the property map, sets the initialized state, etc. 
       * For now, this is called from the AddComponent on the ActorComponentContainer.
       * In the future, this should be called from library initialization behavior, like with Actors
       */
      virtual void Init(const dtCore::ActorType& actorType);

      /// Temporary method to determine if this object (handled as a BaseActorObject)
      /// is a descendant of ActorComponent.
      /// Remove this method when ActorComponent has been promoted to dtCore.
      virtual bool IsActorComponent() const { return true; }

      /**
      * Override this and add whatever properties you want to go out when you call
      * NotifyPartialActorUpdate(). Note - you should not use NotifyPartialActorUpdate()
      * without overriding this - the default implementation logs a warning.
      * Note - This will do nothing if the actor is Remote.
      */
      virtual void GetPartialUpdateProperties(std::vector<dtUtil::RefString>& outPropNames) {}

   protected:

      virtual ~ActorComponent();

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

      virtual bool IsPlaceable() const /*final*/ { return false; }

   private: 

      /** The ComponentBase this component is a part of */
      ActorComponentContainer* mOwner;

      // temporary type.
      ACType mType;

      /// Have we built our property maps, etc.
      bool mInitialized;

   public:
      /// if this actor component is in the GM.
      DT_DECLARE_ACCESSOR(bool, IsInGM);
   };
}
#endif // actorcomponent_h__
