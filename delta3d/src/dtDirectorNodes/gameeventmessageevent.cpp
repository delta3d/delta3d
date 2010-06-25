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

#include <dtDirectorNodes/gameeventmessageevent.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/gameeventactorproperty.h>

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
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      EventNode::Init(nodeType, graph);

      mLabel = EventNode::GetName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtDAL::GameEventActorProperty* eventProp = new dtDAL::GameEventActorProperty(
         "Event", "Event",
         dtDAL::GameEventActorProperty::SetFuncType(this, &GameEventMessageEvent::SetEvent),
         dtDAL::GameEventActorProperty::GetFuncType(this, &GameEventMessageEvent::GetEvent),
         "The Game Event.  Entries are set in the map properties.");
      AddProperty(eventProp);

      mValues.push_back(ValueLink(this, eventProp, false, false, true, false));
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameEventMessageEvent::UsesInstigator()
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::SetEvent(dtDAL::GameEvent* value)
   {
      mEvent = value;

      UpdateLabel();
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::GameEvent* GameEventMessageEvent::GetEvent() const
   {
      return mEvent;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& GameEventMessageEvent::GetName()
   {
      return mLabel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::OnLinkValueChanged(const std::string& linkName)
   {
      if (linkName == "Event")
      {
         UpdateLabel();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameEventMessageEvent::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
   {
      return true;
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

         dtDAL::GameEvent* gameEvent = GetGameEvent("Event");
         if (eventMsg.GetGameEvent() == gameEvent)
         {
            Trigger();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameEventMessageEvent::UpdateLabel()
   {
      std::string eventName = "";
      dtDAL::GameEvent* gameEvent = GetGameEvent("Event");
      if (gameEvent)
      {
         eventName = gameEvent->GetName();
      }
      mLabel = GetType().GetName() + " (" + eventName + ")";
   }
}
