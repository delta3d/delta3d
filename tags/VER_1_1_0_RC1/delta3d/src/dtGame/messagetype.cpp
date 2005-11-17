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
   const MessageType MessageType::INFO_TIMER_ELAPSED("Timer Elapsed", "Info", "Sent when a timer associated with the Game Manager has fired", 3);
   const MessageType MessageType::INFO_ACTOR_CREATED("Actor Create", "Info", "Sent when a new actor has been created and published", 4);
   const MessageType MessageType::INFO_ACTOR_PUBLISHED("Actor Published", "Info", "Sent by the Game Manager locally when an actor is published", 5);
   const MessageType MessageType::INFO_ACTOR_DELETED("Actor Delete", "Info", "Sent when an actor has been removed from the simulation both locally and remotely", 6);
   const MessageType MessageType::INFO_ACTOR_UPDATED("Actor Updated", "Info", "Sent when an actor�s state changes enough to inform the game world", 7);
   const MessageType MessageType::INFO_CLIENT_CONNECTED("Client Connected", "Info", "Sent to other clients when a new client has connected", 8);
   const MessageType MessageType::INFO_MAP_LOADED("Map Loaded", "Info", "Sent after a new map has been loaded successfully", 9);
   const MessageType MessageType::NETCLIENT_REQUEST_CONNECTION("Client Request Connection", "Client", "Sent when a client wants to connect to the server", 10);
   const MessageType MessageType::NETCLIENT_NOTIFY_DISCONNECT("Client Disconnecting", "Client", "Sent when a client wishes to disconnect from the server", 11);
   const MessageType MessageType::NETSERVER_ACCEPT_CONNECTION("Accept Client", "Server", "Sent from a server to a client informing the client that it is ok to connect", 12);
   const MessageType MessageType::NETSERVER_REJECT_CONNECTION("Reject Client", "Server", "Sent from the server to the client if a client is not allowed to connect to the server", 13);
   const MessageType MessageType::COMMAND_LOAD_MAP("Load Map", "Command", "Tells the recipient to load a new map", 14);
   const MessageType MessageType::COMMAND_PAUSE("Pause", "Command", "Tells the recipient to pause the game or simulation", 15);
   const MessageType MessageType::COMMAND_RESUME("Resume", "Command", "Tells the recipient to resume from a paused state", 16);
   const MessageType MessageType::COMMAND_RESTART("Restart", "Command", "Tells the recipient to restart the game or simulation", 17);
   const MessageType MessageType::REQUEST_LOAD_MAP("Request Load Map", "Request", "A client request that a map be loaded", 18);
   const MessageType MessageType::SERVER_REQUEST_REJECTED("Message Rejected", "Server", "A server message sent to a client that a message it sent was rejected as invalid", 19);
}
