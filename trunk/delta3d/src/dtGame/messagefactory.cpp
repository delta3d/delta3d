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

#include "dtGame/messagefactory.h"
#include "dtGame/message.h"
#include "dtGame/basemessages.h"
#include "dtGame/actorupdatemessage.h"
#include <dtCore/refptr.h>

namespace dtGame
{
   IMPLEMENT_ENUM(MessageFactory::MessageFactoryException);
   MessageFactory::MessageFactoryException MessageFactory::MessageFactoryException::TYPE_ALREADY_REGISTERED("Type already registered");
   MessageFactory::MessageFactoryException MessageFactory::MessageFactoryException::TYPE_NOT_REGISTERED("Type not registered");

   MessageFactory::MessageFactory(const std::string &name, 
                                  const MachineInfo& machine, 
                                  const std::string &desc) : 
   mName(name), 
   mDescription(desc), 
   mMachine(&machine) 
   {
      mMessageFactory = new dtUtil::ObjectFactory<const MessageType*, Message>;
      
      //base messages
      RegisterMessageType<dtGame::TickMessage>(dtGame::MessageType::TICK_LOCAL);
      RegisterMessageType<dtGame::TickMessage>(dtGame::MessageType::TICK_REMOTE);
      RegisterMessageType<dtGame::TimerElapsedMessage>(dtGame::MessageType::INFO_TIMER_ELAPSED);
      RegisterMessageType<dtGame::MapLoadedMessage>(dtGame::MessageType::INFO_MAP_LOADED);
      RegisterMessageType<dtGame::NetServerRejectMessage>(dtGame::MessageType::NETSERVER_REJECT_CONNECTION);
      RegisterMessageType<dtGame::RestartMessage>(dtGame::MessageType::COMMAND_RESTART);
      RegisterMessageType<dtGame::ServerMessageRejected>(dtGame::MessageType::SERVER_REQUEST_REJECTED);
      RegisterMessageType<dtGame::ActorUpdateMessage>(dtGame::MessageType::INFO_ACTOR_CREATED);
      RegisterMessageType<dtGame::ActorUpdateMessage>(dtGame::MessageType::INFO_ACTOR_UPDATED);
      RegisterMessageType<dtGame::ActorUpdateMessage>(dtGame::MessageType::INFO_ACTOR_DELETED);
      RegisterMessageType<dtGame::ActorPublishedMessage>(dtGame::MessageType::INFO_ACTOR_PUBLISHED);
   }

   MessageFactory::~MessageFactory()
   {

   }

   bool MessageFactory::IsMessageTypeSupported(const MessageType &msg) const throw()
   {
      return mMessageFactory->IsTypeSupported(&msg);
   }

   void MessageFactory::GetSupportedMessageTypes(std::vector<const MessageType*> &vec) throw()
   {
      mMessageFactory->GetSupportedTypes(vec);
   }

   dtCore::RefPtr<Message> MessageFactory::CreateMessage(const MessageType &msgType)  throw(dtUtil::Exception)
   {
      Message *msg = mMessageFactory->CreateObject(&msgType);

      if (msg == NULL)
      {
         LOGN_ERROR("messagefactory.cpp", "Object factory returned NULL, the message could not be created");
         EXCEPT(MessageFactory::MessageFactoryException::TYPE_NOT_REGISTERED, std::string("Could not create type ") + msgType.GetName());
      }
      msg->SetMessageType(msgType);
      msg->SetSource(*mMachine);
      //default the message to be for the current machine.
      msg->SetDestination(mMachine.get());
      return msg;
   }
   
   dtCore::RefPtr<Message> MessageFactory::CloneMessage(const Message& msg) throw(dtUtil::Exception)
   {
      dtCore::RefPtr<Message> theClone = CreateMessage(msg.GetMessageType());
      try 
      {
         msg.CopyDataTo(*theClone);
      }
      catch (const dtUtil::Exception& ex)
      {
         //log a little extra info about the exception.
         LOGN_DEBUG("messagefactory.cpp", 
            std::string("Exception trying to clone message of class ") + typeid(msg).name() 
            + " with type " + msg.GetMessageType().GetName() + ": " + ex.What());
         throw ex;
      }
      return theClone;
   }
   
}
