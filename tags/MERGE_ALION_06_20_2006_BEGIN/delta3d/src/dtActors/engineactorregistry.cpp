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
 * @author Matthew W. Campbell
 * @author Chris Osborn
*/
#include "dtActors/engineactorregistry.h"
#include "dtActors/infinitelightactorproxy.h"
#include "dtActors/deltaobjectactorproxy.h"
#include "dtActors/staticmeshactorproxy.h"
#include "dtActors/particlesystemactorproxy.h"
#include "dtActors/positionallightactorproxy.h"
#include "dtActors/spotlightactorproxy.h"
#include "dtActors/characteractorproxy.h"
#include "dtActors/infiniteterrainactorproxy.h"
#include "dtActors/autotriggeractorproxy.h"
#include "dtActors/beziercontrolleractorproxy.h"
#include "dtActors/meshterrainactorproxy.h"
#include "dtActors/clouddomeactorproxy.h"
#include "dtActors/cloudplaneactorproxy.h"
#include "dtActors/skyboxactorproxy.h"
#include "dtActors/skydomeactorproxy.h"
#include "dtActors/beziernodeactorproxy.h"
#include "dtActors/beziercontrolpointactorproxy.h"
#include "dtActors/triggeractorproxy.h"
#include "dtActors/proximitytriggeractorproxy.h"
#include "dtActors/cameraactorproxy.h"
#include "dtActors/tripodactorproxy.h"
#include "dtActors/taskactor.h"
#include "dtActors/taskactorgameevent.h"
#include "dtActors/taskactorrollup.h"
#include "dtActors/taskactorordered.h"
#include "dtActors/basicenvironmentactorproxy.h"
#include "dtActors/coordinateconfigactorproxy.h"

