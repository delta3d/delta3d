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
#ifndef DELTA_MESSAGEFACTORY
#define DELTA_MESSAGEFACTORY

#include <osg/Referenced>
#include <vector>
#include <map>
#include <dtCore/refptr.h>
#include <sstream>
#include <dtUtil/objectfactory.h>
#include <dtUtil/enumeration.h>
#include "dtGame/export.h"
#include "dtGame/messagetype.h"
#include "dtGame/message.h"
#include "dtGame/machineinfo.h"

namespace dtGame
{
   class DT_GAME_EXPORT MessageFactory
   {
      public:

         class DT_GAME_EXPORT MessageFactoryException : public dtUtil::Enumeration
         {
            DECLARE_ENUM(MessageFactoryException);
            public:
               static MessageFactoryException TYPE_ALREADY_REGISTERED;
               static MessageFactoryException TYPE_NOT_REGISTERED;
            private:
               MessageFactoryException(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
               virtual ~MessageFactoryException() {}
         };

         /// Constructor
         MessageFactory(const std::string &name, const MachineInfo& machine, const std::string &desc = "");

         /// Destructor
         ~MessageFactory();
         
         /**
          * Function in which all supported message types will be registered
          * @param The type to register
          * @note The template should be the class the MessageType corresponds to
          */
         template <typename T>
         void RegisterMessageType(const MessageType &type);

         /**
          * Gets the name of the factory
          * @return The name
          */
         const std::string& GetName() const { return mName; }

         /**
          * Gets the description of the factory
          * @return The description
          */
         const std::string& GetDescription() const { return mDescription; }

         /**
          * Gets the name of the factory
          * @return The name
          */
         const MachineInfo& GetMachineInfo() const { return *mMachine; }

         /**
          * Tells is a message type is supported
          * @return True if it is, false if not
          */
         bool IsMessageTypeSupported(const MessageType &msg) const throw();

         /**
          * Gets a list of supported types
          * @param The vector to fill
          */
         void GetSupportedMessageTypes(std::vector<const MessageType*> &vec) throw();

         /**
          * Returns a MessageType for the corresponding id.
          * @note this throws an exception rather than returning a NULL pointer when the id is not found because 
          *       One should never call this with an invalid id.
          * @return A reference to the MessageType
          * @throws dtUtil::Exception with enum MessageFactoryException::TYPE_NOT_REGISTERED if the type is not found.
          */
         const MessageType &GetMessageTypeById(unsigned short id) const throw(dtUtil::Exception);

         /**
          * Returns a MessageType for the corresponding name
          * @return A pointer to the MessageType or NULL if there was no matching message type.
          */
         const MessageType* GetMessageTypeByName(const std::string& name) const throw();

         /**
          * Creates a message from the factory
          * @param msgType The type of message to generate
          * @return A pointer to the message, or NULL if error
          * @throws dtUtil::Exception if the given message type is not registered.
          */
         dtCore::RefPtr<Message> CreateMessage(const MessageType &msgType) throw(dtUtil::Exception);

         /**
          * Make a copy of a message.  It makes sure the right type is created and then
          * calls the message to copy the data.
          * @param msg the message to clone
          * @param the clone.
          * @throws dtUtil::Exception if the message cannot be cloned.
          */
         dtCore::RefPtr<Message> CloneMessage(const Message& msg) throw(dtUtil::Exception);
         
      private:

         std::string mName, mDescription;

         dtCore::RefPtr<const MachineInfo> mMachine;

         dtCore::RefPtr<dtUtil::ObjectFactory<const MessageType*, Message> > mMessageFactory;

         std::map<unsigned short, const MessageType*> mIdMap;
   };

   template <typename T>
   void MessageFactory::RegisterMessageType(const MessageType &type)
   {
      if (mIdMap.find(type.GetId()) != mIdMap.end())
      {
         std::ostringstream ss;
         ss << "A MessageType with id " << type.GetId() << " has already been registered.";
         EXCEPT(MessageFactory::MessageFactoryException::TYPE_ALREADY_REGISTERED, ss.str());
      }
      
      mMessageFactory->template RegisterType<T>(&type);
      
      mIdMap.insert(std::make_pair(type.GetId(), &type));
   }
}

#endif

