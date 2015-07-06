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

#include <dtAudio/audiomanager.h>

#include <dtCore/scene.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/functor.h>
#include <dtCore/resourceactorproperty.h>

#include <dtGame/gamemanager.h>

////////////////////////////////////////////////////////////
GameItemActor::GameItemActor()
: mInventoryAddSnd(dtAudio::AudioManager::GetInstance().NewSound())
, mItemUseSnd(dtAudio::AudioManager::GetInstance().NewSound())
, mItemIndex(-1)
, mCollectable(false)
, mIsActivated(false)
{

}

GameItemActor::~GameItemActor()
{

}

void GameItemActor::BuildPropertyMap()
{
   dtActors::GameMeshActor::BuildPropertyMap();

   AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::SOUND,
      "InventorySound", "InventorySound",
      dtUtil::MakeFunctor(&GameItemActor::SetInventoryAddSnd, this),
      "Sets the inventory sound for this item"));

   AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::SOUND,
      "ItemUseSound", "ItemUseSound",
      dtUtil::MakeFunctor(&GameItemActor::SetItemUseSnd, this),
      "Sets the inventory sound for this item"));

   AddProperty(new dtCore::BooleanActorProperty("Activate", "Activate",
      dtUtil::MakeFunctor(&GameItemActor::Activate, this),
      dtUtil::MakeFunctor(&GameItemActor::IsActivated, this),
      "Activates this item"));

   AddProperty(new dtCore::BooleanActorProperty("Collectable", "Collectable",
      dtUtil::MakeFunctor(&GameItemActor::SetCollectable, this),
      dtUtil::MakeFunctor(&GameItemActor::IsCollectable, this),
      "Returns true if this item is collectable"));
}

void GameItemActor::BuildInvokables()
{
   dtActors::GameMeshActor::BuildInvokables();
}

dtCore::ActorProxyIcon* GameItemActor::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH);
   }
   return mBillBoardIcon.get();
}

void GameItemActor::SetItemUseSnd(const std::string& fileName)
{
   if (fileName.empty())
   {
      LOG_ERROR("Attempted to load an empty sound file string.");
      return;
   }

   mItemUseSnd->LoadFile(fileName.c_str());
   GetDrawable()->AddChild(mItemUseSnd.get());
}

void GameItemActor::SetInventoryAddSnd(const std::string& fileName)
{
   if (fileName.empty())
   {
      LOG_ERROR("Attempted to load an empty sound file string.");
      return;
   }

   mInventoryAddSnd->LoadFile(fileName.c_str());
   GetDrawable()->AddChild(mInventoryAddSnd.get());
}

void GameItemActor::Activate(bool enable)
{
   mIsActivated = enable;

   mIsActivated ? mItemUseSnd->Play() : mItemUseSnd->Stop();
}

