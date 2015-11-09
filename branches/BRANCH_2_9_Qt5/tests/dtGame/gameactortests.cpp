/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * @author Eddie Johnson and David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <dtGame/testcomponent.h>

#include <testGameActorLibrary/testgameactorlibrary.h>
#include <testGameActorLibrary/testgameenvironmentactor.h>
#include <testGameActorLibrary/testgamepropertyactor.h>
#include <testGameActorLibrary/testgameactor.h>

#include <dtABC/application.h>

#include <dtCore/camera.h>
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/timer.h>

#include <dtCore/actortype.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/project.h>
#include <dtCore/resourcedescriptor.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/environmentactor.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/datastream.h>
#include <dtUtil/log.h>

#include <osg/Math>

#include "basegmtests.h"

#include <iostream>

class GameActorTests : public dtGame::BaseGMTestFixture
{
   CPPUNIT_TEST_SUITE(GameActorTests);

      CPPUNIT_TEST(TestGameActorProxy);
      //CPPUNIT_TEST(TestGameActorNoDefaultStateSet);
      CPPUNIT_TEST(TestGameActorProxyDeleteError);
      CPPUNIT_TEST(TestSetEnvironmentActor);
      CPPUNIT_TEST(TestAddRemoveFromEnvActor);
      CPPUNIT_TEST(TestInvokables);
      CPPUNIT_TEST(TestInvokableMessageRegistration);
      CPPUNIT_TEST(TestGlobalInvokableMessageRegistration);
      CPPUNIT_TEST(TestGlobalInvokableMessageRegistrationEndOfFrame);
      CPPUNIT_TEST(TestStaticGameActorTypes);
      CPPUNIT_TEST(TestEnvironmentTimeConversions);
      CPPUNIT_TEST(TestDefaultProcessMessageRegistration);
      CPPUNIT_TEST(TestMessageProcessingPerformance);
      CPPUNIT_TEST(TestActorIsInGM);
      CPPUNIT_TEST(TestOnRemovedActor);
      CPPUNIT_TEST(TestUnregisterNextInvokable);
      CPPUNIT_TEST(TestFullUpdateFlags);
      CPPUNIT_TEST(TestPartialUpdateFlags);

   CPPUNIT_TEST_SUITE_END();

public:
   void TestGameActorProxy();
   void TestGameActorNoDefaultStateSet();
   void TestGameActorProxyDeleteError();
   void TestSetEnvironmentActor();
   void TestAddRemoveFromEnvActor();
   void TestInvokables();
   void TestInvokableMessageRegistration();
   void TestDefaultProcessMessageRegistration();
   void TestGlobalInvokableMessageRegistration();
   void TestGlobalInvokableMessageRegistrationEndOfFrame();
   void TestStaticGameActorTypes();
   void TestEnvironmentTimeConversions();
   void TestMessageProcessingPerformance();
   void TestActorIsInGM();
   void TestOnRemovedActor();
   void TestUnregisterNextInvokable();
   void TestFullUpdateFlags();
   void TestPartialUpdateFlags();

private:
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameActorTests);


