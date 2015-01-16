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
 * Pjotr van Amerongen, Curtiss Murphy
 */
#ifndef DELTA_CLIENTNETWORKCOMPONENT
#define DELTA_CLIENTNETWORKCOMPONENT

#include <dtNetGM/export.h>
#include <dtNetGM/networkcomponent.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace OpenThreads
{
   class Block;
}
/// @endcond

namespace dtNetGM
{
   class NetworkComponent;

   class DT_NETGM_EXPORT ClientNetworkComponent : public NetworkComponent
   {
   public:
      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const std::string DEFAULT_NAME;

      typedef NetworkComponent BaseClass;

      /**
       * Main constructor.  Takes an optional type, but this is more for subclasses.
       */
      ClientNetworkComponent(dtCore::SystemComponentType& type = *TYPE);

      /**
       * Construct a ClientNetworkComponent with a game name and version to be used by GNE
       * Calls base class constructor to initialize GNE
       * @param gameName The game name
       * @param gameVersion The game version
       * @param logFile The logfile name
       */
      ClientNetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile = "");

   protected:
      // Destructor
      virtual ~ClientNetworkComponent(void);

   public:

      /** 
       * Overridden from base class to handle the Frame Sync behaviors. Depending on whether Frame Sync
       * is active, calls the base class or calls HandleIncomingMessagesInFrameSyncMode().
       */
      virtual void HandleIncomingMessages();


      /**
       * @brief Setup a new network connection to a server.
       * Call ConnectToServer to establish a connection
       * @param host The hostname to connect to
       * @param portNum The port to connect to
       * @return boolean indicating a successful connection
       */
      bool SetupClient(const std::string& host, const int portNum);

      /**
       * Processes a MessageType::NETSERVER_ACCEPT_CONNECTION Message.
       * @param msg The message
       */
      virtual void ProcessNetServerAcceptConnection(const dtGame::MachineInfoMessage& msg);

      /**
       * Processes a MessageType::NETSERVER_REJECT_CONNECTION Message.
       * @param msg The message
       */
      virtual void ProcessNetServerRejectConnection(const dtGame::NetServerRejectMessage& msg);

      /**
       * Processes a MessageType::INFO_CLIENT_CONNECTED Message.
       * @param msg The message
       */
      virtual void ProcessInfoClientConnected(const dtGame::MachineInfoMessage& msg);

      /**
       * Processes a MessageType::NETCLIENT_NOTIFY_DISCONNECT Message and removes the client
       * from the connected clients vector
       * @param msg The message
       */
      virtual void ProcessNetClientNotifyDisconnect(const dtGame::MachineInfoMessage& msg);

      /**
       * Processes a MessageType::SERVER_REQUEST_REJECTED Message.
       * @param msg The message
       */
      virtual void ProcessNetServerRejectMessage(const dtGame::ServerMessageRejected& msg);

      /**
       * Function called by a NetworkBridge if a connection disconnects
       * @param The NetworkBridge
       */
      virtual void OnDisconnect(NetworkBridge& networkBridge);

      /**
       * Reveals if a server has accepted our connectionrequest
       * @return boolean indicating if the server has accepted our connection request
       */
      bool IsConnectedClient() { return mAcceptedClient; }

      /**
       * Returns TRUE if a client connection is valid.
       */
      bool IsConnected() const;

      /**
       * Returns the MachineInfo of the Server, or NULL if we don't have a accepted connection
       * @return MachineInfo
       */
      const dtGame::MachineInfo* GetServer();

      /**
       * Utility method to put together a request connection message and send it. 
       * Note - Your client will not be able to send or receive messages from the server
       * until you do this. This has no effect if you are not connected to a server yet.
       */
      void SendRequestConnectionMessage();

      /// Overridden. Uses a mutex and adds message to the input buffer. Also checks for specific message types. 
      virtual void AddMessageToInputBuffer(const dtGame::Message& message);

   private:
      // MachineInfo of the Server
      dtCore::RefPtr<dtGame::MachineInfo> mMachineInfoServer;

      // bool indicating if the server has accepted our connection
      bool mAcceptedClient;

      GNE::ClientConnection::sptr mClient;

      /**
       * @brief connect a new GNE network connection to a server.
       * @param host The hostname to connect to
       * @param portNum The port to connect to
       * @return boolean indicating a successfull connect
       */
      bool ConnectToServer(const std::string& host, const int portNum);


      //////////////////////////////////////////////
      // FRAME SYNC BEHAVIORS - See networkcomponent.h for more info. 
      //////////////////////////////////////////////

      void HandleIncomingSyncControlMessage(const dtGame::Message& message);
      void HandleIncomingFrameSyncMessage(const dtGame::Message& message);
      void SafelyGetFrameSyncValues(bool &safeFrameSyncIsEnabled, unsigned int &safeFrameSyncNumPerSecond, 
         float &safeFrameSyncMaxWaitTime, bool &safeFrameSyncValuesAreDirty);
      void HandleIncomingMessagesInFrameSyncMode(unsigned int safeFrameSyncNumPerSecond, 
         float safeFrameSyncMaxWaitTime, bool safeFrameSyncValuesAreDirty);
      void MergeBackLogAndIncomingIntoWorking();
      int CountNumFrameSyncsInWorkingBuffer();
      int ComputeNumFrameSyncsToSend(int numFrameSyncsAvail);
      void ProcessFrameSyncsInWorkingBuffer(int numFrameSyncsAvail, int numFrameSyncsToSend);

      float mNumSyncsExpectingPerFrame;
      float mNumSyncsCurrentlyNeeded; // how many syncs we expect this frame (or next, depending on where we are)
      int mNumSyncsLeftInBackLog; // the number of frame syncs we pushed into the backlog on the previous frame.
      int mNumTimesWithoutAFrameSync; // a gross error checking value in case we stop getting frame syncs for some reason
      OpenThreads::Block mThreadSyncsBlock; // blocks to wait for a frame sync if none yet available
      // Backlog holds messages that are waiting for a frame sync. 
      MessageBufferType mMessageBufferBackLog;

   protected:
      // a vector containing information of other connected clients
      std::vector< dtCore::RefPtr<dtGame::MachineInfo> > mConnectedClients;

      /**
       * Retrieves a dtGame::MachineInfo* from the stored connections including all other connected clients
       * If no matching connection is found, NULL is returned
       * @param dtCore::UniqueId of the MachineInfo
       * @return Pointer to the dtGame::MachineInfo* or NULL
       */
      virtual const dtGame::MachineInfo* GetMachineInfo(const dtCore::UniqueId& uniqueId);

   };
}

#endif // DELTA_CLIENTNETWORKCOMPONENT
