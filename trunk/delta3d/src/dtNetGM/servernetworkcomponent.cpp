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
#include <dtNetGM/servernetworkcomponent.h>
#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/serverconnectionlistener.h>
#include <dtNetGM/machineinfomessage.h>
#include <dtGame/basemessages.h>

namespace dtNetGM
{
    ServerNetworkComponent::ServerNetworkComponent(const std::string &gameName, const int gameVersion, const std::string &logFile)
        : NetworkComponent(gameName, gameVersion, logFile)
        , mAcceptClients(true)
    {
        SetName("ServerNetworkComponent");
    }

    ServerNetworkComponent::~ServerNetworkComponent(void)
    {
    }

    bool ServerNetworkComponent::SetupServer(int portNum) 
    {
        if (!IsGneInitialized())
        {
            LOG_ALWAYS("Network must be initialized first");
        //  EXCEPT(NetworkComponent::NetworkComponentException::NETWORK_UNAVAILABLE, std::string("Unable to start server."));

            return false;
        }

        mMutex.acquire();

        bool ret = true;

        ServerConnectionListener::sptr serverConnListener = ServerConnectionListener::Create(this, mRateIn, mRateOut, mReliable);

        if (serverConnListener->open(portNum))
        {
            LOG_ERROR("Can not open server on port: " + dtUtil::ToString<int>(portNum) );
            ret = false;
        }

        if (serverConnListener->listen() )
        {
            LOG_ERROR("Can not listen on server socket");
            ret = false;
        }
        else
        {
            LOG_INFO("Listening for connections on port: " + dtUtil::ToString<int>(portNum));
        }

        // Adjust MachineInfo of the GameManager
        GetGameManager()->GetMachineInfo().SetHostName(serverConnListener->getLocalAddress().getNameByAddress());
        GetGameManager()->GetMachineInfo().SetIPAddress(serverConnListener->getLocalAddress().toString());

        mMutex.release();
        return ret;
    }

    void ServerNetworkComponent::OnListenSuccess()
    {
        LOG_INFO("On Listen success");
    }

    void ServerNetworkComponent::OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr &listener)
    {
        LOG_ERROR("onListenFailure");
    }

    void ServerNetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
    {
        mMutex.acquire();

        if(networkBridge.IsConnectedClient() && !IsShuttingDown())
        {
            networkBridge.SetClientConnected(false);

            // send an INFO_CLIENT_DISCONNECTED message to other connected clients
            dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
            MachineInfoMessage* machineMsg = static_cast<MachineInfoMessage*> (message.get());
            machineMsg->SetMachineInfo(networkBridge.GetMachineInfo());

            SendNetworkMessage(*machineMsg, DestinationType::ALL_CLIENTS);
        }

        // remove Connection
        networkBridge.SetClientConnected(false);
        NetworkComponent::OnDisconnect(networkBridge);

        mMutex.release();
    }


    void ServerNetworkComponent::ProcessNetClientRequestConnection(const MachineInfoMessage &msg)
    {
        std::string rejectReason = "";
        bool acceptClient = AcceptClient(*msg.GetMachineInfo(), rejectReason);

        if(acceptClient)
        {
            // Inform connected clients of new client
            SendInfoClientConnectedMessage(*msg.GetMachineInfo());

            // Generate a NETSERVER_ACCEPT_CONNECTION message
            // send the MachineInfo of our server to  the new client
            dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION);
            MachineInfoMessage* acceptMsg = static_cast<MachineInfoMessage*>(message.get());
            acceptMsg->SetDestination(&msg.GetSource());
            acceptMsg->SetMachineInfo(GetGameManager()->GetMachineInfo());
            SendNetworkMessage(*acceptMsg); 

            // inform new client of connected clients
            SendConnectedClientMessage(*msg.GetMachineInfo());
            GetConnection(*msg.GetMachineInfo())->SetClientConnected(true);
        }
        else 
        {
            // generate a NETSERVER_REJECT_CONNECTION message    
            dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETSERVER_REJECT_CONNECTION);
            dtGame::NetServerRejectMessage* rejectMsg = static_cast<dtGame::NetServerRejectMessage*>(message.get());
            rejectMsg->SetDestination(&msg.GetSource());
            rejectMsg->SetRejectionMessage(rejectReason);
            GetGameManager()->SendNetworkMessage(*rejectMsg);    

            RemoveConnection(msg.GetSource());
        }
    }

    void ServerNetworkComponent::ProcessNetClientNotifyDisconnect(const dtGame::Message &msg)
    {
        if(*msg.GetDestination() != GetGameManager()->GetMachineInfo()) 
        {
            LOG_ERROR("Received client notify disconnect message from " + msg.GetSource().GetHostName() + ".");
        }
        else 
        {
            
        }

    }

    void ServerNetworkComponent::SendInfoClientConnectedMessage(const dtGame::MachineInfo& machineInfo)
    {    
        dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_CLIENT_CONNECTED);
        MachineInfoMessage* machineMsg = static_cast<MachineInfoMessage*> (msg.get());
        machineMsg->SetMachineInfo(machineInfo);

        // Send Info to all other clients
        machineMsg->SetDestination(NULL);
        SendNetworkMessage(*machineMsg, DestinationType::ALL_CLIENTS);
    }
    
    void ServerNetworkComponent::SendConnectedClientMessage(const dtGame::MachineInfo& machineInfo)
    {
        // Retrieve All Connected Clients
        std::vector<NetworkBridge*> connectedClients;
        GetConnectedClients(connectedClients);

        // Create the Message
        dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_CLIENT_CONNECTED);
        MachineInfoMessage* machineMsg = static_cast<MachineInfoMessage*> (msg.get());

        while(!connectedClients.empty()) 
        {
            // Send our new client info of already connected clients
            if(connectedClients.back()->IsConnectedClient() 
                && connectedClients.back()->GetMachineInfo() != machineInfo) 
            {
                machineMsg->SetMachineInfo(connectedClients.back()->GetMachineInfo());
                machineMsg->SetDestination(&machineInfo);
                SendNetworkMessage(*machineMsg);
            }
            connectedClients.pop_back();
        }
    }

    bool ServerNetworkComponent::AcceptClient(const dtGame::MachineInfo& machineInfo, std::string& rejectionReason)
    {
        if(!mAcceptClients) 
        {
            rejectionReason = "The server is currently not accepting new connections.";
            return false;
        }

        // Accept all connections
        return true;
    }
}