////////////////////////////////////////////////////////////////////////
void GameActorTests::TestActorIsInGM()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actorType = mGM->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGM->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap != NULL);
      CPPUNIT_ASSERT_MESSAGE("BaseActorObject Should not be in GM", !gap->IsInGM());
      CPPUNIT_ASSERT_MESSAGE("BaseActorObject Should not be marked deleted", !gap->IsDeleted());
      gap->SetDeleted(true);

      mGM->AddActor(*gap.get(), false, false);

      CPPUNIT_ASSERT_MESSAGE("BaseActorObject Should be in GM", gap->IsInGM());
      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should have deleted marked false on add.", !gap->IsDeleted());
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestGameActorProxy()
{
   try
   {
      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mGM->CreateActor("ExampleActors", "Test1Actor", gap);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);
      CPPUNIT_ASSERT_MESSAGE("GameActor should have a reference to the proxy", &gap->GetDrawable<dtGame::GameActor>()->GetGameActorProxy() == gap.get());

      dtGame::GameActor* p = NULL;
      gap->GetDrawable(p);

      CPPUNIT_ASSERT_MESSAGE("Actor should not be NULL", p != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActor should always return true", gap->IsGameActor());
      CPPUNIT_ASSERT_MESSAGE("Game Actor should not initially be published", !gap->IsPublished());
      CPPUNIT_ASSERT_MESSAGE("Game Actor should not initially be remote", !gap->IsRemote());


      CPPUNIT_ASSERT_MESSAGE("GameActorProxy's ownership should default to SERVER_LOCAL",
               gap->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      gap->SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy's ownership should have been set", gap->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy local actor update policy should default to ACCEPT_ALL",
               gap->GetLocalActorUpdatePolicy() == dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_ALL);

      gap->SetLocalActorUpdatePolicy(dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy local actor update policy should now be IGNORE_ALL",
               gap->GetLocalActorUpdatePolicy() == dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);

      CPPUNIT_ASSERT(gap->GetPrototype() == NULL);
      CPPUNIT_ASSERT(gap->GetDrawable<dtGame::GameActor>()->GetPrototypeID().ToString().empty());
      CPPUNIT_ASSERT(gap->GetDrawable<dtGame::GameActor>()->GetPrototypeName().empty());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

void GameActorTests::TestGameActorNoDefaultStateSet()
{
   try
   {
      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mGM->CreateActor("ExampleActors", "Test1Actor", gap);
      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);

      dtGame::GameActor* gameActor = NULL;
      gap->GetDrawable(gameActor);

      CPPUNIT_ASSERT_MESSAGE("a newly created game actor should not have stateset.",
               gameActor->GetOSGNode()->getStateSet() == NULL);

      mGM->AddActor(*gap, false, false);

      CPPUNIT_ASSERT_MESSAGE("A game actor that has just been added to the gm should not have stateset",
               gameActor->GetOSGNode()->getStateSet() == NULL);

      gameActor->SetShaderGroup(std::string());

      CPPUNIT_ASSERT_MESSAGE("Setting the shader group to empty should not create a stateset",
               gameActor->GetOSGNode()->getStateSet() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void GameActorTests::TestGameActorProxyDeleteError()
{
   // This test verifies that an error is thrown if the proxy is deleted but the actor is still around
   // but the developer calls GetGameActorProxy.  This can happen because the GameACtor only holds onto
   // an observer_ptr to the parent.
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actorType = mGM->FindActorType("ExampleActors", "Test1Actor");

      CPPUNIT_ASSERT(actorType != NULL);

      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mGM->CreateActor(*actorType, gap);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);
      // TODO this test is about to be invalid.
      CPPUNIT_ASSERT_MESSAGE("GameActor should have a reference to the proxy", &gap->GetGameActor().GetGameActorProxy() == gap.get());

      dtCore::RefPtr<dtGame::GameActor> actor = gap->GetDrawable<dtGame::GameActor>();
      dtCore::ObserverPtr<dtGame::GameActorProxy> gapObserver(gap.get());
      dtCore::ObserverPtr<dtGame::GameActorProxy> gapOb2(gapObserver);
      gap = NULL;

      CPPUNIT_ASSERT_MESSAGE("Nothing should be holding onto the proxy, so the observer should be NULL", !gapObserver.valid());
      ///This should throw an exception because the proxy should have been cleaned up.
      CPPUNIT_ASSERT_THROW(actor->GetGameActorProxy(), dtUtil::Exception);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

void GameActorTests::TestInvokables()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actorType = mGM->FindActorType("ExampleActors", "Test1Actor");

      dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGM->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap != NULL);

      std::vector<dtGame::Invokable*> toFill;
      gap->GetInvokables(toFill);
      CPPUNIT_ASSERT_MESSAGE("The actor should have at least 2 invokables",toFill.size() >= 5);
      dtGame::Invokable* iF = gap->GetInvokable("Fire One");
      dtGame::Invokable* iR = gap->GetInvokable("Reset");
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Fire One ",iF != NULL);
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Reset ",iR != NULL);

      dtCore::BooleanActorProperty* prop = NULL;
      gap->GetProperty("OneIsFired", prop);

      CPPUNIT_ASSERT_MESSAGE("The actor should have a property named \"OneIsFired\"", prop != NULL);

      CPPUNIT_ASSERT_MESSAGE("Property \"OneIsFired\" should be false", !prop->GetValue());

      //need a dummy message
      dtCore::RefPtr<dtGame::Message> message = mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      iF->Invoke(*message);

      CPPUNIT_ASSERT_MESSAGE("Property \"OneIsFired\" should be true", prop->GetValue());

      iR->Invoke(*message);

      CPPUNIT_ASSERT_MESSAGE("Property \"OneIsFired\" should be false", !prop->GetValue());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestInvokableMessageRegistration()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actorType = mGM->FindActorType("ExampleActors", "Test1Actor");

      dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGM->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap != NULL);

      dtGame::Invokable* iToggle = gap->GetInvokable("Toggle Ticks");

      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Toggle Ticks ",iToggle != NULL);

      std::vector<dtGame::Invokable*> toFill1, toFill2;

      gap->GetMessageHandlers(dtGame::MessageType::TICK_LOCAL, toFill1);
      gap->GetMessageHandlers(dtGame::MessageType::TICK_REMOTE, toFill2);

      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick local", toFill1.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("The one handler should be Tick Local", toFill1[0]->GetName() == "Tick Local");
      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick remote", toFill2.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("The one handler should be Tick Remote", toFill2[0]->GetName() == "Tick Remote");

      //need a dummy message
      dtCore::RefPtr<dtGame::Message> message = mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      //this will remove the invokables registration.
      iToggle->Invoke(*message);

      gap->GetMessageHandlers(dtGame::MessageType::TICK_LOCAL, toFill1);
      gap->GetMessageHandlers(dtGame::MessageType::TICK_REMOTE, toFill2);

      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick local", toFill1.size() == 0);
      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick remote", toFill2.size() == 0);

      iToggle->Invoke(*message);

      //now to actually fire them...
      //need a dummy message
      message = mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);
      //so it will reach the actor
      message->SetAboutActorId(gap->GetId());
      //this will remove the invokables registration.
      mGM->AddActor(*gap, false, false);
      mGM->SendMessage(*message);

      CPPUNIT_ASSERT_MESSAGE("Zero local ticks should have been received.", static_cast<dtCore::IntActorProperty*>(gap->GetProperty("TickLocals"))->GetValue() == 0);
      CPPUNIT_ASSERT_MESSAGE("Zero remote ticks should have been received.", static_cast<dtCore::IntActorProperty*>(gap->GetProperty("TickRemotes"))->GetValue() == 0);

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("One local tick should have been received.", static_cast<dtCore::IntActorProperty*>(gap->GetProperty("TickLocals"))->GetValue() == 1);
      CPPUNIT_ASSERT_MESSAGE("Zero remote ticks should have been received.", static_cast<dtCore::IntActorProperty*>(gap->GetProperty("TickRemotes"))->GetValue() == 0);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestDefaultProcessMessageRegistration()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<const dtCore::ActorType> actor2Type = mGM->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtCore::BaseActorObject> proxy1 = mGM->CreateActor(*actor1Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*>(proxy1.get());

      dtCore::RefPtr<dtCore::BaseActorObject> proxy2 = mGM->CreateActor(*actor2Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*>(proxy2.get());

      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap2 != NULL);

      // Make sure the invokable was created
      dtGame::Invokable* iTestListener = gap2->GetInvokable(dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named \"Process Message\"", iTestListener != NULL);

      mGM->AddActor(*gap1, false, false);
      mGM->AddActor(*gap2, false, false);

      // Make sure we can use proxy method to register for messages
      gap2->RegisterForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      std::vector< std::pair<dtGame::GameActorProxy*, std::string> > toFill;
      mGM->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be one registered game actor listener for the actor published message",
         (unsigned int) 1, (unsigned int) toFill.size());

      // And unregister them too
      gap2->UnregisterForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      mGM->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message", toFill.size() == 0);

      // now use the Register For Self behavior - they only go on the actor itself
      gap2->RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
         dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);

      // There shouldnt be any listeners on the GM
      mGM->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap2->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should not be any listeners for the actor published message this time", toFill.size() == 0);
      mGM->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should not be any listeners for the actor published message this time", toFill.size() == 0);

      mGM->PublishActor(*gap1);
      mGM->PublishActor(*gap2);

      dtCore::System::GetInstance().Step();

      // One publish message should have been received. So, count should be 2
      CPPUNIT_ASSERT_EQUAL_MESSAGE("We should only have gotten 1 publish, so count should be 2.",
         int(2), static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void GameActorTests::TestGlobalInvokableMessageRegistrationEndOfFrame()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtGame::GameActorProxy> gap1;
      mGM->CreateActor(*actor1Type, gap1);

      mGM->AddActor(*gap1, false, false);

      dtGame::Invokable* iTestListener = gap1->GetInvokable("Test Message Listener");

      CPPUNIT_ASSERT_THROW(
               mGM->RegisterForMessages(dtGame::MessageType::TICK_END_OF_FRAME, *gap1, iTestListener->GetName()),
               dtUtil::Exception);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void GameActorTests::TestGlobalInvokableMessageRegistration()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<const dtCore::ActorType> actor2Type = mGM->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtGame::GameActorProxy> gap1;
      mGM->CreateActor(*actor1Type, gap1);

      dtCore::RefPtr<dtGame::GameActorProxy> gap2;
      mGM->CreateActor(*actor2Type, gap2);

      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("BaseActorObject should not be NULL", gap2 != NULL);

      dtGame::Invokable* iTestListener = gap2->GetInvokable("Test Message Listener");

      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named \"Test Message Listener\"",iTestListener != NULL);

      mGM->AddActor(*gap1, false, false);
      mGM->AddActor(*gap2, false, false);

      mGM->RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mGM->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), *gap2, iTestListener->GetName());

      std::vector< std::pair<dtGame::GameActorProxy*, std::string> > toFill;

      mGM->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be one registered global listener for the Map Loaded message",
            size_t(1), toFill.size());

      mGM->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message",
            toFill.size() == 1);

      mGM->UnregisterForMessages(dtGame::MessageType::INFO_MAP_LOADED,
            *gap2, iTestListener->GetName());
      mGM->UnregisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), *gap2, iTestListener->GetName());

      mGM->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered global listener for the Map Loaded message",
            toFill.size() == 0);

      mGM->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message",
            toFill.size() == 0);

      mGM->RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mGM->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), *gap2, iTestListener->GetName());

      //std::cout << gap1->GetId() << std::endl;

      mGM->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be one registered global listener for the Map Loaded message",
            size_t(1), toFill.size());

      mGM->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message",
            toFill.size() == 1);

      mGM->PublishActor(*gap1);
      mGM->SendMessage(*mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
      mGM->DeleteActor(*gap1);

      //actors are not removed immediately
      CPPUNIT_ASSERT(mGM->FindGameActorById(gap1->GetId()) != NULL);

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      //Actor should be removed by now.
      CPPUNIT_ASSERT(mGM->FindGameActorById(gap1->GetId()) == NULL);

      //A publish and map loaded message should have been received, but no listener was added for delete.
      CPPUNIT_ASSERT_EQUAL(0, static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue());
      CPPUNIT_ASSERT_EQUAL(1, static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue());
      CPPUNIT_ASSERT_EQUAL(1, static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue());

      dtCore::RefPtr<dtGame::GameActorProxy> gap3;
      mGM->CreateActor(*actor1Type, gap3);

      //add global and actor-specific delete listeners
      mGM->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_DELETED,
            gap3->GetId(), *gap2, iTestListener->GetName());
      mGM->RegisterForMessages(dtGame::MessageType::INFO_ACTOR_DELETED, *gap2, iTestListener->GetName());
      //take off the publish listener so that when the message shows up, it will not be passed on.
      mGM->UnregisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      //add and publish the actor.
      mGM->AddActor(*gap3, false, true);
      //delete it
      mGM->DeleteActor(*gap3);
      //add a load map message again.
      mGM->SendMessage(*mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);

      //test removing the map loaded message and then send one
      mGM->UnregisterForMessages(
         dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mGM->SendMessage(*mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtCore::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestAddRemoveFromEnvActor()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test environment actor in the test game library", type.valid());
      dtCore::RefPtr<dtCore::BaseActorObject> ap = mGM->CreateActor(*type);
      CPPUNIT_ASSERT_MESSAGE("The game manager should have been able to create the test environment actor", ap.valid());

      dtCore::RefPtr<TestGameEnvironmentActorProxy> eap = dynamic_cast<TestGameEnvironmentActorProxy*>(ap.get());
      CPPUNIT_ASSERT_MESSAGE("The dynamic cast should not have returned NULL", eap != NULL);
      dtCore::RefPtr<TestGameEnvironmentActor> ea = dynamic_cast<TestGameEnvironmentActor*>(eap->GetDrawable());
      CPPUNIT_ASSERT_MESSAGE("Should have been able to cast the environment proxy's actor to an environment actor", ea != NULL);

      // SET ENVIRONMENT ACTOR TESTS
      mGM->SetEnvironmentActor(eap.get());
      //One cannot enable paging without a window.
      dtCore::System::GetInstance().Step(0.0167f);

      // --- TEST GM REMOVAL --- START --- //
      // Re-insert environment actor for this sub-test
      mGM->SetEnvironmentActor(eap.get());
      // Add actor 1
      ap = mGM->CreateActor(*type);
      mGM->AddActor(*ap);
      CPPUNIT_ASSERT(dynamic_cast<dtGame::GameActorProxy*>(ap.get())->GetParentActor() == eap);
      CPPUNIT_ASSERT_EQUAL(3, ap->referenceCount());
      // Add actor 2
      dtCore::RefPtr<dtCore::BaseActorObject> ap2 = mGM->CreateActor(*type);
      mGM->AddActor(*ap2);
      CPPUNIT_ASSERT(dynamic_cast<dtGame::GameActorProxy*>(ap2.get())->GetParentActor() == eap);
      CPPUNIT_ASSERT_EQUAL(3, ap2->referenceCount());

      osg::observer_ptr<dtCore::BaseActorObject> apObserver1 = ap.get();
      osg::observer_ptr<dtCore::BaseActorObject> apObserver2 = ap2.get();

      // Check GM count
      CPPUNIT_ASSERT_MESSAGE("The game manager should have 2 actors in its scene",
         ea->GetNumEnvironmentChildren() == 2);

      // Remove actor 2
      dynamic_cast<dtGame::GameActorProxy*>(ap2.get())->SetParentActor(NULL);
      CPPUNIT_ASSERT_MESSAGE("The actor 2 should be removed from the scene", ! ea->ContainsActor(*ap2->GetDrawable()) );
      CPPUNIT_ASSERT_MESSAGE("The game manager should have 1 actor in its scene",
         ea->GetNumEnvironmentChildren() == 1);

      // Add actor 2 as child to actor 1
      dynamic_cast<dtGame::GameActorProxy*>(ap2.get())->SetParentActor(dynamic_cast<dtGame::GameActorProxy*>(ap.get()));

      // Delete actor 1 from GM
      mGM->DeleteActor(*ap);
      dtCore::System::GetInstance().Step(0.0167f);

      // ap 2 should have been deleted as well.
      CPPUNIT_ASSERT(mGM->FindActorById(ap2->GetId()) == NULL);

      std::vector<dtCore::DeltaDrawable*> actors;
      std::vector<dtCore::DeltaDrawable*> drawables;
      // Ensure environment has removed actor 2.
      CPPUNIT_ASSERT_MESSAGE("The environment actor should have removed actor 2", ! ea->ContainsActor(*ap2->GetDrawable()) );
      CPPUNIT_ASSERT_MESSAGE("The environment actor should have removed actor 2", !ea->HasChild(ap2->GetDrawable()) );

      // Ensure all actors are removed from the environment
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());
      dtCore::RefPtr<osg::ObserverSet> obSet = ap->getOrCreateObserverSet();

      CPPUNIT_ASSERT(dynamic_cast<dtGame::GameActorProxy*>(ap2.get())->GetParentActor() == NULL);

      CPPUNIT_ASSERT_EQUAL(1, ap->referenceCount());
      CPPUNIT_ASSERT_EQUAL(1, ap2->referenceCount());
      ap  = NULL;
      ap2 = NULL;

      CPPUNIT_ASSERT(obSet->getObserverdObject() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Actor 1 should have been deleted.", !apObserver1.valid());
      CPPUNIT_ASSERT_MESSAGE("Actor 2 should have been deleted.", !apObserver2.valid());

      mGM->DeleteAllActors(true);
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      // It's 1 because the scene has 1 in it and the GM only deletes what was added.
      CPPUNIT_ASSERT_EQUAL(size_t(1), drawables.size());
      // --- TEST GM REMOVAL --- END --- //
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestSetEnvironmentActor()
{
   try
   {
      dtCore::RefPtr<dtCore::View> view = new dtCore::View;
      view->SetScene(&mGM->GetScene());
      //One cannot enable paging without a window.
      //mGM->GetScene().EnablePaging();
      //CPPUNIT_ASSERT(mGM->GetScene().IsPagingEnabled());
      dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent("name");
      CPPUNIT_ASSERT(tc.valid());
      mGM->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      dtCore::RefPtr<const dtCore::ActorType> type = mGM->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test environment actor in the test game library", type.valid());
      dtCore::RefPtr<dtCore::BaseActorObject> ap = mGM->CreateActor(*type);
      CPPUNIT_ASSERT_MESSAGE("The game manager should have been able to create the test environment actor", ap.valid());

      dtCore::RefPtr<TestGameEnvironmentActorProxy> eap = dynamic_cast<TestGameEnvironmentActorProxy*>(ap.get());
      CPPUNIT_ASSERT_MESSAGE("The dynamic cast should not have returned NULL", eap != NULL);
      dtCore::RefPtr<TestGameEnvironmentActor> ea = dynamic_cast<TestGameEnvironmentActor*>(eap->GetDrawable());
      CPPUNIT_ASSERT_MESSAGE("Should have been able to cast the environment proxy's actor to an environment actor", ea != NULL);

      std::vector<dtCore::DeltaDrawable*> actors;
      std::vector<dtCore::DeltaDrawable*> drawables;
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());

      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The game manager should have the correct number of actors in it",
            0U, unsigned(drawables.size() - 1));

      type = mGM->FindActorType("ExampleActors", "TestPlayer");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test player actor in the game manager", type.valid());

      const unsigned int numActors = 20;
      for (unsigned int i = 0; i < numActors; ++i)
      {
         ap = mGM->CreateActor(*type);
         CPPUNIT_ASSERT_MESSAGE("The game manager should be able to create the test player actor", ap.valid());
         mGM->AddActor(*ap);
         drawables.clear();
         mGM->GetScene().GetChildren(drawables);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The game manager should have the correct number of actors in it",
               i+1, unsigned(drawables.size() - 1));
      }

      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The game manager should have the correct number of actors in it",
            numActors, unsigned(drawables.size() - 1));

      // SET ENVIRONMENT ACTOR TESTS
      mGM->SetEnvironmentActor(eap.get());
      dtCore::System::GetInstance().Step(0.016f);
      std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = tc->GetReceivedProcessMessages();
      bool wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should have been sent", wasMessage);
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("An environment actor was added.  The game manager should only have "
         " the environment actor and the default scene light.", 2U, unsigned(drawables.size()));
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor should now have the number of actors in it", actors.size() == numActors);
      mGM->SetEnvironmentActor(NULL);
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step(0.016f);
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent", !wasMessage);
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Set the environment actor to NULL. The game manager should now have the correct "
            "number of drawables in it", size_t(numActors), drawables.size()-1U);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor should not have any actors in it", actors.empty());
      mGM->SetEnvironmentActor(NULL);
      //CPPUNIT_ASSERT(!mGM->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step(0.016f);
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent, it was a no-op", !wasMessage);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment from NULL to NULL should not have done anything", actors.empty());
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment from NULL to NULL should not have done anything", drawables.size()-1 == numActors);

      type = mGM->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT(type.valid());
      ap = mGM->CreateActor(*type);
      CPPUNIT_ASSERT(ap.valid());
      dtCore::RefPtr<dtGame::IEnvGameActorProxy> eap2 = dynamic_cast<dtGame::IEnvGameActorProxy*>(ap.get());
      CPPUNIT_ASSERT(eap2 != NULL);
      dtCore::RefPtr<dtGame::IEnvGameActor>      ea2  = dynamic_cast<dtGame::IEnvGameActor*>(eap2->GetDrawable());
      CPPUNIT_ASSERT(ea2 != NULL);
      mGM->SetEnvironmentActor(eap.get());
      //One cannot enable paging without a window.
      //CPPUNIT_ASSERT(!mGM->GetScene().IsPagingEnabled());
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Once again, the environment actor should now have all the actors", actors.size() == numActors);
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_MESSAGE("Once again, the game manager should have no actors in the scene except the environment actor", drawables.size() == 2);
      //One cannot enable paging without a window.
      //mGM->GetScene().EnablePaging();
      mGM->SetEnvironmentActor(eap2.get());
      //CPPUNIT_ASSERT(mGM->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); i++)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should have been sent", wasMessage);
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment actor on the game manager from one to another should NOT add any actors to the game manager", drawables.size() == 2);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor was set to another, this environment actor should NOT have any actors", actors.empty());
      ea2->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Since this environment actor was set, it should now have all the actors", actors.size() == numActors);
      mGM->SetEnvironmentActor(NULL);
      //One cannot enable paging without a window.
      //CPPUNIT_ASSERT(mGM->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent", !wasMessage);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());
      ea2->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The previous environment actor should not have any actors", actors.empty());
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      CPPUNIT_ASSERT_MESSAGE("The game manager should now have all the actors", drawables.size() - 1 == numActors);
      mGM->DeleteAllActors(true);
      drawables.clear();
      mGM->GetScene().GetChildren(drawables);
      // It's 1 because it starts out with 1 and the GM only deletes actors it added
      CPPUNIT_ASSERT_EQUAL(size_t(1), drawables.size());
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   //catching std::exception messes up the CPPUNIT failure exceptions.
   //catch(const std::exception &e)
   //{
   //   std::string cause = std::string("std::exception of type: ") + typeid(e).name() + std::string(" caught. Message is: ") + e.what();
   //   CPPUNIT_FAIL(cause);
   //}
}

