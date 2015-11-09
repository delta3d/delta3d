/* -*-c++-*-
 * testAAR - testaarinput (.h & .cpp) - Using 'The MIT License'
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
 * Christopher DuBuc
 * William E. Johnson II
 */

#include "testaarinput.h"
#include "testaarmessagetypes.h"
#include "testaarmessageprocessor.h"
#include "testaargameevent.h"
#include "testaarhud.h"

#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/basemessages.h>
#include <dtGame/logtag.h>
#include <dtGame/messagefactory.h>
#include <dtLMS/lmscomponent.h>
#include <dtCore/gameevent.h>

#include <iostream>

//for snprintf
#include <dtUtil/mswinmacros.h>

const dtCore::RefPtr<dtCore::SystemComponentType> TestAARInput::TYPE(new dtCore::SystemComponentType("TestAARInput", "GMComponents", "Test AAR Input", dtGame::BaseInputComponent::DEFAULT_TYPE));

////////////////////////////////////////////////////////////////////
TestAARInput::TestAARInput(dtGame::LogController& logCtrl, TestAARHUD& hudComp)
   : dtGame::BaseInputComponent(*TYPE)
   , mSimSpeedFactor(1.0)
   , mHudGUI(&hudComp)
   , mPlayer(NULL)
   , mLogController(&logCtrl)
{

}

////////////////////////////////////////////////////////////////////////
TestAARInput::~TestAARInput()
{

}

//////////////////////////////////////////////////////////////////////////
bool TestAARInput::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
{
   std::ostringstream ss;
   bool handled = true;

   switch (key)
   {
      case ' ':
      {
         GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::REQUEST_ALL_CONTROLLER_UPDATES));
      }
      break;

      case 'w':
      case 'a':
      case 's':
      case 'd':
      {
         GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::UPDATE_TASK_CAMERA));
      }
      break;

      case '0':
      {
         mSimSpeedFactor = 1.0f;
         ss << "Resetting Game Manager Speed to [" << mSimSpeedFactor << "] == Realtime.";
         std::cout << ss.str() << std::endl;
         GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
            mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
      }
      break;

      case '1':
      {
         if (mLogController->GetLastKnownStatus().GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::RESET));
         }

         mLogController->RequestChangeStateToIdle();
         GetGameManager()->SetPaused(false);
      }
      break;

      case '2':
      {
         mLogController->RequestChangeStateToRecord();

         FireEvent(*TestAARGameEvent::EVENT_START_RECORD);
      }
      break;

      case '3':
      {
         //Going from idle mode to playback mode so we need to remove the player..
         if (mLogController->GetLastKnownStatus().GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            GetGameManager()->DeleteAllActors();
            dtCore::System::GetInstance().Step();
         }

         mLogController->RequestChangeStateToPlayback();

         GetGameManager()->SetPaused(false);
      }
      break;

      case '-':
      case osgGA::GUIEventAdapter::KEY_KP_Subtract:
      {
         mSimSpeedFactor = mSimSpeedFactor * 0.9f;
         if (mSimSpeedFactor < 0.10f)
         {
            mSimSpeedFactor = 0.10f;
         }
         else
         {
            ss << "Decreasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
            mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
      }
      break;

      case osgGA::GUIEventAdapter::KEY_KP_Add:
      case '=':
      case '+':
      {
         mSimSpeedFactor = mSimSpeedFactor * 1.20f;
         if (mSimSpeedFactor > 10.0f)
         {
            mSimSpeedFactor = 10.0f;
         }
         else
         {
            ss << "Increasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
            mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
      }
      break;

      case 'p':
      {
         GetGameManager()->SetPaused(!GetGameManager()->IsPaused());
      }
      break;

      case 'b':
      {
         if (mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::PLACE_ACTOR);
            GetGameManager()->SendMessage(*msg);
         }
      }
      break;

      case 'g':
      {
         if (mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::PLACE_IGNORED_ACTOR);
            GetGameManager()->SendMessage(*msg);
         }
      }
      break;

      case 't':
      {
         InsertTag();
      }
      break;

      case 'f':
      {
         InsertKeyFrame();
      }
      break;

      case 'i':
      {
         SendPlayerUpdateMsg("Velocity", 10.0f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_FORWARD);
      }
      break;

      case 'k':
      {
         SendPlayerUpdateMsg("Velocity", -10.0f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_BACKWARD);
      }
      break;

      case 'j':
      {
         SendPlayerUpdateMsg("Turn Rate", 0.25f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_LEFT);
      }
      break;

      case 'l':
      {
         SendPlayerUpdateMsg("Turn Rate", -0.25f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_RIGHT);
      }
      break;

      case osgGA::GUIEventAdapter::KEY_F1:
      {
         if (mHudGUI->GetHUDState() == HUDState::HELP)
         {
            mHudGUI->CycleToNextHUDState(); // already in help, so toggle it off
         }
         else
         {
            mHudGUI->SetHUDState(HUDState::HELP);
         }
      }
      break;

      case osgGA::GUIEventAdapter::KEY_F2:
      {
         mHudGUI->CycleToNextHUDState();
      }
      break;

      case osgGA::GUIEventAdapter::KEY_Return:
      {
//         GetGameManager()->GetApplication().GetCamera()->SetNextStatisticsType(); TODO
      }
      break;

      case 'm':
      {
         GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::PRINT_TASKS));
      }
      break;

      case ',':
      {
         GotoPreviousKeyframe();
      }
      break;

      case '.':
      {
         GotoNextKeyframe();
      }
      break;

      default:
      {
         handled = false;
      }
      break;
   };

   if (!handled)
   {
      return GetGameManager()->GetApplication().KeyPressed(keyBoard, key);
   }

   return handled;
}

