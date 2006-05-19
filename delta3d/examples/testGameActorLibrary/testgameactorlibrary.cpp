/* -*-c++-*-
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
#include "testhlaobject.h"
#include "testplayer.h"
#include "countertaskactor.h"
#include "testgameenvironmentactor.h"
#include <dtCore/scene.h>
#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>

dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST1_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Test1Actor", "ExampleActors", "These are example actors"));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST2_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Test2Actor", "ExampleActors", "These are example actors"));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_PLAYER_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("TestPlayer","ExampleActors", "Simple player actor."));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_TASK_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Task Actor","dtcore.Tasks"));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_COUNTER_TASK_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Counter Task","ExampleActors", "task actor that provides a simple counter of something happening.", TEST_TASK_GAME_ACTOR_PROXY_TYPE.get()));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_TANK_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Tank", "TestHLA", "These are test HLA mapping actors"));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_JET_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Jet", "TestHLA", "These are test HLA mapping actors"));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_HELICOPTER_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("Helicopter", "TestHLA", "These are test HLA mapping actors"));
dtCore::RefPtr<dtDAL::ActorType> TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE(new dtDAL::ActorType("TestEnvironmentActor", "ExampleActors", "These are example actors"));


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
   mActorFactory->RegisterType<TestGameActorProxy1> (TEST1_GAME_ACTOR_PROXY_TYPE.get());
   mActorFactory->RegisterType<TestGameActorProxy2> (TEST2_GAME_ACTOR_PROXY_TYPE.get());      
   mActorFactory->RegisterType<TestPlayerProxy>(TEST_PLAYER_GAME_ACTOR_PROXY_TYPE.get());
   
   //This is the actor type for the task actor located in dtActors.  All custom
   //subclasses should at the very least be a sub actor type of this one.
   mActorFactory->RegisterType<CounterTaskActorProxy>(TEST_COUNTER_TASK_GAME_ACTOR_PROXY_TYPE.get());
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_TANK_GAME_ACTOR_PROXY_TYPE.get());      
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_JET_GAME_ACTOR_PROXY_TYPE.get());      
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_HELICOPTER_GAME_ACTOR_PROXY_TYPE.get());      
   mActorFactory->RegisterType<TestGameEnvironmentActorProxy> (TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE.get());
}
