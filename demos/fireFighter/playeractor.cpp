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

#include <fireFighter/playeractor.h>
#include <fireFighter/gameitemactor.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/messages.h>
#include <fireFighter/inputcomponent.h>
#include <fireFighter/gamestate.h>
#include <fireFighter/firehoseactor.h>
#include <fireFighter/fireactor.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/functor.h>
#include <dtCore/isector.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/particlesystem.h>
#include <dtCore/transform.h>
#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>
#include <dtAudio/audiomanager.h>
#include <dtGame/messagefactory.h>

using dtCore::RefPtr;

//////////////////////////////////////////////////////////////
PlayerActorProxy::PlayerActorProxy()
{

}

PlayerActorProxy::~PlayerActorProxy()
{

}

void PlayerActorProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();
}

void PlayerActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

dtCore::ActorProxyIcon* PlayerActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_GENERIC);
   }
   return mBillBoardIcon.get();
}

void PlayerActorProxy::OnRemovedFromWorld()
{
   PlayerActor* pa = GetDrawable<PlayerActor>();

   if (dtAudio::AudioManager::GetInstance().IsInitialized() && pa->mFireHoseSound != NULL)
   {
      dtAudio::AudioManager::GetInstance().FreeSound(pa->mFireHoseSound);
   }
}

//////////////////////////////////////////////////////////////
PlayerActor::PlayerActor(dtGame::GameActorProxy& parent)
   : dtGame::GameActor(parent)
   , mIsector(new dtCore::Isector)
   , mIsCrouched(false)
   , mFireHose(new dtCore::ParticleSystem)
   , mFireHoseSound(dtAudio::AudioManager::GetInstance().NewSound())
   , mLastIntersectedMessage("__InitialID__")
{
   AddChild(mFireHose.get());
   AddChild(mFireHoseSound);

   mFireHose->SetEnabled(false);
   mFireHoseSound->SetLooping(true);
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::OnEnteredWorld()
{
   dtGame::GameActor::OnEnteredWorld();

   mIsector->SetScene(&GetGameActorProxy().GetGameManager()->GetScene());
   // Simulate the isector being camera height
   dtCore::Transform xform;
   mIsector->GetTransform(xform);
   osg::Vec3 pos;
   xform.GetTranslation(pos);
   pos.z() += 0.25f;
   xform.SetTranslation(pos);
   mIsector->SetTransform(xform);

   dtGame::Invokable* listenInvoke = new dtGame::Invokable("ListenForTickMessages",
      dtUtil::MakeFunctor(&PlayerActor::ListenForTickMessages, *this));

   GetGameActorProxy().AddInvokable(*listenInvoke);

   GetGameActorProxy().RegisterForMessages(FireFighterMessageType::GAME_STATE_CHANGED, "ListenForTickMessages");
   GetGameActorProxy().RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
      dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
}

void PlayerActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   ComputeSceneIntersections(tickMessage.GetDeltaSimTime());
}

