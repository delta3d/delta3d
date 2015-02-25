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
// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/datastreampacket.h>
//#include <dtNetGM/machineinfomessage.h>
#include <dtNetGM/networkbridge.h>
//#include <dtNetGM/serverframesyncmessage.h>
//#include <dtNetGM/serversynccontrolmessage.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/basemessages.h>
#include <dtUtil/log.h>
#include <dtUtil/threadpool.h>
#include <dtCore/system.h>

#include <OpenThreads/ScopedLock>
#include <OpenThreads/Atomic>

#include <dtCore/propertymacros.h>

namespace dtNetGM
{
   // The release version will not compile with the following code ????????
   bool NetworkComponent::mGneInitialized = false;


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   class DispatchTask: public dtUtil::ThreadPoolTask
   {
   public:
      DispatchTask()
      {
      }

      virtual void operator () ()
      {
         mComponent->SendNetworkMessages(mMessageBuffer);
         mMessageBuffer.clear();
         --mQueued;
      }

      dtCore::RefPtr<NetworkComponent> mComponent;
      NetworkComponent::MessageBufferType mMessageBuffer;
      OpenThreads::Atomic mQueued;
   };

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

   NetworkComponent::NetworkComponent(dtCore::SystemComponentType& type)
   : dtGame::GMComponent(*TYPE)
   , mShuttingDown(false)
   , mReliable(true)
   , mRateOut(0)
   , mRateIn(0)
   , mMapChangeInProcess(false)
   , mFrameSyncIsEnabled(false)
   , mFrameSyncNumPerSecond(60)
   , mFrameSyncMaxWaitTime(4.0f)
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   NetworkComponent::NetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile)
   : dtGame::GMComponent(*TYPE)
   , mGameName(gameName)
   , mGameVersion(gameVersion)
   , mGNELogFile(logFile)
   , mShuttingDown(false)
   , mReliable(true)
   , mRateOut(0)
   , mRateIn(0)
   , mMapChangeInProcess(false)
   , mFrameSyncIsEnabled(false)
   , mFrameSyncNumPerSecond(60)
   , mFrameSyncMaxWaitTime(4.0f)
   {
      if (GetInstanceCount() == 0)
      {
         mGneInitialized = false;
      }
      RegisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   NetworkComponent::~NetworkComponent(void)
   {
      ShutdownNetwork();
      //De-register after calling ShutdownNetwork because the code expects the instance count to include this instance.
      DeregisterInstance(this);
   }

   DT_IMPLEMENT_ACCESSOR(NetworkComponent, std::string, GameName);
   DT_IMPLEMENT_ACCESSOR(NetworkComponent, int, GameVersion);
   DT_IMPLEMENT_ACCESSOR(NetworkComponent, std::string, GNELogFile);

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();
      dtUtil::RefString NETWORK_SETTINGS_GROUP("Network Settings");
      typedef dtCore::PropertyRegHelper<NetworkComponent> RegHelperType;
      RegHelperType propReg(*this, this, NETWORK_SETTINGS_GROUP);

      DT_REGISTER_PROPERTY(GameName, "The Name of this game from the perspective or the networking.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(GameVersion, "The version this game from the perspective or the networking.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(GNELogFile, "The log file for the GNE networking library.", RegHelperType, propReg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnAddedToGM()
   {
      InitializeNetwork(GetGameName(), GetGameVersion(), GetGNELogFile());

      //      // Just check the first message to see if it was registered.  Unless someone writes code to manually register of
      //      // of the other types in the this method but not the first, this check should prevent double registration.
      //      if (!GetGameManager()->GetMessageFactory().IsMessageTypeSupported(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION))
      //      {
      //         // Register Network specific messages
      //         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION);
      //         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::INFO_CLIENT_CONNECTED);
      //
      //         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION);
      //         GetGameManager()->GetMessageFactory().RegisterMessageType<MachineInfoMessage>(dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT);
      //
      //         GetGameManager()->GetMessageFactory().RegisterMessageType<ServerSyncControlMessage>(dtGame::MessageType::NETSERVER_SYNC_CONTROL);
      //         GetGameManager()->GetMessageFactory().RegisterMessageType<ServerFrameSyncMessage>(dtGame::MessageType::NETSERVER_FRAME_SYNC);
      //      }

      dtCore::RefPtr<DispatchTask> task = new DispatchTask;
      mDispatchTask = task;
      task->mComponent = this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnRemovedFromGM()
   {
      // This is really not a proper shutdown.  It needs to send a message across to notify the other clients
      // but this just makes it drop immediately.
      Disconnect();
      
      mDispatchTask = NULL;
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
         LOGN_ERROR("dtNetGM","This component is not assigned to a GameManager, but received a message.  It will be ignored.");
         return;
      }

      if (message.GetDestination() != NULL && GetGameManager()->GetMachineInfo() != *message.GetDestination())
      {
         LOGN_DEBUG("dtNetGM","Received message has a destination set to a different GameManager than this one. It will be ignored.");
         return;
      }

      // Forward all messages to the appropriate function
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         HandleIncomingMessages();
      }
      // Don't change this to Post-Frame for 2 reasons.  1 - Messages are not usually generated
      // after end-of-frame, and sending messages later just makes it harder for clients to be in sync. 
      // 2 - There is no post frame when system is pause, which can cause messages to
      // be delayed for a long time, or mess up the networking during pause.
      else if (message.GetMessageType() == dtGame::MessageType::TICK_END_OF_FRAME)
      {
         DoEndOfTick();
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION)
      {
         ProcessNetClientRequestConnection(static_cast<const dtGame::MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETSERVER_ACCEPT_CONNECTION)
      {
         ProcessNetServerAcceptConnection(static_cast<const dtGame::MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETSERVER_REJECT_CONNECTION)
      {
         ProcessNetServerRejectConnection(static_cast<const dtGame::NetServerRejectMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_CLIENT_CONNECTED)
      {
         ProcessInfoClientConnected(static_cast<const dtGame::MachineInfoMessage&>(message));
      }
      else if (message.GetMessageType() == dtGame::MessageType::NETCLIENT_NOTIFY_DISCONNECT)
      {
         ProcessNetClientNotifyDisconnect(static_cast<const dtGame::MachineInfoMessage&>(message));
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
   void NetworkComponent::HandleWaitingMessages()
   {
      // No need to do a scope lock - the waiting buffer is ONLY accessed on the main thread.
      mMessageBufferWorking.swap(mMessageBufferWaiting);
      HandleWorkingMessageBuffer();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::HandleIncomingMessages()
   {
      HandleWaitingMessages();

      {
         // safely push all the incoming messages onto the working buffer and then process them. 
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
         mMessageBufferWorking.swap(mMessageBufferIncoming);
      }
      HandleWorkingMessageBuffer();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::HandleWorkingMessageBuffer()
   {
      //MessageBufferType mMessageBufferWorking; // Used to be local - was made a member to prevent re-allocation of memory each frame

      MessageBufferType::iterator i = mMessageBufferWorking.begin();
      MessageBufferType::iterator iend = mMessageBufferWorking.end();
      for (; i != iend; ++i)
      {
         const dtGame::Message& msg = **i;
         HandleOneIncomingMessage(msg);
      }

      // The working buffer is ALWAYS empty for whoever swaps to it next.
      mMessageBufferWorking.clear(); // maintains memory footprint - prevents reallocation
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::HandleOneIncomingMessage(const dtGame::Message& msg)
   {
      std::string rejectMessageString;
      MessageActionCode& code = OnBeforeSendMessage(msg, rejectMessageString);

      if (code == MessageActionCode::SEND)
      {
         //printf("Sending internal Message: %s\n", msg.GetMessageType().GetName().c_str());
         GetGameManager()->SendMessage(msg);
      }
      else if (code == MessageActionCode::WAIT)
      {
         // add it to the waiting queue. All waiting messages are processed again at the start of the tick. 
         mMessageBufferWaiting.push_back(&msg); // The waiting queue is only accessed on main thread. No need to lock.
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
         // Send it back!
         DispatchNetworkMessage(*rejectMessage);
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

         dtUtil::Log::GetInstance("dtNetGM").LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,
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

      LOGN_DEBUG("dtNetGM","Added connection " + networkBridge->GetHostDescription());
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
      LOGN_WARNING("dtNetGM","Connection not found! " + machineInfo.GetName() + " [" + machineInfo.GetHostName()+ "]");
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
   int NetworkComponent::GetConnectionCount() const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      return (int)mConnections.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::DoEndOfTick()
   {
      if (!mMessageBufferOut.empty())
      {
         // At the end of the frame, we want to make sure we queue up all remaining messages.
         // Therefore, if the network publishing task is busy, wait for it to end.
         DispatchTask* task = static_cast<DispatchTask*>(mDispatchTask.get());
         if (task->WaitUntilComplete(100))
         {
            StartSendTask();
         }
         else
         {
            LOGN_ERROR("dtNetGM", "Waited 100 milliseconds for the background message task to complete before sending any remaining messages, but it timed out.");
         }

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::StartSendTask()
   {
      if (mDispatchTask.valid())
      {
         DispatchTask* task = static_cast<DispatchTask*>(mDispatchTask.get());

         // the mQueued is an atomic value that tracks whether the task is busy. If it's not running,
         // then mQueued will be 0. If, after incrementing, it is 1, then it's not busy, so 
         // we can swap safely (no thread issue) and add the task to the thread pool.  
         // The task will decrement mQueued back to 0 when it completes.
         if (++task->mQueued == 1U)
         {
            task->mMessageBuffer.swap(mMessageBufferOut);
            dtUtil::ThreadPool::AddTask(*mDispatchTask, dtUtil::ThreadPool::BACKGROUND);
         }
         else // it was already busy, so we just decrement our queued flag back.
         {
            --task->mQueued;
         }
      }

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
      LOGN_DEBUG("dtNetGM", networkBridge.GetHostDescription() + " is exiting.");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
   {
      LOGN_INFO("dtNetGM", networkBridge.GetHostDescription() + " disconnected.");
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
            if (message.valid())
            {
               // Set the MachineInfo of the NetworkBridge
               dtGame::MachineInfoMessage* machineMsg = static_cast<dtGame::MachineInfoMessage*> (message.get());
               networkBridge.SetMachineInfo(*machineMsg->GetMachineInfo());
               // The source is probably up at this point because only the unique id of the machine info would be set at
               // this point, so setting it from the one of the message will clean that up.
               machineMsg->SetSource(networkBridge.GetMachineInfo());
            }
            else
            {
               LOGN_ERROR("dtNetGM", "Received either a NETCLIENT_REQUEST_CONNECTION or NETCLIENT_ACCEPT_CONNECTION message, "
                     "but was unable to create the message from the stream data.  This is serious.");
            }

            dataStream.Rewind();
         }
         else
         {
            LOGN_WARNING("dtNetGM", "Received DataStream with MessageType " + dtUtil::ToString(msgId) + " while not being a client.");
         }
      }
      // create message, again maybe but with proper Source!
      dataStream.Rewind();
      message = CreateMessage(dataStream, networkBridge);
      if (message.valid())
      {
         OnReceivedNetworkMessage(*message, networkBridge);
         ForwardMessage(*message, networkBridge);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::ForwardMessage(const dtGame::Message& message, NetworkBridge& networkBridge)
   {
      if (message.GetDestination() == NULL ||
            (*message.GetDestination() != GetGameManager()->GetMachineInfo()))
      {
         //         if (message.GetDestination() != NULL)
         //         {
         //            LOGN_ALWAYS("dtNetGM", std::string("Dest: ") + message.GetDestination()->GetUniqueId().ToString() + std::string("  LocalMachine: ") + GetGameManager()->GetMachineInfo().GetUniqueId().ToString());
         //         }

         // forward the message to any other connections
         dtUtil::DataStream dataStreamFwd = CreateDataStream(message);
         for (std::vector<dtNetGM::NetworkBridge*>::iterator iter = mConnections.begin(); iter != mConnections.end(); iter++)
         {
            dtNetGM::NetworkBridge* bridge = *iter;
            if (bridge != &networkBridge && bridge->IsConnectedClient() && bridge->GetMachineInfo() != message.GetSource())
            {
               bridge->SendDataStream(dataStreamFwd, true);
            }
         }
      }
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
            LOGN_ERROR("dtNetGM", "Received " + message.GetMessageType().GetName() + " while connection is not accepted.");
         }
      }

      if (acceptMessage)
      {
         AddMessageToInputBuffer(message);
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
      // trying to send a message across the network to this machin
      if (message.GetDestination() != NULL && *message.GetDestination() == GetGameManager()->GetMachineInfo())
      {
         GetGameManager()->SendMessage(message);
      }

      AddMessageToOutputBuffer(message);

      if (mMessageBufferOut.size() > 5)
      {
         StartSendTask();
      }
   }

   /////////////////////////////////////////////////////////////
   void NetworkComponent::AddMessageToOutputBuffer(const dtGame::Message& message)
   {
      // The mutex is not needed here because SendNetworkMessage in this class locks.  The rest
      // of the work is done on the same thread as the gm.
      mMessageBufferOut.push_back(&message);
   }

   /////////////////////////////////////////////////////////////
   void NetworkComponent::AddMessageToInputBuffer(const dtGame::Message& message)
   {
      // Store the message on the local buffer
      // Message queue will be forwarded to the GM on the next frame tick
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
      mMessageBufferIncoming.push_back(&message);
   }

   /////////////////////////////////////////////////////////////
   void NetworkComponent::SendNetworkMessages(MessageBufferType& messageBuffer)
   {
      MessageBufferType::iterator i, iend;
      i = messageBuffer.begin();
      iend = messageBuffer.end();
      for (; i != iend; ++i)
      {
         const dtGame::Message& message = **i;

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
            if (*message.GetDestination() != GetGameManager()->GetMachineInfo())
            {
               SendNetworkMessage(message);
            }
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
               (*iter)->SendDataStream(dataStream, true);
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
                  (*iter)->SendDataStream(dataStream, true);
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
                  (*iter)->SendDataStream(dataStream, true);
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

      try
      {
         const dtGame::MessageType& messageType = gm->GetMessageFactory().GetMessageTypeById(msgId);

         // Create Message
         msg = gm->GetMessageFactory().CreateMessage(messageType);
      }
      catch (dtGame::MessageFactory::MessageTypeNotRegisteredException&)
      {
         if (mUnknownMessages.insert(msgId).second)
         {
            LOGN_WARNING("dtNetGM", "Received an unsupported message "
                  "(You will only get the log message once per message type). MessageId = " + dtUtil::ToString(msgId));
         }
         return NULL;
      }

      if (!msg.valid())
      {
         // This assumes looking up the message type will work because
         // the code above just tried to do that, and if it had failed, it wouldn't have made it here.
         // plus this is a really unusual case with no known cause.
         LOGN_ERROR("dtNetGM",
               "Unknown error creating message with message type \""
               + gm->GetMessageFactory().GetMessageTypeById(msgId).GetName() + "\"");
         return NULL;
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
         dtCore::RefPtr<dtGame::Message> msg = CreateMessage(dataStream, networkBridge);
         if (msg.valid())
         {
            // more information, there must be a causing message!
            msg->SetCausingMessage(msg);
         }
      }
      return msg;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnFailure(NetworkBridge& networkBridge, const GNE::Error& error)
   {
      LOGN_ERROR("dtNetGM", "OnFailure: " + error.toString() + " Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnError(NetworkBridge& networkBridge, const GNE::Error& error)
   {
      LOGN_ERROR("dtNetGM", "onError: " + error.toString() + " Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnConnectFailure(NetworkBridge& networkBridge, const GNE::Error& error)
   {
      LOGN_ERROR("dtNetGM", "onConnectFailure, Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::OnTimeOut(NetworkBridge& networkBridge)
   {
      LOGN_ERROR("dtNetGM", "OnTimeOut, Host: " + networkBridge.GetHostDescription());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::Disconnect()
   {
      mMessageBufferOut.clear();
      if (mDispatchTask.valid())
      {
         // block until complete to make sure the buffer is empty before disconnecting.
         if (!mDispatchTask->WaitUntilComplete(2000))
         {
            LOGN_ERROR("dtNetGM", "Attempted to wait for the background message send to complete during disconnect, but it never completed after 2 seconds.");
         }
      }

      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      // empty connections
      for (std::vector<NetworkBridge*>::iterator iter = mConnections.begin(); iter < mConnections.end(); iter++)
      {
         (*iter)->Disconnect(-1);
      }
      //mConnections.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::ShutdownNetwork()
   {
      LOGN_INFO("dtNetGM", "Shutting down network...");

      Disconnect();

      mShuttingDown = true;

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

   ////////////////////////////////////////////////////////////////////////////////
   bool NetworkComponent::GetFrameSyncIsEnabled()
   { 
      return mFrameSyncIsEnabled;  
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::SetFrameSyncIsEnabled(bool newValue) 
   { 
      if (mFrameSyncIsEnabled != newValue)
      {
         mFrameSyncIsEnabled = newValue; 
         SetFrameSyncValuesAreDirty(true);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int NetworkComponent::GetFrameSyncNumPerSecond()
   { 
      return mFrameSyncNumPerSecond; 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::SetFrameSyncNumPerSecond(unsigned int newValue) 
   { 
      if (mFrameSyncNumPerSecond != newValue)
      {
         if (newValue <= 0)
         {
            LOGN_ERROR("dtNetGM", "FrameSyncs Per Second cannot be less than 1. Canceling attempt to set to " + dtUtil::ToString(newValue));
         }
         else 
         {
            mFrameSyncNumPerSecond = newValue;
            SetFrameSyncValuesAreDirty(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   float NetworkComponent::GetFrameSyncMaxWaitTime() 
   { 
      return mFrameSyncMaxWaitTime; 
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NetworkComponent::SetFrameSyncMaxWaitTime(float newValue) 
   { 
      if (mFrameSyncMaxWaitTime != newValue)
      {
         if (newValue <= 1.0f)
         {
            LOGN_ERROR("dtNetGM", "MaxWaitTime for FrameSyncs cannot be less than 1.0. Forcing value to be 1.0 instead of the request " + dtUtil::ToString(newValue));
            newValue = 1.0f;
         }

         mFrameSyncMaxWaitTime = newValue; 
         SetFrameSyncValuesAreDirty(true);
      }
   }

} // namespace dtNetGM
