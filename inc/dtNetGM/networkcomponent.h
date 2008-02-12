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
#ifndef DELTA_NETWORKCOMPONENT
#define DELTA_NETWORKCOMPONENT

#ifdef _MSC_VER
#pragma warning ( disable : 4275 )
#pragma warning ( disable : 4251 )
#pragma warning ( disable : 4624 )
#endif

#include <dtNetGM/export.h>
#include <string>
#include <gnelib.h>
#include <dtGame/gmcomponent.h>
#include <dtUtil/enumeration.h>

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
}

namespace dtNetGM
{
    // Forward declaration
    class NetworkBridge;
    class MachineInfoMessage;

    /**
    * @class NetworkComponent
    * @brief baseclass GMComponent to communicate as client - server
    */
    class DT_NETGM_EXPORT NetworkComponent : public dtGame::GMComponent
    {
    protected:
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
            DestinationType(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
            virtual ~DestinationType() {}
        };

    public:

        /**
        * Construct a NetworkComponent with a game name and version to be used by GNE
        * Calls InitializeNetwork to initialize GNE
        * @param gameName The game name
        * @param gameVersion The game version
        * @param logFile The logfile name
        */
        NetworkComponent(const std::string &gameName, const int gameVersion, const std::string &logFile = "");

        /**
        * Called immediately after a component is added to the GM. Used to register
        * 'additional' Network Messages on the GameManager
        */
        virtual void OnAddedToGM();

        /**
        * Function called by a GameManager to process Messages. This function forwards the connection related
        * messages to the functions processing these messages.
        * @param The Message to be process
        */        
        virtual void ProcessMessage(const dtGame::Message& message);

        /**
        * Processes a MessageType::NETCLIENT_REQUEST_CONNECTION Message.
        * @param msg The message
        */
        virtual void ProcessNetClientRequestConnection(const MachineInfoMessage &msg) { };

        /**
        * Processes a MessageType::NETSERVER_ACCEPT_CONNECTION Message.
        * @param msg The message
        */
        virtual void ProcessNetServerAcceptConnection(const MachineInfoMessage &msg) { };

        /**
        * Processes a MessageType::NETSERVER_REJECT_CONNECTION Message.
        * @param msg The message
        */
        virtual void ProcessNetServerRejectConnection(const dtGame::NetServerRejectMessage &msg) { };

        /**
        * Processes a MessageType::INFO_CLIENT_CONNECTED Message.
        * @param msg The message
        */
        virtual void ProcessInfoClientConnected(const MachineInfoMessage &msg) { };

        /**
        * Processes a MessageType::NETCLIENT_NOTIFY_DISCONNECT Message.
        * @param msg The message
        */
        virtual void ProcessNetClientNotifyDisconnect(const MachineInfoMessage &msg) { };

        /**
        * Processes a MessageType::SERVER_REQUEST_REJECTED Message.
        * @param msg The message
        */
        virtual void ProcessNetServerRejectMessage(const dtGame::ServerMessageRejected &msg) { };

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
        * Function called by a GameManager to send Messages accross a Network
        * @param The Message to be sent
        */        
        virtual void DispatchNetworkMessage(const dtGame::Message& message);

          /**
        * Function called by a Networkbridge to Signal a received DataStream
        * The network component creates the message and checks for destination and / or connection info 
        * contained in the message. If appropriate, the message is delivered to the GameManager
        * @param networkBridge The NetworkBridge which received the MessagePakcet
        * @param dataStream The DataStream received
        */        
		virtual void OnReceivedDataStream(NetworkBridge& networkBridge, dtUtil::DataStream& dataStream);

        virtual void OnReceivedNetworkMessage(const dtGame::Message& message, NetworkBridge& networkBridge);

         void SendNetworkMessage(const dtGame::Message& message, const DestinationType& destinationType = DestinationType::DESTINATION);
		
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
        const bool& IsGneInitialized() const { return mGneInitialized; };

        /**
        * Is this NetworkComponent a server?
        * @return server true/false
        */    
        virtual const bool IsServer() const { return false; };

        /**
        * Retrieves a vector containing all NetworkBridge which have an accepted 
        * client connection
        * @param The vector to be filled
        */    
        void GetConnectedClients(std::vector<NetworkBridge*> &connectedClients);

        /**
        * Shutdown network communications and clear connections
        */    
        virtual void ShutdownNetwork();

        /**
        * Are we shutting down?
        * @return ShuttingDown
        */    
        bool IsShuttingDown() { return mShuttingDown; };


    private:
        static bool mGneInitialized; // bool indicating GNE initialization

        /**
        * Called by constructor to initialize GNE
        * if a logfile name is specified, this file will contain debug information from GNE
        * @param gameName The game name
        * @param gameVersion The game version
        * @param logFile The logfile name
        */
        void InitializeNetwork(const std::string &gameName, int gameVersion, const std::string &logFile);

        bool mShuttingDown; // bool indicating if we are shutting down

    protected:
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
        * Retrieves a dtGame::MachineInfo* from the stored connections 
        * If no matching connection is found, NULL is returned
        * @param dtCore::UniqueId of the MachineInfo
        * @return Pointer to the dtGame::MachineInfo* or NULL
        */
        virtual const dtGame::MachineInfo* GetMachineInfo(const dtCore::UniqueId& uniqueId);

        bool mReliable ; // Value describing the GNE connection parameter
        int mRateOut; // Value describing the GNE connection parameter
        int mRateIn; // Value describing the GNE connection parameter

        // Mutex
        GNE::Mutex mMutex;
    };    
}
#endif // DELTA_NETWORKCOMPONENT