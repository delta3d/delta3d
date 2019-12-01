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
 * Pjotr van Amerongen, Curtiss Murphy, David Guthrie
 */
// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include <dtNetGM/clientnetworkcomponent.h>
#include <dtNetGM/clientconnectionlistener.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/networkcomponent.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/basemessages.h>
#include <dtCore/system.h>


namespace dtNetGM
{

   ////////////////////////////////////////////////////////////////////
   ClientNetworkComponent::ClientNetworkComponent(dtCore::SystemComponentType& type)
   : NetworkComponent(type)
   , mAcceptedClient(false)
   , mNumSyncsExpectingPerFrame(0.0f)
   , mNumSyncsCurrentlyNeeded(0.0f)
   , mNumSyncsLeftInBackLog(0)
   , mNumTimesWithoutAFrameSync(0)
   {
   }

   ////////////////////////////////////////////////////////////////////
   ClientNetworkComponent::ClientNetworkComponent(const std::string& gameName, const int gameVersion, const std::string& logFile)
      : NetworkComponent(gameName, gameVersion, logFile)
      , mAcceptedClient(false)
      , mClient()
      , mNumSyncsExpectingPerFrame(0.0f)
      , mNumSyncsCurrentlyNeeded(0.0f)
      , mNumSyncsLeftInBackLog(0)
      , mNumTimesWithoutAFrameSync(0)
   {
      SetName(DEFAULT_NAME);
   }

   ////////////////////////////////////////////////////////////////////
   ClientNetworkComponent::~ClientNetworkComponent(void)
   {
      mConnectedClients.clear();
      mThreadSyncsBlock.release();
   }