void GameActorTests::TestStaticGameActorTypes()
{
   const unsigned int size = 8;
   dtCore::RefPtr<dtCore::ActorType> types[size] =
   {
      TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST2_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST_PLAYER_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST_TASK_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST_COUNTER_TASK_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST_TANK_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST_JET_GAME_ACTOR_TYPE,
      TestGameActorLibrary::TEST_HELICOPTER_GAME_ACTOR_TYPE/*,
      TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE*/
   };
   for (unsigned int i = 0; i < size; ++i)
   {
      dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGM->CreateActor(*types[i]);
      CPPUNIT_ASSERT_MESSAGE("The proxy should not be NULL", proxy.valid());
      CPPUNIT_ASSERT_MESSAGE("The proxy's actor should not be NULL", proxy->GetDrawable() != NULL);
   }
}

void GameActorTests::TestEnvironmentTimeConversions()
{
   dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGM->CreateActor(*TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_TYPE);
   CPPUNIT_ASSERT(proxy.valid());
   dtCore::RefPtr<TestGameEnvironmentActor> envActor = dynamic_cast<TestGameEnvironmentActor*>(proxy->GetDrawable());
   CPPUNIT_ASSERT(envActor.valid());
   std::string testTime = "2006-04-20T18:04:09";
   envActor->SetTimeAndDateString(testTime);
   int year, month, day, hour, min, sec;
   envActor->GetTimeAndDate(year, month, day, hour, min, sec);
   CPPUNIT_ASSERT_MESSAGE("The year should be correct", year == 2006);
   CPPUNIT_ASSERT_MESSAGE("The month should be correct", month == 4);
   CPPUNIT_ASSERT_MESSAGE("The day should be correct", day == 20);
   CPPUNIT_ASSERT_MESSAGE("The hour should be correct", hour == 18);
   CPPUNIT_ASSERT_MESSAGE("The minute should be correct", min == 4);
   CPPUNIT_ASSERT_MESSAGE("The seconds should be close to correct", osg::equivalent(float(sec), 9.0f, 1.0f));

   std::string newValue = envActor->GetTimeAndDateString();
   CPPUNIT_ASSERT_MESSAGE("The time and date string should be equal to the original value", newValue == testTime);
}

