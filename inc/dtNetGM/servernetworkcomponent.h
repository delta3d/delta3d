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
 * @author Pjotr van Amerongen
 */
#ifndef DELTA_SERVERNETWORKCOMPONENT
#define DELTA_SERVERNETWORKCOMPONENT

#include <dtGame/message.h>
#include <dtGame/machineinfo.h>

#include <dtNetGM/export.h>
#include <dtNetGM/networkcomponent.h>


namespace dtGame
{
   class Message;
}

namespace dtNetGM
{
   class NetworkComponent;

   class DT_NETGM_EXPORT ServerNetworkComponent : public NetworkComponent
   {
   public:

      /**
       * Construct a ServerNetworkComponent with a game name and version to be used by GNE
       * Calls base class constructor to initialize GNE
       * @param gameName The game name
       * @param gameVersion The game version
       * @param logFile The logfile name
       */
      ServerNetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile = "");

   protected:
      // Destructor
      ~ServerNetworkComponent(void);

   public:
      /**
       * Start a Server
       * @param The protnumber used by the server
       */
      bool SetupServer(int portNum);

      ///callback to signal a connection is successful
      virtual void OnListenSuccess();

      ///callback to signal the connection to the socket failed
      virtual void OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener);

      /**
       * Function called by a NetworkBridge if a connection disconnects
       * @param The NetworkBridge
       */
      virtual void OnDisconnect(NetworkBridge& networkBridge);

      /**
       * Processes a MessageType::NETCLIENT_REQUEST_CONNECTION Message.
       * @param msg The message
       */
      virtual void ProcessNetClientRequestConnection(const MachineInfoMessage& msg);

      /**
       * Processes a MessageType::NETCLIENT_NOTIFY_DISCONNECT Message.
       * @param msg The message
       */
      virtual void ProcessNetClientNotifyDisconnect(const dtGame::Message& msg);

   protected:

      // should we accept new clients
      bool mAcceptClients;

      /**
       * Function to accept or deny a client connection request
       * @param machineInfo The MachineInfo of the new client
       * @param rejectionReason Rejection reason which is send to the client if denied
       * @return boolean indicating if a client should be accepted
       */
      virtual bool AcceptClient(const dtGame::MachineInfo& machineInfo, std::string& rejectionReason);

      /**
       * Sends a INFO_CLIENT_CONNECTED messages to the already connected clients.
       * @param machineInfo The MachineInfo of the new client
       */
      virtual void SendInfoClientConnectedMessage(const dtGame::MachineInfo& machineInfo);

      /**
       * Sends a INFO_CLIENT_CONNECTED messages to the new client, informing the new client of existing connections
       * @param machineInfo The MachineInfo of the new client
       */
      virtual void SendConnectedClientMessage(const dtGame::MachineInfo& machineInfo);
   };
}

#endif // DELTA_SERVERNETWORKCOMPONENT
