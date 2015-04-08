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
#include <dtDirectorNodes/gamemessageevent.h>

#include <dtCore/stringactorproperty.h>
#include <dtCore/actorproxy.h>

#include <dtDirector/director.h>
#include <dtDirector/messagegmcomponent.h>

#include <dtGame/messagetype.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   GameMessageEvent::GameMessageEvent()
       : EventNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   GameMessageEvent::~GameMessageEvent()
   {
      UnRegisterMessages();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameMessageEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtCore::StringActorProperty* messageTypeProp = new dtCore::StringActorProperty(
         "Message Type", "Message Type",
         dtCore::StringActorProperty::SetFuncType(this, &GameMessageEvent::SetMessageType),
         dtCore::StringActorProperty::GetFuncType(this, &GameMessageEvent::GetMessageType),
         "The name of the message type.");
      AddProperty(messageTypeProp);

      mValues.push_back(ValueLink(this, messageTypeProp, false, false, true, false));

      SetMessageType("Map Changed");
   }

   //////////////////////////////////////////////////////////////////////////
   void GameMessageEvent::SetMessageType(const std::string& typeName)
   {
      mMessageType = typeName;

      RegisterMessages();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& GameMessageEvent::GetMessageType() const
   {
      return mMessageType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameMessageEvent::OnLinkValueChanged(const std::string& linkName)
   {
      EventNode::OnLinkValueChanged(linkName);

      if (linkName == "Message Type")
      {
         RegisterMessages();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameMessageEvent::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
   {
      if (EventNode::CanConnectValue(link, value))
      {
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameMessageEvent::RegisterMessages()
   {
      std::string newMessageType = GetString("Message Type");
      if (mLastMessageType != newMessageType)
      {
         UnRegisterMessages();
         mLastMessageType = newMessageType;

         mName = mLastMessageType;

         dtDirector::MessageGMComponent* component = GetDirector()->GetMessageGMComponent();
         if (component)
         {
            component->RegisterMessage(mLastMessageType, this,
               dtDirector::MessageGMComponent::MsgFunc(this, &dtDirector::GameMessageEvent::OnMessage));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameMessageEvent::UnRegisterMessages()
   {
      if (mLastMessageType != "")
      {
         dtDirector::MessageGMComponent* component = GetDirector()->GetMessageGMComponent();
         if (component)
         {
            component->UnRegisterMessage(mLastMessageType, this);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameMessageEvent::OnMessage(const dtGame::Message& message)
   {
      if (IsEnabled() && GetDirector()->IsEnabled())
      {
         // Trigger this event on this message.
         Trigger("Out", &message.GetAboutActorId());
      }
   }
}
