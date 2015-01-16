/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "testappactorregistry.h"
#include "civilianactor.h"
#include "civilianaiactorcomponent.h"
#include "fireworkactor.h"
#include "lightactorcomponent.h"
#include "meshlampactor.h"
#include "meshobjectactor.h"
#include "terrainactor.h"
#include "vesselactor.h"
#include <dtActors/engineactorregistry.h>
#include <dtAnim/animactorregistry.h>
#include <dtCore/shadermanager.h>

using dtCore::RefPtr;



namespace dtExample
{
   ///////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ///////////////////////////////////////////////////////////////////////////
   RefPtr<dtCore::ActorType> TestAppActorRegistry::TERRAIN_ACTOR_TYPE(
      new dtCore::ActorType("Terrain", "dtExample", "This is an example terrain actor with physics."));
   RefPtr<dtCore::ActorType> TestAppActorRegistry::CIVILIAN_ACTOR_TYPE(
      new dtCore::ActorType("Civilian", "dtExample", "This is an example animated civilian with AI.", dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE));
   RefPtr<dtCore::ActorType> TestAppActorRegistry::FIREWORK_ACTOR_TYPE(
      new dtCore::ActorType("Firework", "dtExample", "This is an example timed particle system effect."));
   RefPtr<dtCore::ActorType> TestAppActorRegistry::MESH_OBJECT_ACTOR_TYPE(
      new dtCore::ActorType("Mesh Object", "dtExample", "Mesh object with automated physics.", dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE));
   RefPtr<dtCore::ActorType> TestAppActorRegistry::MESH_LAMP_ACTOR_TYPE(
      new dtCore::ActorType("Mesh Lamp", "dtExample", "Mesh object with automated physics AND light properties.", TestAppActorRegistry::MESH_OBJECT_ACTOR_TYPE));
   RefPtr<dtCore::ActorType> TestAppActorRegistry::VESSEL_ACTOR_TYPE(
      new dtCore::ActorType("Vessel", "dtExample", "Mesh object with automated physics AND surface vessel properties.", TestAppActorRegistry::MESH_OBJECT_ACTOR_TYPE));
  


   ///////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ///////////////////////////////////////////////////////////////////////////
   extern "C" TEST_APP_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
       return new TestAppActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////
   extern "C" TEST_APP_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
   {
       delete registry;
   }

   ///////////////////////////////////////////////////////////////////////////
   TestAppActorRegistry::TestAppActorRegistry()
      : BaseClass("This library will store some example actors")
   {
      dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/ShaderDefinitions.xml", true);
   }

   ///////////////////////////////////////////////////////////////////////////
   TestAppActorRegistry::~TestAppActorRegistry()
   {
   }

   ///////////////////////////////////////////////////////////////////////////
   void TestAppActorRegistry::RegisterActorTypes()
   {
      mActorFactory->RegisterType<TerrainActor>(TERRAIN_ACTOR_TYPE.get());
      mActorFactory->RegisterType<CivilianActor>(CIVILIAN_ACTOR_TYPE.get());
      mActorFactory->RegisterType<FireworkActor>(FIREWORK_ACTOR_TYPE.get());
      mActorFactory->RegisterType<MeshObjectActor>(MESH_OBJECT_ACTOR_TYPE.get());
      mActorFactory->RegisterType<MeshLampActor>(MESH_LAMP_ACTOR_TYPE.get());
      mActorFactory->RegisterType<VesselActor>(VESSEL_ACTOR_TYPE.get());

      mActorFactory->RegisterType<CivilianAIActorComponent>(CivilianAIActorComponent::TYPE.get());
      mActorFactory->RegisterType<LightActorComponent>(LightActorComponent::TYPE.get());
   }
}
