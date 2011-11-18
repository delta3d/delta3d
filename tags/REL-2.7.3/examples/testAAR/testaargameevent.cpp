/* -*-c++-*-
 * testAAR - testaargameevent (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * William E. Johnson II
 */

#include "testaargameevent.h"
#include <dtCore/gameeventmanager.h>

dtCore::RefPtr<dtCore::GameEvent> TestAARGameEvent::EVENT_START_RECORD;
dtCore::RefPtr<dtCore::GameEvent> TestAARGameEvent::EVENT_BOX_PLACED;
dtCore::RefPtr<dtCore::GameEvent> TestAARGameEvent::EVENT_PLAYER_LEFT;
dtCore::RefPtr<dtCore::GameEvent> TestAARGameEvent::EVENT_PLAYER_RIGHT;
dtCore::RefPtr<dtCore::GameEvent> TestAARGameEvent::EVENT_PLAYER_FORWARD;
dtCore::RefPtr<dtCore::GameEvent> TestAARGameEvent::EVENT_PLAYER_BACKWARD;


TestAARGameEvent::TestAARGameEvent() 
{

}

TestAARGameEvent::~TestAARGameEvent()
{

}

void TestAARGameEvent::InitEvents()
{
   dtCore::GameEventManager& geMan = dtCore::GameEventManager::GetInstance();
   
   EVENT_START_RECORD    = geMan.FindEvent("Event - Start Record");
   EVENT_BOX_PLACED      = geMan.FindEvent("Event - Box Placed");
   EVENT_PLAYER_LEFT     = geMan.FindEvent("Event - Player Left");
   EVENT_PLAYER_RIGHT    = geMan.FindEvent("Event - Player Right");
   EVENT_PLAYER_FORWARD  = geMan.FindEvent("Event - Player Forward");
   EVENT_PLAYER_BACKWARD = geMan.FindEvent("Event - Player Backward");
}
