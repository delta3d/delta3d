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
#include <dtDirectorNodes/sendmessageaction.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

#include <dtGame/messagefactory.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   SendMessageAction::SendMessageAction()
      : ActionNode()
      , mAboutActorID(dtCore::UniqueId(""))
   {
      AddAuthor("Jeff P. Houde");

      mMessageType = "Map Changed";
   }

   ////////////////////////////////////////////////////////////////////////////////
   SendMessageAction::~SendMessageAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      UpdateName();

      mOutputs.push_back(OutputLink(this, "Success", "Activated when the message was successfully sent."));
      mOutputs.push_back(OutputLink(this, "Failed", "Activated when the message has failed to send."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();
      RemoveProperty("Name");

      dtCore::StringActorProperty* messageTypeProp = new dtCore::StringActorProperty(
         "Message Type", "Message Type",
         dtCore::StringActorProperty::SetFuncType(this, &SendMessageAction::SetMessageType),
         dtCore::StringActorProperty::GetFuncType(this, &SendMessageAction::GetMessageType),
         "The name of the message type.");
      AddProperty(messageTypeProp);

      dtCore::ActorIDActorProperty* aboutActorProp = new dtCore::ActorIDActorProperty(
         "AboutActorID", "AboutActorID",
         dtCore::ActorIDActorProperty::SetFuncType(this, &SendMessageAction::SetAboutActorID),
         dtCore::ActorIDActorProperty::GetFuncType(this, &SendMessageAction::GetAboutActorID),
         "", "The actor this message is about.");
      AddProperty(aboutActorProp);

      mValues.push_back(ValueLink(this, messageTypeProp));
      mValues.push_back(ValueLink(this, aboutActorProp));
   }

   //////////////////////////////////////////////////////////////////////////
   bool SendMessageAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      std::string messageType = GetString("Message Type");

      // Find the message type, and create it.
      bool bSucceeded = false;
      dtGame::GameManager* gm = GetDirector()->GetGameManager();
      if (gm)
      {
         const dtGame::MessageType* msgType = gm->GetMessageFactory().GetMessageTypeByName(messageType);
         if (msgType)
         {
            dtCore::RefPtr<dtGame::Message> message = gm->GetMessageFactory().CreateMessage(*msgType);

            if (message.valid())
            {
               message->SetAboutActorId(GetActorID("AboutActorID"));
               gm->SendMessage(*message);
               OutputLink* output = GetOutputLink("Success");
               if (output) output->Activate();
               bSucceeded = true;
            }
         }
      }

      if (!bSucceeded)
      {
         OutputLink* output = GetOutputLink("Failed");
         if (output) output->Activate();
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   void SendMessageAction::SetMessageType(const std::string& typeName)
   {
      mMessageType = typeName;
      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& SendMessageAction::GetMessageType() const
   {
      return mMessageType;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::SetAboutActorID(const dtCore::UniqueId& value)
   {
      mAboutActorID = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId SendMessageAction::GetAboutActorID()
   {
      return mAboutActorID;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::UpdateName()
   {
      mName = GetString("Message Type");
   }
}

////////////////////////////////////////////////////////////////////////////////
