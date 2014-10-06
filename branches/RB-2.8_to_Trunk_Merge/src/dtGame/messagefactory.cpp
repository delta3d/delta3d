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
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/gamestatemessages.h>
#include <dtGame/loggermessages.h>
#include <dtGame/message.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtCore/refptr.h>
#include <sstream>

#include <typeinfo>

namespace dtGame
{
   dtCore::RefPtr<dtUtil::ObjectFactory<const MessageType*, Message> >
      MessageFactory::mMessageFactory (new dtUtil::ObjectFactory<const MessageType*, Message>);

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
   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageFactory::MessageTypeNotRegisteredException::MessageTypeNotRegisteredException(const std::string& message,
                                                                                        const std::string& filename,
                                                                                        unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   IMPLEMENT_ENUM(MessageType);

   const MessageType MessageType::UNKNOWN("Unknown", "Unknown", "This is the default when creating a message with the empty constuctor", 0, (Message*)(NULL));
   const MessageType MessageType::TICK_LOCAL("Tick Local", "Tick", "Signals a frame event. Actors are local and should run the next frame of simulation code", 1, (TickMessage*)(NULL));
   const MessageType MessageType::TICK_REMOTE("Tick Remote", "Tick", "Signals a frame event. Actors are remote and should run the next frame of simulation code", 2, (TickMessage*)(NULL));
   const MessageType MessageType::TICK_END_OF_FRAME("Tick End of Preframe", "Tick", "Signals the end of simulation step.  It is only sent to components.", 3, (TickMessage*)(NULL));

   const MessageType MessageType::INFO_TIMER_ELAPSED("Timer Elapsed", "Info", "Sent when a timer associated with the Game Manager has fired", 10, (TimerElapsedMessage*)(NULL));
   const MessageType MessageType::INFO_ACTOR_CREATED("Actor Create", "Info", "Sent when a new actor has been created and published", 11, (ActorUpdateMessage*)(NULL));
   const MessageType MessageType::INFO_ACTOR_PUBLISHED("Actor Published", "Info", "Sent by the Game Manager locally when an actor is published", 12, (Message*)(NULL));
   const MessageType MessageType::INFO_ACTOR_DELETED("Actor Delete", "Info", "Sent when an actor has been removed from the simulation both locally and remotely", 13, (Message*)(NULL));
   const MessageType MessageType::INFO_ACTOR_UPDATED("Actor Updated", "Info", "Sent when an actor's state changes enough to inform the game world", 14, (ActorUpdateMessage*)(NULL));
   const MessageType MessageType::INFO_CLIENT_CONNECTED("Client Connected", "Info", "Sent to other clients when a new client has connected", 15, (MachineInfoMessage*)(NULL));
   const MessageType MessageType::INFO_MAP_CHANGE_LOAD_END("Map Loaded", "Info", "Sent after a new map has been loaded successfully", 16, (MapMessage*)(NULL));
   const MessageType MessageType::INFO_MAP_CHANGE_UNLOAD_END("Map UnLoaded", "Info", "Sent after an old map has been unloaded successfully", 31, (MapMessage*)(NULL));

   // TODO check message IDs, not to overlap with anything else in Delta
   const MessageType MessageType::INFO_MAPS_OPENED("Maps Opened", "Info", "Sent after a (set of) new map(s) has been loaded", 32, (MapMessage*)(NULL));
   const MessageType MessageType::INFO_MAPS_CLOSED("Maps Closed", "Info", "Sent after a (set of) old map(s) has been unloaded", 33, (MapMessage*)(NULL));

   const MessageType MessageType::INFO_PAUSED("Paused", "Info", "Sent when a game manager pauses the game or simulation", 17, (Message*)(NULL));
   const MessageType MessageType::INFO_RESUMED("Resumed", "Info", "Sent when a game manager resumes from a paused state", 18, (Message*)(NULL));
   const MessageType MessageType::INFO_RESTARTED("Restarted", "Info", "Sent when a game manager restarts the game or simulation", 19, (RestartMessage*)(NULL));
   const MessageType MessageType::INFO_TIME_CHANGED("Simulation Time Changed", "Info", "Sent when a game manager changes the simulation time.", 20, (TimeChangeMessage*)(NULL));
   const MessageType MessageType::INFO_GAME_EVENT("Game Event","Info","Sent when a game event is fired.",21, (GameEventMessage*)(NULL));
   const MessageType MessageType::INFO_ENVIRONMENT_CHANGED("Environment Changed", "Info", "Sent when an environment actor is set on the game manager", 22, (Message*)(NULL));
   const MessageType MessageType::INFO_MAP_CHANGE_LOAD_BEGIN("Map Load Began", "Info", "Sent when loading a map has begun.", 23, (MapMessage*)(NULL));
   const MessageType MessageType::INFO_MAP_CHANGE_UNLOAD_BEGIN("Map Unload Began", "Info", "Sent when unloading a map has begun.", 24, (MapMessage*)(NULL));
   const MessageType MessageType::INFO_MAP_CHANGE_BEGIN("Map Change Began", "Info", "Sent when the program has begun to unload a map and load a new one.  Unload and load messages will be sent", 25, (MapMessage*)(NULL));
   const MessageType MessageType::INFO_MAP_CHANGE_END("Map Changed", "Info", "Sent when the program has completed unloading and loading a new map.", 26, (MapMessage*)(NULL));

   ////////////////////
   // Deprecated
   const MessageType& MessageType::INFO_MAP_LOAD_BEGIN(INFO_MAP_CHANGE_LOAD_BEGIN);
   const MessageType& MessageType::INFO_MAP_UNLOAD_BEGIN(INFO_MAP_CHANGE_UNLOAD_BEGIN);
   const MessageType& MessageType::INFO_MAP_LOADED(INFO_MAP_CHANGE_LOAD_END);
   const MessageType& MessageType::INFO_MAP_UNLOADED(INFO_MAP_CHANGE_UNLOAD_END);
   const MessageType& MessageType::INFO_MAP_CHANGED(INFO_MAP_CHANGE_END);
   ////////////////////

   const MessageType MessageType::INFO_PLAYER_ENTERED_WORLD("Player entered world", "Info", "Sent when the player of a game enters the world.", 30, (Message*)(NULL));

   const MessageType MessageType::COMMAND_LOAD_MAP("Load Map", "Command", "Tells the recipient to load a new map", 50, (MapMessage*)(NULL));
   const MessageType MessageType::COMMAND_PAUSE("Pause", "Command", "Tells the recipient to pause the game or simulation", 51, (Message*)(NULL));
   const MessageType MessageType::COMMAND_RESUME("Resume", "Command", "Tells the recipient to resume from a paused state", 52, (Message*)(NULL));
   const MessageType MessageType::COMMAND_RESTART("Restart", "Command", "Tells the recipient to restart the game or simulation", 53, (RestartMessage*)(NULL));
   const MessageType MessageType::COMMAND_SET_TIME("Set Time", "Command", "Tells the recipient to change the simulation time", 54, (TimeChangeMessage*)(NULL));
   const MessageType MessageType::COMMAND_UNLOAD_MAP("Unload Map", "Command", "Tells the recipient to unload a loaded map", 55, (MapMessage*)(NULL));
   const MessageType MessageType::COMMAND_CHANGE_MAP("Change Map", "Command", "Tells the recipient to change the map set", 56, (MapMessage*)(NULL));

   const MessageType MessageType::REQUEST_LOAD_MAP("Request Load Map", "Request", "A client request to load a new map", 80, (MapMessage*)(NULL));
   const MessageType MessageType::REQUEST_PAUSE("Pause", "Request", "A client request that asks the recipient to pause the game or simulation", 81, (Message*)(NULL));
   const MessageType MessageType::REQUEST_RESUME("Resume", "Request", "A client request that asks the recipient to resume from a paused state", 82, (Message*)(NULL));
   const MessageType MessageType::REQUEST_RESTART("Restart", "Request", "A client request that asks the recipient to restart the game or simulation", 83, (RestartMessage*)(NULL));
   const MessageType MessageType::REQUEST_SET_TIME("Set Time", "Request", "A client request that asks the recipient to change the simulation time", 84, (TimeChangeMessage*)(NULL));
   const MessageType MessageType::REQUEST_UNLOAD_MAP("Request Unload Map", "Request", "A client request to unload a loaded map", 85, (MapMessage*)(NULL));
   const MessageType MessageType::REQUEST_CHANGE_MAP("Request Change Map", "Request", "A client request to change the map set", 86, (MapMessage*)(NULL));

   const MessageType MessageType::SERVER_REQUEST_REJECTED("Message Rejected", "Server", "A server message sent to a client that a message it sent was rejected as invalid", 110, (ServerMessageRejected*)(NULL));
   const MessageType MessageType::NETCLIENT_REQUEST_CONNECTION("Client Request Connection", "Client", "Sent when a client wants to connect to the server", 150, (MachineInfoMessage*)(NULL));
   const MessageType MessageType::NETCLIENT_NOTIFY_DISCONNECT("Client Disconnecting", "Client", "Sent when a client wishes to disconnect from the server", 151, (MachineInfoMessage*)(NULL));
   const MessageType MessageType::NETSERVER_ACCEPT_CONNECTION("Accept Client", "Server", "Sent from a server to a client informing the client that it is ok to connect", 152, (MachineInfoMessage*)(NULL));
   const MessageType MessageType::NETSERVER_REJECT_CONNECTION("Reject Client", "Server", "Sent from the server to the client if a client is not allowed to connect to the server", 153, (NetServerRejectMessage*)(NULL));
   const MessageType MessageType::NETSERVER_SYNC_CONTROL("Server Sync Control", "Server",
      "Sent from the server to tell the client about the frame sync mechanism.", 154, (ServerSyncControlMessage*)(NULL));
   const MessageType MessageType::NETSERVER_FRAME_SYNC("Server Frame Sync", "Server",
      "Sent from the server, every frame, to give clients a chance to sync up.", 155, (ServerFrameSyncMessage*)(NULL));

    // Logger messages
   const MessageType MessageType::LOG_REQ_CHANGESTATE_PLAYBACK("Logger - Change State to Playback",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to change its state to PLAYBACK.  Only works from Idle.", 200, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_CHANGESTATE_RECORD("Logger - Change State to Record",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to change its state to RECORD.  Only works from Idle.", 201, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_CHANGESTATE_IDLE("Logger - Change State to Idle",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to change its state to IDLE.  Only works from Record or Playback.", 202, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_CAPTURE_KEYFRAME("Logger - Capture New Keyframe",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to capture a new keyframe in the current log.  Only works during Record.", 203, (LogCaptureKeyframeMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_GET_KEYFRAMES("Logger - Get List of Keyframes",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to generate a new Keyframe info message.  Only from Record or Playback.", 204, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_GET_LOGFILES("Logger - Get List of Log Files",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to generate a new Logfiles info message.  Anytime.", 205, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_GET_TAGS("Logger - Get List of Tags",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to generate a new Tag info message.  Only in Record or Playback.", 206, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_GET_STATUS("Logger - Get Status",
      "Request", "Sent by the logger controller component (often a client) to request the "
         " logger server component to generate a new Status info message.  Anytime.", 207, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_INSERT_TAG("Logger - Add Tag", "Request",
       "Sent by the logger controller component (often a client) to request the logger server "
         " component to add a new tag.  Only in Record or Playback.", 208, (LogInsertTagMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_DELETE_LOG("Logger - Delete Log", "Request",
       "Sent by the logger controller component (often a client) to request the logger server "
         " component to delete a specific log file.  Only in Idle.", 209, (LogDeleteLogfileMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_SET_LOGFILE("Logger - Set Current Logfile", "Request",
       "Sent by the logger controller component (often a client) to request the logger server "
         " component to change the log file name in prep for a record or playback.  Only in Idle.", 210, (LogSetLogfileMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL("Logger - Set Auto Keyframe Interval",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
         " component to change the automatic keyframe interval (double, in seconds). Only in Record.", 211, (LogSetAutoKeyframeIntervalMessage*)(NULL));
   const MessageType MessageType::LOG_INFO_KEYFRAMES("Logger - Info - Keyframes", "Info",
       "Info message sent by the logger server controller with all the current known keyframes. "
         " May have been requested or not.  Only in Record or Playback.", 212, (LogGetKeyframeListMessage*)(NULL));
   const MessageType MessageType::LOG_INFO_LOGFILES("Logger - Info - Logfiles", "Info",
       "Info message sent by the logger server controller with a list of the known logfiles. "
         " May have been requested or not.  Anytime.", 213, (LogAvailableLogsMessage*)(NULL));
   const MessageType MessageType::LOG_INFO_TAGS("Logger - Info - Tags", "Info",
       "Info message sent by the logger server controller with all the tags in the current logfile. "
         " May have been requested or not.  Only in Record or Playback.", 214, (LogGetTagListMessage*)(NULL));
   const MessageType MessageType::LOG_INFO_STATUS("Logger - Info - Status", "Info",
       "Info message sent by the logger server controller with the current status of "
       " the server logger.  Sent whenever state changes - any state.", 215, (LogStatusMessage*)(NULL));
   const MessageType MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS("Logger - Server Command - Begin Load Keyframe Transaction", "Command",
       "Command message sent by the logger server controller to let clients know that a keyframe "
       "is about to be loaded from the current playback file.  This usually means a massive "
       "amount of messages are about to happen. When the server loads the keyframe, it "
       "can change maps, change system time, add,delete, and update actors. "
       "Can only be sent during playback.", 216, (Message*)(NULL));
   const MessageType MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS("Logger - Server Command - End Load Keyframe Transaction", "Command",
       "Command message sent by the logger server controller to let clients know that the keyframe "
       "load transaction is complete.  This message has the result status in it. "
       "Can only be sent during playback.", 217, (LogEndLoadKeyframeMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_JUMP_TO_KEYFRAME("Logger - Jump To Keyframe",
      "Request", "Sent by the logger controller component (often a client) to request the "
      " logger server component to jump to a specific keyframe in the current log.  Only works during Playback.", 218, (LogJumpToKeyframeMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_ADD_IGNORED_ACTOR("Logger - Add Actor to Ignore List",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
      " component to add an actor to the recording ignore list", 219, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_REMOVE_IGNORED_ACTOR("Logger - Remove Actor from Ignore List",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
      " component to remove an actor from the recording ignore list", 220, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_CLEAR_IGNORE_LIST("Logger - Clear Ignore List",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
      " component to remove all actors from the recording ignore list", 221, (Message*)(NULL));
   const MessageType MessageType::LOG_INFO_PLAYBACK_END_OF_MESSAGES("Logger - Info - Playback End Of Messages", "Info",
      "Info message sent by the logger server controller when playback has run out of messages in the current "
      "message stream.", 222, (Message*)(NULL));
   const MessageType MessageType::LOG_REQ_ADD_IGNORED_MESSAGETYPE("Logger - Add MessageType to Ignore List",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
      " component to add an MessageType to the recording ignore list", 223, (LogIgnoreMessageTypeMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_REMOVE_IGNORED_MESSAGETYPE("Logger - Remove MessageType from Ignore List",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
      " component to remove an MessageType from the recording ignore list", 224, (LogIgnoreMessageTypeMessage*)(NULL));
   const MessageType MessageType::LOG_REQ_CLEAR_IGNORED_MESSAGETYPE_LIST("Logger - Clear Ignore MessageType List",
      "Request", "Sent by the logger controller component (often a client) to request the logger server "
      " component to remove all MessageTypes from the recording ignore list", 225, (Message*)(NULL));

   const MessageType MessageType::SYSTEM_POST_EVENT_TRAVERSAL("Post Event Traversal", "System",
      "Sent on the system message posteventtraversal.  Use this message to respond to input or window events.", 300, (SystemMessage*)(NULL));
   
   const MessageType MessageType::SYSTEM_FRAME_SYNCH("Frame Synch", "System",
      "Sent on the system message framesynch.  Use this message to do anything dependent on per frame camera positions or orientations.", 301, (SystemMessage*)(NULL));

   const MessageType MessageType::SYSTEM_POST_FRAME("Post Frame", "System",
      "Sent on the system message postframe.  Use this to do thing after the rendering, but before the next frame.", 302, (SystemMessage*)(NULL));
   
   
   const MessageType MessageType::INFO_GAME_STATE_CHANGED("Info Game State Changed", "Info",
      "Sent when the game state changes.", 303, (GameStateChangedMessage*)(NULL));

   const MessageType MessageType::REQUEST_GAME_STATE_TRANSITION("Request Game State Transition", "Request",
      "Sent when the game state is requested to change.", 304, (GameStateTransitionRequestMessage*)(NULL));

}

