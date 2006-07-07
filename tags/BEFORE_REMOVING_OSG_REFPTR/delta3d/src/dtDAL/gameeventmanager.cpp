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
 * @author Matthew W. Campbell
 */
#include "dtDAL/gameeventmanager.h"
#include "dtDAL/exceptionenum.h"
#include "dtDAL/gameevent.h"

namespace dtDAL
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
   void GameEventManager::AddEvent(GameEvent &event)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor =
            mEventList.find(event.GetUniqueId());

      if (itor != mEventList.end())
         EXCEPT(ExceptionEnum::BaseException,"Cannot add event: " + event.GetName() +
               " Event with same id already exists.");

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
   void GameEventManager::GetAllEvents(std::vector<dtCore::RefPtr<GameEvent> > &toFill)
   {
      std::map<dtCore::UniqueId,dtCore::RefPtr<GameEvent> >::iterator itor;

      toFill.clear();
      toFill.reserve(mEventList.size());
      for (itor=mEventList.begin(); itor!=mEventList.end(); ++itor)
         toFill.push_back(itor->second);
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
