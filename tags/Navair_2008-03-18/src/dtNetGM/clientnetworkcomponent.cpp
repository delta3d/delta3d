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
#include <dtNetGM/clientnetworkcomponent.h>
#include <dtNetGM/clientconnectionlistener.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/machineinfomessage.h>
#include <dtNetGM/networkcomponent.h>
#include <dtGame/basemessages.h>


namespace dtNetGM
{
    ClientNetworkComponent::ClientNetworkComponent(const std::string &gameName, const int gameVersion, const std::string &logFile)
        : NetworkComponent(gameName, gameVersion, logFile)
        , mAcceptedClient(false)
    {
        SetName("ClientNetworkComponent");
        mConnectedClients.empty();
    }

    ClientNetworkComponent::~ClientNetworkComponent(void)
    {
        mConnectedClients.clear();
    }

    bool ClientNetworkComponent::SetupClient(const std::string &host, const int portNum)
    {
        if(!IsGneInitialized()) {
            LOGN_ERROR("ClientNetworkComponent.cpp", "GNE Network not initialized");
            //EXCEPT(NetworkComponent::NetworkComponentException::NETWORK_NOT_INITIALIZED.GetName(), std::string("Unable to connect to server."));
            return false;
        }

        if(!ConnectToServer(host, portNum)) {
            return false;
        }
        return true;
    }

    bool ClientNetworkComponent::ConnectToServer(const std::string &host, const int portNum)
    {
        GNE::Address address( host );
        address.setPort( portNum );

        if (!address.isValid())
        {
            LOGN_ERROR("ClientNetworkComponent.cpp", "Address invalid");
            //EXCEPT(NetworkComponent::NetworkComponentException::INVALID_HOSTNAME, std::string("Address invalid."));
            return false;
        }

        GNE::ConnectionParams params( ClientConnectionListener::Create(new NetworkBridge(this)));
        params.setUnrel(!mReliable);
        params.setInRate(mRateOut);
        params.setOutRate(mRateIn);

        GNE::ClientConnection::sptr mClient = GNE::ClientConnection::create();

        if (mClient->open( address, params))
        {
            LOG_ERROR("Can not open socket");
            return false;
        }
        mClient->connect();
        
        LOG_INFO("Connecting to server at: " + address.toString());

        mClient->waitForConnect();

        if (mClient->isConnected())
        {
            GetGameManager()->GetMachineInfo().SetHostName(mClient->getLocalAddress(IsReliable()).getNameByAddress());
            GetGameManager()->GetMachineInfo().SetIPAddress(mClient->getLocalAddress(IsReliable()).toString());
            LOG_INFO("Network is connected");
        }
        else
        {
            LOG_ERROR("Network connection failed.");
            return false;
        }
        return true;
    }


    void ClientNetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
    {
        mAcceptedClient = false;
        networkBridge.SetClientConnected(false);

        LOG_INFO("Disconnected from Server: " + networkBridge.GetHostDescription());

        RemoveConnection(networkBridge.GetMachineInfo());
    }

    void ClientNetworkComponent::ProcessNetServerAcceptConnection(const MachineInfoMessage &msg)
    {
        mAcceptedClient = true;

        mMachineInfoServer = new dtGame::MachineInfo("Server");
        *mMachineInfoServer = *msg.GetMachineInfo();

        LOG_INFO("Connection accepted by " + msg.GetSource().GetName() + " {" + msg.GetSource().GetHostName() + "}");
    }

    void ClientNetworkComponent::ProcessNetServerRejectConnection(const dtGame::NetServerRejectMessage &msg)
    {
        mAcceptedClient = false; // should stay false....
        LOG_INFO("Connection rejected by " + msg.GetSource().GetName() + " {" + msg.GetSource().GetHostName() + "}.\nReason: " + msg.GetRejectionMessage());
    }

    void ClientNetworkComponent::ProcessNetServerRejectMessage(const dtGame::ServerMessageRejected &msg)
    {
        LOG_DEBUG("Message[" + dtUtil::ToString(msg.GetMessageType().GetId()) + "] rejected by " + msg.GetSource().GetName() + " Reason: " + msg.GetCause());
    }

    void ClientNetworkComponent::ProcessInfoClientConnected(const MachineInfoMessage &msg)
    {
        mConnectedClients.push_back(msg.GetMachineInfo());

        LOG_DEBUG("InfoClientConnected: " + msg.GetMachineInfo()->GetName() + " {" + msg.GetMachineInfo()->GetHostName() + "} ID [" + msg.GetMachineInfo()->GetUniqueId().ToString() + "].");
    }

    void ClientNetworkComponent::ProcessNetClientNotifyDisconnect(const MachineInfoMessage &msg)
    {
        mMutex.acquire();
        std::vector< dtCore::RefPtr<dtGame::MachineInfo> >::iterator iter;
        dtCore::RefPtr<dtGame::MachineInfo> machineInfo = msg.GetMachineInfo();

        LOG_DEBUG("ClientNotifyDisconnect: " + msg.GetMachineInfo()->GetName() + " {" + msg.GetMachineInfo()->GetHostName() + "} ID [" + msg.GetMachineInfo()->GetUniqueId().ToString() + "].");

        for ( iter = mConnectedClients.begin( ) ; iter != mConnectedClients.end( ) ; iter++ )
        {
            if(*machineInfo == *(*iter)) 
            {
                mConnectedClients.erase(iter);
                break;
            }
        }
        mMutex.release();
    }

    const dtGame::MachineInfo* ClientNetworkComponent::GetServer()
    {
        if(mMachineInfoServer.valid()) 
        {
            return mMachineInfoServer.get();
        }
        else
        {
            return NULL;
        }
    }

    const dtGame::MachineInfo* ClientNetworkComponent::GetMachineInfo(const dtCore::UniqueId& uniqueId)
    {
        mMutex.acquire();   

        // check in direct connections (servers!)
        const dtGame::MachineInfo* machInfo = NetworkComponent::GetMachineInfo(uniqueId);
        if(machInfo == NULL)
        {
            // find MachineInfo among other client-connections
            for(std::vector< dtCore::RefPtr<dtGame::MachineInfo> >::iterator iter = mConnectedClients.begin(); iter != mConnectedClients.end(); iter++)
            {
                if((*iter)->GetUniqueId() == uniqueId)
                {
                    machInfo = static_cast<const dtGame::MachineInfo*> ((*iter).get());
                    break;
                }
            }
        }

        mMutex.release();
        return machInfo;

    }
}