/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005-2010, Alion Science and Technology.
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
 * Pjotr van Amerongen, David Guthrie, Curtiss Murphy
 */

#ifndef DELTA_NETWORKCOMPONENT
#define DELTA_NETWORKCOMPONENT

#ifdef _MSC_VER

#pragma warning ( disable : 4275 )
#pragma warning ( disable : 4251 )
#pragma warning ( disable : 4624 )
#endif

// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include <dtNetGM/export.h>
#include <string>
#include <gnelib.h>
#include <dtGame/gmcomponent.h>
#include <dtUtil/enumeration.h>
#include <OpenThreads/Mutex>
#include <dtUtil/threadpool.h>

#include <deque>

// Forward declaration
namespace dtCore
{
   class UniqueId;
}

// Forward declaration
namespace dtGame
{
   class GMComponent;
   class MachineInfo;
   class NetServerRejectMessage;
   class ServerMessageRejected;
   class MachineInfoMessage;
}

namespace dtNetGM
{
   // Forward declaration
   class NetworkBridge;

   class DT_NETGM_EXPORT MessageActionCode : public dtUtil::Enumeration
   {
      DECLARE_ENUM(MessageActionCode);
   public:
      // Tell the component to Send the message now.
      static MessageActionCode SEND;
      // Tell the component to Wait and ask again.
      static MessageActionCode WAIT;
      // Tell the component to send a reject message to the sender.
      static MessageActionCode REJECT;
      // Tell the component to just drop the message.
      static MessageActionCode DROP;

   private:
      MessageActionCode(const std::string& name);
      virtual ~MessageActionCode();
   };

   /**
    * @class NetworkComponent
    * @brief baseclass GMComponent to communicate as client - server
    */
   class DT_NETGM_EXPORT NetworkComponent : public dtGame::GMComponent
   {
      DECLARE_MANAGEMENT_LAYER(NetworkComponent);
   public:
      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      /**
       * @class DestinationType
       * @brief enumeration class to address different stored connections
       */
      class DT_NETGM_EXPORT DestinationType : public dtUtil::Enumeration
      {
         DECLARE_ENUM(DestinationType);
      public:
         static const DestinationType DESTINATION;
         static const DestinationType ALL_CLIENTS;
         static const DestinationType ALL_NOT_CLIENTS;

      private:
         DestinationType(const std::string& name);
         virtual ~DestinationType();
      };

   public:

      /**
       * Construct a NetworkComponent with a game name and version to be used by GNE
       * Calls InitializeNetwork to initialize GNE
       * @param gameName The game name
       * @param gameVersion The game version
       * @param logFile The logfile name
       */
      NetworkComponent(const std::string& gameName, int gameVersion, const std::string& logFile = "");
      NetworkComponent(dtCore::SystemComponentType& type = *TYPE);

      DT_DECLARE_ACCESSOR(std::string, GameName);
      DT_DECLARE_ACCESSOR(int, GameVersion);
      DT_DECLARE_ACCESSOR(std::string, GNELogFile);

      /**
       * Called immediately after a component is added to the GM. Used to register
       * 'additional' Network Messages on the GameManager
       */
      void OnAddedToGM() override;

      /// Overridden to handle shutdown.
      void OnRemovedFromGM() override;

      void BuildPropertyMap() override;

      /**
       * Function called by a GameManager to process Messages. This function forwards the connection related
       * messages to the functions processing these messages.
       * @param The Message to be process
       */
      void ProcessMessage(const dtGame::Message& message) override;

      /**
      * Processes a MessageType::TICK_LOCAL Message.
      * @param msg The message
      */
      //virtual void ProcessTickLocal(const dtGame::TickMessage& msg);

      /** 
       * Called at Tick Local. This is overridden by subclasses to perform custom behavior
       * This default version pushes ALL incoming messages over to the GM.
       */
      virtual void HandleIncomingMessages();

      /** 
       * Goes through all the messages in mWorkingBuffer and processes them. The intentional 
       * result is that mMessageBuffer will be emptied at the end.
       */
      virtual void HandleWorkingMessageBuffer();

      /** 
       * Puts the waiting queue onto the waiting buffer and then calls HandleWorkingMessageBuffer(). 
       */
      virtual void HandleWaitingMessages();

      /** 
       * Called from HandleIncomingMessages to preprocess and handle one of the incoming messages.
       * @param the single message
       */
      virtual void HandleOneIncomingMessage(const dtGame::Message& msg);

      /**
       * Processes a MessageType::NETCLIENT_REQUEST_CONNECTION Message.
       * @param msg The message
       */
      virtual void ProcessNetClientRequestConnection(const dtGame::MachineInfoMessage& msg) { };

