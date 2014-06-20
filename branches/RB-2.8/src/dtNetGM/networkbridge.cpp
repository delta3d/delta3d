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

#include <queue>

#include <dtNetGM/networkbridge.h>
#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/datastreampacket.h>
#include <dtGame/machineinfo.h>
#include <dtUtil/stringutils.h>
#include <gnelib.h>
#include <dtUtil/log.h>

namespace dtNetGM
{
   NetworkBridge::NetworkBridge(NetworkComponent* networkComp)
      : dtCore::Base("NetworkBridge")
      , mNetworkComponent(networkComp)
      , mMachineInfo(new dtGame::MachineInfo())
      , mGneConnection(NULL)
      , mConnectedClient(false)
      , mLastStream(0)
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
      LOG_DEBUG("NetworkBridge destroyed [" + mMachineInfo->GetHostName() + "]");
   }

   void NetworkBridge::SetMachineInfo(const dtGame::MachineInfo& machineInfo)
   {
      *mMachineInfo = machineInfo;
   }

   const dtGame::MachineInfo& NetworkBridge::GetMachineInfo() const
   {
      return *mMachineInfo;
   }

   bool NetworkBridge::IsNetworkConnected()
   {
      if (mGneConnection != NULL)
      {
         return mGneConnection->isConnected();
      }
      return false;
   }

   bool NetworkBridge::IsConnectedClient() const
   {
      return mConnectedClient;
   }

   void NetworkBridge::OnDisconnect(GNE::Connection& conn)
   {
      // forward to NetworkComponent
      mNetworkComponent->OnDisconnect(*this);

      mConnectedClient = false;
   }

   void NetworkBridge::Disconnect(int waitTime)
   {
      if (mGneConnection != NULL)
      {
         if (waitTime <= 0)
         {
            mGneConnection->disconnect();
         }
         else
         {
            mGneConnection->disconnectSendAll(waitTime);
         }
      }
   }

   void NetworkBridge::OnExit(GNE::Connection& conn)
   {
      // forward to NetworkComponent
      mNetworkComponent->OnExit(*this);
   }

   void NetworkBridge::OnNewConnection(GNE::SyncConnection& conn)
   {
      mGneConnection = conn.getConnection().get();

      // retrieve MachineInfo components from connection
      AquireMachineInfo();

      // forward to NetworkComponent
      mNetworkComponent->OnNewConnection(*this);
   }

   void NetworkBridge::OnConnect(GNE::SyncConnection& conn)
   {
      mGneConnection = conn.getConnection().get();

      // retrieve MachineInfo components from connection
      AquireMachineInfo();

      // forward to NetworkComponent
      mNetworkComponent->OnConnect(*this);
   }

   void NetworkBridge::OnReceive(GNE::Connection& conn)
   {
      // Set the timestamp to current time
      SetTimeStamp();

      // Get next packet from stream
      GNE::Packet* next = conn.stream().getNextPacket();

      while (next != NULL)
      {
         // retrieve type
         int type = next->getType();

         if (type == GNE::PingPacket::ID)
         {
            GNE::PingPacket& ping = *((GNE::PingPacket*)next);
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

         // receive next of datastream packets, we have a reliable connection so packets are received and in correct order
         if (type == DataStreamPacket::ID)
         {
            DataStreamPacket* dataStreamPacket = static_cast<DataStreamPacket*>(next);
            if (mLastStream != dataStreamPacket->GetDataStreamId())
            {
               mDataStream.SetBufferSize(dataStreamPacket->GetDataStreamSize());
               mDataStream.ClearBuffer();
               mLastStream = dataStreamPacket->GetDataStreamId();
            }

            // goto start point of this packet
            mDataStream.Seekp((dataStreamPacket->GetIndex() * DataStreamPacket::MAX_PAYLOAD), dtUtil::DataStream::SeekTypeEnum::SET);
            mDataStream.WriteBinary((char*)dataStreamPacket->GetPayloadBuffer(), dataStreamPacket->GetPayloadSize());

            LOG_DEBUG("Received " + dtUtil::ToString(dataStreamPacket->GetIndex() + 1)
                    + " of " +  dtUtil::ToString(dataStreamPacket->GetPacketCount()) + " packets.");

            // We have a reliable stream, so packets arrive in order
            if (dataStreamPacket->GetIndex() == (dataStreamPacket->GetPacketCount() - 1))
            {
               if (dtUtil::Log::GetInstance().GetLogLevel() == dtUtil::Log::LOG_DEBUG)
               {
                  // Read MessageType::mId for special case
                  mDataStream.Rewind();
                  unsigned short msgId = 0;
                  mDataStream.Read(msgId);
                  mDataStream.Rewind();

                  LOG_DEBUG("Received stream[" + dtUtil::ToString(mLastStream)
                      + "] msgType[" + dtUtil::ToString(msgId) + "] size: "+ dtUtil::ToString(mDataStream.GetBufferSize())
                      + " packetcount: " + dtUtil::ToString(dataStreamPacket->GetPacketCount()) + ")");
               }
               mNetworkComponent->OnReceivedDataStream(*this, mDataStream);
            }
         }
         delete next;
         next = conn.stream().getNextPacket();
      }
   }

   void NetworkBridge::SendDataStream(dtUtil::DataStream& dataStream, bool allowBestEffort)
   {
      // Unreliable;
      bool reliable = allowBestEffort && mGneConnection->getStats(0).openSockets == 0;

      static unsigned int streamId = 0;
      // create packets
      ++streamId;
      unsigned int dataStreamSize = dataStream.GetBufferSize();
      std::queue<dtNetGM::DataStreamPacket> qPackets;
      dataStream.Rewind();

      int index = 0;

      while (dataStream.GetRemainingReadSize() != 0)
      {
         if (dataStream.GetRemainingReadSize() >= unsigned(dtNetGM::DataStreamPacket::MAX_PAYLOAD))
         {
            dtNetGM::DataStreamPacket packet(streamId, dataStreamSize, index++);
            int iBytes = dataStream.ReadBinary((char*) packet.GetPayloadBuffer(), dtNetGM::DataStreamPacket::MAX_PAYLOAD);
            packet.SetPayloadSize(iBytes);
            qPackets.push(packet);
         }
         else
         {
            dtNetGM::DataStreamPacket packet(streamId, dataStreamSize, index++);
            int iBytes = dataStream.ReadBinary((char*) packet.GetPayloadBuffer(), dtNetGM::DataStreamPacket::MAX_PAYLOAD);
            packet.SetPayloadSize(iBytes);
            qPackets.push(packet);
         }
      }

      unsigned int packetCount = qPackets.size();

      // send packets
      if (IsNetworkConnected())
      {
         while (!qPackets.empty())
         {
            dtNetGM::DataStreamPacket packet = qPackets.front();
            qPackets.pop();

            packet.SetPacketCount(packetCount);

            // write packet to reliable stream
            mGneConnection->stream().writePacket(packet, reliable);
         }
         LOG_DEBUG("Send DataStream[" + dtUtil::ToString(streamId) + "] in " + dtUtil::ToString(packetCount) + " packet(s) to " + GetHostDescription());
      }
   }

   void NetworkBridge::OnFailure(GNE::Connection& conn, const GNE::Error& error)
   {
      // forward to NetworkComponent
      mNetworkComponent->OnFailure(*this, error);
   }

   void NetworkBridge::OnError(GNE::Connection& conn, const GNE::Error& error )
   {
      // forward to NetworkComponent
      mNetworkComponent->OnError(*this, error);
   }

   void NetworkBridge::OnConnectFailure(GNE::Connection& conn, const GNE::Error& error)
   {
      // forward to NetworkComponent
      mNetworkComponent->OnConnectFailure(*this, error);
   }

   void NetworkBridge::OnTimeout(GNE::Connection& conn)
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


