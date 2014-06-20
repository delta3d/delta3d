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
#ifndef DELTA_FIRE_FIGHTER_PLAYER_ACTOR
#define DELTA_FIRE_FIGHTER_PLAYER_ACTOR

#include <dtGame/gameactor.h>
#include <fireFighter/export.h>

// Forward declarations
namespace dtCore
{
   class ActorProxyIcon;
}

namespace dtCore
{
   class Isector;
   class ParticleSystem;
}

namespace dtAudio
{
   class Sound;
}

class GameItemActor;

class FIRE_FIGHTER_EXPORT PlayerActor : public dtGame::GameActor
{
   public:

      /// Constructor
      PlayerActor(dtGame::GameActorProxy& parent);

      /// Override
      void OnTickLocal(const dtGame::TickMessage& tickMessage);

      /**
       * Adds a game item to the player
       * @param item The item to add
       */
      void AddItemToInventory(GameItemActor& item);

      /**
       * Returns true if the player has item
       */
      bool IsItemInInventory(GameItemActor& item) const;

      /**
       * Returns true if the player has item
       */
      bool IsItemInInventory(const std::string& itemName) const;

      /**
       * Used the currently selected item in the player's inventory
       * @param use True to enable, false to disable
       */
      void UseSelectedItem(bool use);

      /**
       * Updates the selected item
       */
      void UpdateSelectedItem(bool toTheLeft);

      /**
       * Stops all sounds in the inventory
       */
      void StopAllSounds();

      /**
       * Returns true is the player is crouched
       * @return mIsCrouched
       */
      bool IsCrouched() const { return mIsCrouched; }

      /**
       * Sets if the player is crouched
       * @param crouch true to crouch the player
       */
      void SetIsCrouched(bool crouch);

      /**
       * Returns a pointer to the currently selected item in the
       * player's inventory
       * @return A pointer to a GameItemActor, or NULL if the inventory
       * is empty
       */
      GameItemActor* GetCurrentItem() const { return mInventory.empty() ? NULL : mSelectedItem->second.get(); }

      /// Invokable to listen for tick messages
      void ListenForTickMessages(const dtGame::Message &msg);

   protected:

      /// Override, called when the player enters the world
      virtual void OnEnteredWorld();

      /// Destructor
      virtual ~PlayerActor();

   private:

      /**
       * Computes intersections with the scene
       * If a game item or something else of interest
       * is intersected with, a MessageType::ITEM_INTERSECTED
       * message is sent with the about actor ID of the item
       * that is intersected
       * @param deltaSimTime The difference in time
       */
      void ComputeSceneIntersections(const float deltaSimTime);

      /**
       * Private helper method to send out an ITEM_INTERSECTED message
       * @param id The unique id of the about actor
       */
      void SendItemIntersectedMessage(const dtCore::UniqueId &id);

      friend class PlayerActorProxy;
      dtCore::RefPtr<dtCore::Isector> mIsector;
      std::map<int, dtCore::RefPtr<GameItemActor> > mInventory;
      std::map<int, dtCore::RefPtr<GameItemActor> >::iterator mSelectedItem;
      bool mIsCrouched;
      dtCore::RefPtr<dtCore::ParticleSystem> mFireHose;
      dtAudio::Sound *mFireHoseSound;
      dtCore::UniqueId mLastIntersectedMessage;
};

class FIRE_FIGHTER_EXPORT PlayerActorProxy : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      PlayerActorProxy();

      /// Builds the properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Creates the actor
      virtual void CreateDrawable() { SetDrawable(*new PlayerActor(*this)); }

      // Used in STAGE
      dtCore::ActorProxyIcon* GetBillBoardIcon();

      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

      virtual void OnRemovedFromWorld();

   protected:

      /// Destructor
      virtual ~PlayerActorProxy();

   private:
};

#endif
