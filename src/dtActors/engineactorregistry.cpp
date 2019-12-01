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
 * Chris Osborn
 */
#include <dtActors/engineactorregistry.h>
#include <dtActors/infinitelightactorproxy.h>
#include <dtActors/particlesystemactorproxy.h>
#include <dtActors/dynamicparticlesystemactor.h>
#include <dtActors/positionallightactorproxy.h>
#include <dtActors/spotlightactorproxy.h>


#include <dtActors/infiniteterrainactorproxy.h>
#include <dtActors/autotriggeractorproxy.h>
#include <dtActors/backdropactor.h>
#include <dtActors/beziercontrolleractor.h>
#include <dtActors/meshterrainactorproxy.h>
#include <dtActors/clouddomeactorproxy.h>
#include <dtActors/cloudplaneactorproxy.h>
#include <dtActors/skyboxactorproxy.h>
#include <dtActors/skydomeactorproxy.h>
#include <dtActors/beziernodeactorproxy.h>
#include <dtActors/beziercontrolpointactorproxy.h>
#include <dtActors/triggeractorproxy.h>
#include <dtActors/proximitytriggeractorproxy.h>
#include <dtActors/cameraactorproxy.h>
#include <dtActors/cameradataactor.h>
#include <dtActors/tripodactorproxy.h>
#include <dtActors/taskactor.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/taskactorrollup.h>
#include <dtActors/taskactorordered.h>
#include <dtActors/weatherenvironmentactor.h>
#include <dtActors/skydomeenvironmentactor.h>
#include <dtActors/coordinateconfigactor.h>
#include <dtActors/playerstartactorproxy.h>
#include <dtActors/gamemeshactor.h>
#include <dtActors/distancesensoractor.h>
#include <dtActors/waypointvolumeactor.h>
#include <dtActors/labelactorproxy.h>
#include <dtActors/volumeeditactor.h>
#include <dtActors/linkedpointsactorproxy.h>
#include <dtActors/fenceactor.h>
#include <dtActors/buildingactor.h>
#include <dtActors/watergridactor.h>
#include <dtActors/triggervolumeactorproxy.h>
#include <dtActors/directoractor.h>
#include <dtActors/positiondataactor.h>
#include <dtActors/dynamicparticlesactorcomponent.h>
#include <dtActors/beziercontrolleractorcomponent.h>
#include <dtActors/prefabactor.h>


#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/logcontroller.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/taskcomponent.h>
#include <dtGame/deadreckoningcomponent.h>


#include <dtGame/cascadingdeleteactorcomponent.h>
#include <dtGame/shaderactorcomponent.h>
#include <dtGame/deadreckoninghelper.h>
#include <dtGame/drpublishingactcomp.h>

#include <dtCore/actorfactory.h> // for auto register

