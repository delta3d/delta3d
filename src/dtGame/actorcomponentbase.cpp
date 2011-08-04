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
#include <dtGame/actorcomponentbase.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>
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
      // safety check
      assert(dynamic_cast<GameActor*>(this) != NULL &&
               "ActorComponentBase must derive from dtGame::GameActor!");

      // only one component of a type at a time!
      if (HasComponent(component.GetType()))
      {
         std::ostringstream os;
         os << "Actor component of type " << component.GetType().Get() << " already exists!";
         throw dtUtil::Exception(os.str(), __FILE__, __LINE__);
      }

      // store component
      mComponents[component.GetType()] = &component;

      // pass the component a pointer to its owner
      component.SetOwner(this);

      GameActor* self = static_cast<GameActor*>(this);

      // The call to Init should eventually move to an actor component library behavior
      // like actors have, but until then, this is the only other place to do it.
      component.Init();

      // add actor component properties to the game actor itself
      // note - the only reason we do this is to make other parts of the system work (like STAGE). 
      // In the future, STAGE (et al) should use the actor components directly and we won't add them to the game actor
      // Remove the props from the game actor - This is temporary. See the note in AddComponent()
      self->GetGameActorProxy().AddActorComponentProperties(component);

      // initialize component
      component.OnAddedToActor(*self);
      OnActorComponentAdded(component);

      // if base class is a game actor and the game actor is already instantiated in game:
      if (self != NULL && self->GetGameActorProxy().IsInGM())
      {
         component.SetIsInGM(true);
         component.OnEnteredWorld();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   ActorComponent* ActorComponentBase::GetComponent(const ActorComponent::ACType& type) const
   {
      ActorComponentMap::const_iterator iter = mComponents.find(type);
      if (iter == mComponents.end())
      {
         return NULL;
         // Should not through an exception, all the comments say NULL is OK.
         //std::ostringstream os;
         //os << "Actor component of type " << type.Get() << " not found!";
         //throw dtUtil::Exception(os.str(), __FILE__, __LINE__);
      }
      else 
      {
         return (*iter).second.get();
      }
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
   bool ActorComponentBase::HasComponent(const ActorComponent::ACType& type) const
   {
      ActorComponentMap::const_iterator iter = mComponents.find(type);
      return iter != mComponents.end();
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::RemoveComponent(const ActorComponent::ACType& type)
   {
      // safety check
      assert(dynamic_cast<GameActor*>(this) != NULL &&
               "ActorComponentBase must derive from dtGame::GameActor!");

      ActorComponentMap::iterator iter = mComponents.find(type);
      if (iter != mComponents.end())
      {
         ActorComponent& component = (*iter->second);
         GameActor* self = static_cast<GameActor*>(this);
         GameActorProxy* gap = NULL;
         if (self->IsGameActorProxyValid())
         {
            gap = &self->GetGameActorProxy();
         }

         if (self != NULL && component.GetIsInGM() || (gap != NULL && gap->IsInGM()))
         {
            component.SetIsInGM(false);
            component.OnRemovedFromWorld();
         }
         component.OnRemovedFromActor(*self);

         // Remove the props from the game actor - This is temporary. See the note in AddComponent()
         if (gap != NULL)
         {
            gap->RemoveActorComponentProperties(component);
         }

         OnActorComponentRemoved(component);
         mComponents.erase(iter);
         return;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorComponentBase::RemoveComponent(ActorComponent& component)
   {
      RemoveComponent(component.GetType());
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
