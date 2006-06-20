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

#include "testgameactor2.h"
#include <dtCore/scene.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/messagetype.h>

/////////////////////////////////////////////////////
// Actor Proxy code
/////////////////////////////////////////////////////
TestGameActorProxy2::TestGameActorProxy2()
{
   SetClassName("TestGameActor2");
}

TestGameActorProxy2::~TestGameActorProxy2()
{

}

void TestGameActorProxy2::BuildInvokables()
{
   GameActorProxy::BuildInvokables();

   AddInvokable(*new dtGame::Invokable("Test Message Listener", 
      dtDAL::MakeFunctor(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::LogMessage)));
   
}

void TestGameActorProxy2::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   AddProperty(new dtDAL::IntActorProperty("Actor Deleted Count", "The number of Actor Deleted message received", 
      dtDAL::MakeFunctor(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::SetActorDeletedCount), 
      dtDAL::MakeFunctorRet(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::GetActorDeletedCount), 
      "A property marking the number of actor deleted message received.", ""));

   AddProperty(new dtDAL::IntActorProperty("Actor Published Count", "The number of Actor Published message received", 
      dtDAL::MakeFunctor(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::SetActorPublishedCount), 
      dtDAL::MakeFunctorRet(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::GetActorPublishedCount), 
      "A property marking the number of actor published message received.", ""));

   AddProperty(new dtDAL::IntActorProperty("Map Loaded Count", "The number of Map Loaded message received", 
      dtDAL::MakeFunctor(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::SetMapLoadedCount), 
      dtDAL::MakeFunctorRet(static_cast<TestGameActor2&>(GetGameActor()), &TestGameActor2::GetMapLoadedCount), 
      "A property marking the number of Map Loaded message received.", ""));
}

void TestGameActorProxy2::CreateActor()
{
   mActor = new TestGameActor2(*this);
}

/////////////////////////////////////////////////////
// Actor code
/////////////////////////////////////////////////////
TestGameActor2::TestGameActor2(dtGame::GameActorProxy& proxy): dtGame::GameActor(proxy), mActorDeletedCount(0), mActorPublishedCount(0),
   mMapLoadedCount(0)
{

}

TestGameActor2::~TestGameActor2()
{

}

/////////////////////////////////////////////////////
void TestGameActor2::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_PUBLISHED)
   {
      mActorPublishedCount+=2;
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      mMapLoadedCount+=2;
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
   {
      mActorDeletedCount+=2;
   }
}

/////////////////////////////////////////////////////
void TestGameActor2::LogMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_PUBLISHED)
   {
      mActorPublishedCount++;
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      mMapLoadedCount++;
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
   {
      mActorDeletedCount++;
   }
}
