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
#include <dtActors/deltaobjectactorproxy.h>
#include <dtCore/object.h>

namespace dtCore
{
   class Scene;
}

namespace dtActors
{
   class DT_PLUGIN_EXPORT MeshTerrainActor : public dtCore::Object
   {
      public:

         /// Constructor
         MeshTerrainActor();

         virtual void AddedToScene(dtCore::Scene* scene);

      protected:

         /// Destructor
         virtual ~MeshTerrainActor();

   };
   /**
    * @class MeshTerrainActorProxy
    * @brief This proxy wraps mesh terrains
    */
   class DT_PLUGIN_EXPORT MeshTerrainActorProxy : public dtActors::DeltaObjectActorProxy
   {
      public:

         /**
          * Constructor
          */
         MeshTerrainActorProxy();

         /**
          * Adds the properties that are common to all mesh terrain objects.
          */
         virtual void BuildPropertyMap();

         /**
          * Loads a mesh file which contains terrain.
          * @param fileName The file of the terrain mesh to load.
          * @note Although terrain meshes are the same "type" of file as static meshes
          *  and other geometry, mesh terrains have a special resource of type
          *  DataType::TERRAIN.
          */
         virtual void LoadFile(const std::string &fileName);

         /**
          * Gets the method by which this terrain mesh is rendered.
          * @return If there is no geometry currently assigned, this
          *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
          *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
          *  is returned.
          */
         virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

         /**
          * Gets the billboard used to represent terrain meshes if this proxy's
          * render mode is RenderMode::DRAW_BILLBOARD_ICON.
          * @return
          */
         virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

      protected:
         /**
          * Creates a mesh terrain instance.
          */
         virtual void CreateDrawable();

         virtual ~MeshTerrainActorProxy() {}
   };
}

#endif