//////////////////////////////////////////////////////////////////////////
bool TestAARInput::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = true;

   switch (key)
   {
      case 'i':
      case 'k':
      {
         SendPlayerUpdateMsg("Velocity", 0.0f);
      }
      break;

      case 'j':
      case 'l':
      {
         SendPlayerUpdateMsg("Turn Rate", 0.0f);
      }
      break;

      default:
      {
         handled = false;
      }
      break;
   }

   return handled ? handled : dtGame::BaseInputComponent::HandleKeyReleased(keyboard, key);
}

//////////////////////////////////////////////////////////////////////////
void TestAARInput::FireEvent(dtCore::GameEvent& event)
{
   if (mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
   {
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
      GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, eventMsg);

      eventMsg->SetGameEvent(event);
      GetGameManager()->SendMessage(*eventMsg);
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARInput::GotoPreviousKeyframe()
{
   const std::vector<dtGame::LogKeyframe>& frames = mLogController->GetLastKnownKeyframeList();
   const dtGame::LogKeyframe* prevFrame = NULL;

   for (unsigned int i = 0; i < frames.size(); i ++)
   {
      // Get the oldest frame less than the current sim time.  Allow a 3 second grace period
      // so that we don't have the same problem you do on CD players
      if ((frames[i].GetSimTimeStamp() + 2.25) < GetGameManager()->GetSimulationTime() &&
         (prevFrame == NULL || frames[i].GetSimTimeStamp() > prevFrame->GetSimTimeStamp()))
      {
         prevFrame = &(frames[i]);
      }
   }

   // found one, so jump to keyframe
   if (prevFrame != NULL)
   {
      std::ostringstream ss;
      ss << "## Attempting to Jump to Previous Keyframe: [" << prevFrame->GetName() << "] ##";
      std::cout << ss.str() << std::endl;

      mLogController->RequestJumpToKeyframe(*prevFrame);
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARInput::GotoNextKeyframe()
{
   const std::vector<dtGame::LogKeyframe>& frames = mLogController->GetLastKnownKeyframeList();
   const dtGame::LogKeyframe* nextFrame = NULL;

   for (unsigned int i = 0; i < frames.size(); i ++)
   {
      // Get the first frame older than the current sim time.
      if (frames[i].GetSimTimeStamp() > GetGameManager()->GetSimulationTime() &&
         (nextFrame == NULL || frames[i].GetSimTimeStamp() < nextFrame->GetSimTimeStamp()))
      {
         nextFrame = &(frames[i]);
      }
   }

   // found one, so jump to keyframe
   if (nextFrame != NULL)
   {
      std::ostringstream ss;
      ss << "## Attempting to Jump to Next Keyframe: [" << nextFrame->GetName() << "] ##";
      std::cout << ss.str() << std::endl;

      mLogController->RequestJumpToKeyframe(*nextFrame);
   }
}

//////////////////////////////////////////////////////////////////////////
void TestAARInput::InsertTag()
{
   char clin[100];            // buffer to print
   dtGame::LogTag newTag;

   newTag.SetCaptureKeyframe(false);
   newTag.SetDescription("Nifty Tag Description");
   snprintf(clin, 100, "My Tag At [%.2f]", GetGameManager()->GetSimulationTime());
   newTag.SetName(clin);

   mLogController->RequestInsertTag(newTag);
   mLogController->RequestServerGetTags();
}

//////////////////////////////////////////////////////////////////////////
void TestAARInput::InsertKeyFrame()
{
   char clin[100];            // buffer to print
   dtGame::LogKeyframe newKeyframe;

   newKeyframe.SetDescription("Nifty Keyframe Description");
   snprintf(clin, 100, "Frame At [%.2f]", GetGameManager()->GetSimulationTime());
   newKeyframe.SetName(clin);

   mLogController->RequestCaptureKeyframe(newKeyframe);
   mLogController->RequestServerGetKeyframes();
}

////////////////////////////////////////////////////////////////////////
void TestAARInput::ProcessMessage(const dtGame::Message& message)
{
   const dtGame::MessageType& type = message.GetMessageType();

   if (type == dtGame::MessageType::INFO_ACTOR_DELETED)
   {
      if (mPlayer != NULL && message.GetAboutActorId() == mPlayer->GetId())
      {
         mPlayer = NULL;
      }
   }
}

////////////////////////////////////////////////////////////////////////
void TestAARInput::SendPlayerUpdateMsg(const std::string& paramName, const float value)
{
   if (mPlayer == NULL)
   {
      return;
   }

   dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
   dtGame::ActorUpdateMessage& aum = static_cast<dtGame::ActorUpdateMessage&>(*msg);
   aum.SetAboutActorId(mPlayer->GetId());
   dtGame::MessageParameter* mp = aum.AddUpdateParameter(paramName, dtCore::DataType::FLOAT);
   static_cast<dtGame::FloatMessageParameter*>(mp)->SetValue(value);
   GetGameManager()->SendMessage(aum);
}
