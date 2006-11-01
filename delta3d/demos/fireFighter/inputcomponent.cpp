/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#include <fireFighter/inputcomponent.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/messages.h>
#include <fireFighter/gamestate.h>
#include <fireFighter/playeractor.h>
#include <fireFighter/flysequenceactor.h>
#include <fireFighter/ddgactor.h>
#include <fireFighter/gamelevelactor.h>
#include <fireFighter/gameitemactor.h>
#include <fireFighter/firesuitactor.h>
#include <fireFighter/firehoseactor.h>
#include <fireFighter/scbaactor.h>
#include <fireFighter/hatchactor.h>
#include <dtABC/application.h>
#include <dtAudio/audiomanager.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/infinitelight.h>
#include <dtCore/keyboard.h>
#include <dtGame/basemessages.h>
#include <dtUtil/log.h>
#include <osg/io_utils>
#include <osg/Light>
#include <osg/LightSource>

using dtCore::RefPtr;

const std::string &InputComponent::NAME = "InputComponent";

InputComponent::InputComponent(const std::string &name) : 
   dtGame::BaseInputComponent(name), 
   mCurrentState(&GameState::STATE_UNKNOWN), 
   mPlayer(NULL),
   mMotionModel(NULL), 
   mBellSound(NULL), 
   mDebriefSound(NULL),
   mWalkSound(NULL), 
   mRunSound(NULL), 
   mCrouchSound(NULL),
   mCurrentIntersectedItem(NULL)
{

}

InputComponent::~InputComponent()
{
   if(mBellSound != NULL)
      dtAudio::AudioManager::GetInstance().FreeSound(mBellSound);
   if(mDebriefSound != NULL)
      dtAudio::AudioManager::GetInstance().FreeSound(mDebriefSound);
   if(mWalkSound != NULL)
      dtAudio::AudioManager::GetInstance().FreeSound(mWalkSound);
   if(mRunSound != NULL)
      dtAudio::AudioManager::GetInstance().FreeSound(mRunSound);
   if(mCrouchSound != NULL)
      dtAudio::AudioManager::GetInstance().FreeSound(mCrouchSound);
}

void InputComponent::ProcessMessage(const dtGame::Message &message)
{
   if(message.GetMessageType() == MessageType::GAME_STATE_CHANGED)
   {
      mCurrentState = &(static_cast<const GameStateChangedMessage&>(message)).GetNewState();
      if(*mCurrentState == GameState::STATE_MENU)
      {
         OnMenu();
      }
      else if(*mCurrentState == GameState::STATE_INTRO)
      {
         GetGameManager()->ChangeMap("IntroMap");
      }
      else if(*mCurrentState == GameState::STATE_RUNNING)
      {
         GetGameManager()->ChangeMap("GameMap");
      }
      else if(*mCurrentState == GameState::STATE_DEBRIEF)
      {
         OnDebrief();
      }
      else
      {
         LOG_ERROR("Received a state changed message of: " + mCurrentState->GetName());
      }
   }
   else if(message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      // New map was loaded, we now need to find the player actor
      // Also need to emancipate the camera from its soon to be 
      // invalid parent
      GetGameManager()->GetApplication().GetCamera()->SetParent(NULL);
      mPlayer = NULL;
      IsActorInGameMap(mPlayer);

      mPlayer->AddChild(GetGameManager()->GetApplication().GetCamera());

      if(*mCurrentState == GameState::STATE_INTRO)
         OnIntro();
      else if(*mCurrentState == GameState::STATE_RUNNING)
         OnGame();

      GetGameManager()->GetApplication().GetWindow()->ShowCursor(false);
   }
   else if(message.GetMessageType() == MessageType::ITEM_INTERSECTED)
   {
      if(!message.GetAboutActorId().ToString().empty())
         mCurrentIntersectedItem = dynamic_cast<GameItemActor*>(GetGameManager()->FindGameActorById(message.GetAboutActorId())->GetActor());
      else
         mCurrentIntersectedItem = NULL;
   }
}

void InputComponent::OnIntro()
{
   mMotionModel->SetTarget(NULL);

   std::vector<RefPtr<dtGame::GameActorProxy> > proxies;
   GetGameManager()->GetAllGameActors(proxies);
   FlySequenceActor *fsa = NULL;
   IsActorInGameMap(fsa, false);
   if(fsa == NULL)
   {
      LOG_ALWAYS("Failed to find the fly sequence actor in the intro map. Skipping intro");
      SendGameStateChangedMessage(GameState::STATE_INTRO, GameState::STATE_RUNNING);
   }

   fsa->SetPlayerActor(*mPlayer);
   fsa->StartFlying();
}

