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
#include <dtRender/scenemanager.h>
#include <dtRender/oceanscene.h>
#include <dtRender/guiscene.h>
#include <dtRender/ephemerisscene.h>
#include <dtRender/simplescene.h>
#include <dtRender/ppuscene.h>
#include <dtRender/hdrscene.h>
#include <dtRender/shadowscene.h>
#include <dtRender/multipassscene.h>

// For the autoreg
#include <dtCore/librarymanager.h> 

namespace dtRender
{
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

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::PPU_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("PPU Scene", "dtRender", "This actor is required to do osgPPU effects.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SCENE_MANAGER_ACTOR_TYPE(new dtCore::ActorType
      ("Scene Manager", "dtRender", "This actor is used for managing scene structure.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SHADOW_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Shadow Scene", "dtRender", "This actor applies different types of shadow techniques to the child geometry.")); 

   dtCore::RefPtr<dtCore::ActorType> RenderActorRegistry::SIMPLE_SCENE_ACTOR_TYPE(new dtCore::ActorType
      ("Simple Scene", "dtRender", "This actor represents a bare minimum scene, useful if you only need to add a camera.")); 


   
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
      mActorFactory->RegisterType<EphemerisSceneProxy>(EPHEMERIS_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<GUISceneProxy>(GUI_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<HDRSceneProxy>(HDR_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<MultipassSceneProxy>(MULTIPASS_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<OceanSceneProxy>(OCEAN_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SceneManagerProxy>(SCENE_MANAGER_ACTOR_TYPE.get());
      mActorFactory->RegisterType<ShadowSceneProxy>(SHADOW_SCENE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<SimpleSceneProxy>(SIMPLE_SCENE_ACTOR_TYPE.get());
   }
}
