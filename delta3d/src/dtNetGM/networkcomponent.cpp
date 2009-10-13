/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)   f
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
#include <dtNetGM/datastreampacket.h>
#include <dtNetGM/machineinfomessage.h>
#include <dtNetGM/networkbridge.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/basemessages.h>
#include <dtUtil/log.h>
#include <dtCore/system.h>

#include <OpenThreads/ScopedLock>

namespace dtNetGM
{
   // The release version will not compile with the following code ????????
   bool NetworkComponent::mGneInitialized = false;

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(MessageActionCode);
   MessageActionCode::MessageActionCode(const std::string& name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }
   MessageActionCode::~MessageActionCode() {}

   MessageActionCode MessageActionCode::SEND("SEND");
   MessageActionCode MessageActionCode::WAIT("WAIT");
   MessageActionCode MessageActionCode::REJECT("REJECT");
   MessageActionCode MessageActionCode::DROP("DROP");

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(NetworkComponent::DestinationType);
   NetworkComponent::DestinationType::DestinationType(const std::string& name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   NetworkComponent::DestinationType::~DestinationType() {}

   const NetworkComponent::DestinationType NetworkComponent::DestinationType::DESTINATION("Destination");
   const NetworkComponent::DestinationType NetworkComponent::DestinationType::ALL_CLIENTS("All Clients");
   const NetworkComponent::DestinationType NetworkComponent::DestinationType::ALL_NOT_CLIENTS("All Not Clients");

   IMPLEMENT_MANAGEMENT_LAYER(NetworkComponent);

   ////////////////////////////////////////////////////////////////////////////////
   NetworkComponent::NetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile)
      : dtGame::GMComponent("NetworkComponent")
      , mShuttingDown(false)
      , mReliable(true)
      , mRateOut(0)
      , mRateIn(0)
      , mMapChangeInProcess(false)
   {
      mConnections.clear();

      if (GetInstanceCount() == 0)
      {
         mGneInitialized = false;
      }
      RegisterInstance(this);

      InitializeNetwork(gameName, gameVersion, logFile);
   }

