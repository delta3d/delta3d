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

#ifndef actorcomponentbase_h__
#define actorcomponentbase_h__

#include <dtGame/export.h>
#include <dtGame/actorcomponent.h>
#include <dtCore/refptr.h>
#include <map>

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
   class DT_GAME_EXPORT ActorComponentBase
   {
   public:

      /** a map from component type strings to components */
      typedef std::map<ActorComponent::ACType, dtCore::RefPtr<ActorComponent> > ActorComponentMap;

      //CTOR
      ActorComponentBase();

      /** 
       * Get a component by type. Usage:
       * @code
       * MyComponentClass* component;
       * myComponentBase->GetComponent(component);
       * @endcode
       * @param compType pointer to be set to component
       * @return True if component of this type exists, else false
       */
      template <typename TComp>
      bool GetComponent(TComp*& compType) const
      {
         if(!HasComponent(TComp::TYPE))
         {
            return false;
         }
         compType = static_cast<TComp*>(GetComponent(TComp::TYPE));
         return true;
      }

      /** 
       * Get component by type string
       * @param type The type-string of the ActorComponent to get
       * @return the selected ActorComponent (could be NULL if not found)
       */
      ActorComponent* GetComponent(const ActorComponent::ACType& type) const;

      /**
       * Does base contain a component of given type?
       * @param type The type-string of the ActorComponent to query
       * @return true if ActorComponent is found, false otherwise
       */
      bool HasComponent(const ActorComponent::ACType& type) const;

      /**
       * Add an ActorComponent. Only one ActorComponent of a given type can be added.
       * @param component The ActorComponent to try to add
       */
      void AddComponent(ActorComponent* component);

      /** 
       * Remove component by type
       * @param type The type-string of the ActorComponent to remove
       */
      void RemoveComponent(const ActorComponent::ACType& type);

      /**
       * Remove component by reference
       * @param component : Pointer to the ActorComponent to remove
       */
      void RemoveComponent(ActorComponent* component);

      /**
       * Remove all contained ActorComponent
       */
      void RemoveAllComponents();

      /** 
       * Loop through all ActorComponent and call their SetGameManager() methods
       */
      void InitComponents();

      /**
       * Call the BuildPropertyMap() method of all registered ActorComponent
       */
      void BuildComponentPropertyMaps();

      /**
       * Override this to get informed about newly added ActorComponent 
       * @param component The ActorComponent just added 
       */
      virtual void OnActorComponentAdded(ActorComponent* component);

      /** 
       * Override this to get informed about removed components
       * @param component The ActorComponent just removed 
       */
      virtual void OnActorComponentRemoved(ActorComponent* component);

   protected:
     
      virtual ~ActorComponentBase();

   private:
      
      ActorComponentMap mComponents;

   };
}

#endif // actorcomponentbase_h__
