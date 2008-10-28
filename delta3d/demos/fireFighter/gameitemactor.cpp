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

#include <fireFighter/gameitemactor.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtAudio/audiomanager.h>
#include <dtGame/gamemanager.h>
#include <dtCore/scene.h>

////////////////////////////////////////////////////////////
GameItemActorProxy::GameItemActorProxy()
{

}

GameItemActorProxy::~GameItemActorProxy()
{

}

void GameItemActorProxy::BuildPropertyMap()
{
   dtActors::GameMeshActorProxy::BuildPropertyMap();

   GameItemActor& gia = static_cast<GameItemActor&>(GetGameActor());

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::SOUND,
      "InventorySound", "InventorySound",
      dtDAL::MakeFunctor(gia, &GameItemActor::SetInventoryAddSnd),
      "Sets the inventory sound for this item"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::SOUND,
      "ItemUseSound", "ItemUseSound",
      dtDAL::MakeFunctor(gia, &GameItemActor::SetItemUseSnd),
      "Sets the inventory sound for this item"));

   AddProperty(new dtDAL::BooleanActorProperty("Activate", "Activate",
      dtDAL::MakeFunctor(gia, &GameItemActor::Activate),
      dtDAL::MakeFunctorRet(gia, &GameItemActor::IsActivated),
      "Activates this item"));

   AddProperty(new dtDAL::BooleanActorProperty("Collectable", "Collectable",
      dtDAL::MakeFunctor(gia, &GameItemActor::SetCollectable),
      dtDAL::MakeFunctorRet(gia, &GameItemActor::IsCollectable),
      "Returns true if this item is collectable"));
}

void GameItemActorProxy::BuildInvokables()
{
   dtActors::GameMeshActorProxy::BuildInvokables();
}

dtDAL::ActorProxyIcon* GameItemActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::STATICMESH);
   }
   return mBillBoardIcon.get();
}

////////////////////////////////////////////////////////////
GameItemActor::GameItemActor(dtGame::GameActorProxy& proxy)
   : dtActors::GameMeshActor(proxy)
   , mInventoryAddSnd(dtAudio::AudioManager::GetInstance().NewSound())
   , mItemUseSnd(dtAudio::AudioManager::GetInstance().NewSound())
   , mItemIndex(-1)
   , mCollectable(false)
   , mIsActivated(false)
{

}

GameItemActor::~GameItemActor()
{
   /*
   dtAudio::Sound* snd = mInventoryAddSnd.release();
   if (snd != NULL)
   {
      dtAudio::AudioManager::GetInstance().FreeSound(snd);
   }
   snd = mItemUseSnd.release();
   if (snd != NULL)
   {
      dtAudio::AudioManager::GetInstance().FreeSound(snd);
   }
   //*/
}

void GameItemActor::OnEnteredWorld()
{

}

void GameItemActor::SetItemUseSnd(const std::string& fileName)
{
   if (fileName.empty())
   {
      LOG_ERROR("Attempted to load an empty sound file string.");
      return;
   }

   mItemUseSnd->LoadFile(fileName.c_str());
   AddChild(mItemUseSnd.get());
}

void GameItemActor::SetInventoryAddSnd(const std::string& fileName)
{
   if (fileName.empty())
   {
      LOG_ERROR("Attempted to load an empty sound file string.");
      return;
   }

   mInventoryAddSnd->LoadFile(fileName.c_str());
   AddChild(mInventoryAddSnd.get());
}

void GameItemActor::Activate(bool enable)
{
   mIsActivated = enable;

   mIsActivated ? mItemUseSnd->Play() : mItemUseSnd->Stop();
}

