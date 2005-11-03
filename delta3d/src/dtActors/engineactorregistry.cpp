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
#include <dtActors/engineactorregistry.h>
#include <dtActors/infinitelightactorproxy.h>
#include <dtActors/deltaobjectactorproxy.h>
#include <dtActors/staticmeshactorproxy.h>
#include <dtActors/particlesystemactorproxy.h>
#include <dtActors/positionallightactorproxy.h>
#include <dtActors/spotlightactorproxy.h>
#include <dtActors/characteractorproxy.h>
#include <dtActors/infiniteterrainactorproxy.h>
#include <dtActors/environmentactorproxy.h>
#include <dtActors/triggeractorproxy.h>
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

namespace dtActors
{
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

        //Infinite light...
        dtDAL::ActorType *infLightActor = new dtDAL::ActorType("Infinite Light",
            "dtcore.Lights","Directional light actor.");
        mActorFactory->RegisterType<InfiniteLightActorProxy>(infLightActor);

        //Positional light...
        dtDAL::ActorType *posLightActor = new dtDAL::ActorType("Positional Light",
            "dtcore.Lights","Positional light actor.");
        mActorFactory->RegisterType<PositionalLightActorProxy>(posLightActor);

        //Spotlight...
        dtDAL::ActorType *spotLightActor = new dtDAL::ActorType("Spotlight",
           "dtcore.Lights","Spotlight light actor.");
        mActorFactory->RegisterType<SpotlightActorProxy>(spotLightActor);

        //Static Mesh...
        dtDAL::ActorType *staticMeshActor = new dtDAL::ActorType("Static Mesh",
            "dtcore", "Loadable static mesh actor.");
        mActorFactory->RegisterType<StaticMeshActorProxy>(staticMeshActor);

        //Particle System...
        dtDAL::ActorType *particleActor = new dtDAL::ActorType("Particle System",
            "dtcore", "dtCore::ParticleSystem actor.");
        mActorFactory->RegisterType<ParticleSystemActorProxy>(particleActor);

        //Animated character...
        dtDAL::ActorType *characterActor = new dtDAL::ActorType("Character",
            "dtcore", "dtChar::Character actor.");
        mActorFactory->RegisterType<CharacterActorProxy>(characterActor);

        //Infinite terrain...
        dtDAL::ActorType *infTerrActor = new dtDAL::ActorType("Infinite Terrain",
            "dtcore.Terrain", "dtCore::InfiniteTerrain actor.");
        mActorFactory->RegisterType<InfiniteTerrainActorProxy>(infTerrActor);

        //Terrain mesh actor...
        dtDAL::ActorType *meshActor = new dtDAL::ActorType("Mesh Terrain",
            "dtcore.Terrain", "An terrain actor who's geometry is represeted via a mesh file.");
        mActorFactory->RegisterType<MeshTerrainActorProxy>(meshActor);

        //Skybox actor...
        dtDAL::ActorType *sBoxActor = new dtDAL::ActorType("Sky Box",
            "dtcore.Environment", "dtCore::SkyBox Actor.");
        mActorFactory->RegisterType<SkyBoxActorProxy>(sBoxActor);

        //Skybox actor...
        dtDAL::ActorType *sDomeActor = new dtDAL::ActorType("Sky Dome",
            "dtcore.Environment", "dtCore::SkyDome Actor.");
        mActorFactory->RegisterType<SkyDomeActorProxy>(sDomeActor);

        //BezierNode actor
        dtDAL::ActorType *bNodeActor = new dtDAL::ActorType("Bezier Node", 
           "dtcore.BezierNode", "dtCore::BezierNode Actor");
        mActorFactory->RegisterType<BezierNodeActorProxy>(bNodeActor);

        //BezierControlPoint actor
        dtDAL::ActorType *bCtrlPntActor = new dtDAL::ActorType("Bezier Control Point", 
           "dtcore.BezierControlPoint", "dtCore::BezierControlPoint Actor");
        mActorFactory->RegisterType<BezierControlPointActorProxy>(bCtrlPntActor);

        //CloudPlane actor...
        dtDAL::ActorType *cPlaneActor = new dtDAL::ActorType("Cloud Plane",
            "dtcore.Environment", "dtCore::CloudPlane Actor.");
        mActorFactory->RegisterType<CloudPlaneActorProxy>(cPlaneActor);

        dtDAL::ActorType* triggerActor = new dtDAL::ActorType("Trigger",
           "dtcore.Triggers", "dtABC::Trigger Actor."); 
        mActorFactory->RegisterType<TriggerActorProxy>(triggerActor);

        dtDAL::ActorType* proxTriggerActor = new dtDAL::ActorType("Proximity Trigger",
           "dtcore.Triggers", "dtABC::ProximityTrigger Actor."); 
        mActorFactory->RegisterType<ProximityTriggerActorProxy>(proxTriggerActor);

        dtDAL::ActorType* cameraActor = new dtDAL::ActorType("Camera",
           "dtcore", "dtCore::Camera Actor.");
        mActorFactory->RegisterType<CameraActorProxy>(cameraActor);

    }
}
