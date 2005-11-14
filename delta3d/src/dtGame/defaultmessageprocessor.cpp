/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author William E. Johnson II
 */

#include "dtGame/defaultmessageprocessor.h"
#include "dtGame/gamemanager.h"
#include "dtGame/message.h"
#include "dtGame/actorupdatemessage.h"
#include "dtGame/basemessages.h"
#include "dtGame/messagetype.h"
#include "dtGame/gameactor.h"

namespace dtGame
{
   DefaultMessageProcessor::DefaultMessageProcessor() : GMComponent()
   {

   }

   DefaultMessageProcessor::~DefaultMessageProcessor()
   {

   }

   void DefaultMessageProcessor::ProcessMessage(const Message &msg)
   {
      if(msg.GetMessageType() == MessageType::INFO_ACTOR_CREATED)
         ProcessCreateActor((ActorUpdateMessage&)msg);
      else if(msg.GetMessageType() == MessageType::INFO_ACTOR_UPDATED)
         ProcessUpdateActor((ActorUpdateMessage&)msg);
      else if(msg.GetMessageType() == MessageType::INFO_ACTOR_DELETED)
         ProcessDeleteActor((ActorDeletedMessage&)msg);
      else if(msg.GetMessageType() == MessageType::TICK_LOCAL)
         ProcessTick((TickMessage&)msg);
      else if(msg.GetMessageType() == MessageType::TICK_REMOTE)
         ProcessTick((TickMessage&)msg);
      else 
      {
         LOG_ERROR("The default message component is processing an unhandled local message");
         ProcessUnhandledLocalMessage(msg);
      }
   }

   void DefaultMessageProcessor::ProcessTick(const TickMessage &msg)
   {

   }

   void DefaultMessageProcessor::ProcessLocalCreateActor(const ActorUpdateMessage &msg)
   {

   }

   void DefaultMessageProcessor::ProcessLocalUpdateActor(const ActorUpdateMessage &msg)
   {

   }

   void DefaultMessageProcessor::ProcessLocalDeleteActor(const ActorDeletedMessage &msg)
   {

   }

   GameActorProxy* DefaultMessageProcessor::ProcessRemoteCreateActor(const ActorUpdateMessage &msg)
   {
      return NULL;
   }

   void DefaultMessageProcessor::ProcessRemoteUpdateActor(const ActorUpdateMessage &msg)
   {

   }

   void DefaultMessageProcessor::ProcessRemoteDeleteActor(const ActorDeletedMessage &msg)
   {

   }

   void DefaultMessageProcessor::ProcessCreateActor(const ActorUpdateMessage &msg)
   {
      GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetSendingActorId());
      if(proxy && proxy->IsRemote())
      {
         GameActorProxy *p = ProcessRemoteCreateActor(msg);
         GetGameManager()->AddActor(*p, true, false);
         // TODO call OnUpdateActor
      }
      else if(proxy && !proxy->IsRemote())
      {
         ProcessLocalCreateActor(msg);
      }
      else
      {
         LOG_ERROR("Message received from an invalid proxy");
         return;
      }
   }

   void DefaultMessageProcessor::ProcessUpdateActor(const ActorUpdateMessage &msg)
   {
      GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetSendingActorId());
      if(proxy && proxy->IsRemote())
         ProcessRemoteUpdateActor(msg);
      else if(proxy && !proxy->IsRemote())
         ProcessLocalUpdateActor(msg);
      else
         ProcessCreateActor(msg);   
   }

   void DefaultMessageProcessor::ProcessDeleteActor(const ActorDeletedMessage &msg)
   {
      GameActorProxy *proxy = GetGameManager()->FindGameActorById(msg.GetSendingActorId());
      if(proxy && proxy->IsRemote())
         ProcessRemoteDeleteActor(msg);
      else if(proxy && !proxy->IsRemote())
         ProcessRemoteDeleteActor(msg);
      else
      {
         LOG_ERROR("Message received from an invalid proxy");
         return;
      }
   }

   void DefaultMessageProcessor::ProcessUnhandledLocalMessage(const Message &msg)
   {
   
   }
      
   void DefaultMessageProcessor::ProcessUnhandledRemoteMessage(const Message &msg)
   {

   }
}
