/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/gameeventmessageevent.h>

#include <dtCore/actorproxy.h>
#include <dtCore/gameeventactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/messagegmcomponent.h>

#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   GameEventMessageEvent::GameEventMessageEvent()
       : EventNode()
       , mEvent(NULL)
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   GameEventMessageEvent::~GameEventMessageEvent()
   {
      UnRegisterMessages();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtCore::GameEventActorProperty* eventProp = new dtCore::GameEventActorProperty(
         "Event", "Event",
         dtCore::GameEventActorProperty::SetFuncType(this, &GameEventMessageEvent::SetEvent),
         dtCore::GameEventActorProperty::GetFuncType(this, &GameEventMessageEvent::GetEvent),
         "The Game Event.  Entries are set in the map properties.");
      AddProperty(eventProp);

      mValues.push_back(ValueLink(this, eventProp, false, false, false, false));
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameEventMessageEvent::UsesActorFilters()
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::SetEvent(dtCore::GameEvent* value)
   {
      mEvent = value;

      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::GameEvent* GameEventMessageEvent::GetEvent() const
   {
      return mEvent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::OnLinkValueChanged(const std::string& linkName)
   {
      EventNode::OnLinkValueChanged(linkName);

      if (linkName == "Event")
      {
         UpdateName();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameEventMessageEvent::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
   {
      if (EventNode::CanConnectValue(link, value))
      {
         if (link->GetName() == "Event")
         {
            if (value->CanBeType(dtCore::DataType::STRING))
            {
               return true;
            }
            return false;
         }
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::RegisterMessages()
   {
      dtDirector::MessageGMComponent* component = GetDirector()->GetMessageGMComponent();
      if (component)
      {
         component->RegisterMessage("Game Event", this,
            dtDirector::MessageGMComponent::MsgFunc(this, &dtDirector::GameEventMessageEvent::OnMessage));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::UnRegisterMessages()
   {
      dtDirector::MessageGMComponent* component = GetDirector()->GetMessageGMComponent();
      if (component)
      {
         component->UnRegisterMessage("Game Event", this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::OnMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
      {
         const dtGame::GameEventMessage& eventMsg =
            static_cast<const dtGame::GameEventMessage&>(message);

         dtCore::GameEvent* gameEvent = GetGameEvent("Event");
         if (eventMsg.GetGameEvent() == gameEvent)
         {
            Trigger("Out");
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::UpdateName()
   {
      dtCore::GameEvent* gameEvent = GetGameEvent("Event");

      mName = gameEvent ? gameEvent->GetName() : "";
   }
}
