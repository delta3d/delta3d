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
      : dtCore::ActorComponentContainer()
      , dtGame::ActorComponentContainer()
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
      // Have to const cast so it can set the property defaults.
      component.Init(const_cast<dtCore::ActorType&>(*component.GetType()));

      // store component
      for (ActorComponentMap::const_iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         if (i->second == &component)
            return;
      }
      mComponents.push_back(std::pair<ActorComponent::ACType, dtCore::RefPtr<ActorComponent> >(component.GetType(), &component));

      OnActorComponentAdded(component);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::AddComponent(dtCore::BaseActorObject& component)
   {
      ActorComponent* actorComp = dynamic_cast<ActorComponent*>(&component);

      if (actorComp != NULL)
      {
         AddComponent(*actorComp);
      }
      else
      {
         LOG_WARNING("Cannot add \"" + component.GetName() + "\" as an ActorComponent.");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActorComponentVector ActorComponentBase::GetComponents(ActorComponent::ACType type) const
   {
      ActorComponentVector foundComponents;

      GetComponents(type, foundComponents);

      return foundComponents;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::GetComponents(ActorComponent::ACType type, ActorComponentVector& outComponents) const
   {
      ActorComponentMap::const_iterator iter = mComponents.begin();
      while (iter != mComponents.end())
      {
         if (iter->first->InstanceOf(*type))
         {
            outComponents.push_back(iter->second.get());
         }
         ++iter;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::GetComponents(dtCore::ActorTypePtr type, dtCore::ActorPtrVector& outComponents) const
   {
      ActorComponentVector comps;
      GetComponents(type, comps);
      outComponents.insert(outComponents.end(), comps.begin(), comps.end());
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::GetAllComponents(dtGame::ActorComponentVector& toFill)
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
   void ActorComponentBase::GetAllComponents(dtGame::ActorComponentVectorConst& toFill) const
   {
      toFill.reserve(toFill.size() + mComponents.size());
      ActorComponentMap::const_iterator i, iend;
      i = mComponents.begin();
      iend = mComponents.end();
      for (; i != iend; ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::GetAllComponents(dtCore::ActorPtrVector& toFill)
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
         if (iter->first->InstanceOf(*type))
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
         if (mComponents[componentIndex].first->InstanceOf(*type))
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
         RemoveComponent(*(mComponents.back()).second);
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