      /**
       * Processes a MessageType::NETSERVER_ACCEPT_CONNECTION Message.
       * @param msg The message
       */
      virtual void ProcessNetServerAcceptConnection(const dtGame::MachineInfoMessage& msg) { };

      /**
       * Processes a MessageType::NETSERVER_REJECT_CONNECTION Message.
       * @param msg The message
       */
      virtual void ProcessNetServerRejectConnection(const dtGame::NetServerRejectMessage& msg) { };

      /**
       * Processes a MessageType::INFO_CLIENT_CONNECTED Message.
       * @param msg The message
       */
      virtual void ProcessInfoClientConnected(const dtGame::MachineInfoMessage& msg) { };

      /**
       * Processes a MessageType::NETCLIENT_NOTIFY_DISCONNECT Message.
       * @param msg The message
       */
      virtual void ProcessNetClientNotifyDisconnect(const dtGame::MachineInfoMessage& msg) { };

      /**
       * Processes a MessageType::SERVER_REQUEST_REJECTED Message.
       * @param msg The message
       */
      virtual void ProcessNetServerRejectMessage(const dtGame::ServerMessageRejected& msg) { };

      /**
       * Sets the connection Parameters to be used by GNE
       * @param reliable The reliability of the connection
       * @param bandWidthIn The incoming bandwidth throttle
       * @param bandWidthOut The outgoing bandwidth throttle
       */
      void SetConnectionParameters(bool reliable = true, int bandWidthIn = 0, int bandWidthOut = 0);

      /**
       * Function called by a NetworkBridge if a new connection is received
       * @param The NetworkBridge
       */
      virtual void OnNewConnection(NetworkBridge& networkBridge);

      /**
       * Function called by a NetworkBridge if a new connection is made
       * @param The NetworkBridge
       */
      virtual void OnConnect(NetworkBridge& networkBridge);

      /**
       * Function called by a NetworkBridge if a connection exits
       * @param The NetworkBridge
       */
      virtual void OnExit(NetworkBridge& networkBridge);

      /**
       * Function called by a NetworkBridge if a connection disconnects
       * @param The NetworkBridge
       */
      virtual void OnDisconnect(NetworkBridge& networkBridge);

      /**
       * Function called by a NetworkBridge if a failure occurs
       * @param networkBridge The NetworkBridge
       * @param error The GNE::Error description
       */
      virtual void OnFailure(NetworkBridge& networkBridge, const GNE::Error& error);

      /**
       * Function called by a NetworkBridge if an error occurs
       * @param networkBridge The NetworkBridge
       * @param error The GNE::Error description
       */
      virtual void OnError(NetworkBridge& networkBridge, const GNE::Error& error);

      /**
       * Function called by a NetworkBridge if a connection failure occurs
       * @param networkBridge The NetworkBridge
       * @param error The GNE::Error description
       */
      virtual void OnConnectFailure(NetworkBridge& networkBridge, const GNE::Error& error);

      /**
       * Function called by a NetworkBridge if a timeout occurs
       * @param networkBridge The NetworkBridge
       */
      virtual void OnTimeOut(NetworkBridge& networkBridge);

      /**
       * Function called by a GameManager to send Messages across a Network
       * @param The Message to be sent
       */
      void DispatchNetworkMessage(const dtGame::Message& message) override;

      /// Simple utility that takes this message and adds it to the end of the output message buffer. 
      void AddMessageToOutputBuffer(const dtGame::Message& message);

      /// Creates mutex and adds to input buffer. Made into a method so subclass could override behavior.
      virtual void AddMessageToInputBuffer(const dtGame::Message& message);

      /**
       * Function called by a Networkbridge to Signal a received DataStream
       * The network component creates the message and checks for destination and / or connection info
       * contained in the message. If appropriate, the message is delivered to the GameManager
       * @param networkBridge The NetworkBridge which received the MessagePakcet
       * @param dataStream The DataStream received
       */
      virtual void OnReceivedDataStream(NetworkBridge& networkBridge, dtUtil::DataStream& dataStream);

      virtual void OnReceivedNetworkMessage(const dtGame::Message& message, NetworkBridge& networkBridge);

      virtual void ForwardMessage(const dtGame::Message& message, NetworkBridge& networkBridge);

      /**
       * This is called when the component is ready to send a message to the game manager so the user.
       * can decide what to do with it.  By default, the message is sent unless the gm is waiting for
       * a map change to complete, then it returns wait.
       */
      virtual MessageActionCode& OnBeforeSendMessage(const dtGame::Message& message, std::string& rejectReason);


