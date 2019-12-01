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
#include "testgameactor.h"

#include <dtCore/scene.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/propertymacros.h>
#include <dtGame/invokable.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>

#include <dtUtil/datastream.h>

////////////////////////////////////////////////////////////////////
// Actor Component Code
////////////////////////////////////////////////////////////////////

const dtGame::ActorComponent::ACType TestActorComponent1::TYPE(new dtCore::ActorType("TestActorComponent1", "ActorComponents",
       "Test1",
       dtGame::ActorComponent::BaseActorComponentType));

TestActorComponent1::TestActorComponent1()
: dtGame::ActorComponent(TYPE)
, mAddAnotherActor(false)
, mAddActorInitialized(false)
, mWasAdded(false)
, mWasRemoved(false)
, mEnteredWorld(false)
, mLeftWorld(false)
{
   // These shouldn't crash if called before being added.
   RegisterForTick();
   UnregisterForTick();
}

void TestActorComponent1::OnAddedToActor(dtCore::BaseActorObject& /*actor*/)
{
   mWasAdded = true;
}

void TestActorComponent1::OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/)
{
   mWasRemoved = true;
}

////////////////////////////////////////////////////////////////////////////////
void TestActorComponent1::OnEnteredWorld()
{
   mEnteredWorld = true;
   if (mAddAnotherActor)
   {
      dtGame::GameManager* gm = GetOwner<dtGame::GameActorProxy>()->GetGameManager();
      dtCore::RefPtr<TestGameActor1> newActor;
      gm->CreateActor("ExampleActors", "Test1Actor", newActor);
      gm->AddActor(*newActor, false, false);
      mAddActorInitialized = newActor->IsInGM();
   }
}

////////////////////////////////////////////////////////////////////////////////
void TestActorComponent1::OnRemovedFromWorld()
{
   mLeftWorld = true;
}
const dtGame::ActorComponent::ACType TestActorComponent2::TYPE(new dtCore::ActorType("TestActorComponent2", "ActorComponents",
       "Test2",
       dtGame::ActorComponent::BaseActorComponentType));

TestActorComponent2::TestActorComponent2()
: dtGame::ActorComponent(TYPE)
, mWasAdded(false)
, mWasRemoved(false)
, mEnteredWorld(false)
, mLeftWorld(false)
{
}

void TestActorComponent2::OnAddedToActor(dtCore::BaseActorObject& /*actor*/)
{
   mWasAdded = true;
}

void TestActorComponent2::OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/)
{
   mWasRemoved = true;
}

////////////////////////////////////////////////////////////////////////////////
void TestActorComponent2::OnEnteredWorld()
{
   mEnteredWorld = true;
}

////////////////////////////////////////////////////////////////////////////////
void TestActorComponent2::OnRemovedFromWorld()
{
   mLeftWorld = true;
}
////////////////////////////////////////////////////////////////////
// Proxy Code
////////////////////////////////////////////////////////////////////
TestGameActor1::TestGameActor1()
: mTestActorId(false)
, mTestActorIdFound(false)
, mTestActorIdInitialized(false)
, mTestActorNameFound(false)
, mTestActorNameInitialized(false)
, mCompletedOnEnteredWorld(false)
, ticksEnabled(false)
, fired(false)
, tickLocals(0)
, tickRemotes(0)
{
   SetClassName("TestGameActor1");
}

TestGameActor1::~TestGameActor1()
{
}

DT_IMPLEMENT_ACCESSOR(TestGameActor1, bool, TestActorIdFound)
DT_IMPLEMENT_ACCESSOR(TestGameActor1, bool, TestActorIdInitialized)
DT_IMPLEMENT_ACCESSOR(TestGameActor1, bool, TestActorNameFound)
DT_IMPLEMENT_ACCESSOR(TestGameActor1, bool, TestActorNameInitialized)
DT_IMPLEMENT_ACCESSOR(TestGameActor1, bool, CompletedOnEnteredWorld)


void TestGameActor1::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   static const dtUtil::RefString GROUPNAME = "TestGameActor1";

   typedef dtCore::PropertyRegHelper<TestGameActor1&, TestGameActor1> RegHelperType;
   RegHelperType propReg(*this, this, GROUPNAME);

   DT_REGISTER_PROPERTY(OneIsFired, "if this actor has fired.", RegHelperType, propReg);
   DT_REGISTER_PROPERTY(TickRemotes, "the number of remote tick messages counted.", RegHelperType, propReg);
   DT_REGISTER_PROPERTY(TickLocals, "the number of local tick messages counted.", RegHelperType, propReg);
   DT_REGISTER_PROPERTY(TestActorId, "An example linked actor property.", RegHelperType, propReg);
   DT_REGISTER_PROPERTY(TestActorNameToLookup, "An example string property for finding an actor by name.", RegHelperType, propReg);

   //add these to the accept filter so the tests can check them
   AddPropertyToLocalUpdateAcceptFilter("OneIsFired");
   AddPropertyToLocalUpdateAcceptFilter("TickRemotes");

   // Add and remove a property so the tests can verify this works.
   AddPropertyToLocalUpdateAcceptFilter("TickLocals");
   RemovePropertyFromLocalUpdateAcceptFilter("TickLocals");
}

void TestGameActor1::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();

   AddInvokable(*new dtGame::Invokable("Fire One",
      dtUtil::MakeFunctor(&TestGameActor1::FireOne, *this)));

   AddInvokable(*new dtGame::Invokable("Reset",
      dtUtil::MakeFunctor(&TestGameActor1::Reset, *this)));

   AddInvokable(*new dtGame::Invokable("Toggle Ticks",
      dtUtil::MakeFunctor(&TestGameActor1::ToggleTicks, this)));

   //register local tick handles.
   //This is just to test local handler registration.  If you want to
   //register to receive tick messages, you would override OnEnteredWorld()
   //and add code like GetGameManager()->RegisterForMessages(dtGame::MessageType::TICK_LOCAL, *this, "Tick Local")
   RegisterForMessagesAboutSelf(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   RegisterForMessagesAboutSelf(dtGame::MessageType::TICK_REMOTE, dtGame::GameActorProxy::TICK_REMOTE_INVOKABLE);
   ticksEnabled = true;
}

void TestGameActor1::OnEnteredWorld()
{
   if (!mTestActorId.IsNull())
   {
      dtGame::GameActorProxy* toFind = NULL;
      GetGameManager()->FindActorById(mTestActorId, toFind);
      mTestActorIdFound = toFind != NULL;
      if (mTestActorIdFound)
         mTestActorIdInitialized = toFind->IsInGM();
   }

   if (!mTestActorNameToLookup.empty())
   {
      dtGame::GameActorProxy* toFind = NULL;
      GetGameManager()->FindActorByName(mTestActorNameToLookup, toFind);
      mTestActorNameFound = toFind != NULL;
      if (mTestActorNameFound)
         mTestActorNameInitialized = toFind->IsInGM();
   }
}

void TestGameActor1::CreateDrawable()
{
   SetDrawable(*new dtGame::GameActor(*this));
}

void TestGameActor1::ToggleTicks(const dtGame::Message& message)
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

void TestGameActor1::FireOne(const dtGame::Message& message)
{
   fired = true;
}

void TestGameActor1::Reset(const dtGame::Message& message)
{
   fired = false;
}

void TestGameActor1::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   tickLocals++;
}

void TestGameActor1::OnTickRemote(const dtGame::TickMessage& tickMessage)
{
   tickRemotes++;
}
