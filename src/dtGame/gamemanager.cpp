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
 * Matthew W. Campbell, William E. Johnson II, David Guthrie, Curtiss Murphy
 */

#include <prefix/dtgameprefix.h>

#include <dtABC/application.h>

#include <dtGame/gamemanager.h>
#include <dtGame/gamemanager.inl>

#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/environmentactor.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gameactor.h>

#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>
#include <dtGame/machineinfo.h>
#include <dtGame/mapchangestatedata.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtGame/message.h>

#include <dtGame/gmstatistics.h>
#include <dtGame/gmimpl.h>
#include <dtGame/gmsettings.h>

#include <dtCore/actortype.h>
#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/actorfactory.h>

#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

#include <list>

namespace dtGame
{
   IMPLEMENT_MANAGEMENT_LAYER(GameManager);

   const std::string GameManager::CONFIG_STATISTICS_INTERVAL("GameManager.Statistics.Interval");
   const std::string GameManager::CONFIG_STATISTICS_TO_CONSOLE("GameManager.Statistics.ToConsole");
   const std::string GameManager::CONFIG_STATISTICS_OUTPUT_FILE("GameManager.Statistics.OutputFile");

   IMPLEMENT_ENUM(GameManager::ComponentPriority);

   ///@return the order id.  The higher the priority, the lower the number.
   unsigned int GameManager::ComponentPriority::GetOrderId() const { return mOrderId; }

