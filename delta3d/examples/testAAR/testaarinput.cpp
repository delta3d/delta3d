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
#include <dtGame/actorupdatemessage.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/basemessages.h>
#include <dtGame/logtag.h>
#include <dtLMS/lmscomponent.h>
#include <dtDAL/gameevent.h>
#include <ctime>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #if !defined snprintf
      #define snprintf _snprintf
   #endif
#endif

////////////////////////////////////////////////////////////////////
TestAARInput::TestAARInput(const std::string &name, dtGame::LogController &logCtrl, TestAARHUD &hudComp) :
   dtGame::BaseInputComponent(name),
   mKeyIsPressed(false), 
   mSimSpeedFactor(1.0), 
   mHudGUI(&hudComp), 
   mLogController(&logCtrl)
{
  
}

////////////////////////////////////////////////////////////////////////
TestAARInput::~TestAARInput()
{

}

//////////////////////////////////////////////////////////////////////////
bool TestAARInput::HandleKeyPressed(const dtCore::Keyboard *keyBoard,
                                    Producer::KeyboardKey key, 
                                    Producer::KeyCharacter character)
{
   std::ostringstream ss;
   bool handled = true;
   
   switch(key)
   {
      case Producer::Key_space:
      {
         GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::REQUEST_ALL_CONTROLLER_UPDATES));
      }
      break;

      case Producer::Key_W:
      case Producer::Key_A:
      case Producer::Key_S:
      case Producer::Key_D:
      {
         GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::UPDATE_TASK_CAMERA));
      }
      break;

      case Producer::Key_0:
      {
         mSimSpeedFactor = 1.0f;
         ss << "Resetting Game Manager Speed to [" << mSimSpeedFactor << "] == Realtime.";
         std::cout << ss.str() << std::endl;
         GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
            mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
      }
      break;

      case Producer::Key_1:
      {
         if(mLogController->GetLastKnownStatus().GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::RESET));
         }

         mLogController->RequestChangeStateToIdle();
         GetGameManager()->SetPaused(false);
      }
      break;

      case Producer::Key_2:
      {
         mLogController->RequestChangeStateToRecord();
         
         FireEvent(*TestAARGameEvent::EVENT_START_RECORD);
      }
      break;

      case Producer::Key_3:
      {
         //Going from idle mode to playback mode so we need to remove the player..
         if(mLogController->GetLastKnownStatus().GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE)
         {
            GetGameManager()->DeleteAllActors();
            dtCore::System::GetInstance().Step();
         }

         mLogController->RequestChangeStateToPlayback();
         
         GetGameManager()->SetPaused(false);
      }
      break;

      case Producer::Key_minus:
      case Producer::Key_KP_Subtract:
      {
         mSimSpeedFactor = mSimSpeedFactor * 0.9f;
         if(mSimSpeedFactor < 0.10f)
            mSimSpeedFactor = 0.10f;
         else
         {
            ss << "Decreasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
            mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
      }
      break;

      case Producer::Key_KP_Add:
      case Producer::Key_equal:
      case Producer::KeyChar_plus:
      {
         mSimSpeedFactor = mSimSpeedFactor * 1.20f;
         if(mSimSpeedFactor > 10.0f)
            mSimSpeedFactor = 10.0f;
         else
         {
            ss << "Increasing Game Manager Speed to [" << mSimSpeedFactor << "]X Realtime.";
            std::cout << ss.str() << std::endl;
         }

         GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
            mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
      }
      break;

      case Producer::Key_P:
      {
         GetGameManager()->SetPaused(!GetGameManager()->IsPaused());
      }
      break;

      case Producer::Key_B:
      {
         if(mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::PLACE_ACTOR);
            GetGameManager()->SendMessage(*msg);
         }   
      }
      break;

      case Producer::Key_G:
      {
         if(mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
         {
            dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::PLACE_IGNORED_ACTOR);
            GetGameManager()->SendMessage(*msg);
         }   
      }
      break;

      case Producer::Key_T:
      {
         InsertTag();
      }
      break;

      case Producer::Key_F:
      {
         InsertKeyFrame();
      }
      break;

      case Producer::Key_I:
      {
         SendPlayerUpdateMsg("Velocity", 10.0f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_FORWARD);
      }
      break;

      case Producer::Key_K:
      {
         SendPlayerUpdateMsg("Velocity", -10.0f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_BACKWARD);
      }
      break;

      case Producer::Key_J:
      {
         SendPlayerUpdateMsg("Turn Rate", 0.25f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_LEFT);
      }
      break;

      case Producer::Key_L:
      {
         SendPlayerUpdateMsg("Turn Rate", -0.25f);
         FireEvent(*TestAARGameEvent::EVENT_PLAYER_RIGHT);
      }
      break;

      case Producer::Key_F1:
      {
         if(mHudGUI->GetHUDState() == HUDState::HELP)
            mHudGUI->CycleToNextHUDState(); // already in help, so toggle it off
         else
            mHudGUI->SetHUDState(HUDState::HELP);
      }
      break;

      case Producer::Key_F2:
      {
         mHudGUI->CycleToNextHUDState();
      }
      break;

      case Producer::Key_Return:
      {
         GetGameManager()->GetApplication().GetCamera()->SetNextStatisticsType();
      }
      break;

      case Producer::Key_M:
      {
         GetGameManager()->SendMessage(*GetGameManager()->GetMessageFactory().CreateMessage(TestAARMessageType::PRINT_TASKS));
      }
      break;

      case Producer::Key_comma:
      {
         GotoPreviousKeyframe();
      }
      break;

      case Producer::Key_period:
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

   if(!handled)
      return GetGameManager()->GetApplication().KeyPressed(keyBoard, key, character);

   return handled;
}

