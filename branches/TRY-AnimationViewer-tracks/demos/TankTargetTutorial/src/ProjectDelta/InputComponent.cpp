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
 * @author Curtiss Murphy
 */

#include "InputComponent.h"
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtABC/application.h>

////////////////////////////////////////////////////////////////////
InputComponent::InputComponent(const std::string &name) :
   dtGame::BaseInputComponent(name),
   mToggleEngineEvent(0),
   mSpeedBoost(0),
   mTankFired(0)
{
}

////////////////////////////////////////////////////////////////////
void InputComponent::SetupEvents()
{
   // TUTORIAL - FIND THE GAME EVENTS HERE 
}

////////////////////////////////////////////////////////////////////
bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard,
   Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   bool handled = true;
   switch(key)
   {
      // TUTORIAL - ADD YOUR KEYS AND CALLS TO FireGameEvent HERE 

      default:
         handled = false;
         break;
   }

   // the default case handles the escape key to quit.
   if (!handled)
      return BaseInputComponent::HandleKeyPressed(keyboard, key, character);

   return handled;
}

//////////////////////////////////////////////////////////////////////////
void InputComponent::ProcessMessage(const dtGame::Message& message)
{
   // After the map is finished loading, we will get this.  At which point, our events
   // have been loaded from the map and we can now find them
   if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      SetupEvents();
   }
   // Once the system is started, we find the log controller so we can manipulate it later
   else if (message.GetMessageType() == dtGame::MessageType::INFO_RESTARTED)
   {
      mLogController = dynamic_cast<dtGame::LogController *> (GetGameManager()->
         GetComponentByName("LogController")); 
   }
}

//////////////////////////////////////////////////////////////////////////
void InputComponent::FireGameEvent(dtDAL::GameEvent &event)
{
   // TUTORIAL - CREATE AND SEND YOUR GAMEEVENTMESSAGE HERE 
}

