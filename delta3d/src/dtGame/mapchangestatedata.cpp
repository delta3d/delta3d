/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/actortype.h>

#include <dtGame/exceptionenum.h>
#include <dtGame/mapchangestatedata.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>

namespace dtGame
{
   IMPLEMENT_ENUM(MapChangeStateData::MapChangeState);

   ///////////////////////////////////////////////////////////////////////////////
   const MapChangeStateData::MapChangeState MapChangeStateData::MapChangeState::UNLOAD("UNLOAD");

   ///////////////////////////////////////////////////////////////////////////////
   const MapChangeStateData::MapChangeState MapChangeStateData::MapChangeState::LOAD("LOAD");

   ///////////////////////////////////////////////////////////////////////////////
   const MapChangeStateData::MapChangeState MapChangeStateData::MapChangeState::IDLE("IDLE");
   
   ///////////////////////////////////////////////////////////////////////////////
   MapChangeStateData::MapChangeStateData(GameManager& gm):
      osg::Referenced(), mGameManager(&gm), mCurrentState(&MapChangeStateData::MapChangeState::IDLE), 
      mAddBillboards(false), mEnableDatabasePaging(false) 
   {
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::BeginMapChange(const std::string& oldMapName, const std::string& newMapName, bool addBillboards, bool enableDatabasePaging)
   {
      if (!mGameManager.valid())
      {
         std::string msg("The Game Manager used by the MapChangeStateData class has been deleted, but the BeginMapChange was called.");
         LOGN_ERROR("gamemanager.cpp", msg);
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, msg, __FUNCTION__, __LINE__);
      }

      mOldMapName = oldMapName;
      mNewMapName = newMapName;
      mAddBillboards = addBillboards;
      mEnableDatabasePaging = enableDatabasePaging;

      mCurrentState = &MapChangeState::UNLOAD;

      //We are only changing maps if the new map name is not empty
      if (!mNewMapName.empty())
      {
         const std::set<std::string>& names = dtDAL::Project::GetInstance().GetMapNames();
         if (names.find(mNewMapName) == names.end())
         {
            mCurrentState = &MapChangeState::IDLE;
            std::string msg = mNewMapName + " is not a valid map.";
            throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, msg, __FUNCTION__, __LINE__);
         }
         
         SendMapMessage(MessageType::INFO_MAP_CHANGE_BEGIN, mNewMapName);
      }
      
      //mark all actors for deletion.
      //Does not send create messages when this object is not in IDLE state. 
      mGameManager->DeleteAllActors(false);