void GameActorTests::TestPartialUpdateFlags()
{
   dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "TestGamePropertyActor");
   dtCore::RefPtr<TestGamePropertyActor> actor1;
   mGM->CreateActor(*actor1Type, actor1);
   CPPUNIT_ASSERT_MESSAGE("Actor should not be NULL", actor1 != NULL);

   std::vector<dtUtil::RefString> names;
   actor1->GetPartialUpdateProperties(names);

   // DG - The translation and rotation are added the partial update list by default, unless you have a DR helper.
   CPPUNIT_ASSERT_EQUAL(2U, unsigned(names.size()));
//   // Check the list for the required ones.
//   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtCore::TransformableActorProxy::PROPERTY_TRANSLATION) != names.end());
//   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), dtCore::TransformableActorProxy::PROPERTY_ROTATION) != names.end());

   dtCore::PropertyContainer::PropertyVector pv;

   actor1->GetPropertyList(pv);

   for(unsigned i = 0; i < pv.size(); ++i)
   {
      dtCore::ActorProperty* prop = pv[i];
      // Set half to false;
      prop->SetSendInPartialUpdate((i % 2) == 1);
   }
   names.clear();
   actor1->GetPartialUpdateProperties(names);

   for(unsigned i = 0; i < pv.size(); ++i)
   {
      dtCore::ActorProperty* prop = pv[i];
      bool found = std::find(names.begin(), names.end(), prop->GetName()) != names.end();
      if (prop->GetSendInPartialUpdate())
      {
         CPPUNIT_ASSERT(found);
      }
      else
      {
         CPPUNIT_ASSERT(!found);
      }
   }
}

