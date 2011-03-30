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

#include <dtDirectorNodes/sendmessageaction.h>

#include <dtDAL/stringactorproperty.h>

#include <dtDirector/director.h>

#include <dtGame/messagefactory.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   SendMessageAction::SendMessageAction()
      : ActionNode()
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

      UpdateLabel();

      mOutputs.push_back(OutputLink(this, "Success"));
      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringActorProperty* messageTypeProp = new dtDAL::StringActorProperty(
         "Message Type", "Message Type",
         dtDAL::StringActorProperty::SetFuncType(this, &SendMessageAction::SetMessageType),
         dtDAL::StringActorProperty::GetFuncType(this, &SendMessageAction::GetMessageType),
         "The name of the message type.");
      AddProperty(messageTypeProp);

      mValues.push_back(ValueLink(this, messageTypeProp, false, false, true, true));
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
      UpdateLabel();
   }

   //////////////////////////////////////////////////////////////////////////
   void SendMessageAction::SetMessageType(const std::string& typeName)
   {
      mMessageType = typeName;
      UpdateLabel();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& SendMessageAction::GetMessageType() const
   {
      return mMessageType;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& SendMessageAction::GetName()
   {
      return mLabel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendMessageAction::UpdateLabel()
   {
      std::string messageType = GetString("Message Type");
      mLabel = messageType;
   }
}

////////////////////////////////////////////////////////////////////////////////
