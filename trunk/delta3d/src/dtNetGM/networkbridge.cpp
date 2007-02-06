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

#include <dtNetGM/networkbridge.h>
#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/messagepacket.h>
#include <dtGame/machineinfo.h>
#include <dtNetGM/machineinfomessage.h>
#include <gnelib.h>
#include <dtUtil/log.h>

namespace dtNetGM 
{
    NetworkBridge::NetworkBridge(NetworkComponent *networkComp)
        : mNetworkComponent(networkComp)
        , mConnectedClient(false)
        , mMachineInfo(new dtGame::MachineInfo())
        , mGneConnection(NULL)
    {
        mMachineInfo->SetName("Not Connected");
        mMachineInfo->SetHostName("");
        mMachineInfo->SetIPAddress("");
        mMachineInfo->SetUniqueId(dtCore::UniqueId(""));
        mMachineInfo->SetPing(-1);
        mMachineInfo->SetTimeStamp(-1);

        LOG_DEBUG("NetworkBridge created.");
    }

    NetworkBridge::~NetworkBridge(void)
    {
        LOG_DEBUG("NetworkBridge destroyed.");
    }

    void NetworkBridge::SetMachineInfo(const dtGame::MachineInfo& machineInfo)
    {
        *mMachineInfo = machineInfo;
    }

    const dtGame::MachineInfo& NetworkBridge::GetMachineInfo() const
    {
        return *mMachineInfo;
    }

    const bool NetworkBridge::IsNetworkConnected() 
    {
        if(mGneConnection != NULL) {
            return mGneConnection->isConnected();
        }
        return false;
    }    
    
    const bool NetworkBridge::IsConnectedClient() const
    { 
        return mConnectedClient;
    }

    void NetworkBridge::onDisconnect( GNE::Connection& conn )
    {
        // forward to NetworkComponent
        mNetworkComponent->OnDisconnect(*this);

        // Connection is no longer available
        mGneConnection = NULL;
    }

    void NetworkBridge::Disconnect(int waitTime)
    {
        if(mGneConnection != NULL) {
            if(mGneConnection->getState() == GNE::Connection::State::Connected) 
            {
                mGneConnection->disconnectSendAll(waitTime);
            }

            if(mGneConnection->getState() == GNE::Connection::State::Disconnected ||
                mGneConnection->getState() == GNE::Connection::State::Disconnecting)
            {
            }
        }
    }

    void NetworkBridge::onExit( GNE::Connection& conn )
    {
        // forward to NetworkComponent
        mNetworkComponent->OnExit(*this);
    }

    void NetworkBridge::onNewConn( GNE::SyncConnection& conn)
    {
        mGneConnection = conn.getConnection().get();

        // retrieve MachineInfo components from connection
        AquireMachineInfo();

        // forward to NetworkComponent
        mNetworkComponent->OnNewConnection(*this);
    }

    void NetworkBridge::onConnect( GNE::SyncConnection &conn )
    {
        mGneConnection = conn.getConnection().get();

        // retrieve MachineInfo components from connection
        AquireMachineInfo();

        // forward to NetworkComponent
        mNetworkComponent->OnConnect(*this);
    }

    void NetworkBridge::onReceive( GNE::Connection& conn )
    {
        // Set the timestamp to current time
        SetTimeStamp();        

        // Get next packet from stream
        GNE::Packet *next = conn.stream().getNextPacket();

        while (next != NULL)
        {
            // retrieve type
            int type = next->getType();

            if(type == GNE::PingPacket::ID) 
            {
                GNE::PingPacket &ping = *((GNE::PingPacket*)next);
                if (ping.isRequest())
                {
                    // reply to ping request
                    ping.makeReply();
                    conn.stream().writePacket(ping, true);
                }
                else
                {
                    // store ping in MachineInfo
                    mMachineInfo->SetPing(ping.getPingInformation().pingTime.getTotalmSec());
                }
            }

            if(type == MessagePacket::ID) 
            {
                MessagePacket *msgPacket = static_cast<MessagePacket*>(next);
                
                LOG_DEBUG("Received NetworkMessage(" + dtUtil::ToString(msgPacket->GetMessageId()) + ") from " + GetHostDescription() );

                if(mNetworkComponent.valid())
                {
                    // forward MessagePacket to NetworkComponent
                    mNetworkComponent->OnReceivedMessagePacket(*this, *msgPacket);
                }
            }
            delete next;

            // get next packet
            next = conn.stream().getNextPacket();
        }
    }

    void NetworkBridge::SendPacket(const MessagePacket& msgPacket)
    {
        if(IsNetworkConnected()) {
            LOG_DEBUG("Sending Networkmessage[" + dtUtil::ToString(msgPacket.GetMessageId()) 
                + "] to " + GetMachineInfo().GetName() + "{" 
                + GetMachineInfo().GetHostName() + "}"
                );

            // write packet to reliablestream            
            mGneConnection->stream().writePacket(msgPacket, true);
        }
    }

    void NetworkBridge::onFailure(GNE::Connection& conn, const GNE::Error& error )
    {
        // forward to NetworkComponent
        mNetworkComponent->OnFailure(*this, error);
    }

    void NetworkBridge::onError(GNE::Connection& conn, const GNE::Error& error )
    {
        // forward to NetworkComponent
        mNetworkComponent->OnError(*this, error);
    }

    void NetworkBridge::onConnectFailure(GNE::Connection &conn, const GNE::Error &error)
    {
        // forward to NetworkComponent
        mNetworkComponent->OnConnectFailure(*this, error);
    }

    void NetworkBridge::onTimeout(GNE::Connection &conn) 
    {
        // forward to NetworkComponent
        mNetworkComponent->OnTimeOut(*this);
    }

    void NetworkBridge::AquireMachineInfo()
    {
        // Set name, host name and ip
        mMachineInfo->SetName("New Connection");
        mMachineInfo->SetHostName(mGneConnection->getRemoteAddress(mNetworkComponent->IsReliable()).getNameByAddress());
        mMachineInfo->SetIPAddress(mGneConnection->getRemoteAddress(mNetworkComponent->IsReliable()).toString());

        // set 'fake' UniqueId, will be changed if appropriate info arrives
        mMachineInfo->SetUniqueId(dtCore::UniqueId(mMachineInfo->GetHostName()));

        // Set Timestamp
        SetTimeStamp();        
    }

    void NetworkBridge::SetTimeStamp()
    {
        // set to current real clock time
        mMachineInfo->SetTimeStamp(mNetworkComponent->GetGameManager()->GetRealClockTime());
    }

    std::string NetworkBridge::GetHostDescription()
    {
        std::string szHost;
        szHost = mMachineInfo->GetName() + "[" + mMachineInfo->GetHostName() + "]";
        return szHost;
    }
}


