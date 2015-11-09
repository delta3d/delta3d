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
 * William E. Johnson II, David Guthrie, Curtiss Murphy
 */

#ifndef DELTA_MESSAGETYPE
#define DELTA_MESSAGETYPE

#include <string>
#include <dtUtil/enumeration.h>
#include <dtUtil/typetraits.h>
#include <dtGame/export.h>
#include <dtGame/messagefactory.h>

namespace dtGame
{
   class MessageFactory;
   /**
    * Class that enumerates the message types used by the GameManager
    * @see class dtGame::GameManager
    *
    * 
    * To add new message types subclass this type using
    * @code
    * DT_DECLARE_MESSAGE_TYPE_CLASS_BEGIN(CustomMessageType, NEW_EXPORT_MACRO)
    *    static const unsigned short START_SHORT_INT_VALUE;
    *
    *    static const CustomMessageType MSG_TYPE_1;
    *    static const CustomMessageType MSG_TYPE_2;
    * DT_DECLARE_MESSAGE_TYPE_CLASS_END()
    * @endcode
    *
    * ..in the cpp file
    * @code
    * DT_IMPLEMENT_MESSAGE_TYPE_CLASS(CustomMessageType);
    * const unsigned short START_SHORT_INT_VALUE = dtGame::MessageType::USER_DEFINED_MESSAGE_TYPE;
    *
    * const CustomMessageType CustomMessageType::MSG_TYPE_1("MSG_TYPE_1", "Info", "Description", START_SHORT_INT_VALUE, DT_MSG_CLASS(Message));
    * const CustomMessageType CustomMessageType::MSG_TYPE_2("MSG_TYPE_2", "Info", "Description", START_SHORT_INT_VALUE + 1US, DT_MSG_CLASS(Message));
    * @endcode
    * Note that the DT_MSG_CLASS() macro takes the subclass of dtGame::Message that is uses for
    * the message.  It will auto register and unregister the message at startup/library load and shutdown/library unload
    * so you no longer need to do that explicitly.
    *
    * @see messagemacros.h for macros to make it easier to make a new message subclass.
    */
   class DT_GAME_EXPORT MessageType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(MessageType);
      public:
         static const std::string CATEGORY_INFO;
         static const std::string CATEGORY_REQUEST;
         static const std::string CATEGORY_COMMAND;
         static const std::string CATEGORY_TICK;

         /// his should never be used.  It is the default when creating a message object.
         static const MessageType UNKNOWN;
       
         //TICK MESSAGES
         static const MessageType TICK_LOCAL;
         static const MessageType TICK_REMOTE;
         static const MessageType TICK_END_OF_FRAME;
         ///Sent when a timer time
         static const MessageType INFO_TIMER_ELAPSED;
         static const MessageType INFO_ACTOR_CREATED;
         static const MessageType INFO_ACTOR_PUBLISHED;
         static const MessageType INFO_ACTOR_DELETED;
         static const MessageType INFO_ACTOR_UPDATED;
         static const MessageType INFO_CLIENT_CONNECTED;
         static const MessageType INFO_MAP_CHANGE_LOAD_END;
         static const MessageType INFO_MAP_CHANGE_UNLOAD_END;
         // renamed to INFO_MAP_CHANGE_LOAD_END
          static const MessageType& INFO_MAP_LOADED;
         // renamed to INFO_MAP_CHANGE_UNLOAD_END
         static const MessageType& INFO_MAP_UNLOADED;

         static const MessageType INFO_MAPS_OPENED;
         static const MessageType INFO_MAPS_CLOSED;


         static const MessageType INFO_PAUSED;
         static const MessageType INFO_RESUMED;
         static const MessageType INFO_RESTARTED;
         static const MessageType INFO_TIME_CHANGED;
         static const MessageType INFO_GAME_EVENT;
         static const MessageType INFO_ENVIRONMENT_CHANGED;

         static const MessageType INFO_MAP_CHANGE_LOAD_BEGIN;
         static const MessageType INFO_MAP_CHANGE_UNLOAD_BEGIN;
         // renamed to INFO_MAP_CHANGE_LOAD_BEGIN
         static const MessageType& INFO_MAP_LOAD_BEGIN;
         // renamed to INFO_MAP_CHANGE_UNLOAD_BEGIN
         static const MessageType& INFO_MAP_UNLOAD_BEGIN;
         static const MessageType INFO_MAP_CHANGE_BEGIN;
         static const MessageType INFO_MAP_CHANGE_END;
         // renamed to INFO_MAP_CHANGE_END
         static const MessageType& INFO_MAP_CHANGED;

         ///Message sent when a player enters the world.  The Actor deleted message can be used when the player leaves.
         static const MessageType INFO_PLAYER_ENTERED_WORLD;

         static const MessageType COMMAND_LOAD_MAP;
         static const MessageType COMMAND_UNLOAD_MAP;
         static const MessageType COMMAND_CHANGE_MAP;
         static const MessageType COMMAND_PAUSE;
         static const MessageType COMMAND_RESUME;
         static const MessageType COMMAND_RESTART;
         static const MessageType COMMAND_SET_TIME;

         static const MessageType REQUEST_LOAD_MAP;
         static const MessageType REQUEST_UNLOAD_MAP;
         static const MessageType REQUEST_CHANGE_MAP;
         static const MessageType REQUEST_PAUSE;
         static const MessageType REQUEST_RESUME;
         static const MessageType REQUEST_RESTART;
         static const MessageType REQUEST_SET_TIME;

         static const MessageType SERVER_REQUEST_REJECTED;