namespace dtActors
{ // "display name", "category", "description/tooltip"
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::TASK_ACTOR_TYPE(new dtCore::ActorType("Task Actor", "dtcore.Tasks","Generic task actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE(new dtCore::ActorType("GameEvent Task Actor", "dtcore.Tasks","Game event task actor.", TASK_ACTOR_TYPE.get()));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::ROLL_UP_TASK_ACTOR_TYPE(new dtCore::ActorType("Rollup Task Actor", "dtcore.Tasks", "Rollup or container based task actor.", TASK_ACTOR_TYPE.get()));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::ORDERED_TASK_ACTOR_TYPE(new dtCore::ActorType("Ordered Task Actor", "dtcore.Tasks", "Ordered task actor similar to a roll up task actor but with a constraint on the order in which sub tasks may be completed.", TASK_ACTOR_TYPE.get()));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::INFINITE_LIGHT_ACTOR_TYPE(new dtCore::ActorType("Infinite Light", "dtcore.Lights","Directional light actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::POSITIONAL_LIGHT_ACTOR_TYPE(new dtCore::ActorType("Positional Light", "dtcore.Lights", "Positional light actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::SPOT_LIGHT_ACTOR_TYPE(new dtCore::ActorType("Spotlight", "dtcore.Lights", "Spotlight light actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::PARTICLE_SYSTEM_ACTOR_TYPE(new dtCore::ActorType("Particle System", "dtcore", "dtCore::ParticleSystem actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::DYNAMIC_PARTICLE_SYSTEM_ACTOR_TYPE(new dtCore::ActorType("Dynamic Particle System", "dtcore", "dtCore::DynamicParticleSystem actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::INFINITE_TERRAIN_ACTOR_TYPE(new dtCore::ActorType("Infinite Terrain", "dtcore.Terrain", "dtCore::InfiniteTerrain actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::MESH_TERRAIN_ACTOR_TYPE(new dtCore::ActorType("Mesh Terrain", "dtcore.Terrain", "An terrain actor who's geometry is represented via a mesh file."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::SKY_BOX_ACTOR_TYPE(new dtCore::ActorType("Sky Box", "dtcore.Environment", "dtCore::SkyBox Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::SKY_DOME_ACTOR_TYPE(new dtCore::ActorType("Sky Dome", "dtcore.Environment", "dtCore::SkyDome Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::BEZIER_NODE_ACTOR_TYPE(new dtCore::ActorType("Bezier Node", "dtcore.Curve", "dtABC::BezierNode Actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::BEZIER_CONTROL_POINT_ACTOR_TYPE(new dtCore::ActorType("Bezier Control Point", "dtcore.Curve", "dtABC::BezierControlPoint Actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::BEZIER_CONTROLLER_ACTOR_TYPE(new dtCore::ActorType("Bezier Controller", "dtcore.Action", "dtABC::BezierController Actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::CLOUD_PLANE_ACTOR_TYPE(new dtCore::ActorType("Cloud Plane", "dtcore.Environment", "dtCore::CloudPlane Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::CLOUD_DOME_ACTOR_TYPE(new dtCore::ActorType("Cloud Dome", "dtcore.Environment", "dtCore::CloudDome Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::TRIGGER_ACTOR_TYPE(new dtCore::ActorType("Trigger", "dtcore.Triggers", "dtABC::Trigger Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::PROXIMITY_TRIGGER_ACTOR_TYPE(new dtCore::ActorType("Proximity Trigger","dtcore.Triggers", "dtABC::ProximityTrigger Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::AUTOTRIGGER_ACTOR_TYPE(new dtCore::ActorType("AutoTrigger", "dtcore.Triggers", "dtABC::AutoTrigger Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::CAMERA_ACTOR_TYPE(new dtCore::ActorType("Camera", "dtcore", "dtCore::Camera Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::CAMERA_DATA_ACTOR_TYPE(new dtCore::ActorType("CameraData", "dtActors", "dtActors::CameraData Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::TRIPOD_ACTOR_TYPE(new dtCore::ActorType("Tripod", "dtcore", "dtCore::Tripod Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::WEATHER_ENVIRONMENT_ACTOR_TYPE(new dtCore::ActorType("WeatherEnvironment", "dtcore.Environment", "dtCore::Environment controlled by dtABC::Weather"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::SKYDOME_ENVIRONMENT_ACTOR_TYPE(new dtCore::ActorType("SkyDomeEnvironment", "dtcore.Environment", "dtCore::Environment plus a SkyDome"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::COORDINATE_CONFIG_ACTOR_TYPE(new dtCore::ActorType("Coordinate Config", "dtutil", "dtUtil::Coordinates Actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::PLAYER_START_ACTOR_TYPE(new dtCore::ActorType("Player Start", "dtcore", "This can be dropped into a map and the player actor will spawn himself there on startup"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::WAYPOINT_VOLUME_ACTOR_TYPE(new dtCore::ActorType("WaypointVolume", "dtai.waypoint", "dtActos::Waypoint VolumeActor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::LABEL_ACTOR_TYPE(new dtCore::ActorType("Label", "dtcore", "Simple 3D label drawable."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::GAME_MESH_ACTOR_TYPE(new dtCore::ActorType("Game Mesh Actor", "dtcore.Game.Actors", "Game Actor that supports a single Mesh geometry"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::DISTANCE_SENSOR_ACTOR_TYPE(new dtCore::ActorType("Distance Sensor", "dtai.Game.Actors", "Game Actor that wraps and triggers a dtAI distance sensor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::VOLUME_EDIT_ACTOR_TYPE(new dtCore::ActorType("Volume Edit", "dtutil", "dtutil::VolumeEdit actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::PREFAB_ACTOR_TYPE(new dtCore::ActorType("Prefab", "dtActors", "dtActors.Prefab actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::LINKED_POINTS_ACTOR_TYPE(new dtCore::ActorType("LinkedPoints", "dtActors", "dtActors.LinkedPoints actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::FENCE_ACTOR_TYPE(new dtCore::ActorType("Fence", "dtActors", "dtActors.Fence actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::BUILDING_ACTOR_TYPE(new dtCore::ActorType("Building", "dtActors", "dtActors.Building actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::BACKDROP_ACTOR_TYPE(new dtCore::ActorType("Backdrop", "dtActors", "dtActors.Backdrop actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::WATER_GRID_ACTOR_TYPE(new dtCore::ActorType("Water", "dtActors", "dtActors.Water actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::TRIGGER_VOLUME_ACTOR_TYPE(new dtCore::ActorType("Trigger Volume Actor", "dtActors", "dtActors.Trigger Volume Actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::DIRECTOR_ACTOR_TYPE(new dtCore::ActorType("Director Actor", "dtActors", "dtActors.Director Actor"));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::POSITION_DATA_ACTOR_TYPE(new dtCore::ActorType("Position Data Actor", "dtActors", "dtActors.Position Data Actor"));

   /// deprecated types
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::ENVIRONMENT_ACTOR_TYPE(new dtCore::ActorType("Environment", "dtcore.Environment", "dtCore::BasicEnvironment Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::ENV_ACTOR_TYPE(new dtCore::ActorType("Env", "dtcore.Environment", "dtCore::Environment Actor."));
   dtCore::RefPtr<dtCore::ActorType> EngineActorRegistry::STATIC_MESH_ACTOR_TYPE(new dtCore::ActorType("Static Mesh", "dtcore", "Loadable static mesh actor."));


   extern "C" DT_PLUGIN_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new EngineActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_PLUGIN_EXPORT void DestroyPluginRegistry(
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
   EngineActorRegistry::EngineActorRegistry() : dtCore::ActorPluginRegistry("CoreActors", "The actors in this registry are the default base "
         "actors provided by the Delta3D engine.")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EngineActorRegistry::RegisterActorTypes()
   {

      // In this method we will add each of the actor types the engine actor
      // registry supports to the object factory.  These will then get
      // registered with the ActorFactory.

      // Generic Task Actor
      mActorFactory->RegisterType<TaskActorProxy>(TASK_ACTOR_TYPE.get());
      // Game Event Task Actor
      mActorFactory->RegisterType<TaskActorGameEventProxy>(GAME_EVENT_TASK_ACTOR_TYPE.get());
      // Rollup Task Actor - Parent is generic task actor.
      mActorFactory->RegisterType<TaskActorRollupProxy>(ROLL_UP_TASK_ACTOR_TYPE.get());
      // Ordered Task Actor - Parent is generic task actor.
      mActorFactory->RegisterType<TaskActorOrderedProxy>(ORDERED_TASK_ACTOR_TYPE.get());
      // Infinite light...
      mActorFactory->RegisterType<InfiniteLightActorProxy>(INFINITE_LIGHT_ACTOR_TYPE.get());
      // Positional light...
      mActorFactory->RegisterType<PositionalLightActorProxy>(POSITIONAL_LIGHT_ACTOR_TYPE.get());
      // Spotlight...
      mActorFactory->RegisterType<SpotlightActorProxy>(SPOT_LIGHT_ACTOR_TYPE.get());
      // Particle Systems...
      mActorFactory->RegisterType<ParticleSystemActor>(PARTICLE_SYSTEM_ACTOR_TYPE.get());
      mActorFactory->RegisterType<DynamicParticleSystemActor>(DYNAMIC_PARTICLE_SYSTEM_ACTOR_TYPE.get());
      // Infinite terrain...
      mActorFactory->RegisterType<InfiniteTerrainActorProxy>(INFINITE_TERRAIN_ACTOR_TYPE.get());
      // Terrain mesh actor...
      mActorFactory->RegisterType<MeshTerrainActor>(MESH_TERRAIN_ACTOR_TYPE.get());
      // Skybox actor...
      mActorFactory->RegisterType<SkyBoxActorProxy>(SKY_BOX_ACTOR_TYPE.get());
      // Skybox actor...
      mActorFactory->RegisterType<SkyDomeActorProxy>(SKY_DOME_ACTOR_TYPE.get());
      // BezierNode actor
      mActorFactory->RegisterType<BezierNodeActorProxy>(BEZIER_NODE_ACTOR_TYPE.get());
      // BezierControlPoint actor
      mActorFactory->RegisterType<BezierControlPointActorProxy>(BEZIER_CONTROL_POINT_ACTOR_TYPE.get());
      // BezierController actor
      mActorFactory->RegisterType<BezierControllerActor>(BEZIER_CONTROLLER_ACTOR_TYPE.get());
      // CloudPlane actor...
      mActorFactory->RegisterType<CloudPlaneActorProxy>(CLOUD_PLANE_ACTOR_TYPE.get());
      // CloudDome Actor
      mActorFactory->RegisterType<CloudDomeActorProxy>(CLOUD_DOME_ACTOR_TYPE.get());
      mActorFactory->RegisterType<TriggerActorProxy>(TRIGGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<ProximityTriggerActorProxy>(PROXIMITY_TRIGGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<AutoTriggerActorProxy>(AUTOTRIGGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CameraActorProxy>(CAMERA_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CameraDataActorProxy>(CAMERA_DATA_ACTOR_TYPE.get());
      mActorFactory->RegisterType<TripodActorProxy>(TRIPOD_ACTOR_TYPE.get());
      mActorFactory->RegisterType<WeatherEnvironmentActorProxy>(WEATHER_ENVIRONMENT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SkyDomeEnvironmentActorProxy>(SKYDOME_ENVIRONMENT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CoordinateConfigActorProxy>(COORDINATE_CONFIG_ACTOR_TYPE.get());
      mActorFactory->RegisterType<PlayerStartActorProxy>(PLAYER_START_ACTOR_TYPE.get());
      mActorFactory->RegisterType<WaypointVolumeActorProxy>(WAYPOINT_VOLUME_ACTOR_TYPE.get());
      mActorFactory->RegisterType<LabelActorProxy>(LABEL_ACTOR_TYPE.get());
      mActorFactory->RegisterType<PrefabActor>(PREFAB_ACTOR_TYPE.get());
      mActorFactory->RegisterType<VolumeEditActorProxy>(VOLUME_EDIT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<LinkedPointsActorProxy>(LINKED_POINTS_ACTOR_TYPE.get());
      mActorFactory->RegisterType<BackdropActorProxy>(BACKDROP_ACTOR_TYPE.get());
      mActorFactory->RegisterType<FenceActorProxy>(FENCE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<BuildingActorProxy>(BUILDING_ACTOR_TYPE.get());
      mActorFactory->RegisterType<WaterGridActorProxy>(WATER_GRID_ACTOR_TYPE.get());
      mActorFactory->RegisterType<TriggerVolumeActorProxy>(TRIGGER_VOLUME_ACTOR_TYPE.get());
      mActorFactory->RegisterType<DirectorActor>(DIRECTOR_ACTOR_TYPE.get());
      mActorFactory->RegisterType<PositionDataActorProxy>(POSITION_DATA_ACTOR_TYPE.get());

      // Base Game Mesh actor - typically subclassed (maybe shouldn't even be registered)
      mActorFactory->RegisterType<GameMeshActor>(GAME_MESH_ACTOR_TYPE.get());

      mActorFactory->RegisterType<DistanceSensorActorProxy>(DISTANCE_SENSOR_ACTOR_TYPE.get());

      mActorFactory->RegisterType<dtGame::DeadReckoningComponent>();
      mActorFactory->RegisterType<dtGame::DefaultNetworkPublishingComponent>();
      mActorFactory->RegisterType<dtGame::DefaultMessageProcessor>();
      mActorFactory->RegisterType<dtGame::LogController>();
      //mActorFactory->RegisterType<dtGame::ServerLoggerComponent>();
      mActorFactory->RegisterType<dtGame::TaskComponent>();

      mActorFactory->RegisterType<dtGame::DeadReckoningActorComponent>();
      mActorFactory->RegisterType<dtGame::ShaderActorComponent>();
      mActorFactory->RegisterType<dtGame::DRPublishingActComp>();
      mActorFactory->RegisterType<dtGame::CascadingDeleteActorComponent>();

      mActorFactory->RegisterType<dtActors::DynamicParticlesActorComponent>();
      mActorFactory->RegisterType<dtActors::BezierControllerActorComponent>();

   }

   //////////////////////////////////////////////////////////////////////////
   void EngineActorRegistry::GetReplacementActorTypes(dtCore::ActorPluginRegistry::ActorTypeReplacements &replacements) const
   {
      replacements.push_back(std::make_pair(ENVIRONMENT_ACTOR_TYPE->GetFullName(), WEATHER_ENVIRONMENT_ACTOR_TYPE->GetFullName()));
      replacements.push_back(std::make_pair(ENV_ACTOR_TYPE->GetFullName(), SKY_DOME_ACTOR_TYPE->GetFullName()));
      replacements.push_back(std::make_pair(STATIC_MESH_ACTOR_TYPE->GetFullName(), GAME_MESH_ACTOR_TYPE->GetFullName()));
   }
} // namespace dtActors
