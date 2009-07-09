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
 * Matthew W. Campbell
*/
#ifndef DELTA_ENGINE_ACTOR_REGISTRY
#define DELTA_ENGINE_ACTOR_REGISTRY

#include <dtDAL/actorpluginregistry.h>
#include <dtDAL/plugin_export.h>

/** 
 * @namespace dtActors 
 * An actor library representing most of the classes from dtCore and dtABC. 
 * This DLL is intended to be loaded dynamically from STAGE.
 */
namespace dtActors 
{
    /**
     * This actor registry handles the actor proxies that are supported by the core
     * Delta3D engine.
     */
    class DT_PLUGIN_EXPORT EngineActorRegistry : public dtDAL::ActorPluginRegistry
    {
    public:
         
       static dtCore::RefPtr<dtDAL::ActorType> TASK_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> GAME_EVENT_TASK_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> ROLL_UP_TASK_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> ORDERED_TASK_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> INFINITE_LIGHT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> POSITIONAL_LIGHT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> SPOT_LIGHT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> STATIC_MESH_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> PARTICLE_SYSTEM_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> INFINITE_TERRAIN_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> MESH_TERRAIN_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> SKY_BOX_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> SKY_DOME_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> BEZIER_NODE_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> BEZIER_CONTROL_POINT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> BEZIER_CONTROLLER_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> CLOUD_PLANE_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> CLOUD_DOME_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> TRIGGER_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> PROXIMITY_TRIGGER_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> AUTOTRIGGER_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> CAMERA_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> TRIPOD_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> WEATHER_ENVIRONMENT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> SKYDOME_ENVIRONMENT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> COORDINATE_CONFIG_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> PLAYER_START_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> GAME_MESH_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> WAYPOINT_VOLUME_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> LABEL_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> DISTANCE_SENSOR_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> PREFAB_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> VOLUME_EDIT_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> LINKED_POINTS_ACTOR_TYPE;
       static dtCore::RefPtr<dtDAL::ActorType> FENCE_ACTOR_TYPE;

        /**
         * Constructs the engine actor registry.
         */
        EngineActorRegistry();

        /**
         * Registers the actor types representing the core engine objects.
         */
        void RegisterActorTypes();

        /** 
          * Get the list of deprecated ActorType names; provided for backwards
          * compatibility.
          */
        virtual void GetReplacementActorTypes(dtDAL::ActorPluginRegistry::ActorTypeReplacements &replacements) const;
    private:
       static dtCore::RefPtr<dtDAL::ActorType> ENVIRONMENT_ACTOR_TYPE; ///<deprecated 3/18/09
       static dtCore::RefPtr<dtDAL::ActorType> ENV_ACTOR_TYPE; ///<deprecated 3/18/09

    };
}

#endif
