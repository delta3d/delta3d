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
 * @author William E. Johnson II
 */
#include "testgameactorlibrary.h"
#include "testgameactor.h"
#include "testgameactor2.h"
#include "testplayer.h"
#include "countertaskactor.h"
#include <dtCore/scene.h>
#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>

extern "C" DT_EXAMPLE_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
{
   return new TestGameActorLibrary;
}

extern "C" DT_EXAMPLE_EXPORT void DestroyPluginRegistry(dtDAL::ActorPluginRegistry* registry)
{
   if (registry != NULL)
      delete registry;
}

TestGameActorLibrary::TestGameActorLibrary() : dtDAL::ActorPluginRegistry("TestGameActorLibrary")
{

}

void TestGameActorLibrary::RegisterActorTypes()
{
   dtDAL::ActorType *test1 = new dtDAL::ActorType("Test1Actor", "ExampleActors", 
      "These are example actors");   
   mActorFactory->RegisterType<TestGameActorProxy1> (test1);

   dtDAL::ActorType *test2 = new dtDAL::ActorType("Test2Actor", "ExampleActors", 
      "These are example actors");
   mActorFactory->RegisterType<TestGameActorProxy2> (test2);      
   
   dtDAL::ActorType *player = new dtDAL::ActorType("TestPlayer","ExampleActors",
      "Simple player actor.");
   mActorFactory->RegisterType<TestPlayerProxy>(player);
   
   //This is the actor type for the task actor located in dtActors.  All custom
   //subclasses should at the very least be a sub actor type of this one.
   const dtDAL::ActorType *taskParent = new dtDAL::ActorType("Task Actor","dtcore.Tasks");
   
   dtDAL::ActorType *counterTask = new dtDAL::ActorType("Counter Task","ExampleActors","Example "
      "task actor that provides a simple counter of something happening.",taskParent);
   mActorFactory->RegisterType<CounterTaskActorProxy>(counterTask);

   dtDAL::ActorType *hla1 = new dtDAL::ActorType("Tank", "TestHLA", 
      "These are test HLA mapping actors");
   mActorFactory->RegisterType<TestGameActorProxy2> (hla1);      

   dtDAL::ActorType *hla2 = new dtDAL::ActorType("Jet", "TestHLA", 
      "These are test HLA mapping actors");
   mActorFactory->RegisterType<TestGameActorProxy2> (hla2);      

   dtDAL::ActorType *hla3 = new dtDAL::ActorType("Helicopter", "TestHLA", 
      "These are test HLA mapping actors");
   mActorFactory->RegisterType<TestGameActorProxy2> (hla3);      

}
