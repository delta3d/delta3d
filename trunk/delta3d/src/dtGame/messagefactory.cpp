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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <dtGame/messagefactory.h>
#include <dtGame/message.h>
#include <dtGame/basemessages.h>
#include <dtGame/loggermessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtCore/refptr.h>
#include <sstream>

#include <typeinfo>

namespace dtGame
{
   IMPLEMENT_ENUM(MessageFactory::MessageFactoryException);
   MessageFactory::MessageFactoryException MessageFactory::MessageFactoryException::TYPE_ALREADY_REGISTERED("Type already registered");
   MessageFactory::MessageFactoryException MessageFactory::MessageFactoryException::TYPE_NOT_REGISTERED("Type not registered");

   /////////////////////////////////////////////////////////////////
   MessageFactory::MessageFactory(const std::string& name,
                                  const MachineInfo& machine,
                                  const std::string& desc) :
   mName(name),
   mDescription(desc),
   mMachine(&machine)
   {
      mMessageFactory = new dtUtil::ObjectFactory<const MessageType*, Message>;

      //base messages
      RegisterMessageType<TickMessage>(MessageType::TICK_LOCAL);
      RegisterMessageType<TickMessage>(MessageType::TICK_REMOTE);
      RegisterMessageType<TickMessage>(MessageType::TICK_END_OF_FRAME);
      RegisterMessageType<TimerElapsedMessage>(MessageType::INFO_TIMER_ELAPSED);

      RegisterMessageType<MapMessage>(MessageType::INFO_MAP_LOADED);
      RegisterMessageType<MapMessage>(MessageType::INFO_MAP_UNLOADED);
      RegisterMessageType<MapMessage>(MessageType::INFO_MAP_LOAD_BEGIN);
      RegisterMessageType<MapMessage>(MessageType::INFO_MAP_UNLOAD_BEGIN);
      RegisterMessageType<MapMessage>(MessageType::INFO_MAP_CHANGE_BEGIN);
      RegisterMessageType<MapMessage>(MessageType::INFO_MAP_CHANGED);

      RegisterMessageType<Message>(MessageType::INFO_PAUSED);
      RegisterMessageType<Message>(MessageType::INFO_RESUMED);
      RegisterMessageType<RestartMessage>(MessageType::INFO_RESTARTED);
      RegisterMessageType<TimeChangeMessage>(MessageType::INFO_TIME_CHANGED);

      RegisterMessageType<NetServerRejectMessage>(MessageType::NETSERVER_REJECT_CONNECTION);

      RegisterMessageType<MapMessage>(MessageType::COMMAND_LOAD_MAP);
      RegisterMessageType<Message>(MessageType::COMMAND_PAUSE);
      RegisterMessageType<Message>(MessageType::COMMAND_RESUME);
      RegisterMessageType<RestartMessage>(MessageType::COMMAND_RESTART);
      RegisterMessageType<TimeChangeMessage>(MessageType::COMMAND_SET_TIME);

      RegisterMessageType<ServerMessageRejected>(MessageType::SERVER_REQUEST_REJECTED);
      RegisterMessageType<ActorUpdateMessage>(MessageType::INFO_ACTOR_CREATED);
      RegisterMessageType<ActorUpdateMessage>(MessageType::INFO_ACTOR_UPDATED);
      RegisterMessageType<Message>(MessageType::INFO_ACTOR_DELETED);
      RegisterMessageType<Message>(MessageType::INFO_ACTOR_PUBLISHED);
      RegisterMessageType<Message>(MessageType::INFO_ENVIRONMENT_CHANGED);

      RegisterMessageType<Message>(MessageType::INFO_PLAYER_ENTERED_WORLD);

      RegisterMessageType<MapMessage>(MessageType::REQUEST_LOAD_MAP);
      RegisterMessageType<Message>(MessageType::REQUEST_PAUSE);
      RegisterMessageType<Message>(MessageType::REQUEST_RESUME);
      RegisterMessageType<RestartMessage>(MessageType::REQUEST_RESTART);
      RegisterMessageType<TimeChangeMessage>(MessageType::REQUEST_SET_TIME);

      //Logger messages.
      RegisterMessageType<Message>(MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);
      RegisterMessageType<Message>(MessageType::LOG_REQ_CHANGESTATE_RECORD);
      RegisterMessageType<Message>(MessageType::LOG_REQ_CHANGESTATE_IDLE);
      RegisterMessageType<Message>(MessageType::LOG_REQ_GET_KEYFRAMES);
      RegisterMessageType<Message>(MessageType::LOG_REQ_GET_LOGFILES);
      RegisterMessageType<Message>(MessageType::LOG_REQ_GET_TAGS);
      RegisterMessageType<Message>(MessageType::LOG_REQ_GET_STATUS);
      RegisterMessageType<LogCaptureKeyframeMessage>(MessageType::LOG_REQ_CAPTURE_KEYFRAME);
      RegisterMessageType<LogInsertTagMessage>(MessageType::LOG_REQ_INSERT_TAG);
      RegisterMessageType<LogDeleteLogfileMessage>(MessageType::LOG_REQ_DELETE_LOG);
      RegisterMessageType<LogSetLogfileMessage>(MessageType::LOG_REQ_SET_LOGFILE);
      RegisterMessageType<LogSetAutoKeyframeIntervalMessage>
         (MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL);
      RegisterMessageType<LogAvailableLogsMessage>(MessageType::LOG_INFO_LOGFILES);
      RegisterMessageType<LogGetKeyframeListMessage>(MessageType::LOG_INFO_KEYFRAMES);
      RegisterMessageType<LogGetTagListMessage>(MessageType::LOG_INFO_TAGS);
      RegisterMessageType<LogStatusMessage>(MessageType::LOG_INFO_STATUS);
      RegisterMessageType<Message>(MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS);
      RegisterMessageType<LogEndLoadKeyframeMessage>(MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS);
      RegisterMessageType<LogJumpToKeyframeMessage>(MessageType::LOG_REQ_JUMP_TO_KEYFRAME);
      RegisterMessageType<Message>(MessageType::LOG_REQ_ADD_IGNORED_ACTOR);
      RegisterMessageType<Message>(MessageType::LOG_REQ_REMOVE_IGNORED_ACTOR);
      RegisterMessageType<Message>(MessageType::LOG_REQ_CLEAR_IGNORE_LIST);

      //Game Event...
      RegisterMessageType<GameEventMessage>(MessageType::INFO_GAME_EVENT);

      //System Messages
      RegisterMessageType<SystemMessage>(MessageType::SYSTEM_POST_EVENT_TRAVERSAL);
      RegisterMessageType<SystemMessage>(MessageType::SYSTEM_FRAME_SYNCH);
      RegisterMessageType<SystemMessage>(MessageType::SYSTEM_POST_FRAME);
   }