   ////////////////////////////////////////////////////////////////////
   bool ClientNetworkComponent::SetupClient(const std::string& host, const int portNum)
   {
      if (!IsGneInitialized())
      {
         LOGN_ERROR("ClientNetworkComponent.cpp", "GNE Network not initialized");
         //EXCEPT(NetworkComponent::NetworkComponentException::NETWORK_NOT_INITIALIZED.GetName(), std::string("Unable to connect to server."));
         return false;
      }

      if (!ConnectToServer(host, portNum))
      {
         return false;
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////
   bool ClientNetworkComponent::ConnectToServer(const std::string& host, const int portNum)
   {
      GNE::Address address(host);
      address.setPort(portNum);

      if (!address.isValid())
      {
         LOGN_ERROR("ClientNetworkComponent.cpp", "Address " + address.toString() + " invalid for host \"" + host + "\"");
         //EXCEPT(NetworkComponent::NetworkComponentException::INVALID_HOSTNAME, std::string("Address invalid."));
         return false;
      }

      GNE::ConnectionParams params(ClientConnectionListener::Create(new NetworkBridge(this)));
      params.setUnrel(!mReliable);
      params.setInRate(mRateOut);
      params.setOutRate(mRateIn);

      mClient = GNE::ClientConnection::create();

      if (mClient->open(address, params))
      {
         LOGN_ERROR("dtNetGM", "Can not open socket");
         return false;
      }
      mClient->connect();

      LOGN_ALWAYS("dtNetGM", "Connecting to server at: " + address.toString());

      GNE::Error error = mClient->waitForConnect();

      if (mClient->isConnected())
      {
         GetGameManager()->GetMachineInfo().SetHostName(mClient->getLocalAddress(IsReliable()).getNameByAddress());
         GetGameManager()->GetMachineInfo().SetIPAddress(mClient->getLocalAddress(IsReliable()).toString());
         LOGN_ALWAYS("dtNetGM", "Network is connected");
      }
      else
      {
         std::ostringstream ss;
         ss << "Could not connect to server \"" << host << "\" at " << address.toString() << " with error :" <<  error.toString();
         if (error.getCode() == GNE::Error::GNETheirVersionHigh || error.getCode() == GNE::Error::GNETheirVersionLow)
         {
            GNE::GNEProtocolVersionNumber pvn = GNE::getGNEProtocolVersion();
            ss << "Protocol Version: " << int(pvn.version) << " " << int(pvn.subVersion) << " " << int(pvn.build);
         }
         LOGN_ERROR("dtNetGM", ss.str());
         return false;
      }
      return true;
   }

   ///////////////////////////////////////////////////////////
   void ClientNetworkComponent::OnDisconnect(NetworkBridge& networkBridge)
   {
      mAcceptedClient = false;
      networkBridge.SetClientConnected(false);

      LOGN_ALWAYS("dtNetGM", "Disconnected from Server: " + networkBridge.GetHostDescription());

      RemoveConnection(networkBridge.GetMachineInfo());
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::ProcessNetServerAcceptConnection(const dtGame::MachineInfoMessage& msg)
   {
      mAcceptedClient = true;

      mMachineInfoServer = new dtGame::MachineInfo("Server");
      *mMachineInfoServer = msg.GetSource();

      LOGN_INFO("dtNetGM", "Connection accepted by " + msg.GetSource().GetName() + " {" + msg.GetSource().GetHostName() + "}");
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::ProcessNetServerRejectConnection(const dtGame::NetServerRejectMessage& msg)
   {
      mAcceptedClient = false; // should stay false....
      LOGN_INFO("dtNetGM", "Connection rejected by " + msg.GetSource().GetName() + " {" + msg.GetSource().GetHostName() + "}.\nReason: " + msg.GetRejectionMessage());
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::ProcessNetServerRejectMessage(const dtGame::ServerMessageRejected& msg)
   {
      LOGN_DEBUG("dtNetGM", "Message[" + dtUtil::ToString(msg.GetMessageType().GetId()) + "] rejected by " + msg.GetSource().GetName() + " Reason: " + msg.GetCause());
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::ProcessInfoClientConnected(const dtGame::MachineInfoMessage& msg)
   {
      mConnectedClients.push_back(msg.GetMachineInfo());

      LOGN_DEBUG("dtNetGM", "InfoClientConnected: " + msg.GetMachineInfo()->GetName() + " {" + msg.GetMachineInfo()->GetHostName() + "} ID [" + msg.GetMachineInfo()->GetUniqueId().ToString() + "].");
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::ProcessNetClientNotifyDisconnect(const dtGame::MachineInfoMessage& msg)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      std::vector< dtCore::RefPtr<dtGame::MachineInfo> >::iterator iter;
      dtCore::RefPtr<dtGame::MachineInfo> machineInfo = msg.GetMachineInfo();

      LOGN_DEBUG("dtNetGM", "ClientNotifyDisconnect: " + msg.GetMachineInfo()->GetName() + " {" + msg.GetMachineInfo()->GetHostName() + "} ID [" + msg.GetMachineInfo()->GetUniqueId().ToString() + "].");

      for (iter = mConnectedClients.begin(); iter != mConnectedClients.end(); iter++)
      {
         if (*machineInfo == *(*iter))
         {
            mConnectedClients.erase(iter);
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////
   bool ClientNetworkComponent::IsConnected() const
   {
      return mClient.get() != NULL &&  mClient->isConnected();
   }

   ////////////////////////////////////////////////////////////////////
   const dtGame::MachineInfo* ClientNetworkComponent::GetServer()
   {
      if (mMachineInfoServer.valid())
      {
         return mMachineInfoServer.get();
      }
      else
      {
         return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////
   const dtGame::MachineInfo* ClientNetworkComponent::GetMachineInfo(const dtCore::UniqueId& uniqueId)
   {
      // check in direct connections (servers!)
      const dtGame::MachineInfo* machInfo = NetworkComponent::GetMachineInfo(uniqueId);

      if (machInfo == NULL)
      {
         //lock after the above method call to avoid a recursive lock.
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
         
         // find MachineInfo among other client-connections
         for (std::vector< dtCore::RefPtr<dtGame::MachineInfo> >::iterator iter = mConnectedClients.begin(); iter != mConnectedClients.end(); iter++)
         {
            if ((*iter)->GetUniqueId() == uniqueId)
            {
               machInfo = static_cast<const dtGame::MachineInfo*> ((*iter).get());
               break;
            }
         }
      }

      return machInfo;
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::SendRequestConnectionMessage()
   {
      dtCore::RefPtr<dtGame::MachineInfoMessage> message;
      GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::NETCLIENT_REQUEST_CONNECTION, message);
      message->SetDestination(GetServer());
      message->SetMachineInfo(GetGameManager()->GetMachineInfo());
      SendNetworkMessage(*message, dtNetGM::NetworkComponent::DestinationType::ALL_NOT_CLIENTS);
   }



   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////
   //
   // The rest of this file relates to Frame Syncs and message processing on the client.
   // 
   ////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::AddMessageToInputBuffer(const dtGame::Message& message)
   {      
      // COMMON - If on, we will get one of these roughly every frame - 
      if (message.GetMessageType() == dtGame::MessageType::NETSERVER_FRAME_SYNC)
      {
         HandleIncomingFrameSyncMessage(message);
      }

      // RARE - Server is changing the frame sync behavior
      else if (message.GetMessageType() == dtGame::MessageType::NETSERVER_SYNC_CONTROL)
      {
         HandleIncomingSyncControlMessage(message);
      }

      // EVERYTHING ELSE
      else 
      {
         // Store the message on the local buffer - Message queue will be forwarded to the GM on the next frame tick
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
         mMessageBufferIncoming.push_back(&message);
      }
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::HandleIncomingFrameSyncMessage(const dtGame::Message& message)
   {
      { // The sync lock needs to end before we release our block.
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
         mMessageBufferIncoming.push_back(&message); // add sync to incoming buffer
         mThreadSyncsBlock.release();
      }
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::HandleIncomingSyncControlMessage(const dtGame::Message& message)
   {
      { // Scoped lock for the frame sync values. Free lock before releasing block below
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
         const dtGame::ServerSyncControlMessage* serverSyncMessage =
            dynamic_cast<const dtGame::ServerSyncControlMessage*>(&message);

         bool syncEnabledHasChanged = GetFrameSyncIsEnabled() != serverSyncMessage->GetSyncEnabled();
         SetFrameSyncIsEnabled(serverSyncMessage->GetSyncEnabled());
         SetFrameSyncMaxWaitTime(serverSyncMessage->GetMaxWaitTime());
         SetFrameSyncNumPerSecond(serverSyncMessage->GetNumSyncsPerSecond());
         if (syncEnabledHasChanged && GetFrameSyncIsEnabled())  // was off and is now ON
         {
            mNumSyncsCurrentlyNeeded = 0.0f; // starting over...
            mNumSyncsLeftInBackLog = 0;
         }
         else if (syncEnabledHasChanged && !GetFrameSyncIsEnabled())
         {          
            // was on and is now off - so take anything left on our backlog and stick it at the front of the incoming
            MergeBackLogAndIncomingIntoWorking(); // puts the result into working and empties incoming
            mMessageBufferIncoming.swap(mMessageBufferWorking); // swap into incoming. Working is now empty.
         }
         // else - just changed some of the minor values - no biggie

         //printf("Got Frame Sync Control Message %s, %f, %d.\r\n", (GetFrameSyncIsEnabled() ? "ON " : "OFF"), 
         //   GetFrameSyncMaxWaitTime(), GetFrameSyncNumPerSecond());
      }

      // Error checking - must be in fixed time step - ok to check outside of scope lock, because only the above lines set those values anyway.
      if (GetFrameSyncIsEnabled() && !(dtCore::System::GetInstance().GetUsesFixedTimeStep()))
      {
         LOGN_ERROR("dtNetGM", "CRITICAL ERROR - Server has sent a frame sync enabled message, but the client is not in fixed Time Step. Make sure your config.xml settings are correct. Forcing Fixed Time Step in order to continue.");
         dtCore::System::GetInstance().SetFrameRate((double) GetFrameSyncNumPerSecond());
         dtCore::System::GetInstance().SetUseFixedTimeStep(true);
      }
      // If turning off, we need to release any active blocks.
      else if (!GetFrameSyncIsEnabled())
      {
         mThreadSyncsBlock.release();
      }

      // Note - we throw the sync control message away after taking the values. If multiple 
      // are received in the same frame, the last one wins and the others become irrelevant.
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::SafelyGetFrameSyncValues(bool &safeFrameSyncIsEnabled, unsigned int &safeFrameSyncNumPerSecond, 
      float &safeFrameSyncMaxWaitTime, bool &safeFrameSyncValuesAreDirty)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);
      safeFrameSyncIsEnabled = GetFrameSyncIsEnabled(); 
      safeFrameSyncNumPerSecond = GetFrameSyncNumPerSecond(); // guaranteed to be > 0.
      safeFrameSyncMaxWaitTime = GetFrameSyncMaxWaitTime();
      safeFrameSyncValuesAreDirty = GetFrameSyncValuesAreDirty();

      SetFrameSyncValuesAreDirty(false);
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::HandleIncomingMessages()
   {
      // We have local copies of the frame sync values to be thread safe. We lock to copy the real values to local
      bool safeFrameSyncIsEnabled;
      unsigned int safeFrameSyncNumPerSecond;
      float safeFrameSyncMaxWaitTime;
      bool safeFrameSyncValuesAreDirty;
      SafelyGetFrameSyncValues(safeFrameSyncIsEnabled, safeFrameSyncNumPerSecond, safeFrameSyncMaxWaitTime, safeFrameSyncValuesAreDirty);

      if (!safeFrameSyncIsEnabled)
      {
         BaseClass::HandleIncomingMessages();
      }
      else 
      {
         // Waiting messages go out first to maintain order. Waiting messages have already passed frame-sync testing. 
         HandleWaitingMessages();

         // Note - if somehow, we receive a sync control between the call to SafelyGetFrameSyncValues and now, 
         // it really doesn't matter, because it will just do the adjustment the next time through.
         HandleIncomingMessagesInFrameSyncMode(safeFrameSyncNumPerSecond, 
            safeFrameSyncMaxWaitTime, safeFrameSyncValuesAreDirty);
      }
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::HandleIncomingMessagesInFrameSyncMode(unsigned int safeFrameSyncNumPerSecond, 
      float safeFrameSyncMaxWaitTime, bool safeFrameSyncValuesAreDirty)
   {
      // This process may seem 'magical'. So, here's a synopsis of how it works. CMM
      // 
      // For this to make sense, don't think about messages. Instead, think of 'groupings of
      // messages' that each end in a Frame Sync. Depending on what the server is doing, there are 
      // a variety of cases we can get into. We could get lots of messages w/o a frame sync, 
      // or we could get a whole bunch of frame syncs (plus their messages), or in an ideal world, 
      // we could get one group of messages, ending with a frame sync, and nothing else. 
      // 
      // Given that, there are 2 keys that make frame syncs work.  1) Blocking, and 2) the backlog. 
      // 
      // (1) BLOCKING -- This allows us to hold our thread for a few milliseconds, in the case 
      // where we didn't quite get a frame sync yet. This has a HUGE impact because previously, the computers
      // might have been just slightly off, and cause 0, 2, 0, 2, 0, 2, ... types of scenarios because the 
      // time is just ever so slightly off. But, if we can hold our thread up, even for just a few milliseconds, 
      // then we can often turn that right back into 1, 1, 1, 1, 1, ... The process uses an Open thread Block. 
      //
      // (2) BACKLOG -- The backlog is where we keep extra messages to be looked at next time. They could 
      // be messages without a frame sync, or in some cases, we could have a full frame plus frame sync 
      // in our backlog. As a rule, if we have more frame syncs than we need, then we can only keep 
      // 1 of the extra frame syncs in the backlog. So, if the server has a huge hiccup, and we 
      // got 10 frame syncs at once, we will likley send out 9 frame syncs worth and put one in 
      // our backlog. If we needed a frame sync this time around but didn't get one, then we put 
      // all the remaining messages into our backlog. The backlog is best for cases where we 
      // occasional get too many: 2, 0, 1, 1, 1, 1, ...


      // If the values changed, recompute our expectations per frame.
      if (safeFrameSyncValuesAreDirty)
      {
         float sysFrameRate = (float) (dtCore::System::GetInstance().GetFrameRate());
         mNumSyncsExpectingPerFrame =  sysFrameRate / safeFrameSyncNumPerSecond;
      }

      // we increment how many syncs we need at the start of the whole process, not at the end
      mNumSyncsCurrentlyNeeded += mNumSyncsExpectingPerFrame;

      // BLOCKING - Do our blocking (as described above)
      // First, check that we didn't have extra Frame Syncs left over in our backlog. If not, 
      // then use our block. Note that the block won't actually always block, 
      // because the incoming thread is likely to have already pushed a message 
      // into the incoming buffer at which point it calls release(). This can happen either
      // in advance of our need, or after we need it. In either case, it keeps us in sync.
      // Note - The 'floor' handles disparities in sync rate, such as the server publishing @ 30 
      // and client running @ 60 (or server 60 vs client 20). In those case, we don't 
      // want to block until we need a FULL frame sync.
      if (mNumSyncsLeftInBackLog < floor(mNumSyncsCurrentlyNeeded))
      {
         unsigned long waitTime = (unsigned long) (safeFrameSyncMaxWaitTime);
         bool syncFoundWhileWaiting = mThreadSyncsBlock.block(waitTime);
         if (!syncFoundWhileWaiting) // Bummer.
         {
            // We didn't get a frame sync. Which is a bummer and will probably cause a visual blip. 
            // The rest of the method just carries on, but we count how many times this happens in a row. 
            mNumTimesWithoutAFrameSync ++;
         }
      }

      { // We lock out here because the reset has to be in sync with the incoming list merge.
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mBufferMutex);         
         MergeBackLogAndIncomingIntoWorking(); // Build one combined list of messages
         mThreadSyncsBlock.reset(); // 
      }

      // From here on, everything uses the working buffer

      int numFrameSyncsAvail = CountNumFrameSyncsInWorkingBuffer();
      int numFrameSyncsToSend = ComputeNumFrameSyncsToSend(numFrameSyncsAvail);

      // If we have enough to send anything, then that restarts the 'currently needed' counter
      if (numFrameSyncsToSend > 0)
      {
         mNumSyncsCurrentlyNeeded = 0.0f; 
      }
      // If we got anything, then reset our gross error checking
      if (numFrameSyncsAvail > 0)
      {
         mNumTimesWithoutAFrameSync = 0;
      }

      ProcessFrameSyncsInWorkingBuffer(numFrameSyncsAvail, numFrameSyncsToSend);
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::MergeBackLogAndIncomingIntoWorking()
   {
      // Do a lock before and after this method call.
      // Note - working always STARTS empty.

      // Start with backlog messages first.
      mMessageBufferWorking.swap(mMessageBufferBackLog);
      mNumSyncsLeftInBackLog = 0;

      // Safely Append messages from Incoming
      MessageBufferType::iterator i = mMessageBufferIncoming.begin();
      MessageBufferType::iterator iend = mMessageBufferIncoming.end();
      for (; i != iend; ++i)
      {
         const dtGame::Message& msg = **i;
         mMessageBufferWorking.push_back(&msg);
      }

      // Clear out the incoming because we didn't do a swap 
      mMessageBufferIncoming.clear();
   }

   ////////////////////////////////////////////////////////////////////
   int ClientNetworkComponent::CountNumFrameSyncsInWorkingBuffer()
   {
      int result = 0;

      // Safely Append messages from Incoming
      MessageBufferType::iterator i = mMessageBufferWorking.begin();
      MessageBufferType::iterator iend = mMessageBufferWorking.end();
      for (; i != iend; ++i)
      {
         const dtGame::Message& msg = **i;
         if (msg.GetMessageType() == dtGame::MessageType::NETSERVER_FRAME_SYNC)
         {
            result ++;
         }
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////
   int ClientNetworkComponent::ComputeNumFrameSyncsToSend(int numFrameSyncsAvail)
   {
      int result = 0;

      float numFramesWorth = ((float) numFrameSyncsAvail) / mNumSyncsExpectingPerFrame;

      // Exactly what we need or less.
      if (numFramesWorth <= 1.0f)
      {
         // Catches the normal case of 1 avail and 1 expecting. But, also catches if 
         // we have frame syncs (ex 1), but not quite enough (need 2), then we push what we got. 
         // Note - we don't do the same with regular messages, only with a full grouping (ends at sync)
         result = numFrameSyncsAvail; 
      }

      else // More than we need
      {
         // Leave up to 1 full frames worth of frame syncs to go to the backlog.
         // ex - 14 frame syncs, 3 per frame, sends 12, leaves 2.
         result = (int) ((ceil(numFramesWorth) - 1.0f) * mNumSyncsExpectingPerFrame);

         // special case - if syncs expecting per frame < 1.0, it might show result = 0. 
         // In that case, if we do actually need a sync, then we send 1 
         if (result < 1 && mNumSyncsCurrentlyNeeded >= 1.0f)
            result = 1;
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////
   void ClientNetworkComponent::ProcessFrameSyncsInWorkingBuffer(int numFrameSyncsAvail, int numFrameSyncsToSend)
   {
      // Loop through our buffer. As long as we still have syncs avail and syncs that we should
      // send, then keep pushing messages. Once that stops, then put the rest in the backlog.

      MessageBufferType::iterator i = mMessageBufferWorking.begin();
      MessageBufferType::iterator iend = mMessageBufferWorking.end();

      for (; i != iend; ++i)
      {
         const dtGame::Message& msg = **i;

         // PUSH TO GM - Do we still have frame syncs to push out?
         if (numFrameSyncsAvail > 0 && numFrameSyncsToSend > 0)
         {
            if (msg.GetMessageType() == dtGame::MessageType::NETSERVER_FRAME_SYNC)
            {
               numFrameSyncsAvail --;
               numFrameSyncsToSend --;
               // Don't send frame syncs to the GM.
            }
            else // EVERYTHING ELSE
            {
               HandleOneIncomingMessage(msg); // pushes to GM, to waiting queue, etc.
            }
         }

         // ERROR CASE
         // If we have gone 100 frames without a frame sync, then just start 
         // sending whatever we have. If we get a frame sync later, all will settle back down
         // again.
         else if (mNumTimesWithoutAFrameSync > 100)
         {
            HandleOneIncomingMessage(msg); // pushes to GM, to waiting queue, etc.
         }

         // REST GOES IN BACKLOG
         else 
         {
            if (msg.GetMessageType() == dtGame::MessageType::NETSERVER_FRAME_SYNC)
            {
               mNumSyncsLeftInBackLog ++;
            }

            mMessageBufferBackLog.push_back(&msg);
         }
      }

      mMessageBufferWorking.clear();
   }

}
