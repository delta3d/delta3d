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
#include <fireFighter/helpwindow.h>
#include <dtABC/application.h>
#include <dtAudio/audiomanager.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/keyboard.h>
#include <dtCore/system.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtCore/actorproperty.h>
#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtActors/taskactorordered.h>
#include <dtActors/taskactorrollup.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/engineactorregistry.h>

#include <osgViewer/View>

#include <iostream>

using dtCore::RefPtr;

const RefPtr<dtCore::SystemComponentType> InputComponent::NAME(new dtCore::SystemComponentType("InputComponent", "GMComponents", "Firefighter input", dtGame::BaseInputComponent::DEFAULT_TYPE));

InputComponent::InputComponent(dtCore::SystemComponentType& type)
   : dtGame::BaseInputComponent(type)
   , mCurrentState(&GameState::STATE_UNKNOWN)
   , mPlayer(NULL)
   , mMotionModel(NULL)
   , mBellSound(NULL)
   , mDebriefSound(NULL)
   , mWalkSound(NULL)
   , mRunSound(NULL)
   , mCrouchSound(NULL)
   , mCurrentIntersectedItem(NULL)
   , mTasksSetup(false)
{

}

InputComponent::~InputComponent()
{
}

void InputComponent::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == FireFighterMessageType::GAME_STATE_CHANGED)
   {
      mCurrentState = &(static_cast<const GameStateChangedMessage&>(message)).GetNewState();
      if (*mCurrentState == GameState::STATE_MENU)
      {
         OnMenu();
      }
      else if (*mCurrentState == GameState::STATE_INTRO)
      {
         GetGameManager()->ChangeMap("IntroMap");
      }
      else if (*mCurrentState == GameState::STATE_RUNNING)
      {
         GetGameManager()->ChangeMap("GameMap");
      }
      else if (*mCurrentState == GameState::STATE_DEBRIEF)
      {
         OnDebrief();
      }
      else
      {
         LOG_ERROR("Received a state changed message of: " + mCurrentState->GetName());
      }
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      // New map was loaded, we now need to find the player actor
      // Also need to emancipate the camera from its soon to be
      // invalid parent
      GetGameManager()->GetApplication().GetCamera()->SetParent(NULL);
      mPlayer = NULL;
      IsActorInGameMap(mPlayer);

      mPlayer->AddChild(GetGameManager()->GetApplication().GetCamera());

      if (*mCurrentState == GameState::STATE_INTRO)
      {
         OnIntro();
      }
      else if (*mCurrentState == GameState::STATE_RUNNING)
      {
         OnGame();
      }
   }
   else if (message.GetMessageType() == FireFighterMessageType::ITEM_INTERSECTED)
   {
      if (!message.GetAboutActorId().ToString().empty())
      {
         mCurrentIntersectedItem = dynamic_cast<GameItemActor*>(GetGameManager()->FindGameActorById(message.GetAboutActorId())->GetDrawable());
      }
      else
      {
         mCurrentIntersectedItem = NULL;
      }
   }
   else if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      if (*mCurrentState == GameState::STATE_RUNNING)
      {
         ProcessTasks();
      }
   }
   else if (message.GetMessageType() == FireFighterMessageType::HELP_WINDOW_OPENED)
   {
      mMotionModel->SetTarget(NULL);
   }
   else if (message.GetMessageType() == FireFighterMessageType::HELP_WINDOW_CLOSED)
   {
      mMotionModel->SetTarget(mPlayer);
   }
}

void InputComponent::OnIntro()
{
   if (mMotionModel != NULL)
   {
      mMotionModel->SetTarget(NULL);
   }

   // Turn off the scene light and use the light maps/shadow maps
   dtCore::Camera& camera = *GetGameManager()->GetApplication().GetCamera();
   GetGameManager()->GetApplication().GetView()->GetOsgViewerView()->setLightingMode(osg::View::NO_LIGHT);
   osg::StateSet* globalState = camera.GetOSGCamera()->getOrCreateStateSet();
   globalState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

   std::vector<dtGame::GameActorProxy*> proxies;
   GetGameManager()->GetAllGameActors(proxies);
   FlySequenceActor* fsa = NULL;
   IsActorInGameMap(fsa, false);
   if (fsa == NULL)
   {
      LOG_ALWAYS("Failed to find the fly sequence actor in the intro map. Skipping intro");
      SendGameStateChangedMessage(GameState::STATE_INTRO, GameState::STATE_RUNNING);
   }

   fsa->SetPlayerActor(*mPlayer);
   fsa->StartFlying();
}

