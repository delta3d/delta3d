/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005, MOVES Institute
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
#ifndef DELTA_PLAYER_START_ACTOR_PROXY
#define DELTA_PLAYER_START_ACTOR_PROXY
 
#include <dtCore/transformable.h>
#include <dtCore/plugin_export.h>
#include <dtCore/transformableactorproxy.h>

namespace dtCore
{
   class ActorProxyIcon;
}

namespace dtActors
{
   class DT_PLUGIN_EXPORT PlayerStartActor : public dtCore::Transformable
   {
      public:

         /// Constructor
         PlayerStartActor();

      protected:

         /// Destructor
         virtual ~PlayerStartActor();

      private:
   };

   class DT_PLUGIN_EXPORT PlayerStartActorProxy : public dtCore::TransformableActorProxy
   {
      public:

         /// Constructor
         PlayerStartActorProxy();

         /// Creates the actor associated with this proxy
         void CreateDrawable() { SetDrawable(*new PlayerStartActor); }

         /// Builds the properties of this proxy
         void BuildPropertyMap();

         /**
           * Gets the billboard used to represent static meshes if this proxy's
           * render mode is RenderMode::DRAW_BILLBOARD_ICON.
           * @return a pointer to the icon
           */
         virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

         /**
           * Gets the current render mode for positional lights.
           * @return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
           */
         virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
         {
            return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
         }

      protected:

         /// Destructor
         virtual ~PlayerStartActorProxy();
   };
}
#endif
