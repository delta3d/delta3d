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
#include <iostream>
#include "testgameactor.h"
#include <dtCore/scene.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>

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
   //and add code like GetGameManager()->RegisterGlobalMessageListener(dtGame::MessageType::TICK_LOCAL, *this, "Tick Local")
   RegisterMessageHandler(dtGame::MessageType::TICK_LOCAL, "Tick Local");
   RegisterMessageHandler(dtGame::MessageType::TICK_REMOTE, "Tick Remote");
   ticksEnabled = true;
}

void TestGameActorProxy1::CreateActor()
{
   mActor = new TestGameActor1(*this);
}

void TestGameActorProxy1::ToggleTicks(const dtGame::Message& message)
{
   if (ticksEnabled)
   {
      UnregisterMessageHandler(dtGame::MessageType::TICK_LOCAL, "Tick Local");
      UnregisterMessageHandler(dtGame::MessageType::TICK_REMOTE, "Tick Remote");
      ticksEnabled = false;
   }
   else
   {
      RegisterMessageHandler(dtGame::MessageType::TICK_LOCAL, "Tick Local");
      RegisterMessageHandler(dtGame::MessageType::TICK_REMOTE, "Tick Remote");
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
   std::cout << "TickLocal" << std::endl;
   tickLocals++;
}

void TestGameActor1::TickRemote(const dtGame::Message& tickMessage)
{
      std::cout << "TickRemote" << std::endl;
   tickRemotes++;
}
