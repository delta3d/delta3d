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
#ifndef DELTA_FIRE_FIGHTER_GAME_ITEM_ACTOR
#define DELTA_FIRE_FIGHTER_GAME_ITEM_ACTOR

#include <dtActors/gamemeshactor.h>
#include <fireFighter/export.h>
#include <dtAudio/sound.h>
#include <dtCore/actorproxyicon.h>

class FIRE_FIGHTER_EXPORT GameItemActor : public dtActors::GameMeshActor
{
   public:
      
      /// Constructor
      GameItemActor(dtGame::GameActorProxy &proxy);

      /**
       * Base class method for activating a game item
       * @param enable true to enable the item
       */
      virtual void Activate(bool enable = true);

      /**
       * Returns true is this item is activated
       * @return mIsActivated
       */
      bool IsActivated() const { return mIsActivated; }

      /**
       * Returns true if this game item is collectable
       * @return mCollectable
       */
      bool IsCollectable() const { return mCollectable; }
      
      /**
       * Sets if this item is collectable
       * @param enable true for collectable
       */
      void SetCollectable(bool enable) { mCollectable = enable; }

      /**
       * Sets the sound played when the item is acquired
       * @param fileName The sound file name
       */
      void SetInventoryAddSnd(const std::string &fileName); 
      
      /**
       * Returns the file name of the inventory sound
       * @return The file name
       */
      std::string GetInventoryAddSnd()  { return mInventoryAddSnd->GetFilename(); }
      
      /**
       * Plays the inventory add sound
       */
      void PlayInventoryAddSnd() { mInventoryAddSnd->Play(); }
      
      /**
       * Stops the inventory add sound
       */
      void StopInventoryAddSnd() { mInventoryAddSnd->Stop(); }

      /**
       * Sets the sound used when the item is activated/used
       * @param fileName The sound file to use
       */
      void SetItemUseSnd(const std::string &fileName); 
      
      /**
       * Returns the file name of the item use sound
       * @return The file name
       */
      std::string GetItemUseSnd() { return mItemUseSnd->GetFilename(); }
      
      /**
       * Plays the item use sound
       */
      void PlayItemUseSnd() { mItemUseSnd->Play(); }
      
      /**
       * Stops the item use sound
       */
      void StopItemUseSnd() { mItemUseSnd->Stop(); }

      /**
       * Returns the index this item uses as its key into the inventory
       * map on the player class. 
       * @see class PlayerActor
       * @return mItemIndex
       */
      virtual int GetItemIndex() const { return mItemIndex; }

   protected:

      /// Called when the actor is added to the GM
      virtual void OnEnteredWorld();

      /// Destructor
      virtual ~GameItemActor();

      dtCore::RefPtr<dtAudio::Sound> mInventoryAddSnd; 
      dtCore::RefPtr<dtAudio::Sound> mItemUseSnd; 
      int mItemIndex;

   private:
      
      bool mCollectable;
      bool mIsActivated;
 };

class FIRE_FIGHTER_EXPORT GameItemActorProxy : public dtActors::GameMeshActorProxy
{
   public:

      /// Constructor
      GameItemActorProxy();

      /// Builds the actor properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Instantiates the actor
      virtual void CreateDrawable() { SetDrawable(*new GameItemActor(*this)); }

      // Used in STAGE
      dtCore::ActorProxyIcon* GetBillBoardIcon();

      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
      }

   protected:

      /// Destructor
      virtual ~GameItemActorProxy();

   private:

      dtCore::RefPtr<dtCore::ActorProxyIcon> mBillBoardIcon; 
};

#endif
