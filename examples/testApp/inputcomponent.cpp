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
   ////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString InputComponent::DEFAULT_NAME("InputComponent");
   const dtUtil::RefString InputComponent::DEFAULT_ATTACH_ACTOR_NAME("Gina");
   const dtUtil::RefString InputComponent::DEFAULT_PLAYER_START_NAME("PlayerStart");



   ////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////
   InputComponent::InputComponent()
      : BaseClass(DEFAULT_NAME)
      , mSimSpeedFactor(1.0)
      , mMotionModelMode(&dtExample::MotionModelType::NONE)
      , mMotionModel(NULL)
      , mCamera(NULL)
      , mCameraPivot(NULL)
      , mGroundClamper(NULL)
      , mAttachActorName(DEFAULT_ATTACH_ACTOR_NAME)
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

      if (type == dtGame::MessageType::TICK_LOCAL)
      {
         const dtGame::TickMessage* tickMessage
            = static_cast<const dtGame::TickMessage*>(&message);

         Update(tickMessage->GetDeltaSimTime(), tickMessage->GetDeltaRealTime());
      }
      else if (type == dtGame::MessageType::INFO_MAP_LOADED)
      {
         SetCameraToPlayerStart();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::OnAddedToGM()
   {
      BaseClass::OnAddedToGM();

      mCamera = GetGameManager()->GetApplication().GetCamera();
      mCameraPivot = new dtCore::Transformable;

      // Set a motion model default.
      SetMotionModel(MotionModelType::FLY);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::Update(float simTimeDelta, float realTimeDelta)
   {
      DoGroundClamping();
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
   dtCore::Transformable* InputComponent::GetActorByName(const std::string& name)
   {
      dtCore::Transformable* actor = NULL;
      dtCore::TransformableActorProxy* proxy = NULL;

      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorByName(name, proxy);

      if (proxy != NULL)
      {
         proxy->GetActor(actor);

         if (actor == NULL)
         {
            LOG_ERROR("Could not access actor \"" + name + "\".");
         }
      }
      else
      {
         LOG_ERROR("Could not find proxy for actor \"" + name + "\".");
      }

      return actor;
   }

   ////////////////////////////////////////////////////////////////////////
   bool InputComponent::SetCameraPivotToActor(const std::string& actorName)
   {
      // Detach from the current actor.
      if (mCurrentActor.valid())
      {
         // If this is the same actor, exit early.
         if (mCurrentActor->GetName() == actorName)
         {
            // Return true because the camera pivot should be attached.
            return true;
         }

         mCurrentActor->RemoveChild(mCameraPivot.get());
      }

      mCurrentActor = GetActorByName(actorName);

      if ( ! mCurrentActor.valid())
      {
         return false;
      }

      dtCore::Transform originalXform;
      mCurrentActor->GetTransform(originalXform);

      // Temporarily set the character to the origin
      // just to be sure things are attached to their
      // proper locations.
      osg::Vec3 offset;
      osg::Vec3 originalPos = originalXform.GetTranslation();

      dtCore::Transform xform;
      mCurrentActor->AddChild(mCameraPivot);
      mCameraPivot->SetTransform(originalXform);
      offset.set(0.0f, 0.0f, 1.25f);
      xform.SetTranslation(offset);
      mCameraPivot->SetTransform(xform, dtCore::Transformable::REL_CS);
      
      dtCore::Transform camXform;
      mCameraPivot->AddChild(mCamera);
      mCamera->SetTransform(originalXform);
      offset.set(0.0f, -5.0f, 0.0f);
      camXform.SetTranslation(offset);
      mCamera->SetTransform(camXform, dtCore::Transformable::REL_CS);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SetCameraToPlayerStart()
   {
      const std::string ACTOR_NAME(DEFAULT_PLAYER_START_NAME);
      dtCore::Transformable* actor = GetActorByName(ACTOR_NAME);

      if (actor != NULL)
      {
         dtCore::Transform xform;
         actor->GetTransform(xform);

         mCamera->SetTransform(xform);
      }
      else
      {
         LOG_ERROR("Could not move camera to actor \"" + ACTOR_NAME + "\".");
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
   void InputComponent::SendMotionModelChangedMessage(const dtExample::MotionModelType& motionModelType)
   {
      dtGame::GameManager* gm = GetGameManager();
      dtGame::MessageFactory& factory = gm->GetMessageFactory();
      
      dtCore::RefPtr<dtExample::MotionModelChangedMessage> message;
      factory.CreateMessage(dtExample::TestAppMessageType::MOTION_MODEL_CHANGED, message);
      
      message->SetNewMotionModelType(motionModelType);

      gm->SendMessage(*message);
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::SetMotionModel(const dtExample::MotionModelType& motionModelType)
   {
      // Prevent changing the motion model if it exists
      // and is the same type that is specified.
      if ( ! mCamera.valid() || (mMotionModel.valid() && mMotionModelMode == &motionModelType))
      {
         return;
      }

      dtCore::RefPtr<dtCore::MotionModel> motionModel;

      dtGame::GameManager* gm = GetGameManager();
      dtCore::Scene* scene = &gm->GetScene();
      dtABC::Application* app = &gm->GetApplication();

      dtCore::Keyboard* keyboard = app->GetKeyboard();
      dtCore::Mouse* mouse = app->GetMouse();

      bool attachToCharacter = false;

      if (&motionModelType == &MotionModelType::WALK)
      {
         dtCore::RefPtr<dtCore::WalkMotionModel> wmm
            = new dtCore::WalkMotionModel(keyboard, mouse);
         wmm->SetScene(scene);
         motionModel = wmm;
      }
      else if (&motionModelType == &MotionModelType::FLY)
      {
         motionModel = new dtCore::FlyMotionModel(keyboard, mouse);
      }
      else if (&motionModelType == &MotionModelType::UFO)
      {
         motionModel = new dtCore::UFOMotionModel(keyboard, mouse);
      }
      else if (&motionModelType == &MotionModelType::ORBIT)
      {
         dtCore::RefPtr<dtCore::OrbitMotionModel> omm = new dtCore::OrbitMotionModel(keyboard, mouse);
         omm->SetDistance(0.0f);
         motionModel = omm;
         attachToCharacter = true;
      }
      else if (&motionModelType == &MotionModelType::FPS)
      {
         dtCore::RefPtr<dtCore::FPSMotionModel> fmm
            = new dtCore::FPSMotionModel(keyboard, mouse);
         fmm->SetScene(scene);
         motionModel = fmm;
      }
      else if (&motionModelType == &MotionModelType::COLLISION)
      {
         motionModel = new dtCore::CollisionMotionModel(1.5f, 0.4f, 0.1f, scene, keyboard, mouse);
      }
      else if (&motionModelType == &MotionModelType::RTS)
      {
         motionModel = new dtCore::RTSMotionModel(keyboard, mouse);
      }


      // Swap to the new motion model.
      mMotionModel = motionModel;
      mMotionModelMode = &motionModelType;

      // Acquire the camera's current transform in case
      // it is to be dettached from an actor.
      dtCore::Transform xform;
      mCamera->GetTransform(xform);

      if (attachToCharacter)
      {
         SetCameraPivotToActor(mAttachActorName);
         
         mMotionModel->SetTarget(mCameraPivot.get());
      }
      else
      {
         mCurrentActor = NULL;

         mCameraPivot->RemoveChild(mCamera.get());

         // The camera may have been detached so set
         // the previous transform so that it does not
         // warp to a place that does not make sense.
         mCamera->SetTransform(xform);

         mMotionModel->SetTarget(mCamera.get());
      }

      SendMotionModelChangedMessage(motionModelType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputComponent::DoGroundClamping()
   {
      /*if ( ! mGroundClamper.valid())
      {
         mGroundClamper = new dtGame::DefaultGroundClamper;
      }

      // Ground clamp dynamically created actors
      mGroundClamper->SetTerrainActor(GetTerrain());

      dtGame::GroundClampingData gcData;
      gcData.SetAdjustRotationToGround(false);
      gcData.SetUseModelDimensions(false);
      gcData.SetGroundClampType(dtGame::GroundClampTypeEnum::KEEP_ABOVE_GROUND);

      dtCore::Transform transform;
      mCamera->GetTransform(transform, dtCore::Transformable::REL_CS);

      dtGame::GameActorProxy& actor = mActorList[actorIndex]->GetGameActorProxy();

      // Add this actor to the ground clamp batch
      mGroundClamper->ClampToGround(dtGame::BaseGroundClamper::GroundClampRangeType::RANGED,
         0.0, transform, actor, gcData, true);

      // Run the batch ground clamp
      mGroundClamper->FinishUp();*/
   }

} // END - namespace dtExample
