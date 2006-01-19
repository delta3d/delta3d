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
#include <dtCore/scene.h>
#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>

extern "C" DT_EXAMPLE_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
{
   return new TestGameActorLibrary;
}

extern "C" DT_EXAMPLE_EXPORT void DestroyPluginRegistry(dtDAL::ActorPluginRegistry *registry)
{
   if(registry)
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
}
