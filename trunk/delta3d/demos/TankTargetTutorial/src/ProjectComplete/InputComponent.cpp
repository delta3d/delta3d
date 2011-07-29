/* -*-c++-*-
 * TutorialLibrary - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
 * @author Curtiss Murphy
 */

#include "InputComponent.h"

#include <dtABC/application.h>

#include <dtCore/shadermanager.h>

#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>

////////////////////////////////////////////////////////////////////
InputComponent::InputComponent(const std::string& name, bool inPlaybackMode)
   : dtGame::BaseInputComponent(name)
   , mToggleEngineEvent(0)
   , mSpeedBoost(0)
   , mTankFired(0)
   , mInPlaybackMode(inPlaybackMode)
{
}

////////////////////////////////////////////////////////////////////
void InputComponent::SetupEvents()
{
   mToggleEngineEvent = dtCore::GameEventManager::GetInstance().FindEvent("ToggleEngine");
   if (mToggleEngineEvent == NULL)
   {
      LOG_ERROR("Failed to find event 'ToggleEngine'. Make sure it is in the map!");
   }

   mSpeedBoost = dtCore::GameEventManager::GetInstance().FindEvent("SpeedBoost");
   if (mSpeedBoost == NULL)
   {
      LOG_ERROR("Failed to find event 'SpeedBoost'. Make sure it is in the map!");
   }

   mTankFired = dtCore::GameEventManager::GetInstance().FindEvent("TankFired");
   if (mTankFired == NULL)
   {
      LOG_ERROR("Failed to find event 'TankFired'. Make sure it is in the map!");
   }

   mTestShaders = dtCore::GameEventManager::GetInstance().FindEvent("TestShaders");
   if (mTestShaders == NULL)
   {
      LOG_ERROR("Failed to find event 'TestShaders'. Make sure it is in the map!");
   }

   mReset = dtCore::GameEventManager::GetInstance().FindEvent("ResetStuff");
   if (mReset == NULL)
   {
      LOG_ERROR("Failed to find event 'ResetStuff'. Make sure it is in the map!");
   }

   // Below is an example of how to create a game event directly in code. ie, without STAGE
   // Note, we set the unique id here because that is typically done in STAGE.
   //mToggleEngineEvent = new dtCore::GameEvent("ToggleEngine");
   //mToggleEngineEvent->SetUniqueId(dtCore::UniqueId("ToggleEngine")); // best to set the ID to help replay work until we put these in the map
   //dtCore::GameEventManager::GetInstance().AddEvent(*mToggleEngineEvent);
}

////////////////////////////////////////////////////////////////////
bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = true;
   switch (key)
   {
   case ' ':
      if (!mInPlaybackMode && mToggleEngineEvent != NULL)
      {
         FireGameEvent(*mToggleEngineEvent);
      }
      break;
   case osgGA::GUIEventAdapter::KEY_Return:
      if (!mInPlaybackMode && mSpeedBoost != NULL)
      {
         FireGameEvent(*mSpeedBoost);
      }
      break;
   case 'f':
      if (!mInPlaybackMode && mTankFired != NULL)
      {
         FireGameEvent(*mTankFired);
      }
      break;
   case 'p':
      //dtCore::ShaderManager::GetInstance().Clear();
      dtCore::ShaderManager::GetInstance().ReloadAndReassignShaderDefinitions("Shaders/TutorialShaderDefs.xml");
      //if (!mInPlaybackMode && mTestShaders != NULL)
      //{
      //   FireGameEvent(*mTestShaders);
      //}
      break;
   case 'r':
      if (!mInPlaybackMode && mReset != NULL)
      {
         FireGameEvent(*mReset);
      }
      break;

   case osgGA::GUIEventAdapter::KEY_Insert:
      GetGameManager()->GetApplication().SetNextStatisticsType();
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
         if (mLogController->GetLastKnownStatus().GetStateEnum()
            == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            mLogController->RequestChangeStateToPlayback();
            mLogController->RequestServerGetKeyframes();
         }
         // go back to the beginning.
         else if (mLogController->GetLastKnownStatus().GetStateEnum()
            == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            const std::vector<dtGame::LogKeyframe>& frames = mLogController->GetLastKnownKeyframeList();
            if (frames.size() > 0)
            {
               mLogController->RequestJumpToKeyframe(frames[0]);
            }
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
         {
            GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
               speedFactor, GetGameManager()->GetSimulationClockTime());
         }
      }
      break;

   // FASTER
   case '8':
      {
         float speedFactor = GetGameManager()->GetTimeScale() * 1.20f;
         if (speedFactor <= 10.0f)
         {
            GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
               speedFactor, GetGameManager()->GetSimulationClockTime());
         }
      }
      break;

   default:
      handled = false;
      break;
   }

   // the default case handles the escape key to quit.
   if (!handled)
   {
      return BaseInputComponent::HandleKeyPressed(keyboard, key);
   }

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
      mLogController = dynamic_cast<dtGame::LogController*>(GetGameManager()->
         GetComponentByName("LogController"));
   }
}

//////////////////////////////////////////////////////////////////////////
void InputComponent::FireGameEvent(const dtCore::GameEvent& event)
{
   dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
   GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, eventMsg);

   eventMsg->SetGameEvent(event);
   GetGameManager()->SendMessage(*eventMsg);
}
