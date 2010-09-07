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
#include <prefix/dtgameprefix.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtCore/refptr.h>
#include <sstream>

#include <typeinfo>

namespace dtGame
{
   IMPLEMENT_ENUM(MessageFactory::MessageFactoryException);
   MessageFactory::MessageFactoryException MessageFactory::MessageFactoryException::TYPE_ALREADY_REGISTERED("Type already registered");
   MessageFactory::MessageFactoryException MessageFactory::MessageFactoryException::TYPE_NOT_REGISTERED("Type not registered");

   dtCore::RefPtr<dtUtil::ObjectFactory<const MessageType*, Message> >
      MessageFactory::mMessageFactory(new dtUtil::ObjectFactory<const MessageType*, Message>);

   std::map<unsigned short, const MessageType*> MessageFactory::mIdMap;

   /////////////////////////////////////////////////////////////////
   MessageFactory::MessageFactory(const std::string& name,
                                  const MachineInfo& machine,
                                  const std::string& desc) :
   mName(name),
   mDescription(desc),
   mMachine(&machine)
   {
   }

   /////////////////////////////////////////////////////////////////
   MessageFactory::~MessageFactory()
   {

   }

   /////////////////////////////////////////////////////////////////
   void MessageFactory::UnregisterMessageType(const MessageType& type)
   {
      mMessageFactory->RemoveType(&type);
      std::map<unsigned short, const MessageType*>::iterator i = mIdMap.find(type.GetId());
      if (i != mIdMap.end())
      {
         mIdMap.erase(i);
      }
   }

   /////////////////////////////////////////////////////////////////
   bool MessageFactory::IsMessageTypeSupported(const MessageType& msg)
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
   const MessageType& MessageFactory::GetMessageTypeById(unsigned short id)
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
   const MessageType* MessageFactory::GetMessageTypeByName(const std::string& name)
   {
      for (std::map<unsigned short, const MessageType*>::const_iterator i = mIdMap.begin(); i != mIdMap.end(); ++i)
      {
         if (i->second->GetName() == name)
            return i->second;
      }

      return NULL;
   }

   /////////////////////////////////////////////////////////////////
   void MessageFactory::ThrowIdException(const MessageType& type)
   {
      std::ostringstream ss;
      ss << "A MessageType with id " << type.GetId() << " has already been registered.";
      throw dtGame::MessageFactory::MessageTypeAlreadyRegisteredException(
         ss.str(), __FILE__, __LINE__);
   }

   /////////////////////////////////////////////////////////////////
   void MessageFactory::InsertMessageTypeInIdMap(const MessageType& type)
   {
      mIdMap.insert(std::make_pair(type.GetId(), &type));
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
