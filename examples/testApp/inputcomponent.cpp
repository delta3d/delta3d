/* -*-c++-*-
 * testApp - Using 'The MIT License'
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
//#include <dtCore/walkmotionmodel.h>

#include <dtPhysics/geometry.h>
#include <dtPhysics/charactercontroller.h>
#include <dtPhysics/charactermotionmodel.h>
#include <dtPhysics/physicscomponent.h>

#include <dtCore/shadermanager.h> //for reloading shader defs

#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basegroundclamper.h>
#include <dtGame/basemessages.h>
#include <dtGame/gamestatemessages.h>
#include <dtGame/messagefactory.h>
#include <dtCore/gameevent.h>

#include <dtRender/scenemanager.h>
#include <dtRender/ephemerisscene.h>

#if defined(USE_INSPECTOR)
   #include <dtInspectorQt/inspectorqt.h>
#endif

#include <dtActors/engineactorregistry.h>
#include <iostream>



namespace dtExample
{
   ////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString InputComponent::DEFAULT_NAME("InputComponent");
   const dtUtil::RefString InputComponent::DEFAULT_ATTACH_ACTOR_NAME("Gina");
   const dtUtil::RefString InputComponent::DEFAULT_PLAYER_START_NAME("PlayerStart");
   const dtUtil::RefString InputComponent::DEFAULT_TERRAIN_NAME("Terrain");

   static const float CAMERA_HEIGHT = 0.0f;



   ////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////
   InputComponent::InputComponent()
      : BaseClass(DEFAULT_NAME)
      , mClampCameraEnabled(false)
      , mMotionModelMode(&dtExample::MotionModelType::NONE)
      , mMotionModel(NULL)
      , mCamera(NULL)
      , mCameraPivot(NULL)
      , mGroundClampedXformable(NULL)
      , mGroundClampedObject(NULL)
      , mGroundClamper(NULL)
      , mAttachActorName(DEFAULT_ATTACH_ACTOR_NAME)
   {
      int argc = 0;
      mInspector = new dtInspectorQt::InspectorQt(argc, NULL);
      mInspector->SetVisible(false);
   }

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
            dtRender::SceneManager* sm = dynamic_cast<dtRender::SceneManager*>(GetGameManager()->GetEnvironmentActor()->GetDrawable());

            if(sm != NULL)
            {

               dtRender::EphemerisScene* eph = dynamic_cast<dtRender::EphemerisScene*>(sm->FindSceneByType(*dtRender::EphemerisScene::EPHEMERIS_SCENE));
               if(eph != NULL)
               {
                  eph->SetTimeFromSystem();
               }
            }
         }
         break;

         case '\\':
         case '|':
         {
            GetGameManager()->GetApplication().SetNextStatisticsType();
         }
         break;
         case '-':
         case osgGA::GUIEventAdapter::KEY_KP_Subtract:
         {
            IncrementTime(-200.0f);
         }
         break;

         case osgGA::GUIEventAdapter::KEY_KP_Add:
         case '=':
         case '+':
         {
            IncrementTime(200.0f);
         }
         break;

         case 'p':
         {
            dtCore::ShaderManager::GetInstance().ReloadAndReassignShaderDefinitions("shaders/ShaderDefinitions.xml");
         }
         break;
         case 'P':
         {
            dtPhysics::PhysicsComponent* physComp = NULL;
            GetGameManager()->GetComponentByName(dtPhysics::PhysicsComponent::DEFAULT_NAME, physComp);
            if (physComp != NULL)
            {
               physComp->SetNextDebugDrawMode();
            }
         }
         break;
#if defined(USE_INSPECTOR)
         case '`':
         {
            mInspector->SetVisible(true);
            return true;
         }
#endif

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

      /*switch (key)
      {
      default:
      {
         handled = false;
      }
      break;
      }*/

      // TODO: Replace this line if handing key released.
      handled = false;

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
         
         DoGroundClamping(tickMessage->GetSimulationTime());
      }
      else if (type == dtGame::MessageType::INFO_MAP_LOADED)
      {
         SetCameraToPlayerStart();

         // Set a motion model default.
         SetMotionModel(MotionModelType::FLY);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::OnAddedToGM()
   {
      BaseClass::OnAddedToGM();
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::Update(float simTimeDelta, float realTimeDelta)
   {
      // TODO:
   }

   ////////////////////////////////////////////////////////////////////////
   dtCore::TransformableActorProxy* InputComponent::GetActorByName(const std::string& name)
   {
      dtCore::TransformableActorProxy* actor = NULL;

      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorByName(name, actor);

      if (actor == NULL)
      {
         LOG_DEBUG("Could not find actor \"" + name + "\".");
      }

      return actor;
   }

   ////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* InputComponent::GetDrawableByName(const std::string& name)
   {
      dtCore::Transformable* xformable = NULL;
      dtCore::TransformableActorProxy* actor = GetActorByName(name);

      if (actor != NULL)
      {
         actor->GetDrawable(xformable);
      }

      if (xformable == NULL)
      {
         LOG_DEBUG("Could not find  \"" + name + "\".");
      }

      return xformable;
   }

   ////////////////////////////////////////////////////////////////////////
   bool InputComponent::SetCameraPivot(const std::string& actorName)
   {
      // Detach from the current actor.
      if (mGroundClampedXformable.valid())
      {
         // If this is the same actor, exit early.
         if (mGroundClampedXformable->GetName() == actorName)
         {
            // Return true because the camera pivot should be attached.
            return true;
         }

         mGroundClampedXformable->RemoveChild(mCameraPivot);
      }

      mGroundClampedXformable = GetDrawableByName(actorName);

      if ( ! mGroundClampedXformable.valid())
      {
         return false;
      }

      dtCore::Transform originalXform;
      mGroundClampedXformable->GetTransform(originalXform);

      // Temporarily set the character to the origin
      // just to be sure things are attached to their
      // proper locations.
      osg::Vec3 offset;
      //osg::Vec3 originalPos = originalXform.GetTranslation();

      dtCore::Transform xform;
      mGroundClampedXformable->AddChild(mCameraPivot);
      mCameraPivot->SetTransform(originalXform);
      offset.set(0.0f, 0.0f, 1.25f);
      xform.SetTranslation(offset);
      mCameraPivot->SetTransform(xform, dtCore::Transformable::REL_CS);
      
      dtCore::Transform camXform;
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

      dtGame::GameManager* gm = GetGameManager();

      mCamera = GetGameManager()->GetApplication().GetCamera();
      mCameraPivot = new dtCore::Transformable;

      //try to find the default player start actor type
      dtCore::TransformableActorProxy* playerStart = NULL;      
      gm->FindActorByType(*dtActors::EngineActorRegistry::PLAYER_START_ACTOR_TYPE, playerStart);

      //otherwise try to find by the default player start actor name
      if(playerStart == NULL)
      {
         playerStart = GetActorByName(ACTOR_NAME);
      }


      if(playerStart != NULL)
      {
         mGroundClampedObject = playerStart;
      
         dtCore::Transformable* xformable = NULL;
         mGroundClampedObject->GetDrawable(xformable);
         mCameraPivot = xformable ;
         mCameraPivot->Emancipate();

         mCameraPivot->AddChild(mCamera);
      }
      else
      {
         LOG_DEBUG("Could not move camera to actor \"" + ACTOR_NAME + "\".");
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

      // Some motion models do not have ground clamping.
      // Use a control variable to determine if the new
      // motion model needs it.
      bool enableGroundClamper = false;

      if (&motionModelType == &MotionModelType::WALK)
      {
         dtCore::Transform xform;
         mCamera->GetTransform(xform);
         dtCore::RefPtr<dtPhysics::Geometry> charShape = dtPhysics::Geometry::CreateCapsuleGeometry(xform, 1.5f, 0.6f, 1.0f);
         charShape->SetMargin(0.3);
         dtCore::RefPtr<dtPhysics::CharacterController> charController = new dtPhysics::CharacterController(*charShape);
         charController->SetStepHeight(0.3f);
         charController->SetSkinWidth(0.04f);
         charController->SetMaxInclineAngle(60.0f);
         charController->Init();

         charController->Warp(xform.GetTranslation());
         dtCore::RefPtr<dtPhysics::CharacterMotionModel> wmm
            = new dtPhysics::CharacterMotionModel(keyboard, mouse, charController, 6.5, 1.5, 2.5, 0.25, true, true);
         wmm->SetScene(scene);
         motionModel = wmm;
         enableGroundClamper = true;
      }
      else if (&motionModelType == &MotionModelType::FLY)
      {
         motionModel = new dtCore::FlyMotionModel(keyboard, mouse);
         enableGroundClamper = true;
      }
      else if (&motionModelType == &MotionModelType::UFO)
      {
         motionModel = new dtCore::UFOMotionModel(keyboard, mouse);
         enableGroundClamper = true;
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
         enableGroundClamper = true;
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
         SetCameraPivot(mAttachActorName);
      }
      else
      {
         if (mGroundClampedXformable.valid())
         {
            mGroundClampedXformable->RemoveChild(mCameraPivot);
            mGroundClampedXformable = NULL;
         }

         osg::Vec3 hpr = xform.GetRotation();
         hpr.y() = 0.0f;
         hpr.z() = 0.0f;
         xform.SetRotation(hpr);

         // The camera may have been detached so set
         // the previous transform so that it does not
         // warp to a place that does not make sense.
         osg::Vec3 offset(0.0f, 0.0f, CAMERA_HEIGHT);
         osg::Vec3 camPos(xform.GetTranslation());
         // --- Move the attach point under the current camera position.
         xform.SetTranslation(camPos - offset);
         mCameraPivot->SetTransform(xform);
         // --- Restore the position and ensure the camera is at
         //     the absolute position at which it left the previous
         //     motion model.
         dtCore::Transform camXform;
         camXform.SetTranslation(offset);
         mCamera->SetTransform(camXform, dtCore::Transformable::REL_CS);
      }
      
      mMotionModel->SetTarget(mCameraPivot.get());

      mClampCameraEnabled = enableGroundClamper;

      SendMotionModelChangedMessage(motionModelType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputComponent::DoGroundClamping(float simTime)
   {
      // Ground clamp dynamically created actors
      dtCore::Transformable* terrain = GetDrawableByName(DEFAULT_TERRAIN_NAME.Get());
      if ( ! mClampCameraEnabled
         || ! mGroundClampedObject.valid() || terrain == NULL)
      {
         // Cannot clamp to nothing.
         return;
      }

      if ( ! mGroundClamper.valid())
      {
         mGroundClamper = new dtGame::DefaultGroundClamper;
         mGroundClamper->SetTerrainActor(terrain);
      }

      dtGame::GroundClampingData gcData;
      gcData.SetAdjustRotationToGround(false);
      gcData.SetUseModelDimensions(false);
      gcData.SetGroundClampType(dtGame::GroundClampTypeEnum::KEEP_ABOVE);

      dtCore::Transform transform;
      mCameraPivot->GetTransform(transform, dtCore::Transformable::ABS_CS);

      // Add this actor to the ground clamp batch
      mGroundClamper->ClampToGround(dtGame::BaseGroundClamper::GroundClampRangeType::RANGED,
         simTime, transform, *mGroundClampedObject, gcData, true);

      // Run the batch ground clamp
      mGroundClamper->FinishUp();
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::IncrementTime(float numSeconds)
   {
      dtRender::SceneManager* sm = dynamic_cast<dtRender::SceneManager*>(GetGameManager()->GetEnvironmentActor()->GetDrawable());
      
      if(sm != NULL)
      {

         dtRender::EphemerisScene* eph = dynamic_cast<dtRender::EphemerisScene*>(sm->FindSceneByType(*dtRender::EphemerisScene::EPHEMERIS_SCENE));
         if(eph != NULL)
         {
            dtUtil::DateTime dt = eph->GetDateTime();
            dt.IncrementClock(numSeconds);
            eph->SetDateTime(dt);
         }
      }
         
   }


} // END - namespace dtExample
