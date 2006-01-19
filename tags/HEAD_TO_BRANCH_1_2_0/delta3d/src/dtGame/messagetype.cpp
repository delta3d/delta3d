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

#include "dtGame/messagetype.h"

namespace dtGame
{
   IMPLEMENT_ENUM(MessageType);

   const MessageType MessageType::UNKNOWN("Unknown", "Unknown", "This is the default when creating a message with the empty constuctor", 0);
   const MessageType MessageType::TICK_LOCAL("Tick Local", "Tick", "Signals a frame event. Actors are local and should run the next frame of simulation code", 1);
   const MessageType MessageType::TICK_REMOTE("Tick Remote", "Tick", "Signals a frame event. Actors are remote and should run the next frame of simulation code", 2);

   const MessageType MessageType::INFO_TIMER_ELAPSED("Timer Elapsed", "Info", "Sent when a timer associated with the Game Manager has fired", 10);
   const MessageType MessageType::INFO_ACTOR_CREATED("Actor Create", "Info", "Sent when a new actor has been created and published", 11);
   const MessageType MessageType::INFO_ACTOR_PUBLISHED("Actor Published", "Info", "Sent by the Game Manager locally when an actor is published", 12);
   const MessageType MessageType::INFO_ACTOR_DELETED("Actor Delete", "Info", "Sent when an actor has been removed from the simulation both locally and remotely", 13);
   const MessageType MessageType::INFO_ACTOR_UPDATED("Actor Updated", "Info", "Sent when an actor's state changes enough to inform the game world", 14);
   const MessageType MessageType::INFO_CLIENT_CONNECTED("Client Connected", "Info", "Sent to other clients when a new client has connected", 15);
   const MessageType MessageType::INFO_MAP_LOADED("Map Loaded", "Info", "Sent after a new map has been loaded successfully", 16);
   const MessageType MessageType::INFO_PAUSED("Paused", "Info", "Sent when a game manager pauses the game or simulation", 17);
   const MessageType MessageType::INFO_RESUMED("Resumed", "Info", "Sent when a game manager resumes from a paused state", 18);
   const MessageType MessageType::INFO_RESTARTED("Restarted", "Info", "Sent when a game manager restarts the game or simulation", 19);
   const MessageType MessageType::INFO_TIME_CHANGED("Simulation Time Changed", "Info", "Sent when a game manager changes the simulation time.", 20);
   
   const MessageType MessageType::COMMAND_LOAD_MAP("Load Map", "Command", "Tells the recipient to load a new map", 50);
   const MessageType MessageType::COMMAND_PAUSE("Pause", "Command", "Tells the recipient to pause the game or simulation", 51);
   const MessageType MessageType::COMMAND_RESUME("Resume", "Command", "Tells the recipient to resume from a paused state", 52);
   const MessageType MessageType::COMMAND_RESTART("Restart", "Command", "Tells the recipient to restart the game or simulation", 53);
   const MessageType MessageType::COMMAND_SET_TIME("Set Time", "Command", "Tells the recipient to change the simulation time", 54);

   const MessageType MessageType::REQUEST_LOAD_MAP("Request Load Map", "Request", "A client request that a map be loaded", 80);
   const MessageType MessageType::REQUEST_PAUSE("Pause", "Request", "A client request that asks the recipient to pause the game or simulation", 81);
   const MessageType MessageType::REQUEST_RESUME("Resume", "Request", "A client request that asks the recipient to resume from a paused state", 82);
   const MessageType MessageType::REQUEST_RESTART("Restart", "Request", "A client request that asks the recipient to restart the game or simulation", 83);
   const MessageType MessageType::REQUEST_SET_TIME("Set Time", "Request", "A client request that asks the recipient to change the simulation time", 84);

   const MessageType MessageType::SERVER_REQUEST_REJECTED("Message Rejected", "Server", "A server message sent to a client that a message it sent was rejected as invalid", 110);
   const MessageType MessageType::NETCLIENT_REQUEST_CONNECTION("Client Request Connection", "Client", "Sent when a client wants to connect to the server", 150);
   const MessageType MessageType::NETCLIENT_NOTIFY_DISCONNECT("Client Disconnecting", "Client", "Sent when a client wishes to disconnect from the server", 151);
   const MessageType MessageType::NETSERVER_ACCEPT_CONNECTION("Accept Client", "Server", "Sent from a server to a client informing the client that it is ok to connect", 152);
   const MessageType MessageType::NETSERVER_REJECT_CONNECTION("Reject Client", "Server", "Sent from the server to the client if a client is not allowed to connect to the server", 153);
   
