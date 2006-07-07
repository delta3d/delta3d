/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */
#include "countertaskactor.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/messagetype.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>
 
//////////////////////////////////////////////////////////////////////////////
CounterTaskActor::CounterTaskActor(dtGame::GameActorProxy &proxy) : dtActors::TaskActor(proxy)
{
}
 
//////////////////////////////////////////////////////////////////////////////
CounterTaskActor::~CounterTaskActor()
{
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
CounterTaskActorProxy::CounterTaskActorProxy()
{
   mCounter = -1000000;
   mTarget = 1000000;
}

//////////////////////////////////////////////////////////////////////////////
CounterTaskActorProxy::~CounterTaskActorProxy()
{
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::BuildPropertyMap()
{
   dtActors::TaskActorProxy::BuildPropertyMap();
   
   AddProperty(new dtDAL::IntActorProperty("Counter","Counter",
      dtDAL::MakeFunctor(*this,&CounterTaskActorProxy::SetCounter),
      dtDAL::MakeFunctorRet(*this,&CounterTaskActorProxy::GetCounter)));
      
   AddProperty(new dtDAL::IntActorProperty("TargetCount","TargetCount",
      dtDAL::MakeFunctor(*this,&CounterTaskActorProxy::SetTargetCount),
      dtDAL::MakeFunctorRet(*this,&CounterTaskActorProxy::GetTargetCount)));
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::BuildInvokables()
{
   dtActors::TaskActorProxy::BuildInvokables();
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::SetCounter(int value)
{
   mCounter = value;
   if (mCounter >= mTarget)
   {
      dtActors::TaskActor *task = dynamic_cast<dtActors::TaskActor *>(mActor.get());
      task->SetComplete(true);
   }
   
   if (!IsRemote() && GetGameManager() != NULL)
   {
      dtCore::RefPtr<dtGame::Message> updateMsg = 
         GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      
      GetGameManager()->SendMessage(*updateMsg);
   }
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::SetTargetCount(int value)
{
   mTarget = value;
    
   if (!IsRemote() && GetGameManager() != NULL)
   {
      dtCore::RefPtr<dtGame::Message> updateMsg = 
         GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      
      GetGameManager()->SendMessage(*updateMsg);
   }
}
