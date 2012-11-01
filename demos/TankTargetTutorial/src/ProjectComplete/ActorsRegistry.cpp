/* -*-c++-*-
 * TutorialLibrary - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */

#include "ActorsRegistry.h"
#include "TankActor.h"
#include "KillableTargetActor.h"
#include <dtCore/shadermanager.h>

dtCore::RefPtr<dtCore::ActorType> ActorsRegistry::TANK_ACTOR_TYPE(new dtCore::ActorType("Tank","MyActors.Tanks","A really cool tank!."));
dtCore::RefPtr<dtCore::ActorType> ActorsRegistry::KILLABLE_ACTOR_TYPE(new dtCore::ActorType("KillableTarget", "MyActors.Targets", "Would wouldn't want to shoot this thing?"));

///////////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new ActorsRegistry();
}

///////////////////////////////////////////////////////////////////////////////
extern "C" TUTORIAL_TANK_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry *registry)
{
   if (registry != NULL)
   {
      delete registry;
   }
}

//////////////////////////////////////////////////////////////////////////
ActorsRegistry::ActorsRegistry() :
   dtCore::ActorPluginRegistry("TutorialActors")
{
   SetDescription("This is a library of actors used by the Game Manager Tutorials.");

   dtCore::ShaderManager::GetInstance().LoadShaderDefinitions("Shaders/TutorialShaderDefs.xml", false);
}

//////////////////////////////////////////////////////////////////////////
void ActorsRegistry::RegisterActorTypes()
{
   mActorFactory->RegisterType<TankActorProxy>(TANK_ACTOR_TYPE.get());
   mActorFactory->RegisterType<KillableTargetActorProxy>(KILLABLE_ACTOR_TYPE.get());
}
