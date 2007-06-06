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
 * Matthew W. Campbell, William E. Johnson II, David Guthrie
 */
#include <prefix/dtgameprefix-src.h>

#include <dtGame/gamemanager.h>
#include <dtGame/messagefactory.h>
#include <dtGame/basemessages.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>
#include <dtGame/mapchangestatedata.h>

#include <dtDAL/actortype.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/gameeventmanager.h>

#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/timer.h>

#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>

using namespace dtCore;

namespace dtGame
{
   IMPLEMENT_MANAGEMENT_LAYER(GameManager);

   IMPLEMENT_ENUM(GameManager::ComponentPriority);

   const GameManager::ComponentPriority GameManager::ComponentPriority::HIGHEST("HIGHEST", 1);
   const GameManager::ComponentPriority GameManager::ComponentPriority::HIGHER("HIGHER", 2);
   const GameManager::ComponentPriority GameManager::ComponentPriority::NORMAL("NORMAL", 3);
   const GameManager::ComponentPriority GameManager::ComponentPriority::LOWER("LOWER", 4);
   const GameManager::ComponentPriority GameManager::ComponentPriority::LOWEST("LOWEST", 5);

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::GameManager(dtCore::Scene &scene) :
      mMachineInfo(new MachineInfo()),
      mSendCreatesAndDeletes(true),
      mAddActorsToScene(true),
      mScene(&scene),
      mFactory("GameManager MessageFactory", *mMachineInfo, ""),
      mStatsLastFragmentDump(0),
      mStatsNumProcMessages(0),
      mStatsNumSendNetworkMessages(0),
      mStatsNumFrames(0),
      mStatsCumGMProcessTime(0),
      mStatisticsInterval(0),
      mPrintFileToConsole(false),
      mDoStatsOnTheComponents(false),
      mDoStatsOnTheActors(false),
      mApplication(NULL),
      mRemoveGameEventsOnMapChange(true)
   {

      mLibMgr = &dtDAL::LibraryManager::GetInstance();
      mLogger = &dtUtil::Log::GetInstance("gamemanager.cpp");
      AddSender(&dtCore::System::GetInstance());
      mPaused = dtCore::System::GetInstance().GetPause();

      mMapChangeStateData = new MapChangeStateData(*this);

      // when we come alive, the first message everyone gets will be INFO_RESTARTED
      dtCore::RefPtr<Message> restartMessage = 
         GetMessageFactory().CreateMessage(MessageType::INFO_RESTARTED);
      SendMessage(*restartMessage);
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::GameManager(const GameManager&):
      mMachineInfo(new MachineInfo()),
      mFactory("GameManager MessageFactory",*mMachineInfo,"")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameManager& GameManager::operator=(const GameManager&) {return *this;}

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::~GameManager()
   {
      //RemoveSender(&dtCore::System::GetInstance());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::LoadActorRegistry(const std::string &libName)
   {
      mLibMgr->LoadActorRegistry(libName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnloadActorRegistry(const std::string &libName)
   {
      mLibMgr->UnloadActorRegistry(libName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetActorTypes(std::vector<dtDAL::ActorType*> &actorTypes)
   {
      mLibMgr->GetActorTypes(actorTypes);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorType* GameManager::FindActorType(const std::string &category, const std::string &name)
   {
      return mLibMgr->FindActorType(category, name);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorPluginRegistry* GameManager::GetRegistry(const std::string &name)
   {
      return mLibMgr->GetRegistry(name);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorPluginRegistry* GameManager::GetRegistryForType(dtDAL::ActorType& actorType)
   {
      return mLibMgr->GetRegistryForType(actorType);
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string GameManager::GetPlatformSpecificLibraryName(const std::string &libBase)
   {
      return mLibMgr->GetPlatformSpecificLibraryName(libBase);
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string GameManager::GetPlatformIndependentLibraryName(const std::string &libName)
   {
      return mLibMgr->GetPlatformIndependentLibraryName(libName);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtABC::Application& GameManager::GetApplication()
   {
      if (mApplication == NULL)
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, 
         "No Application was ever assigned to the GameManager.", __FILE__, __LINE__);

      return *mApplication;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtABC::Application& GameManager::GetApplication() const
   {
      if (mApplication == NULL)
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, 
         "No Application was ever assigned to the GameManager.", __FILE__, __LINE__);

      return *mApplication;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetApplication(dtABC::Application& application)
   {
      mApplication = &application;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Scene& GameManager::GetScene() { return *mScene; }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::Scene& GameManager::GetScene() const { return *mScene; }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetScene(dtCore::Scene &newScene) { mScene = &newScene; }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::OnMessage(MessageData *data)
   {
      if (data->message == "preframe")
      {
         double* timeChange = (double*)data->userData;
         PreFrame(timeChange[0], timeChange[1]);
      }
      else if (data->message == "postframe")
      {
         PostFrame();
      }
      else if (data->message == "pause_start")
      {
         if (!IsPaused())
            SetPaused(true);
      }
      else if (data->message == "pause_end")
      {
         if (IsPaused())
            SetPaused(false);
      }
      else if (data->message == "pause")
      {
         if (!IsPaused())
            SetPaused(true);

         double* timeChange = (double*)data->userData;
         PreFrame(0.0, *timeChange);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SendNetworkMessage(const Message& message)
   {
      mSendNetworkMessageQueue.push(dtCore::RefPtr<const Message>(&message));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SendMessage(const Message& message)
   {
      mSendMessageQueue.push(dtCore::RefPtr<const Message>(&message));
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
   Timer_t GameManager::GetSimulationClockTime() const
   {
      return dtCore::System::GetInstance().GetSimulationClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Timer_t GameManager::GetRealClockTime() const
   {
      return dtCore::System::GetInstance().GetRealClockTime();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ChangeTimeSettings(double newTime, float newTimeScale, const Timer_t &newClockTime)
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
   void GameManager::SetPaused(bool pause)
   {
      if (mPaused == pause)
         return;

      mPaused = pause;
      dtCore::System::GetInstance().SetPause(mPaused);
      if (mPaused)
      {
         SendMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_PAUSED));
      }
      else
      {
         SendMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_RESUMED));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PreFrame(double deltaSimTime, double deltaRealTime)
   {
      // stats information used to track statistics per fragment (usually about 1 second)
      Timer statsTickClock;
      Timer_t frameTickStart(0);
      Timer_t frameTickStartCurrent(0);
      bool logComponents = mStatisticsInterval > 0 && mDoStatsOnTheComponents;
      bool logActors = mStatisticsInterval > 0 && mDoStatsOnTheActors;

      if (mStatisticsInterval > 0)
         frameTickStart = statsTickClock.Tick();

      // SEND MESSAGES - Forward Send Messages to all components (no actors)
      while (!mSendNetworkMessageQueue.empty())
      {
         mStatsNumSendNetworkMessages += 1;
         dtCore::RefPtr<const Message> message = mSendNetworkMessageQueue.front();
         mSendNetworkMessageQueue.pop();

         if (!message.valid())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, 
               "Message in send to network queue is NULL.  Something is majorly wrong with the GameManager.");
            continue;
         }

         try
         {
            for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
            {
               /////////////////////////
               // Statistics information
               if (logComponents)
                  frameTickStartCurrent = statsTickClock.Tick();

               (*i)->DispatchNetworkMessage(*message);

               // Statistics information
               if(logComponents)
               {
                  double frameTickDelta = statsTickClock.DeltaSec(frameTickStartCurrent, statsTickClock.Tick());
                  UpdateDebugStats((*i)->GetUniqueId(),(*i)->GetName(), frameTickDelta, true, false);
               }
            }
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
         }
      }

      double simulationTime = dtCore::System::GetInstance().GetSimulationTime();

      dtCore::RefPtr<Message> tick = GetMessageFactory().CreateMessage(MessageType::TICK_LOCAL);

      // Force Tick Local message
      TickMessage& tickMessage = static_cast<TickMessage&>(*tick);
      tickMessage.SetDeltaRealTime((float)deltaRealTime);
      tickMessage.SetDeltaSimTime((float)deltaSimTime);
      tickMessage.SetSimTimeScale(GetTimeScale());
      tickMessage.SetDestination(&GetMachineInfo());
      tickMessage.SetSimulationTime(simulationTime);

      dtCore::RefPtr<Message> tickRemote = GetMessageFactory().CreateMessage(MessageType::TICK_REMOTE);

      // Force Tick Remote Message
      TickMessage& tickRemoteMessage = static_cast<TickMessage&>(*tickRemote);
      tickRemoteMessage.SetDeltaRealTime((float)deltaRealTime);
      tickRemoteMessage.SetDeltaSimTime((float)deltaSimTime);
      tickRemoteMessage.SetSimTimeScale(GetTimeScale());
      tickRemoteMessage.SetDestination(&GetMachineInfo());
      tickRemoteMessage.SetSimulationTime(simulationTime);

      if (mMapChangeStateData.valid())
      {
         mMapChangeStateData->ContinueMapChange();
         if (mMapChangeStateData->GetCurrentState() == MapChangeStateData::MapChangeState::IDLE)
         {
            mLoadedMap = mMapChangeStateData->GetNewMapName();
         }
      }

      SendMessage(*tick);
      SendMessage(*tickRemote);

      ProcessTimers(mRealTimeTimers, GetRealClockTime());
      ProcessTimers(mSimulationTimers, GetSimulationClockTime());

      // PROCESS MESSAGES - Send all Process messages to components and interested actors
      while (!mSendMessageQueue.empty())
      {
         mStatsNumProcMessages += 1;

         // Forward to Components first
         dtCore::RefPtr<const Message> message = mSendMessageQueue.front();
         mSendMessageQueue.pop();
         
         if (!message.valid())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, 
               "Message in send queue is NULL.  Something is majorly wrong with the GameManager.");
            continue;
         }
         
         bool isATickLocalMessage = (tick.get() == message.get());
         //if(logComponents || logActors)
         //{
            //const TickMessage* tickType = dynamic_cast<const TickMessage*>(message.get());
            //if(tickType != NULL)    isATickLocalMessage = true;
         //}

         // LOOP THROUGH COMPONENTS
         for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
         {
            // Statistics information
            if (logComponents)
               frameTickStartCurrent = statsTickClock.Tick();

            try
            {
               (*i)->ProcessMessage(*message);
            }
            catch (const dtUtil::Exception& ex)
            {
               ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
            }

            // Statistics information
            if (logComponents)
            {
               double frameTickDelta = statsTickClock.DeltaSec(frameTickStartCurrent, statsTickClock.Tick());
               UpdateDebugStats((*i)->GetUniqueId(),(*i)->GetName(), frameTickDelta, true, isATickLocalMessage);
            }
         }

         // GLOBAL INVOKABLES - Process it on globally registered invokables
         const MessageType &msgType = message->GetMessageType();
         std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator  itor
            = mGlobalMessageListeners.find(&msgType);

         while (itor != mGlobalMessageListeners.end() && itor->first == &msgType)
         {
            std::pair<dtCore::RefPtr<GameActorProxy>, std::string >& listener = itor->second;
            // hold onto the actor in a refptr so that the stats code
            // won't crash if the actor unregisters for the message. 
            dtCore::RefPtr<GameActorProxy> listenerActorProxy = listener.first;
            Invokable* invokable = listener.first->GetInvokable(listener.second);
            ++itor;

            if (invokable != NULL)
            {
               // Statistics information
               if (logActors)
                  frameTickStartCurrent = statsTickClock.Tick();

               try
               {
                  invokable->Invoke(*message);
               }
               catch (const dtUtil::Exception& ex)
               {
                  ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
               }

               // Statistics information
               if (logActors)
               {
                  double frameTickDelta = statsTickClock.DeltaSec(frameTickStartCurrent, statsTickClock.Tick());
                  UpdateDebugStats(listenerActorProxy->GetId(), listenerActorProxy->GetName(), frameTickDelta, false, isATickLocalMessage);
               }
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                      "Invokable named %s is registered as a listener, but Proxy %s does not have an invokable by that name.",
                                      listener.second.c_str(), listener.first->GetActorType().GetName().c_str());
            }
         }

         // ABOUT ACTOR - The actor itself and others registered against a particular actor
         if (!message->GetAboutActorId().ToString().empty())
         {
            //if we have an about actor, first try to send it to the actor itself
            GameActorProxy* aboutActor = FindGameActorById(message->GetAboutActorId());
            if (aboutActor != NULL)
            {
               std::vector<dtGame::Invokable*> aboutActorInvokables;

               aboutActor->GetMessageHandlers(message->GetMessageType(), aboutActorInvokables);
               unsigned int aboutActorsSize = aboutActorInvokables.size(); // checking size on vector is slow :(
               for (unsigned i = 0; i < aboutActorsSize; ++i)
               {
                  // Statistics information
                  if (logActors)
                     frameTickStartCurrent = statsTickClock.Tick();

                  try
                  {
                     aboutActorInvokables[i]->Invoke(*message);
                  }
                  catch (const dtUtil::Exception& ex)
                  {
                     ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
                  }

                  // Statistics information
                  if (logActors)
                  {
                     double frameTickDelta = statsTickClock.DeltaSec(frameTickStartCurrent, statsTickClock.Tick());
                     UpdateDebugStats(aboutActor->GetId(), aboutActor->GetName(), frameTickDelta, false, false);
                  }
               }
            }

            //next, sent it to all actors listening to that actor for that message type.
            // TODO - This should be refactored like we did for the Global Invokables a few lines up. It should work with the map directly instead of filling lists repeatedly
            std::vector<std::pair<GameActorProxy*, std::string > > toFill;
            GetRegistrantsForMessagesAboutActor(message->GetMessageType(), message->GetAboutActorId(), toFill);
            unsigned int toFillSize = toFill.size(); // checking size on vector is slow :(
            for (unsigned i = 0; i < toFillSize; ++i)
            {
               std::pair<GameActorProxy*, std::string >& listener = toFill[i];
               Invokable* invokable = listener.first->GetInvokable(listener.second);
               if (invokable != NULL)
               {
                  // Statistics information
                  if (logActors)
                     frameTickStartCurrent = statsTickClock.Tick();
                  try
                  {
                     invokable->Invoke(*message);
                  }
                  catch (const dtUtil::Exception& ex)
                  {
                     ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
                  }                  

                  if (logActors)
                  {
                     double frameTickDelta = statsTickClock.DeltaSec(frameTickStartCurrent, statsTickClock.Tick());
                     UpdateDebugStats(listener.first->GetId(), listener.first->GetName(), frameTickDelta, false, false);
                  }
               }
               else
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                         "Invokable named %s is registered as a listener, but Proxy %s does not have an invokable by that name.",
                                         listener.second.c_str(), listener.first->GetActorType().GetName().c_str());
               }
            }
         }
      }

      // DELETE ACTORS
      unsigned int deleteListSize = mDeleteList.size(); // checking size on vector is slow :(
      for (unsigned int i = 0; i < deleteListSize; ++i)
      {
         GameActorProxy& gameActorProxy = *mDeleteList[i];

         std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(gameActorProxy.GetId());

         dtCore::UniqueId id;
         if (itor != mGameActorProxyMap.end())
         {
            id = itor->first;
            UnregisterAllMessageListenersForActor(*itor->second);
            mGameActorProxyMap.erase(itor);
            RemoveActorFromScene(gameActorProxy);
         }

         gameActorProxy.SetGameManager(NULL);

      }

      mDeleteList.clear();

      // STATISTICS - If fragment time occured, dump out the GM statistics
      if (mStatisticsInterval > 0)
      {
         mStatsNumFrames++;
         // Compute GM process time.  Note - You can't use GetRealClockTime() for GM work time
         // because mClock on system is only updated at the start of the whole tick.
         Timer_t frameTickStop = statsTickClock.Tick();
         double fragmentDelta = statsTickClock.DeltaMicro(mStatsLastFragmentDump, frameTickStop);
         mStatsCumGMProcessTime += dtCore::Timer_t(statsTickClock.DeltaMicro(frameTickStart, frameTickStop));

         if (fragmentDelta < 0) // handle wierd case of wrap around (just to be safe)
            mStatsLastFragmentDump = frameTickStop;

         else if (fragmentDelta >= (mStatisticsInterval * 1000000))
         {
            Timer_t realTimeElapsed = dtCore::Timer_t(statsTickClock.DeltaMicro(mStatsLastFragmentDump, frameTickStop));
            DebugStatisticsPrintOut(realTimeElapsed);
            mStatsLastFragmentDump  = frameTickStop;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PostFrame()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddComponent(GMComponent& component, const GameManager::ComponentPriority& priority)
   {
      if(GetComponentByName(component.GetName()) != NULL)
         throw dtUtil::Exception(ExceptionEnum::INVALID_PARAMETER, 
         "A component was already registered with the Game Manager with the name: " + component.GetName(), __FILE__, __LINE__);
      
      component.SetGameManager(this);
      component.SetComponentPriority(priority);
      //we sort the items by priority so that components of higher priority get messages first.
      bool inserted = false;
      for (unsigned i = 0; i < mComponentList.size(); ++i)
      {
         if (mComponentList[i]->GetComponentPriority().GetOrderId() > priority.GetOrderId())
         {
           mComponentList.insert(mComponentList.begin() + i, dtCore::RefPtr<GMComponent>(&component));
           inserted = true;
           break;
         }
      }

      if (!inserted)
         mComponentList.push_back(dtCore::RefPtr<GMComponent>(&component));

      // notify the component that it was added to the GM
      component.OnAddedToGM();

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RemoveComponent(GMComponent& component)
   {
      for(std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
      {
         if(i->get() == &component)
         {
            component.OnRemovedFromGM();
            component.SetGameManager(NULL);
            mComponentList.erase(i);
            return;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllComponents(std::vector<GMComponent*>& toFill)
   {
      toFill.clear();
      unsigned int componentListSize = mComponentList.size(); 
      toFill.reserve(componentListSize);

      for (unsigned i = 0; i < componentListSize; ++i)
      {
         toFill.push_back(mComponentList[i].get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllComponents(std::vector<const GMComponent*>& toFill) const
   {
      toFill.clear();
      unsigned int componentListSize = mComponentList.size();
      toFill.reserve(componentListSize);

      for (unsigned i = 0; i < componentListSize; ++i)
      {
         toFill.push_back(mComponentList[i].get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   GMComponent* GameManager::GetComponentByName(const std::string &name)
   {
      for(std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin();
          i != mComponentList.end(); ++i)
      {
         if((*i)->GetName() == name)
            return (*i).get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const GMComponent* GameManager::GetComponentByName(const std::string &name) const
   {
      for(std::vector<dtCore::RefPtr<GMComponent> >::const_iterator i = mComponentList.begin();
         i != mComponentList.end(); ++i)
      {
         if((*i)->GetName() == name)
            return (*i).get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtGame::GameActorProxy> GameManager::CreateRemoteGameActor(dtDAL::ActorType& actorType)
   {
      dtCore::RefPtr<dtGame::GameActorProxy> result;
      CreateActor(actorType, result);

      if (result.valid())
      {
         result->SetRemote(true);
      }
      else 
      {
         throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER, "The actor type \"" 
            + actorType.GetCategory() + "." + actorType.GetName() + "\" is invalid because it is not a game actor type."
            , __FILE__, __LINE__);
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProxy> GameManager::CreateActor(dtDAL::ActorType& actorType) 
   {
      try
      {
         dtCore::RefPtr<dtDAL::ActorProxy> ap = dtDAL::LibraryManager::GetInstance().CreateActorProxy(actorType).get();
         if (ap->IsInstanceOf("dtGame::GameActor"))
         {

            dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
            if (gap != NULL)
            {
               gap->SetGameManager(this);
               gap->BuildInvokables();
            }
            else
               throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, 
               "ERROR: Actor has the type of a GameActor, but casting it to a GameActorProxy failed.", __FILE__, __LINE__);
         }

         return ap;
      }
      catch (const dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Error creating actor: %s", ex.What().c_str());
         throw ex;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProxy> GameManager::CreateActor(const std::string &category, const std::string &name) 
   {
      dtCore::RefPtr<dtDAL::ActorType> type = FindActorType(category, name);
      if(!type.valid())
         throw dtUtil::Exception(ExceptionEnum::UNKNOWN_ACTOR_TYPE, 
         "No actor exists of the specified name and category", __FILE__, __LINE__);

      return CreateActor(*type);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActor(dtDAL::ActorProxy& actorProxy)
   {
      if(mEnvironment != NULL)
      {
         if(mEnvironment.get() != &actorProxy)
         {
            EnvironmentActor *ea = dynamic_cast<EnvironmentActor*>(mEnvironment->GetActor());
            if(ea == NULL)
            {
               LOG_ERROR("An environment actor proxy has an invalid actor");
               return;
            }
            ea->AddActor(*actorProxy.GetActor());
            mActorProxyMap.insert(std::make_pair(actorProxy.GetId(), &actorProxy));
         }
         else
         {
            mActorProxyMap.insert(std::make_pair(mEnvironment->GetId(), mEnvironment.get()));
            SendEnvironmentChangedMessage(mEnvironment.get());
         }
      }
      else
      {
         mActorProxyMap.insert(std::make_pair(actorProxy.GetId(), &actorProxy));
         mScene->AddDrawable(actorProxy.GetActor());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActor(GameActorProxy& gameActorProxy, bool isRemote, bool publish)
   {
      gameActorProxy.SetRemote(isRemote);
      gameActorProxy.SetIsInGM(true);

      if(mEnvironment != NULL)
      {
         if(mEnvironment.get() != &gameActorProxy)
         {
            EnvironmentActor *ea = static_cast<EnvironmentActor*>(mEnvironment->GetActor());
            ea->AddActor(*gameActorProxy.GetActor());
            mGameActorProxyMap.insert(std::make_pair(gameActorProxy.GetId(), &gameActorProxy));
         }
         else
         {
            mGameActorProxyMap.insert(std::make_pair(mEnvironment->GetId(), mEnvironment.get()));
            mScene->AddDrawable(mEnvironment->GetActor());
            SendEnvironmentChangedMessage(mEnvironment.get());
         }
      }
      else
      {
         mGameActorProxyMap.insert(std::make_pair(gameActorProxy.GetId(), &gameActorProxy));
         mScene->AddDrawable(gameActorProxy.GetActor());
      }

      // Remote actors are normally created in response to a create message, so sending another is silly.
      // Also, this doen't currently send messages when loading a map, so check here for that state.
      if (!isRemote && mMapChangeStateData->GetCurrentState() == MapChangeStateData::MapChangeState::IDLE)
      {
         dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_CREATED);
         gameActorProxy.PopulateActorUpdate(static_cast<ActorUpdateMessage&>(*msg));
         SendMessage(*msg);
      }
      if (publish)
         PublishActor(gameActorProxy);

      gameActorProxy.InvokeEnteredWorld();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActorAsAPrototype(GameActorProxy& gameActorProxy)
   {
      gameActorProxy.SetGameManager(this);
      mPrototypeActors.insert(std::make_pair(gameActorProxy.GetId(), &gameActorProxy));
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProxy> GameManager::CreateActorFromPrototype(const dtCore::UniqueId& uniqueID)
   {
      dtDAL::ActorProxy *ourObject = FindPrototypeByID(uniqueID);
      if(ourObject != NULL)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> temp = ourObject->Clone().get();
         dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(temp.get());
         if(gap != NULL)
         {
            gap->SetGameManager(this);
            gap->BuildInvokables();
         }
         return temp;
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetEnvironmentActor(EnvironmentActorProxy *envActor)
   {
      if(envActor != NULL)
      {
         // Already set? No-Op
         if(mEnvironment == envActor)
         {
            LOG_WARNING("Tried to set the internal environment proxy pointer to itself");
            return;
         }

         EnvironmentActor *ea = static_cast<EnvironmentActor*>(envActor->GetActor());

         dtCore::RefPtr<EnvironmentActorProxy> oldProxy = mEnvironment;
         bool wasPagingEnabled = mScene->IsPagingEnabled();
         if(wasPagingEnabled)
            mScene->DisablePaging();

         if(mEnvironment.valid())
         {
            // Internal pointer is valid, so we must remove all of its children and Emancipate them
            // Since the pointer is still valid, the children still think they have a valid
            // parent and otherwise the call to AddChild will fail
            DeleteActor(*mEnvironment);
         }

         // Internal pointer is not valid, we are setting a new environment
         // We need to remove all the drawables from the scene and add them
         // to the new environment
         std::vector<dtDAL::ActorProxy*> actors;
         GetAllActors(actors);
         mScene->RemoveAllDrawables();
         mScene->UseSceneLight(true);
         unsigned int actorsSize = actors.size(); // checking size on vector is slow :(
         for(unsigned int i = 0; i < actorsSize; i++)
         {
            if(actors[i] != oldProxy.get())
               ea->AddActor(*actors[i]->GetActor());
         }

         mEnvironment = envActor;
         mEnvironment->SetGameManager(this);
         AddActor(*mEnvironment, false, false);
         if(wasPagingEnabled)
            mScene->EnablePaging();
         ea = dynamic_cast<EnvironmentActor*>(mEnvironment->GetActor());
         if(ea == NULL)
         {
            LOG_ERROR("The environment actor proxy parameter has an invalid actor");
            return;
         }

         time_t timer = time_t(GetSimulationClockTime() / long(1e6));
         tm *time = localtime(&timer);

         if (time == NULL)
         {
            LOG_WARNING("The sim clock time is set to a time before January 1, 1970. Unsupported time");
         }
         else
         {
            ea->SetTimeAndDate(time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
         }
      }
      else
      {
         // Setting current valid env actor to NULL
         if(mEnvironment != NULL)
         {
            if(mScene->IsPagingEnabled())
            {
               mScene->DisablePaging();
               DeleteActor(*mEnvironment);
               mScene->EnablePaging();
            }
            else
               DeleteActor(*mEnvironment);
         }
         // else, Currently NULL internal env actor being set to NULL. No-Op
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PublishActor(GameActorProxy& gameActorProxy)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(gameActorProxy.GetId());

      if (itor == mGameActorProxyMap.end())
      {
         throw dtUtil::Exception(ExceptionEnum::INVALID_ACTOR_STATE,
            "A GameActor may only be published if it's added to the GameManager as a game actor.", __FILE__, __LINE__);
      }

      if (gameActorProxy.IsRemote())
         throw dtUtil::Exception(ExceptionEnum::ACTOR_IS_REMOTE, "A remote game actor may not be published", __FILE__, __LINE__);

      gameActorProxy.SetPublished(true);
      dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_PUBLISHED);
      msg->SetDestination(&GetMachineInfo());
      msg->SetAboutActorId(gameActorProxy.GetId());
      msg->SetSendingActorId(gameActorProxy.GetId());
      SendMessage(*msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteActor(dtDAL::ActorProxy& actorProxy)
   {
      // Is it an environment actor proxy?
      EnvironmentActorProxy *eap = dynamic_cast<EnvironmentActorProxy*>(&actorProxy);
      if(eap != NULL && mScene->GetDrawableIndex(eap->GetActor()) != mScene->GetNumberOfAddedDrawable())
      {
         // First we have to remove all of the actors from it
         EnvironmentActor *e = dynamic_cast<EnvironmentActor*>(&eap->GetGameActor());
         std::vector<dtCore::DeltaDrawable*> actors;
         e->GetAllActors(actors);
         e->RemoveAllActors();

         // Now that all the old actors are removed add them back to the scene
         // Also invalidate the delete environment parent by calling Emancipate
         for(unsigned int i = 0; i < actors.size(); ++i)
         {
            mScene->AddDrawable(actors[i]);
         }

         // Are we deleting the environment pointer?
         if(eap == mEnvironment.get())
            mEnvironment = NULL;
      }

      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(actorProxy.GetId());

      dtCore::UniqueId id;
      if (itor == mGameActorProxyMap.end())
      {
         //it's not in the game manager as a game actor proxy, maybe it's in there
         //as a regular actor proxy.
         std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor = mActorProxyMap.find(actorProxy.GetId());

         if (itor != mActorProxyMap.end())
         {
            RemoveActorFromScene(actorProxy);
            mActorProxyMap.erase(itor);
         }
      }
      else
      {
         id = itor->first;
         GameActorProxy& gameActorProxy = *itor->second;
         mDeleteList.push_back(itor->second);
         // Remote actors are deleted in response to a delete message, so sending another is silly.
         // Also, this doen't currently send messages when closing a map, so check here for that state.
         if (!gameActorProxy.IsRemote() && mMapChangeStateData->GetCurrentState() == MapChangeStateData::MapChangeState::IDLE)
         {
            gameActorProxy.SetIsInGM(false);

            dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_DELETED);
            msg->SetAboutActorId(id);

            SendMessage(*msg);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RemoveActorFromScene(dtDAL::ActorProxy& proxy)
   {
      dtCore::DeltaDrawable& dd = *proxy.GetActor();

      if (dd.GetSceneParent() != mScene.get())
         return;

      //find all of the children that have actor proxies associated with them to move them up
      //one level in the scene.
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > childrenToMove;
      for (unsigned i = 0; i < dd.GetNumChildren(); ++i)
      {
         dtCore::DeltaDrawable& child = *dd.GetChild(i);
         dtDAL::ActorProxy* childProxy = FindActorById(child.GetUniqueId());
         if (childProxy != NULL)
         {

            childrenToMove.push_back(childProxy);
         }
      }

      if (dd.GetParent() == NULL)
      {
         //remove the proxy drawable
         mScene->RemoveDrawable(&dd);

         //put all the children in the base scene.
         for (unsigned i = 0; i < childrenToMove.size(); ++i)
         {
            mScene->AddDrawable(childrenToMove[i]->GetActor());
         }
      }
      else
      {
         //add all the children to the parent drawable.
         for (unsigned i = 0; i < childrenToMove.size(); ++i)
         {
            dtCore::DeltaDrawable* child = childrenToMove[i]->GetActor();
            child->Emancipate();
            dd.GetParent()->AddChild(child);
         }
         //remove the proxy drawable from the parent.
         dd.Emancipate();
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteAllActors(bool immediate)
   {
      if (immediate)
      {
         mScene->RemoveAllDrawables();

         if (mEnvironment.valid())
         {
            static_cast<dtGame::EnvironmentActor&>(mEnvironment->GetGameActor()).RemoveAllActors();
            mEnvironment = NULL;
         }

         mActorProxyMap.clear();
         mGlobalMessageListeners.clear();
         mActorMessageListeners.clear();
         mGameActorProxyMap.clear();
         mRealTimeTimers.clear();
         mSimulationTimers.clear();

         //all the actors are deleted now, so the problems with clearing the list
         //of deleted actors is not a problem.
         mDeleteList.clear();
      }
      else
      {
         while (!mActorProxyMap.empty())
         {
            DeleteActor(*mActorProxyMap.begin()->second);
         }

         for(std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator i = mGameActorProxyMap.begin();
            i != mGameActorProxyMap.end(); ++i)
         {
            DeleteActor(*i->second);
         }
      }

      DeleteAllPrototypes();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteAllPrototypes()
   {
      mPrototypeActors.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeletePrototype(const dtCore::UniqueId& uniqueId)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mPrototypeActors.find(uniqueId);
      if(itor != mPrototypeActors.end())
      {
         mPrototypeActors.erase(itor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetUsedActorTypes(std::set<const dtDAL::ActorType*>& toFill) const
   {
      toFill.clear();      

      // spin through the actors and add all used actor types to the set, so that we don't
      // get duplicates. 
      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mGameActorProxyMap.begin();
         itor != mGameActorProxyMap.end(); ++itor)
         toFill.insert(&itor->second->GetActorType());

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor = mActorProxyMap.begin();
         itor != mActorProxyMap.end(); ++itor)
         toFill.insert(&itor->second->GetActorType());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllGameActors(std::vector<GameActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mGameActorProxyMap.size());

      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor;
      for(itor = mGameActorProxyMap.begin(); itor != mGameActorProxyMap.end(); ++itor)
         toFill.push_back(itor->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllNonGameActors(std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mActorProxyMap.size());

      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor;
      for(itor = mActorProxyMap.begin(); itor != mActorProxyMap.end(); ++itor)
         toFill.push_back(itor->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllActors(std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mGameActorProxyMap.size() + mActorProxyMap.size() + mPrototypeActors.size());

      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor;
      for(itor = mGameActorProxyMap.begin(); itor != mGameActorProxyMap.end(); ++itor)
         toFill.push_back(itor->second.get());

      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator iter;
      for(iter = mActorProxyMap.begin(); iter != mActorProxyMap.end(); ++iter)
         toFill.push_back(iter->second.get());

      for(itor = mPrototypeActors.begin(); itor != mPrototypeActors.end(); ++itor)
         toFill.push_back(itor->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllPrototypes(std::vector<dtDAL::ActorProxy*> &toFill) const
   {  
      toFill.clear();
      toFill.reserve(mPrototypeActors.size());

      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor;
      for(itor = mPrototypeActors.begin(); itor != mPrototypeActors.end(); ++itor)
         toFill.push_back(itor->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetActorsInScene(std::vector<dtCore::DeltaDrawable*> &vec) const
   {
      vec.clear();
      vec.reserve(mScene->GetNumberOfAddedDrawable());

      for(unsigned int i = 0; i < mScene->GetNumberOfAddedDrawable(); i++)
         vec.push_back(mScene->GetDrawable(i));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByName(const std::string &name, std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mGameActorProxyMap.size() + mActorProxyMap.size());

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mGameActorProxyMap.begin();
          itor != mGameActorProxyMap.end(); ++itor)
      {
         if (dtUtil::Match(const_cast<char*>(name.c_str()), const_cast<char*>(itor->second->GetName().c_str())))
             toFill.push_back(itor->second.get());
      }

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor = mActorProxyMap.begin();
          itor != mActorProxyMap.end(); ++itor)
      {
          if (dtUtil::Match(const_cast<char*>(name.c_str()), const_cast<char*>(itor->second->GetName().c_str())))
             toFill.push_back(itor->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByType(const dtDAL::ActorType &type, std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mGameActorProxyMap.size() + mActorProxyMap.size());

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mGameActorProxyMap.begin();
         itor != mGameActorProxyMap.end(); ++itor)
      {
         if (itor->second->GetActorType() == type)
            toFill.push_back(itor->second.get());
      }

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor = mActorProxyMap.begin();
         itor != mActorProxyMap.end(); ++itor)
      {
         if (itor->second->GetActorType() == type)
            toFill.push_back(itor->second.get());
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByClassName(const std::string &className, 
      std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mActorProxyMap.size() + mGameActorProxyMap.size());

      if(!className.empty()) 
      {
         for(std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator i = mActorProxyMap.begin();
            i != mActorProxyMap.end(); ++i) 
         {
            dtDAL::ActorProxy *ap = i->second.get();
            if(ap->IsInstanceOf(className))
               toFill.push_back(ap);
         }

         for(std::map<dtCore::UniqueId, dtCore::RefPtr<dtGame::GameActorProxy> >::const_iterator i = mGameActorProxyMap.begin();
             i != mGameActorProxyMap.end(); ++i)
         {
            dtGame::GameActorProxy *gap = i->second.get();
            if(gap->IsInstanceOf(className))
               toFill.push_back(gap);
         }
      } 
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindPrototypesByActorType(const dtDAL::ActorType &type, std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mPrototypeActors.size());

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mPrototypeActors.begin();
         itor != mPrototypeActors.end(); ++itor)
      {
         if (itor->second->GetActorType() == type)
            toFill.push_back(itor->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindPrototypesByName(const std::string &name, std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
      toFill.reserve(mPrototypeActors.size());

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mPrototypeActors.begin();
         itor != mPrototypeActors.end(); ++itor)
      {
         if (dtUtil::Match(const_cast<char*>(name.c_str()), const_cast<char*>(itor->second->GetName().c_str())))
            toFill.push_back(itor->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxy* GameManager::FindPrototypeByID(const dtCore::UniqueId& uniqueID)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mPrototypeActors.find(uniqueID);
      if(itor != mPrototypeActors.end())
         return itor->second.get();

      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsWithinRadius(const float radius, std::vector<dtDAL::ActorProxy*> &toFill) const
   {
      toFill.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameActorProxy* GameManager::FindGameActorById(const dtCore::UniqueId &id) const
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mGameActorProxyMap.find(id);
      return itor == mGameActorProxyMap.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxy* GameManager::FindActorById(const dtCore::UniqueId &id) const
   {
      dtDAL::ActorProxy* actorProxy = FindGameActorById(id);

      if (actorProxy != NULL)
         return actorProxy;

      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor = mActorProxyMap.find(id);
      return itor == mActorProxyMap.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameManager::SaveGameState()
   {
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GameManager::LoadGameState()
   {
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::CloseCurrentMap()
   {
      if (mMapChangeStateData->GetCurrentState() != MapChangeStateData::MapChangeState::IDLE)
      {
         static std::string changeMessage("You may not close the map while a map change is already in progress.");
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, changeMessage.c_str());
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, changeMessage, __FILE__, __LINE__);
      }
      mMapChangeStateData->BeginMapChange(mLoadedMap, "", false, false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ChangeMap(const std::string &mapName, bool addBillboards, bool enableDatabasePaging) 
   {
      if (mMapChangeStateData->GetCurrentState() != MapChangeStateData::MapChangeState::IDLE)
      {
         static std::string changeMessage("You may not change the map while a map change is already in progress.");
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, changeMessage.c_str());
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, changeMessage, __FILE__, __LINE__);
      }

      if (mapName.empty())
         throw dtUtil::Exception(ExceptionEnum::INVALID_PARAMETER, "Empty string is not a valid map name.", __FILE__, __LINE__);

      mMapChangeStateData->BeginMapChange(mLoadedMap, mapName, addBillboards, enableDatabasePaging);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetTimer(const std::string& name, const GameActorProxy* aboutActor,
      float time, bool repeat, bool realTime)
   {
      TimerInfo t;
      t.name = name;

      if (aboutActor == NULL)
         t.aboutActor = dtCore::UniqueId("");
      else
         t.aboutActor = aboutActor->GetId();

      t.interval = (Timer_t)(time * 1e6);
      if(realTime)
         t.time = GetRealClockTime() + t.interval;
      else
         t.time = GetSimulationClockTime() + t.interval;

      t.repeat = repeat;
      realTime ? mRealTimeTimers.insert(t) : mSimulationTimers.insert(t);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ClearTimer(const std::string& name, const GameActorProxy *proxy)
   {
      std::set<TimerInfo>::iterator i = mRealTimeTimers.begin();
      while (i != mRealTimeTimers.end())
      {
         std::set<TimerInfo>::iterator toDelete;
         if (proxy == NULL)
         {
            if (i->name == name)
            {
               toDelete = i;
               ++i;
               mRealTimeTimers.erase(toDelete);
            }
            else
               ++i; // avoid infinite loop
         }
         else if(i->aboutActor == proxy->GetId() && i->name == name)
         {
            toDelete = i;
            ++i;
            mRealTimeTimers.erase(toDelete);
         }
         else
            ++i;
      }

      i = mSimulationTimers.begin();
      while (i != mSimulationTimers.end())
      {
         std::set<TimerInfo>::iterator toDelete;
         if (proxy == NULL)
         {
            if (i->name == name)
            {
               toDelete = i;
               ++i;
               mSimulationTimers.erase(toDelete);
            }
            else
               ++i; // avoid infinite loop
         }
         else if(i->aboutActor == proxy->GetId() && i->name == name)
         {
            toDelete = i;
            ++i;
            mSimulationTimers.erase(toDelete);
         }
         else
            ++i;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetRegistrantsForMessages(const MessageType& type, std::vector<std::pair<GameActorProxy*, std::string > >& toFill) const
   {
      toFill.clear();
      toFill.reserve(mGlobalMessageListeners.size());

      std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::const_iterator  itor
         = mGlobalMessageListeners.find(&type);

      while (itor != mGlobalMessageListeners.end() && itor->first == &type)
      {
         //add the game actor proxy and invokable name to a new pair in the vector.
         toFill.push_back(std::make_pair(itor->second.first.get(), itor->second.second));
         ++itor;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetRegistrantsForMessagesAboutActor(const MessageType& type, const dtCore::UniqueId& targetActorId, std::vector<std::pair<GameActorProxy*, std::string > >& toFill) const
   {
      toFill.clear();
      toFill.reserve(mActorMessageListeners.size());

      std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > >::const_iterator itor
         = mActorMessageListeners.find(&type);

      if (itor != mActorMessageListeners.end())
      {

         //second on itor is the internal map.
         std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::const_iterator itorInner
            = itor->second.find(targetActorId);
         while (itorInner != itor->second.end() && itorInner->first == targetActorId)
         {
            toFill.push_back(std::make_pair(itorInner->second.first.get(), itorInner->second.second));
            ++itorInner;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RegisterForMessages(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName)
   {
      mGlobalMessageListeners.insert(std::make_pair(&type, std::make_pair(dtCore::RefPtr<GameActorProxy>(&proxy), invokableName)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterForMessages(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName)
   {
      std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator itor
         = mGlobalMessageListeners.find(&type);

      while (itor != mGlobalMessageListeners.end() && itor->first == &type)
      {
         if (itor->second.first.get() == &proxy && itor->second.second == invokableName)
         {
            mGlobalMessageListeners.erase(itor);
            return;
         }
         else
         {
            ++itor;
         }

      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RegisterForMessagesAboutActor(const MessageType& type, const dtCore::UniqueId& targetActorId, GameActorProxy& proxy, const std::string& invokableName)
   {
      std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > >::iterator itor
         = mActorMessageListeners.find(&type);
      std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >* mapForType = NULL;
      if (itor == mActorMessageListeners.end())
      {
         itor = mActorMessageListeners.insert(std::make_pair(&type, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >() )).first;
         if (itor != mActorMessageListeners.end())
            mapForType = &itor->second;
         else
            //this is one of those "it should never happen" things.
            throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, 
            "Internal Error: Unable to find item just inserted in the map.", __FILE__, __LINE__);
      }
      else
      {
         mapForType = &itor->second;
      }
      mapForType->insert(std::make_pair(targetActorId, std::make_pair(dtCore::RefPtr<GameActorProxy>(&proxy), invokableName)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterForMessagesAboutActor(const MessageType& type, const dtCore::UniqueId& targetActorId, GameActorProxy& proxy, const std::string& invokableName)
   {
      std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > >::iterator itor
         = mActorMessageListeners.find(&type);

      if (itor != mActorMessageListeners.end())
      {
         //second on itor is the internal map.
         std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator itorInner
            = itor->second.find(targetActorId);
         while (itorInner != itor->second.end() && itorInner->first == targetActorId)
         {
            //second is a pair.
            //second.first is the game actor proxy to receive the message second.second is the name of the invokable
            if (itorInner->second.first.get() == &proxy && itorInner->second.second == invokableName)
            {
               std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator toDelete =
                  itorInner;
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
   void GameManager::UnregisterAllMessageListenersForActor(GameActorProxy& proxy)
   {
      for (std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator i
         = mGlobalMessageListeners.begin(); i != mGlobalMessageListeners.end();)
      {
         std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator toDelete = i;
         ++i;
         if (toDelete->second.first.get() == &proxy)
         {
            mGlobalMessageListeners.erase(toDelete);
         }

      }

      for (std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > >::iterator i
         = mActorMessageListeners.begin(); i != mActorMessageListeners.end(); ++i)
      {
         for (std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator j
            = i->second.begin(); j != i->second.end();)
         {
            std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator toDelete = j;
            ++j;
            if (toDelete->first == proxy.GetId() || toDelete->second.first.get() == &proxy)
            {
               i->second.erase(toDelete);
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RejectMessage(const dtGame::Message &reasonMessage, const std::string &rejectDescription)
   {
      const MachineInfo &causingMachine = reasonMessage.GetSource();
      dtCore::RefPtr<Message> rejPtrMsg = mFactory.CreateMessage(MessageType::SERVER_REQUEST_REJECTED);
      ServerMessageRejected *rejectMsg = static_cast<ServerMessageRejected *> (rejPtrMsg.get());
      rejectMsg->SetCausingMessage(&reasonMessage);
      rejectMsg->SetCause(rejectDescription);
      rejectMsg->SetDestination(&causingMachine);

      // if it's from us, just process it
      if (*mMachineInfo == causingMachine)
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
   void GameManager::SetProjectContext(const std::string &context, bool readOnly)
   {
      dtDAL::Project::GetInstance().SetContext(context, readOnly);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string& GameManager::GetProjectContext() const
   {
      return dtDAL::Project::GetInstance().GetContext();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ProcessTimers(std::set<TimerInfo> &listToProcess, Timer_t clockTime)
   {
      std::set<TimerInfo>::iterator itor;
      std::set<TimerInfo> repeatingTimers;
      for(itor=listToProcess.begin(); itor!=listToProcess.end();)
      {
         if(itor->time <= clockTime)
         {
            dtCore::RefPtr<TimerElapsedMessage> timerMsg =
               static_cast<TimerElapsedMessage*>(mFactory.CreateMessage(MessageType::INFO_TIMER_ELAPSED).get());

            timerMsg->SetTimerName(itor->name);
            float lateTime = float((clockTime - itor->time));
            // convert from microseconds to seconds
            lateTime /= 1e6;
            timerMsg->SetLateTime(lateTime);
            timerMsg->SetAboutActorId(itor->aboutActor);
            SendMessage(*timerMsg.get());
            if(itor->repeat)
            {
               TimerInfo tInfo = *itor;
               tInfo.time += tInfo.interval;
               repeatingTimers.insert(tInfo);
            }

            std::set<TimerInfo>::iterator toDelete = itor;
            ++itor;
            listToProcess.erase(toDelete);
         }
         else
            break;
      }

      //Repeating timers have to be readded so they are processed again later
      listToProcess.insert(repeatingTimers.begin(), repeatingTimers.end());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SendEnvironmentChangedMessage(EnvironmentActorProxy *envActor)
   {
      dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ENVIRONMENT_CHANGED);
      msg->SetAboutActorId(mEnvironment.valid() ? envActor->GetActor()->GetUniqueId() : dtCore::UniqueId(""));
      msg->SetSource(*mMachineInfo);
      SendMessage(*msg);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   /*                            Statistics Information                          */
   ////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   void GameManager::UpdateDebugStats(const dtCore::UniqueId &uniqueIDToFind, 
      const std::string& nameOfObject, float elapsedTime, bool isComponent, bool ticklocal)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator itor = 
         mDebugLoggerInformation.find(uniqueIDToFind);
      if (itor != mDebugLoggerInformation.end())
      {
         LogDebugInformation& debugInfo = *itor->second;
         if(ticklocal)
         {
            debugInfo.mTickLocalTime +=elapsedTime;
            debugInfo.mTimesThrough += 1;
            debugInfo.mTotalTime += elapsedTime; 
         }
         else
         {
            debugInfo.mTotalTime += elapsedTime;  
         }
      }
      else 
      {
         dtCore::RefPtr<LogDebugInformation> toPushDebugInfo = new LogDebugInformation(nameOfObject, uniqueIDToFind, isComponent);
         toPushDebugInfo->mTotalTime = elapsedTime;
         mDebugLoggerInformation.insert(std::make_pair(uniqueIDToFind, toPushDebugInfo));
      }
   }


   //////////////////////////////////////////////////////////////////////////////
   void GameManager::DebugStatisticsTurnOn(bool logComponents, bool logActors, 
      const int statisticsInterval, bool toConsole, const std::string& path)
   {
      mDoStatsOnTheComponents    = logComponents;
      mDoStatsOnTheActors        = logActors;
      mPrintFileToConsole        = toConsole;
      mFilePathToPrintDebugInformation = path;
      mStatisticsInterval        = statisticsInterval;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DebugStatisticsTurnOff(bool logLastTime, bool clearList)
   {
      mDoStatsOnTheComponents    = false;
      mDoStatsOnTheActors        = false;
      mStatisticsInterval        = 0;
      if(logLastTime)
      {
         // temporarily 100, fill in to correct time if needbe.
         DebugStatisticsPrintOut(100);
      }

      if(clearList)
      {
         mDebugLoggerInformation.clear();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::Shutdown()
   {
      std::vector<GMComponent*> components;
      GetAllComponents(components);
      for(size_t i = 0; i < components.size(); i++)
      {
         RemoveComponent(*components[i]);
      }
      
      mDebugLoggerInformation.clear();
     
      while(!mSendNetworkMessageQueue.empty())
      {    
         mSendNetworkMessageQueue.pop();
      }
      
      while(!mSendMessageQueue.empty())
      {
         mSendMessageQueue.pop();
      }

      DeleteAllActors(true);

      mMapChangeStateData = NULL;
      mApplication        = NULL;
      mMachineInfo        = NULL;
      mLibMgr = NULL;
      mScene  = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DebugStatisticsPrintOut(const float realTimeElapsed)
   {
      // real time is milliseconds. Convert to seconds (*1000) and then trunc to 4 digits 
      float truncRealTime = ((int)(realTimeElapsed / 10.0)) / 100000.0; 
      float truncCumGMTime = ((int)((float) mStatsCumGMProcessTime / 10.0)) / 100000.0; 
      float gmPercentTime = ComputeStatsPercent(truncRealTime, truncCumGMTime);
      float cumulativeTime = 0.0;
      int numComps = 0, numActors = 0;
      std::vector<dtCore::RefPtr<LogDebugInformation> > debugDeleteList;

      std::ostringstream ss;
      ss << "==========Printing Debug information===========" << 
#ifdef _DEBUG
         "  [DEBUG BUILD]  " << std::endl;
#else
         "  [RELEASE BUILD]  " << std::endl;
#endif

      float fps = ((int)((mStatsNumFrames/truncRealTime) * 10.0)) / 10.0; // force data truncation to 1 place
      ss << "GM Stats: CurSimTime[" << GetSimulationTime() << "], TimeInGM[" << gmPercentTime << 
         "%, " << truncCumGMTime << "s], ReportTime[" << truncRealTime << 
         "s], Ticks[" << mStatsNumFrames << "], FPS[" << fps << 
         "], #Msgs[" << mStatsNumProcMessages << " Local/" << mStatsNumSendNetworkMessages <<
         " Ntwrk], #Actors[" << mActorProxyMap.size() << "/" << mGameActorProxyMap.size() << 
         " Game/" << mActorProxyMap.size() << "]" << std::endl;
      
      // reset values for next fragment
      mStatsNumFrames         = 0;
      mStatsNumProcMessages   = 0;
      mStatsCumGMProcessTime  = 0;
      mStatsNumSendNetworkMessages = 0;

      // Build up all the information in the stream
      std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator iter = mDebugLoggerInformation.begin();
      if(mDoStatsOnTheComponents)
      {
         ss << "*************** STARTING LOGGING OF TIME IN COMPONENTS *****************" << std::endl;
         for(; iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            if(debugInfo.mIsComponent)
            {
               float percentTime = ComputeStatsPercent(truncRealTime, debugInfo.mTotalTime);
               float truncTotalTime = ((int)(debugInfo.mTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
               ss << "* Time[" << percentTime << "% / " << truncTotalTime << " Total], Name[" << 
                  debugInfo.mNameOfLogInfo.c_str() << "]" << std::endl;
               debugInfo.mTotalTime = 0;
               debugInfo.mTimesThrough = 0;
               debugInfo.mTickLocalTime = 0;
               cumulativeTime += truncTotalTime;
               numComps += 1;

               if (debugInfo.mTotalTime == 0.0)
                  debugDeleteList.push_back(iter->second);
            }
         }
         ss << "*************** ENDING LOGGING OF TIME IN COMPONENTS *****************" << std::endl;
      }

      // ACTORs
      if(mDoStatsOnTheActors)
      {
         int numIgnored = 0;
         float ignoredCumulativeTime = 0.0;

         ss << "********** STARTING LOGGING OF TIME IN ACTORS -- if (> 0.1%) ************" << std::endl;
         for(iter = mDebugLoggerInformation.begin(); iter != mDebugLoggerInformation.end(); ++iter)
         {
            LogDebugInformation& debugInfo = *iter->second;
            if(!debugInfo.mIsComponent)
            {
               float percentTime = ComputeStatsPercent(truncRealTime, debugInfo.mTotalTime);
               float truncTotalTime = ((int)(debugInfo.mTotalTime * 10000)) / 10000.0; // force data truncation to 4 places
               if (percentTime > 0.1)
               {
                  ss << "* Time[" << percentTime << "% / " << truncTotalTime << " Total], Name[" << 
                     debugInfo.mNameOfLogInfo.c_str() << "]" << 
                     ", UniqueId[" << debugInfo.mUniqueID.ToString().c_str() << "]" << std::endl;
               }
               else if (debugInfo.mTotalTime == 0.0)
                  debugDeleteList.push_back(iter->second);
               else 
               {
                  numIgnored ++;
                  ignoredCumulativeTime += truncTotalTime;
               }
               debugInfo.mTotalTime = 0;
               debugInfo.mTimesThrough = 0;
               debugInfo.mTickLocalTime = 0;
               cumulativeTime += truncTotalTime;
               numActors += 1;
            }
         }
         // ignored actors -- too much data with 500+ actors for one actor per line
         if (numIgnored > 0)
         {
            float percentTime = ComputeStatsPercent(truncRealTime, ignoredCumulativeTime);
            ss << "*** Ignored [" << numIgnored << "] actors for [" << ignoredCumulativeTime << 
               ", " << percentTime << "%]." << std::endl;
         }
         ss << "************ ENDING LOGGING OF TIME IN ACTORS *********************" << std::endl;
      }
      // total stats
      if (mDoStatsOnTheComponents || mDoStatsOnTheActors)
      {
         float percentTime = ComputeStatsPercent(truncRealTime, cumulativeTime);
         ss << "* Cumulative Time [" << percentTime << "%, " << cumulativeTime << "s] for ";
         if (mDoStatsOnTheActors)
            ss << "[" << numActors << " actors]";
         if (mDoStatsOnTheComponents)
            ss << "[" << numComps << " comps]";
         ss << std::endl;
      }
      ss << "============ Ending Debug information ==============" << std::endl;

      // Do the writing
      if(mPrintFileToConsole)
      {
         mLogger->LogMessage(__FUNCTION__, __LINE__, ss.str(), dtUtil::Log::LOG_ALWAYS);
      }
      else // print to file
      {
         FILE* temp = fopen(mFilePathToPrintDebugInformation.c_str(), "a");
         fprintf(temp, "%s", ss.str().c_str());
         fclose(temp);
      }

      // delete items that had no time this report. Easy way to clean up old components and actors
      for (unsigned int i = 0; i < debugDeleteList.size(); ++i)
      {
         LogDebugInformation &debugInfo = *debugDeleteList[i];
         std::map<dtCore::UniqueId, dtCore::RefPtr<LogDebugInformation> >::iterator deleteIter = 
            mDebugLoggerInformation.find(debugInfo.mUniqueID);
         if (deleteIter != mDebugLoggerInformation.end())
            mDebugLoggerInformation.erase(deleteIter);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   float GameManager::ComputeStatsPercent(const float total, const float partial) const
   {
      float returnValue = 0.0;

      if (total > 0)
      {
         returnValue = 1.0 - ((total - partial) / (float)total);
         returnValue = ((int)(returnValue * 1000)) / 10.0; // force data truncation
      }

      return returnValue;
   }
} // end namespace
