/*
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
#include <dtVoxel/voxelactorregistry.h>
#include <dtVoxel/voxelactor.h>
#include <dtVoxel/volumescene.h>

#include <dtCore/actorfactory.h> // for auto register
#include <dtCore/project.h>
#include <dtCore/resourcehelper.h>

namespace dtVoxel
{ // "display name", "category", "description/tooltip"
   dtCore::RefPtr<dtCore::ActorType> VoxelActorRegistry::VOXEL_ACTOR_TYPE(new dtCore::ActorType("Voxel Actor", "dtVoxel", "Actor for loading OpenVDB voxel grids."));
   
   dtCore::RefPtr<dtCore::ActorType> VoxelActorRegistry::VOLUME_SCENE_ACTOR_TYPE(new dtCore::ActorType("Volume Scene Actor", "dtVoxel", "A scene for rendering OSG Volumes."));


   extern "C" DT_VOXEL_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new VoxelActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_VOXEL_EXPORT void DestroyPluginRegistry(
       dtCore::ActorPluginRegistry *registry)
   {
      if (registry != NULL)
      {
         delete registry;
      }
   }

   //static dtCore::AutoLibraryRegister<EngineActorRegistry> mAuto("dtActors");

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   VoxelActorRegistry::VoxelActorRegistry() : dtCore::ActorPluginRegistry("VoxelActors", "The actors in this registry work with open vdb voxel grids ")
   {
      static bool initRun = false;
      if (!initRun)
      {
         openvdb::initialize();
         std::map<std::string, std::string> extFilter;

         extFilter.insert(std::make_pair("vdb","Voxel Database Archive."));
         dtCore::RefPtr<dtCore::DefaultResourceTypeHandler> handler = new dtCore::DefaultResourceTypeHandler(dtCore::DataType::VOLUME, "Open VDB Format.", extFilter);
         dtCore::Project::GetInstance().RegisterResourceTypeHander(*handler);
         initRun = true;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void VoxelActorRegistry::RegisterActorTypes()
   {

      // Generic Task Actor
      mActorFactory->RegisterType<VoxelActor>(VOXEL_ACTOR_TYPE.get());
      mActorFactory->RegisterType<VolumeSceneActor>(VOLUME_SCENE_ACTOR_TYPE.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void VoxelActorRegistry::GetReplacementActorTypes(dtCore::ActorPluginRegistry::ActorTypeReplacements& replacements) const
   {
   }
} // namespace dtActors
