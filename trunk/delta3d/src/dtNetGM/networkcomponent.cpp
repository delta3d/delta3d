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

#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/messagepacket.h>
#include <dtNetGM/machineinfomessage.h>
#include <dtNetGM/networkbridge.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/basemessages.h>
#include <dtUtil/log.h>
#include <dtCore/system.h>

namespace dtNetGM
{
    // The release version will not compile with the following code ????????
    bool NetworkComponent::mGneInitialized = false;

    IMPLEMENT_ENUM(NetworkComponent::NetworkComponentException);

    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::UNABLE_TO_START_SERVER("Unable to start server");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::UNABLE_TO_CONNECT_TO_SERVER("Unable to connect to server");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_NOT_INITIALIZED("Network not initialized.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_UNAVAILABLE("Network connection unavailable.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::INVALID_HOSTNAME("Invalid hostname.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::INVALID_DESTINATION("Invalid destination.");
    
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_ERROR("Network error.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_FAILURE("Network failure.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_LISTEN_FAILURE("Network listen failure.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_CONNECT_FAILURE("Network connect failure.");
    const NetworkComponent::NetworkComponentException NetworkComponent::NetworkComponentException::NETWORK_TIMEOUT("Network timeout.");

    IMPLEMENT_ENUM(NetworkComponent::DestinationType);

    const NetworkComponent::DestinationType NetworkComponent::DestinationType::DESTINATION("Destination");
    const NetworkComponent::DestinationType NetworkComponent::DestinationType::ALL_CLIENTS("All Clients");
    const NetworkComponent::DestinationType NetworkComponent::DestinationType::ALL_NOT_CLIENTS("All Not Clients");

    NetworkComponent::NetworkComponent(const std::string &gameName, const int gameVersion, const std::string &logFile)
        : dtGame::GMComponent("NetworkComponent")
        , mShuttingDown(false)
        , mReliable(true)
        , mRateOut(0)
        , mRateIn(0)
    {
        mConnections.clear();

        if(GetInstanceCount() == 0) 
        {
            mGneInitialized = false;
        }

        InitializeNetwork(gameName, gameVersion, logFile);
    }

    NetworkComponent::~NetworkComponent(void)
    {
        mConnections.clear();
    }

    void NetworkComponent::OnAddedToGM()
    { 
        // Register Network specific messages
        GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION);
        GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::INFO_CLIENT_CONNECTED);

        GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION);
        GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
    }

    void NetworkComponent::ProcessMessage(const dtGame::Message& message)
    {
        if (GetGameManager() == NULL)
        {
            LOG_ERROR("This component is not assigned to a GameManager, but received a message.  It will be ignored.");         
            return;
        }  

        if (message.GetDestination() != NULL && GetGameManager()->GetMachineInfo() != *message.GetDestination())
        {
            LOG_DEBUG("Received message has a destination set to a different GameManager than this one. It will be ignored.");         
            return;
        }

        // Forward all messages to the appropriate function
        if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION)
            ProcessNetClientRequestConnection(static_cast<const MachineInfoMessage&>(message));

        else if(message.GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION)
            ProcessNetServerAcceptConnection(static_cast<const MachineInfoMessage&>(message));

        else if(message.GetMessageType() == dtGame::MessageType::NETSERVER_REJECT_CONNECTION)
            ProcessNetServerRejectConnection(static_cast<const dtGame::NetServerRejectMessage&>(message));

        else if(message.GetMessageType() == dtGame::MessageType::INFO_CLIENT_CONNECTED)
            ProcessInfoClientConnected(static_cast<const MachineInfoMessage&>(message));

        else if(message.GetMessageType() == dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT)
            ProcessNetClientNotifyDisconnect(static_cast<const MachineInfoMessage&>(message));

        else if(message.GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED)
            ProcessNetServerRejectMessage(static_cast<const dtGame::ServerMessageRejected&>(message));
    }

    void NetworkComponent::SetConnectionParameters(bool reliable, int bandWidthIn, int bandWidthOut)
    {
        mReliable = reliable;
        mRateIn = bandWidthIn;
        mRateOut = bandWidthOut;
    }