      if (!mOldMapName.empty())
      {
         SendMapMessage(MessageType::INFO_MAP_UNLOAD_BEGIN, mOldMapName);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::ContinueMapChange()
   {
      if (!mGameManager.valid())
      {
         std::string msg("The Game Manager used by the MapChangeStateData class has been deleted, but ContinueMapChange was called.");
         LOGN_ERROR("gamemanager.cpp", msg);
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, msg, __FUNCTION__, __LINE__);
      }
      
      //This shouldn't be called, but it should definitely not do anything.
      if (*mCurrentState == MapChangeState::IDLE)
         return;
      
      if (*mCurrentState == MapChangeState::UNLOAD)
      {
         if (!mOldMapName.empty())
         {
            dtDAL::Map &oldMap = dtDAL::Project::GetInstance().GetMap(mOldMapName);
            // Clear out all the game events that came from the old map
            if (mGameManager->GetRemoveGameEventsOnMapChange())
            {
               std::vector<dtDAL::GameEvent* > events;
               oldMap.GetEventManager().GetAllEvents(events);
               for (unsigned int i = 0; i < events.size(); i ++)
                  dtDAL::GameEventManager::GetInstance().RemoveEvent(events[i]->GetUniqueId());
            }
            dtDAL::Project::GetInstance().CloseMap(oldMap, true);
            
            SendMapMessage(MessageType::INFO_MAP_UNLOADED, mOldMapName);
         }
         
         if (!mNewMapName.empty())
         {
            //Make the map load.
            try 
            {
               dtDAL::Project::GetInstance().GetMap(mNewMapName);
            }
            catch (const dtUtil::Exception&)
            {
               //if we can't load the map, we go back to idle and send and
               //empty string map change ended message
               mCurrentState = &MapChangeState::IDLE;               
               SendMapMessage(MessageType::INFO_MAP_CHANGED, "");
               mNewMapName.clear();
               return;
            }            

            SendMapMessage(MessageType::INFO_MAP_LOAD_BEGIN, mNewMapName);
            mCurrentState = &MapChangeState::LOAD;
         }
         else
         {
            mCurrentState = &MapChangeState::IDLE;
         }
      } 
      else if (mCurrentState == &MapChangeState::LOAD)
      {
         dtDAL::Map &map = dtDAL::Project::GetInstance().GetMap(mNewMapName);
         //add all the events in the map to the game manager.
         std::vector<dtDAL::GameEvent* > events;
         map.GetEventManager().GetAllEvents(events);
         dtDAL::GameEventManager& mainGEM = dtDAL::GameEventManager::GetInstance();
         for (unsigned int i = 0; i < events.size(); i ++)
         {
            if (mainGEM.FindEvent(events[i]->GetUniqueId()) == NULL)
               mainGEM.AddEvent(*events[i]);
         }

         if (map.GetEnvironmentActor() != NULL)
         {
            dtGame::EnvironmentActorProxy *eap = 
               static_cast<dtGame::EnvironmentActorProxy*>(map.GetEnvironmentActor());
            
            mGameManager->SetEnvironmentActor(eap);
         }

         std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
         map.GetAllProxies(proxies);

         for (unsigned int i = 0; i < proxies.size(); i++)
         {
            dtDAL::ActorProxy& aProxy = *proxies[i];
            // Ensure that we don't try and add the environment actor
            if(map.GetEnvironmentActor() == &aProxy)
               continue;
            
            if (aProxy.IsGameActorProxy())
            {
               GameActorProxy* gameProxy = dynamic_cast<GameActorProxy*>(&aProxy);
               if (gameProxy != NULL)
               {
                  gameProxy->BuildInvokables();
                  if(gameProxy->GetInitialOwnership() == GameActorProxy::Ownership::PROTOTYPE)
                  {
                     mGameManager->AddActorAsAPrototype(*gameProxy);
                  }
                  else
                  {
                     bool shouldPublish = gameProxy->GetInitialOwnership() == GameActorProxy::Ownership::SERVER_PUBLISHED;
                     //neither sends create messages nor adds to the scene when
                     //this object is not in IDLE state :-) 
                     mGameManager->AddActor(*gameProxy, false, shouldPublish);
                  }
               }
               else
               {
                  dtUtil::Log::GetInstance("mapchangestatedata.cpp").LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "Actor has the type of a GameActor, but casting it to a GameActorProxy failed.  Actor %s of type %s.%s will not be added to the scene.",
                     gameProxy->GetName().c_str(), gameProxy->GetActorType().GetCategory().c_str(), gameProxy->GetActorType().GetName().c_str());
               }
            }
            else
            {
               mGameManager->AddActor(aProxy);
            }
         }
         
         mGameManager->GetScene().DisablePaging();
         if (mEnableDatabasePaging)
            mGameManager->GetScene().EnablePaging();
            
         SendMapMessage(MessageType::INFO_MAP_LOADED, mNewMapName);            
         SendMapMessage(MessageType::INFO_MAP_CHANGED, mNewMapName);            
         mCurrentState = &MapChangeState::IDLE;
      }
   }
   
   void MapChangeStateData::SendMapMessage(const MessageType& type, const std::string& name)
   {
      dtCore::RefPtr<MapLoadedMessage> closeMessage;
      mGameManager->GetMessageFactory().CreateMessage(type, closeMessage);
      closeMessage->SetLoadedMapName(name);

      mGameManager->SendMessage(*closeMessage);
   }
   
}
