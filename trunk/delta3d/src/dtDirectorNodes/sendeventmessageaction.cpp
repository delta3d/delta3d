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
#include <dtDirectorNodes/sendeventmessageaction.h>

#include <dtDAL/gameeventactorproperty.h>

#include <dtDirector/director.h>

#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   SendEventMessageAction::SendEventMessageAction()
      : ActionNode()
      , mEvent(NULL)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   SendEventMessageAction::~SendEventMessageAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendEventMessageAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      UpdateLabel();

      mOutputs.push_back(OutputLink(this, "Success"));
      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendEventMessageAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::GameEventActorProperty* eventProp = new dtDAL::GameEventActorProperty(
         "Event", "Event",
         dtDAL::GameEventActorProperty::SetFuncType(this, &SendEventMessageAction::SetEvent),
         dtDAL::GameEventActorProperty::GetFuncType(this, &SendEventMessageAction::GetEvent),
         "The Game Event.  Entries are set in the map properties.");
      AddProperty(eventProp);

      mValues.push_back(ValueLink(this, eventProp, false, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool SendEventMessageAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // Find the message type, and create it.
      bool bSucceeded = false;
      dtGame::GameManager* gm = GetDirector()->GetGameManager();
      if (gm)
      {
         dtCore::RefPtr<dtGame::Message> message = gm->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);
         if (message.valid())
         {
            dtGame::GameEventMessage* eventMsg = dynamic_cast<dtGame::GameEventMessage*>(message.get());
            if (eventMsg)
            {
               dtDAL::GameEvent* gameEvent = GetGameEvent("Event");
               if (gameEvent)
               {
                  eventMsg->SetGameEvent(*gameEvent);

                  gm->SendMessage(*eventMsg);
                  OutputLink* output = GetOutputLink("Success");
                  if (output) output->Activate();
                  bSucceeded = true;
               }
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
   void SendEventMessageAction::OnLinkValueChanged(const std::string& linkName)
   {
      UpdateLabel();
   }

   //////////////////////////////////////////////////////////////////////////
   void SendEventMessageAction::SetEvent(dtDAL::GameEvent* gameEvent)
   {
      mEvent = gameEvent;
      UpdateLabel();
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::GameEvent* SendEventMessageAction::GetEvent() const
   {
      return mEvent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SendEventMessageAction::UpdateLabel()
   {
      dtDAL::GameEvent* gameEvent = GetGameEvent("Event");

      mName = gameEvent ? gameEvent->GetName() : "";
   }
}

////////////////////////////////////////////////////////////////////////////////
