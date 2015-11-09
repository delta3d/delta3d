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

#include <fireFighter/gamelevelactor.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/messages.h>
#include <fireFighter/gamestate.h>
#include <fireFighter/hatchactor.h>
#include <dtGame/invokable.h>
#include <dtGame/gamemanager.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/functor.h>
#include <dtUtil/log.h>
#include <dtAudio/audiomanager.h>
#include <osg/MatrixTransform>

/////////////////////////////////////////////////////////////////
GameLevelActorProxy::GameLevelActorProxy()
{

}

GameLevelActorProxy::~GameLevelActorProxy()
{

}

void GameLevelActorProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   GameLevelActor& gla = *GetDrawable<GameLevelActor>();

   AddProperty(new dtCore::ResourceActorProperty(*this, dtCore::DataType::STATIC_MESH,
      "Model", "Model",
      dtUtil::MakeFunctor(&GameLevelActor::LoadFile, gla),
      "Loads the model file for the level"));
}

void GameLevelActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

dtCore::ActorProxyIcon* GameLevelActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH);
   }
   return mBillBoardIcon.get();
}

void GameLevelActorProxy::OnEnteredWorld()
{
//   dtGame::Invokable* invoke = new dtGame::Invokable("ResetCollisionMesh",
//      dtUtil::MakeFunctor(&GameLevelActor::ResetCollisionMesh,
//            GetDrawable<GameLevelActor>()));

//   AddInvokable(*invoke);

   RegisterForMessages(FireFighterMessageType::ITEM_ACTIVATED,   "ResetCollisionMesh");
   RegisterForMessages(FireFighterMessageType::ITEM_DEACTIVATED, "ResetCollisionMesh");
}

/////////////////////////////////////////////////////////////////
GameLevelActor::GameLevelActor(dtGame::GameActorProxy& parent)
   : dtGame::GameActor(parent)
   , mAmbientSound(NULL)
{

}

GameLevelActor::~GameLevelActor()
{
   if (mAmbientSound != NULL)
   {
      dtAudio::AudioManager::GetInstance().FreeSound(mAmbientSound);
   }
}

void GameLevelActor::OnEnteredWorld()
{
   dtGame::GameActor::OnEnteredWorld();

   dtGame::Invokable* invoke = new dtGame::Invokable("StopSounds",
      dtUtil::MakeFunctor(&GameLevelActor::StopSounds, *this));

   GetGameActorProxy().AddInvokable(*invoke);
   GetGameActorProxy().RegisterForMessages(FireFighterMessageType::GAME_STATE_CHANGED, "StopSounds");

   mAmbientSound = dtAudio::AudioManager::GetInstance().NewSound();
   mAmbientSound->LoadFile("Sounds/ambientVentilation.wav");
   mAmbientSound->SetLooping(true);
   mAmbientSound->SetGain(0.25f);
   mAmbientSound->Play();
}

void GameLevelActor::StopSounds(const dtGame::Message& msg)
{
   const GameStateChangedMessage& gscm = static_cast<const GameStateChangedMessage&>(msg);
   if (gscm.GetNewState() == GameState::STATE_MENU   ||
      gscm.GetNewState() == GameState::STATE_DEBRIEF)
   {
      if (mAmbientSound != NULL && mAmbientSound->IsPlaying())
      {
         mAmbientSound->Stop();
      }
   }
}

void GameLevelActor::LoadFile(const std::string& filename)
{
   osg::Node* node = dtCore::Loadable::LoadFile(filename);
   if (node == NULL)
   {
      LOG_ERROR("Failed to load the game level file: " + filename);
      return;
   }

   GetMatrixNode()->addChild(node);
}
