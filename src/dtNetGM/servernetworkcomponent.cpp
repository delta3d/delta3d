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
// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include <dtNetGM/servernetworkcomponent.h>
#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/serverconnectionlistener.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtCore/timer.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/configproperties.h>

namespace dtNetGM
{
   // Config settings for the Frame Sync behavior. Initially read during the OnAddedToGM() method. 
   // To override the config settings or the defaults, call the Set methods AFTER adding it to the GM.
   const dtUtil::RefString ServerNetworkComponent::CONFIG_PROP_FRAMESYNC_ISENABLED("dtNetGM.FrameSyncIsEnabled");
   const dtUtil::RefString ServerNetworkComponent::CONFIG_PROP_FRAMESYNC_NUMPERSECOND("dtNetGM.FrameSyncNumPerSecond");
   const dtUtil::RefString ServerNetworkComponent::CONFIG_PROP_FRAMESYNC_MAXWAITTIME("dtNetGM.FrameSyncMaxWaitTime");

   ////////////////////////////////////////////////////////////////////////////////
   ServerNetworkComponent::ServerNetworkComponent(dtCore::SystemComponentType& type)
   : NetworkComponent(type)
   , mAcceptClients(true)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ServerNetworkComponent::ServerNetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile)
   : NetworkComponent(gameName, gameVersion, logFile)
   , mAcceptClients(true)
   {
      SetName(DEFAULT_NAME);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ServerNetworkComponent::~ServerNetworkComponent(void)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::OnAddedToGM()
   {
      BaseClass::OnAddedToGM();

      // FRAME SYNC CONFIG
      // Read the frame sync config properties.  To override the config based settings, 
      // make sure you call the Set methods after adding it to the GM. 
      // If Frame Sync is enabled, we publish a FrameSyncControl message every time a client connects
      std::string strSyncIsEnabled = GetGameManager()->GetConfiguration().
         GetConfigPropertyValue(CONFIG_PROP_FRAMESYNC_ISENABLED, "false");
      if (strSyncIsEnabled == "true" || strSyncIsEnabled == "TRUE" || strSyncIsEnabled == "1")
      {
         SetFrameSyncIsEnabled(true);

         // NUM PER SECOND
         std::string strNumPerSecond = GetGameManager()->GetConfiguration().
            GetConfigPropertyValue(CONFIG_PROP_FRAMESYNC_NUMPERSECOND, "60");
         if (!strNumPerSecond.empty())
         {
            unsigned int numPerSecond = dtUtil::ToUnsignedInt(strNumPerSecond);
            SetFrameSyncNumPerSecond(numPerSecond);
         }
         else
         {
            LOGN_WARNING("dtNetGM", "Frame Sync is enabled, but no config value was found for " + CONFIG_PROP_FRAMESYNC_NUMPERSECOND + ".");
         }

         // MAX WAIT TIME
         std::string strMaxWaitTime = GetGameManager()->GetConfiguration().
            GetConfigPropertyValue(CONFIG_PROP_FRAMESYNC_MAXWAITTIME, "4.0");
         if (!strMaxWaitTime.empty())
         {
            float maxWaitTime = dtUtil::ToUnsignedInt(strMaxWaitTime);
            SetFrameSyncMaxWaitTime(maxWaitTime);
         }
         else
         {
            LOGN_WARNING("dtNetGM", "Frame Sync is enabled, but no config value was found for " + CONFIG_PROP_FRAMESYNC_MAXWAITTIME + ".");
         }

      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::DoEndOfTick()
   {
      // If our values have changed, then send out a Frame Sync Control message
      if (GetFrameSyncValuesAreDirty())
      {
         SendFrameSyncControlMessage();
         // Clear dirty
         SetFrameSyncValuesAreDirty(false);
      }

      if (GetFrameSyncIsEnabled())
      {
         // Create frame sync message.
         dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETSERVER_FRAME_SYNC);
         dtGame::ServerFrameSyncMessage* frameSync = static_cast<dtGame::ServerFrameSyncMessage*>(message.get());
         frameSync->SetServerSimTimeSinceStartup(GetGameManager()->GetSimTimeSinceStartup());
         frameSync->SetServerTimeScale(GetGameManager()->GetTimeScale());

         AddMessageToOutputBuffer(*frameSync); // Note - should go to all clients
      }

      BaseClass::DoEndOfTick();

   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::SendFrameSyncControlMessage()
   {
      // Create control message
      dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETSERVER_SYNC_CONTROL);
      dtGame::ServerSyncControlMessage* controlMsg = static_cast<dtGame::ServerSyncControlMessage*>(message.get());
      controlMsg->SetMaxWaitTime(GetFrameSyncMaxWaitTime());
      controlMsg->SetNumSyncsPerSecond(GetFrameSyncNumPerSecond());
      controlMsg->SetSyncEnabled(GetFrameSyncIsEnabled());

      AddMessageToOutputBuffer(*controlMsg);  // Note - should go to all clients
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ServerNetworkComponent::SetupServer(int portNum)
   {
      if (!IsGneInitialized())
      {
         LOGN_ALWAYS("dtNetGM", "Network must be initialized first");
         //  EXCEPT(NetworkComponent::NetworkComponentException::NETWORK_UNAVAILABLE, std::string("Unable to start server."));

         return false;
      }

      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      bool ret = true;

      ServerConnectionListener::sptr serverConnListener = ServerConnectionListener::Create(this, mRateIn, mRateOut, mReliable);

      if (serverConnListener->open(portNum))
      {
         LOGN_ERROR("dtNetGM", "Can not open server on port: " + dtUtil::ToString<int>(portNum) );
         ret = false;
      }

      if (serverConnListener->listen())
      {
         LOGN_ERROR("dtNetGM", "Can not listen on server socket");
         ret = false;
      }
      else
      {
         LOGN_ALWAYS("dtNetGM", "Listening for connections on port: " + dtUtil::ToString<int>(portNum));
      }

      // Adjust MachineInfo of the GameManager
      dtGame::GameManager* gameManager = GetGameManager();
      assert(gameManager);
      dtGame::MachineInfo& machineInfo = gameManager->GetMachineInfo();
      machineInfo.SetHostName(serverConnListener->getLocalAddress().getNameByAddress());
      machineInfo.SetIPAddress(serverConnListener->getLocalAddress().toString());

      return ret;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::OnListenSuccess()
   {
      LOGN_INFO("dtNetGM","On Listen success");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener)
   {
      using namespace GNE;
      std::ostringstream ss;
      if (error.getCode() == Error::GNETheirVersionHigh || error.getCode() == Error::GNETheirVersionLow)
      {
         GNEProtocolVersionNumber pvn = getGNEProtocolVersion();
         ss << "onListenFailure: " << error.toString() << " " << int(pvn.version) << " " << int(pvn.subVersion) << " " << int(pvn.build);
      }
      else
      {
         ss << "onListenFailure: " << error.toString();
      }
      LOGN_ERROR("dtNetGM", ss.str());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::Disconnect()
   {
      NetworkComponent::Disconnect();
      dtNetGM::ServerConnectionListener::closeAllListeners();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      if (networkBridge.IsConnectedClient() && !IsShuttingDown())
      {
         networkBridge.SetClientConnected(false);

         // send an INFO_CLIENT_DISCONNECTED message to other connected clients
         dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
         dtGame::MachineInfoMessage* machineMsg = static_cast<dtGame::MachineInfoMessage*>(message.get());
         machineMsg->SetMachineInfo(networkBridge.GetMachineInfo());

         SendNetworkMessage(*machineMsg, DestinationType::ALL_CLIENTS);
      }

      // remove Connection
      NetworkComponent::OnDisconnect(networkBridge);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::ProcessNetClientRequestConnection(const dtGame::MachineInfoMessage& msg)
   {
      std::string rejectReason = "";

      if (GetConnection(msg.GetSource()) == NULL)
      {
         return;
      }

      bool acceptClient = AcceptClient(msg.GetSource(), rejectReason);

      if (acceptClient)
      {
         // Inform connected clients of new client
         SendInfoClientConnectedMessage(msg.GetSource());

         // Generate a NETSERVER_ACCEPT_CONNECTION message
         // send the MachineInfo of our server to  the new client
         dtCore::RefPtr<dtGame::MachineInfoMessage> acceptMsg;
         GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION, acceptMsg);
         acceptMsg->SetDestination(&msg.GetSource());
         acceptMsg->SetMachineInfo(GetGameManager()->GetMachineInfo());
         SendNetworkMessage(*acceptMsg);

         // inform new client of connected clients
         SendConnectedClientMessage(msg.GetSource());
         GetConnection(msg.GetSource())->SetClientConnected(true);

         // Let the new client (and any others) know what the current frame sync mode is
         SendFrameSyncControlMessage();
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

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::ProcessNetClientNotifyDisconnect(const dtGame::Message& msg)
   {
      if (*msg.GetDestination() != GetGameManager()->GetMachineInfo())
      {
         LOGN_ERROR("dtNetGM", "Received client notify disconnect message from " + msg.GetSource().GetHostName() + ".");
      }
      else
      {
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::SendInfoClientConnectedMessage(const dtGame::MachineInfo& machineInfo)
   {
      dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_CLIENT_CONNECTED);
      dtGame::MachineInfoMessage* machineMsg = static_cast<dtGame::MachineInfoMessage*>(msg.get());
      machineMsg->SetMachineInfo(machineInfo);

      // Send Info to all other clients
      machineMsg->SetDestination(NULL);
      SendNetworkMessage(*machineMsg, DestinationType::ALL_CLIENTS);
      // Send it locally too.
      GetGameManager()->SendMessage(*machineMsg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ServerNetworkComponent::SendConnectedClientMessage(const dtGame::MachineInfo& machineInfo)
   {
      // Retrieve All Connected Clients
      std::vector<NetworkBridge*> connectedClients;
      GetConnectedClients(connectedClients);

      // Create the Message
      dtCore::RefPtr<dtGame::MachineInfoMessage> machineMsg;
      GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_CLIENT_CONNECTED, machineMsg);

      while (!connectedClients.empty())
      {
         // Send our new client info of already connected clients
         if (connectedClients.back()->IsConnectedClient()
            && connectedClients.back()->GetMachineInfo() != machineInfo)
         {
            machineMsg->SetMachineInfo(connectedClients.back()->GetMachineInfo());
            machineMsg->SetDestination(&machineInfo);
            SendNetworkMessage(*machineMsg);
         }
         connectedClients.pop_back();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ServerNetworkComponent::AcceptClient(const dtGame::MachineInfo& machineInfo, std::string& rejectionReason)
   {
      if (!mAcceptClients)
      {
         rejectionReason = "The server is currently not accepting new connections.";
         return false;
      }

      // Accept all connections
      return true;
   }
} // namespace dtNetGM
