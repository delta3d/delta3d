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

#include <dtGame/actorcomponentbase.h>
#include <dtGame/gameactor.h>
#include <dtUtil/exception.h>
#include <cassert>
#include <sstream>

using namespace dtGame;

/////////////////////////////////////////////////////
ActorComponentBase::ActorComponentBase()
{
}


//////////////////////////////////////////////////////////////////////////
ActorComponentBase::~ActorComponentBase()
{
}


//////////////////////////////////////////////////////////////////////////
void ActorComponentBase::AddComponent(ActorComponent* component)
{
   // only one component of a type at a time!
   if (HasComponent(component->GetType()))
   {
      std::ostringstream os;
      os << "Actor component of type " << component->GetType().Get() << " already exists!";
      throw dtUtil::Exception(os.str(), __FILE__, __LINE__);
   }

   // store component
   mComponents[component->GetType()] = component;

   // pass the component a pointer to its owner
   component->SetOwner(this);

   // if base class is a game actor and the game actor is already instantiated in game:
   GameActor* self = static_cast<GameActor*>(this);
   if (self->GetGameActorProxy().IsInGM())
   {
      // initialize component
      component->OnAddedToActor(*self);
      OnActorComponentAdded(component);
   }
}

//////////////////////////////////////////////////////////////////////////
ActorComponent* ActorComponentBase::GetComponent(const ActorComponent::ACType& type) const
{
   ActorComponentMap::const_iterator iter = mComponents.find(type);
   if (iter == mComponents.end())
   {
      std::ostringstream os;
      os << "Actor component of type " << type.Get() << " not found!";
      throw dtUtil::Exception(os.str(), __FILE__, __LINE__);
   }
   return (*iter).second.get();
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
   ActorComponentMap::iterator iter = mComponents.find(type);
   if (iter != mComponents.end())
   {
      iter->second->OnRemovedFromActor(*static_cast<GameActor*>(this));
      OnActorComponentRemoved(iter->second.get());
      mComponents.erase(iter);
      return;
   }
}

//////////////////////////////////////////////////////////////////////////
void ActorComponentBase::RemoveComponent(ActorComponent* component)
{
   RemoveComponent(component->GetType());
}

//////////////////////////////////////////////////////////////////////////
void ActorComponentBase::RemoveAllComponents()
{
   while (!mComponents.empty())
   {
      RemoveComponent((*mComponents.begin()).second.get());
   }
}

//////////////////////////////////////////////////////////////////////////
void ActorComponentBase::InitComponents()
{
   // safety check
   assert(dynamic_cast<GameActor*>(this) != NULL &&
      "ActorComponentBase must derive from dtGame::GameActor!");

   // loop through all components and call their OnAddedToActor method
   ActorComponentMap::iterator iter;
   for (iter = mComponents.begin(); iter != mComponents.end(); ++iter)
   {
      (*iter).second->OnAddedToActor(*static_cast<GameActor*>(this));
      OnActorComponentAdded((*iter).second.get());
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

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponentBase::OnActorComponentAdded(ActorComponent* component)
{
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponentBase::OnActorComponentRemoved(ActorComponent* component)
{
}
