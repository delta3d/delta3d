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

dtCore::RefPtr<dtDAL::GameEvent> TestAARGameEvent::EVENT_START_RECORD;
dtCore::RefPtr<dtDAL::GameEvent> TestAARGameEvent::EVENT_BOX_PLACED;
dtCore::RefPtr<dtDAL::GameEvent> TestAARGameEvent::EVENT_PLAYER_LEFT;
dtCore::RefPtr<dtDAL::GameEvent> TestAARGameEvent::EVENT_PLAYER_RIGHT;
dtCore::RefPtr<dtDAL::GameEvent> TestAARGameEvent::EVENT_PLAYER_FORWARD;
dtCore::RefPtr<dtDAL::GameEvent> TestAARGameEvent::EVENT_PLAYER_BACKWARD;


TestAARGameEvent::TestAARGameEvent() 
{

}

TestAARGameEvent::~TestAARGameEvent()
{

}

void TestAARGameEvent::InitEvents()
{
   dtDAL::GameEventManager& geMan = dtDAL::GameEventManager::GetInstance();
   
   EVENT_START_RECORD    = geMan.FindEvent("Event - Start Record");
   EVENT_BOX_PLACED      = geMan.FindEvent("Event - Box Placed");
   EVENT_PLAYER_LEFT     = geMan.FindEvent("Event - Player Left");
   EVENT_PLAYER_RIGHT    = geMan.FindEvent("Event - Player Right");
   EVENT_PLAYER_FORWARD  = geMan.FindEvent("Event - Player Forward");
   EVENT_PLAYER_BACKWARD = geMan.FindEvent("Event - Player Backward");
}
