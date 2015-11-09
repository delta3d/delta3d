/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005-2010, Alion Science and Technology
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
 * Pjotr van Amerongen, Curtiss Murphy
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
      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const dtUtil::RefString DEFAULT_NAME;
      // Frame Sync config properties. To override the config or default values, call the Sets AFTER adding to the GM
      static const dtUtil::RefString CONFIG_PROP_FRAMESYNC_ISENABLED;
      static const dtUtil::RefString CONFIG_PROP_FRAMESYNC_NUMPERSECOND;
      static const dtUtil::RefString CONFIG_PROP_FRAMESYNC_MAXWAITTIME;

      typedef NetworkComponent BaseClass;


      /**
       * Main constructor.  The optional parameter is for subclasses to make a new type.
       */
      ServerNetworkComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * Construct a ServerNetworkComponent with a game name and version to be used by GNE
       * Calls base class constructor to initialize GNE
       * @note calling the other constructor is preferred.
       * @param gameName The game name
       * @param gameVersion The game version
       * @param logFile The logfile name
       */
      ServerNetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile = "");

      /** 
       * Sends out a sync control message to all clients. Sent when control status changes or when a new client connects
       */
      void SendFrameSyncControlMessage();

   protected:
      // Destructor
      ~ServerNetworkComponent(void);

      /// When the tick is over, we force a final send. The subclasses might also do work.  
      virtual void DoEndOfTick();

   public:
      /**
       * Start a Server
       * @param The port number used by the server
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
      virtual void ProcessNetClientRequestConnection(const dtGame::MachineInfoMessage& msg);

      /**
       * Processes a MessageType::NETCLIENT_NOTIFY_DISCONNECT Message.
       * @param msg The message
       */
      virtual void ProcessNetClientNotifyDisconnect(const dtGame::Message& msg);

      /// Disconnects both open connections and the server listener.
      virtual void Disconnect();

      /// Overridden to handle config properties.
      virtual void OnAddedToGM();

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