         static const MessageType NETCLIENT_REQUEST_CONNECTION;
         static const MessageType NETCLIENT_NOTIFY_DISCONNECT;
         static const MessageType NETSERVER_ACCEPT_CONNECTION;
         static const MessageType NETSERVER_REJECT_CONNECTION;
         static const MessageType NETSERVER_SYNC_CONTROL;
         static const MessageType NETSERVER_FRAME_SYNC;

         //LOGGER MESSAGES
         static const MessageType LOG_REQ_CHANGESTATE_PLAYBACK;
         static const MessageType LOG_REQ_CHANGESTATE_RECORD;
         static const MessageType LOG_REQ_CHANGESTATE_IDLE;
         static const MessageType LOG_REQ_CAPTURE_KEYFRAME; // data
         static const MessageType LOG_REQ_GET_KEYFRAMES;
         static const MessageType LOG_REQ_GET_LOGFILES;
         static const MessageType LOG_REQ_GET_TAGS;
         static const MessageType LOG_REQ_GET_STATUS;
         static const MessageType LOG_REQ_INSERT_TAG; // DATA
         static const MessageType LOG_REQ_DELETE_LOG; // DATA
         static const MessageType LOG_REQ_SET_LOGFILE; // DATA
         static const MessageType LOG_REQ_SET_AUTOKEYFRAMEINTERVAL; // DATA
         static const MessageType LOG_INFO_KEYFRAMES; // DATA
         static const MessageType LOG_INFO_LOGFILES; // DATA
         static const MessageType LOG_INFO_TAGS; // DATA
         static const MessageType LOG_INFO_STATUS; // DATA
         static const MessageType LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS;
         static const MessageType LOG_COMMAND_END_LOADKEYFRAME_TRANS; // data
         static const MessageType LOG_REQ_JUMP_TO_KEYFRAME; // data
         static const MessageType LOG_REQ_ADD_IGNORED_ACTOR;
         static const MessageType LOG_REQ_REMOVE_IGNORED_ACTOR;
         static const MessageType LOG_REQ_CLEAR_IGNORE_LIST;
         static const MessageType LOG_INFO_PLAYBACK_END_OF_MESSAGES;
         static const MessageType LOG_REQ_ADD_IGNORED_MESSAGETYPE;
         static const MessageType LOG_REQ_REMOVE_IGNORED_MESSAGETYPE;
         static const MessageType LOG_REQ_CLEAR_IGNORED_MESSAGETYPE_LIST;

         //SYSTEM MESSAGES
         static const MessageType SYSTEM_POST_EVENT_TRAVERSAL;
         static const MessageType SYSTEM_FRAME_SYNCH;
         static const MessageType SYSTEM_POST_FRAME;

         // GENERIC GAME SYSTEM MESSAGES
         static const MessageType INFO_GAME_STATE_CHANGED;

         static const MessageType REQUEST_GAME_STATE_TRANSITION;


         /**
          * Gets the category of this message type
          * @return The category
          */
         const std::string& GetCategory() const { return mCategory; }

         /**
          * Gets the description of this message type
          * @return The description
          */
         const std::string& GetDescription() const { return mDescription; }

         /**
          * Gets the id of this message type
          * @return The id
          */
         unsigned short GetId() const { return mId; }

         /**
          * Sets the category of this message type
          * @param The new category
          */
         void SetCategory(const std::string& newCategory) { mCategory = newCategory; }

         /**
          * Sets the description of this message type
          * @param The new description
          */
         void SetDescription(const std::string& newDescription) { mDescription = newDescription; }

      protected:

         /**
          * Message type constructor takes a pointer the class that implements the message.
          * It only takes the extra parameter because you can't explicitly pass template parameters
          * to a templated constructor so pass (MessageClass*)(NULL)
          *
          * Then it will auto register the message with the message factory at creation time.
          */
         template<typename MessageClass>
         MessageType(const std::string& name, const std::string& category,
                     const std::string& description, const unsigned short id, const MessageClass*)
            : dtUtil::Enumeration(name)
            , mCategory(category)
            , mDescription(description)
            , mId(id)
         {
            AddInstance(this);
            dtGame::MessageFactory::RegisterMessageType<MessageClass>(*this);
         }

         /// Destructor
         virtual ~MessageType();

         static const unsigned short USER_DEFINED_MESSAGE_TYPE = 1024;

      private:

         std::string mCategory, mDescription;
         const unsigned short mId;
   };
}

/// @see dtGame::MesssageType
#define DT_DECLARE_MESSAGE_TYPE_CLASS_BEGIN(CLS, EXPORT_MACRO) \
   class EXPORT_MACRO CLS : public dtGame::MessageType \
   { \
   DECLARE_ENUM(CLS)\
   protected:\
      \
      template<typename MessageClass> \
      CLS(const std::string& name, const std::string& category, \
          const std::string& description, const unsigned short id, const MessageClass*) \
      : dtGame::MessageType(name, category, description, id, (const MessageClass*)(NULL))\
      { \
         AddInstance(this);\
      }\
      virtual ~CLS() {} \
   public: \

/// @see dtGame::MesssageType
#define DT_DECLARE_MESSAGE_TYPE_CLASS_END() };

/// @see dtGame::MesssageType
#define DT_IMPLEMENT_MESSAGE_TYPE_CLASS(CLS) \
         IMPLEMENT_ENUM(CLS)

/// @see dtGame::MesssageType
#define DT_MSG_CLASS(MessageClass) (const MessageClass*)(NULL)

#endif