void InputComponent::OnGame()
{
   GetGameManager()->GetApplication().GetMouse()->SetPosition(0.0f, 0.0f);
//   GameLevelActor* gla = NULL;
//   IsActorInGameMap(gla);
//   gla->SetCollisionMesh();

   dtCore::Transform xform;
   mPlayer->GetTransform(xform);

//   if (!mMotionModel.valid())
//   {
//      osg::Vec3 pos;
//      xform.GetTranslation(pos);
//      mMotionModel = new dtCore::CollisionMotionModel(pos.z(),
//         mRadius, mK, mTheta, &GetGameManager()->GetScene(),
//         GetGameManager()->GetApplication().GetKeyboard(),
//         GetGameManager()->GetApplication().GetMouse());
//
//      mMotionModel->SetUseMouseButtons(false);
//      mMotionModel->SetCanJump(false);
//   }
//
//   mMotionModel->SetTarget(mPlayer);
//   mMotionModel->GetFPSCollider().Reset();

   // Turn off the scene light and use the light maps/shadow maps
   dtCore::Camera& camera = *GetGameManager()->GetApplication().GetCamera();
   GetGameManager()->GetApplication().GetView()->GetOsgViewerView()->setLightingMode(osg::View::NO_LIGHT);
   osg::StateSet* globalState = camera.GetOSGCamera()->getOrCreateStateSet();
   globalState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

   SetupTasks();

   if (mBellSound == NULL)
   {
      mBellSound = dtAudio::AudioManager::GetInstance().NewSound();
   }
   mBellSound->LoadFile("Sounds/bellAndAnnouncement.wav");
   mBellSound->Play();

   if (mWalkSound == NULL)
   {
      mWalkSound = dtAudio::AudioManager::GetInstance().NewSound();
   }
   mWalkSound->LoadFile("Sounds/walk.wav");

   if (mRunSound == NULL)
   {
      mRunSound = dtAudio::AudioManager::GetInstance().NewSound();
   }
   mRunSound->LoadFile("Sounds/running.wav");

   if (mCrouchSound == NULL)
   {
      mCrouchSound = dtAudio::AudioManager::GetInstance().NewSound();
   }
   mCrouchSound->LoadFile("Sounds/walkingCrouched.wav");
}

void InputComponent::OnDebrief()
{
   StopSounds();

   if (mMotionModel != NULL)
   {
      mMotionModel->SetTarget(NULL);
   }

   if (mDebriefSound == NULL)
   {
      mDebriefSound = dtAudio::AudioManager::GetInstance().NewSound();
   }
   mDebriefSound->LoadFile("Sounds/anchorsAweigh.wav");
   mDebriefSound->Play();
}

void InputComponent::OnMenu()
{
   StopSounds();
   if (mMotionModel != NULL)
   {
      mMotionModel->SetTarget(NULL);
   }
}

