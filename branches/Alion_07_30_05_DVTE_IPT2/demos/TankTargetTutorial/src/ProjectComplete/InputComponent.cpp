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
#include <dtCore/camera.h>

////////////////////////////////////////////////////////////////////
InputComponent::InputComponent(const std::string &name, bool inPlaybackMode) :
   dtGame::BaseInputComponent(name),
   mToggleEngineEvent(0),
   mSpeedBoost(0),
   mTankFired(0),
   mInPlaybackMode(inPlaybackMode)

{
   // Create a few game events and register them with the event manager.
   mToggleEngineEvent = new dtDAL::GameEvent("ToggleEngine");
   mToggleEngineEvent->SetUniqueId(dtCore::UniqueId("ToggleEngine")); // set until we put these in the map
   dtDAL::GameEventManager::GetInstance().AddEvent(*mToggleEngineEvent);

   mSpeedBoost = new dtDAL::GameEvent("SpeedBoost");
   mSpeedBoost->SetUniqueId(dtCore::UniqueId("SpeedBoost")); // set until we put these in the map
   dtDAL::GameEventManager::GetInstance().AddEvent(*mSpeedBoost);

   mTankFired = new dtDAL::GameEvent("TankFired");
   mTankFired->SetUniqueId(dtCore::UniqueId("TankFired")); // set until we put these in the map
   dtDAL::GameEventManager::GetInstance().AddEvent(*mTankFired);

   mTestShaders = new dtDAL::GameEvent("TestShaders");
   mTestShaders->SetUniqueId(dtCore::UniqueId("TestShaders")); // set until we put these in the map
   dtDAL::GameEventManager::GetInstance().AddEvent(*mTestShaders);

   mReset = new dtDAL::GameEvent("ResetStuff");
   mReset->SetUniqueId(dtCore::UniqueId("ResetStuff")); // set until we put these in the map
   dtDAL::GameEventManager::GetInstance().AddEvent(*mReset);
}

////////////////////////////////////////////////////////////////////
bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard,
   Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   bool handled = true;
   switch(key)
   {
      case Producer::Key_space:
         if (!mInPlaybackMode)
            FireGameEvent(*mToggleEngineEvent);
         break;
      case Producer::Key_Return:
         if (!mInPlaybackMode)
            FireGameEvent(*mSpeedBoost);
         break;
      case Producer::Key_F:
         if (!mInPlaybackMode)
            FireGameEvent(*mTankFired);
         break;
      case Producer::Key_P:
         if (!mInPlaybackMode)
            FireGameEvent(*mTestShaders);
         break;
      case Producer::Key_R:
         if (!mInPlaybackMode)
            FireGameEvent(*mReset);
         break;

      case Producer::Key_Insert:
         GetGameManager()->GetApplication().GetCamera()->SetNextStatisticsType();
         break;

      case Producer::Key_1:
         if (mLogController.valid())
         {
            mLogController->RequestChangeStateToIdle();
            GetGameManager()->SetPaused(false);
         }

         break;

      case Producer::Key_2:
         if (mLogController.valid() && !mInPlaybackMode && 
            mLogController->GetLastKnownStatus().GetStateEnum() 
            == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            mLogController->RequestChangeStateToRecord();
         }

         break;

      case Producer::Key_3:
         if (mLogController.valid() && mInPlaybackMode)
         {
            // start playback
            if(mLogController->GetLastKnownStatus().GetStateEnum() 
               == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
            {
               mLogController->RequestChangeStateToPlayback();
               mLogController->RequestServerGetKeyframes();
            }
            // go back to the beginning.
            else if(mLogController->GetLastKnownStatus().GetStateEnum() 
               == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
            {
               const std::vector<dtGame::LogKeyframe> &frames = mLogController->GetLastKnownKeyframeList();
               if (frames.size() > 0)
                  mLogController->RequestJumpToKeyframe(frames[0]);
               GetGameManager()->SetPaused(false);
            }
         }
         else 
         {
            LOG_ERROR("ERROR - Cannot switch to playback mode directly. Please restart the application and add '-startPlayback 1' to your command parameters. In addition, consider using an additional map without the main tank actor by setting map ('--mapName mapone_playback')."); 
         }

         break;

      case Producer::Key_6:
         GetGameManager()->SetPaused(!GetGameManager()->IsPaused());
         break;

      // SLOWER
      case Producer::Key_7:
         {
            float speedFactor = GetGameManager()->GetTimeScale() * 0.8f;
            if (speedFactor >= 0.10f)
               GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
                  speedFactor, GetGameManager()->GetSimulationClockTime());
         }
         break;

      // FASTER
      case Producer::Key_8:
         {
            float speedFactor = GetGameManager()->GetTimeScale() * 1.20f;
            if (speedFactor <= 10.0f)
               GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
                  speedFactor, GetGameManager()->GetSimulationClockTime());
         }
         break;

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
   if (message.GetMessageType() == dtGame::MessageType::INFO_RESTARTED)
   {
      mLogController = dynamic_cast<dtGame::LogController *> (GetGameManager()->
         GetComponentByName("LogController")); 
   }
}

//////////////////////////////////////////////////////////////////////////
void InputComponent::FireGameEvent(dtDAL::GameEvent &event)
{
   dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
   GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, eventMsg);

   eventMsg->SetGameEvent(event);
   GetGameManager()->SendMessage(*eventMsg);   
}
