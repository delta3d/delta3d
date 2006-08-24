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
 * @author William E. Johnson II
 */
#include <dtCore/transformable.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>

namespace dtDAL
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

   class DT_PLUGIN_EXPORT PlayerStartActorProxy : public dtDAL::TransformableActorProxy
   {
      public:

         /// Constructor
         PlayerStartActorProxy();

         /// Creates the actor associated with this proxy
         void CreateActor() { mActor = new PlayerStartActor; }

         /// Builds the properties of this proxy
         void BuildPropertyMap();

         /**
           * Gets the billboard used to represent static meshes if this proxy's
           * render mode is RenderMode::DRAW_BILLBOARD_ICON.
           * @return a pointer to the icon
           */
         virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();

         /**
           * Gets the current render mode for positional lights.
           * @return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.
           */
         virtual const ActorProxy::RenderMode& GetRenderMode()
         {
            return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
         }

      protected:

         /// Destructor
         virtual ~PlayerStartActorProxy();
   };
}