bool InputComponent::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = true;
   bool isGameRunning = (*mCurrentState == GameState::STATE_RUNNING);

   switch (key)
   {
      case 'w':
      case 'a':
      case 's':
      case 'd':
      {
         if (isGameRunning)
         {
            // We are crouched
            if (mPlayer->IsCrouched())
            {
               // Get the crouched height of the player and update the motion model
               dtCore::Transform xform;
               mPlayer->GetTransform(xform);

               mMotionModel->SetMaximumWalkSpeed(0.5f);
               osg::Vec3 pos;
               xform.GetTranslation(pos);
               UpdateCollider(pos.z());
               mCrouchSound->Play();
            }
            else
            {
               dtABC::Application& app = GetGameManager()->GetApplication();
               // Run
               if (app.GetKeyboard()->GetKeyState(osgGA::GUIEventAdapter::KEY_Shift_L))
               {
                  mMotionModel->SetMaximumWalkSpeed(6.0f);
                  mRunSound->Play();
               }
               // Walk
               else
               {
                  dtCore::Transform xform;
                  mPlayer->GetTransform(xform);

                  mMotionModel->SetMaximumWalkSpeed(2.0f);
                  osg::Vec3 pos;
                  xform.GetTranslation(pos);
                  UpdateCollider(pos.z());
                  mWalkSound->Play();
               }
            }
         }
      }
      break;

      case 'f':
      {
         if (isGameRunning)
         {
            if (mCurrentIntersectedItem != NULL)
            {
               // If it is collectable, pick it up
               // Else, activate it
               if (mCurrentIntersectedItem->IsCollectable())
               {
                  mPlayer->AddItemToInventory(*mCurrentIntersectedItem);
               }
               else
               {
                  // Special case. The hatch actor can be opened or closed
                  if (dynamic_cast<HatchActor*>(mCurrentIntersectedItem) != NULL)
                  {
                     mCurrentIntersectedItem->Activate(!mCurrentIntersectedItem->IsActivated());
                  }
                  else
                  {
                     mCurrentIntersectedItem->Activate(true);
                  }
               }
            }
         }
      }
      break;

      case 'm':
      {
         if (isGameRunning)
         {
            SendGameStateChangedMessage(*mCurrentState, GameState::STATE_DEBRIEF);
         }
      }
      break;

      /*case 'c':
      {
         if (isGameRunning)
         {
            mPlayer->SetIsCrouched(!mPlayer->IsCrouched());
         }
      }
      break;*/

      case '[':
      {
         if (isGameRunning)
         {
            mPlayer->UpdateSelectedItem(true);
         }
      }
      break;

      case ']':
      {
         if (isGameRunning)
         {
            mPlayer->UpdateSelectedItem(false);
         }
      }
      break;

      case '8':
      {
         dtCore::Transform playerXform;
         mPlayer->GetTransform(playerXform);
         osg::Vec3 pos;
         playerXform.GetTranslation(pos);
         std::cout << "Player pos is: " << pos << '\n';
      }
      break;

      case '9':
      {
         dtCore::Transform playerXform;
         mPlayer->GetTransform(playerXform);
         osg::Vec3 hpr;
         playerXform.GetRotation(hpr);
         std::cout << "Player rotation is: " << hpr << '\n';
      }
      break;

      case 'n':
      {
         if (*mCurrentState == GameState::STATE_INTRO)
         {
            SendGameStateChangedMessage(GameState::STATE_INTRO, GameState::STATE_RUNNING);
         }
         else
         {
            handled = false;
         }
      }
      break;

      case osgGA::GUIEventAdapter::KEY_F1:
      {
         if (isGameRunning)
         {
            RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::HELP_WINDOW_OPENED);
            GetGameManager()->SendMessage(*msg);
         }
      }
      break;

      case osgGA::GUIEventAdapter::KEY_Escape:
      {
         if (isGameRunning)
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

bool InputComponent::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
{
   bool handled = true;

   switch (key)
   {
      case 'w':
      case 'a':
      case 's':
      case 'd':
      {
         if (*mCurrentState == GameState::STATE_RUNNING)
         {
            if (mCrouchSound != NULL && mCrouchSound->IsPlaying())
            {
               mCrouchSound->Stop();
            }
            if (mWalkSound != NULL && mWalkSound->IsPlaying())
            {
               mWalkSound->Stop();
            }
            if (mRunSound != NULL && mRunSound->IsPlaying())
            {
               mRunSound->Stop();
            }
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
   switch (button)
   {
      case dtCore::Mouse::LeftButton:
      {
         if (*mCurrentState == GameState::STATE_RUNNING)
         {
            if (mPlayer->GetCurrentItem() != NULL)
            {
               // Unlike the other game items, the fire hose only stays activated
               // while the mouse key is down. The other items stay activated until
               // the mouse is clicked again.
               if (dynamic_cast<FireHoseActor*>(mPlayer->GetCurrentItem()) != NULL)
               {
                  mPlayer->UseSelectedItem(true);
               }
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
   switch (button)
   {
      case dtCore::Mouse::LeftButton:
      {
         if (*mCurrentState == GameState::STATE_RUNNING)
         {
            // Unlike the other game items, the fire hose only stays activated
            // while the mouse key is down. The other items stay activated until
            // the mouse is clicked again.
            if (mPlayer->GetCurrentItem() != NULL)
            {
               if (dynamic_cast<FireHoseActor*>(mPlayer->GetCurrentItem()) != NULL)
               {
                  mPlayer->UseSelectedItem(false);
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

void InputComponent::OnAddedToGM()
{
   dtGame::BaseInputComponent::OnAddedToGM();
}

void InputComponent::UpdateCollider(float newHeight)
{
   //mMotionModel->GetFPSCollider().SetDimensions(newHeight, mRadius, mK, mTheta);
}

void InputComponent::SendGameStateChangedMessage(GameState& oldState, GameState& newState)
{
   RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::GAME_STATE_CHANGED);
   GameStateChangedMessage& gscm = static_cast<GameStateChangedMessage&>(*msg);
   gscm.SetOldState(oldState);
   gscm.SetNewState(newState);
   LOG_ALWAYS("Changing game state to: " + newState.GetName());
   GetGameManager()->SendMessage(gscm);
}

void InputComponent::StopSounds()
{
   if (mBellSound != NULL && mBellSound->IsPlaying())
   {
      mBellSound->Stop();
   }

   if (mDebriefSound != NULL && mDebriefSound->IsPlaying())
   {
      mDebriefSound->Stop();
   }

   if (mWalkSound != NULL && mWalkSound->IsPlaying())
   {
      mWalkSound->Stop();
   }

   if (mRunSound != NULL && mRunSound->IsPlaying())
   {
      mRunSound->Stop();
   }

   if (mCrouchSound != NULL && mCrouchSound->IsPlaying())
   {
      mCrouchSound->Stop();
   }

   if (mPlayer != NULL)
   {
      mPlayer->StopAllSounds();
   }
}

void InputComponent::SetupTasks()
{
   dtGame::GameManager& mgr = *GetGameManager();
   dtCore::ActorPtrVector proxies;

   ///////////////////////// Mission Task /////////////////////////////////////
   mgr.FindActorsByName("TaskRootMission", proxies);
   mMission = dynamic_cast<dtActors::TaskActorOrderedProxy*>(proxies[0]);
   proxies.clear();

   mTasksSetup = true;
}

void InputComponent::ProcessTasks()
{
   if (!mTasksSetup)
   {
      return;
   }

   // Mission completed?
   if (mMission->GetScore() == mMission->GetPassingScore())
   {
      RefPtr<dtGame::Message> msg =
         GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::MISSION_COMPLETE);

      msg->SetAboutActorId(mMission->GetId());
      GetGameManager()->SendMessage(*msg);
      return;
   }

   // Mission failed?
   // Process the subtasks
   std::vector<dtActors::TaskActorProxy*> tasks;
   mMission->GetAllSubTasks(tasks);
   for (unsigned int i = 0; i < tasks.size(); i++)
   {
      // Ensure this subtask isn't another parent task
      const dtActors::TaskActorOrderedProxy* orderedTask = dynamic_cast<const dtActors::TaskActorOrderedProxy*>(tasks[i]);
      if (orderedTask != NULL)
      {
         const dtActors::TaskActorProxy* failedTask = orderedTask->GetFailingTaskProxy();
         // Failure
         if (failedTask != NULL)
         {
            const dtActors::TaskActorOrderedProxy* failedOrderedTask = dynamic_cast<const dtActors::TaskActorOrderedProxy*>(failedTask);
            if (failedOrderedTask != NULL)
            {
               const dtActors::TaskActorProxy* failedChildTask = failedOrderedTask->GetFailingTaskProxy();
               if (failedChildTask != NULL)
               {
                  RefPtr<dtGame::Message> msg =
                     GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::MISSION_FAILED);

                  msg->SetAboutActorId(failedChildTask->GetId());
                  GetGameManager()->SendMessage(*msg);
               }
            }
            else
            {
               RefPtr<dtGame::Message> msg =
                  GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::MISSION_FAILED);

               msg->SetAboutActorId(failedTask->GetId());
               GetGameManager()->SendMessage(*msg);
            }
         }
      }
   }
}

void InputComponent::OnRemovedFromGM()
{
   //assuming this is our cue to clean up after ourself
   dtAudio::AudioManager &mgr = dtAudio::AudioManager::GetInstance();

   if (mBellSound != NULL)    { mgr.FreeSound(mBellSound);    }
   if (mDebriefSound != NULL) { mgr.FreeSound(mDebriefSound); }
   if (mWalkSound != NULL)    { mgr.FreeSound(mWalkSound);    }
   if (mRunSound != NULL)     { mgr.FreeSound(mRunSound);     }
   if (mCrouchSound != NULL)  { mgr.FreeSound(mCrouchSound);  }
}
