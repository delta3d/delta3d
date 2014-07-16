/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "inputcomponent.h"
#include "guicomponent.h"

#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/rtsmotionmodel.h>
#include <dtCore/system.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/walkmotionmodel.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtCore/gameevent.h>

#include <iostream>



namespace dtExample
{
   ////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////
   enum MotionModelType
   {
      NONE, // 0
      WALK,
      FLY,
      UFO,
      ORBIT,
      FPS,
      COLLISION,
      RTS
   };



   ////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////
   InputComponent::InputComponent()
      : BaseClass("InputComponent")
      , mMotionModelMode(dtExample::NONE)
      , mSimSpeedFactor(1.0)
   {}

   ////////////////////////////////////////////////////////////////////////
   InputComponent::~InputComponent()
   {}

   //////////////////////////////////////////////////////////////////////////
   bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
   {
      std::ostringstream ss;
      bool handled = true;

      switch (key)
      {
         // --- Motion Model Section --- //
         case '1':
            SetMotionModel(dtExample::WALK);
            break;
         case '2':
            SetMotionModel(dtExample::FLY);
            break;
         case '3':
            SetMotionModel(dtExample::UFO);
            break;
         case '4':
            SetMotionModel(dtExample::ORBIT);
            break;
         case '5':
            SetMotionModel(dtExample::FPS);
            break;
         case '6':
            SetMotionModel(dtExample::COLLISION);
            break;
         case '7':
            SetMotionModel(dtExample::RTS);
            break;

         // --- Menu Navigation Keys Section --- //
         case osgGA::GUIEventAdapter::KEY_Escape:
            SendTransition(dtExample::Transition::TRANSITION_BACK);
            break;
            
         case osgGA::GUIEventAdapter::KEY_Return:
            SendTransition(dtExample::Transition::TRANSITION_FORWARD);
            break;

         // --- Character Control Section --- //
         case 'w':
         case 'a':
         case 's':
         case 'd':
         {
            // Do nothing.
         }
         break;

         // --- Control Adjustment Section --- //
         case '0':
         {
            mSimSpeedFactor = 1.0f;
            ss << "Resetting Game Manager Speed to [" << mSimSpeedFactor << "] == Realtime.";
            std::cout << ss.str() << std::endl;
            GetGameManager()->ChangeTimeSettings(GetGameManager()->GetSimulationTime(),
               mSimSpeedFactor, GetGameManager()->GetSimulationClockTime());
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

         case 'i':
         {
            SendPlayerUpdateMsg("Velocity", 10.0f);
         }
         break;

         case 'k':
         {
            SendPlayerUpdateMsg("Velocity", -10.0f);
         }
         break;

         case 'j':
         {
            SendPlayerUpdateMsg("Turn Rate", 0.25f);
         }
         break;

         case 'l':
         {
            SendPlayerUpdateMsg("Turn Rate", -0.25f);
         }
         break;

         case osgGA::GUIEventAdapter::KEY_F1:
         {
   //         if (mHudGUI->GetHUDState() == HUDState::HELP)
   //         {
   //            mHudGUI->CycleToNextHUDState(); // already in help, so toggle it off
   //         }
   //         else
   //         {
   //            mHudGUI->SetHUDState(HUDState::HELP);
   //         }
         }
         break;

         case osgGA::GUIEventAdapter::KEY_F2:
         {
   //         mHudGUI->CycleToNextHUDState();
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
   bool InputComponent::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
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

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::ProcessMessage(const dtGame::Message& message)
   {
      const dtGame::MessageType& type = message.GetMessageType();

      if (type == dtGame::MessageType::INFO_ACTOR_DELETED)
      {
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::OnAddedToGM()
   {
      BaseClass::OnAddedToGM();

      SetMotionModel(dtExample::WALK);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendPlayerUpdateMsg(const std::string& paramName, const float value)
   {
   //   dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
   //   dtGame::ActorUpdateMessage& aum = static_cast<dtGame::ActorUpdateMessage&>(*msg);
   //   aum.SetAboutActorId(mPlayer->GetId());
   //   dtGame::MessageParameter* mp = aum.AddUpdateParameter(paramName, dtCore::DataType::FLOAT);
   //   static_cast<dtGame::FloatMessageParameter*>(mp)->SetValue(value);
   //   GetGameManager()->SendMessage(aum);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendTransition(const dtExample::Transition& transition)
   {
      // TODO:
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SetMotionModel(int motionModelType)
   {
      dtCore::RefPtr<dtCore::MotionModel> motionModel;

      dtGame::GameManager* gm = GetGameManager();
      dtCore::Scene* scene = &gm->GetScene();
      dtABC::Application* app = &gm->GetApplication();

      dtCore::Keyboard* keyboard = app->GetKeyboard();
      dtCore::Mouse* mouse = app->GetMouse();

      switch (motionModelType)
      {
      case dtExample::WALK:
         {
            dtCore::RefPtr<dtCore::WalkMotionModel> wmm
               = new dtCore::WalkMotionModel(keyboard, mouse);
            wmm->SetScene(scene);
            motionModel = wmm;
         }
         break;
      case dtExample::FLY:
         motionModel = new dtCore::FlyMotionModel(keyboard, mouse);
         break;
      case dtExample::UFO:
         motionModel = new dtCore::UFOMotionModel(keyboard, mouse);
         break;
      case dtExample::ORBIT:
         motionModel = new dtCore::OrbitMotionModel(keyboard, mouse);
         break;
      case dtExample::FPS:
         {
            dtCore::RefPtr<dtCore::FPSMotionModel> fmm
               = new dtCore::FPSMotionModel(keyboard, mouse);
            fmm->SetScene(scene);
            motionModel = fmm;
         }
         break;
      case dtExample::COLLISION:
         motionModel = new dtCore::CollisionMotionModel(1.5f, 0.4f, 0.1f, scene, keyboard, mouse);
         break;
      case dtExample::RTS:
         motionModel = new dtCore::RTSMotionModel(keyboard, mouse);
         break;
      default:
         break;
      }

      motionModel->SetTarget(app->GetCamera());

      // Swap to the new motion model.
      mMotionModel = motionModel;
      mMotionModelMode = motionModelType;
   }

} // END - namespace dtExample
