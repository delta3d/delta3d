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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "testappactorregistry.h"
#include "terrainactorproxy.h"
#include <dtActors/engineactorregistry.h>
#include <dtCore/shadermanager.h>



using dtCore::RefPtr;

namespace dtExample
{
   ///////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ///////////////////////////////////////////////////////////////////////////
   RefPtr<dtCore::ActorType> TestAppActorRegistry::TERRAIN_ACTOR_TYPE(
      new dtCore::ActorType("Terrain", "dtExample", "This is an example terrain actor with physics."));
  


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
      dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/ShaderDefinitions.xml", false);
   }

   ///////////////////////////////////////////////////////////////////////////
   TestAppActorRegistry::~TestAppActorRegistry()
   {
   }

   ///////////////////////////////////////////////////////////////////////////
   void TestAppActorRegistry::RegisterActorTypes()
   {
      mActorFactory->RegisterType<TerrainActorProxy>(TERRAIN_ACTOR_TYPE.get());
   }
}
