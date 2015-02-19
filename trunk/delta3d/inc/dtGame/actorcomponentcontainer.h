/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2010, MOVES Institute, Alion Science and Technology Inc.
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

#ifndef ACTORCOMPONENTCONTAINER_H_
#define ACTORCOMPONENTCONTAINER_H_

#include <dtGame/export.h>
#include <dtGame/actorcomponent.h>
#include <dtCore/refptr.h>
#include <dtUtil/breakoverride.h>
#include <dtUtil/deprecationmgr.h>

#include <dtUtil/referencedinterface.h>

#include <algorithm>
#include <map>

namespace dtGame
{
   typedef std::vector<ActorComponent*> ActorComponentVector;
   typedef std::vector<const ActorComponent*> ActorComponentVectorConst;

   /**
    * A pure virtual container for ActorComponents.
    * Use multiple inheritance to add this to any actor like class.
    * A simple component system for modularizing game actor functionality.
    * Each actor has a number of components that can be retrieved by type.
    */
   class DT_GAME_EXPORT ActorComponentContainer
   {
   public:
      ActorComponentContainer() {}

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
         ActorComponentVector components = GetComponents(TComp::TYPE);
         if (!components.empty())
         {
            compType = static_cast<TComp*>(components[0]);
         }
         else
         {
            compType = NULL;
         }
         return compType != NULL;
      }


      /**
       * Get a component by type. Usage:
       * @code
       * dtCore::RefPtr<MyComponentClass> component;
       * myComponentBase->GetComponent(component);
       * @endcode
       * @param compType pointer to be set to component
       * @return True if component of this type exists, else false
       */
      template <typename TComp>
      bool GetComponent(dtCore::RefPtr<TComp>& compType) const
      {
         ActorComponentVector components = GetComponents(TComp::TYPE);
         if (!components.empty())
         {
            compType = static_cast<TComp*>(components[0]);
         }
         else
         {
            compType = NULL;
         }
         return compType.valid();
      }

      /**
       * Gets the first component of the desired type added to the ActorComponentContainer.
       * Usage:
       * @code
       * MyComponentClass* component = myComponentBase->GetComponent<MyComponentClass>();
       * @endcode
       * @return The first ActorComponent of the templated parameter type, or NULL if it doesn't exist
       */
      template <class T> T* GetComponent() const
      {
         T* component = NULL;
         if (HasComponent(T::TYPE))
         {
            component = static_cast<T*>(GetComponents(T::TYPE)[0]);
         }
         return component;
      }

   //private:
      // Override virtual std::vector<ActorComponent*> GetComponents(ActorComponent::ACType type) const instead
      //BREAK_OVERRIDE(GetComponent(ActorComponent::ACType) const) ///deprecated 4/4/12
   public:

      /**
       * Get all components matching this type string
       * @param type The type-string of the ActorComponent to get
       * @return the selected ActorComponents (will be empty if not found)
       */
      virtual ActorComponentVector GetComponents(ActorComponent::ACType type) const = 0;
      virtual void GetComponents(ActorComponent::ACType type, ActorComponentVector& outComponents) const = 0;

      /**
       * Fill the vector with all the actor components.
       */
      virtual void GetAllComponents(ActorComponentVector& toFill) = 0;

      /**
       * Fill the vector with all the actor components as constant pointers.
       */
      virtual void GetAllComponents(ActorComponentVectorConst& toFill) const = 0;

      /**
       * Does base contain a component of given type?
       * @param type The type-string of the ActorComponent to query
       * @return true if ActorComponent is found, false otherwise
       */
      virtual bool HasComponent(ActorComponent::ACType type) const = 0;

      /**
       * Add an ActorComponent. Only one ActorComponent of a given type can be added.
       * @param component The ActorComponent to try to add
       */
      virtual void AddComponent(ActorComponent& component) = 0;

      /**
       * Remove component by type
       * @param type The type-string of the ActorComponent to remove
       */
   private:
      // Override virtual void RemoveAllComponentsOfType(ActorComponent::ACType type) instead
      BREAK_OVERRIDE(RemoveComponent(ActorComponent::ACType)) ///deprecated 4/4/12
   public:

      /**
       * Removes all components with a particular type
       * @param type The type-string of the ActorComponent to remove
       */
      virtual void RemoveAllComponentsOfType(ActorComponent::ACType type) = 0;

      /**
       * Remove component by reference
       * @param component : Reference to the ActorComponent to remove
       */
      virtual void RemoveComponent(ActorComponent& component) = 0;

      /**
       * Remove all contained ActorComponent
       */
      virtual void RemoveAllComponents() = 0;

      /**
       * Loop through all ActorComponents call their OnEnteredWorld()
       */
      virtual void CallOnEnteredWorldForActorComponents() = 0;

      /**
       * Loop through all ActorComponents call their OnRemovedWorld()
       */
      virtual void CallOnRemovedFromWorldForActorComponents() = 0;

      /**
       * Call the BuildPropertyMap() method of all registered ActorComponent
       */
      virtual void BuildComponentPropertyMaps() = 0;

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
      virtual ~ActorComponentContainer() {}
   };

}

#endif /* ACTORCOMPONENTCONTAINER_H_ */