   GameManager::ComponentPriority::ComponentPriority(const std::string& name, unsigned int orderId)
      : Enumeration(name)
      , mOrderId(orderId)
   {
      AddInstance(this);
   }
   GameManager::ComponentPriority GameManager::ComponentPriority::HIGHEST("HIGHEST", 1);
   GameManager::ComponentPriority GameManager::ComponentPriority::HIGHER("HIGHER", 2);
   GameManager::ComponentPriority GameManager::ComponentPriority::NORMAL("NORMAL", 3);
   GameManager::ComponentPriority GameManager::ComponentPriority::LOWER("LOWER", 4);
   GameManager::ComponentPriority GameManager::ComponentPriority::LOWEST("LOWEST", 5);

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::GameManager(dtCore::Scene& scene)
   : dtCore::Base("GameManager")
   , mGMImpl(new GMImpl(scene))
   {
      AddSender(&dtCore::System::GetInstance());

      mGMImpl->mMapChangeStateData = new MapChangeStateData(*this);

      // when we come alive, the first message everyone gets will be INFO_RESTARTED
      dtCore::RefPtr<Message> restartMessage =
         GetMessageFactory().CreateMessage(MessageType::INFO_RESTARTED);
      SendMessage(*restartMessage);
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::GameManager(const GameManager&)
   : mGMImpl(NULL) //added just to get rid of a warning.
   {
      // THIS IS PRIVATE AND PREVENTS USE OF COPY CONSTRUCTOR. DO NOT PUT ANYTHING HERE
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameManager& GameManager::operator=(const GameManager&) { return *this; }

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::~GameManager()
   {
      RemoveSender(&dtCore::System::GetInstance());
      delete mGMImpl;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::LoadActorRegistry(const std::string& libName)
   {
      mGMImpl->mLibMgr->LoadActorRegistry(libName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnloadActorRegistry(const std::string& libName)
   {
      mGMImpl->mLibMgr->UnloadActorRegistry(libName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetActorTypes(dtCore::ActorTypeVec& actorTypes)
   {
      mGMImpl->mLibMgr->GetActorTypes(actorTypes);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::ActorType* GameManager::FindActorType(const std::string& category, const std::string& name)
   {
      return mGMImpl->mLibMgr->FindActorType(category, name);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ActorPluginRegistry* GameManager::GetRegistry(const std::string& name)
   {
      return mGMImpl->mLibMgr->GetRegistry(name);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ActorPluginRegistry* GameManager::GetRegistryForType(dtCore::ActorType& actorType)
   {
      return mGMImpl->mLibMgr->GetRegistryForType(actorType);
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string GameManager::GetPlatformSpecificLibraryName(const std::string& libBase)
   {
      return mGMImpl->mLibMgr->GetPlatformSpecificLibraryName(libBase);
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string GameManager::GetPlatformIndependentLibraryName(const std::string& libName)
   {
      return mGMImpl->mLibMgr->GetPlatformIndependentLibraryName(libName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtUtil::ConfigProperties& GameManager::GetConfiguration()
   {
      return GetApplication();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtUtil::ConfigProperties& GameManager::GetConfiguration() const
   {
      return GetApplication();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtABC::Application& GameManager::GetApplication()
   {
      if (mGMImpl->mApplication == NULL)
      {
         throw dtGame::GeneralGameManagerException(
         "No Application was ever assigned to the GameManager.", __FILE__, __LINE__);
      }

      return *mGMImpl->mApplication;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtABC::Application& GameManager::GetApplication() const
   {
      if (mGMImpl->mApplication == NULL)
      {
         throw dtGame::GeneralGameManagerException(
         "No Application was ever assigned to the GameManager.", __FILE__, __LINE__);
      }

      return *mGMImpl->mApplication;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetApplication(dtABC::Application& application)
   {
      mGMImpl->mApplication = &application;
      std::string value;
      value = mGMImpl->mApplication->GetConfigPropertyValue(CONFIG_STATISTICS_INTERVAL, "0");
      int interval = dtUtil::ToType<int>(value);
      if (interval <= 0)
      {
         DebugStatisticsTurnOff(false, true);
      }
      else
      {
         value = mGMImpl->mApplication->GetConfigPropertyValue(CONFIG_STATISTICS_TO_CONSOLE, "true");
         bool toConsole = dtUtil::ToType<bool>(value);
         value = mGMImpl->mApplication->GetConfigPropertyValue(CONFIG_STATISTICS_OUTPUT_FILE, "");
         if (value.empty())
         {
            DebugStatisticsTurnOn(true, true, interval, toConsole);
         }
         else
         {
            DebugStatisticsTurnOn(true, true, interval, toConsole, value);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GameManager::GetCurrentMap() const
   {
      static std::string emptyString;
      if (mGMImpl->mLoadedMaps.empty())
      {
         return emptyString;
      }
      return mGMImpl->mLoadedMaps[0];
   }

   ///////////////////////////////////////////////////////////////////////////////
   const GameManager::NameVector& GameManager::GetCurrentMapSet() const
   {
      return mGMImpl->mLoadedMaps;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Scene& GameManager::GetScene() { return *mGMImpl->mScene; }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::Scene& GameManager::GetScene() const { return *mGMImpl->mScene; }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetScene(dtCore::Scene& newScene) { mGMImpl->mScene = &newScene; }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::OnMessage(MessageData* data)
   {
      if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "GM Starting delta message \"" + data->message + "\"");
      }

      if (data->message == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
      {
         double* timeChange = (double*)data->userData;
         PostEventTraversal(timeChange[0], timeChange[1]);
      }
      else if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
      {
         double* timeChange = (double*)data->userData;
         PreFrame(timeChange[0], timeChange[1]);
      }
      else if (data->message == dtCore::System::MESSAGE_FRAME_SYNCH)
      {
         double* timeChange = (double*)data->userData;
         FrameSynch(timeChange[0], timeChange[1]);
      }
      else if (data->message == dtCore::System::MESSAGE_POST_FRAME)
      {
         double* timeChange = (double*)data->userData;
         PostFrame(timeChange[0], timeChange[1]);
      }
      else if (data->message == dtCore::System::MESSAGE_PAUSE_START)
      {
         SendMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_PAUSED));
      }
      else if (data->message == dtCore::System::MESSAGE_PAUSE_END)
      {
         SendMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_RESUMED));
      }
      else if (data->message == dtCore::System::MESSAGE_PAUSE)
      {
         double* timeChange = (double*)data->userData;
         PreFrame(0.0, *timeChange);
      }

      if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG,__FUNCTION__, __LINE__,
                  "GM Finishing delta message \"" + data->message + "\"");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SendNetworkMessage(const Message& message)
   {
      mGMImpl->mSendNetworkMessageQueue.push(dtCore::RefPtr<const Message>(&message));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SendMessage(const Message& message)
   {
      mGMImpl->mSendMessageQueue.push(dtCore::RefPtr<const Message>(&message));
   }

   ///////////////////////////////////////////////////////////////////////////////
   float GameManager::GetTimeScale() const
   {
      return dtCore::System::GetInstance().GetTimeScale();
   }

   ///////////////////////////////////////////////////////////////////////////////
   double GameManager::GetSimulationTime() const
   {
      return dtCore::System::GetInstance().GetSimulationTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   double GameManager::GetSimTimeSinceStartup() const
   {
      return dtCore::System::GetInstance().GetSimTimeSinceStartup();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Timer_t GameManager::GetSimulationClockTime() const
   {
      return dtCore::System::GetInstance().GetSimulationClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Timer_t GameManager::GetRealClockTime() const
   {
      return dtCore::System::GetInstance().GetRealClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ChangeTimeSettings(double newTime, float newTimeScale, const dtCore::Timer_t& newClockTime)
   {
      dtCore::System::GetInstance().SetSimulationClockTime(newClockTime);
      dtCore::System::GetInstance().SetSimulationTime(newTime);
      dtCore::System::GetInstance().SetTimeScale(newTimeScale);

      dtCore::RefPtr<Message> timeChangeMsg = GetMessageFactory().CreateMessage(MessageType::INFO_TIME_CHANGED);
      TimeChangeMessage* tcm = static_cast<TimeChangeMessage*>(timeChangeMsg.get());
      tcm->SetSimulationTime(newTime);
      tcm->SetSimulationClockTime(newClockTime);
      tcm->SetTimeScale(newTimeScale);
      SendMessage(*timeChangeMsg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameManager::IsPaused() const
   {
      return dtCore::System::GetInstance().GetPause();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetPaused(bool pause)
   {
      dtCore::System::GetInstance().SetPause(pause);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PopulateTickMessage(TickMessage& tickMessage,
         double deltaSimTime, double deltaRealTime, double simulationTime)
   {
      tickMessage.SetDeltaSimTime(float(deltaSimTime));
      tickMessage.SetDeltaRealTime(float(deltaRealTime));
      tickMessage.SetSimTimeScale(GetTimeScale());
      tickMessage.SetDestination(&GetMachineInfo());
      tickMessage.SetSimulationTime(simulationTime);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PostEventTraversal(double deltaSimTime, double deltaRealTime)
   {
      double simulationTime = dtCore::System::GetInstance().GetSimulationTime();

      dtCore::RefPtr<SystemMessage> postEventTraversal;
      GetMessageFactory().CreateMessage(MessageType::SYSTEM_POST_EVENT_TRAVERSAL, postEventTraversal);
      PopulateTickMessage(*postEventTraversal, deltaSimTime, deltaRealTime, simulationTime);

      try
      {
         DoSendMessage(*postEventTraversal);
      }
      catch (const GMShutdownException&)
      {
         // Do nothing, just exit.
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FrameSynch(double deltaSimTime, double deltaRealTime)
   {
      double simulationTime = dtCore::System::GetInstance().GetSimulationTime();

      dtCore::RefPtr<SystemMessage> frameSynch;
      GetMessageFactory().CreateMessage(MessageType::SYSTEM_FRAME_SYNCH, frameSynch);
      PopulateTickMessage(*frameSynch, deltaSimTime, deltaRealTime, simulationTime);

      try
      {
         DoSendMessage(*frameSynch);
      }
      catch (const GMShutdownException&)
      {
         // Do nothing, just exit.
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PreFrame(double deltaSimTime, double deltaRealTime)
   {
      try
      {
         // information used to track statistics over a fragment of time (ex 30 seconds)
         dtCore::Timer_t frameTickStart = mGMImpl->mGMStatistics.mStatsTickClock.Tick();
         //frameTickStart = mGMImpl->mGMStatistics.mStatsTickClock.Tick();

         DoSendNetworkMessages();

         if (mGMImpl->mMapChangeStateData.valid())
         {
            const MapChangeStateData::MapChangeState* pPrevState = &mGMImpl->mMapChangeStateData->GetCurrentState();
            mGMImpl->mMapChangeStateData->ContinueMapChange();

            // Update mLoadedMaps only when a Map Change takes place.
            // This check is needed to keep the name vec consistent, as single maps may be loaded/unloaded
            // without changing the whole set.
            if ((*pPrevState == MapChangeStateData::MapChangeState::LOAD || *pPrevState == MapChangeStateData::MapChangeState::UNLOAD) &&
               mGMImpl->mMapChangeStateData->GetCurrentState() == MapChangeStateData::MapChangeState::IDLE)
            {
               mGMImpl->mLoadedMaps = mGMImpl->mMapChangeStateData->GetNewMapNames();
            }
         }

         double simulationTime = dtCore::System::GetInstance().GetSimulationTime();

         // Send out Tick Local and process all responses.
         dtCore::RefPtr<TickMessage> tick;
         GetMessageFactory().CreateMessage(MessageType::TICK_LOCAL, tick);
         PopulateTickMessage(*tick, deltaSimTime, deltaRealTime, simulationTime);
         SendMessage(*tick);
         mGMImpl->ProcessTimers(*this, mGMImpl->mRealTimeTimers, GetRealClockTime());
         mGMImpl->ProcessTimers(*this, mGMImpl->mSimulationTimers, dtCore::Timer_t(GetSimTimeSinceStartup() * 1000000.0));
         DoSendMessages();

         // The tick remote comes after ALL responses to Tick Local
         dtCore::RefPtr<TickMessage> tickRemote;
         GetMessageFactory().CreateMessage(MessageType::TICK_REMOTE, tickRemote);
         PopulateTickMessage(*tickRemote, deltaSimTime, deltaRealTime, simulationTime);
         SendMessage(*tickRemote);

         do
         {
            // Process all the delete messages if actors delete other actors.
            DoSendMessages();
         }while (!RemoveDeletedActors());

         DoSendMessages();

         dtCore::RefPtr<TickMessage> tickEnd;
         GetMessageFactory().CreateMessage(MessageType::TICK_END_OF_FRAME, tickEnd);
         PopulateTickMessage(*tickEnd, deltaSimTime, deltaRealTime, simulationTime);

         DoSendMessageToComponents(*tickEnd, false);

         // End the stats for this frame.
         mGMImpl->mGMStatistics.FragmentTimeDump(frameTickStart, *this, mGMImpl->mLogger);
      }
      catch (const GMShutdownException&)
      {
         // Do nothing, just exit.
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameManager::RemoveDeletedActors()
   {
      // DELETE ACTORS
      // IT IS CRUCIAL TO SAVE OFF THE SIZE BECAUSE ACTORS CAN DELETE OTHER ACTORS IN THE OnRemovedFromWorld
      // AND THEIR INFO_ACTOR_DELETED MESSAGES MUST BE HANDLED BEFORE ACTUALLY DELETING THEM.
      // This means that the function can only remove the actors that are in the vector at the time the function starts
      // even though the vector can grow during the execution.
      unsigned deleteSize = mGMImpl->mDeleteList.size();
      for (unsigned int i = 0; i < deleteSize; ++i)
      {
         GameActorProxy& gameActorProxy = *mGMImpl->mDeleteList[i];

         // Notify the Game Actor that it was removed from the world.
         // It could listen for the ACTOR_DELETE_MESSAGE instead.
         if (gameActorProxy.GetGameManager() == NULL)
         {
            LOG_WARNING("Found an actor with a null GM before deletion is complete.  Reassigning temporarily before completing delete.");
            gameActorProxy.SetGameManager(this);
         }

         try
         {
            gameActorProxy.InvokeRemovedFromWorld();
         }
         catch (dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR);
         }

         GMImpl::GameActorMap::iterator itor = mGMImpl->mGameActorProxyMap.find(gameActorProxy.GetId());

         dtCore::UniqueId id;
         if (itor != mGMImpl->mGameActorProxyMap.end())
         {
            id = itor->first;
            UnregisterAllMessageListenersForActor(gameActorProxy);
            mGMImpl->mGameActorProxyMap.erase(itor);
            mGMImpl->ReparentDanglingDrawables(*this, gameActorProxy.GetDrawable());
            gameActorProxy.SetParentActor(NULL);

            //mGMImpl->RemoveActorFromScene(*this, gameActorProxy);
            dtCore::DeltaDrawable* dd = gameActorProxy.GetDrawable();
            if ( dd != NULL )
            {
               dd->Emancipate();
            }
            mGMImpl->ClearTimersForActor(mGMImpl->mSimulationTimers, gameActorProxy);
            mGMImpl->ClearTimersForActor(mGMImpl->mRealTimeTimers, gameActorProxy);
         }

         gameActorProxy.SetGameManager(NULL);
      }

      mGMImpl->mDeleteList.erase(mGMImpl->mDeleteList.begin(), mGMImpl->mDeleteList.begin() + deleteSize);
      return mGMImpl->mDeleteList.empty();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DoSendNetworkMessages()
   {
      // SEND MESSAGES - Forward Send Messages to all components (no actors)
      while (!mGMImpl->mSendNetworkMessageQueue.empty())
      {
         mGMImpl->mGMStatistics.mStatsNumSendNetworkMessages += 1;

         if (!mGMImpl->mSendNetworkMessageQueue.front().valid())
         {
            mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Message in send to network queue is NULL.  Something is majorly wrong with the GameManager.");
            continue;
         }

         DoSendMessageToComponents(*mGMImpl->mSendNetworkMessageQueue.front(), true);
         mGMImpl->mSendNetworkMessageQueue.pop();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DoSendMessageToComponents(const Message& message, bool toNetwork)
   {
      //statistics stuff.
      bool logComponents = mGMImpl->mGMStatistics.ShouldWeLogComponents();
      dtCore::Timer_t frameTickStartCurrent(0);
      bool isATickLocalMessage = (message.GetMessageType() == MessageType::TICK_LOCAL);

      // Components get messages first
      GMImpl::GMComponentContainer::iterator compItr = mGMImpl->mComponentList.begin();
      while (compItr != mGMImpl->mComponentList.end())
      {
         if (mGMImpl->mShuttingDown)
         {
            throw GMShutdownException();
         }

         if (compItr->valid() == false) //set from a previous call to RemoveComponent()
         {
            //Erase this value from the container and move on. Iterator gets pointed to next in container.
            compItr = mGMImpl->mComponentList.erase(compItr);
            continue;
         }

         // Statistics information
         if (logComponents)
         {
            frameTickStartCurrent = mGMImpl->mGMStatistics.mStatsTickClock.Tick();
         }

         //RefPtr in case it get deleted during a Message. We need to hang onto it for a bit.
         dtCore::RefPtr<GMComponent>& component = *compItr;

         if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Sending Message Type \"" + message.GetMessageType().GetName() + "\" to GMComponent \"" +
               component->GetName() + "\"");
         }

         try
         {
            if (toNetwork)
            {
               component->DispatchNetworkMessage(message);
            }
            else
            {
               component->ProcessMessage(message);
            }
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR, *mGMImpl->mLogger);
         }
         catch (const std::exception& ex)
         {
            LOG_ERROR(std::string("Caught a std::exception derivative: ") + ex.what());
         }
         catch (...)
         {
            LOG_ERROR("Caught an unknown exception in the GM!  Continuing.");
         }

         // Statistics information
         if (logComponents)
         {
            double frameTickDelta =
               mGMImpl->mGMStatistics.mStatsTickClock.DeltaSec(frameTickStartCurrent,
                                                               mGMImpl->mGMStatistics.mStatsTickClock.Tick());

            mGMImpl->mGMStatistics.UpdateDebugStats(component->GetId(),
                                                    component->GetName(),
                                                    frameTickDelta, true, isATickLocalMessage);
         }

         ++compItr;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DoSendMessage(const Message& message)
   {
      DoSendMessageToComponents(message, false);

      // The component message sending checks for this internally
      // But if the last component were to call shutdown, then the code would read this point.
      if (mGMImpl->mShuttingDown)
      {
         throw GMShutdownException();
      }

      InvokeGlobalInvokables(message);

      // ABOUT ACTOR - The actor itself and others registered against a particular actor
      if (!message.GetAboutActorId().ToString().empty())
      {
         // if we have an about actor, first try to send it to the actor itself
         GameActorProxy* aboutActor = FindGameActorById(message.GetAboutActorId());
         if (aboutActor != NULL && !aboutActor->IsDeleted())
         {
            InvokeForActorInvokables(message, *aboutActor);
         }

         InvokeOtherActorInvokables(message);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DoSendMessages()
   {
      // PROCESS MESSAGES - Send all Process messages to components and interested actors
      while (!mGMImpl->mSendMessageQueue.empty())
      {
         mGMImpl->mGMStatistics.mStatsNumProcMessages += 1;

         // Forward to Components first
         dtCore::RefPtr<const Message> messageRef = mGMImpl->mSendMessageQueue.front();
         mGMImpl->mSendMessageQueue.pop();

         if (!messageRef.valid())
         {
            mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Message in send queue is NULL.  Something is majorly wrong with the GameManager.");
            continue;
         }

         const Message& message = *messageRef;
         DoSendMessage(message);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::InvokeGlobalInvokables(const Message& message)
   {
      // statistics stuff.
      const bool logActors = mGMImpl->mGMStatistics.ShouldWeLogActors();
      dtCore::Timer_t frameTickStartCurrent(0);
      const bool isATickLocalMessage = (message.GetMessageType() == MessageType::TICK_LOCAL);

      // GLOBAL INVOKABLES - Process it on globally registered invokables

      //find all matches of MessageType
      typedef std::pair<GMImpl::GlobalMessageListenerMap::iterator, GMImpl::GlobalMessageListenerMap::iterator> IterPair;
      IterPair msgTypeMatches = mGMImpl->mGlobalMessageListeners.equal_range(&message.GetMessageType());

      GMImpl::GlobalMessageListenerMap::iterator itor = msgTypeMatches.first;

      while (itor != msgTypeMatches.second)
      {
         GMImpl::ProxyInvokablePair& listener = itor->second;

         // hold onto the actor in a refptr so that the stats code
         // won't crash if the actor unregisters for the message.
         dtCore::RefPtr<GameActorProxy> listenerActorProxy = listener.first;

         if (listenerActorProxy.valid() == false)
         {
            //Erase this value from the container and move on.
            //Note the postfix operator to increment to the next item
            mGMImpl->mGlobalMessageListeners.erase(itor++);
            continue;
         }

         ++itor;

         Invokable* invokable = NULL;

         if (listenerActorProxy->IsInGM())
         {
            invokable = listenerActorProxy->GetInvokable(listener.second);
         }


         if (invokable != NULL)
         {
            // Statistics information
            if (logActors)
            {
               frameTickStartCurrent = mGMImpl->mGMStatistics.mStatsTickClock.Tick();
            }

            try
            {
               if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                           "Sending Message Type \"" + message.GetMessageType().GetName() + "\" to Actor \"" +
                           listenerActorProxy->GetName() + "\" of Type \"" + listenerActorProxy->GetActorType().GetFullName()
                           + "\"");
               }
               invokable->Invoke(message);
            }
            catch (const dtUtil::Exception& ex)
            {
               ex.LogException(dtUtil::Log::LOG_ERROR, *mGMImpl->mLogger);
            }

            // Statistics information
            if (logActors)
            {
               double frameTickDelta
               = mGMImpl->mGMStatistics.mStatsTickClock.DeltaSec(frameTickStartCurrent,
                                                        mGMImpl->mGMStatistics.mStatsTickClock.Tick());

               mGMImpl->mGMStatistics.UpdateDebugStats(listenerActorProxy->GetId(),
                                                       listenerActorProxy->GetName(),
                                                       frameTickDelta,
                                                       false, isATickLocalMessage);
            }
         }
         else
         {
            if (listenerActorProxy->IsInGM())
            {
               if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
               {
                  mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                      "Invokable named %s is registered as a listener, but "
                                      "Proxy %s does not have an invokable by that name.",
                                      listener.second.c_str(),
                                      listener.first->GetActorType().GetName().c_str());
               }
            }
            else
            {
               if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                  mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                      "Invokable named %s is registered as a listener, "
                                      "but Proxy %s is no longer in the GM and is probably "
                                      "being deleted.",
                                      listener.second.c_str(),
                                      listenerActorProxy->GetActorType().GetName().c_str());
               }
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::InvokeForActorInvokables(const Message& message, GameActorProxy& aboutActor)
   {
      // statistics stuff.
      bool logActors = mGMImpl->mGMStatistics.ShouldWeLogActors();
      dtCore::Timer_t frameTickStartCurrent(0);

      std::vector<dtGame::Invokable*> aboutActorInvokables;

      aboutActor.GetMessageHandlers(message.GetMessageType(), aboutActorInvokables);

      std::vector<dtGame::Invokable*>::iterator i, iend;
      i = aboutActorInvokables.begin();
      iend = aboutActorInvokables.end();

      for (;i != iend; ++i)
      {
         // Statistics information
         if (logActors)
         {
            frameTickStartCurrent = mGMImpl->mGMStatistics.mStatsTickClock.Tick();
         }

         try
         {
            if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
                mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG,__FUNCTION__, __LINE__,
                         "Sending Message Type \"" + message.GetMessageType().GetName() + "\" to Actor \"" +
                         aboutActor.GetName() + "\" of Type \"" + aboutActor.GetActorType().GetFullName()
                         + "\"");
            }
            (*i)->Invoke(message);
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR, *mGMImpl->mLogger);
         }

         // Statistics information
         if (logActors)
         {
            double frameTickDelta =
                     mGMImpl->mGMStatistics.mStatsTickClock.DeltaSec(frameTickStartCurrent,
                                                            mGMImpl->mGMStatistics.mStatsTickClock.Tick());

            mGMImpl->mGMStatistics.UpdateDebugStats(aboutActor.GetId(), aboutActor.GetName(),
                                           frameTickDelta, false, false);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::InvokeOtherActorInvokables(const Message& message)
   {
      // statistics stuff.
      bool logActors = mGMImpl->mGMStatistics.ShouldWeLogActors();
      dtCore::Timer_t frameTickStartCurrent(0);

      // next, sent it to all actors listening to that actor for that message type.
      // TODO - This should be refactored like we did for the Global
      // Invokables a few lines up. It should work with the map directly instead of filling lists repeatedly
      std::vector<std::pair<GameActorProxy*, std::string > > toFill;
      GetRegistrantsForMessagesAboutActor(message.GetMessageType(), message.GetAboutActorId(), toFill);

      std::vector<std::pair<GameActorProxy*, std::string > >::iterator i, iend;
      i = toFill.begin();
      iend = toFill.end();
      for (;i != iend; ++i)
      {
         std::pair<GameActorProxy*, std::string >& listener = *i;
         GameActorProxy& currentProxy = *listener.first;
         Invokable* invokable = NULL;

         /// Don't want to invoke
         if (currentProxy.IsInGM())
         {
            invokable = currentProxy.GetInvokable(listener.second);
         }

         if (invokable != NULL)
         {
            // Statistics information
            if (logActors)
            {
               frameTickStartCurrent = mGMImpl->mGMStatistics.mStatsTickClock.Tick();
            }

            try
            {
               if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               {
                   mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                            "Sending Message Type \"" + message.GetMessageType().GetName() + "\" to Actor \"" +
                            currentProxy.GetName() + "\" of Type \"" + currentProxy.GetActorType().GetFullName()
                            + "\"");
               }
               invokable->Invoke(message);
            }
            catch (const dtUtil::Exception& ex)
            {
               ex.LogException(dtUtil::Log::LOG_ERROR, *mGMImpl->mLogger);
            }

            if (logActors)
            {
               double frameTickDelta =
                  mGMImpl->mGMStatistics.mStatsTickClock.DeltaSec(frameTickStartCurrent,
                                                         mGMImpl->mGMStatistics.mStatsTickClock.Tick());
               mGMImpl->mGMStatistics.UpdateDebugStats(currentProxy.GetId(), currentProxy.GetName(),
                                              frameTickDelta, false, false);
            }
         }
         else if (currentProxy.IsInGM())
         {
            if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
            {
               mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                   "Invokable named %s is registered as a listener, but Proxy %s does not have an invokable by that name.",
                                   listener.second.c_str(), currentProxy.GetActorType().GetName().c_str());
            }
         }
         else
         {
            if (mGMImpl->mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Invokable named %s is registered as a listener, but Proxy %s is no longer in the GM and is probably being deleted.",
                                   listener.second.c_str(), currentProxy.GetActorType().GetName().c_str());
            }
         }
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PostFrame(double deltaSimTime, double deltaRealTime)
   {
      double simulationTime = dtCore::System::GetInstance().GetSimulationTime();

      dtCore::RefPtr<SystemMessage> postFrame;
      GetMessageFactory().CreateMessage(MessageType::SYSTEM_POST_FRAME, postFrame);
      PopulateTickMessage(*postFrame, deltaSimTime, deltaRealTime, simulationTime);

      try
      {
         DoSendMessage(*postFrame);
      }
      catch (const GMShutdownException&)
      {
         // Do nothing, just exit.
      }
   }

   //////////////////////////////////////////////////////////////////////////
   //local function used to compare priorities of GMComponents
   bool CompareComponentPriority(const dtCore::RefPtr<GMComponent>& first,
                                 const dtCore::RefPtr<GMComponent>& second)
   {
      if (first.valid() && second.valid())
      {
         //the lower the value, the higher the priority
         return (first->GetComponentPriority().GetOrderId() < second->GetComponentPriority().GetOrderId());
      }
      else
      {
         return first < second;//compare pointers?  Not sure we care what happens here
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddComponent(GMComponent& component, GameManager::ComponentPriority& priority)
   {
      if (GetComponentByName(component.GetName()) != NULL)
      {
         std::string errorText = "A component was already registered with the Game Manager with the name: " + component.GetName();
         LOG_ERROR(errorText);
         throw dtGame::InvalidParameterException(errorText, __FILE__, __LINE__);
      }

      component.SetGameManager(this);
      component.SetComponentPriority(priority);

      mGMImpl->mComponentList.push_back(dtCore::RefPtr<GMComponent>(&component)); //vector, list

      // we sort the items by priority so that components of higher priority get messages first.
      mGMImpl->mComponentList.sort(CompareComponentPriority);

      // notify the component that it was added to the GM
      component.OnAddedToGM();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RemoveComponent(GMComponent& component)
   {
      GMImpl::GMComponentContainer::iterator found =
         std::find(mGMImpl->mComponentList.begin(), mGMImpl->mComponentList.end(), &component);

      if (found != mGMImpl->mComponentList.end())
      {
         (*found)->OnRemovedFromGM();
         (*found)->SetGameManager(NULL);
         (*found) = NULL; //RefPtr will be erased from the container later on
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllComponents(std::vector<GMComponent*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mGMImpl->mComponentList.size());

      GMImpl::GMComponentContainer::iterator compItr = mGMImpl->mComponentList.begin();

      while (compItr != mGMImpl->mComponentList.end())
      {
         if (compItr->valid())
         {
            toFill.push_back(compItr->get());
         }
         ++compItr;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllComponents(std::vector<const GMComponent*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mGMImpl->mComponentList.size());

      GMImpl::GMComponentContainer::const_iterator compItr = mGMImpl->mComponentList.begin();

      while (compItr != mGMImpl->mComponentList.end())
      {
         if (compItr->valid())
         {
            toFill.push_back(compItr->get());
         }
         ++compItr;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   //little class used to find valid GMComponent by name
   struct ComponentNameFind : public std::unary_function<dtCore::RefPtr<GMComponent>, bool>
   {
      ComponentNameFind(const std::string& name):mName(name)
      {}

      bool operator()(dtCore::RefPtr<GMComponent> elem) const
      {
         if (elem.valid() && elem->GetName() == mName) {return true;}
         else {return false;}
      }

      std::string mName;
   };

   ///////////////////////////////////////////////////////////////////////////////
   GMComponent* GameManager::GetComponentByName(const std::string& name)
   {
      GMImpl::GMComponentContainer::iterator found = std::find_if(mGMImpl->mComponentList.begin(),
                                                          mGMImpl->mComponentList.end(),
                                                          ComponentNameFind(name));
      if (found != mGMImpl->mComponentList.end())
      {
         return *found;
      }
      else
      {
         return NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const GMComponent* GameManager::GetComponentByName(const std::string& name) const
   {
      GMImpl::GMComponentContainer::const_iterator found = std::find_if(mGMImpl->mComponentList.begin(),
                                                                mGMImpl->mComponentList.end(),
                                                                ComponentNameFind(name));
      if (found != mGMImpl->mComponentList.end())
      {
         return *found;
      }
      else
      {
         return NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtGame::GameActorProxy> GameManager::CreateRemoteActor(const dtCore::ActorType& actorType)
   {
      dtCore::RefPtr<dtGame::GameActorProxy> result;
      CreateActor(actorType, result);

      if (result.valid())
      {
         result->SetRemote(true);
      }
      else
      {
         throw dtGame::InvalidParameterException( "The actor type \""
            + actorType.GetFullName() + "\" is invalid because it is not a game actor type."
            , __FILE__, __LINE__);
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::BaseActorObject> GameManager::CreateActor(const dtCore::ActorType& actorType)
   {
      try
      {
         dtCore::RefPtr<dtCore::BaseActorObject> ap = dtCore::ActorFactory::GetInstance().CreateActor(actorType).get();
         if (ap->IsInstanceOf("dtGame::GameActor"))
         {
            dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
            if (gap != NULL)
            {
               gap->SetGameManager(this);
            }
            else
            {
               throw dtGame::GeneralGameManagerException(
               "ERROR: Actor has the type of a GameActor, but casting it to a GameActorProxy failed.", __FILE__, __LINE__);
            }
         }

         return ap;
      }
      catch (const dtUtil::Exception& ex)
      {
         mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error creating actor: %s", ex.What().c_str());
         throw;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::BaseActorObject> GameManager::CreateActor(const std::string& category, const std::string& name)
   {
      dtCore::RefPtr<const dtCore::ActorType> type = FindActorType(category, name);
      if (!type.valid())
      {
         throw dtGame::UnknownActorTypeException(
            "No actor exists of the specified name and category", __FILE__, __LINE__);
      }

      return CreateActor(*type);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActor(dtCore::BaseActorObject& actor)
   {
      if (actor.GetId().ToString().empty())
      {
         throw dtGame::InvalidActorStateException(
            "Actors may not be added the GM with an empty unique id", __FILE__, __LINE__);
      }

      if (actor.IsSystemComponent())
      {
         dtGame::GMComponent* sysComp = dynamic_cast<dtGame::GMComponent*>(&actor);
         if (sysComp != NULL)
         {
            AddComponent(*sysComp, sysComp->GetComponentPriority());
         }
         else
         {
            throw dtGame::InvalidActorStateException(
               std::string("Actor has the type of a SystemComponent, but casting it to a GMComponent failed.  ") +
               "Actor \""+actor.GetName()+"\" of type \"" + actor.GetActorType().GetFullName() + "\" will not be added to the game manager.",
               __FILE__, __LINE__);

         }
      }
      else if (actor.IsGameActor())
      {
         GameActorProxy* gameActor = dynamic_cast<GameActorProxy*>(&actor);
         if (gameActor != NULL)
         {
            if (gameActor->GetInitialOwnership() == GameActorProxy::Ownership::PROTOTYPE)
            {
               AddActorAsAPrototype(*gameActor);
            }
            else
            {
               bool isRemote = gameActor->IsRemote();
               bool shouldPublish = !isRemote && gameActor->GetInitialOwnership() == GameActorProxy::Ownership::SERVER_PUBLISHED;

               bool isClient = GetGMSettings().GetClientRole();
               bool isServer = GetGMSettings().GetServerRole();
               bool shouldAddActor = isRemote ||
                  (isClient && gameActor->GetInitialOwnership() == GameActorProxy::Ownership::CLIENT_LOCAL)
                  || ((isClient || isServer) && gameActor->GetInitialOwnership() == GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL)
                  || (isServer && gameActor->GetInitialOwnership() == GameActorProxy::Ownership::SERVER_PUBLISHED)
                  || (isServer && gameActor->GetInitialOwnership() == GameActorProxy::Ownership::SERVER_LOCAL);

               if (shouldAddActor)
               {
                  AddActor(*gameActor, isRemote, shouldPublish);
               }
            }
         }
         else
         {
            throw dtGame::InvalidActorStateException(
               std::string("Actor has the type of a GameActor, but casting it to a GameActorProxy failed.  ") +
               "Actor \""+actor.GetName()+"\" of type \"" + actor.GetActorType().GetFullName() + "\" will not be added to the game manager.",
               __FILE__, __LINE__);
         }

      }
      else
      {
         mGMImpl->AddActorToScene(actor);

         mGMImpl->mBaseActorObjectMap.insert(std::make_pair(actor.GetId(), &actor));
      }
   }


   //////////////////////////////////////////////////////////////////////////////
   void GameManager::BeginBatchAdd()
   {
      if (!mGMImpl->mBatchData.valid())
      {
         mGMImpl->mBatchData = new BatchData;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameManager::CompleteBatchAdd()
   {
      if (mGMImpl->mBatchData.valid())
      {
         // add actor calls made from this point will completely run so actors adding actors in their init will see them initialize all the way.
         mGMImpl->mBatchData->mOkToAddActors = true;

         std::vector<dtCore::RefPtr<GameActorProxy> >::iterator i, iend;
         i = mGMImpl->mBatchData->mBatchItems.begin();
         iend = mGMImpl->mBatchData->mBatchItems.end();
         for (unsigned i = 0; i < mGMImpl->mBatchData->mBatchItems.size(); ++i)
         {
            try
            {
               mGMImpl->AddActorToWorld(*this, *mGMImpl->mBatchData->mBatchItems[i]);
            }
            catch (const dtUtil::Exception& ex)
            {
               // Actors can just decide to not be added by throwing an exception.  If it's an error, the actor
               // should log it as such, but here it's only a warning.
               ex.LogException(dtUtil::Log::LOG_WARNING, *mGMImpl->mLogger);
            }
         }
      }
      mGMImpl->mBatchData = NULL;
   }


   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActor(GameActorProxy& actorRoot, bool isRemote, bool publish)
   {
      if (GetGMSettings().GetEditorMode())
      {
         isRemote = true;
         publish = false;
      }

      GameActorProxy::iterator i, iend;
      i = actorRoot.begin();
      iend = actorRoot.end();
      for (;i != iend; ++i)
      {
         GameActorProxy& actor = *i->value;
         if (actor.IsInGM())
            return;

         if (actor.GetId().IsNull())
         {
            throw dtGame::InvalidActorStateException(
               "Actors may not be added the GM with an empty unique id", __FILE__, __LINE__);
         }

         // Fail early here so that it doesn't fail is PublishActor and need to wait a tick to
         // clean up the actor.
         if (publish && isRemote)
         {
            throw dtGame::ActorIsRemoteException( "A remote game actor may not be published", __FILE__, __LINE__);
         }

         if (actor.GetInitialOwnership() == dtGame::GameActorProxy::Ownership::PROTOTYPE)
         {
            // don't leave it as prototype.
            actor.SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_LOCAL);
         }

         actor.SetGameManager(this);
         actor.SetRemote(isRemote);

         bool envChanged = mGMImpl->AddActorToScene(actor);

         mGMImpl->mGameActorProxyMap.insert(std::make_pair(actor.GetId(), &actor));
         if (envChanged) mGMImpl->SendEnvironmentChangedMessage(*this, mGMImpl->mEnvironment.get());


         // Remote actors are normally created in response to a create message, so sending another is silly.
         // Also, this doen't currently send messages when loading a map, so check here for that state.
         if (!isRemote && mGMImpl->mMapChangeStateData->GetCurrentState() == MapChangeStateData::MapChangeState::IDLE)
         {
            dtCore::RefPtr<Message> msg = mGMImpl->mFactory.CreateMessage(MessageType::INFO_ACTOR_CREATED);
            actor.PopulateActorUpdate(static_cast<ActorUpdateMessage&>(*msg));
            SendMessage(*msg);
         }


         // Save the publish value so we don't have to pass it around.
         actor.SetPublished(publish);
         actor.SetDeleted(false);

         if (!mGMImpl->mBatchData.valid() || mGMImpl->mBatchData->mOkToAddActors )
         {
            mGMImpl->AddActorToWorld(*this, actor);
         }
         else
         {
            // In a batch, exceptions are just logged, though the actors are still deleted.
            mGMImpl->mBatchData->mBatchItems.push_back(&actor);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActorAsAPrototype(GameActorProxy& gameActorProxy)
   {
      gameActorProxy.SetGameManager(this);
      mGMImpl->mPrototypeActors.insert(std::make_pair(gameActorProxy.GetId(), &gameActorProxy));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::CreateActorsFromPrefab(const dtCore::ResourceDescriptor& rd, dtCore::ActorRefPtrVector& actorsOut, bool isRemote)
   {
      dtCore::Project::GetInstance().LoadPrefab(rd, actorsOut);
      dtCore::ActorRefPtrVector::iterator i, iend;
      i = actorsOut.begin();
      iend = actorsOut.end();
      for (; i != iend; ++i)
      {
         if ((*i)->IsGameActor())
         {
            dtGame::GameActorProxy* gap = static_cast<dtGame::GameActorProxy*>(i->get());
            gap->SetRemote(isRemote);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::BaseActorObject> GameManager::CreateActorFromPrototype(const dtCore::UniqueId& uniqueID, bool isRemote /*= false*/)
   {
      dtCore::BaseActorObject* ourObject = FindPrototypeByID(uniqueID);
      if (ourObject != NULL)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> temp = ourObject->Clone().get();
         temp->SetPrototype(ourObject);
         dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(temp.get());
         if (gap != NULL)
         {
            gap->SetGameManager(this);
            gap->SetRemote(isRemote);
         }
         return temp;
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetEnvironmentActor(IEnvGameActorProxy* envActor)
   {
      if (envActor != NULL)
      {
         // Already set? No-Op
         if (mGMImpl->mEnvironment == envActor)
         {
            LOG_WARNING("Tried to set the internal environment actor pointer to itself");
            return;
         }

         dtCore::RefPtr<IEnvGameActorProxy> oldProxy = mGMImpl->mEnvironment;

         if (mGMImpl->mEnvironment.valid())
         {
            // Internal pointer is valid, so we must remove all of its children and Emancipate them
            // Since the pointer is still valid, the children still think they have a valid
            // parent and otherwise the call to AddChild will fail
            DeleteActor(*mGMImpl->mEnvironment);
         }

         IEnvGameActor* ea = NULL;
         envActor->GetDrawable(ea);

         // Internal pointer is not valid, we are setting a new environment
         // We need to remove all the drawables from the scene and add them
         // to the new environment
         dtCore::ActorPtrVector actors;
         GetAllNonGameActors(actors);
         mGMImpl->mScene->RemoveAllDrawables();
         mGMImpl->mScene->UseSceneLight(true);
         size_t actorsSize = actors.size();
         for (size_t i = 0; i < actorsSize; i++)
         {
            dtCore::BaseActorObject* curActor = actors[i];
            dtCore::DeltaDrawable* dd = curActor->GetDrawable();
            if (dd != NULL && dd->GetParent() == NULL)
            {
               if (ea != NULL)
               {
                  ea->AddActor(*dd);
               }
               else
               {
                  ea->AddChild(dd);
               }
            }
         }

         std::vector<GameActorProxy*> gameActors;
         GetAllGameActors(gameActors);
         actorsSize = gameActors.size();
         for (size_t i = 0; i < actorsSize; i++)
         {
            GameActorProxy* curActor = gameActors[i];
            if (curActor != oldProxy.get() && curActor->GetParentActor() == NULL)
            {
               curActor->SetParentActor(envActor);
            }
         }

         mGMImpl->mEnvironment = envActor;
         mGMImpl->mEnvironment->SetGameManager(this);
         AddActor(*mGMImpl->mEnvironment, false, false);
      }
      else
      {
         // Setting current valid env actor to NULL
         if (mGMImpl->mEnvironment != NULL)
         {
            DeleteActor(*mGMImpl->mEnvironment);
         }
         // else, Currently NULL internal env actor being set to NULL. No-Op
      }
   }

   //////////////////////////////////////////////////////////////////////////
   IEnvGameActorProxy* GameManager::GetEnvironmentActor()
   {
      return mGMImpl->mEnvironment.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PublishActor(GameActorProxy& gameActorProxy)
   {
      GMImpl::GameActorMap::iterator itor = mGMImpl->mGameActorProxyMap.find(gameActorProxy.GetId());

      if (itor == mGMImpl->mGameActorProxyMap.end())
      {
         throw dtGame::InvalidActorStateException(
            "A GameActor may only be published if it's added to the GameManager as a game actor.", __FILE__, __LINE__);
      }

      if (gameActorProxy.IsRemote())
      {
         throw dtGame::ActorIsRemoteException( "A remote game actor may not be published", __FILE__, __LINE__);
      }

      gameActorProxy.SetPublished(true);
      dtCore::RefPtr<Message> msg = mGMImpl->mFactory.CreateMessage(MessageType::INFO_ACTOR_PUBLISHED);
      msg->SetDestination(&GetMachineInfo());
      msg->SetAboutActorId(gameActorProxy.GetId());
      msg->SetSendingActorId(gameActorProxy.GetId());
      SendMessage(*msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SwitchActorToLocal(GameActorProxy& gameActorProxy, bool publish)
   {
      SwitchActorToLocalOrRemote(gameActorProxy, true, publish);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SwitchActorToRemote(GameActorProxy& gameActorProxy)
   {
      SwitchActorToLocalOrRemote(gameActorProxy, false, false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SwitchActorToLocalOrRemote(GameActorProxy& gameActorProxy, bool local, bool publish)
   {
      if (gameActorProxy.IsInGM() && gameActorProxy.GetGameManager() == this)
      {
         gameActorProxy.SetIsInGM(false);
         try
         {
            gameActorProxy.InvokeRemovedFromWorld();
         }
         catch (dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR);
         }

         UnregisterAllMessageListenersForActor(gameActorProxy);
         mGMImpl->ClearTimersForActor(mGMImpl->mSimulationTimers, gameActorProxy);
         mGMImpl->ClearTimersForActor(mGMImpl->mRealTimeTimers, gameActorProxy);

         gameActorProxy.SetRemote(!local);

         gameActorProxy.SetIsInGM(true);

         try
         {
            // If publishing fails. we need to delete the actor as well.
            if (publish)
            {
               PublishActor(gameActorProxy);
            }
            else
            {
               gameActorProxy.SetPublished(false);
            }

            gameActorProxy.InvokeEnteredWorld();
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR, *mGMImpl->mLogger);
            DeleteActor(gameActorProxy);
            throw;
         }
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteActor(const dtCore::UniqueId& id)
   {
      GMImpl::GameActorMap::iterator itor = mGMImpl->mGameActorProxyMap.find(id);
      if (itor == mGMImpl->mGameActorProxyMap.end())
      {
         // it's not in the game manager as a game actor, maybe it's in there
         // as a regular actor
         GMImpl::ActorMap::iterator itor = mGMImpl->mBaseActorObjectMap.find(id);

         if (itor != mGMImpl->mBaseActorObjectMap.end())
         {
            dtCore::DeltaDrawable* dd = itor->second->GetDrawable();
            if (dd != NULL)
            {
               //mGMImpl->RemoveActorFromScene(*this, *itor->second);
               dd->Emancipate();
               mGMImpl->ReparentDanglingDrawables(*this, dd);
               mGMImpl->mBaseActorObjectMap.erase(itor);
            }
         }
      }
      else
      {
         GameActorProxy& gameActorRoot = *itor->second;
         if (&gameActorRoot == mGMImpl->mEnvironment.get())
         {
            mGMImpl->mEnvironment = NULL;

            // First un hook the game actors and add them to the scene.  They have code to clean up from being in an environment actor.
            GameActorProxy::child_iterator i, iend;
            i = gameActorRoot.begin_child();
            iend = gameActorRoot.end_child();
            while (i != iend)
            {
               GameActorProxy::child_iterator cur = i;
               ++i;
               cur->value->SetParentActor(NULL);
               mGMImpl->AddActorToScene(*cur);
            }

            dtCore::DeltaDrawable::DrawableList drawables;
            // The environment actor is a special case where we don't want to do a cascading delete.
            // Is it THE environment actor?
            IEnvGameActorProxy* eap = dynamic_cast<IEnvGameActorProxy*>(&gameActorRoot);
            if (eap != NULL && mGMImpl->mScene->GetChildIndex(eap->GetDrawable()) != mGMImpl->mScene->GetNumberOfAddedDrawable())
            {

               // Next remove any other drawables and add them to the scene.
               IEnvGameActor* e = NULL;
               eap->GetDrawable(e);
               e->GetAllActors(drawables);
               e->RemoveAllActors();
            }
            else
            {
               dtCore::DeltaDrawable* dd = gameActorRoot.GetDrawable();
               if (dd != NULL)
               {
                  dd->GetChildren(drawables);
                  // Now that all the old actors are removed add them back to the scene
                  for (size_t i = 0; i < drawables.size(); ++i)
                  {
                     drawables[i]->Emancipate();
                     dtCore::BaseActorObject* actor = FindActorById(drawables[i]->GetUniqueId());
                     if (actor != NULL)
                     {
                        mGMImpl->AddActorToScene(*actor);
                     }
                  }
              }
            }

            mGMImpl->InternalMarkSingleActorForRemoval(*this, gameActorRoot);
         }
         else
         {
            GameActorProxy::reverse_iterator i, iend;
            i = gameActorRoot.rbegin();
            iend = gameActorRoot.rend();
            for (; i != iend; ++i)
            {
               GameActorProxy& gameActor = *i->value;
               mGMImpl->InternalMarkSingleActorForRemoval(*this, gameActor);
               // It will keep traversing Up beyond the actor that is the root here
               // even though rend() would sort of suggest it would stop there, it actually stops
               // at the root of the entire tree, so I have to short-circuit it.
               if (&gameActor == &gameActorRoot)
               {
                  break;
               }
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteActor(dtCore::BaseActorObject& actorProxy)
   {
      DeleteActor(actorProxy.GetId());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteAllActors(bool immediate)
   {
      // Clear all the timers first so the delete actor calls don't have to
      // iterate over the lists a bunch of times.  We have to clear this list anyway
      // to get rid of the timers not related to actors if no one has cleaned them up.
      mGMImpl->mRealTimeTimers.clear();
      mGMImpl->mSimulationTimers.clear();

      while (!mGMImpl->mBaseActorObjectMap.empty())
      {
         DeleteActor(*mGMImpl->mBaseActorObjectMap.begin()->second);
      }

      for (GMImpl::GameActorMap::iterator i = mGMImpl->mGameActorProxyMap.begin();
         i != mGMImpl->mGameActorProxyMap.end(); ++i)
      {
         DeleteActor(i->first);
      }

      if (immediate)
      {
         do
         {
            // Process all the delete messages if actors delete other actors.
            DoSendMessages();
         }
         while (!RemoveDeletedActors());
         // Other places this is done, another DoSend is called, but it's not necessary here because
         // this is just a helper function mainly for unit tests, and the messages sent now won't be related
         // to a new actor that is currently being deleted, but rather one that would be gone already anyway.
         // so just let it wait until the next frame.
      }

      DeleteAllPrototypes();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteAllPrototypes()
   {
      mGMImpl->mPrototypeActors.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeletePrototype(const dtCore::UniqueId& uniqueId)
   {
      GMImpl::GameActorMap::iterator itor = mGMImpl->mPrototypeActors.find(uniqueId);
      if (itor != mGMImpl->mPrototypeActors.end())
      {
         mGMImpl->mPrototypeActors.erase(itor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetUsedActorTypes(std::set<const dtCore::ActorType*>& toFill) const
   {
      toFill.clear();

      // spin through the actors and add all used actor types to the set, so that we don't
      // get duplicates.
      for (GMImpl::GameActorMap::const_iterator itor = mGMImpl->mGameActorProxyMap.begin();
           itor != mGMImpl->mGameActorProxyMap.end(); ++itor)
      {
         toFill.insert(&itor->second->GetActorType());
      }

      for (GMImpl::ActorMap::const_iterator itor = mGMImpl->mBaseActorObjectMap.begin();
           itor != mGMImpl->mBaseActorObjectMap.end(); ++itor)
      {
         toFill.insert(&itor->second->GetActorType());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   size_t GameManager::GetNumGameActors() const
   {
      return mGMImpl->mGameActorProxyMap.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllGameActors(std::vector<GameActorProxy*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mGMImpl->mGameActorProxyMap.size());

      GMImpl::GameActorMap::const_iterator itor;
      for (itor = mGMImpl->mGameActorProxyMap.begin(); itor != mGMImpl->mGameActorProxyMap.end(); ++itor)
      {
         toFill.push_back(itor->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllNonGameActors(dtCore::ActorPtrVector& toFill) const
   {
      toFill.clear();
      toFill.reserve(mGMImpl->mBaseActorObjectMap.size());

      GMImpl::ActorMap::const_iterator itor;
      for (itor = mGMImpl->mBaseActorObjectMap.begin(); itor != mGMImpl->mBaseActorObjectMap.end(); ++itor)
      {
         toFill.push_back(itor->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllActors(dtCore::ActorPtrVector& toFill) const
   {
      toFill.clear();
      toFill.reserve(mGMImpl->mGameActorProxyMap.size() + mGMImpl->mBaseActorObjectMap.size() + mGMImpl->mPrototypeActors.size());

      GMImpl::GameActorMap::const_iterator itor;
      for (itor = mGMImpl->mGameActorProxyMap.begin(); itor != mGMImpl->mGameActorProxyMap.end(); ++itor)
      {
         toFill.push_back(itor->second.get());
      }

      GMImpl::ActorMap::const_iterator iter;
      for (iter = mGMImpl->mBaseActorObjectMap.begin(); iter != mGMImpl->mBaseActorObjectMap.end(); ++iter)
      {
         toFill.push_back(iter->second.get());
      }

      //for (itor = mGMImpl->mPrototypeActors.begin(); itor != mGMImpl->mPrototypeActors.end(); ++itor)
      //{
      //   toFill.push_back(itor->second.get());
      //}
   }

   //////////////////////////////////////////////////////////////////////////
   size_t GameManager::GetNumAllActors() const
   {
      return mGMImpl->mGameActorProxyMap.size() + mGMImpl->mBaseActorObjectMap.size();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllPrototypes(dtCore::ActorPtrVector& toFill) const
   {
      toFill.clear();
      toFill.reserve(mGMImpl->mPrototypeActors.size());

      GMImpl::GameActorMap::const_iterator itor;
      for (itor = mGMImpl->mPrototypeActors.begin(); itor != mGMImpl->mPrototypeActors.end(); ++itor)
      {
         toFill.push_back(itor->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   class GMWildMatchSearchFunc
   {
   public:
      GMWildMatchSearchFunc(const std::string& name)
         : mName(name)
      {
      }

      bool operator()(dtCore::BaseActorObject& actor)
      {
         const std::string& name = actor.GetName();
         return dtUtil::Match(const_cast<char*>(mName.c_str()), const_cast<char*>(name.c_str()));
      }

      const std::string& mName;
   };

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByName(const std::string& name, dtCore::ActorPtrVector& toFill)
   {
      toFill.reserve(mGMImpl->mGameActorProxyMap.size() + mGMImpl->mBaseActorObjectMap.size());

      GMWildMatchSearchFunc searchFunc(name);
      FindActorsIf(searchFunc, toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////
   class GMTypeMatchSearchFunc
   {
   public:
      GMTypeMatchSearchFunc(const dtCore::ActorType& type)
         : mType(type)
      {
      }

      bool operator()(dtCore::BaseActorObject& actor)
      {
         return actor.GetActorType().InstanceOf(mType);
      }

      const dtCore::ActorType& mType;
   };

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByType(const dtCore::ActorType& type, dtCore::ActorPtrVector& toFill)
   {
      toFill.reserve(mGMImpl->mGameActorProxyMap.size() + mGMImpl->mBaseActorObjectMap.size());

      GMTypeMatchSearchFunc searchFunc(type);
      FindActorsIf(searchFunc, toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////
   class GMClassMatchSearchFunc
   {
   public:
      GMClassMatchSearchFunc(const std::string& type)
         : mType(type)
      {
      }

      bool operator()(dtCore::BaseActorObject& actor)
      {
         return actor.IsInstanceOf(mType);
      }

      const std::string& mType;
   };

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByClassName(const std::string& className,
      dtCore::ActorPtrVector& toFill)
   {
      if (!className.empty())
      {
         toFill.reserve(mGMImpl->mBaseActorObjectMap.size() + mGMImpl->mGameActorProxyMap.size());
         GMClassMatchSearchFunc searchFunc(className);
         FindActorsIf(searchFunc, toFill);
      }
      else
      {
         toFill.clear();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindPrototypesByActorType(const dtCore::ActorType& type, dtCore::ActorPtrVector& toFill) const
   {
      toFill.reserve(mGMImpl->mPrototypeActors.size());

      GMTypeMatchSearchFunc searchFunc(type);
      FindPrototypesIf(searchFunc, toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindPrototypesByName(const std::string& name, dtCore::ActorPtrVector& toFill) const
   {
      toFill.reserve(mGMImpl->mPrototypeActors.size());

      GMWildMatchSearchFunc searchFunc(name);
      FindPrototypesIf(searchFunc, toFill);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::BaseActorObject* GameManager::FindPrototypeByID(const dtCore::UniqueId& uniqueID)
   {
      GMImpl::GameActorMap::const_iterator itor = mGMImpl->mPrototypeActors.find(uniqueID);
      if (itor != mGMImpl->mPrototypeActors.end())
      {
         return itor->second.get();
      }

      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameActorProxy* GameManager::FindGameActorById(const dtCore::UniqueId& id)
   {
      GMImpl::GameActorMap::const_iterator itor = mGMImpl->mGameActorProxyMap.find(id);
      GameActorProxy* result = itor == mGMImpl->mGameActorProxyMap.end() ? NULL : itor->second.get();
      if (result != NULL && mGMImpl->mBatchData.valid() && !result->IsInGM() && !result->IsDeleted())
      {
         try
         {
            mGMImpl->AddActorToWorld(*this, *result);
         }
         catch (const dtUtil::Exception& ex)
         {
            // Actors can just decide to not be added by throwing an exception.  If it's an error, the actor
            // should log it as such, but here it's only a warning.
            ex.LogException(dtUtil::Log::LOG_WARNING, *mGMImpl->mLogger);
            result = NULL;
         }
      }
      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::BaseActorObject* GameManager::FindActorById(const dtCore::UniqueId& id)
   {
      dtCore::BaseActorObject* actor = FindGameActorById(id);

      if (actor != NULL)
      {
         return actor;
      }

      GMImpl::ActorMap::const_iterator itor = mGMImpl->mBaseActorObjectMap.find(id);
      return itor == mGMImpl->mBaseActorObjectMap.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::CloseCurrentMap()
   {
      if (mGMImpl->mMapChangeStateData->GetCurrentState() != MapChangeStateData::MapChangeState::IDLE)
      {
         static std::string changeMessage("You may not close the map while a map change is already in progress.");
         mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, changeMessage.c_str());
         throw dtGame::GeneralGameManagerException( changeMessage, __FILE__, __LINE__);
      }
      std::vector<std::string> emptyVec;
      mGMImpl->mMapChangeStateData->BeginMapChange(mGMImpl->mLoadedMaps, emptyVec, false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ChangeMap(const std::string& mapName, bool addBillboards)
   {
      std::vector<std::string> names;
      names.push_back(mapName);
      ChangeMapSet(names, addBillboards);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ChangeMapSet(const GameManager::NameVector& mapNames, bool addBillboards)
   {
      if (mapNames.empty())
      {
         throw dtGame::InvalidParameterException( "At least one map must be passed to ChangeMapSet.", __FILE__, __LINE__);
      }

      std::vector<std::string>::const_iterator i = mapNames.begin();
      std::vector<std::string>::const_iterator end = mapNames.end();
      for (; i != end; ++i)
      {
         if (i->empty())
         {
            throw dtGame::InvalidParameterException( "Empty string is not a valid map name.", __FILE__, __LINE__);
         }
      }

      if (mGMImpl->mMapChangeStateData->GetCurrentState() != MapChangeStateData::MapChangeState::IDLE)
      {
         static std::string changeMessage("You may not change the map set while a map change is already in progress.");
         mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, changeMessage.c_str());
         throw dtGame::GeneralGameManagerException( changeMessage, __FILE__, __LINE__);
      }

      mGMImpl->mMapChangeStateData->BeginMapChange(mGMImpl->mLoadedMaps, mapNames, addBillboards);
   }



   //////////////////////////////////////////////////////////////////////////
   void GameManager::OpenAdditionalMapSet( const NameVector& mapNames )
   {
      NameVector actuallyLoadedMaps;

      // Loop on map vec, and directly load all of them.
      // This will send
      NameVector::const_iterator mapItor = mapNames.begin();
      for(; mapItor != mapNames.end(); ++mapItor)
      {
         if (std::find(mGMImpl->mLoadedMaps.begin(), mGMImpl->mLoadedMaps.end(), *mapItor) != mGMImpl->mLoadedMaps.end())
            continue;

         // check whether this is a good map on the current project
         try
         {
            dtCore::Project::GetInstance().GetMap(*mapItor);
         }
         catch (const dtUtil::Exception& ex)
         {
            // if can't open this map, log an erro and keep going with the rest of the map list
            mGMImpl->mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "GameManager failed to open map [%s].", mapItor->c_str());
            ex.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance("mapchangestatedata.cpp"));

            continue;
         }

         // Actual map loading routine.
         // This will fire the INFO_ACTOR_CREATE message for every loaded actor
         mGMImpl->mMapChangeStateData->LoadSingleMapIntoGM(*mapItor);

         // Add a reference to the loaded map set on the GM
         mGMImpl->mLoadedMaps.push_back(*mapItor);

         // add a ref to send out a message at the end of the routing
         actuallyLoadedMaps.push_back(*mapItor);
      }

      // when we're done with the maps, end the sequence with a INFO_MAPS_OPENED message
      if (!actuallyLoadedMaps.empty())
      {
         dtCore::RefPtr<MapMessage> mapMessage;
         GetMessageFactory().CreateMessage(MessageType::INFO_MAPS_OPENED, mapMessage);
         mapMessage->SetMapNames(actuallyLoadedMaps);

         SendMessage(*mapMessage);
      }
   }


   //////////////////////////////////////////////////////////////////////////
   void GameManager::CloseAdditionalMapSet(const NameVector& mapNames)
   {
      NameVector actuallyUnloadedMaps;

      // loop on maps to unload
      NameVector::const_iterator mapItor = mapNames.begin();
      for(; mapItor != mapNames.end(); ++mapItor)
      {
         // skip the map if it's not currently loaded in the GM
         NameVector::iterator foundMap = std::find(mGMImpl->mLoadedMaps.begin(), mGMImpl->mLoadedMaps.end(), *mapItor);
         if (foundMap == mGMImpl->mLoadedMaps.end())
         {
            continue;
         }

            // Get the map - no need to check further for map validity,
         // as it has already been loaded...
         dtCore::Map& map = dtCore::Project::GetInstance().GetMap(*mapItor);

         // loop on all map's actors
         dtCore::ActorRefPtrVector proxies;
         map.GetAllProxies(proxies);

         // this call will actually fire the INFO_ACTOR_DELETE message for every deleted actor
         // and will delete actors at the end of this frame.
         dtCore::ActorRefPtrVector::iterator i, iend;
         i = proxies.begin();
         iend = proxies.end();
         for (; i != iend; ++i)
         {
            DeleteActor((*i)->GetId());
         }

         // delete from the list of loaded map on the GM
         mGMImpl->mLoadedMaps.erase(foundMap);
         actuallyUnloadedMaps.push_back(*mapItor);

         // TODO what to do with the libraries?? maybe they're needed by other maps...
         mGMImpl->mMapChangeStateData->CloseSingleMap(*mapItor, false);
      }

      // when done, send out the INFO_MAPS_CLOSED message
      if (!actuallyUnloadedMaps.empty())
      {
         dtCore::RefPtr<MapMessage> mapMessage;
         GetMessageFactory().CreateMessage(MessageType::INFO_MAPS_CLOSED, mapMessage);
         mapMessage->SetMapNames(actuallyUnloadedMaps);

         SendMessage(*mapMessage);
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetTimer(const std::string& name, const GameActorProxy* aboutActor,
      float time, bool repeat, bool realTime)
   {
      GMImpl::TimerInfo t;
      t.name = name;

      if (aboutActor == NULL)
      {
         t.aboutActor = dtCore::UniqueId(false);
      }
      else
      {
         t.aboutActor = aboutActor->GetId();
      }

      t.interval = dtCore::Timer_t(time * 1e6);
      if (realTime)
      {
         t.time = GetRealClockTime() + t.interval;
      }
      else
      {
         t.time = dtCore::Timer_t(GetSimTimeSinceStartup() * 1000000.0) + t.interval;
      }

      t.repeat = repeat;
      realTime ? mGMImpl->mRealTimeTimers.insert(t) : mGMImpl->mSimulationTimers.insert(t);
   }



   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ClearTimer(const std::string& name, const GameActorProxy* actor)
   {
      mGMImpl->ClearTimerSingleSet(mGMImpl->mRealTimeTimers, name, actor);
      mGMImpl->ClearTimerSingleSet(mGMImpl->mSimulationTimers, name, actor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetRegistrantsForMessages(const MessageType& type,
         std::vector< std::pair<GameActorProxy*, std::string> >& toFill) const
   {
      typedef std::pair<GMImpl::GlobalMessageListenerMap::const_iterator,
                        GMImpl::GlobalMessageListenerMap::const_iterator> IterPair;
      IterPair msgTypeMatches = mGMImpl->mGlobalMessageListeners.equal_range(&type);

      toFill.clear();
      toFill.reserve(std::distance(msgTypeMatches.first, msgTypeMatches.second));

      GMImpl::GlobalMessageListenerMap::const_iterator itor = msgTypeMatches.first;

      while (itor != msgTypeMatches.second)
      {
         // add the game actor and invokable name to a new pair in the vector.
         if (itor->second.first.valid())
         {
            toFill.push_back(std::make_pair(itor->second.first.get(), itor->second.second));
         }
         ++itor;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetRegistrantsForMessagesAboutActor(const MessageType& type,
                                                         const dtCore::UniqueId& targetActorId,
         std::vector< std::pair<GameActorProxy*,std::string> >& toFill) const
   {
      toFill.clear();

      GMImpl::ActorMessageListenerMap::const_iterator itor = mGMImpl->mActorMessageListeners.find(&type);

      if (itor != mGMImpl->mActorMessageListeners.end())
      {
         typedef std::pair<GMImpl::ProxyInvokableMap::const_iterator,
                           GMImpl::ProxyInvokableMap::const_iterator> IterPair;

         //second on itor is the internal map.
         IterPair foundBeginEnd = itor->second.equal_range(targetActorId);
         toFill.reserve(std::distance(foundBeginEnd.first, foundBeginEnd.second));

         GMImpl::ProxyInvokableMap::const_iterator targetActorItor = foundBeginEnd.first;
         while (targetActorItor != foundBeginEnd.second)
         {
            toFill.push_back(std::make_pair(targetActorItor->second.first.get(), targetActorItor->second.second));
            ++targetActorItor;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   static void ValidateMessageType(const MessageType& type, GameActorProxy& actor,
            const std::string& invokableName)
   {
      if (type == MessageType::TICK_END_OF_FRAME)
      {
         std::ostringstream ss;
         ss << "Unable to register globally for MessageType \"" << MessageType::TICK_END_OF_FRAME.GetName() << " for Actor \""
            << actor.GetActorType() << "\" "
            << " using invokable named \"" << invokableName << "\".  Only components may register for that message type.";

         throw dtUtil::Exception(ss.str(), __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RegisterForMessages(const MessageType& type, GameActorProxy& actor,
         const std::string& invokableName)
   {
      ValidateMessageType(type, actor, invokableName);

      mGMImpl->mGlobalMessageListeners.insert(
            std::make_pair(&type,
                  std::make_pair(dtCore::RefPtr<GameActorProxy>(&actor), invokableName)));

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterForMessages(const MessageType& type, GameActorProxy& actor,
                                           const std::string& invokableName)
   {
      typedef std::pair<GMImpl::GlobalMessageListenerMap::iterator, GMImpl::GlobalMessageListenerMap::iterator> IterPair;

      //find all matches of MessageType
      IterPair msgTypeMatches = mGMImpl->mGlobalMessageListeners.equal_range(&type);

      //NULL out the one that also match the supplied actor and invokableName
      for (GMImpl::GlobalMessageListenerMap::iterator itor = msgTypeMatches.first;
                                              itor != msgTypeMatches.second;
                                              ++itor)
      {
         if (itor->second.first.get() == &actor &&
             itor->second.second == invokableName)
         {
            //we'll actually erase this item next time it's invoked
            itor->second.first = NULL;
            itor->second.second = "";
            return;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RegisterForMessagesAboutActor(const MessageType& type,
         const dtCore::UniqueId& targetActorId, GameActorProxy& actor,
         const std::string& invokableName)
   {
      ValidateMessageType(type, actor, invokableName);

      GMImpl::ProxyInvokableMap& mapForType = mGMImpl->mActorMessageListeners[&type];
      mapForType.insert(std::make_pair(targetActorId, std::make_pair(dtCore::RefPtr<GameActorProxy>(&actor), invokableName)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterForMessagesAboutActor(const MessageType& type,
         const dtCore::UniqueId& targetActorId, GameActorProxy& actor,
         const std::string& invokableName)
   {
      GMImpl::ActorMessageListenerMap::iterator itor = mGMImpl->mActorMessageListeners.find(&type);

      if (itor != mGMImpl->mActorMessageListeners.end())
      {
         //second on itor is the internal map.
         GMImpl::ProxyInvokableMap::iterator itorInner = itor->second.find(targetActorId);
         while (itorInner != itor->second.end() && itorInner->first == targetActorId)
         {
            //second is a pair.
            //second.first is the game actor to receive the message second.second is the name of the invokable
            if (itorInner->second.first.get() == &actor && itorInner->second.second == invokableName)
            {
               GMImpl::ProxyInvokableMap::iterator toDelete = itorInner;
               ++itorInner;
               itor->second.erase(toDelete);
            }
            else
            {
               ++itorInner;
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterAllMessageListenersForActor(GameActorProxy& actor)
   {
      for (GMImpl::GlobalMessageListenerMap::iterator i = mGMImpl->mGlobalMessageListeners.begin();
           i != mGMImpl->mGlobalMessageListeners.end();)
      {
         GMImpl::GlobalMessageListenerMap::iterator toDelete = i;
         ++i;
         if (toDelete->second.first.get() == &actor)
         {
            mGMImpl->mGlobalMessageListeners.erase(toDelete);
         }
      }

      for (GMImpl::ActorMessageListenerMap::iterator i = mGMImpl->mActorMessageListeners.begin(); i != mGMImpl->mActorMessageListeners.end(); ++i)
      {
         for (GMImpl::ProxyInvokableMap::iterator j = i->second.begin(); j != i->second.end();)
         {
            GMImpl::ProxyInvokableMap::iterator toDelete = j;
            ++j;
            if (toDelete->first == actor.GetId()  ||  toDelete->second.first.get() == &actor )
            {
               i->second.erase(toDelete);
            }
            else if (actor.GetDrawable() != NULL && toDelete->first == actor.GetDrawable()->GetUniqueId())
            {
               LOG_WARNING("Actor Object and drawable have different IDs and found a message registration for the drawable, not the actor.");
               i->second.erase(toDelete);
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RejectMessage(const dtGame::Message& reasonMessage, const std::string& rejectDescription)
   {
      const MachineInfo& causingMachine = reasonMessage.GetSource();
      dtCore::RefPtr<Message> rejPtrMsg = mGMImpl->mFactory.CreateMessage(MessageType::SERVER_REQUEST_REJECTED);
      ServerMessageRejected* rejectMsg = static_cast<ServerMessageRejected*>(rejPtrMsg.get());
      rejectMsg->SetCausingMessage(&reasonMessage);
      rejectMsg->SetCause(rejectDescription);
      rejectMsg->SetDestination(&causingMachine);

      // if it's from us, just process it
      if (*mGMImpl->mMachineInfo == causingMachine)
      {
         SendMessage(*rejectMsg);
      }
      // if from someone else, only send it.  Else, if we are the server GM, our local components
      // may try to process a reject message that was meant for a client - most components assume
      // a reject message was only sent to them
      else
      {
         SendNetworkMessage(*rejectMsg);
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetProjectContext(const std::string& context, bool readOnly)
   {
      dtCore::Project::GetInstance().SetContext(context, readOnly);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GameManager::GetProjectContext() const
   {
      return dtCore::Project::GetInstance().GetContext();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::Shutdown()
   {
      if (!mGMImpl->mLoadedMaps.empty())
      {
         dtCore::Project& project = dtCore::Project::GetInstance();

         project.CloseAllMaps(true);
         // clear the maps vector because if someone happens to call shutdown again,
         // it will reopen all the maps.
         mGMImpl->mLoadedMaps.clear();
      }

      // Delete the actors
      DeleteAllActors();

      // flush all the deleted messages
      do
      {
         // Process all the delete messages if actors delete other actors.
         DoSendMessages();
         DoSendNetworkMessages();
      }
      while (!RemoveDeletedActors());

      // Just make sure to flush the last of the messages in case the last set of deletes
      // happened to send a messages that wasn't related to deleting an actor.
      DoSendMessages();
      DoSendNetworkMessages();

      //tell all the components they've been removed
      GMImpl::GMComponentContainer::iterator compItr = mGMImpl->mComponentList.begin();
      while (compItr != mGMImpl->mComponentList.end())
      {
         if (compItr->valid())
         {
            RemoveComponent(**compItr);
         }
         ++compItr;
      }

      //now purge the component container for real
      mGMImpl->mComponentList.clear();

      mGMImpl->mGMStatistics.mDebugLoggerInformation.clear();

      while (!mGMImpl->mSendNetworkMessageQueue.empty())
      {
         mGMImpl->mSendNetworkMessageQueue.pop();
      }

      while (!mGMImpl->mSendMessageQueue.empty())
      {
         mGMImpl->mSendMessageQueue.pop();
      }

      mGMImpl->mShuttingDown = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameManager::IsShuttingDown() const
   {
      return mGMImpl->mShuttingDown;
   }


   ///////////////////////////////////////////////////////////////////////////////
   GMSettings& GameManager::GetGMSettings()
   {
      return *mGMImpl->mGMSettings;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetGMSettings(GMSettings& newSettings)
   {
      // Simply overwrites the existing settings.
      mGMImpl->mGMSettings = &newSettings;
   }


   ////////////////////////////////////////////////////////////////////////////////
   /*                            Statistics Information                          */
   ////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   int GameManager::GetStatisticsInterval() const
   {
      return mGMImpl->mGMStatistics.GetStatisticsInterval();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GameManager::GetStatisticsToConsole() const
   {
      return mGMImpl->mGMStatistics.ShouldWeLogToConsole();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& GameManager::GetStatisticsLogFilePath() const
   {
      return mGMImpl->mGMStatistics.GetFilePathToPrintDebugInformation();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DebugStatisticsTurnOff(bool logLastTime, bool clearList)
   {
      mGMImpl->mGMStatistics.DebugStatisticsTurnOff(*this, logLastTime, clearList);
   }

   //////////////////////////////////////////////////////////////////////////
   bool GameManager::GetRemoveGameEventsOnMapChange() const
   {
      return mGMImpl->mRemoveGameEventsOnMapChange;
   }

   //////////////////////////////////////////////////////////////////////////
   void GameManager::SetRemoveGameEventsOnMapChange(const bool removeGameEventsOnMapChange)
   {
      mGMImpl->mRemoveGameEventsOnMapChange = removeGameEventsOnMapChange;
   }

   //////////////////////////////////////////////////////////////////////////
   MessageFactory& GameManager::GetMessageFactory()
   {
      return mGMImpl->mFactory;
   }

   //////////////////////////////////////////////////////////////////////////
   const MessageFactory& GameManager::GetMessageFactory() const
   {
      return mGMImpl->mFactory;
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameManager::DebugStatisticsTurnOn(bool logComponents, bool logActors,
      const int statisticsInterval, bool toConsole, const std::string& path)
   {
      mGMImpl->mGMStatistics.DebugStatisticsTurnOn(logComponents, logActors,
         statisticsInterval, toConsole, path);
   }

   //////////////////////////////////////////////////////////////////////////
   const MachineInfo& GameManager::GetMachineInfo() const
   {
      return *mGMImpl->mMachineInfo;
   }

   //////////////////////////////////////////////////////////////////////////
   MachineInfo& GameManager::GetMachineInfo()
   {
      return *mGMImpl->mMachineInfo;
   }
} // namespace dtGame