void GameActorTests::TestFullUpdateFlags()
{
   dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "TestGamePropertyActor");
   dtCore::RefPtr<TestGamePropertyActor> actor1;
   mGM->CreateActor(*actor1Type, actor1);
   CPPUNIT_ASSERT_MESSAGE("Actor should not be NULL", actor1 != NULL);


   dtCore::PropertyContainer::PropertyVector pv;
   actor1->GetPropertyList(pv);

   dtCore::RefPtr<dtGame::ActorUpdateMessage> updateMsg;
   mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, updateMsg);

   for(unsigned i = 0; i < pv.size(); ++i)
   {
      dtCore::ActorProperty* prop = pv[i];
      // Set half to false;
      prop->SetSendInFullUpdate((i % 2) == 1);
   }

   actor1->PopulateActorUpdate(*updateMsg);

   for(unsigned i = 0; i < pv.size(); ++i)
   {
      dtCore::ActorProperty* prop = pv[i];
      if (prop->GetSendInFullUpdate() && !prop->IsReadOnly())
      {
         CPPUNIT_ASSERT(updateMsg->GetUpdateParameter(prop->GetName()) != NULL);
      }
      else
      {
         CPPUNIT_ASSERT(updateMsg->GetUpdateParameter(prop->GetName()) == NULL);
      }
   }

}