   ////////////////////////////////////////////////////////////////////////////////
   NetworkComponent::~NetworkComponent(void)
   {
      ShutdownNetwork();
      //De-register after calling ShutdownNetwork because the code expects the instance count to include this instance.
      DeregisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnAddedToGM()
   {
      // Just check the first message to see if it was registered.  Unless someone writes code to manually register of
      // of the other types in the this method but not the first, this check should prevent double registration.
      if (!GetGameManager()->GetMessageFactory().IsMessageTypeSupported(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION))
      {
         // Register Network specific messages
         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION);
         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::INFO_CLIENT_CONNECTED);

         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION);
         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnRemovedFromGM()
   {
      // This is really not a proper shutdown.  It needs to send a message across to notify the other clients
      // but this just makes it drop immediately.
      Disconnect();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (IsShuttingDown())
      {
         return;
      }

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
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         ProcessTickLocal(static_cast<const dtGame::TickMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION)
      {
         ProcessNetClientRequestConnection(static_cast<const MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION)
      {
         ProcessNetServerAcceptConnection(static_cast<const MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETSERVER_REJECT_CONNECTION)
      {
         ProcessNetServerRejectConnection(static_cast<const dtGame::NetServerRejectMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_CLIENT_CONNECTED)
      {
         ProcessInfoClientConnected(static_cast<const MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT)
      {
         ProcessNetClientNotifyDisconnect(static_cast<const MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED)
      {
         ProcessNetServerRejectMessage(static_cast<const dtGame::ServerMessageRejected&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_CHANGE_BEGIN)
      {
         mMapChangeInProcess = true;
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_CHANGED)
      {
         mMapChangeInProcess = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::ProcessTickLocal(const dtGame::TickMessage& msg)
   {
      MessageBufferType swapBuffer;

      {
         // safely push all the received messages onto the GameManager message queue
         //printf("Buffer [%x].\n", &mBufferMutex);
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
         swapBuffer.swap(mMessageBuffer);
      }

      std::string rejectMessageString;
      MessageBufferType::iterator i, iend;
      i = swapBuffer.begin();
      iend = swapBuffer.end();
      for (; i != iend; ++i)
      {
         // pass the message to the GM
         const dtGame::Message& msg = **i;

         MessageActionCode& code = OnBeforeSendMessage(msg, rejectMessageString);
         if (code == MessageActionCode::SEND)
         {
            GetGameManager()->SendMessage(msg);
         }
         else if (code == MessageActionCode::WAIT)
         {
            //put it back in the queue
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
            mMessageBuffer.push_back(&msg);
         }
         else if (code == MessageActionCode::DROP)
         {
            //do nothing
         }
         //check reject last because it's the least likely
         else if (code == MessageActionCode::REJECT)
         {
            dtCore::RefPtr<dtGame::ServerMessageRejected> rejectMessage;
            GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::SERVER_REQUEST_REJECTED, rejectMessage);
            rejectMessage->SetCausingMessage(&msg);
            rejectMessage->SetDestination(&msg.GetSource());
            rejectMessage->SetCause(rejectMessageString);
         }
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::SetConnectionParameters(bool reliable, int bandWidthIn, int bandWidthOut)
   {
      mReliable = reliable;
      mRateIn = bandWidthIn;
      mRateOut = bandWidthOut;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::InitializeNetwork(const std::string& gameName, int gameVersion, const std::string& logFile)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      // Is GNE already initialized??
      // if so we should actually check gamename and version, but not yet implemented
      if (!IsGneInitialized())
      {
         // Initialize GNE
         if (GNE::initGNE(NL_IP, atexit, 1000))
         {
            LOG_ERROR("Can't initialize network");
            return;
         }

         // Set Gameinfo
         GNE::setGameInformation(gameName, gameVersion);

         GNE::GNEProtocolVersionNumber num = GNE::getGNEProtocolVersion();

         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
             "Using GNE protocol: %d.%d.%d", num.version, num.subVersion, num.build );

#ifdef _DEBUG
         if (dtUtil::Log::GetInstance().GetLogLevel() == dtUtil::Log::LOG_DEBUG && !logFile.empty())
         {
            GNE::initDebug(GNE::DLEVEL1 | GNE::DLEVEL2 | GNE::DLEVEL3 | GNE::DLEVEL4 | GNE::DLEVEL5, logFile.c_str());
         }
#endif

         // and of course register the new DataStreamPacket!!!!!!!!!
         GNE::PacketParser::defaultRegisterPacket<DataStreamPacket>();

         mGneInitialized = true;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnNewConnection(NetworkBridge& networkBridge)
   {
      networkBridge.SetClientConnected(false);
      AddConnection(&networkBridge);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::AddConnection(NetworkBridge* networkBridge)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
      mConnections.push_back(networkBridge);

      LOG_DEBUG("Added connection " + networkBridge->GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::RemoveConnection(const dtGame::MachineInfo& machineInfo)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
      {
         if ((*iter)->GetMachineInfo() == machineInfo)
         {
            mConnections.erase(iter);
            return;
         }
      }
      LOG_WARNING("Connection not found! " + machineInfo.GetName() + " [" + machineInfo.GetHostName()+ "]");
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtGame::MachineInfo* NetworkComponent::GetMachineInfo(const dtCore::UniqueId& uniqueId)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      if (uniqueId == GetGameManager()->GetMachineInfo().GetUniqueId())
      {
         return &(GetGameManager()->GetMachineInfo());
      }

      for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
      {
         if ((*iter)->GetMachineInfo().GetUniqueId() == uniqueId)
         {
            return &((*iter)->GetMachineInfo());
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   NetworkBridge* NetworkComponent::GetConnection(const dtGame::MachineInfo& machineInfo)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
      {
         if ((*iter)->GetMachineInfo() == machineInfo)
         {
            return (*iter);
         }
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::GetConnectedClients(std::vector<NetworkBridge*>& connectedClients)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      connectedClients.clear();

      for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
      {
         if ((*iter)->IsConnectedClient())
         {
            connectedClients.push_back((*iter));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnConnect(NetworkBridge& networkBridge)
   {
      networkBridge.SetClientConnected(false);
      AddConnection(&networkBridge);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnExit(NetworkBridge& networkBridge)
   {
      LOG_DEBUG(networkBridge.GetHostDescription() + " is exiting.");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
   {
      LOG_INFO(networkBridge.GetHostDescription() + " disconnected.");
      RemoveConnection(networkBridge.GetMachineInfo());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnReceivedDataStream(NetworkBridge& networkBridge, dtUtil::DataStream& dataStream)
   {
      if (IsShuttingDown() || GetGameManager() == NULL)
      {
         return;
      }

      dtCore::RefPtr<dtGame::Message> message;
      if (!networkBridge.IsConnectedClient())
      {
         // Read MessageType::mId for special case
         dataStream.Rewind();
         unsigned short msgId = 0;
            dataStream.Read(msgId);
            dataStream.Rewind();

         if (msgId == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION.GetId()
            || msgId == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION.GetId())
         {
            message = CreateMessage(dataStream, networkBridge);
            // Set the MachineInfo of the NetworkBridge
            MachineInfoMessage* machineMsg = static_cast<MachineInfoMessage*> (message.get());
            networkBridge.SetMachineInfo(*machineMsg->GetMachineInfo());
            // The source is probably up at this point because only the unique id of the machine info would be set at
            // this point, so setting it from the one of the message will clean that up.
            machineMsg->SetSource(networkBridge.GetMachineInfo());

            dataStream.Rewind();
         }
         else
         {
            LOG_DEBUG("Received DataStream with MessageType " + dtUtil::ToString(msgId) + " while not being a client.");
         }
      }
      // create message, again maybe but with proper Source!
      dataStream.Rewind();
      message = CreateMessage(dataStream, networkBridge);
      OnReceivedNetworkMessage(*message, networkBridge);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnReceivedNetworkMessage(const dtGame::Message& message, NetworkBridge& networkBridge)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      bool acceptMessage = networkBridge.IsConnectedClient();

      if (!acceptMessage)
      {
         if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION
            || message.GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION)
         {
            acceptMessage = true;

            if (message.GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION)
            {
               networkBridge.SetClientConnected(true);
            }
         }
         else
         {
            LOG_ERROR("Received " + message.GetMessageType().GetName() + " while connection is not accepted.");
         }
      }

      if (acceptMessage)
      {
         // Store the message on the local buffer
         // Message queue will be forwarded to the GM on the next frame tick
         //printf("Buffer [%x].\n", &mBufferMutex);
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
         mMessageBuffer.push_back(&message);
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageActionCode& NetworkComponent::OnBeforeSendMessage(const dtGame::Message& message, std::string& rejectReason)
   {
      if (mMapChangeInProcess)
      {
         return MessageActionCode::WAIT;
      }
      else
      {
         return MessageActionCode::SEND;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::DispatchNetworkMessage(const dtGame::Message& message)
   {
      // The mutex is not needed here because SendNetworkMessage in this class locks.  The rest
      // of the work is done on the same thread as the gm.

      if (message.GetDestination() == NULL)
      {
         // No Destination

         // Send a connection request to all non-client connections, but only
         // if we are requesting a connection
         if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION)
         {
            // This message should be send to connections which are not clients!
            SendNetworkMessage(message, DestinationType::ALL_NOT_CLIENTS);
         }
         else
         {
            // Send message to all ClientConnections, default behavior for null destination!
            SendNetworkMessage(message, DestinationType::ALL_CLIENTS);
         }
      }
      else
      {
         // trying to send a message across the network to ourselves
         if (*message.GetDestination() == GetGameManager()->GetMachineInfo())
         {
            GetGameManager()->SendMessage(message);
         }
         else
         {
            SendNetworkMessage(message);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::SendNetworkMessage(const dtGame::Message& message, const DestinationType& destinationType)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      if (IsShuttingDown())
      {
         return;
      }

      // Create the MessageDataStream
      dtUtil::DataStream dataStream = CreateDataStream(message);

      if (destinationType == DestinationType::DESTINATION)
      {
         for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
         {
            if ((*iter)->GetMachineInfo() == *(message.GetDestination()))
            {
               (*iter)->SendDataStream(dataStream);
               return;
            }
         }
      } // DestinationType::DESTINATION
      else
      {
         if (destinationType == DestinationType::ALL_CLIENTS)
         {
            for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
            {
               if ((*iter)->IsConnectedClient())
               {
                  (*iter)->SendDataStream(dataStream);
               }
            }
         } // DestinationType::ALL_CLIENTS
         else
         {
            // DestinationType::ALL_NOT_CLIENTS
            for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
            {
               if (!(*iter)->IsConnectedClient())
               {
                  (*iter)->SendDataStream(dataStream);
               }
            }
         } // DestinationType::ALL_NOT_CLIENTS
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtUtil::DataStream NetworkComponent::CreateDataStream(const dtGame::Message& message)
   {
      dtUtil::DataStream stream;

      stream.Write(message.GetMessageType().GetId()); // MessageType.mId
      stream.Write(message.GetSource().GetUniqueId().ToString()); // Source
      if (message.GetDestination() != NULL)
      {
         stream.Write(message.GetDestination()->GetUniqueId().ToString()); // Destination
      }
      else
      {
         stream.Write(std::string(""));
      }
      stream.Write(message.GetSendingActorId().ToString()); // Sending Actor
      stream.Write(message.GetAboutActorId().ToString()); // About Actor

      message.ToDataStream(stream);

      if (message.GetCausingMessage() != NULL)
      {
         stream.AppendDataStream(CreateDataStream(*message.GetCausingMessage()));
         // append causing message??
      }

      return stream;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtGame::Message> NetworkComponent::CreateMessage(dtUtil::DataStream& dataStream, const NetworkBridge& networkBridge)
   {
      //Sometimes the thread isn't stopped yet when the component is removed from the GM, so this ends up as NULL
      dtGame::GameManager* gm = GetGameManager();
      if (gm == NULL)
      {
         return NULL;
      }

      dtCore::RefPtr<dtGame::Message> msg;
      unsigned short msgId = 0;

      // MessageType.mId
      dataStream.Read(msgId);

      // Check if message is supported
      if (!gm->GetMessageFactory().IsMessageTypeSupported(
            gm->GetMessageFactory().GetMessageTypeById(msgId)))
      {
         LOG_ERROR("Received an unsupported message. MessageId = " + dtUtil::ToString(msgId));
         return msg;
      }

      // Create Message
      msg = gm->GetMessageFactory().CreateMessage(gm->GetMessageFactory().GetMessageTypeById(msgId));
      if (!msg.valid())
      {
         LOG_ERROR("Error creating message from stream.");
         return msg;
      }
      std::string szUniqueId;

      // Source
      dataStream.Read(szUniqueId);

      const dtGame::MachineInfo* machInfo = GetMachineInfo(dtCore::UniqueId(szUniqueId));

      if (machInfo != NULL)
      {
         msg->SetSource(*machInfo);
      }
      else
      {
         // It's either from a host that this client is talking to, or it could be the
         // first message to come over.  Either way, the machine info for the source may not be NULL.
         msg->SetSource(networkBridge.GetMachineInfo());
      }

      // Destination
      dataStream.Read(szUniqueId);
      if (szUniqueId.size() != 0)
      {
         msg->SetDestination(GetMachineInfo(dtCore::UniqueId(szUniqueId)));
      }

   // Sending Actor
      dataStream.Read(szUniqueId);
      msg->SetSendingActorId(dtCore::UniqueId(szUniqueId));

      // About Actor
      dataStream.Read(szUniqueId);
      msg->SetAboutActorId(dtCore::UniqueId(szUniqueId));

      msg->FromDataStream(dataStream);

      if (dataStream.GetRemainingReadSize() != 0)
      {
         // more information, there must be a causing message!
         msg->SetCausingMessage((CreateMessage(dataStream, networkBridge)).get());
      }
      return msg;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnFailure(NetworkBridge& networkBridge, const GNE::Error& error)
   {
      LOG_ERROR("OnFailure: " + error.toString() + " Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnError(NetworkBridge& networkBridge, const GNE::Error& error)
   {
      LOG_ERROR("onError: " + error.toString() + " Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnConnectFailure(NetworkBridge& networkBridge, const GNE::Error& error)
   {
      LOG_ERROR("onConnectFailure, Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnTimeOut(NetworkBridge& networkBridge)
   {
      LOG_ERROR("OnTimeOut, Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::Disconnect()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      mShuttingDown = true;

      // empty connections
      for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter < mConnections.end(); iter++)
      {
         (*iter)->Disconnect(-1);
      }
      mConnections.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::ShutdownNetwork()
   {
      LOG_INFO("Shutting down network...");

      Disconnect();

      // Only shutdown if this network component is the only existing one, otherwise some other
      // instance will still want GNE running, Eventually it will get shutdown by the destructor of the last
      // network component.
      if (mGneInitialized && GetInstanceCount() <= 1)
      {
         GNE::shutdownGNE();
         mGneInitialized = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string NetworkComponent::GetHostName()
   {
      const dtGame::MachineInfo* mi = GetMachineInfo(GetGameManager()->GetMachineInfo().GetUniqueId());

      if(mi == NULL)
      {
         return "";
      }
      else
      {
         return mi->GetHostName();
      }
   }
} // namespace dtNetGM
