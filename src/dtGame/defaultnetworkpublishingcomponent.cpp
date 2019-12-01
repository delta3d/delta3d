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
#include <prefix/dtgameprefix.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/gameactorproxy.h>
#include <dtCore/actortype.h>
#include <dtUtil/log.h>

namespace dtGame
{
   /////////////////////////////////////////////////////////////////////////////
   DefaultNetworkPublishingComponent::DefaultNetworkPublishingComponent(dtCore::SystemComponentType& type)
   : GMComponent(type)
   {
      mLogger = &dtUtil::Log::GetInstance("defaultnetworkpublishingcomponent.cpp");
      //Set the name so subclasses get the same name.
      SetName(DEFAULT_NAME);
   }

   /////////////////////////////////////////////////////////////////////////////
   DefaultNetworkPublishingComponent::~DefaultNetworkPublishingComponent()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::ProcessMessage(const Message& msg)
   {
      if (GetGameManager() == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "DefaultNetworkPublishingComponent has no GameManager, but a message was received.");
         return;
      }

      if (msg.GetSource() == GetGameManager()->GetMachineInfo())
      {
         if (msg.GetMessageType() == MessageType::INFO_ACTOR_PUBLISHED)
         {
            GameActorProxy* ga = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
            if (ga != NULL && ga->IsPublished())
               ProcessPublishActor(msg, *ga);
         }
         else if (msg.GetMessageType() == MessageType::INFO_ACTOR_DELETED)
         {
            GameActorProxy* ga = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
            if (ga != NULL && ga->IsPublished())
               ProcessDeleteActor(msg);
         }
         else if (msg.GetMessageType() == MessageType::INFO_ACTOR_UPDATED)
         {
            GameActorProxy* ga = GetGameManager()->FindGameActorById(msg.GetAboutActorId());
            if (ga != NULL && ga->IsPublished())
               ProcessUpdateActor(static_cast<const ActorUpdateMessage&>(msg));
         }
         else if (PublishesAdditionalMessageType(msg.GetMessageType()))
         {
            GetGameManager()->SendNetworkMessage(msg);
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Received a message of unknown type: %s",  msg.GetMessageType().GetName().c_str());
            ProcessUnhandledLocalMessage(msg);
         }
      }
      else
      {
         ProcessUnhandledRemoteMessage(msg);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::DispatchNetworkMessage(const Message& msg)
   {
      if (GetGameManager() == NULL)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "DefaultNetworkPublishingComponent has no GameManager, but a message was received.");
         return;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool DefaultNetworkPublishingComponent::PublishesAdditionalMessageType(const MessageType& type) const
   {
      return mMessageTypesToPublish.find(&type) != mMessageTypesToPublish.end();
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::AddMessageTypeToPublish(const MessageType& type)
   {
      mMessageTypesToPublish.insert(&type);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::RemoveMessageTypeToPublish(const MessageType& type)
   {
      std::set<const MessageType*>::iterator i = mMessageTypesToPublish.find(&type);
      if (i != mMessageTypesToPublish.end())
      {
         mMessageTypesToPublish.erase(i);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::ProcessPublishActor(const Message& msg, GameActorProxy& gap)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                  "Publishing Actor \"" + gap.GetName() + "\" With type \"" + gap.GetActorType().GetFullName() + "\"");
      }

      dtCore::RefPtr<Message> newMsg = GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_CREATED);
      gap.PopulateActorUpdate(static_cast<ActorUpdateMessage&>(*newMsg));
      GetGameManager()->SendNetworkMessage(*newMsg);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::ProcessDeleteActor(const Message& msg)
   {
      GetGameManager()->SendNetworkMessage(msg);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::ProcessUpdateActor(const ActorUpdateMessage& msg)
   {
      GetGameManager()->SendNetworkMessage(msg);
   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::ProcessUnhandledLocalMessage(const Message& msg)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   void DefaultNetworkPublishingComponent::ProcessUnhandledRemoteMessage(const Message &msg)
   {

   }
}