void PlayerActor::AddItemToInventory(GameItemActor& item)
{
   if (!item.IsCollectable())
   {
      std::string error("Failed to add the item " + item.GetName() +
         " to the inventory because it is not collectable");

      LOG_ERROR(error.c_str());
      return;
   }

   if (!IsItemInInventory(item))
   {
      // Hack to ensure our iterator gets set. Since the inventory is empty
      // We know this only happens once and now set the iterator to the first
      // item in the map.
      if (mInventory.empty())
      {
         mInventory.insert(std::make_pair(item.GetItemIndex(), &item));
         mSelectedItem = mInventory.begin();
      }
      else
      {
         mInventory.insert(std::make_pair(item.GetItemIndex(), &item));
      }

      // Since we need to always have the fire hose with the player, if the
      // new item acquired is the fire hose, add as a child to the player
      FireHoseActor* fha = dynamic_cast<FireHoseActor*>(&item);
      if (fha != NULL)
      {
         mFireHose->LoadFile(fha->GetStreamFilename().c_str());
         mFireHoseSound->LoadFile(fha->GetItemUseSnd().c_str());

         // Offset the particle system a bit, so it doesn't block
         // the camera
         dtCore::Transform xform;
         mFireHose->GetTransform(xform, ABS_CS);
         osg::Vec3 pos;
         xform.GetTranslation(pos);
         pos.z() -= 0.1f;
         xform.SetTranslation(pos);
         mFireHose->SetTransform(xform, ABS_CS);

         mFireHose->GetTransform(xform, REL_CS);
         xform.GetTranslation(pos);
         pos.y() += 0.1f;
         xform.SetTranslation(pos);
         mFireHose->SetTransform(xform, REL_CS);

         // Acquired the fire hose, fire the event
         const std::string& name = "AcquireFireHose";

         dtCore::GameEvent* event = dtCore::GameEventManager::GetInstance().FindEvent(name);
         if (event == NULL)
         {
            throw dtUtil::Exception("Failed to find the game event: " + name, __FILE__, __LINE__);
         }

         dtGame::GameManager& mgr = *GetGameActorProxy().GetGameManager();
         RefPtr<dtGame::Message> msg =
            mgr.GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);

         dtGame::GameEventMessage& gem = static_cast<dtGame::GameEventMessage&>(*msg);
         gem.SetGameEvent(*event);
         mgr.SendMessage(gem);
      }

      // Play the added to inventory sound and send out a message
      item.PlayInventoryAddSnd();
      RefPtr<dtGame::Message> msg = GetGameActorProxy().GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::ITEM_ACQUIRED);
      msg->SetAboutActorId(item.GetId());
      GetGameActorProxy().GetGameManager()->SendMessage(*msg);
   }
}

bool PlayerActor::IsItemInInventory(GameItemActor &item) const
{
   return mInventory.find(item.GetItemIndex()) != mInventory.end();
}

bool PlayerActor::IsItemInInventory(const std::string &itemName) const
{
   for (std::map<int, RefPtr<GameItemActor> >::const_iterator i = mInventory.begin();
       i != mInventory.end(); ++i)
   {
      if (i->second->GetName() == itemName)
      {
         return true;
      }
   }

   return false;
}

void PlayerActor::UseSelectedItem(bool use)
{
   if (mInventory.empty())
   {
      LOG_ERROR("The inventory is empty. No items can be used.");
      return;
   }

   // Special case. See above comments about the Fire Hose Actor
   FireHoseActor* fha = dynamic_cast<FireHoseActor*>(mSelectedItem->second.get());
   if (fha != NULL)
   {
      mFireHose->SetEnabled(use);
      use ? mFireHoseSound->Play() : mFireHoseSound->Stop();
   }
   else
   {
      //use ? mSelectedItem->second->PlayItemUseSnd() : mSelectedItem->second->StopItemUseSnd();
      mSelectedItem->second->Activate(use);
   }
   RefPtr<dtGame::Message> msg = GetGameActorProxy().GetGameManager()->GetMessageFactory().CreateMessage(use ? FireFighterMessageType::ITEM_ACTIVATED : FireFighterMessageType::ITEM_DEACTIVATED);
   msg->SetAboutActorId(mSelectedItem->second->GetId());
   GetGameActorProxy().GetGameManager()->SendMessage(*msg);
}

void PlayerActor::UpdateSelectedItem(bool toTheLeft)
{
   if (mInventory.empty())
   {
      LOG_ERROR("The inventory is empty. No items can be updated.");
      return;
   }

   if (!toTheLeft)
   {
      std::map<int, RefPtr<GameItemActor> >::iterator oldItem = mSelectedItem++;
      if (mSelectedItem == mInventory.end())
      {
         mSelectedItem = oldItem;
      }
   }
   else
   {
      if (mSelectedItem != mInventory.begin())
      {
         mSelectedItem--;
      }
   }

   RefPtr<dtGame::Message> msg = GetGameActorProxy().GetGameManager()->GetMessageFactory().CreateMessage(FireFighterMessageType::ITEM_SELECTED);
   msg->SetAboutActorId(mSelectedItem->second->GetId());
   GetGameActorProxy().GetGameManager()->SendMessage(*msg);
}

