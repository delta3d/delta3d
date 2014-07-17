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
#include "testappconstants.h"
#include "testappmessages.h"
#include "testappmessagetypes.h"

#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/rtsmotionmodel.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/walkmotionmodel.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/gamestatemessages.h>
#include <dtGame/messagefactory.h>
#include <dtCore/gameevent.h>

#include <iostream>



namespace dtExample
{
   ////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////
   InputComponent::InputComponent()
      : BaseClass("InputComponent")
      , mMotionModelMode(dtExample::MotionModelType::NONE)
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
            SetMotionModel(MotionModelType::WALK);
            break;
         case '2':
            SetMotionModel(MotionModelType::FLY);
            break;
         case '3':
            SetMotionModel(MotionModelType::UFO);
            break;
         case '4':
            SetMotionModel(MotionModelType::ORBIT);
            break;
         case '5':
            SetMotionModel(MotionModelType::FPS);
            break;
         case '6':
            SetMotionModel(MotionModelType::COLLISION);
            break;
         case '7':
            SetMotionModel(MotionModelType::RTS);
            break;

         // --- Menu Navigation Keys Section --- //
         case osgGA::GUIEventAdapter::KEY_Escape:
            SendTransitionMessage(dtExample::Transition::TRANSITION_BACK);
            break;
            
         case osgGA::GUIEventAdapter::KEY_Return:
            SendTransitionMessage(dtExample::Transition::TRANSITION_FORWARD);
            break;
            
         case osgGA::GUIEventAdapter::KEY_F1:
         case 'h':
            SendUIToggleMessage(dtExample::UINames::UI_HELP);
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

      if (type == dtGame::MessageType::INFO_MAP_LOADED)
      {
         SetCameraToPlayerStart();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::OnAddedToGM()
   {
      BaseClass::OnAddedToGM();

      // Set a motion model default.
      SetMotionModel(MotionModelType::FLY);
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
   void InputComponent::SetCameraToPlayerStart()
   {
      const std::string ACTOR_NAME("PlayerStart");

      dtCore::TransformableActorProxy* proxy = NULL;

      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorByName(ACTOR_NAME, proxy);

      if (proxy != NULL)
      {
         dtCore::Transformable* actor = NULL;
         proxy->GetActor(actor);

         if (actor != NULL)
         {
            dtCore::Transform xform;
            actor->GetTransform(xform);

            dtCore::Camera* camera = gm->GetApplication().GetCamera();
            camera->SetTransform(xform);
         }
         else
         {
            LOG_ERROR("Could not access actor for \"" + ACTOR_NAME + "\" proxy");
         }
      }
      else
      {
         LOG_ERROR("Could not find \"" + ACTOR_NAME + "\" to set initial camera position.");
      }
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendTransitionMessage(const dtExample::Transition& transition)
   {
      dtGame::GameManager* gm = GetGameManager();
      dtGame::MessageFactory& factory = gm->GetMessageFactory();
      
      dtCore::RefPtr<dtGame::GameStateTransitionRequestMessage> transitionMessage;
      factory.CreateMessage(dtGame::MessageType::REQUEST_GAME_STATE_TRANSITION, transitionMessage);
      
      transitionMessage->SetTransition(transition);

      gm->SendMessage(*transitionMessage);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendUIMessage(const std::string& uiName, const dtGame::MessageType& messageType)
   {
      dtGame::GameManager* gm = GetGameManager();
      dtGame::MessageFactory& factory = gm->GetMessageFactory();
      
      dtCore::RefPtr<dtExample::UIMessage> message;
      factory.CreateMessage(messageType, message);
      
      message->SetUIName(uiName);

      gm->SendMessage(*message);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendUIToggleMessage(const std::string& uiName)
   {
      SendUIMessage(uiName, TestAppMessageType::UI_TOGGLE);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendUIVisibilityMessage(const std::string& uiName, bool visible)
   {
      if (visible)
      {
         SendUIMessage(uiName, TestAppMessageType::UI_SHOW);
      }
      else // Hide
      {
         SendUIMessage(uiName, TestAppMessageType::UI_HIDE);
      }
   }
   
   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SendMotionModelChangedMessage(int motionModelType)
   {
      dtGame::GameManager* gm = GetGameManager();
      dtGame::MessageFactory& factory = gm->GetMessageFactory();
      
      dtCore::RefPtr<dtExample::MotionModelChangedMessage> message;
      factory.CreateMessage(dtExample::TestAppMessageType::MOTION_MODEL_CHANGED, message);
      
      message->SetNewMotionModelType(motionModelType);

      gm->SendMessage(*message);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SetMotionModel(int motionModelType)
   {
      // Prevent changing the motion model if it exists
      // and is the same type that is specified.
      if (mMotionModel.valid() && mMotionModelMode == motionModelType)
      {
         return;
      }

      dtCore::RefPtr<dtCore::MotionModel> motionModel;

      dtGame::GameManager* gm = GetGameManager();
      dtCore::Scene* scene = &gm->GetScene();
      dtABC::Application* app = &gm->GetApplication();

      dtCore::Keyboard* keyboard = app->GetKeyboard();
      dtCore::Mouse* mouse = app->GetMouse();

      switch (motionModelType)
      {
      case MotionModelType::WALK:
         {
            dtCore::RefPtr<dtCore::WalkMotionModel> wmm
               = new dtCore::WalkMotionModel(keyboard, mouse);
            wmm->SetScene(scene);
            motionModel = wmm;
         }
         break;

      case MotionModelType::FLY:
         motionModel = new dtCore::FlyMotionModel(keyboard, mouse);
         break;

      case MotionModelType::UFO:
         motionModel = new dtCore::UFOMotionModel(keyboard, mouse);
         break;

      case MotionModelType::ORBIT:
         motionModel = new dtCore::OrbitMotionModel(keyboard, mouse);
         break;

      case MotionModelType::FPS:
         {
            dtCore::RefPtr<dtCore::FPSMotionModel> fmm
               = new dtCore::FPSMotionModel(keyboard, mouse);
            fmm->SetScene(scene);
            motionModel = fmm;
         }
         break;

      case MotionModelType::COLLISION:
         motionModel = new dtCore::CollisionMotionModel(1.5f, 0.4f, 0.1f, scene, keyboard, mouse);
         break;

      case MotionModelType::RTS:
         motionModel = new dtCore::RTSMotionModel(keyboard, mouse);
         break;

      default:
         break;
      }

      motionModel->SetTarget(app->GetCamera());

      // Swap to the new motion model.
      mMotionModel = motionModel;
      mMotionModelMode = motionModelType;

      SendMotionModelChangedMessage(motionModelType);
   }

} // END - namespace dtExample
