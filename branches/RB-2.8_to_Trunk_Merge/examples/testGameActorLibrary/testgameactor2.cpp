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
#include "testgameactor2.h"

#include <dtCore/scene.h>

#include <dtCore/intactorproperty.h>

#include <dtGame/invokable.h>
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
      dtUtil::MakeFunctor(&TestGameActor2::LogMessage, static_cast<TestGameActor2*>(GetDrawable()))));
   
}

void TestGameActorProxy2::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   TestGameActor2* actor = NULL;
   GetDrawable(actor);

   AddProperty(new dtCore::IntActorProperty("Actor Deleted Count", "The number of Actor Deleted message received", 
      dtCore::IntActorProperty::SetFuncType(actor, &TestGameActor2::SetActorDeletedCount),
      dtCore::IntActorProperty::GetFuncType(actor, &TestGameActor2::GetActorDeletedCount),
      "A property marking the number of actor deleted message received.", ""));

   AddProperty(new dtCore::IntActorProperty("Actor Published Count", "The number of Actor Published message received", 
      dtCore::IntActorProperty::SetFuncType(actor, &TestGameActor2::SetActorPublishedCount),
      dtCore::IntActorProperty::GetFuncType(actor, &TestGameActor2::GetActorPublishedCount),
      "A property marking the number of actor published message received.", ""));

   AddProperty(new dtCore::IntActorProperty("Map Loaded Count", "The number of Map Loaded message received", 
      dtCore::IntActorProperty::SetFuncType(actor, &TestGameActor2::SetMapLoadedCount),
      dtCore::IntActorProperty::GetFuncType(actor, &TestGameActor2::GetMapLoadedCount),
      "A property marking the number of Map Loaded message received.", ""));
}

void TestGameActorProxy2::CreateDrawable()
{
   SetDrawable(*new TestGameActor2(*this));
}

/////////////////////////////////////////////////////
// Actor code
/////////////////////////////////////////////////////
TestGameActor2::TestGameActor2(dtGame::GameActorProxy& parent): dtGame::GameActor(parent), mActorDeletedCount(0), mActorPublishedCount(0),
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