    void NetworkComponent::InitializeNetwork(const std::string &gameName, int gameVersion, const std::string &logFile ) 
    {
        mMutex.acquire();

        // Is GNE already initialized??
        // if so we should actually check gamename and version, but not yet implemented
        if(!IsGneInitialized()) 
        {
            // Initialize GNE
            if (GNE::initGNE(NL_IP, atexit, 1000) )
            {
                LOG_ERROR("Can't initialize network");
                return;
            }

            // Set Gameinfo
            GNE::setGameInformation(gameName, gameVersion );

            GNE::GNEProtocolVersionNumber num = GNE::getGNEProtocolVersion();

            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,     
                "Using GNE protocol: %d.%d.%d", num.version, num.subVersion, num.build );

            #ifdef _DEBUG
                GNE::initDebug(GNE::DLEVEL1 | GNE::DLEVEL2 | GNE::DLEVEL3 | GNE::DLEVEL4 | GNE::DLEVEL5, logFile.c_str());
            #endif
            // register our MessagePacket to send messages across the network
            GNE::PacketParser::defaultRegisterPacket<MessagePacket>();

            mGneInitialized = true;
        }
        mMutex.release();
    }

    void NetworkComponent::OnNewConnection(NetworkBridge& networkBridge)
    {
        mMutex.acquire();

        networkBridge.SetClientConnected(false);
        AddConnection(&networkBridge);

        mMutex.release();
    }

    void NetworkComponent::AddConnection(NetworkBridge* networkBridge)
    {
        mMutex.acquire();
        mConnections.push_back(networkBridge);
    
        LOG_DEBUG("Added connection " + networkBridge->GetHostDescription());

        mMutex.release();
    }

    void NetworkComponent::RemoveConnection(const dtGame::MachineInfo& machineInfo)
    {
        mMutex.acquire();

        for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
        {
            if((*iter)->GetMachineInfo() == machineInfo)
            {
                mConnections.erase(iter);
                mMutex.release();
                return;
            }
        }

        LOG_ERROR("Connection not found! " + machineInfo.GetName() + " [" + machineInfo.GetHostName()+ "]");
        mMutex.release();
    }

    const dtGame::MachineInfo* NetworkComponent::GetMachineInfo(const dtCore::UniqueId& uniqueId)
    {
        mMutex.acquire();

        for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
        {
            if((*iter)->GetMachineInfo().GetUniqueId() == uniqueId)
            {
                mMutex.release();
                return &((*iter)->GetMachineInfo());
            }
        }

        if(uniqueId == GetGameManager()->GetMachineInfo().GetUniqueId()) 
        {
            mMutex.release();
            return &(GetGameManager()->GetMachineInfo());
        }

        mMutex.release();
        return NULL;
    }

    NetworkBridge* NetworkComponent::GetConnection(const dtGame::MachineInfo& machineInfo)
    {
        mMutex.acquire();

        for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
        {
            if((*iter)->GetMachineInfo() == machineInfo)
            {
                mMutex.release();
                return (*iter);
            }
        }
        mMutex.release();
        return NULL;
    }

    void NetworkComponent::GetConnectedClients(std::vector<NetworkBridge*> &connectedClients)
    {
        mMutex.acquire();

        connectedClients.clear();

        for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
        {
            if((*iter)->IsConnectedClient())
            {
                connectedClients.push_back((*iter));
            }
        }
        mMutex.release();
    }

    void NetworkComponent::OnConnect(NetworkBridge& networkBridge)
    {
        networkBridge.SetClientConnected(false);        
        AddConnection(&networkBridge);
    }

    void NetworkComponent::OnExit(NetworkBridge& networkBridge)
    {
        LOG_DEBUG(networkBridge.GetHostDescription() + " is exiting.");
    }

    void NetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
    {
        LOG_INFO(networkBridge.GetHostDescription() + " disconnected.");
        RemoveConnection(networkBridge.GetMachineInfo());
    }

    void NetworkComponent::OnReceivedMessagePacket(NetworkBridge& networkBridge, const MessagePacket& msgPacket)
    {
        mMutex.acquire();

        // is the message to our GameManager?
        if(msgPacket.GetDestinatonId() != GetGameManager()->GetMachineInfo().GetUniqueId()) 
        {
            // If it is not a connection request, or connected client-info, do nothing......
            if(msgPacket.GetMessageId() != dtGame::MessageType::INFO_CLIENT_CONNECTED.GetId() &&
                msgPacket.GetMessageId() != dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT.GetId()&&
                msgPacket.GetMessageId() != dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION.GetId()) 
            {
                return;
            }
        }

        // Is the NetworkBridge a Client or connection info contained in message??
        if(networkBridge.IsConnectedClient() 
            || msgPacket.GetMessageId() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION.GetId() 
            || msgPacket.GetMessageId() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION.GetId() 
            || msgPacket.GetMessageId() == dtGame::MessageType::NETSERVER_REJECT_CONNECTION.GetId()
            || msgPacket.GetMessageId() == dtGame::MessageType::INFO_CLIENT_CONNECTED.GetId()
            || msgPacket.GetMessageId() != dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION.GetId()) 
        {
            // Check if message is supported
            if(!GetGameManager()->GetMessageFactory().IsMessageTypeSupported(GetGameManager()->GetMessageFactory().GetMessageTypeById(msgPacket.GetMessageId()))) {
                LOG_ERROR("Received an unsupported message. MessageId = " + dtUtil::ToString(msgPacket.GetMessageId()) + "from " + networkBridge.GetHostDescription() + ".");

                mMutex.release();
                return;
            }

            // Create Message from MessagePacket 
            dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(GetGameManager()->GetMessageFactory().GetMessageTypeById(msgPacket.GetMessageId()));
            if(!message.valid()) {
                LOG_ERROR("Error creating networkmessage.");
            }
            msgPacket.FillMessage(*message);

            if(message->GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION 
                || message->GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION) 
            {
                // store MachineInfo from Source in NetworkBridge
                MachineInfoMessage* machineMsg = static_cast<MachineInfoMessage*> (message.get());
                networkBridge.SetMachineInfo(*machineMsg->GetMachineInfo());

                if(message->GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION)
                {
                    networkBridge.SetClientConnected(true);
                }
            }

            message->SetSource(*GetMachineInfo(msgPacket.GetSourceId()));
            message->SetDestination(GetMachineInfo(msgPacket.GetDestinatonId()));
    
            OnReceivedNetworkMessage(*message);
        }

        mMutex.release();
    }


    void NetworkComponent::OnReceivedNetworkMessage(const dtGame::Message& message)
    {
        mMutex.acquire();

            // Send the Message to our GameManagerMessageProcessor
        GetGameManager()->SendMessage(message);

        mMutex.release();
    }

    void NetworkComponent::DispatchNetworkMessage(const dtGame::Message& message)
    {
        mMutex.acquire();

        if(message.GetDestination() == NULL) {
            // No Destination
            
            // Send a connection request to all non-client connections, but only
            // if we are requesting a connection
            if(message.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION) {
                // This message should be send to connections which are not clients!
                SendNetworkMessage(message, DestinationType::ALL_NOT_CLIENTS);
            }
            else {
                // Send message to all ClientConnections, default behavior for null destination!
                SendNetworkMessage(message, DestinationType::ALL_CLIENTS);
            }
        }
        else {
            // trying to send a message across the network to ourselves
            if(message.GetDestination()->GetUniqueId() == GetGameManager()->GetMachineInfo().GetUniqueId()) {
                GetGameManager()->SendMessage(message);
            }
            else {
                SendNetworkMessage(message);
            }
        }
        mMutex.release();
    }

    void NetworkComponent::SendNetworkMessage(const dtGame::Message& message, const DestinationType& destinationType)
    {     
        mMutex.acquire();

        if(IsShuttingDown())
            return;

        // Create the MessagePakcet
       MessagePacket msgPacket(message);
 
        if(destinationType == DestinationType::DESTINATION) 
        {
            for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
            {
                if((*iter)->GetMachineInfo() == *(message.GetDestination()) )
                {
                    (*iter)->SendPacket(msgPacket);
                    mMutex.release();
                    return;
                }
            }           
        } // DestinationType::DESTINATION
        else 
        {
            if(destinationType == DestinationType::ALL_CLIENTS) 
            { 
                for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
                {
                    if((*iter)->IsConnectedClient())
                    {
                       (*iter)->SendPacket(msgPacket);
                    }
                }

            } // DestinationType::ALL_CLIENTS
            else 
            { 
                // DestinationType::ALL_NOT_CLIENTS  
                for(std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
                {
                    if(!(*iter)->IsConnectedClient())
                    {
                       (*iter)->SendPacket(msgPacket);
                    }
                }
            } // DestinationType::ALL_NOT_CLIENTS
        }

        mMutex.release();
    }

    void NetworkComponent::OnFailure(NetworkBridge& networkBridge, const GNE::Error& error)
    {
        LOG_ERROR("OnFailure: " + error.toString() + " Host: " + networkBridge.GetHostDescription());
    }

    void NetworkComponent::OnError(NetworkBridge& networkBridge, const GNE::Error& error)
    {
        LOG_ERROR("onError: " + error.toString() + " Host: " + networkBridge.GetHostDescription());
    }

    void NetworkComponent::OnConnectFailure(NetworkBridge& networkBridge, const GNE::Error& error)
    {
        LOG_ERROR("onConnectFailure, Host: " + networkBridge.GetHostDescription());
    }

    void NetworkComponent::OnTimeOut(NetworkBridge& networkBridge)
    {
        LOG_ERROR("OnTimeOut, Host: " + networkBridge.GetHostDescription());
    }
    
    void NetworkComponent::ShutdownNetwork()
    {
        mMutex.acquire();

        LOG_INFO("Shutting down network...");
        mShuttingDown = true;


        // empty connections 
        while(!mConnections.empty()) 
        {
            mConnections.back()->Disconnect();
            mConnections.pop_back();
        }

        // shutdown GNE
        GNE::Connection::disconnectAll();
        GNE::Timer::stopAll();
        GNE::Thread::requestAllShutdown(GNE::Thread::USER); 

        mMutex.release();
    }
}