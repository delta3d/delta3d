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

#ifndef actorcomponentbase_h__
#define actorcomponentbase_h__

#include <dtGame/export.h>
#include <dtCore/actorcomponentcontainer.h>
#include <dtGame/actorcomponentcontainer.h>
#include <dtUtil/assocvector.h>

namespace dtGame
{
   /**
    * A container for ActorComponents.
    * Use multiple inheritance to add this to a dtGame::GameActor.
    * A simple component system for modularizing game actor functionality.
    * Each actor has a number of components that can be retrieved by type.

    * GameActors can be extended with ActorComponentBase (using multiple inheritance) to
    * include component functionality.
    */
   class DT_GAME_EXPORT ActorComponentBase : public dtGame::ActorComponentContainer,
      // NOTE: Secondary base class of the same name as the first is the intended destination
      // for the first base class' functionality. The first base class will be removed in the
      // future. This ugly multiple inheritence is only temporary for gradual migration of code.
      public virtual dtCore::ActorComponentContainer
   {
   public:
      typedef dtGame::ActorComponentContainer Baseclass;

      /** a map from component type strings to components */
      typedef std::vector< std::pair<ActorComponent::ACType, dtCore::RefPtr<ActorComponent> > > ActorComponentMap;

      //CTOR
      ActorComponentBase();

      /**
       * Allows performing an operation on each actor component.
       * @param func a class with an operator() that takes an actor component by reference (dtGame::ActorComponent&)
       */
      template <typename UnaryFunctor>
      void ForEachComponent(UnaryFunctor func) const;

      /**
       * Get all components matching this type
       * @param type The type of the ActorComponent to get
       * @return the selected ActorComponents (will be empty if not found)
       */
      virtual ActorComponentVector GetComponents(ActorComponent::ACType type) const;

      /**
       * Get all components matching this type
       * @param type The type of the ActorComponent to get
       * @return the selected ActorComponents (will be empty if not found)
       */
      virtual void GetComponents(ActorComponent::ACType type, ActorComponentVector& outComponents) const;

      /**
       * Get all components matching this type.
       * @note This uses base types for use with the code that saves actor components in
       * maps. There is really no reason to call this in application code
       * @param type The type of the ActorComponent to get
       * @return the selected ActorComponents (will be empty if not found)
       */
      virtual void GetComponents(dtCore::ActorTypePtr type, dtCore::ActorPtrVector& outComponents) const;

      /**
       * Fill the vector with all the actor components.
       */
      void GetAllComponents(ActorComponentVector& toFill);
      void GetAllComponents(ActorComponentVectorConst& toFill) const;
   
      void GetAllComponents(dtCore::ActorPtrVector& toFill);

      /**
       * Does base contain a component of given type?
       * @param type The type-string of the ActorComponent to query
       * @return true if ActorComponent is found, false otherwise
       */
      bool HasComponent(ActorComponent::ACType type) const;

      /**
       * Add an ActorComponent. Only one ActorComponent of a given type can be added.
       * @param component The ActorComponent to try to add
       */
      virtual void AddComponent(ActorComponent& component);

      virtual void AddComponent(dtCore::BaseActorObject& component);

      /**
       * Remove component by reference
       * @param component : Reference to the ActorComponent to remove
       */
      virtual void RemoveComponent(ActorComponent& component);

      /**
       * Removes all components with a particular type
       * @param type The type-string of the ActorComponent to remove
       */
      void RemoveAllComponentsOfType(ActorComponent::ACType type);

      /**
       * Remove all contained ActorComponent
       */
      void RemoveAllComponents();

      /**
       * Loop through all ActorComponents call their OnEnteredWorld()
       */
      void CallOnEnteredWorldForActorComponents();

      /**
       * Loop through all ActorComponents call their OnRemovedWorld()
       */
      void CallOnRemovedFromWorldForActorComponents();

      /**
       * Call the BuildPropertyMap() method of all registered ActorComponent
       */
      void BuildComponentPropertyMaps();

      /**
       * Override this to get informed about newly added ActorComponent
       * @param component The ActorComponent just added
       */
      virtual void OnActorComponentAdded(ActorComponent& /*component*/) {};

      /**
       * Override this to get informed about removed components
       * @param component The ActorComponent just removed
       */
      virtual void OnActorComponentRemoved(ActorComponent& /*component*/) {};

   protected:

      virtual ~ActorComponentBase();

   private:

      ActorComponentMap mComponents;

   };

   template <typename UnaryFunctor, typename pairType>
   class BindActorComponent
   {
   public:
      BindActorComponent(UnaryFunctor func)
      : mFunc(func)
      {}

      void operator () (pairType thePair)
      {
         mFunc(*thePair.second);
      }
   private:
      UnaryFunctor mFunc;
   };

   template <typename UnaryFunctor>
   inline void ActorComponentBase::ForEachComponent(UnaryFunctor func) const
   {
      BindActorComponent<UnaryFunctor, ActorComponentBase::ActorComponentMap::value_type> actorCompMapBindFunc(func);
      std::for_each(mComponents.begin(), mComponents.end(), actorCompMapBindFunc);
   }
}

#endif // actorcomponentbase_h__
