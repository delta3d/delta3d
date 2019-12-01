/* -*-c++-*-
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
#include <dtRender/dtrenderactorregistry.h>
#include <dtRender/dynamiclight.h>
#include <dtRender/lightscene.h>
#include <dtRender/scenemanager.h>
#include <dtRender/oceanscene.h>
#include <dtRender/guiscene.h>
#include <dtRender/ephemerisscene.h>
#include <dtRender/simplescene.h>
#include <dtRender/ppuscene.h>
#include <dtRender/hdrscene.h>
#include <dtRender/shadowscene.h>
#include <dtRender/multipassscene.h>
#include <dtRender/dofscene.h>
#include <dtRender/cubemapscene.h>
#include <dtRender/ssaoscene.h>
#include <dtRender/videoscene.h>
#include <dtRender/atmospherescene.h>

#include <dtRender/uniformactcomp.h>
#include <dtRender/optimizeractcomp.h>


// For the autoreg
#include <dtCore/actorfactory.h> 

namespace dtRender
{
   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::ATMOSPHERE_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Atmosphere Scene", "dtRender", "This actor creates a post process fog effect."));

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::DOF_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("DOF Scene", "dtRender", "This actor creates a depth of field post process effect.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::DYNAMIC_LIGHT_ACTOR_TYPE(new dtCore::ActorType
      ("Dynamic Light", "dtRender", "This actor creates a light which is simulated in the LightScene.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::EPHEMERIS_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Ephemeris Scene", "dtRender", "This actor creates an osgEphemeris sky dome.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::GUI_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("GUI Scene", "dtRender", "This actor renders stuff on top of the main render pass.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::HDR_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("HDR Scene", "dtRender", "This actor uses osgPPU to accumulate hdr light values and apply tone mapping.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::MULTIPASS_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Multipass Scene", "dtRender", "This actor enables multipass effects.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::OCEAN_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Ocean Scene", "dtRender", "This actor creates a large water mesh.")); 


   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::LIGHT_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Light Scene", "dtRender", "This actor manages dynamic light actors.")); 


   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::PPU_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("PPU Scene", "dtRender", "This the base actor for osgPPU effects.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::CUBEMAP_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Cube Map Scene", "dtRender", "This actor creates a cubemap render target.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SCENE_MANAGER_ACTOR_TYPE(new dtCore::ActorType
      ("Scene Manager", "dtRender", "This actor is used for managing scene structure.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SHADOW_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Shadow Scene", "dtRender", "This actor applies different types of shadow techniques to the child geometry.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Simple Scene", "dtRender", "This actor represents a bare minimum scene, useful if you only need to add a camera.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SPOT_LIGHT_ACTOR_TYPE(new dtCore::ActorType
      ("Spotlight", "dtRender", "This actor creates a runtime spotlight.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SSAO_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("SSAO Scene", "dtRender", "This actor creates a Screen Space Ambient Occlusion post process effect.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::VIDEO_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Video Scene", "dtRender", "This actor renders a video to a texture.")); 


   // Must be after the types or it will crash.
   //dtCore::AutoLibraryRegister<RenderActorRegistry> gAutoReg("dtRender");

   ////////////////////////////////////////////////////////////////////////////
   extern "C" DT_RENDER_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new dtRender::RenderActorRegistry;
   }

   ////////////////////////////////////////////////////////////////////////////
   extern "C" DT_RENDER_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
   {
      delete registry;
   }

   ////////////////////////////////////////////////////////////////////////////
   RenderActorRegistry::RenderActorRegistry() : dtCore::ActorPluginRegistry("dtRender", "This library contains actors related to rendering effects.")
   {
      
   }

   ////////////////////////////////////////////////////////////////////////////
   void RenderActorRegistry::RegisterActorTypes()
   {
      mActorFactory->RegisterType<AtmosphereSceneActor>(ATMOSPHERE_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CubeMapSceneActor>(CUBEMAP_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<DynamicLight>(DYNAMIC_LIGHT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<DOFSceneActor>(DOF_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<EphemerisSceneActor>(EPHEMERIS_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<GUISceneActor>(GUI_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<HDRSceneActor>(HDR_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<LightSceneActor>(LIGHT_SCENE_ACTOR_TYPE.get());
      
      mActorFactory->RegisterType<MultipassSceneActor>(MULTIPASS_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<OceanSceneActor>(OCEAN_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SceneManagerActor>(SCENE_MANAGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<ShadowSceneActor>(SHADOW_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SimpleSceneActor>(SIMPLE_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SpotLight>(SPOT_LIGHT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SSAOSceneActor>(SSAO_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<VideoSceneActor>(VIDEO_SCENE_ACTOR_TYPE.get());

      mActorFactory->RegisterType<OptimizerActComp>();
      mActorFactory->RegisterType<UniformActComp>();

   }
}