void InputComponent::OnGame()
{  
   dtCore::Scene &scene = GetGameManager()->GetScene();
   dtCore::Camera &camera = *GetGameManager()->GetApplication().GetCamera();
   
   mMotionModel->SetTarget(mPlayer);

   GameLevelActor *gla = NULL;
   IsActorInGameMap(gla);

   // Turn off the scene light and use the light maps/shadows maps
   camera.GetSceneHandler()->GetSceneView()->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);
   osg::StateSet *globalState = camera.GetSceneHandler()->GetSceneView()->getGlobalStateSet();
   globalState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   
   mBellSound = dtAudio::AudioManager::GetInstance().NewSound();
   mBellSound->LoadFile("Sounds/bellAndAnnouncement.wav");
   //mBellSound->Play();

   mWalkSound = dtAudio::AudioManager::GetInstance().NewSound();
   mWalkSound->LoadFile("Sounds/walk.wav");

   mRunSound = dtAudio::AudioManager::GetInstance().NewSound();
   mRunSound->LoadFile("Sounds/running.wav");

   mCrouchSound = dtAudio::AudioManager::GetInstance().NewSound();
   mCrouchSound->LoadFile("Sounds/walkingCrouched.wav");
}

void InputComponent::OnDebrief()
{
   StopSounds();
   mMotionModel->SetTarget(NULL);

   mDebriefSound = dtAudio::AudioManager::GetInstance().NewSound();
   mDebriefSound->LoadFile("Sounds/anchorsAweigh.wav");
   mDebriefSound->Play();
}

void InputComponent::OnMenu()
{
   StopSounds();
   mMotionModel->SetTarget(NULL);
}

bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, 
                                      Producer::KeyboardKey key, 
                                      Producer::KeyCharacter character)
{
   bool handled = true;
   bool isGameRunning = (*mCurrentState == GameState::STATE_RUNNING);

   switch(key)
   {
      case Producer::Key_W:
      case Producer::Key_A:
      case Producer::Key_S:
      case Producer::Key_D:
      {
         if(isGameRunning)
         {
            // We are crouched
            if(mPlayer->IsCrouched())
            {
               mMotionModel->SetMaximumFlySpeed(0.5f);
               mCrouchSound->Play();
            }
            else
            {
               dtABC::Application& app = GetGameManager()->GetApplication();
               // Run 
               if(app.GetKeyboard()->GetKeyState(Producer::Key_Shift_L))
               {
                  mMotionModel->SetMaximumFlySpeed(6.0f);
                  mRunSound->Play();
               }   
               // Walk
               else
               {
                  mMotionModel->SetMaximumFlySpeed(2.0f);
                  mWalkSound->Play();
               }
            }
         }
      }
      break;
      
      case Producer::Key_F:
      {
         if(isGameRunning)
         {
            if(mCurrentIntersectedItem != NULL)
            {
               // If it is collectable, pick it up
               // Else, activate it
               if(mCurrentIntersectedItem->IsCollectable())
               {
                  mPlayer->AddItemToInventory(*mCurrentIntersectedItem);
               }   
               else
               {
                  // Special case. The hatch actor can be opened or closed
                  if(dynamic_cast<HatchActor*>(mCurrentIntersectedItem) != NULL)
                     mCurrentIntersectedItem->Activate(!mCurrentIntersectedItem->IsActivated());
                  else
                     mCurrentIntersectedItem->Activate(true);
               }
            }
         }
      }
      break;

      case Producer::Key_M:
      {
         if(isGameRunning)
            SendGameStateChangedMessage(*mCurrentState, GameState::STATE_DEBRIEF);
      }
      break;

      case Producer::Key_C:
      {
         if(isGameRunning)
            mPlayer->SetIsCrouched(!mPlayer->IsCrouched());
      }
      break;

      case Producer::Key_bracketleft:
      {
         if(isGameRunning)
            mPlayer->UpdateSelectedItem(true);
      }
      break;

      case Producer::Key_bracketright:
      {
         if(isGameRunning)
            mPlayer->UpdateSelectedItem(false);
      }
      break;
      
      case Producer::Key_8:
      {
         dtCore::Transform playerXform;
         mPlayer->GetTransform(playerXform);
         std::cout << "Player pos is: " << playerXform.GetTranslation() << '\n';
      }
      break;

      case Producer::Key_9:
      {
         dtCore::Transform playerXform;
         mPlayer->GetTransform(playerXform);
         osg::Vec3 hpr;
         playerXform.GetRotation(hpr);
         std::cout << "Player rotation is: " << hpr << '\n';
      }
      break;

      case Producer::Key_N:
      {
         if(*mCurrentState == GameState::STATE_INTRO)
         {
            SendGameStateChangedMessage(GameState::STATE_INTRO, GameState::STATE_RUNNING);
         }
         else
         {
            handled = false;
         }
      }
      break;

      case Producer::Key_Escape:
      {
         if(isGameRunning)
         {
            SendGameStateChangedMessage(GameState::STATE_RUNNING, GameState::STATE_MENU);
         }
         else
         {
            handled = false;
         }
      }
      break;

      default:
      {
         handled = false;
      }
      break;
   }

   return handled;
}

