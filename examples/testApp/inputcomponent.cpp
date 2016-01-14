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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "inputcomponent.h"
#include "lightactorcomponent.h"
#include "testappmessages.h"
#include "testappmessagetypes.h"

#include <dtUtil/mathdefines.h>

#include <dtABC/application.h>
#include <dtABC/trigger.h>
#include <dtABC/beziercontroller.h>
#include <dtCore/camera.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/rtsmotionmodel.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/ufomotionmodel.h>

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
#include <dtGame/gamemanager.inl>
#include <dtCore/gameevent.h>

#include <dtRender/scenemanager.h>
#include <dtRender/ephemerisscene.h>

#if defined(USE_INSPECTOR)
   #include <dtInspectorQt/inspectorqt.h>
#endif

#include <dtActors/engineactorregistry.h>
#include <dtActors/beziercontrolleractor.h>
#include <iostream>

#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtAI/aiplugininterface.h>


namespace dtExample
{
   ////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString InputComponent::DEFAULT_ATTACH_ACTOR_NAME("Gina");
   const dtUtil::RefString InputComponent::DEFAULT_PLAYER_START_NAME("PlayerStart");
   const dtUtil::RefString InputComponent::DEFAULT_TERRAIN_NAME("Terrain");

   static const float CAMERA_HEIGHT = 0.0f;



   ////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////
   InputComponent::InputComponent()
      : BaseClass()
      , mTimeOffset(0.0f)
      , mLampIntensity(1.0f)
      , mClampCameraEnabled(false)
      , mMotionModelsEnabled(false)
      , mMotionModelSpeed(5.0f)
      , mMotionModelMode(&dtExample::MotionModelType::NONE)
      , mMotionModelMode_Previous(&dtExample::MotionModelType::NONE)
      , mMotionModel(NULL)
      , mCamera(NULL)
      , mCameraPivot(NULL)
      , mGroundClampedXformable(NULL)
      , mGroundClampedObject(NULL)
      , mGroundClamper(NULL)
   {
#if defined(USE_INSPECTOR)
      int argc = 0;
      mInspector = new dtInspectorQt::InspectorQt(argc, NULL);
      mInspector->SetVisible(false);
#endif
   }

