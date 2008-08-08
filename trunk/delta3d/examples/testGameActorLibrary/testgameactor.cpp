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
#include <iostream>
#include "testgameactor.h"
#include <dtCore/scene.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>
#include <dtGame/invokable.h>

////////////////////////////////////////////////////////////////////
// Proxy Code
////////////////////////////////////////////////////////////////////
TestGameActorProxy1::TestGameActorProxy1():ticksEnabled(false)
{
   SetClassName("TestGameActor1");
}

TestGameActorProxy1::~TestGameActorProxy1() 
{

}

void TestGameActorProxy1::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();
   
   AddProperty(new dtDAL::BooleanActorProperty("Has Fired", "Has this actor fired", 
      dtDAL::MakeFunctor(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::SetOneIsFired), 
      dtDAL::MakeFunctorRet(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::OneIsFired), 
      "Sets/Gets if this actor has fired.", ""));

   AddProperty(new dtDAL::IntActorProperty("Local Tick Count", "The number of tick messages received", 
      dtDAL::MakeFunctor(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::SetTickLocals), 
      dtDAL::MakeFunctorRet(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::GetTickLocals), 
      "Sets/Gets the number of local tick messages counted.", ""));
   AddProperty(new dtDAL::IntActorProperty("Remote Tick Count", "The number of tick messages received", 
      dtDAL::MakeFunctor(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::SetTickRemotes), 
      dtDAL::MakeFunctorRet(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::GetTickRemotes), 
      "Sets/Gets the number of remote tick messages counted.", ""));
}

void TestGameActorProxy1::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
   
   AddInvokable(*new dtGame::Invokable("Fire One", 
      dtDAL::MakeFunctor(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::FireOne)));

   AddInvokable(*new dtGame::Invokable("Reset", 
      dtDAL::MakeFunctor(static_cast<TestGameActor1&>(GetGameActor()), &TestGameActor1::Reset)));

   AddInvokable(*new dtGame::Invokable("Toggle Ticks", 
      dtDAL::MakeFunctor(*this, &TestGameActorProxy1::ToggleTicks)));
   
   //register local tick handles.  
   //This is just to test local handler registration.  If you want to
   //register to receive tick messages, you would override OnEnteredWorld()
   //and add code like GetGameManager()->RegisterForMessages(dtGame::MessageType::TICK_LOCAL, *this, "Tick Local")
   RegisterForMessagesAboutSelf(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   RegisterForMessagesAboutSelf(dtGame::MessageType::TICK_REMOTE, dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
   //RegisterMessageHandler(dtGame::MessageType::TICK_LOCAL, "Tick Local");
   //RegisterMessageHandler(dtGame::MessageType::TICK_REMOTE, "Tick Remote");
   ticksEnabled = true;
}

void TestGameActorProxy1::CreateActor()
{
   SetActor(*new TestGameActor1(*this));
}

void TestGameActorProxy1::ToggleTicks(const dtGame::Message& message)
{
   if (ticksEnabled)
   {
      UnregisterForMessagesAboutSelf(dtGame::MessageType::TICK_LOCAL, 
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
      UnregisterForMessagesAboutSelf(dtGame::MessageType::TICK_REMOTE, 
         dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
      ticksEnabled = false;
   }
   else
   {
      RegisterForMessagesAboutSelf(dtGame::MessageType::TICK_LOCAL, 
         dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
      RegisterForMessagesAboutSelf(dtGame::MessageType::TICK_REMOTE, 
         dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
      ticksEnabled = true;
   }      
}
////////////////////////////////////////////////////////////////////
// Actor Code
////////////////////////////////////////////////////////////////////
TestGameActor1::TestGameActor1(dtGame::GameActorProxy& proxy): dtGame::GameActor(proxy), fired(false), tickLocals(0), tickRemotes(0)
{
}

TestGameActor1::~TestGameActor1()
{

}

void TestGameActor1::FireOne(const dtGame::Message& message)
{
   fired = true;
}

void TestGameActor1::Reset(const dtGame::Message& message)
{
   fired = false;      
}

void TestGameActor1::TickLocal(const dtGame::Message& tickMessage)
{
   tickLocals++;
}

void TestGameActor1::TickRemote(const dtGame::Message& tickMessage)
{
   tickRemotes++;
}
