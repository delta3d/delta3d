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
#ifndef DELTA_FIRE_FIGHTER_COLLIDABLE_ACTOR
#define DELTA_FIRE_FIGHTER_COLLIDABLE_ACTOR

#include <dtGame/gameactor.h>
#include <fireFighter/export.h>
#include <dtCore/actorproxyicon.h>

class FIRE_FIGHTER_EXPORT CollidableActor : public dtGame::GameActor
{
   public:

      /// Constructor
      CollidableActor(dtGame::GameActorProxy& parent);

   protected:

      /// Destructor
      virtual ~CollidableActor();

   private:
};

class FIRE_FIGHTER_EXPORT CollidableActorProxy : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      CollidableActorProxy();

      /// Builds the properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Creates the actor
      virtual void CreateDrawable() { SetDrawable(*new CollidableActor(*this)); }

      /**
       * Gets the billboard used to represent static meshes if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON.
       */
      dtCore::ActorProxyIcon* GetBillBoardIcon();

      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

   protected:

      /// Destructor
      virtual ~CollidableActorProxy();

   private:

      dtCore::RefPtr<dtCore::ActorProxyIcon> mBillboardIcon;
};

#endif