   ////////////////////////////////////////////////////////////////////////
   InputComponent::~InputComponent()
   {}

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::SetMotionModelEnabled(bool enabled)
   {
      // Only change the state of the current motion model
      // only if it exists and is allowed to be turned on.
      if (mMotionModelsEnabled && mMotionModel)
      {
         mMotionModel->SetEnabled(enabled);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputComponent::IsMotionModelEnabled() const
   {
      return mMotionModel.valid() && mMotionModel->IsEnabled();
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputComponent::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
   {
      if (mMotionModelMode == &MotionModelType::ORBIT)
      {
         dtCore::OrbitMotionModel* omm = dynamic_cast<dtCore::OrbitMotionModel*>(mMotionModel.get());
         if (omm != nullptr)
         {
            float change = float(delta);
            dtUtil::Clamp(change, -0.5f, 0.5f);

            float distance = omm->GetDistance();
            distance += change;
            dtUtil::Clamp(distance, 1.0f, 1000.0f);

            omm->SetDistance(distance);
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
   {
      std::ostringstream ss;

      bool handled = HandleMotionModelKey(key);

      if ( ! handled)
      {
         switch (key)
         {
            // --- Menu Navigation Keys Section --- //
         case osgGA::GUIEventAdapter::KEY_BackSpace:
               SendTransitionMessage(dtExample::Transition::TRANSITION_BACK);
               break;
               
            case osgGA::GUIEventAdapter::KEY_Return:
               SendTransitionMessage(dtExample::Transition::TRANSITION_FORWARD);
               break;
               
            case osgGA::GUIEventAdapter::KEY_F1:
            case 'h':
               SendUIToggleMessage(dtExample::UINames::UI_HELP);
               break;
               
            case osgGA::GUIEventAdapter::KEY_Page_Up :
               IncrementTime(250.0f);
               break;

            case osgGA::GUIEventAdapter::KEY_Page_Down :
               IncrementTime(-250.0f);
               break;

            case 'c':
               SendUIToggleMessage(dtExample::UINames::UI_CONTROL_PANEL);
               break;

            // --- Control Adjustment Section --- //
            case '0':
            {
               dtRender::SceneManager* sm = GetSceneManager();

               if(sm != NULL)
               {

                  dtRender::EphemerisScene* eph = dynamic_cast<dtRender::EphemerisScene*>(sm->FindSceneByType(*dtRender::EphemerisScene::EPHEMERIS_SCENE));
                  if(eph != NULL)
                  {
                     eph->SetTimeFromSystem();
                     
                     dtUtil::DateTime dt;
                     dt.SetToLocalTime();
                     dt.SetMonth(10);
                     dt.SetDay(8);
                     eph->SetDateTime(dt);
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
               IncrementMotionModelSpeed(-2.0f);
            }
            break;

            case osgGA::GUIEventAdapter::KEY_KP_Add:
            case '=':
            case '+':
            {
               IncrementMotionModelSpeed(2.0f);
            }
            break;

            case 'p':
            {
               ReloadShaders();
            }
            break;
            case 'P':
            {
               TogglePhysicsDrawMode();
            }
            break;
   #if defined(USE_INSPECTOR)
            case '`':
            {
               mInspector->SetVisible(true);
               return true;
            }
   #endif

            case 'n':
            {
               SetLampIntensity(mLampIntensity - 0.1f);
            }
            break;
            case 'm':
            {
               SetLampIntensity(mLampIntensity + 0.1f);
            }
            break;

            default:
            {
               handled = false;
            }
            break;
         };
      }

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
   bool InputComponent::HandleMotionModelKey(int key)
   {
      bool handled = false;

      if (mMotionModelsEnabled)
      {
         switch (key)
         {
         // --- Motion Model Section --- //
         case '1':
            SetMotionModel(MotionModelType::WALK);
            handled = true;
            break;
         case '2':
            SetMotionModel(MotionModelType::FLY);
            handled = true;
            break;
         case '3':
            SetMotionModel(MotionModelType::UFO);
            handled = true;
            break;
         case '4':
            SetMotionModel(MotionModelType::ORBIT);
            handled = true;
            break;
         case '5':
            SetMotionModel(MotionModelType::FPS);
            handled = true;
            break;
         case '6':
            SetMotionModel(MotionModelType::RTS);
            handled = true;
            break;
         default:
            break;
         }
      }

      return handled;
   }

   ////////////////////////////////////////////////////////////////////////
   void InputComponent::HandleGameStateChange(dtGame::GameState::Type& newState)
   {
      bool isRunningState = &newState == &TestAppGameState::STATE_GAME;

      if (isRunningState != mMotionModelsEnabled)
      {
         // Enable motion models only for a paused/non-action state.
         mMotionModelsEnabled = isRunningState;

         const MotionModelType* newMode = &dtExample::MotionModelType::NONE;

         if (mMotionModelsEnabled)
         {
            newMode = mMotionModelMode_Previous;

            if (newMode == &MotionModelType::NONE)
            {
               newMode = &MotionModelType::WALK;
            }
         }
         else
         {
            mMotionModelMode_Previous = mMotionModelMode;
         }

         SetMotionModel(*newMode);
      }
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
      else if (type == dtGame::MessageType::INFO_GAME_STATE_CHANGED)
      {
         const dtGame::GameStateChangedMessage& gscm
            = static_cast<const dtGame::GameStateChangedMessage&>(message);

         HandleGameStateChange(gscm.GetNewState());
      }
      else if (type == dtExample::TestAppMessageType::REQUEST_TIME_OFFSET)
      {
         const dtExample::RequestTimeOffsetMessage& rtom
            = static_cast<const dtExample::RequestTimeOffsetMessage&>(message);

         // Offsets will be treated in terms of hours.
         float newOffset = rtom.GetOffset();
         float difference = newOffset - mTimeOffset;
         mTimeOffset = newOffset;

         float seconds = difference * 3600.0f;
         IncrementTime(seconds);
      }
      else if (type == dtExample::TestAppMessageType::REQUEST_ATTACH)
      {
         const dtExample::RequestAttachMessage& ram
            = static_cast<const dtExample::RequestAttachMessage&>(message);

         mAttachActorId = ram.GetActorId();

         // Attch to the actor if the name is valid.
         if ( ! mAttachActorId.ToString().empty())
         {
            DetachFromController();

            if(!AttachToBezierController(mAttachActorId))
            {
               // Determine if the current motion model is proper.
               if (mMotionModelMode != &MotionModelType::ORBIT)
               {
                  SetMotionModel(MotionModelType::ORBIT);
               }
               else // Already using a proper motion model for attachment.
               {
                  SetCameraPivotById(mAttachActorId);
               }
            }
         }
      }
      else if (type == dtGame::MessageType::INFO_MAP_LOADED)
      {
         SetCameraToPlayerStart();
         AttachBanner();
         SetLampIntensity(0.0f);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::OnAddedToGM()
   {
      BaseClass::OnAddedToGM();

      // TODO: Something for custom initialization?
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
         LOG_DEBUG("Could not find actor with name \"" + name + "\".");
      }

      return actor;
   }

   ////////////////////////////////////////////////////////////////////////
   dtCore::TransformableActorProxy* InputComponent::GetActorById(const dtCore::UniqueId& id)
   {
      dtCore::TransformableActorProxy* actor = NULL;

      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorById(id, actor);

      if (actor == NULL)
      {
         LOG_DEBUG("Could not find actor with id \"" + id.ToString() + "\".");
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
         LOG_DEBUG("Could not find drawable with name \"" + name + "\".");
      }

      return xformable;
   }

   ////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* InputComponent::GetDrawableById(const dtCore::UniqueId& id)
   {
      dtCore::Transformable* xformable = NULL;
      dtCore::TransformableActorProxy* actor = GetActorById(id);

      if (actor != NULL)
      {
         actor->GetDrawable(xformable);
      }

      if (xformable == NULL)
      {
         LOG_DEBUG("Could not find drawable with id \"" + id.ToString() + "\".");
      }

      return xformable;
   }

   ////////////////////////////////////////////////////////////////////////
   bool InputComponent::SetCameraPivotByName(const std::string& actorName)
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

      dtCore::Transformable* drawable = GetDrawableByName(actorName);

      return SetCameraPivot(drawable);
   }

   ////////////////////////////////////////////////////////////////////////
   bool InputComponent::SetCameraPivotById(const dtCore::UniqueId& id)
   {
      // Detach from the current actor.
      if (mGroundClampedXformable.valid())
      {
         // If this is the same actor, exit early.
         if (mGroundClampedXformable->GetUniqueId() == id)
         {
            // Return true because the camera pivot should be attached.
            return true;
         }

         mGroundClampedXformable->RemoveChild(mCameraPivot);
      }

      dtCore::Transformable* drawable = GetDrawableById(id);

      return SetCameraPivot(drawable);
   }

   ////////////////////////////////////////////////////////////////////////
   bool InputComponent::SetCameraPivot(dtCore::Transformable* drawable)
   {
      mGroundClampedXformable = drawable;

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

      DetachFromController();

      // Prevent changing the motion model if it exists
      // and is the same type that is specified.
      if ( ! mCamera.valid() || (mMotionModel.valid() && mMotionModelMode == &motionModelType))
      {
         return;
      }


      // If the type is set to NONE, just disable the current
      // motion model and ground clamping, then escape.
      if (&motionModelType == &dtExample::MotionModelType::NONE)
      {
         mMotionModel = NULL;
         mClampCameraEnabled = false;
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
         dtCore::RefPtr<dtPhysics::Geometry> charShape = dtPhysics::Geometry::CreateCapsuleGeometry(xform, 1.0f, 0.4f, 1.0f);
         charShape->SetMargin(0.3);
         dtCore::RefPtr<dtPhysics::CharacterController> charController = new dtPhysics::CharacterController(*charShape);
         charController->SetStepHeight(0.2f);
         charController->SetSkinWidth(0.04f);
         charController->SetMaxInclineAngle(60.0f);
         charController->Init();

         charController->Warp(xform.GetTranslation());
         dtCore::RefPtr<dtPhysics::CharacterMotionModel> wmm
            = new dtPhysics::CharacterMotionModel(keyboard, mouse, charController, 6.5, 1.5, 0.75, 0.9f, 0.3f, true, true);
         wmm->SetScene(scene);
         wmm->SetUseMouseButtons(true);

         motionModel = wmm;
         enableGroundClamper = true;
      }
      else if (&motionModelType == &MotionModelType::FLY)
      {
         dtCore::RefPtr<dtCore::FlyMotionModel> flyMotionModel = new dtCore::FlyMotionModel(keyboard, mouse);
         motionModel = flyMotionModel;
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
         SetCameraPivotById(mAttachActorId);
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

      SetMotionModelSpeed(*mMotionModelMode, *mMotionModel, mMotionModelSpeed);

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
      dtRender::SceneManager* sm = GetSceneManager();
      
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

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::IncrementMotionModelSpeed(float increment)
   {
      if (mMotionModel.valid())
      {
         mMotionModelSpeed += increment;

         dtUtil::Clamp(mMotionModelSpeed, 0.1f, 100.0f);

         SetMotionModelSpeed(*mMotionModelMode, *mMotionModel, mMotionModelSpeed);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputComponent::SetMotionModelSpeed(const dtExample::MotionModelType& motionModelType,
      dtCore::MotionModel& motionModel, float speed)
   {
      bool success = false;

      if (&motionModelType == &MotionModelType::WALK)
      {
         dtPhysics::CharacterMotionModel* walk
            = dynamic_cast<dtPhysics::CharacterMotionModel*>(&motionModel);
         
         if (walk != NULL)
         {
            walk->SetMaximumWalkSpeed(speed);
            walk->SetMaximumSidestepSpeed(speed);
            success = true;
         }
      }
      else if (&motionModelType == &MotionModelType::FLY)
      {
         dtCore::FlyMotionModel* fly
            = dynamic_cast<dtCore::FlyMotionModel*>(&motionModel);
         
         if (fly != NULL)
         {
            fly->SetMaximumFlySpeed(speed);
            success = true;
         }
      }
      else if (&motionModelType == &MotionModelType::UFO)
      {
         dtCore::UFOMotionModel* ufo
            = dynamic_cast<dtCore::UFOMotionModel*>(&motionModel);
         
         if (ufo != NULL)
         {
            ufo->SetMaximumFlySpeed(speed);
            success = true;
         }
      }
      else if (&motionModelType == &MotionModelType::ORBIT)
      {
         dtCore::OrbitMotionModel* orbit
            = dynamic_cast<dtCore::OrbitMotionModel*>(&motionModel);
       
         if (orbit != NULL)
         {
            // TODO:
            success = true;
         }
      }
      else if (&motionModelType == &MotionModelType::FPS)
      {
         dtCore::FPSMotionModel* fps
            = dynamic_cast<dtCore::FPSMotionModel*>(&motionModel);

         if (fps != NULL)
         {
            fps->SetMaximumWalkSpeed(speed);
            fps->SetMaximumSidestepSpeed(speed);
            success = true;
         }
      }
      else if (&motionModelType == &MotionModelType::RTS)
      {
         dtCore::RTSMotionModel* rts
            = dynamic_cast<dtCore::RTSMotionModel*>(&motionModel);
         
         if (rts != NULL)
         {
            rts->SetLinearRate(speed);
            success = true;
         }
      }

      if ( ! success)
      {
         LOG_ERROR("Cannot cast motion model to type: " + motionModelType.GetName());
      }
   }

   void InputComponent::SetLampIntensity(float intensity)
   {
      if (intensity > 1.0f)
         intensity = 1.0f;
      if (intensity < 0.0f)
         intensity = 0.0f;

      mLampIntensity = intensity;

      typedef dtCore::ActorPtrVector ActorList;
      ActorList actors;

      dtGame::GameManager* gm = GetGameManager();
      gm->ForEachActor([intensity](dtCore::BaseActorObject& baseActor)
            {
               baseActor.IsGameActor();
               dtGame::GameActorProxy& actor = static_cast<dtGame::GameActorProxy&>(baseActor);

               LightActorComponent* lac = nullptr;
               actor.GetComponent(lac);
               if (lac != nullptr)
               {
                  lac->SetLightIntensity(intensity);
               }
            }, true);
   }

   void InputComponent::ReloadShaders()
   {
      dtCore::ShaderManager::GetInstance().ReloadAndReassignShaderDefinitions();
   }

   void InputComponent::ToggleAIWaypointDrawMode()
   {
      dtAI::AIInterfaceActor* interfaceActor = NULL;
      GetGameManager()->FindActorByType(*dtAI::AIActorRegistry::AI_INTERFACE_ACTOR_TYPE, interfaceActor);
      if (interfaceActor != NULL)
      {
         dtAI::AIDebugDrawable* aidd = NULL;
         interfaceActor->GetDrawable(aidd);
         if (aidd != NULL)
         {
            dtAI::WaypointRenderInfo* renderInfo = aidd->GetRenderInfo();
            renderInfo->SetEnableDepthTest(true);
            renderInfo->SetAllRenderingOptions(!renderInfo->IsAnyRenderingEnabled());
            aidd->OnRenderInfoChanged();
            if (!aidd->HasWaypointData())
            {
               interfaceActor->GetAIInterface()->SetDebugDrawable(NULL);
               interfaceActor->GetAIInterface()->SetDebugDrawable(aidd);
            }
         }
      }
   }

   void InputComponent::TogglePhysicsDrawMode()
   {
      dtPhysics::PhysicsComponent* physComp = NULL;
      GetGameManager()->GetComponentByName(dtPhysics::PhysicsComponent::DEFAULT_NAME, physComp);
      if (physComp != NULL)
      {
         physComp->SetNextDebugDrawMode();
      }
   }

   void InputComponent::SetSeaState(dtActors::WaterGridActor::SeaState& seaState)
   {
      dtActors::WaterGridActorProxy* waterActor =NULL;
      GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::WATER_GRID_ACTOR_TYPE, waterActor);
      if(waterActor != NULL)
      {
         dtActors::WaterGridActor* water = NULL;
         waterActor->GetDrawable(water);

         water->SetSeaState(seaState);
      }
   }

   void InputComponent::SetWaterChop(dtActors::WaterGridActor::ChopSettings& chop)
   {
      dtActors::WaterGridActorProxy* waterActor =NULL;
      GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::WATER_GRID_ACTOR_TYPE, waterActor);
      if(waterActor != NULL)
      {
         dtActors::WaterGridActor* water = NULL;
         waterActor->GetDrawable(water);

         water->SetChop(chop);
      }
   }


   dtActors::WaterGridActor::SeaState& InputComponent::GetSeaState() const
   {
      dtActors::WaterGridActorProxy* waterActor =NULL;
      GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::WATER_GRID_ACTOR_TYPE, waterActor);
      if(waterActor != NULL)
      {
         dtActors::WaterGridActor* water = NULL;
         waterActor->GetDrawable(water);

         return water->GetSeaState();
      }

      return dtActors::WaterGridActor::SeaState::SeaState_0;
   }

   dtActors::WaterGridActor::ChopSettings& InputComponent::GetWaterChop() const
   {
      dtActors::WaterGridActorProxy* waterActor =NULL;
      GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::WATER_GRID_ACTOR_TYPE, waterActor);
      if(waterActor != NULL)
      {
         dtActors::WaterGridActor* water = NULL;
         waterActor->GetDrawable(water);

         return water->GetChop();
      }

      return dtActors::WaterGridActor::ChopSettings::CHOP_FLAT;

   }



   std::vector<dtCore::RefPtr<dtABC::Trigger> > triggers;

   void InputComponent::ToggleFireworks()
   {
      triggers.clear();

      dtActors::BezierControllerActor* bactor = NULL;
      GetGameManager()->FindActorByName("FireworkA_Controller", bactor);
      if (bactor != NULL)
      {
         dtABC::BezierController* controller = NULL;
         bactor->GetDrawable(controller);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller);
         t->SetTimeDelay( dtUtil::RandFloat(0.0f, 2.0f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller);
         t2->SetTimeDelay( dtUtil::RandFloat(10.0f, 12.0f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);

         dtABC::Trigger* t3 = new dtABC::Trigger();
         t3->SetAction(controller);
         t3->SetTimeDelay( dtUtil::RandFloat(28.0f, 30.0f));
         t3->SetEnabled(true);
         t3->Fire();
         triggers.push_back(t3);

      }

      dtActors::BezierControllerActor* bactor2 = NULL;
      GetGameManager()->FindActorByName("FireworkB_Controller", bactor2);
      if (bactor2 != NULL)
      {
         dtABC::BezierController* controller2 = NULL;
         bactor2->GetDrawable(controller2);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller2);
         t->SetTimeDelay( dtUtil::RandFloat(0.5f, 2.0f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller2);
         t2->SetTimeDelay( dtUtil::RandFloat(10.5f, 12.0f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);
      }

      dtActors::BezierControllerActor* bactor3 = NULL;
      GetGameManager()->FindActorByName("FireworkC_Controller", bactor3);
      if (bactor3 != NULL)
      {
         dtABC::BezierController* controller3 = NULL;
         bactor3->GetDrawable(controller3);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller3);
         t->SetTimeDelay( dtUtil::RandFloat(0.5f, 2.5f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller3);
         t2->SetTimeDelay( dtUtil::RandFloat(12.5f, 15.5f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);

         dtABC::Trigger* t3 = new dtABC::Trigger();
         t3->SetAction(controller3);
         t3->SetTimeDelay( dtUtil::RandFloat(28.5f, 30.5f));
         t3->SetEnabled(true);
         t3->Fire();
         triggers.push_back(t3);
      }

      dtActors::BezierControllerActor* bactor4 = NULL;
      GetGameManager()->FindActorByName("FireworkD_Controller", bactor4);
      if (bactor4 != NULL)
      {
         dtABC::BezierController* controller4 = NULL;
         bactor4->GetDrawable(controller4);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller4);
         t->SetTimeDelay( dtUtil::RandFloat(1.5f, 3.0f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller4);
         t2->SetTimeDelay( dtUtil::RandFloat(13.5f, 16.0f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);
      }

      dtActors::BezierControllerActor* bactor5 = NULL;
      GetGameManager()->FindActorByName("FireworkF_Controller", bactor5);
      if (bactor5 != NULL)
      {
         dtABC::BezierController* controller5 = NULL;
         bactor5->GetDrawable(controller5);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller5);
         t->SetTimeDelay( dtUtil::RandFloat(1.5f, 3.5f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller5);
         t2->SetTimeDelay( dtUtil::RandFloat(15.5f, 23.5f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);

         dtABC::Trigger* t3 = new dtABC::Trigger();
         t3->SetAction(controller5);
         t3->SetTimeDelay( dtUtil::RandFloat(28.5f, 30.5f));
         t3->SetEnabled(true);
         t3->Fire();
         triggers.push_back(t3);

      }

      dtActors::BezierControllerActor* bactor6 = NULL;
      GetGameManager()->FindActorByName("FireworkG_Controller", bactor6);
      if (bactor6 != NULL)
      {
         dtABC::BezierController* controller6 = NULL;
         bactor6->GetDrawable(controller6);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller6);
         t->SetTimeDelay( dtUtil::RandFloat(2.0f, 5.0f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller6);
         t2->SetTimeDelay( dtUtil::RandFloat(16.0f, 25.0f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);
      }

      dtActors::BezierControllerActor* bactor7 = NULL;
      GetGameManager()->FindActorByName("FireworkH_Controller", bactor7);
      if (bactor7 != NULL)
      {
         dtABC::BezierController* controller7 = NULL;
         bactor7->GetDrawable(controller7);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller7);
         t->SetTimeDelay( dtUtil::RandFloat(2.5f, 7.0f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller7);
         t2->SetTimeDelay( dtUtil::RandFloat(17.5f, 27.0f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);
      }

      dtActors::BezierControllerActor* bactor8 = NULL;
      GetGameManager()->FindActorByName("FireworkI_Controller", bactor8);
      if (bactor8 != NULL)
      {
         dtABC::BezierController* controller8 = NULL;
         bactor8->GetDrawable(controller8);

         dtABC::Trigger* t = new dtABC::Trigger();
         t->SetAction(controller8);
         t->SetTimeDelay( dtUtil::RandFloat(3.5f, 8.0f));
         t->SetEnabled(true);
         t->Fire();
         triggers.push_back(t);

         dtABC::Trigger* t2 = new dtABC::Trigger();
         t2->SetAction(controller8);
         t2->SetTimeDelay( dtUtil::RandFloat(23.5f, 28.0f));
         t2->SetEnabled(true);
         t2->Fire();
         triggers.push_back(t2);
      }
   }

   void InputComponent::DetachFromController()
   {
      if (mCurrentController.valid())
      {
         mCurrentController->SetTargetObject(NULL);
         mCurrentController = NULL;
      }
   }

   bool InputComponent::AttachToBezierController( const dtCore::UniqueId& id)
   {
      bool success = false;

      if (mCameraPivot.valid())
      {
         dtActors::BezierControllerActor* actor = NULL;

         dtGame::GameManager* gm = GetGameManager();
         gm->FindActorById(id, actor);


         if(actor != NULL)
         {
            dtABC::BezierController* controller = NULL;
            actor->GetDrawable<dtABC::BezierController*>(controller);

            if(controller != NULL)
            {
               dtCore::Transform defXform;
               mCameraPivot->SetTransform(defXform);
               mCamera->SetTransform(defXform);

               SetMotionModel(MotionModelType::FLY);
               SetMotionModel(MotionModelType::NONE );
               controller->SetTargetObject(mCameraPivot.get());
               controller->Pause();
               controller->Start();

               mCurrentController = controller;
               
               success = true;
            }

         }
      }

      return success;
   }

   void InputComponent::SetAmbience( float amt )
   {
      dtRender::SceneManager* sm = GetSceneManager();

      if(sm != NULL)
      {
         sm->SetAmbience(amt);
      }
   }

   float InputComponent::GetAmbience()
   {
      dtRender::SceneManager* sm = GetSceneManager();

      return sm != NULL ? sm->GetAmbience() : 0.0f;
   }

   void InputComponent::SetLuminance( float amt )
   {
      dtRender::SceneManager* sm = GetSceneManager();

      if(sm != NULL)
      {
         sm->SetLuminance(amt);
      }
   }

   float InputComponent::GetLuminance()
   {
      dtRender::SceneManager* sm = GetSceneManager();

      return sm != NULL ? sm->GetLuminance() : 1.0f;
   }

   dtRender::SceneManager* InputComponent::GetSceneManager()
   {
      dtGame::IEnvGameActorProxy* envActor = GetGameManager()->GetEnvironmentActor();
      if (envActor != NULL)
      {
         return dynamic_cast<dtRender::SceneManager*>(envActor->GetDrawable());
      }
      else
      {
         return NULL;
      }
   }

   void InputComponent::AttachBanner()
   {
      /* NOTE: This method is temporary, due to shortage of time for the release.
      * This method simply attaches one actor to another. Currently delta3d does
      * not give actors the ability to do this directly, so some workaround code
      * may need to be written, such as this or else where in a custom actor component.
      */

      dtGame::GameActorProxy* plane = NULL;
      dtGame::GameActorProxy* banner = NULL;

      dtGame::GameManager* gm = GetGameManager();
      gm->FindActorByName("plane", plane);
      gm->FindActorByName("delta3d_banner", banner);

      if (plane != NULL && banner != NULL)
      {
         dtCore::Transformable* planeDrawable = NULL;
         dtCore::Transformable* bannerDrawable = NULL;

         plane->GetDrawable(planeDrawable);
         banner->GetDrawable(bannerDrawable);

         if (planeDrawable != NULL && bannerDrawable != NULL)
         {
            // This removes the drawable from the scene draw traversal.
            bannerDrawable->Emancipate();

            dtCore::Transform xform;
            planeDrawable->GetTransform(xform);

            // This will add the banner drawable to the plane hierarchy
            // and also add it back to the scene draw traversal.
            planeDrawable->AddChild(bannerDrawable);

            bannerDrawable->SetTransform(xform);
         }
      }
      else
      {
         if (plane == NULL)
         {
            LOG_ERROR("Could not access plane actor.");
         }
         else
         {
            LOG_ERROR("Could not access banner actor.");
         }
      }
   }

} // END - namespace dtExample
