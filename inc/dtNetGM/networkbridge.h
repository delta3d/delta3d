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

#ifndef DELTA_NETWORKBRIDGE
#define DELTA_NETWORKBRIDGE

#ifdef _MSC_VER
#pragma warning ( disable : 4275 )
#pragma warning ( disable : 4251 )
#endif

#include <dtNetGM/export.h>
#include <gnelib/ConnectionListener.h>
#include <gnelib/Error.h>
#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>
#include <dtCore/base.h>
#include <dtUtil/datastream.h>
#include <dtGame/machineinfo.h>

// Forward declaration
namespace dtGame
{
   class Message;
}

namespace dtNetGM
{
   class NetworkComponent;

   /**
    * @class NetworkBridge
    * @brief contains GNE components for communication across a network
    * This class represents a single host on the other side of the network
    */
   class  DT_NETGM_EXPORT NetworkBridge : public dtCore::Base
   {
   public:
      // Constructor
      NetworkBridge(NetworkComponent* networkComp);

      // Deconstuctor
      virtual ~NetworkBridge(void);

      //// pointer used by GNE
      //typedef GNE::SmartPtr<NetworkBridge> sptr;
      //// pointer used by GNE
      //typedef GNE::WeakPtr<NetworkBridge> wptr;

      /////static method used to create a new NetworkBridge
      //static sptr Create(NetworkComponent *networkComp)
      //{
      //    return sptr(new NetworkBridge(networkComp));
      //};

      /**
       * Sets the MachineInfo
       * @param The machineinfo
       */
      void SetMachineInfo(const dtGame::MachineInfo& machineInfo);

      /**
       * Gets the MachineInfo from the host
       * @return The name
       */
      const dtGame::MachineInfo& GetMachineInfo() const;

      /**
       * Returns if the networkbridge is connected
       * @return network connected
       */
      bool IsNetworkConnected();

      /**
       * Returns if the networkbridge is connected as an accepted client to a server
       * @return is connected client
       */
      bool IsConnectedClient() const;

      /**
       * Sets if the networkbridge is an accepted client
       * @param The client state
       */
      void SetClientConnected(bool client = true) { mConnectedClient = client; };

      /**
       * Sends a DataStream across the network
       * @param The messagepacket
       */
      void SendDataStream(dtUtil::DataStream& dataStream, bool allowBestEffort);

      /**
       * Disconnects the current connection
       */
      void Disconnect(int waitTime = -1);

      /**
       * Callback function for GNE::ConnectionListener
       * @param The GNE::SyncConnection
       */
      void OnNewConnection(GNE::SyncConnection& conn);

      /**
       * Callback function for GNE::ConnectionListener
       * @param The GNE::SyncConnection
       */
      void OnConnect(GNE::SyncConnection& conn);

      /**
       * Callback function for GNE::ConnectionListener
       * @param The GNE::Connection
       */
      void OnReceive(GNE::Connection& conn);

      /**
       * Callback function for GNE::ConnectionListener
       * @param The GNE::Connection
       */
      void OnDisconnect(GNE::Connection& conn);

      /**
       * Callback function for GNE::ConnectionListener
       * @param The GNE::Connection
       */
      void OnExit(GNE::Connection& conn);

      /**
       * Callback function for GNE::ConnectionListener
       * @param conn The GNE::Connection
       * @param error The GNE::Error description
       */
      void OnFailure(GNE::Connection& conn, const GNE::Error& error);

      /**
       * Callback function for GNE::ConnectionListener
       * @param conn The GNE::Connection
       * @param error The GNE::Error description
       */
      void OnError(GNE::Connection& conn, const GNE::Error& error);

      /**
       * Callback function for GNE::ConnectionListener
       * @param conn The GNE::Connection
       * @param error The GNE::Error description
       */
      void OnConnectFailure(GNE::Connection& conn, const GNE::Error& error);

      /**
       * Callback function for GNE::ConnectionListener
       * @param The GNE::Connection
       */
      void OnTimeout(GNE::Connection& conn);

      /**
       * Returns a string describing the host
       * @return string describing the host
       */
      std::string GetHostDescription();

   private:
      dtCore::RefPtr<NetworkComponent> mNetworkComponent; ///Reference to our NetworkComponent
      dtCore::RefPtr<dtGame::MachineInfo> mMachineInfo; // MachineInfo of the remote GameManager

      GNE::Connection* mGneConnection; // Our GNE network connection for sending Packets
      bool mConnectedClient; // bool containing accepted client status

      unsigned int mLastStream;
      dtUtil::DataStream mDataStream;
      /**
       * Sets the timestamp of the machineinfo to the current time
       */
      void SetTimeStamp();

      /**
       * Acquire MachineInfo information from the current connection,
       * fills the IP address and hostname from the connection info
       */
      void AquireMachineInfo();
   };
}

#endif //DELTA_NETWORKBRIDGE
