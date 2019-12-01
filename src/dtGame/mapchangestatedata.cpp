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
#include <prefix/dtgameprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/actortype.h>

#include <dtGame/exceptionenum.h>
#include <dtGame/environmentactor.h>
#include <dtGame/mapchangestatedata.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gmsettings.h>
#include <dtGame/gmcomponent.h>

#include <dtCore/system.h>
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
      mAddBillboards(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::BeginMapChange(const MapChangeStateData::NameVector& oldMapNames, const MapChangeStateData::NameVector& newMapNames, bool addBillboards)
   {
      if (!mGameManager.valid())
      {
         std::string msg("The Game Manager used by the MapChangeStateData class has been deleted, but the BeginMapChange was called.");
         LOGN_ERROR("gamemanager.cpp", msg);
         throw dtGame::GeneralGameManagerException( msg, __FUNCTION__, __LINE__);
      }

      // set the app to pause so we dont get a huge timestep when we're through
      mGameManager->SetPaused(true);

      mOldMapNames = oldMapNames;
      mNewMapNames = newMapNames;
      mAddBillboards = addBillboards;

      mCurrentState = &MapChangeState::UNLOAD;

      // We are only changing maps if the new map list is not empty
      if (!mNewMapNames.empty())
      {
         const std::set<std::string>& names = dtCore::Project::GetInstance().GetMapNames();
         MapChangeStateData::NameVector::const_iterator i = mNewMapNames.begin();
         MapChangeStateData::NameVector::const_iterator end = mNewMapNames.end();
         for (; i != end; ++i)
         {
            const std::string& curName = *i;
            if (names.find(curName) == names.end())
            {
               mCurrentState = &MapChangeState::IDLE;
               std::string msg = curName + " is not a valid map.";
               throw dtGame::GeneralGameManagerException( msg, __FUNCTION__, __LINE__);
            }
         }

         SendMapMessage(MessageType::INFO_MAP_CHANGE_BEGIN, mNewMapNames);
      }

      // mark all actors for deletion.
      // Does not send create messages when this object is not in IDLE state.
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
            CloseSingleMap(*i, true);
         }

         SendMapMessage(MessageType::INFO_MAPS_CLOSED, mOldMapNames);
         SendMapMessage(MessageType::INFO_MAP_UNLOADED, mOldMapNames);
      }
   }


   //////////////////////////////////////////////////////////////////////////
   void MapChangeStateData::CloseSingleMap(const std::string& mapName, bool deleteLibraries )
   {
      if (!dtCore::Project::GetInstance().IsMapOpen(mapName)
            && (!mGameManager->GetRemoveGameEventsOnMapChange() || dtCore::GameEventManager::GetInstance().GetNumEvents() == 0))
      {
         return;
      }

      if (!dtCore::Project::GetInstance().IsMapOpen(mapName))
      {
         LOG_WARNING("Forced to reopen map \"" + mapName + "\" to remove the map's game events from the main game event manager on map close.");
      }

      dtCore::Map& oldMap = dtCore::Project::GetInstance().GetMap(mapName);
      // Clear out all the game events that came from the old map
      if (mGameManager->GetRemoveGameEventsOnMapChange())
      {
         std::vector<dtCore::GameEvent*> events;
         oldMap.GetEventManager().GetAllEvents(events);
         dtCore::GameEventManager& mainGEM = dtCore::GameEventManager::GetInstance();

         std::vector<dtCore::GameEvent*>::const_iterator j = events.begin();
         std::vector<dtCore::GameEvent*>::const_iterator jend = events.end();
         for (; j != jend; ++j)
         {
            mainGEM.RemoveEvent((*j)->GetUniqueId());
         }
      }
      dtCore::Project::GetInstance().CloseMap(oldMap, deleteLibraries);
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
            // Make the map load.
            try
            {
               dtCore::Project::GetInstance().GetMap(*i);
            }
            catch (const dtUtil::Exception& ex)
            {
               // if we can't load a map, we go back to idle and send and
               // empty string map change ended message
               mCurrentState = &MapChangeState::IDLE;
               SendMapMessage(MessageType::INFO_MAP_CHANGED, MapChangeStateData::NameVector());
               dtUtil::Log::GetInstance("mapchangestatedata.cpp").LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Critical failure occurred while opening map[%s].", (*i).c_str()); 
               ex.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance("mapchangestatedata.cpp"));
               mNewMapNames.clear();
               success = false;
               break;
            }
         }
         if (success)
         {
            SendMapMessage(MessageType::INFO_MAP_LOAD_BEGIN, mNewMapNames);
         }
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
      dtCore::Map& map = dtCore::Project::GetInstance().GetMap(mapName);
      // add all the events in the map to the game manager.
      std::vector<dtCore::GameEvent* > events;
      map.GetEventManager().GetAllEvents(events);
      dtCore::GameEventManager& mainGEM = dtCore::GameEventManager::GetInstance();

      std::vector<dtCore::GameEvent*>::const_iterator i = events.begin();
      std::vector<dtCore::GameEvent*>::const_iterator iend = events.end();
      for (; i != iend; ++i)
      {
         dtCore::GameEvent* currEvent = *i;
         if (mainGEM.FindEvent(currEvent->GetUniqueId()) == NULL)
         {
            mainGEM.AddEvent(*currEvent);
         }
      }

      ScopedGMBatchAdd batch(*mGameManager);

      if (map.GetEnvironmentActor() != NULL)
      {
         dtGame::IEnvGameActorProxy* eap =
            static_cast<dtGame::IEnvGameActorProxy*>(map.GetEnvironmentActor());

         mGameManager->SetEnvironmentActor(eap);
      }

      dtCore::ActorRefPtrVector proxies;
      map.GetAllProxies(proxies);

      for (unsigned int i = 0; i < proxies.size(); ++i)
      {
         dtCore::BaseActorObject& curAddActor = *proxies[i];
         // Ensure that we don't add the environment actor
         if (map.GetEnvironmentActor() == &curAddActor)
         {
            continue;
         }
         // Child actors are added when the parents are.
         else if (curAddActor.IsGameActor() && static_cast<GameActorProxy&>(curAddActor).GetParentActor() != NULL)
         {
            continue;
         }
         else
         {
            try
            {
               mGameManager->AddActor(curAddActor);
            }
            catch (const dtUtil::Exception& ex)
            {
               dtUtil::Log::GetInstance("mapchangestatedata.cpp").LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "A problem occurred adding Actor with name \"%s\" of type \"%s\" to the GameManager.",
                     curAddActor.GetName().c_str(), curAddActor.GetActorType().GetFullName().c_str());
               ex.LogException(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance("mapchangestatedata.cpp"));
            }
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
         throw dtGame::GeneralGameManagerException( msg, __FUNCTION__, __LINE__);
      }

      // This shouldn't be called, but it should definitely not do anything.
      if (*mCurrentState == MapChangeState::IDLE)
      {
         return;
      }

      if (*mCurrentState == MapChangeState::UNLOAD)
      {
         CloseOldMaps();

         if (OpenNewMaps())
         {
            mCurrentState = &MapChangeState::LOAD;
         }
         else
         {
            // set the app to unpause so time stepping is correct
            mGameManager->SetPaused(false);
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

         // set the app to unpause so time stepping is correct
         mGameManager->SetPaused(false);

         SendMapMessage(MessageType::INFO_MAPS_OPENED, mNewMapNames);
         SendMapMessage(MessageType::INFO_MAP_CHANGE_LOAD_END, mNewMapNames);
         SendMapMessage(MessageType::INFO_MAP_CHANGE_END, mNewMapNames);
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

   
} // namespace dtGame
