/* -*-c++-*-
* testGameActorLibrary - This source file (.h & .cpp) - Using 'The MIT License'
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* William E. Johnson II
*/
#include "testgameactorlibrary.h"
#include "testgameactor.h"
#include "testgameactor2.h"
#include "testgameactorcrash.h"
#include "testhlaobject.h"
#include "testplayer.h"
#include "countertaskactor.h"
#include "testgameenvironmentactor.h"
#include "testgamepropertyactor.h"
#include <dtCore/scene.h>
#include <dtUtil/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>
#include <dtCore/actorfactory.h>

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Test1Actor", "ExampleActors", "These are example actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST2_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Test2Actor", "ExampleActors", "These are example actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_GAME_ACTOR_CRASH_TYPE(
      new dtCore::ActorType("TestCrash","ExampleActors", "An actor that throws an exception as it enters the world."));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_PLAYER_GAME_ACTOR_TYPE(
      new dtCore::ActorType("TestPlayer","ExampleActors", "Simple player actor."));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_TASK_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Task Actor","dtcore.Tasks"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_COUNTER_TASK_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Counter Task","ExampleActors", ""
            "task actor that provides a simple counter of something happening.", TEST_TASK_GAME_ACTOR_TYPE.get()));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_TANK_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Tank", "TestHLA", "These are test HLA mapping actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_JET_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Jet", "TestHLA", "These are test HLA mapping actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_CULTURAL_FEATURE_GAME_ACTOR_TYPE(
      new dtCore::ActorType("CulturalFeature", "TestHLA", "These are test HLA mapping actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_SENSOR_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Sensor", "TestHLA", "These are test HLA mapping actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_HELICOPTER_GAME_ACTOR_TYPE(
      new dtCore::ActorType("Helicopter", "TestHLA", "These are test HLA mapping actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_TYPE(
      new dtCore::ActorType("TestEnvironmentActor", "ExampleActors", "These are example actors"));

dtCore::RefPtr<dtCore::ActorType> TestGameActorLibrary::TEST_GAME_PROPERTY_TYPE(
      new dtCore::ActorType("TestGamePropertyActor", "ExampleActors", "Has an example of most property types"));


extern "C" DT_EXAMPLE_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
{
   return new TestGameActorLibrary;
}

extern "C" DT_EXAMPLE_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
{
   if (registry != NULL)
      delete registry;
}

TestGameActorLibrary::TestGameActorLibrary() : dtCore::ActorPluginRegistry("TestGameActorLibrary")
{

}

void TestGameActorLibrary::RegisterActorTypes()
{
   mActorFactory->RegisterType<TestGameActor1> (TEST1_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestGameActorProxy2> (TEST2_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestGameActorCrashProxy> (TEST_GAME_ACTOR_CRASH_TYPE.get());
   mActorFactory->RegisterType<TestPlayerProxy>(TEST_PLAYER_GAME_ACTOR_TYPE.get());
   
   //This is the actor type for the task actor located in dtActors.  All custom
   //subclasses should at the very least be a sub actor type of this one.
   mActorFactory->RegisterType<CounterTaskActorProxy>(TEST_COUNTER_TASK_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_TANK_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_JET_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_CULTURAL_FEATURE_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_SENSOR_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestHLAObjectProxy> (TEST_HELICOPTER_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestGameEnvironmentActorProxy> (TEST_ENVIRONMENT_GAME_ACTOR_TYPE.get());
   mActorFactory->RegisterType<TestGamePropertyActor> (TEST_GAME_PROPERTY_TYPE.get());
      
   mActorFactory->RegisterType<TestActorComponent1>(TestActorComponent1::TYPE.get());
   mActorFactory->RegisterType<TestActorComponent2>(TestActorComponent2::TYPE.get());
}
