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
 * William E. Johnson II
 */
#include "testaargameevent.h"
#include <dtDAL/gameeventmanager.h>

dtCore::RefPtr<TestAARGameEvent> TestAARGameEvent::EVENT_START_RECORD(new TestAARGameEvent("Event - Start Record"));
dtCore::RefPtr<TestAARGameEvent> TestAARGameEvent::EVENT_BOX_PLACED(new TestAARGameEvent("Event - Box Placed"));
dtCore::RefPtr<TestAARGameEvent> TestAARGameEvent::EVENT_PLAYER_LEFT(new TestAARGameEvent("Event - Player Left"));
dtCore::RefPtr<TestAARGameEvent> TestAARGameEvent::EVENT_PLAYER_RIGHT(new TestAARGameEvent("Event - Player Right"));
dtCore::RefPtr<TestAARGameEvent> TestAARGameEvent::EVENT_PLAYER_FORWARD(new TestAARGameEvent("Event - Player Forward"));
dtCore::RefPtr<TestAARGameEvent> TestAARGameEvent::EVENT_PLAYER_BACKWARD(new TestAARGameEvent("Event - Player Backward"));


TestAARGameEvent::TestAARGameEvent(const std::string &name) : dtDAL::GameEvent(name) 
{

}

TestAARGameEvent::~TestAARGameEvent()
{

}

void TestAARGameEvent::InitEvents()
{
   EVENT_START_RECORD->SetUniqueId(dtCore::UniqueId("Event - Start Record"));
   EVENT_BOX_PLACED->SetUniqueId(dtCore::UniqueId("Event - Box Placed"));
   EVENT_PLAYER_LEFT->SetUniqueId(dtCore::UniqueId("Event - Player Left"));
   EVENT_PLAYER_RIGHT->SetUniqueId(dtCore::UniqueId("Event - player Right"));
   EVENT_PLAYER_FORWARD->SetUniqueId(dtCore::UniqueId("Event - Player Forward"));
   EVENT_PLAYER_BACKWARD->SetUniqueId(dtCore::UniqueId("Event - Player Backward"));

   dtDAL::GameEventManager::GetInstance().AddEvent(*EVENT_START_RECORD);
   dtDAL::GameEventManager::GetInstance().AddEvent(*EVENT_BOX_PLACED);
   dtDAL::GameEventManager::GetInstance().AddEvent(*EVENT_PLAYER_LEFT);
   dtDAL::GameEventManager::GetInstance().AddEvent(*EVENT_PLAYER_RIGHT);
   dtDAL::GameEventManager::GetInstance().AddEvent(*EVENT_PLAYER_FORWARD);
   dtDAL::GameEventManager::GetInstance().AddEvent(*EVENT_PLAYER_BACKWARD);
}
