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
#ifndef DELTA_MESSAGETYPE
#define DELTA_MESSAGETYPE

#include "dtGame/message.h"
#include "dtGame/messageparameter.h"

namespace dtGame
{
   /**
    * Class that enumerates the message types used by the GameManager
    * @see class dtGame::GameManager
    */
   class DT_GAMEMANAGER_EXPORT MessageType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(MessageType);
      public:
         /// his should never be used.  It is the default when creating a message object.
         static const MessageType UNKNOWN;
         static const MessageType TICK_LOCAL;
         static const MessageType TICK_REMOTE;
         static const MessageType INFO_TIMER_ELAPSED;
         static const MessageType INFO_ACTOR_CREATED;
         static const MessageType INFO_ACTOR_PUBLISHED;
         static const MessageType INFO_ACTOR_DELETED;
         static const MessageType INFO_ACTOR_UPDATED;
         static const MessageType INFO_CLIENT_CONNECTED;
         static const MessageType INFO_MAP_LOADED;
         static const MessageType NETCLIENT_REQUEST_CONNECTION;
         static const MessageType NETCLIENT_NOTIFY_DISCONNECT;
         static const MessageType NETSERVER_ACCEPT_CONNECTION;
         static const MessageType NETSERVER_REJECT_CONNECTION;
         static const MessageType COMMAND_LOAD_MAP;
         static const MessageType COMMAND_PAUSE;
         static const MessageType COMMAND_RESUME;
         static const MessageType COMMAND_RESTART;
         static const MessageType REQUEST_LOAD_MAP;
         static const MessageType SERVER_REQUEST_REJECTED;

         /**
          * Gets the category of this message type
          * @return The category
          */
         inline std::string& GetCategory() { return category; }

         /**
          * Gets the description of this message type
          * @return The description
          */
         inline std::string& GetDescription() { return description; }

         /**
          * Gets the id of this message type
          * @return The id
          */
         inline const unsigned short GetId() const { return mId; }

         /**
          * Sets the category of this message type
          * @param The new category
          */
         void SetCategory(const std::string &newCategory) { category = newCategory; }

         /**
          * Sets the description of this message type
          * @param The new description
          */
         void SetDescription(const std::string &newDescription) { description = newDescription; }

      protected:

         /// Constructor
         MessageType(const std::string &name, const std::string &category = "", 
                     const std::string description = "", const unsigned short id = 0) : 
                     dtUtil::Enumeration(name), category(category), description(description), mId(id) 
         {
            AddInstance(this);
         }

         /// Destructor
         virtual ~MessageType() { } 

         std::string category, description;
         const unsigned short mId;
         static const unsigned short USER_DEFINED_MESSAGE_TYPE = 1024;
   };
}

#endif
