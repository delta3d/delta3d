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
#ifndef DELTA_MESSAGEFACTORY
#define DELTA_MESSAGEFACTORY

#include <osg/Referenced>
#include <vector>
#include <map>
#include <dtCore/refptr.h>
#include <dtUtil/objectfactory.h>
#include <dtUtil/enumeration.h>
#include <dtGame/export.h>
#include <dtGame/message.h>
#include <dtGame/machineinfo.h>

namespace dtGame
{
   class MessageType;

   class DT_GAME_EXPORT MessageFactory
   {
      public:

         class MessageTypeAlreadyRegisteredException : public dtUtil::Exception
         {
         public:
         	MessageTypeAlreadyRegisteredException(const std::string& message, const std::string& filename, unsigned int linenum);
         	virtual ~MessageTypeAlreadyRegisteredException() {};
         };

         class MessageTypeNotRegisteredException : public dtUtil::Exception
         {
         public:
         	MessageTypeNotRegisteredException(const std::string& message, const std::string& filename, unsigned int linenum);
         	virtual ~MessageTypeNotRegisteredException() {};
         };

         /// Constructor
         MessageFactory(const std::string& name, const MachineInfo& machine, const std::string& desc = "");

         /// Destructor
         ~MessageFactory();

         /**
          * Function in which all supported message types will be registered
          * @param The type to register
          * @note The template should be the class the MessageType corresponds to
          */
         template <typename T>
         static void RegisterMessageType(const MessageType& type);

         static void UnregisterMessageType(const MessageType& type);

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
         static bool IsMessageTypeSupported(const MessageType& msg);

         /**
          * Gets a list of supported types
          * @param The vector to fill
          */
         static void GetSupportedMessageTypes(std::vector<const MessageType*>& vec);

         /**
          * Returns a MessageType for the corresponding id.
          * @note this throws an exception rather than returning a NULL pointer when the id is not found because
          *       One should never call this with an invalid id.
          * @return A reference to the MessageType
          * @throws dtUtil::Exception with enum MessageFactoryException::TYPE_NOT_REGISTERED if the type is not found.
          */
         static const MessageType& GetMessageTypeById(unsigned short id);

         /**
          * Returns a MessageType for the corresponding name
          * @return A pointer to the MessageType or NULL if there was no matching message type.
          */
         static const MessageType* GetMessageTypeByName(const std::string& name);

         /**
          * Creates a message from the factory and fills a passed in refptr.
          * This is templated so one can pass in a ref ptr to the actual subclass of
          * dtGame::Message and fill it without having to do a cast outside the method.
          * @param msgType The type of message to generate
          * @param result A RefPtr to fill with the created message.
          * @return A pointer to the message, or NULL if error
          * @throws dtUtil::Exception if the given message type is not registered.
          */
         template <typename MessageClassName>
         void CreateMessage(const MessageType& msgType, dtCore::RefPtr<MessageClassName>& result)
         {
            dtCore::RefPtr<Message> msg = CreateMessage(msgType);
            result = static_cast<MessageClassName*>(msg.get());
         }

         /**
          * Creates a message from the factory.  This only returns a poiner to a Message.
          * One should use the templated version of the method unless there is no need
          * to have a subclass of Message.
          * @param msgType The type of message to generate
          * @return A pointer to the message, or NULL if error
          * @throws dtUtil::Exception if the given message type is not registered.
          */
         dtCore::RefPtr<Message> CreateMessage(const MessageType& msgType) const;

         /**
          * Make a copy of a message.  It makes sure the right type is created and then
          * calls the message to copy the data.
          * @param msg the message to clone
          * @param the clone.
          * @throws dtUtil::Exception if the message cannot be cloned.
          */
         dtCore::RefPtr<Message> CloneMessage(const Message& msg) const;

      private:
         static void ThrowIdException(const MessageType& type);

         std::string mName, mDescription;

         dtCore::RefPtr<const MachineInfo> mMachine;

         static dtCore::RefPtr<dtUtil::ObjectFactory<const MessageType*, Message> > mMessageFactory;

         static std::map<unsigned short, const MessageType*> mIdMap;
         static void InsertMessageTypeInIdMap(const MessageType& type);
   };

   template <typename T>
   void MessageFactory::RegisterMessageType(const MessageType& type)
   {
      if (IsMessageTypeSupported(type))
      {
         ThrowIdException(type);
      }

      mMessageFactory->template RegisterType<T>(&type);

      InsertMessageTypeInIdMap(type);
   }
}

#endif