//////////////////////////////////////////////////////////////////////////
bool TestAARInput::HandleKeyReleased(const dtCore::Keyboard* keyboard, 
                                     Producer::KeyboardKey key,
                                     Producer::KeyCharacter character)
{
   bool handled = true;

   switch(key)
   {
      case Producer::Key_I:
      case Producer::Key_K:
      {
         SendPlayerUpdateMsg("Velocity", 0.0f);
      }
      break;

      case Producer::Key_J:
      case Producer::Key_L:
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

   return handled ? handled : dtGame::BaseInputComponent::HandleKeyReleased(keyboard, key, character); 
}

//////////////////////////////////////////////////////////////////////////
void TestAARInput::FireEvent(TestAARGameEvent &event)
{
   if(mLogController->GetLastKnownStatus().GetStateEnum() != dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK)
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
   const std::vector<dtGame::LogKeyframe> &frames = mLogController->GetLastKnownKeyframeList();
   const dtGame::LogKeyframe *prevFrame = NULL;

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
   const std::vector<dtGame::LogKeyframe> &frames = mLogController->GetLastKnownKeyframeList();
   const dtGame::LogKeyframe *nextFrame = NULL;

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
   const dtGame::MessageType &type = message.GetMessageType();
   
   if(type == dtGame::MessageType::INFO_ACTOR_DELETED)
   {
      if(mPlayer != NULL && message.GetAboutActorId() == mPlayer->GetId())
         mPlayer = NULL;
   }
}

////////////////////////////////////////////////////////////////////////
void TestAARInput::SendPlayerUpdateMsg(const std::string &paramName, const float value)
{
   if(mPlayer == NULL)
      return;

   dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
   dtGame::ActorUpdateMessage &aum = static_cast<dtGame::ActorUpdateMessage&>(*msg);
   aum.SetAboutActorId(mPlayer->GetId());
   dtGame::MessageParameter *mp = aum.AddUpdateParameter(paramName, dtDAL::DataType::FLOAT);
   static_cast<dtGame::FloatMessageParameter*>(mp)->SetValue(value);
   GetGameManager()->SendMessage(aum);
}
