/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2016, Chipper Chickadee Studios, LLC
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
 * Bradley Anderegg
 */

#include <dtVR/openvractorregistry.h>
#include <dtVR/openvrscene.h>

#include <dtCore/actorfactory.h> // for auto register
#include <dtCore/project.h>
#include <dtCore/resourcehelper.h>

namespace dtVR
{ // "display name", "category", "description/tooltip"
   
   dtCore::RefPtr<dtCore::ActorType> OpenVRActorRegistry::OPENVR_SCENE_ACTOR_TYPE(new dtCore::ActorType("OpenVR Scene Actor", "dtVR", "A scene for integrating OpenVR."));


   extern "C" DT_VR_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new OpenVRActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_VR_EXPORT void DestroyPluginRegistry(
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
   OpenVRActorRegistry::OpenVRActorRegistry() : dtCore::ActorPluginRegistry("VoxelActors", "The actors in this registry work with open vdb voxel grids ")
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void OpenVRActorRegistry::RegisterActorTypes()
   {
      
      mActorFactory->RegisterType<OpenVRSceneActor>(OPENVR_SCENE_ACTOR_TYPE.get());
   }

   //////////////////////////////////////////////////////////////////////////
   void OpenVRActorRegistry::GetReplacementActorTypes(dtCore::ActorPluginRegistry::ActorTypeReplacements& replacements) const
   {
   }
} // namespace dtActors