namespace dtActors
{
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::TASK_ACTOR_TYPE(new dtDAL::ActorType("Task Actor", "dtcore.Tasks","Generic task actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE(new dtDAL::ActorType("GameEvent Task Actor", "dtcore.Tasks","Game event task actor.", TASK_ACTOR_TYPE.get()));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::ROLL_UP_TASK_ACTOR_TYPE(new dtDAL::ActorType("Rollup Task Actor", "dtcore.Tasks", "Rollup or container based task actor.", TASK_ACTOR_TYPE.get()));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::ORDERED_TASK_ACTOR_TYPE(new dtDAL::ActorType("Ordered Task Actor", "dtcore.Tasks", "Ordered task actor similar to a roll up task actor but with a constraint on the order in which sub tasks may be completed.", TASK_ACTOR_TYPE.get()));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::INFINITE_LIGHT_ACTOR_TYPE(new dtDAL::ActorType("Infinite Light", "dtcore.Lights","Directional light actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::POSITIONAL_LIGHT_ACTOR_TYPE(new dtDAL::ActorType("Positional Light", "dtcore.Lights", "Positional light actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::SPOT_LIGHT_ACTOR_TYPE(new dtDAL::ActorType("Spotlight", "dtcore.Lights", "Spotlight light actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::STATIC_MESH_ACTOR_TYPE(new dtDAL::ActorType("Static Mesh", "dtcore", "Loadable static mesh actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::PARTICLE_SYSTEM_ACTOR_TYPE(new dtDAL::ActorType("Particle System", "dtcore", "dtCore::ParticleSystem actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::CHARACTER_ACTOR_TYPE(new dtDAL::ActorType("Character", "dtcore", "dtChar::Character actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::INFINITE_TERRAIN_ACTOR_TYPE(new dtDAL::ActorType("Infinite Terrain", "dtcore.Terrain", "dtCore::InfiniteTerrain actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::MESH_TERRAIN_ACTOR_TYPE(new dtDAL::ActorType("Mesh Terrain", "dtcore.Terrain", "An terrain actor who's geometry is represented via a mesh file."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::SKY_BOX_ACTOR_TYPE(new dtDAL::ActorType("Sky Box", "dtcore.Environment", "dtCore::SkyBox Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::SKY_DOME_ACTOR_TYPE(new dtDAL::ActorType("Sky Dome", "dtcore.Environment", "dtCore::SkyDome Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::BEZIER_NODE_ACTOR_TYPE(new dtDAL::ActorType("Bezier Node", "dtcore.Curve", "dtABC::BezierNode Actor"));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::BEZIER_CONTROL_POINT_ACTOR_TYPE(new dtDAL::ActorType("Bezier Control Point", "dtcore.Curve", "dtABC::BezierControlPoint Actor"));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::BEZIER_CONTROLLER_ACTOR_TYPE(new dtDAL::ActorType("Bezier Controller", "dtcore.Action", "dtABC::BezierController Actor"));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::CLOUD_PLANE_ACTOR_TYPE(new dtDAL::ActorType("Cloud Plane", "dtcore.Environment", "dtCore::CloudPlane Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::TRIGGER_ACTOR_TYPE(new dtDAL::ActorType("Trigger", "dtcore.Triggers", "dtABC::Trigger Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::PROXIMITY_TRIGGER_ACTOR_TYPE(new dtDAL::ActorType("Proximity Trigger","dtcore.Triggers", "dtABC::ProximityTrigger Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::AUTOTRIGGER_ACTOR_TYPE(new dtDAL::ActorType("AutoTrigger", "dtcore.Triggers", "dtABC::AutoTrigger Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::CAMERA_ACTOR_TYPE(new dtDAL::ActorType("Camera", "dtcore", "dtCore::Camera Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::TRIPOD_ACTOR_TYPE(new dtDAL::ActorType("Tripod", "dtcore", "dtCore::Tripod Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::ENVIRONMENT_ACTOR_TYPE(new dtDAL::ActorType("Environment", "dtcore.Environment", "dtCore::BasicEnvironment Actor."));
   dtCore::RefPtr<dtDAL::ActorType> EngineActorRegistry::COORDINATE_CONFIG_ACTOR_TYPE(new dtDAL::ActorType("Coordinate Config", "dtutil", "dtUtil::Coordinates Actor"));

   extern "C" DT_PLUGIN_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new EngineActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_PLUGIN_EXPORT void DestroyPluginRegistry(
       dtDAL::ActorPluginRegistry *registry)
   {
      if (registry != NULL)
         delete registry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   EngineActorRegistry::EngineActorRegistry() : dtDAL::ActorPluginRegistry("CoreActors")
   {
      mDescription = "The actors in this registry are the default base "
         "actors provided by the Delta3D engine.";
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EngineActorRegistry::RegisterActorTypes()
   {

      //In this method we will add each of the actor types the engine actor
      //registry supports to the object factory.  These will then get
      //registered with the LibraryManager.

      //Generic Task Actor
      mActorFactory->RegisterType<TaskActorProxy>(TASK_ACTOR_TYPE.get());
      // Game Event Task Actor
      mActorFactory->RegisterType<TaskActorGameEventProxy>(GAME_EVENT_TASK_ACTOR_TYPE.get());
      //Rollup Task Actor - Parent is generic task actor.
      mActorFactory->RegisterType<TaskActorRollupProxy>(ROLL_UP_TASK_ACTOR_TYPE.get());
      //Ordered Task Actor - Parent is generic task actor.
      mActorFactory->RegisterType<TaskActorOrderedProxy>(ORDERED_TASK_ACTOR_TYPE.get());
      //Infinite light...
      mActorFactory->RegisterType<InfiniteLightActorProxy>(INFINITE_LIGHT_ACTOR_TYPE.get());
      //Positional light...
      mActorFactory->RegisterType<PositionalLightActorProxy>(POSITIONAL_LIGHT_ACTOR_TYPE.get());
      //Spotlight...
      mActorFactory->RegisterType<SpotlightActorProxy>(SPOT_LIGHT_ACTOR_TYPE.get());
      //Static Mesh...
      mActorFactory->RegisterType<StaticMeshActorProxy>(STATIC_MESH_ACTOR_TYPE.get());
      //Particle System...
      mActorFactory->RegisterType<ParticleSystemActorProxy>(PARTICLE_SYSTEM_ACTOR_TYPE.get());
      //Animated character...
      mActorFactory->RegisterType<CharacterActorProxy>(CHARACTER_ACTOR_TYPE.get());
      //Infinite terrain...
      mActorFactory->RegisterType<InfiniteTerrainActorProxy>(INFINITE_TERRAIN_ACTOR_TYPE.get());
      //Terrain mesh actor...
      mActorFactory->RegisterType<MeshTerrainActorProxy>(MESH_TERRAIN_ACTOR_TYPE.get());
      //Skybox actor...
      mActorFactory->RegisterType<SkyBoxActorProxy>(SKY_BOX_ACTOR_TYPE.get());
      //Skybox actor...
      mActorFactory->RegisterType<SkyDomeActorProxy>(SKY_DOME_ACTOR_TYPE.get());
      //BezierNode actor
      mActorFactory->RegisterType<BezierNodeActorProxy>(BEZIER_NODE_ACTOR_TYPE.get());
      //BezierControlPoint actor
      mActorFactory->RegisterType<BezierControlPointActorProxy>(BEZIER_CONTROL_POINT_ACTOR_TYPE.get());
      //BezierController actor
      mActorFactory->RegisterType<BezierControllerActorProxy>(BEZIER_CONTROLLER_ACTOR_TYPE.get());
      //CloudPlane actor...
      mActorFactory->RegisterType<CloudPlaneActorProxy>(CLOUD_PLANE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<TriggerActorProxy>(TRIGGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<ProximityTriggerActorProxy>(PROXIMITY_TRIGGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<AutoTriggerActorProxy>(AUTOTRIGGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CameraActorProxy>(CAMERA_ACTOR_TYPE.get());
      mActorFactory->RegisterType<TripodActorProxy>(TRIPOD_ACTOR_TYPE.get());
      mActorFactory->RegisterType<BasicEnvironmentActorProxy>(ENVIRONMENT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CoordinateConfigActorProxy>(COORDINATE_CONFIG_ACTOR_TYPE.get());
   }

}
