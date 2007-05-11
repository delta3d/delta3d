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
 * William E. Johnson II
 */
#include <prefix/dtgameprefix-src.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtUtil/log.h>

namespace dtGame
{
   DefaultNetworkPublishingComponent::DefaultNetworkPublishingComponent(const std::string& name) : GMComponent(name)
   {
      mLogger = &dtUtil::Log::GetInstance("defaultnetworkpublishingcomponent.cpp");
   }

   DefaultNetworkPublishingComponent::~DefaultNetworkPublishingComponent()
   {

   }

   void DefaultNetworkPublishingComponent::ProcessMessage(const Message &msg)
   {
      if (GetGameManager() == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "DefaultNetworkPublishingComponent has no GameManager, but a message was received.");
         return;
      }

      if (msg.GetSource() == GetGameManager()->GetMachineInfo())
      {
         if(msg.GetMessageType() == MessageType::TICK_LOCAL)
         {
            ProcessTick(static_cast<const TickMessage&>(msg));
         }
         else if(msg.GetMessageType() == MessageType::TICK_REMOTE)
         {
            ProcessTick(static_cast<const TickMessage&>(msg));
         }
         else if(msg.GetMessageType() == MessageType::INFO_ACTOR_PUBLISHED)
         {
            GameActorProxy* ga = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
            if(ga && ga->IsPublished())
               ProcessPublishActor(static_cast<const ActorPublishedMessage&>(msg));
         }
         else if(msg.GetMessageType() == MessageType::INFO_ACTOR_DELETED)
         {
            GameActorProxy *ga = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
            if(ga && ga->IsPublished())
               ProcessDeleteActor(static_cast<const ActorDeletedMessage&>(msg));
         }
         else if(msg.GetMessageType() == MessageType::INFO_ACTOR_UPDATED)
         {
            GameActorProxy *ga = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
            if(ga && ga->IsPublished())
               ProcessUpdateActor(static_cast<const ActorUpdateMessage&>(msg));
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Received a message of unknown type: ",  msg.GetMessageType().GetName().c_str());
            ProcessUnhandledLocalMessage(msg);
         }
      }
      else
      {
         ProcessUnhandledRemoteMessage(msg);  
      }
   }

   void DefaultNetworkPublishingComponent::DispatchNetworkMessage(const Message &msg)
   {
      if (GetGameManager() == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "DefaultNetworkPublishingComponent has no GameManager, but a message was received.");
         return;
      }
   }

   void DefaultNetworkPublishingComponent::ProcessTick(const TickMessage &msg)
   {
   }

   void DefaultNetworkPublishingComponent::ProcessPublishActor(const ActorPublishedMessage &msg)
   {
      GameActorProxy *gap = GetGameManager()->FindGameActorById(msg.GetSendingActorId());
      if(gap != NULL)
      {
         dtCore::RefPtr<Message> newMsg = GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_CREATED);
         gap->PopulateActorUpdate(static_cast<ActorUpdateMessage&>(*newMsg));
         GetGameManager()->SendNetworkMessage(*newMsg);
      }
      else
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Received a publish message from an actor that isn't part of the GameManager");
   }

   void DefaultNetworkPublishingComponent::ProcessDeleteActor(const ActorDeletedMessage &msg)
   {
      GameActorProxy *gap = GetGameManager()->FindGameActorById(msg.GetSendingActorId());
      if(gap != NULL)
      {
         try
         {
            GetGameManager()->SendNetworkMessage(msg);
         }
         catch (const dtUtil::Exception& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Exception trying to clone a delete message: ", ex.What().c_str());
         }
      }
      else
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Received a delete actor message from an actor that isn't part of the GameManager");
   }

   void DefaultNetworkPublishingComponent::ProcessUpdateActor(const ActorUpdateMessage &msg)
   {
      GameActorProxy *gap = GetGameManager()->FindGameActorById(msg.GetSendingActorId());
      if(gap != NULL)
      {
         GetGameManager()->SendNetworkMessage(msg);
      }
      else
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Received a update actor message from an actor that isn't part of the GameManager");
   }

   void DefaultNetworkPublishingComponent::ProcessUnhandledLocalMessage(const Message &msg)
   {

   }

   void DefaultNetworkPublishingComponent::ProcessUnhandledRemoteMessage(const Message &msg)
   {

   }
}
