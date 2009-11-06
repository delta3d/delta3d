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

#include <dtGame/actorcomponent.h>
#include <cassert>
#include <dtGame/gameactor.h>
#include <dtDAL/functor.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/basemessages.h> //for TickMessage

using namespace dtGame;

////////////////////////////////////////////////////////////////////////////////
dtGame::ActorComponent::ActorComponent(const ACType& type) : 
  mOwner(NULL)
, mType(type) 
{

}


////////////////////////////////////////////////////////////////////////////////
dtGame::ActorComponent::~ActorComponent()
{

}

////////////////////////////////////////////////////////////////////////////////
const dtGame::ActorComponent::ACType& dtGame::ActorComponent::GetType() const
{
   return mType;
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponent::OnAddedToActor(dtGame::GameActor& actor)
{

}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponent::OnRemovedFromActor(dtGame::GameActor& actor)
{

}

////////////////////////////////////////////////////////////////////////////////
ActorComponentBase* dtGame::ActorComponent::GetOwner() const
{
   return mOwner;
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponent::SetOwner(ActorComponentBase* owner)
{
   mOwner = owner;
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponent::OnTickLocal(const TickMessage& tickMessage)
{

}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ActorComponent::BuildPropertyMap()
{

}

//////////////////////////////////////////////////////////////////////////
void ActorComponent::RegisterForTicks()
{
   assert(GetOwner() != NULL);
   GameActor* owner;
   GetOwner(owner);
   std::string tickInvokable = "Tick Local " + GetType().Get();
   if(!owner->GetGameActorProxy().GetInvokable(tickInvokable))
   {
      owner->GetGameActorProxy().AddInvokable(*new Invokable(tickInvokable, dtDAL::MakeFunctor(*this, &ActorComponent::OnTickLocal)));
   }
   owner->GetGameActorProxy().RegisterForMessages(MessageType::TICK_LOCAL, tickInvokable);
}

//////////////////////////////////////////////////////////////////////////
void ActorComponent::UnregisterForTicks()
{
   assert(GetOwner() != NULL);
   GameActor* owner;
   GetOwner(owner);
   std::string tickInvokable = "Tick Local " + GetType().Get();
   owner->GetGameActorProxy().UnregisterForMessages(MessageType::TICK_LOCAL, tickInvokable);
   owner->GetGameActorProxy().RemoveInvokable(tickInvokable);
}

