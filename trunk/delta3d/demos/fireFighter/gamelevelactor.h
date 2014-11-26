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
#ifndef DELTA_FIRE_FIGHTER_GAME_LEVEL_ACTOR
#define DELTA_FIRE_FIGHTER_GAME_LEVEL_ACTOR

#include <dtGame/gameactor.h>
#include <dtCore/loadable.h>
#include <dtCore/actorproxyicon.h>
#include <fireFighter/export.h>

namespace dtAudio
{
   class Sound;
}

class FIRE_FIGHTER_EXPORT GameLevelActor : public dtGame::GameActor, public dtCore::Loadable
{
   public:

      /// Constructor
      GameLevelActor(dtGame::GameActorProxy& parent);

      /**
       * Loads the game level file
       * @param fileName The file to load
       */
      void LoadFile(const std::string &filename);

      /// Stops any sounds this actor has
      void StopSounds(const dtGame::Message &msg);


   protected:

      /// Called when the actor is added to the game manager
      virtual void OnEnteredWorld();

      /// Destructor
      virtual ~GameLevelActor();

   private:

      dtAudio::Sound *mAmbientSound;
};

class FIRE_FIGHTER_EXPORT GameLevelActorProxy : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      GameLevelActorProxy();

      /// Builds the actor properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Creates the actor
      virtual void CreateDrawable() { SetDrawable(*new GameLevelActor(*this)); }

      /**
       * Gets the billboard used to represent static meshes if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON.
       */
      dtCore::ActorProxyIcon* GetBillBoardIcon();

      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
      }

   protected:

      /// Called when the actor is added to the game manager
      virtual void OnEnteredWorld();

      /// Destructor
      virtual ~GameLevelActorProxy();

   private:

      dtCore::RefPtr<dtCore::ActorProxyIcon> mBillboardIcon;
};

#endif