      dtUtil::DataStream CreateDataStream(const dtGame::Message& message);
      dtCore::RefPtr<dtGame::Message> CreateMessage(dtUtil::DataStream& dataStream, const NetworkBridge& networkBridge);

      /**
       * Is our GNE connection reliable
       * @return The reliability of the connection
       */
      const bool& IsReliable() const { return mReliable; };

      /**
       * Is GNE already initialized
       * @return GNE initialization
       */
      static const bool& IsGneInitialized() { return mGneInitialized; };

      /**
       * Is this NetworkComponent a server?
       * @return server true/false
       */
      virtual bool IsServer() const { return false; };

      /**
       * Retrieves a vector containing all NetworkBridge which have an accepted
       * client connection
       * @param The vector to be filled
       */
      void GetConnectedClients(std::vector<NetworkBridge*>& connectedClients);

      /**
       * Disconnect from all active network connections
       */
      virtual void Disconnect();

      /**
       * Shutdown network communications and clear connections plus shutdown the internal networking subsystem.
       */
      virtual void ShutdownNetwork();

      /**
       * Are we shutting down?
       * @return ShuttingDown
       */
      bool IsShuttingDown() { return mShuttingDown; };

      /**
       * Return hostname of the machine this NetworkComponent is running on.
       */
      std::string GetHostName();


      /** 
       * Returns true if server frame sync'ing is currently active. In this mode, the server 
       * will send a frame sync message out to the clients. The clients will use the frame sync
       * to try to spread out incoming messages to match the sync timing.  
       * Note - this value is controlled by the server and sent out to the clients.
       * @return The FrameSyncIsEnabled value (default false)
       * @see dtNetGM::ServerFrameSyncMessage
       * @see dtNetGM::ServerSyncControlMessage
       */
      bool GetFrameSyncIsEnabled();

      /** 
       * Sets the FrameSyncIsEnabled value. See GetFrameSyncIsEnabled()
       * Note - This value is set on the Server via the 'dtNetGM.FrameSyncIsEnabled' config property
       *    in the OnAddedToGM() method. To override the config setting, set it AFTER adding it to the GM.
       * @param The new FrameSyncIsEnabled value.
       * @see GetFrameSyncIsEnabled
       */
      void SetFrameSyncIsEnabled(bool newValue);
      
      /** 
       * The number of frame syncs to send/receive per second. Only relevant if GetFrameSyncIsEnabled(). 
       * If frame sync'ing is active, this value is typically something like 60. 
       * Note - this value is controlled by the server and sent out to the clients.
       * @return The FrameSyncNumPerSecond value (default 60)
       * @see GetFrameSyncIsEnabled
       * @see dtNetGM::ServerFrameSyncMessage
       * @see dtNetGM::ServerSyncControlMessage
       */
      unsigned int GetFrameSyncNumPerSecond();

      /** 
       * Sets the number of frame syncs to send/receive per second. See GetFrameSyncNumPerSecond()
       * Note - This value is set on the Server via the 'dtNetGM.FrameSyncNumPerSecond' config property
       *    in the OnAddedToGM() method. To override the config setting, set it AFTER adding it to the GM.
       * @param The FrameSyncNumPerSecond value.
       * @see GetFrameSyncNumPerSecond
       */
      void SetFrameSyncNumPerSecond(unsigned int newValue);

      /** 
       * The amount of time (in ms) to wait for a frame sync. Only relevant if GetFrameSyncIsEnabled(). 
       * If frame sync'ing is active, this value is typically something like 4 ms. 
       * Note - this value is controlled by the server and sent out to the clients.
       * Note - for now, the thread block code only uses whole numbers (in ms) for the timeout. So, use 1.0, 2.0, ...
       * @return The FrameSyncMaxWaitTime value (default 4.0)
       * @see dtNetGM::ServerFrameSyncMessage
       * @see dtNetGM::ServerSyncControlMessage
       */
      float GetFrameSyncMaxWaitTime();

      /** 
       * The amount of time (in ms) to wait for a frame sync. See GetFrameSyncMaxWaitTime()
       * Note - This value is set on the Server via the 'dtNetGM.FrameSyncMaxWaitTime' config property
       *    in the OnAddedToGM() method. To override the config setting, set it AFTER adding it to the GM.
       * Note - for now, the thread block code only uses whole numbers (in ms) for the timeout. So, use 1.0, 2.0, ...
       * @param The FrameSyncMaxWaitTime value.
       * @see GetFrameSyncMaxWaitTime
       */
      void SetFrameSyncMaxWaitTime(float newValue);