    // Logger messages
   const MessageType MessageType::LOG_REQ_CHANGESTATE_PLAYBACK("Logger - Change State to Playback", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to change its state to PLAYBACK.  Only works from Idle.", 200);
   const MessageType MessageType::LOG_REQ_CHANGESTATE_RECORD("Logger - Change State to Record", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to change its state to RECORD.  Only works from Idle.", 201);
   const MessageType MessageType::LOG_REQ_CHANGESTATE_IDLE("Logger - Change State to Idle", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to change its state to IDLE.  Only works from Record or Playback.", 202);
   const MessageType MessageType::LOG_REQ_CAPTURE_KEYFRAME("Logger - Capture New Keyframe", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to capture a new keyframe in the current log.  Only works during Record.", 203);
   const MessageType MessageType::LOG_REQ_GET_KEYFRAMES("Logger - Get List of Keyframes", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to generate a new Keyframe info message.  Only from Record or Playback.", 204);
   const MessageType MessageType::LOG_REQ_GET_LOGFILES("Logger - Get List of Log Files", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to generate a new Logfiles info message.  Anytime.", 205);
   const MessageType MessageType::LOG_REQ_GET_TAGS("Logger - Get List of Tags", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to generate a new Tag info message.  Only in Record or Playback.", 206);
   const MessageType MessageType::LOG_REQ_GET_STATUS("Logger - Get Status", 
      "Request", "Sent by the logger controller component (often a client) to request the " 
         " logger server component to generate a new Status info message.  Anytime.", 207);
   const MessageType MessageType::LOG_REQ_INSERT_TAG("Logger - Add Tag", "Request", 
       "Sent by the logger controller component (often a client) to request the logger server " 
         " component to add a new tag.  Only in Record or Playback.", 208); 
   const MessageType MessageType::LOG_REQ_DELETE_LOG("Logger - Delete Log", "Request", 
       "Sent by the logger controller component (often a client) to request the logger server " 
         " component to delete a specific log file.  Only in Idle.", 209);
   const MessageType MessageType::LOG_REQ_SET_LOGFILE("Logger - Set Current Logfile", "Request", 
       "Sent by the logger controller component (often a client) to request the logger server " 
         " component to change the log file name in prep for a record or playback.  Only in Idle.", 210);
   const MessageType MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL("Logger - Set Auto Keyframe Interval", 
      "Request", "Sent by the logger controller component (often a client) to request the logger server " 
         " component to change the automatic keyframe interval (double, in seconds). Only in Record.", 211);
   const MessageType MessageType::LOG_INFO_KEYFRAMES("Logger - Info - Keyframes", "Info", 
       "Info message sent by the logger server controller with all the current known keyframes. " 
         " May have been requested or not.  Only in Record or Playback.", 212);
   const MessageType MessageType::LOG_INFO_LOGFILES("Logger - Info - Logfiles", "Info", 
       "Info message sent by the logger server controller with a list of the known logfiles. " 
         " May have been requested or not.  Anytime.", 213);
   const MessageType MessageType::LOG_INFO_TAGS("Logger - Info - Tags", "Info", 
       "Info message sent by the logger server controller with all the tags in the current logfile. " 
         " May have been requested or not.  Only in Record or Playback.", 214);
   const MessageType MessageType::LOG_INFO_STATUS("Logger - Info - Status", "Info", 
       "Info message sent by the logger server controller with the current status of "
       " the server logger.  Sent whenever state changes - any state.", 215);
   const MessageType MessageType::LOG_COMMAND_BEGIN_LOADKEYFRAME_TRANS("Logger - Server Command - Begin Load Keyframe Transaction", "Command", 
       "Command message sent by the logger server controller to let clients know that a keyframe "
       "is about to be loaded from the current playback file.  This usually means a massive "
       "amount of messages are about to happen. When the server loads the keyframe, it "
       "can change maps, change system time, add,delete, and update actors. "
       "Can only be sent during playback.", 216);
   const MessageType MessageType::LOG_COMMAND_END_LOADKEYFRAME_TRANS("Logger - Server Command - End Load Keyframe Transaction", "Command", 
       "Command message sent by the logger server controller to let clients know that the keyframe "
       "load transaction is complete.  This message has the result status in it. "
       "Can only be sent during playback.", 217);

}
