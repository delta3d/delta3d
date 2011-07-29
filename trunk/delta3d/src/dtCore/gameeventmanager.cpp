/*
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
 * Matthew W. Campbell
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/gameevent.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<GameEventManager> GameEventManager::mInstance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   GameEventManager::GameEventManager()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameEventManager::~GameEventManager()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameEventManager::GameEventManager(const GameEventManager &rhs) 
   {
      mEventList = rhs.mEventList; 
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   GameEventManager& GameEventManager::operator=(const GameEventManager &rhs) 
   {
      if (&rhs != this)
      {
         mEventList = rhs.mEventList;
      }  
      return *this; 
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventManager::AddEvent(GameEvent &event)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor =
            mEventList.find(event.GetUniqueId());

      if (itor != mEventList.end())
         throw dtCore::BaseException("Cannot add event: " + event.GetName() +
               " Event with same id already exists.", __FILE__, __LINE__);

      mEventList.insert(std::make_pair(event.GetUniqueId(),&event));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventManager::RemoveEvent(GameEvent &event)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor =
            mEventList.find(event.GetUniqueId());

      if (itor != mEventList.end())
         mEventList.erase(itor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventManager::RemoveEvent(const dtCore::UniqueId &id)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor =
            mEventList.find(id);

      if (itor != mEventList.end())
         mEventList.erase(itor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventManager::ClearAllEvents()
   {
      mEventList.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameEventManager::GetAllEvents(std::vector<GameEvent* > &toFill)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor;

      toFill.clear();
      toFill.reserve(mEventList.size());
      for (itor=mEventList.begin(); itor!=mEventList.end(); ++itor)
         toFill.push_back(itor->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameEvent *GameEventManager::FindEvent(const dtCore::UniqueId &id)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor =
            mEventList.find(id);

      if (itor != mEventList.end())
         return itor->second.get();
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameEvent *GameEventManager::FindEvent(const std::string &name)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor;

      for (itor=mEventList.begin(); itor!=mEventList.end(); ++itor)
      {
         if (itor->second->GetName() == name)
            return itor->second.get();
      }

      return NULL;
   }

}
