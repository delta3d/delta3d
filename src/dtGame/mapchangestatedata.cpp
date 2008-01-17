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

#include <dtCore/scene.h>

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
   void MapChangeStateData::BeginMapChange(const MapChangeStateData::NameVector& oldMapNames, const MapChangeStateData::NameVector& newMapNames, bool addBillboards, bool enableDatabasePaging)
   {
      if (!mGameManager.valid())
      {
         std::string msg("The Game Manager used by the MapChangeStateData class has been deleted, but the BeginMapChange was called.");
         LOGN_ERROR("gamemanager.cpp", msg);
         throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, msg, __FUNCTION__, __LINE__);
      }

      mOldMapNames = oldMapNames;
      mNewMapNames = newMapNames;
      mAddBillboards = addBillboards;
      mEnableDatabasePaging = enableDatabasePaging;

      mCurrentState = &MapChangeState::UNLOAD;

      //We are only changing maps if the new map list is not empty
      if (!mNewMapNames.empty())
      {
         const std::set<std::string>& names = dtDAL::Project::GetInstance().GetMapNames();
         MapChangeStateData::NameVector::const_iterator i = mNewMapNames.begin();
         MapChangeStateData::NameVector::const_iterator end = mNewMapNames.end();
         for (; i != end; ++i)
         {
            if (names.find(*i) == names.end())
            {
               mCurrentState = &MapChangeState::IDLE;
               std::string msg = *i + " is not a valid map.";
               throw dtUtil::Exception(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, msg, __FUNCTION__, __LINE__);
            }
         }
         
         SendMapMessage(MessageType::INFO_MAP_CHANGE_BEGIN, mNewMapNames);
      }
      
      //mark all actors for deletion.
      //Does not send create messages when this object is not in IDLE state. 
      mGameManager->DeleteAllActors(false);

      if (!mOldMapNames.empty())
      {
         SendMapMessage(MessageType::INFO_MAP_UNLOAD_BEGIN, mOldMapNames);
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::CloseOldMaps()
   {
      if (!mOldMapNames.empty())
      {
         MapChangeStateData::NameVector::const_iterator i = mOldMapNames.begin();
         MapChangeStateData::NameVector::const_iterator end = mOldMapNames.end();
   
         for (; i != end; ++i)
         {
            dtDAL::Map& oldMap = dtDAL::Project::GetInstance().GetMap(*i);
            // Clear out all the game events that came from the old map
            if (mGameManager->GetRemoveGameEventsOnMapChange())
            {
               std::vector<dtDAL::GameEvent*> events;
               oldMap.GetEventManager().GetAllEvents(events);
               dtDAL::GameEventManager& mainGEM = dtDAL::GameEventManager::GetInstance();

               std::vector<dtDAL::GameEvent*>::const_iterator j = events.begin();
               std::vector<dtDAL::GameEvent*>::const_iterator jend = events.end();
               for (; j != jend; ++j)
                  mainGEM.RemoveEvent((*j)->GetUniqueId());
            }
            dtDAL::Project::GetInstance().CloseMap(oldMap, true);
         }
         
         SendMapMessage(MessageType::INFO_MAP_UNLOADED, mOldMapNames);
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   bool MapChangeStateData::OpenNewMaps()
   {
      bool success = true;
      if (!mNewMapNames.empty())
      {
         MapChangeStateData::NameVector::const_iterator i = mNewMapNames.begin();
         MapChangeStateData::NameVector::const_iterator end = mNewMapNames.end();
   
         for (; i != end; ++i)
         {
            //Make the map load.
            try 
            {
               dtDAL::Project::GetInstance().GetMap(*i);
            }
            catch (const dtUtil::Exception&)
            {
               //if we can't load a map, we go back to idle and send and
               //empty string map change ended message
               mCurrentState = &MapChangeState::IDLE;
               SendMapMessage(MessageType::INFO_MAP_CHANGED, MapChangeStateData::NameVector());
               mNewMapNames.clear();
               success = false;
               break;
            }
         }
         if (success)
            SendMapMessage(MessageType::INFO_MAP_LOAD_BEGIN, mNewMapNames);
      }
      else
      {
         success = false;
      }
      return success;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::LoadSingleMapIntoGM(const std::string& mapName)
   {
      dtDAL::Map &map = dtDAL::Project::GetInstance().GetMap(mapName);
      //add all the events in the map to the game manager.
      std::vector<dtDAL::GameEvent* > events;
      map.GetEventManager().GetAllEvents(events);
      dtDAL::GameEventManager& mainGEM = dtDAL::GameEventManager::GetInstance();
      
      std::vector<dtDAL::GameEvent*>::const_iterator i = events.begin();
      std::vector<dtDAL::GameEvent*>::const_iterator iend = events.end();
      for (; i != iend; ++i)
      {
         if (mainGEM.FindEvent((*i)->GetUniqueId()) == NULL)
            mainGEM.AddEvent(**i);
      }

      if (map.GetEnvironmentActor() != NULL)
      {
         dtGame::IEnvGameActorProxy *eap = 
            static_cast<dtGame::IEnvGameActorProxy*>(map.GetEnvironmentActor());
         
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
               gameProxy->SetGameManager(mGameManager.get());
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
                  try
                  {
                     mGameManager->AddActor(*gameProxy, false, shouldPublish);
                  }
                  catch (const dtUtil::Exception& ex)
                  {
                     dtUtil::Log::GetInstance("mapchangestatedata.cpp").LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "An error occurred adding actor \"%s\" of type \"%s\".  The exception will follow.", 
                           gameProxy->GetName().c_str(), gameProxy->GetActorType().ToString().c_str());
                     ex.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance("mapchangestatedata.cpp"));
                  }
               }
            }
            else
            {
               dtUtil::Log::GetInstance("mapchangestatedata.cpp").LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Actor has the type of a GameActor, but casting it to a GameActorProxy failed.  "
                  "Actor \"%s\" of type \"%s\" will not be added to the scene.",
                  gameProxy->GetName().c_str(), gameProxy->GetActorType().ToString().c_str());
            }
         }
         else
         {
            mGameManager->AddActor(aProxy);
         }
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
         CloseOldMaps();
         
         if (OpenNewMaps())
         {
            mCurrentState = &MapChangeState::LOAD;
         }
         else
         {
            mCurrentState = &MapChangeState::IDLE;
         }
      } 
      else if (mCurrentState == &MapChangeState::LOAD)
      {
         MapChangeStateData::NameVector::const_iterator i = mNewMapNames.begin();
         MapChangeStateData::NameVector::const_iterator iend = mNewMapNames.end();
   
         for (; i != iend; ++i)
         {
            LoadSingleMapIntoGM(*i);
         }

         if (mGameManager->GetScene().IsPagingEnabled())
            mGameManager->GetScene().DisablePaging();

         if (mEnableDatabasePaging)
            mGameManager->GetScene().EnablePaging();
            
         SendMapMessage(MessageType::INFO_MAP_LOADED, mNewMapNames);
         SendMapMessage(MessageType::INFO_MAP_CHANGED, mNewMapNames);
         mCurrentState = &MapChangeState::IDLE;
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::SendMapMessage(const MessageType& type, const MapChangeStateData::NameVector& names)
   {
      dtCore::RefPtr<MapMessage> mapMessage;
      mGameManager->GetMessageFactory().CreateMessage(type, mapMessage);
      mapMessage->SetMapNames(names);

      mGameManager->SendMessage(*mapMessage);
   }
   
}
