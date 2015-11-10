/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
#ifndef _MESH_TERRAIN_ACTOR_PROXY_H_
#define _MESH_TERRAIN_ACTOR_PROXY_H_

#include <dtCore/plugin_export.h>

#include <dtActors/gamemeshactor.h>

namespace dtActors
{
   /**
    * @class MeshTerrainActor
    * @brief This proxy wraps mesh terrains
    */
   class DT_PLUGIN_EXPORT MeshTerrainActor : public GameMeshActor
   {
      public:

         typedef GameMeshActor BaseClass;

         /**
          * Constructor
          */
         MeshTerrainActor();

         /**
          * Adds the properties that are common to all mesh terrain objects.
          */
         void BuildPropertyMap() override;

         /**
          * Gets the method by which this terrain mesh is rendered.
          * @return If there is no geometry currently assigned, this
          *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
          *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
          *  is returned.
          */
         const dtCore::BaseActorObject::RenderMode& GetRenderMode() override;

         /**
          * Gets the billboard used to represent terrain meshes if this proxy's
          * render mode is RenderMode::DRAW_BILLBOARD_ICON.
          * @return
          */
         dtCore::ActorProxyIcon* GetBillBoardIcon() override;

      protected:
         virtual ~MeshTerrainActor() {}
   };
}

#endif