   /////////////////////////////////////////////////////////////////
   MessageFactory::~MessageFactory()
   {

   }

   /////////////////////////////////////////////////////////////////
   bool MessageFactory::IsMessageTypeSupported(const MessageType& msg) const
   {
      return mMessageFactory->IsTypeSupported(&msg);
   }

   /////////////////////////////////////////////////////////////////
   void MessageFactory::GetSupportedMessageTypes(std::vector<const MessageType*>& vec)
   {
      mMessageFactory->GetSupportedTypes(vec);
   }

   /////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Message> MessageFactory::CreateMessage(const MessageType& msgType) const
   {
      dtCore::RefPtr<Message> msg = mMessageFactory->CreateObject(&msgType);

      if (msg == NULL)
      {
         LOGN_ERROR("messagefactory.cpp", "Object factory returned NULL, the message could not be created");
         throw dtGame::MessageFactory::MessageTypeNotRegisteredException(
            std::string("Could not create type ") + msgType.GetName(), __FILE__, __LINE__);
      }
      msg->SetMessageType(msgType);
      msg->SetSource(*mMachine);
      //default the message to be unknown.
      msg->SetDestination(NULL);
      return msg;
   }

   /////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Message> MessageFactory::CloneMessage(const Message& msg) const
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
            + " with type " + msg.GetMessageType().GetName() + ": " + ex.ToString());
         throw ex;
      }
      return theClone;
   }

   /////////////////////////////////////////////////////////////////
   const MessageType& MessageFactory::GetMessageTypeById(unsigned short id) const
   {
      std::map<unsigned short, const MessageType*>::const_iterator itor = mIdMap.find(id);
      if (itor == mIdMap.end())
      {
         std::ostringstream ss;
         ss << "Message ID: " << id << " was not found in the message "
            "type map.";
         throw dtGame::MessageFactory::MessageTypeNotRegisteredException(ss.str(), __FILE__, __LINE__);
      }

      return *itor->second;
   }

   /////////////////////////////////////////////////////////////////
   const MessageType* MessageFactory::GetMessageTypeByName(const std::string& name) const
   {
      for (std::map<unsigned short, const MessageType*>::const_iterator i = mIdMap.begin(); i != mIdMap.end(); ++i)
      {
         if (i->second->GetName() == name)
            return i->second;
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////
   void MessageFactory::ThrowIdException(const MessageType& type) const
   {
      std::ostringstream ss;
      ss << "A MessageType with id " << type.GetId() << " has already been registered.";
      throw dtGame::MessageFactory::MessageTypeAlreadyRegisteredException(
         ss.str(), __FILE__, __LINE__);
   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageFactory::MessageTypeAlreadyRegisteredException::MessageTypeAlreadyRegisteredException(const std::string& message,
                                                                                                const std::string& filename,
                                                                                                unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
      mType = &MessageFactory::MessageFactoryException::TYPE_ALREADY_REGISTERED;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageFactory::MessageTypeNotRegisteredException::MessageTypeNotRegisteredException(const std::string& message,
                                                                                        const std::string& filename,
                                                                                        unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
      mType = &MessageFactory::MessageFactoryException::TYPE_NOT_REGISTERED;
   }
}