   private:
      static bool mGneInitialized; // bool indicating GNE initialization

      /**
       * Called by constructor to initialize GNE
       * if a logfile name is specified, this file will contain debug information from GNE
       * @param gameName The game name
       * @param gameVersion The game version
       * @param logFile The logfile name
       */
      void InitializeNetwork(const std::string& gameName, int gameVersion, const std::string& logFile);

      bool mShuttingDown; // bool indicating if we are shutting down

   protected:
      friend class DispatchTask;

      typedef std::deque<dtCore::RefPtr<const dtGame::Message> > MessageBufferType;

      std::vector<NetworkBridge*> mConnections; // vector containing the NetworkBridges.

      // Destructor
      virtual ~NetworkComponent(void);

      /**
       * Adds a NetworkBridge to the map
       * @param The networkbridge
       */
      void AddConnection(NetworkBridge* networkBridge);

      /**
       * Removes a NetworkBridge from the map
       * @param The MachineInfo of the connection to be removed
       */
      void RemoveConnection(const dtGame::MachineInfo& machineInfo);

      /**
       * Retrieves a NetworkBridge from the map
       * If no networkbridge is found, NULL is returned
       * @param The machineinfo
       * @return Pointer to the networkbridge
       */
      NetworkBridge* GetConnection(const dtGame::MachineInfo& machineInfo);

      /**
       * Method for simply determining the number of connections
       * held by this component.
       */
      int GetConnectionCount() const;

      /**
       * Retrieves a dtGame::MachineInfo* from the stored connections
       * If no matching connection is found, NULL is returned
       * @param dtCore::UniqueId of the MachineInfo
       * @return Pointer to the dtGame::MachineInfo* or NULL
       */
      virtual const dtGame::MachineInfo* GetMachineInfo(const dtCore::UniqueId& uniqueId);

      void SendNetworkMessages(MessageBufferType& messages);

      void SendNetworkMessage(const dtGame::Message& message, const DestinationType& destinationType = DestinationType::DESTINATION);

      /// When the tick is over, we force a final send. The subclasses might also do work.  
      virtual void DoEndOfTick();

      /// Starts the message send background task.
      void StartSendTask();

      /// FrameSyncValues are marked dirty when one changes. The subclasses each handle this differently
      bool GetFrameSyncValuesAreDirty() { return mFrameSyncValuesAreDirty; };
      /// FrameSyncValues are marked dirty when one changes. The subclasses each handle this differently
      void SetFrameSyncValuesAreDirty(bool newValue) { mFrameSyncValuesAreDirty = newValue; };

      bool mReliable ; // Value describing the GNE connection parameter
      int mRateOut; // Value describing the GNE connection parameter
      int mRateIn; // Value describing the GNE connection parameter

      // Mutex
      mutable OpenThreads::Mutex mMutex;
      // mutex for accessing the GameManager message queue
      OpenThreads::Mutex mBufferMutex;

      // buffer to store messages received from the network. Used by client & server subclasses
      MessageBufferType mMessageBufferIncoming;
      // Working buffer. Uses a member var vice a local var to prevent lots of memory reallocation. 
      // ONLY Accessed on the main thread. Used like a local var, just a member to prevent reallocation
      MessageBufferType mMessageBufferWorking;

   private:
      // out buffer doesn't need a mutex because it is only accessed on the main thread
      // despite the fact that the outgoing messages are sent in a background thread.
      MessageBufferType mMessageBufferOut;
      // messages that were told to wait end up in the wait buffer. They are rechecked every frame. 
      MessageBufferType mMessageBufferWaiting;

      std::set<short> mUnknownMessages;

      dtCore::RefPtr<dtUtil::ThreadPoolTask> mDispatchTask;
      bool mMapChangeInProcess;

      // these params are used for the server-client frame syncing behavior. 
      // Frame Syncing is used to keep a server & client in EXTREMELY tight synchronization
      // (ex 60 publishes per second) to prevent huge visual anomalies caused when the server/client
      // get slightly out of phase. See ServerFrameSyncMessage and ServerSyncControlMessage
      bool mFrameSyncIsEnabled; // Are we frame syncing? Controlled on the server config props and used by all clients
      unsigned int mFrameSyncNumPerSecond; // how often should frame syncs be sent/received
      float mFrameSyncMaxWaitTime; // how long should the client wait to get a frame sync
      bool mFrameSyncValuesAreDirty; // tracks if the frame sync values have changed

   };
}
#endif // DELTA_NETWORKCOMPONENT
