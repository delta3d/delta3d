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
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString INVOKABLE_PREFIX_TICK_LOCAL("Tick Local ");
   static const dtUtil::RefString INVOKABLE_PREFIX_TICK_REMOTE("Tick Remote ");

////////////////////////////////////////////////////////////////////////////////
GameActorComponent::GameActorComponent(ActorComponent::ACType type)
  : BaseClass(type)
{
}


////////////////////////////////////////////////////////////////////////////////
GameActorComponent::~GameActorComponent()
{

}

//////////////////////////////////////////////////////////////////////////
void GameActorComponent::RegisterForTick()
{
   GameActorProxy* owner = NULL;
   GetOwner(owner);

   if (owner == NULL) // This should probably be an error log
      return;

   if (!owner->IsRemote())
   {
      std::string tickInvokable = INVOKABLE_PREFIX_TICK_LOCAL.Get() + GetType()->GetFullName();
      if(!owner->GetInvokable(tickInvokable))
      {
         owner->AddInvokable(*new Invokable(tickInvokable, dtUtil::MakeFunctor(&GameActorComponent::OnTickLocal, this)));
      }
      owner->RegisterForMessages(MessageType::TICK_LOCAL, tickInvokable);
   }
   else
   {
      std::string tickInvokable = INVOKABLE_PREFIX_TICK_REMOTE.Get() + GetType()->GetFullName();
      if(!owner->GetInvokable(tickInvokable))
      {
         owner->AddInvokable(*new Invokable(tickInvokable, dtUtil::MakeFunctor(&GameActorComponent::OnTickLocal, this)));
      }
      owner->RegisterForMessages(MessageType::TICK_REMOTE, tickInvokable);
   }
 }

//////////////////////////////////////////////////////////////////////////
void GameActorComponent::UnregisterForTick()
{
   GameActorProxy* owner = NULL;
   GetOwner(owner);

   if (owner == NULL)
      return;

   if (!owner->IsRemote())
   {
      std::string tickInvokable = INVOKABLE_PREFIX_TICK_LOCAL.Get() + GetType()->GetFullName();
      owner->UnregisterForMessages(MessageType::TICK_LOCAL, tickInvokable);
   }
   else
   {
      std::string tickInvokable = INVOKABLE_PREFIX_TICK_REMOTE.Get() + GetType()->GetFullName();
      owner->UnregisterForMessages(MessageType::TICK_REMOTE, tickInvokable);
   }
}

}
