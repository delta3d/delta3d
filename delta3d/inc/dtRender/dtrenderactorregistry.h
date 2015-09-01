/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2014, Caper Holdings, LLC
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
#ifndef DELTA_RENDER_ACTOR_REGISTRY
#define DELTA_RENDER_ACTOR_REGISTRY

#include <dtCore/actorpluginregistry.h>
#include <dtRender/dtrenderexport.h>
#include <dtCore/refptr.h>

namespace dtRender
{
   class DT_RENDER_EXPORT RenderActorRegistry : public dtCore::ActorPluginRegistry
   {
   public:

      static dtCore::RefPtr<dtCore::ActorType> ATMOSPHERE_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> DOF_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> DYNAMIC_LIGHT_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> EPHEMERIS_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> GUI_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> HDR_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> MULTIPASS_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> OCEAN_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> LIGHT_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> PPU_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> CUBEMAP_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> SCENE_MANAGER_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> SHADOW_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> SIMPLE_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> SPOT_LIGHT_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> SSAO_SCENE_ACTOR_TYPE;

      static dtCore::RefPtr<dtCore::ActorType> VIDEO_SCENE_ACTOR_TYPE;
      

      // constructor
      RenderActorRegistry();

      void RegisterActorTypes();
   };
} // end namespace

#endif //DELTA_RENDER_ACTOR_REGISTRY
