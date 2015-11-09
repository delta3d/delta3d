/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#ifndef DELTA_VOXEL_ACTOR_REGISTRY
#define DELTA_VOXEL_ACTOR_REGISTRY

#include <dtCore/actorpluginregistry.h>
#include <dtVoxel/export.h>

/** 
 * @namespace dtVoxel
 */
namespace dtVoxel
{
   /**
    * This actor registry handles the actors in this library
    */
   class DT_VOXEL_EXPORT VoxelActorRegistry : public dtCore::ActorPluginRegistry
   {
   public:

      static dtCore::RefPtr<dtCore::ActorType> VOXEL_ACTOR_TYPE;
      static dtCore::RefPtr<dtCore::ActorType> VOLUME_SCENE_ACTOR_TYPE;

      /**
       * Constructs the voxel actor registry.
       */
      VoxelActorRegistry();

      /**
       * Registers the actor types representing the core engine objects.
       */
      void RegisterActorTypes();

      /**
       * Get the list of deprecated ActorType names; provided for backwards
       * compatibility.
       */
      virtual void GetReplacementActorTypes(dtCore::ActorPluginRegistry::ActorTypeReplacements& replacements) const;
   private:
   };
}

#endif