void GameActorTests::TestMessageProcessingPerformance()
{
   int numActors = 20;
   int numTicks = 42;
   try
   {
      dtCore::Project::GetInstance().CreateContext("Working Project");
      dtCore::Project::GetInstance().SetContext("Working Project");

      dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "TestGamePropertyActor");

      // Start time in microseconds
      dtCore::Timer_t startTime(0);// = mGM->GetRealClockTime();
      dtCore::Timer statsTickClock;
      //Timer_t frameTickStart(0);
      startTime = statsTickClock.Tick();

      std::vector<TestGamePropertyActor*> testActors;
      testActors.reserve(size_t(numActors));
      for (int i = 0; i < numActors; ++i)
      {
         dtCore::RefPtr<TestGamePropertyActor> actor1;
         mGM->CreateActor(*actor1Type, actor1);
         CPPUNIT_ASSERT_MESSAGE("Actor should not be NULL", actor1 != NULL);

         actor1->SetRegisterListeners(false);
         // add it as a remote actor
         mGM->AddActor(*actor1, true, false);
         testActors.push_back(actor1);
      }

      dtCore::System::GetInstance().Step(0.016);

      // loop multiple ticks.
      for (int tickCounter = 0; tickCounter < numTicks; ++tickCounter)
      {
         // loop through the TON of actors (38 properties each)
         for (unsigned int actorIndex = 0; actorIndex < testActors.size(); ++actorIndex)
         {
            // create and populate an actor update message with ALL properties for this actor
            dtCore::RefPtr<dtGame::ActorUpdateMessage> updateMsg;
            mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, updateMsg);
            testActors[actorIndex]->PopulateActorUpdate(*updateMsg);
            mGM->SendMessage(*updateMsg);
         }

         dtCore::System::GetInstance().Step(0.016);
      }

      dtUtil::FileUtils::GetInstance().DirDelete("Working Project", true);
      CPPUNIT_ASSERT(!dtUtil::FileUtils::GetInstance().DirExists("Working Project"));

      // Start time in microseconds
      dtCore::Timer_t stopTime = statsTickClock.Tick();
      double timeDelta = statsTickClock.DeltaSec(startTime, stopTime);
      //Timer_t stopTime = mGM->GetRealClockTime();

      // 1 second???
      std::ostringstream ss;
      ss << "Update Msgs for " << numActors << " actors, 38 props, " << numTicks << " ticks took - [" << timeDelta << "] seconds " << std::endl;
      //CPPUNIT_ASSERT_MESSAGE(ss.str(), timeDelta < 10.0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////
