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
#include "dtGame/loggermessages.h"
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
      RegisterMessageType<TickMessage>(MessageType::TICK_LOCAL);
      RegisterMessageType<TickMessage>(MessageType::TICK_REMOTE);
      RegisterMessageType<TimerElapsedMessage>(MessageType::INFO_TIMER_ELAPSED);
      RegisterMessageType<MapLoadedMessage>(MessageType::INFO_MAP_LOADED);

      RegisterMessageType<Message>(MessageType::INFO_PAUSED);
      RegisterMessageType<Message>(MessageType::INFO_RESUMED);
      RegisterMessageType<RestartMessage>(MessageType::INFO_RESTARTED);
      RegisterMessageType<TimeChangeMessage>(MessageType::INFO_TIME_CHANGED);
      
      RegisterMessageType<NetServerRejectMessage>(MessageType::NETSERVER_REJECT_CONNECTION);

      RegisterMessageType<Message>(MessageType::COMMAND_PAUSE);
      RegisterMessageType<Message>(MessageType::COMMAND_RESUME);
      RegisterMessageType<RestartMessage>(MessageType::COMMAND_RESTART);
      RegisterMessageType<TimeChangeMessage>(MessageType::COMMAND_SET_TIME);

      RegisterMessageType<ServerMessageRejected>(MessageType::SERVER_REQUEST_REJECTED);
      RegisterMessageType<ActorUpdateMessage>(MessageType::INFO_ACTOR_CREATED);
      RegisterMessageType<ActorUpdateMessage>(MessageType::INFO_ACTOR_UPDATED);
      RegisterMessageType<ActorUpdateMessage>(MessageType::INFO_ACTOR_DELETED);
      RegisterMessageType<ActorPublishedMessage>(MessageType::INFO_ACTOR_PUBLISHED);


      RegisterMessageType<Message>(MessageType::REQUEST_PAUSE);
      RegisterMessageType<Message>(MessageType::REQUEST_RESUME);
      RegisterMessageType<RestartMessage>(MessageType::REQUEST_RESTART);
      RegisterMessageType<TimeChangeMessage>(MessageType::REQUEST_SET_TIME);

      
      //Logger messages.
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_GET_KEYFRAMES);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_GET_LOGFILES);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_GET_TAGS);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_REQ_GET_STATUS);
      RegisterMessageType<dtGame::LogCaptureKeyframeMessage>(dtGame::MessageType::LOG_REQ_CAPTURE_KEYFRAME);
      RegisterMessageType<dtGame::LogInsertTagMessage>(dtGame::MessageType::LOG_REQ_INSERT_TAG);
      RegisterMessageType<dtGame::LogDeleteLogfileMessage>(dtGame::MessageType::LOG_REQ_DELETE_LOG);
      RegisterMessageType<dtGame::LogSetLogfileMessage>(dtGame::MessageType::LOG_REQ_SET_LOGFILE);
      RegisterMessageType<dtGame::LogSetAutoKeyframeIntervalMessage>
         (dtGame::MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL);
      //   static const MessageType LOG_INFO_KEYFRAMES;
      //   static const MessageType LOG_INFO_LOGFILES;
      //   static const MessageType LOG_INFO_TAGS;
      RegisterMessageType<dtGame::LogStatusMessage>(dtGame::MessageType::LOG_INFO_STATUS);
      RegisterMessageType<dtGame::Message>(dtGame::MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS);
      RegisterMessageType<dtGame::LogEndLoadKeyframeMessage>(dtGame::MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS);
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
      //default the message to be unknown.
      msg->SetDestination(NULL);
      return msg;
   }
   
   dtCore::RefPtr<Message> MessageFactory::CloneMessage(const Message& msg) throw(dtUtil::Exception)
   {
      dtCore::RefPtr<Message> theClone = CreateMessage(msg.GetMessageType());
      try 
      {
         msg.CopyDataTo(*theClone);
         if (msg.GetCausingMessage() != NULL)
         {
            dtCore::RefPtr<Message> causingClone = CloneMessage(*msg.GetCausingMessage());
            theClone->SetCausingMessage(causingClone.get());
         }
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
   
   const MessageType &MessageFactory::GetMessageTypeById(unsigned short id) const throw(dtUtil::Exception)
   {
      std::map<unsigned short, const MessageType*>::const_iterator itor = mIdMap.find(id);
      if (itor == mIdMap.end())
      {
         std::ostringstream ss;
         ss << "Message ID: " << id << " was not found in the message "
            "type map.";
         EXCEPT(MessageFactoryException::TYPE_NOT_REGISTERED,ss.str());
      }
       
      return *itor->second;
   }
   
   const MessageType* MessageFactory::GetMessageTypeByName(const std::string& name) const throw()
   {
      for (std::map<unsigned short, const MessageType*>::const_iterator i = mIdMap.begin(); i != mIdMap.end(); ++i)
      {
         if (i->second->GetName() == name)
            return i->second;
      }
      
      return NULL;       
   }
}
