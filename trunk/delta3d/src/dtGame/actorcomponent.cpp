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
#include <dtGame/actorcomponent.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/basemessages.h> //for TickMessage
#include <dtUtil/log.h>
#include <stdexcept>

namespace dtGame
{

const ActorComponent::ACType ActorComponent::BaseActorComponentType(new dtCore::ActorType("Base", "ActorComponents", "A base type so that all actor component types should set as a parent"));


////////////////////////////////////////////////////////////////////////////////
ActorComponent::ActorComponent(ActorComponent::ACType type)
  : mOwner(NULL)
  , mType(type)
  , mInitialized(false)
  , mIsInGM(false)
{
}


////////////////////////////////////////////////////////////////////////////////
ActorComponent::~ActorComponent()
{

}

////////////////////////////////////////////////////////////////////////////////
ActorComponent::ACType ActorComponent::GetType() const
{
   return mType;
}

////////////////////////////////////////////////////////////////////////////////
ActorComponentContainer* ActorComponent::GetOwner() const
{
   return mOwner;
}

////////////////////////////////////////////////////////////////////////////////
void ActorComponent::SetOwner(ActorComponentContainer* owner)
{
   mOwner = owner;
}

//////////////////////////////////////////////////////////////////////////
void ActorComponent::Init(const dtCore::ActorType& actorType)
{
   if (!mInitialized)
   {
      BaseClass::Init(actorType);
      mInitialized = true;
   }
}

//////////////////////////////////////////////////////////////////////////
void ActorComponent::RegisterForTicks()
{
   GameActorProxy* owner = NULL;
   GetOwner(owner);
   std::string tickInvokable = "Tick Local " + GetType()->GetFullName();
   if(!owner->GetInvokable(tickInvokable))
   {
      owner->AddInvokable(*new Invokable(tickInvokable, dtUtil::MakeFunctor(&ActorComponent::OnTickLocal, this)));
   }
   owner->RegisterForMessages(MessageType::TICK_LOCAL, tickInvokable);
}

//////////////////////////////////////////////////////////////////////////
void ActorComponent::UnregisterForTicks()
{
   GameActorProxy* owner = NULL;
   GetOwner(owner);
   std::string tickInvokable = "Tick Local " + GetType()->GetFullName();
   owner->UnregisterForMessages(MessageType::TICK_LOCAL, tickInvokable);
   owner->RemoveInvokable(tickInvokable);
}

////////////////////////////////////////////////////////////////////////////
//void ActorComponent::GetOwner(GameActor* ga) const
//{
//   LOG_ERROR("Error, you called the GetOwner template, but expected a type derived from GameActor.  "
//         "This is no longer supported, you need to fix your code.  This will assert in Debug so you can find it more easily.");
//   throw std::bad_cast();
//}

//////////////////////////////////////////////////////////////////////////
DT_IMPLEMENT_ACCESSOR(ActorComponent, bool, IsInGM);
}