bool InputComponent::HandleKeyReleased(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   bool handled = true;

   switch(key)
   {
      case Producer::Key_W:
      case Producer::Key_A:
      case Producer::Key_S:
      case Producer::Key_D:
      {
         if(*mCurrentState == GameState::STATE_RUNNING)
         {
            if(mCrouchSound != NULL && mCrouchSound->IsPlaying())
               mCrouchSound->Stop();
            if(mWalkSound != NULL && mWalkSound->IsPlaying())
               mWalkSound->Stop();
            if(mRunSound != NULL && mRunSound->IsPlaying())
               mRunSound->Stop();
         }
         default:
            handled = false;
         break;
      }
      break;
   }

   return handled;
}

bool InputComponent::HandleButtonPressed(const dtCore::Mouse* mouse, 
                                         dtCore::Mouse::MouseButton button)
{
   bool handled = true;
   switch(button)
   {
      case dtCore::Mouse::LeftButton:
      {
         if(*mCurrentState == GameState::STATE_RUNNING)
         {
            if(mPlayer->GetCurrentItem() != NULL)
            {
               // Unlike the other game items, the fire hose only stays activated
               // while the mouse key is down. The other items stay activated until
               // the mouse is clicked again.
               if(dynamic_cast<FireHoseActor*>(mPlayer->GetCurrentItem()) != NULL)
                  mPlayer->UseSelectedItem(true);
               else
               {
                  mPlayer->UseSelectedItem(!mPlayer->GetCurrentItem()->IsActivated());
               }
            }
         } 
      }
      break;

      default:
         handled = false;
      break;
   }
   return handled;
}

bool InputComponent::HandleButtonReleased(const dtCore::Mouse* mouse, 
                                         dtCore::Mouse::MouseButton button)
{
   bool handled = true;
   switch(button)
   {
      case dtCore::Mouse::LeftButton:
      {
         if(*mCurrentState == GameState::STATE_RUNNING)
         {
            // Unlike the other game items, the fire hose only stays activated
            // while the mouse key is down. The other items stay activated until
            // the mouse is clicked again.
            if(mPlayer->GetCurrentItem() != NULL)
            {
               if(dynamic_cast<FireHoseActor*>(mPlayer->GetCurrentItem()) != NULL)
                  mPlayer->UseSelectedItem(false);
            }
         }
      }
      break;

      default:
         handled = false;
      break;
   }
   return handled;
}

void InputComponent::OnAddedToGM()
{
//#ifdef _DEBUG
   //mMotionModel = new dtCore::CollisionMotionModel(1.0f, 0.4f, 0.25f, 0.1f, 
   //   GetGameManager()->GetScene().GetSpaceID(), osg::Vec3(), //GetGameManager()->GetScene().GetGravity(), 
   //   GetGameManager()->GetApplication().GetKeyboard(), 
   //   GetGameManager()->GetApplication().GetMouse());
   mMotionModel = new dtCore::FlyMotionModel(GetGameManager()->GetApplication().GetKeyboard(), 
                                             GetGameManager()->GetApplication().GetMouse());
   mMotionModel->SetMaximumFlySpeed(2);
//#else
//   mMotionModel = new dtCore::FPSMotionModel(GetGameManager()->GetApplication().GetKeyboard(), 
//                                             GetGameManager()->GetApplication().GetMouse());
//#endif
   
   dtGame::BaseInputComponent::OnAddedToGM();
}

void InputComponent::SendGameStateChangedMessage(GameState &oldState, GameState &newState)
{
   RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(MessageType::GAME_STATE_CHANGED);
   GameStateChangedMessage &gscm = static_cast<GameStateChangedMessage&>(*msg);
   gscm.SetOldState(oldState);
   gscm.SetNewState(newState);
   LOG_ALWAYS("Changing game state to: " + newState.GetName());
   GetGameManager()->SendMessage(gscm);
}

void InputComponent::StopSounds()
{
   if(mBellSound != NULL && mBellSound->IsPlaying())
      mBellSound->Stop();
   
   if(mDebriefSound != NULL && mDebriefSound->IsPlaying())
      mDebriefSound->Stop();
   
   if(mWalkSound != NULL && mWalkSound->IsPlaying())
      mWalkSound->Stop();
   
   if(mRunSound != NULL && mRunSound->IsPlaying())
      mRunSound->Stop();
   
   if(mCrouchSound != NULL && mCrouchSound->IsPlaying())
      mCrouchSound->Stop();

   if(mPlayer != NULL)
      mPlayer->StopAllSounds();
}