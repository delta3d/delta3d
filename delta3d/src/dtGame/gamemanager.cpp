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
* @author Matthew W. Campbell, William E. Johnson II, David Guthrie
*/

#include "dtGame/gamemanager.h"
#include "dtGame/messagefactory.h"
#include "dtGame/basemessages.h"
#include "dtGame/actorupdatemessage.h"
#include "dtGame/exceptionenum.h"
#include "dtGame/gmcomponent.h"
#include <dtDAL/actortype.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtCore/system.h>
#include <dtUtil/stringutils.h>

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
      mScene(&scene),  
      mFactory("GameManager MessageFactory",*mMachineInfo,""),
      mStatisticsInterval(0),
      mStatsLastFragmentDump(0),
      mStatsNumProcMessages(0),
      mStatsNumSendMessages(0),
      mStatsNumFrames(0),
      mStatsCumGMProcessTime(0)
      
   {
      mLibMgr = &dtDAL::LibraryManager::GetInstance();
      mLogger = &dtUtil::Log::GetInstance("gamemanager.cpp");
      AddSender(dtCore::System::Instance());
      mPaused = dtCore::System::Instance()->GetPause();

   }
   
   ///////////////////////////////////////////////////////////////////////////////
   GameManager::GameManager(const GameManager&):
      mMachineInfo(new MachineInfo()),
      mFactory("GameManager MessageFactory",*mMachineInfo,"") {}

   ///////////////////////////////////////////////////////////////////////////////
   GameManager& GameManager::operator=(const GameManager&) {return *this;}

   ///////////////////////////////////////////////////////////////////////////////
   GameManager::~GameManager() 
   {
      //RemoveSender(dtCore::System::Instance());
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
   void GameManager::GetActorTypes(std::vector<dtCore::RefPtr<dtDAL::ActorType> > &actorTypes)
   {
      mLibMgr->GetActorTypes(actorTypes);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorType> GameManager::FindActorType(const std::string &category, const std::string &name)
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
   void GameManager::SendMessage(const Message& message)
   {
      mSendMessageQueue.push(dtCore::RefPtr<const Message>(&message));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ProcessMessage(const Message& message)
   {
      mProcessMessageQueue.push(dtCore::RefPtr<const Message>(&message));  
   }

   ///////////////////////////////////////////////////////////////////////////////
   float GameManager::GetTimeScale() const 
   { 
      return dtCore::System::Instance()->GetTimeScale(); 
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   double GameManager::GetSimulationTime() const
   { 
      return dtCore::System::Instance()->GetSimulationTime(); 
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Timer_t GameManager::GetSimulationClockTime() const
   {
      return dtCore::System::Instance()->GetSimulationClockTime();    
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Timer_t GameManager::GetRealClockTime() const
   {
      return dtCore::System::Instance()->GetRealClockTime();
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ChangeTimeSettings(double newTime, float newTimeScale, dtCore::Timer_t newClockTime)
   {
      dtCore::System::Instance()->SetSimulationClockTime(newClockTime);
      dtCore::System::Instance()->SetSimulationTime(newTime);
      dtCore::System::Instance()->SetTimeScale(newTimeScale);
      
      dtCore::RefPtr<Message> timeChangeMsg = GetMessageFactory().CreateMessage(MessageType::INFO_TIME_CHANGED);
      TimeChangeMessage* tcm = static_cast<TimeChangeMessage*>(timeChangeMsg.get());
      tcm->SetSimulationTime(newTime);
      tcm->SetSimulationClockTime(newClockTime);
      tcm->SetTimeScale(newTimeScale);
      ProcessMessage(*timeChangeMsg);      
   }  
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::SetPaused(bool pause)
   {
      if (mPaused == pause)
         return;
         
      mPaused = pause;
      dtCore::System::Instance()->SetPause(mPaused);
      if (mPaused)
      {
         ProcessMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_PAUSED));
      }
      else
      {
         ProcessMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_RESUMED));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PreFrame(double deltaSimTime, double deltaRealTime)
   {
      // stats information used to track statistics per fragment (usually about 1 second)
      dtCore::Timer statsTickClock; 
      dtCore::Timer_t frameTickStart;
      
      if (mStatisticsInterval > 0) 
         frameTickStart = statsTickClock.tick();

      // SEND MESSAGES - Forward Send Messages to all components (no actors)
      while (!mSendMessageQueue.empty())
      {
         mStatsNumSendMessages += 1;
         dtCore::RefPtr<const Message> message = mSendMessageQueue.front();
         mSendMessageQueue.pop();
         for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
            (*i)->SendMessage(*message);
      }

      double simulationTime = dtCore::System::Instance()->GetSimulationTime();

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
      
      ProcessMessage(*tick);
      ProcessMessage(*tickRemote);
      
      ProcessTimers(mRealTimeTimers, GetRealClockTime());
      ProcessTimers(mSimulationTimers, GetSimulationClockTime());

      // PROCESS MESSAGES - Send all Process messages to components and interested actors
      while (!mProcessMessageQueue.empty())
      {
         mStatsNumProcMessages += 1;

         // Forward to Components first
         dtCore::RefPtr<const Message> message = mProcessMessageQueue.front();
         mProcessMessageQueue.pop();
         for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
         {
            (*i)->ProcessMessage(*message);
         }

         // GLOBAL INVOKABLES - Process it on globally registered invokables 
         const MessageType &msgType = message->GetMessageType();
         std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator  itor 
            = mGlobalMessageListeners.find(&msgType);
         
         while (itor != mGlobalMessageListeners.end() && itor->first == &msgType) 
         {
            std::pair<dtCore::RefPtr<GameActorProxy>, std::string >& listener = itor->second;
            Invokable* invokable = listener.first->GetInvokable(listener.second);
            if (invokable != NULL)
            {
               invokable->Invoke(*message);
            }
            else
            {
               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, 
                                      "Invokable named %s is registered as a listener, but Proxy %s does not have an invokable by that name.",
                                      listener.second.c_str(), listener.first->GetActorType().GetName().c_str());
            }
            ++itor;
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
               for (unsigned i = 0; i < aboutActorInvokables.size(); ++i)
               {
                  aboutActorInvokables[i]->Invoke(*message);
               }
            }
            
            //next, sent it to all actors listening to that actor for that message type.
            // TODO - This should be refactored like we did for the Global Invokables a few lines up. It should work with the map directly instead of filling lists repeatedly
            std::vector<std::pair<GameActorProxy*, std::string > > toFill;
            GetGameActorMessageListeners(message->GetMessageType(), message->GetAboutActorId(), toFill);
            for (unsigned i = 0; i < toFill.size(); ++i)
            {
               std::pair<GameActorProxy*, std::string >& listener = toFill[i];
               Invokable* invokable = listener.first->GetInvokable(listener.second);
               if (invokable != NULL)
               {
                  invokable->Invoke(*message);
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
      for (unsigned int i = 0; i < mDeleteList.size(); ++i)
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
         dtCore::Timer_t frameTickStop = statsTickClock.tick();
         double fragmentDelta = statsTickClock.delta_u(mStatsLastFragmentDump, frameTickStop);
         mStatsCumGMProcessTime += (dtCore::Timer_t)(statsTickClock.delta_u(frameTickStart, frameTickStop));

         if (fragmentDelta < 0) // handle wierd case of wrap around (just to be safe)
            mStatsLastFragmentDump = frameTickStop;
            
         else if (fragmentDelta >= (mStatisticsInterval * 1000000))
         {
            dtCore::Timer_t realTimeElapsed = (dtCore::Timer_t)statsTickClock.delta_u(mStatsLastFragmentDump, frameTickStop);
            float gmPercentTime = ComputeStatsPercent(realTimeElapsed, mStatsCumGMProcessTime);

            std::ostringstream ss;
            ss << "GM Stats: SimTime[" << GetSimulationTime() << "], TimeInGM[" << gmPercentTime << "%], Ticks[" << 
                  mStatsNumFrames << "], #Msgs[" << mStatsNumProcMessages << " P/" << mStatsNumSendMessages << 
                  " S], #Actors[" << mActorProxyMap.size() << "/" << mGameActorProxyMap.size() << " Game]";
            mLogger->LogMessage(__FUNCTION__, __LINE__, ss.str(), dtUtil::Log::LOG_ALWAYS);

            // reset values for next fragment
            mStatsLastFragmentDump = frameTickStop;
            mStatsNumFrames = 0;
            mStatsNumProcMessages = 0;
            mStatsNumSendMessages = 0;
            mStatsCumGMProcessTime = 0;
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
      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RemoveComponent(GMComponent& component)
   {
      for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
      {
         if (i->get() == &component)
         {
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
      for (unsigned i = 0; i < mComponentList.size(); ++i)
      {
         toFill.push_back(mComponentList[i].get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllComponents(std::vector<const GMComponent*>& toFill) const
   {
      toFill.clear();
      for (unsigned i = 0; i < mComponentList.size(); ++i)
      {
         toFill.push_back(mComponentList[i].get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProxy> GameManager::CreateActor(dtDAL::ActorType& actorType) throw(dtUtil::Exception)
   {
      try 
      {
         dtCore::RefPtr<dtDAL::ActorProxy> ap = dtDAL::LibraryManager::GetInstance().CreateActorProxy(actorType).get();
         if (ap->IsInstanceOf("dtGame::GameActor"))
         {
            dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());          
            if (gap != NULL)
            {
               gap->BuildInvokables();
            }
            else
               EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, "ERROR: Actor has the type of a GameActor, but casting it to a GameActorProxy failed.");
            
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
   void GameManager::AddActor(dtDAL::ActorProxy& actorProxy) 
   {
      mActorProxyMap.insert(std::make_pair(actorProxy.GetId(), &actorProxy));
      mScene->AddDrawable(actorProxy.GetActor());
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddActor(GameActorProxy& gameActorProxy, bool isRemote, bool publish) throw(dtUtil::Exception)
   {
      
      gameActorProxy.SetRemote(isRemote);
      
      gameActorProxy.SetGameManager(this);
      gameActorProxy.InvokeEnteredWorld();

      mGameActorProxyMap.insert(std::make_pair(gameActorProxy.GetId(), &gameActorProxy));
      mScene->AddDrawable(gameActorProxy.GetActor());
      
      if (!isRemote) 
      {
         dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_CREATED);
         gameActorProxy.PopulateActorUpdate((ActorUpdateMessage&)*msg);
         ProcessMessage(*msg); 
      }
      if (publish)
         PublishActor(gameActorProxy);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PublishActor(GameActorProxy& gameActorProxy)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(gameActorProxy.GetId());
      
      if (itor == mGameActorProxyMap.end())
      {
         EXCEPT(ExceptionEnum::INVALID_ACTOR_STATE, "A GameActor may only be published if it's added to the GameManager as a game actor.");
      }
      
      if (gameActorProxy.IsRemote())
         EXCEPT(ExceptionEnum::ACTOR_IS_REMOTE, "A remote game actor may not be published");
      
      gameActorProxy.SetPublished(true);
      dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_PUBLISHED);
      msg->SetDestination(&GetMachineInfo());
      msg->SetAboutActorId(gameActorProxy.GetId());
      msg->SetSendingActorId(gameActorProxy.GetId());
      ProcessMessage(*msg);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteActor(dtDAL::ActorProxy& actorProxy)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(actorProxy.GetId());
      
      dtCore::UniqueId id;
      if (itor == mGameActorProxyMap.end())
      {
         //it's not in the game manager as a game actor proxy, maybe it's in there
         //as a regular actor proxy.
         std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor = mActorProxyMap.find(actorProxy.GetId());
   
         if (itor != mActorProxyMap.end())
         {
            mActorProxyMap.erase(itor);
            RemoveActorFromScene(actorProxy);
         }
      }
      else
      {
         id = itor->first;
         GameActorProxy& gameActorProxy = *itor->second;
         mDeleteList.push_back(itor->second);
         if (!gameActorProxy.IsRemote())
         {
         
            dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_DELETED);
            msg->SetAboutActorId(id);  
      
            ProcessMessage(*msg);
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
         mActorProxyMap.clear();
         
         mGlobalMessageListeners.clear();
         mActorMessageListeners.clear();
         mGameActorProxyMap.clear();
      
         //all the actors are deleted now, so the problems with clearing the list 
         //of deleted actors is not a problem.
         mDeleteList.clear();
      }
      else
      {
         for(std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator i = mGameActorProxyMap.begin(); 
            i != mGameActorProxyMap.end(); ++i)
         {
            DeleteActor(*i->second);           
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetActorTypes(std::vector<const dtDAL::ActorType*> &toFill) const
   {
      toFill.clear();

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::const_iterator itor = mGameActorProxyMap.begin();
         itor != mGameActorProxyMap.end(); ++itor)
         toFill.push_back(&itor->second->GetActorType());

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor = mActorProxyMap.begin();
         itor != mActorProxyMap.end(); ++itor)
         toFill.push_back(&itor->second->GetActorType());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllGameActors(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill)
   {
      toFill.clear();

      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor;
      for(itor = mGameActorProxyMap.begin(); itor != mGameActorProxyMap.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllNonGameActors(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill)
   {
      toFill.clear();

      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor;
      for(itor = mActorProxyMap.begin(); itor != mActorProxyMap.end(); ++itor)
         toFill.push_back(itor->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetAllActors(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill)
   {
      toFill.clear();

      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor;
      for(itor = mGameActorProxyMap.begin(); itor != mGameActorProxyMap.end(); ++itor)
         toFill.push_back(itor->second.get());

      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator iter;
      for(iter = mActorProxyMap.begin(); iter != mActorProxyMap.end(); ++iter)
         toFill.push_back(iter->second);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByName(const std::string &name, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill)
   {
      toFill.clear();

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.begin();
          itor != mGameActorProxyMap.end(); ++itor)
      {
         if (dtUtil::Match(const_cast<char*>(name.c_str()), const_cast<char*>(itor->second->GetName().c_str())))
             toFill.push_back(itor->second.get());
      }

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor = mActorProxyMap.begin();
          itor != mActorProxyMap.end(); ++itor)
      {
          if (dtUtil::Match(const_cast<char*>(name.c_str()), const_cast<char*>(itor->second->GetName().c_str())))
             toFill.push_back(itor->second);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsByType(const dtDAL::ActorType &type, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill)
   {
      toFill.clear();

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.begin();
         itor != mGameActorProxyMap.end(); ++itor)
      {
         if (itor->second->GetActorType() == type)
            toFill.push_back(itor->second.get());
      }

      for (std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor = mActorProxyMap.begin();
         itor != mActorProxyMap.end(); ++itor)
      {
         if (itor->second->GetActorType() == type)
            toFill.push_back(itor->second);
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::FindActorsWithinRadius(const float radius, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill)
   {
      toFill.clear(); 
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameActorProxy* GameManager::FindGameActorById(const dtCore::UniqueId &id)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(id);
      return itor == mGameActorProxyMap.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxy* GameManager::FindActorById(const dtCore::UniqueId &id)
   {
      dtDAL::ActorProxy* actorProxy = FindGameActorById(id);
      
      if (actorProxy != NULL)
         return actorProxy;
      
      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor = mActorProxyMap.find(id);
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
   void GameManager::ChangeMap(const std::string &mapName, bool addBillboards, bool enableDatabasePaging) throw(dtUtil::Exception)
   {
      dtDAL::Map &map = dtDAL::Project::GetInstance().GetMap(mapName);
   
      std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;
      map.GetAllProxies(proxies);

      //delete all actors after making sure the map loaded correctly.
      DeleteAllActors(true);

      //Close the old map.
      if (!mLoadedMap.empty())
      {
         dtDAL::Map &oldMap = dtDAL::Project::GetInstance().GetMap(mLoadedMap);
         dtDAL::Project::GetInstance().CloseMap(oldMap, true);
      }

      mRealTimeTimers.clear();
      mSimulationTimers.clear();
            
      //Set the loaded map now even if the code later fails because we
      //want to close the map on the next change.
      mLoadedMap = mapName;
      
      for (unsigned int i = 0; i < proxies.size(); i++)
      {
         if (proxies[i]->IsInstanceOf("dtGame::GameActor"))
         {
            dtCore::RefPtr<GameActorProxy> proxy = dynamic_cast<GameActorProxy*> (proxies[i].get());  
            if (proxy.valid())
            {
               proxy->BuildInvokables();
               mGameActorProxyMap.insert(std::make_pair(proxy->GetId(), proxy));
               proxy->SetGameManager(this);
            }
            else
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "ERROR: Actor has the type of a GameActor, but casting it to a GameActorProxy failed.  Actor %s of type %s.%s will not be added to the scene.",
                  proxies[i]->GetName().c_str(), proxies[i]->GetActorType().GetCategory().c_str(), proxies[i]->GetActorType().GetName().c_str());
         }
         else
         {
            mActorProxyMap.insert(std::make_pair(proxies[i]->GetId(), dtCore::RefPtr<dtDAL::ActorProxy>(proxies[i].get())));
         }
                  
         //mScene->AddDrawable(proxies[i]->GetActor());

      }
      dtDAL::Project::GetInstance().LoadMapIntoScene(map, *mScene.get(), addBillboards, enableDatabasePaging);
      
      for (unsigned int i = 0; i < proxies.size(); i++)
      {
         if (proxies[i]->IsGameActorProxy())
         {
            dtCore::RefPtr<GameActorProxy> proxy = dynamic_cast<GameActorProxy*> (proxies[i].get());  
            proxy->InvokeEnteredWorld();

            if (proxy->GetInitialOwnership() == GameActorProxy::Ownership::SERVER_PUBLISHED)
               PublishActor(*proxy);
         }
      }

      dtCore::RefPtr<Message> refMsg = mFactory.CreateMessage(MessageType::INFO_MAP_LOADED);
      MapLoadedMessage *loadedMsg = static_cast<MapLoadedMessage*>(refMsg.get());
      loadedMsg->SetLoadedMapName(mapName);
      ProcessMessage(*loadedMsg); // rules component determines whether to send and/or process 
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
         
      t.interval = (dtCore::Timer_t)(time * 1e6);
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
   void GameManager::GetGlobalMessageListeners(const MessageType& type, std::vector<std::pair<GameActorProxy*, std::string > >& toFill)
   {
      toFill.clear();
      std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator  itor 
         = mGlobalMessageListeners.find(&type);
      
      while (itor != mGlobalMessageListeners.end() && itor->first == &type) 
      {
         //add the game actor proxy and invokable name to a new pair in the vector.
         toFill.push_back(std::make_pair(itor->second.first.get(), itor->second.second));
         ++itor;
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::GetGameActorMessageListeners(const MessageType& type, const dtCore::UniqueId& targetActorId, std::vector<std::pair<GameActorProxy*, std::string > >& toFill)
   {
      toFill.clear();
      std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > >::iterator itor 
         = mActorMessageListeners.find(&type);
      
      if (itor != mActorMessageListeners.end())
      {
         
         //second on itor is the internal map.
         std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> >::iterator itorInner 
            = itor->second.find(targetActorId);
         while (itorInner != itor->second.end() && itorInner->first == targetActorId)
         {
            toFill.push_back(std::make_pair(itorInner->second.first.get(), itorInner->second.second));
            ++itorInner;
         }
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::RegisterGlobalMessageListener(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName)
   {
      mGlobalMessageListeners.insert(std::make_pair(&type, std::make_pair(dtCore::RefPtr<GameActorProxy>(&proxy), invokableName)));
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterGlobalMessageListener(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName)
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
   void GameManager::RegisterGameActorMessageListener(const MessageType& type, const dtCore::UniqueId& targetActorId, GameActorProxy& proxy, const std::string& invokableName)
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
            EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, "Internal Error: Unable to find item just inserted in the map.");
      }
      else
      {
         mapForType = &itor->second;         
      }
      mapForType->insert(std::make_pair(targetActorId, std::make_pair(dtCore::RefPtr<GameActorProxy>(&proxy), invokableName)));
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::UnregisterGameActorMessageListener(const MessageType& type, const dtCore::UniqueId& targetActorId, GameActorProxy& proxy, const std::string& invokableName)
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
         ProcessMessage(*rejectMsg);
      } 
      // if from someone else, only send it.  Else, if we are the server GM, our local components
      // may try to process a reject message that was meant for a client - most components assume 
      // a reject message was only sent to them
      else 
      {
         SendMessage(*rejectMsg);
      }

   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ProcessTimers(std::set<TimerInfo> &listToProcess, dtCore::Timer_t clockTime)
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
            ProcessMessage(*timerMsg.get());
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
   float GameManager::ComputeStatsPercent(dtCore::Timer_t total, dtCore::Timer_t partial)
   {
      float returnValue = 0.0;

      if (total > 0)
      {
         returnValue = 1.0 - ((total - partial) / (float)total);
         returnValue = ((int)(returnValue * 1000)) / 10.0; // force data truncation
      } 

      return returnValue;
   }

}