void PlayerActor::ComputeSceneIntersections(const float deltaSimTime)
{
   // Update the isector values
   mIsector->Reset();

   dtCore::Transform xform;
   GetTransform(xform);
   osg::Vec3 pos;
   xform.GetTranslation(pos);
   osg::Matrix matrix;
   xform.GetRotation(matrix);
   osg::Vec3 at(matrix(1, 0), matrix(1, 1), matrix(1, 2));
   at *= 5;
   mIsector->SetStartPosition(pos);
   mIsector->SetEndPosition(pos + at);

   // On collision
   if (mIsector->Update())
   {
      dtCore::DeltaDrawable* dd = mIsector->GetClosestDeltaDrawable();
      if (dd == NULL)
      {
         SendItemIntersectedMessage(dtCore::UniqueId(""));
         return;
      }

      // Check for a collision with the fire hose spray versus the wall
      // or fire
      if (mFireHose->IsEnabled())
      {
         // Check for collision with the fire
         FireActor* fa = nullptr;
         GetGameActorProxy().GetGameManager()->FindGameActorById(dd->GetUniqueId(), fa);
         if (fa != nullptr)
         {
            fa->DecreaseIntensity(deltaSimTime);
         }
      }

      // Check for collision with a game item and send a message to show the
      // hand or not
      GameItemActor* gia = nullptr;
      GetGameActorProxy().GetGameManager()->FindGameActorById(dd->GetUniqueId(), gia);
      if (gia == NULL)
      {
         SendItemIntersectedMessage(dtCore::UniqueId(""));
         return;
      }

      LOG_INFO("Closest drawable is: " + gia->GetName());

      // Now we know we have intersected with a game item
      // we need to send an intersection message
      SendItemIntersectedMessage(gia->GetId());
    }
   else
   {
      SendItemIntersectedMessage(dtCore::UniqueId(""));
   }
}

void PlayerActor::StopAllSounds()
{
   for (std::map<int, RefPtr<GameItemActor> >::iterator i = mInventory.begin();
       i != mInventory.end(); ++i)
   {
      if (i->second->IsActivated())
      {
         i->second->Activate(false);
      }
   }
}

void PlayerActor::ListenForTickMessages(const dtGame::Message& msg)
{
   const GameStateChangedMessage& gscm = static_cast<const GameStateChangedMessage&>(msg);

   if (gscm.GetNewState() == GameState::STATE_RUNNING)
   {
      //GetGameActorProxy().RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
      //   dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   }
   else if (gscm.GetNewState() == GameState::STATE_DEBRIEF ||
           gscm.GetNewState() == GameState::STATE_MENU)
   {
      GetGameActorProxy().UnregisterForMessages(dtGame::MessageType::TICK_LOCAL,
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   }
}

void PlayerActor::SendItemIntersectedMessage(const dtCore::UniqueId& id)
{
   if (mLastIntersectedMessage != id)
   {
      mLastIntersectedMessage = id;
      dtGame::GameManager& mgr = *GetGameActorProxy().GetGameManager();
      RefPtr<dtGame::Message> msg = mgr.GetMessageFactory().CreateMessage(FireFighterMessageType::ITEM_INTERSECTED);

      msg->SetAboutActorId(id);
      mgr.SendMessage(*msg);
   }
}

void PlayerActor::SetIsCrouched(bool crouch)
{
   mIsCrouched = crouch;

   dtCore::Transform xform;
   GetTransform(xform);
   osg::Vec3 pos;
   xform.GetTranslation(pos);
   mIsCrouched ? pos.z() /= 2.0 : pos.z() *= 2.0;
   xform.SetTranslation(pos);
   SetTransform(xform);
}
