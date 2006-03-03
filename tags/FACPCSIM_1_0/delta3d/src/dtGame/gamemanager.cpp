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
#include <dtDAL/actortype.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtCore/system.h>
#include <dtUtil/stringutils.h>

namespace dtGame 
{
   
   IMPLEMENT_MANAGEMENT_LAYER(GameManager)
   
   ///////////////////////////////////////////////////////////////////////////////
   GameManager::GameManager(dtCore::Scene &scene) : 
      mMachineInfo(new MachineInfo()),
      mScene(&scene),  
      mFactory("GameManager MessageFactory",*mMachineInfo,"")
   {
      mLibMgr = &dtDAL::LibraryManager::GetInstance();
      mLogger = &dtUtil::Log::GetInstance("gamemanager.cpp");
      AddSender(dtCore::System::Instance());
      mPaused = dtCore::System::Instance()->GetPause();
   }

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
      while (!mSendMessageQueue.empty())
      {
         dtCore::RefPtr<const Message> message = mSendMessageQueue.front();
         mSendMessageQueue.pop();
         for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
            (*i)->SendMessage(*message);
      }

      double simulationTime = dtCore::System::Instance()->GetSimulationTime();

      dtCore::RefPtr<Message> tick = GetMessageFactory().CreateMessage(MessageType::TICK_LOCAL);
      
      TickMessage& tickMessage = static_cast<TickMessage&>(*tick);
      tickMessage.SetDeltaRealTime((float)deltaRealTime);
      tickMessage.SetDeltaSimTime((float)deltaSimTime);
      tickMessage.SetSimTimeScale(GetTimeScale());
      tickMessage.SetDestination(&GetMachineInfo());
      tickMessage.SetSimulationTime(simulationTime);
      
      dtCore::RefPtr<Message> tickRemote = GetMessageFactory().CreateMessage(MessageType::TICK_REMOTE);
      
      TickMessage& tickRemoteMessage = static_cast<TickMessage&>(*tickRemote);
      tickRemoteMessage.SetDeltaRealTime((float)deltaRealTime);
      tickRemoteMessage.SetDeltaSimTime((float)deltaSimTime);
      tickRemoteMessage.SetSimTimeScale(GetTimeScale());
      tickRemoteMessage.SetDestination(&GetMachineInfo());
      tickRemoteMessage.SetSimulationTime(simulationTime);
      
      ProcessMessage(*tick);
      ProcessMessage(*tickRemote);
      
      //TODO send timer messages
      
      while (!mProcessMessageQueue.empty())
      {
         dtCore::RefPtr<const Message> message = mProcessMessageQueue.front();
         mProcessMessageQueue.pop();
         for (std::vector<dtCore::RefPtr<GMComponent> >::iterator i = mComponentList.begin(); i != mComponentList.end(); ++i)
         {
            (*i)->ProcessMessage(*message);
         }

         std::vector<std::pair<GameActorProxy*, std::string > > toFill;
         GetGlobalMessageListeners(message->GetMessageType(), toFill);
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

      for (unsigned i = 0; i < mDeleteList.size(); ++i)
      {
         GameActorProxy& gameActorProxy = *mDeleteList[i];
         
         std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(gameActorProxy.GetId());
         
         dtCore::UniqueId id;
         if (itor != mGameActorProxyMap.end())
         {
            id = itor->first;
            UnregisterAllMessageListenersForActor(*itor->second);
            mGameActorProxyMap.erase(itor);
            mScene->RemoveDrawable(&gameActorProxy.GetGameActor());
         }
         
         gameActorProxy.SetGameManager(NULL);
         
      }
      mDeleteList.clear();
      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::PostFrame()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::AddComponent(GMComponent& component)
   {
      component.SetGameManager(this);
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
   void GameManager::DeleteActor(GameActorProxy& gameActorProxy)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator itor = mGameActorProxyMap.find(gameActorProxy.GetId());
      
      dtCore::UniqueId id;
      if (itor != mGameActorProxyMap.end())
      {
         id = itor->first;
         mDeleteList.push_back(itor->second);
      }
      if (!gameActorProxy.IsRemote())
      {
      
         dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_DELETED);
         msg->SetAboutActorId(id);  
   
         ProcessMessage(*msg);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteAllActors(bool sendMessage)
   {
      mScene->RemoveAllDrawables();
      mActorProxyMap.clear();
      
      std::vector<dtCore::RefPtr<dtGame::Message> > deleteMessages;
      
      if (sendMessage)
      {
         for(std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> >::iterator i = mGameActorProxyMap.begin(); 
            i != mGameActorProxyMap.end(); ++i)
         {
            if (!i->second->IsRemote())
            {            
               dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ACTOR_DELETED);
               msg->SetAboutActorId(i->first);
               i->second->SetGameManager(NULL);
               deleteMessages.push_back(msg);
            }
         }
      }
      
      mGlobalMessageListeners.clear();
      mActorMessageListeners.clear();
      mGameActorProxyMap.clear();
      //all the actors are deleted now, so the problems with clearing the list 
      //of deleted actors is not a problem.
      mDeleteList.clear();
      
      if (sendMessage)
      {
         for (unsigned i = 0; i < deleteMessages.size(); ++i)
         {
            ProcessMessage(*deleteMessages[i]);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::DeleteActor(dtDAL::ActorProxy& actorProxy)
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor = mActorProxyMap.find(actorProxy.GetId());

      if (itor != mActorProxyMap.end())
         mActorProxyMap.erase(itor);
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
   void GameManager::ChangeMap(const std::string &mapName, bool enableDatabasePaging) throw(dtUtil::Exception)
   {
      dtDAL::Map &map = dtDAL::Project::GetInstance().GetMap(mapName);
   
      std::vector<osg::ref_ptr<dtDAL::ActorProxy> > proxies;
      map.GetAllProxies(proxies);

      //delete all actors after making sure the map loaded correctly.
      DeleteAllActors(false);

      //Close the old map.
      if (!mLoadedMap.empty())
      {
         dtDAL::Map &oldMap = dtDAL::Project::GetInstance().GetMap(mLoadedMap);
         dtDAL::Project::GetInstance().CloseMap(oldMap, true);
      }
            
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
      dtDAL::Project::GetInstance().LoadMapIntoScene(map, *mScene.get(), false, enableDatabasePaging);
      
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
   void GameManager::SetTimer(
      const std::string& name, 
      GameActorProxy& aboutActor, 
      float time, 
      bool repeat, 
      bool realTime)
   {
      TimerInfo t;
      t.name = name;
      t.aboutActor = aboutActor.GetId();
      t.time = GetSimulationTime() + (double)time;
      t.repeat = repeat;
      t.realTime = realTime;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameManager::ClearTimer(const std::string& name)
   {

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

}
