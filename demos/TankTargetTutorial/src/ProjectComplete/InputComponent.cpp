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
#include <dtCore/shadermanager.h>

////////////////////////////////////////////////////////////////////
InputComponent::InputComponent(const std::string &name, bool inPlaybackMode) :
   dtGame::BaseInputComponent(name),
   mToggleEngineEvent(0),
   mSpeedBoost(0),
   mTankFired(0),
   mInPlaybackMode(inPlaybackMode)

{
}

////////////////////////////////////////////////////////////////////
void InputComponent::SetupEvents()
{

   mToggleEngineEvent = dtDAL::GameEventManager::GetInstance().FindEvent("ToggleEngine");
   if (mToggleEngineEvent == NULL)
      LOG_ERROR("Failed to find event 'ToggleEngine'. Make sure it is in the map!");

   mSpeedBoost = dtDAL::GameEventManager::GetInstance().FindEvent("SpeedBoost");
   if (mSpeedBoost == NULL)
      LOG_ERROR("Failed to find event 'SpeedBoost'. Make sure it is in the map!");

   mTankFired = dtDAL::GameEventManager::GetInstance().FindEvent("TankFired");
   if (mTankFired == NULL)
      LOG_ERROR("Failed to find event 'TankFired'. Make sure it is in the map!");

   mTestShaders = dtDAL::GameEventManager::GetInstance().FindEvent("TestShaders");
   if (mTestShaders == NULL)
      LOG_ERROR("Failed to find event 'TestShaders'. Make sure it is in the map!");

   mReset = dtDAL::GameEventManager::GetInstance().FindEvent("ResetStuff");
   if (mReset == NULL)
      LOG_ERROR("Failed to find event 'ResetStuff'. Make sure it is in the map!");

   // Below is an example of how to create a game event directly in code. ie, without STAGE
   // Note, we set the unique id here because that is typically done in STAGE.
   //mToggleEngineEvent = new dtDAL::GameEvent("ToggleEngine");
   //mToggleEngineEvent->SetUniqueId(dtCore::UniqueId("ToggleEngine")); // best to set the ID to help replay work until we put these in the map
   //dtDAL::GameEventManager::GetInstance().AddEvent(*mToggleEngineEvent);
}

////////////////////////////////////////////////////////////////////
bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = true;
   switch(key)
   {
      case ' ':
         if (!mInPlaybackMode && mToggleEngineEvent != NULL)
            FireGameEvent(*mToggleEngineEvent);
         break;
      case osgGA::GUIEventAdapter::KEY_Return:
         if (!mInPlaybackMode && mSpeedBoost != NULL)
            FireGameEvent(*mSpeedBoost);
         break;
      case 'f':
         if (!mInPlaybackMode && mTankFired != NULL)
            FireGameEvent(*mTankFired);
         break;
      case 'p':
         //dtCore::ShaderManager::GetInstance().Clear();
         dtCore::ShaderManager::GetInstance().ReloadAndReassignShaderDefinitions("Shaders/TutorialShaderDefs.xml");
         //if (!mInPlaybackMode && mTestShaders != NULL)
         //   FireGameEvent(*mTestShaders);
         break;
      case 'r':
         if (!mInPlaybackMode && mReset != NULL)
            FireGameEvent(*mReset);
         break;

      case osgGA::GUIEventAdapter::KEY_Insert:
         //GetGameManager()->GetApplication().GetCamera()->SetNextStatisticsType();
         break;

      case '1':
         if (mLogController.valid())
         {
            mLogController->RequestChangeStateToIdle();
            GetGameManager()->SetPaused(false);
         }

         break;

      case '2':
         if (mLogController.valid() && !mInPlaybackMode && 
            mLogController->GetLastKnownStatus().GetStateEnum() 
            == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            mLogController->RequestChangeStateToRecord();
         }

         break;

      case '3':
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

      case '6':
         GetGameManager()->SetPaused(!GetGameManager()->IsPaused());
         break;

      // SLOWER
      case '7':
         {
            float speedFactor = GetGameManager()->GetTimeScale() * 0.8f;
            if (speedFactor >= 0.10f)
               GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
                  speedFactor, GetGameManager()->GetSimulationClockTime());
         }
         break;

      // FASTER
      case '8':
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
      return BaseInputComponent::HandleKeyPressed(keyboard, key);

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
   dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
   GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, eventMsg);

   eventMsg->SetGameEvent(event);
   GetGameManager()->SendMessage(*eventMsg);   
}