void GameActorTests::TestOnRemovedActor()
{
   // This only tests that the new OnRemovedFromWorld method is called
   // Normal delete actor tests are done elsewhere.

   dtCore::RefPtr<TestGamePropertyActor> proxy1;
   dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "TestGamePropertyActor");
   mGM->CreateActor(*actor1Type, proxy1);
   mGM->AddActor(*proxy1, true, false);

   dtCore::System::GetInstance().Step(0.016f);

   CPPUNIT_ASSERT_MESSAGE("Proxy should NOT be marked as 'RemovedFromWorld'", !proxy1->IsRemovedFromWorld());

   mGM->DeleteActor(*proxy1);
   dtCore::System::GetInstance().Step(0.016f);

   CPPUNIT_ASSERT_MESSAGE("Proxy should BE marked as 'RemovedFromWorld'", proxy1->IsRemovedFromWorld());
}


////////////////////////////////////////////////////////////////////////////////
void GameActorTests::TestUnregisterNextInvokable()
{
   dtCore::RefPtr<const dtCore::ActorType> actor1Type = mGM->FindActorType("ExampleActors", "Test1Actor");
   dtCore::RefPtr<const dtCore::ActorType> actor2Type = mGM->FindActorType("ExampleActors", "Test1Actor");

   dtCore::RefPtr<dtCore::BaseActorObject> proxy1 = mGM->CreateActor(*actor1Type);
   dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*>(proxy1.get());

   dtCore::RefPtr<dtCore::BaseActorObject> proxy2 = mGM->CreateActor(*actor2Type);
   dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*>(proxy2.get());

   dtGame::Invokable* invokable1 = gap1->GetInvokable(dtGame::MessageType::TICK_LOCAL.GetName());
   dtGame::Invokable* invokable2 = gap2->GetInvokable(dtGame::MessageType::TICK_LOCAL.GetName());

   mGM->AddActor(*gap1, false, false);
   mGM->AddActor(*gap2, false, false);

   mGM->RegisterForMessages(dtGame::MessageType::TICK_LOCAL, *gap1, invokable1->GetName());
   mGM->RegisterForMessages(dtGame::MessageType::TICK_LOCAL, *gap2, invokable2->GetName());


   dtCore::System::GetInstance().Step();
}
