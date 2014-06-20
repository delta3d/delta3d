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

#include <prefix/dtgameprefix.h>
#include <list>
#include <dtGame/actorcomponentbase.h>
#include <dtUtil/exception.h>

#include <cassert>
#include <sstream>

namespace dtGame
{

   /////////////////////////////////////////////////////
   ActorComponentBase::ActorComponentBase()
   {
   }


   //////////////////////////////////////////////////////////////////////////
   ActorComponentBase::~ActorComponentBase()
   {
   }


   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::AddComponent(ActorComponent& component)
   {
      // pass the component a pointer to its owner
      component.SetOwner(this);

      // The call to Init should eventually move to an actor component library behavior
      // like actors have, but until then, this is the only other place to do it.
      component.Init(*component.GetType());

      // store component
      mComponents.push_back(std::pair<ActorComponent::ACType, dtCore::RefPtr<ActorComponent> >(component.GetType(), &component));

      OnActorComponentAdded(component);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<ActorComponent*> ActorComponentBase::GetComponents(ActorComponent::ACType type) const
   {
      std::vector<ActorComponent*> foundComponents;

      ActorComponentMap::const_iterator iter = mComponents.begin();
      while (iter != mComponents.end())
      {
         if (iter->first == type)
         {
            foundComponents.push_back(iter->second.get());
         }
         ++iter;
      }

      return foundComponents;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::GetAllComponents(std::vector<ActorComponent*>& toFill)
   {
      toFill.reserve(toFill.size() + mComponents.size());
      ActorComponentMap::iterator i, iend;
      i = mComponents.begin();
      iend = mComponents.end();
      for (; i != iend; ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorComponentBase::HasComponent(ActorComponent::ACType type) const
   {
      ActorComponentMap::const_iterator iter = mComponents.begin();
      while (iter != mComponents.end())
      {
         if (iter->first == type)
         {
            return true;
         }
         ++iter;
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::RemoveComponent(ActorComponent& component)
   {
      OnActorComponentRemoved(component);

      bool removedComponent = false;
      ActorComponentMap::iterator iter = mComponents.begin();
      while (iter != mComponents.end())
      {
         if (iter->second == &component)
         {
            // Clear the component's owner pointer
            iter->second->SetOwner(NULL);
            mComponents.erase(iter);
            removedComponent = true;
            break;
         }
         ++iter;
      }

      if (!removedComponent)
      {
         LOG_ERROR("Could not find ActorComponent to remove from Actor");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::RemoveAllComponentsOfType(ActorComponent::ACType type)
   {
      for (int componentIndex = mComponents.size() - 1; componentIndex >= 0; --componentIndex)
      {
         if (mComponents[componentIndex].first == type)
         {
            RemoveComponent(*mComponents[componentIndex].second);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::RemoveAllComponents()
   {
      while (!mComponents.empty())
      {
         RemoveComponent(*(*mComponents.begin()).second);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::CallOnEnteredWorldForActorComponents()
   {
      /*
         Copy current list of components and iterate through that.
         Otherwise, when actor components add other actor components to the actor
         in their OnAddedToActor method, the OnAddedToActor method of that new component
         would be called twice: once immediately (because actor is now in game),
         and once when they are reached by iteration.
      */
      std::list<ActorComponent*> components;
      for (ActorComponentMap::iterator iter = mComponents.begin(); iter != mComponents.end(); ++iter)
      {
         components.push_back(iter->second);
      }

      // loop through all components and call their OnAddedToActor method
      for(std::list<ActorComponent*>::iterator iter = components.begin(); iter != components.end(); ++iter)
      {
         ActorComponent& component = (**iter);
         component.SetIsInGM(true);
         component.OnEnteredWorld();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::CallOnRemovedFromWorldForActorComponents()
   {
      std::list<ActorComponent*> components;
      for (ActorComponentMap::iterator iter = mComponents.begin(); iter != mComponents.end(); ++iter)
      {
         components.push_back(iter->second);
      }

      // loop through all components and call their OnAddedToActor method
      for(std::list<ActorComponent*>::iterator iter = components.begin(); iter != components.end(); ++iter)
      {
         ActorComponent& component = (**iter);
         component.SetIsInGM(false);
         component.OnRemovedFromWorld();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::BuildComponentPropertyMaps()
   {
      // loop through all components and call their BuildPropertyMap() method
      ActorComponentMap::iterator iter;
      for (iter = mComponents.begin(); iter != mComponents.end(); ++iter)
      {
         (*iter).second->BuildPropertyMap();
      }
   }
}
