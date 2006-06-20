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
 * @author Eddie Johnson and David Guthrie
 */

#include <iostream>
#include <osg/Math>
#include <dtUtil/log.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/camera.h>
#include <dtDAL/datatype.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actortype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/environmentactor.h>
#include <dtGame/gmcomponent.h>
#include <testGameActorLibrary/testgameactorlibrary.h>
#include <testGameActorLibrary/testgameenvironmentactor.h>

#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class TestComponent: public dtGame::GMComponent
{
   public:
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedProcessMessages()
      { return mReceivedProcessMessages; }
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedDispatchNetworkMessages()
      { return mReceivedDispatchNetworkMessages; }

      virtual void ProcessMessage(const dtGame::Message& msg)
      {
         mReceivedProcessMessages.push_back(&msg);
      }
      virtual void DispatchNetworkMessage(const dtGame::Message& msg)
      {
         mReceivedDispatchNetworkMessages.push_back(&msg);
      }

      void reset()
      {
         mReceivedDispatchNetworkMessages.clear();
         mReceivedProcessMessages.clear();
      }

      dtCore::RefPtr<const dtGame::Message> FindProcessMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedProcessMessages.size(); ++i)
         {
            if (mReceivedProcessMessages[i]->GetMessageType() == type)
               return mReceivedProcessMessages[i];
         }
         return NULL;
      }
      dtCore::RefPtr<const dtGame::Message> FindDispatchNetworkMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedDispatchNetworkMessages.size(); ++i)
         {
            if (mReceivedDispatchNetworkMessages[i]->GetMessageType() == type)
               return mReceivedDispatchNetworkMessages[i];
         }
         return NULL;
      }
   private:
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedProcessMessages;
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedDispatchNetworkMessages;
};

class GameActorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GameActorTests);

      CPPUNIT_TEST(TestGameActor);
      CPPUNIT_TEST(TestGameActorProxy);
      CPPUNIT_TEST(TestSetEnvironmentActor);
      CPPUNIT_TEST(TestInvokables);
      CPPUNIT_TEST(TestInvokableMessageRegistration);
      CPPUNIT_TEST(TestGlobalInvokableMessageRegistration);
      CPPUNIT_TEST(TestStaticGameActorTypes);
      CPPUNIT_TEST(TestEnvironmentTimeConversions);
      CPPUNIT_TEST(TestDefaultProcessMessageRegistration);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   void TestGameActor();
   void TestGameActorProxy();
   void TestSetEnvironmentActor();
   void TestInvokables();
   void TestInvokableMessageRegistration();
   void TestDefaultProcessMessageRegistration();
   void TestGlobalInvokableMessageRegistration();
   void TestStaticGameActorTypes();
   void TestEnvironmentTimeConversions();

private:
   static std::string mTestGameActorLibrary;
   static std::string mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameActorTests);

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
   std::string GameActorTests::mTestGameActorLibrary = "testGameActorLibrary";
   std::string GameActorTests::mTestActorLibrary     = "testActorLibrary";
#else
   std::string GameActorTests::mTestGameActorLibrary = "testGameActorLibrary";
   std::string GameActorTests::mTestActorLibrary     = "testActorLibrary";
#endif

void GameActorTests::setUp()
{
   try
   {
      dtCore::System::Instance()->SetShutdownOnWindowClose(false);
      dtCore::System::Instance()->Start();
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      //dtUtil::Log* logger = &dtUtil::Log::GetInstance("MessageParameter");
      //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);

      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene;
      mManager = new dtGame::GameManager(*scene);
      mManager->LoadActorRegistry(mTestGameActorLibrary);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& ex)
   {
      CPPUNIT_FAIL(std::string("Error: ") + ex.what());
   }
}

void GameActorTests::tearDown()
{
   dtCore::System::Instance()->Stop();
   if (mManager.valid())
   {
      mManager->DeleteAllActors(true);
      mManager->UnloadActorRegistry(mTestGameActorLibrary);
      mManager = NULL;
   }
}


void GameActorTests::TestGameActor()
{
   try
   {

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

void GameActorTests::TestGameActorProxy()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");

      CPPUNIT_ASSERT(actorType != NULL);

      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);
      CPPUNIT_ASSERT_MESSAGE("GameActor should have a reference to the proxy", &gap->GetGameActor().GetGameActorProxy() == gap.get());

      dtGame::GameActor *p = dynamic_cast<dtGame::GameActor*> (&gap->GetGameActor());

      CPPUNIT_ASSERT_MESSAGE("Actor should not be NULL", p != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActor should always return true", gap->IsGameActorProxy());
      CPPUNIT_ASSERT_MESSAGE("Game Actor should not initially be published", !gap->IsPublished());
      CPPUNIT_ASSERT_MESSAGE("Game Actor should not initially be remote", !gap->IsRemote());

      gap->SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy's ownership should have been set", gap->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::SERVER_LOCAL);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

void GameActorTests::TestInvokables()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap != NULL);

      std::vector<dtGame::Invokable*> toFill;
      gap->GetInvokables(toFill);
      CPPUNIT_ASSERT_MESSAGE("The actor should have at least 2 invokables",toFill.size() >= 5);
      dtGame::Invokable* iF = gap->GetInvokable("Fire One");
      dtGame::Invokable* iR = gap->GetInvokable("Reset");
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Fire One ",iF != NULL);
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Reset ",iR != NULL);

      dtDAL::BooleanActorProperty* prop = static_cast<dtDAL::BooleanActorProperty*>(gap->GetProperty("Has Fired"));

      CPPUNIT_ASSERT_MESSAGE("The actor should have a property named \"Has Fired\"", prop != NULL);

      CPPUNIT_ASSERT_MESSAGE("Property \"Has Fired\" should be false", !prop->GetValue());

      //need a dummy message
      dtCore::RefPtr<dtGame::Message> message = mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      iF->Invoke(*message);

      CPPUNIT_ASSERT_MESSAGE("Property \"Has Fired\" should be true", prop->GetValue());

      iR->Invoke(*message);

      CPPUNIT_ASSERT_MESSAGE("Property \"Has Fired\" should be false", !prop->GetValue());

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestInvokableMessageRegistration()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap != NULL);

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
      dtCore::RefPtr<dtGame::Message> message = mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      //this will remove the invokables registration.
      iToggle->Invoke(*message);

      gap->GetMessageHandlers(dtGame::MessageType::TICK_LOCAL, toFill1);
      gap->GetMessageHandlers(dtGame::MessageType::TICK_REMOTE, toFill2);

      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick local", toFill1.size() == 0);
      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick remote", toFill2.size() == 0);

      iToggle->Invoke(*message);

      //now to actually fire them...
      //need a dummy message
      message = mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);
      //so it will reach the actor
      message->SetAboutActorId(gap->GetId());
      //this will remove the invokables registration.
      mManager->AddActor(*gap, false, false);
      mManager->SendMessage(*message);

      CPPUNIT_ASSERT_MESSAGE("Zero local ticks should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Local Tick Count"))->GetValue() == 0);
      CPPUNIT_ASSERT_MESSAGE("Zero remote ticks should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Remote Tick Count"))->GetValue() == 0);

      SLEEP(10);
      dtCore::System::Instance()->Step();

      CPPUNIT_ASSERT_MESSAGE("One local tick should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Local Tick Count"))->GetValue() == 1);
      CPPUNIT_ASSERT_MESSAGE("Zero remote ticks should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Remote Tick Count"))->GetValue() == 0);

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestDefaultProcessMessageRegistration()
{
   try 
   {
      dtCore::RefPtr<dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<dtDAL::ActorType> actor2Type = mManager->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy1 = mManager->CreateActor(*actor1Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*>(proxy1.get());

      dtCore::RefPtr<dtDAL::ActorProxy> proxy2 = mManager->CreateActor(*actor2Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*>(proxy2.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap2 != NULL);

      // Make sure the invokable was created
      dtGame::Invokable* iTestListener = gap2->GetInvokable(dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named \"Process Message\"",iTestListener != NULL);

      mManager->AddActor(*gap1, false, false);
      mManager->AddActor(*gap2, false, false);

      // Make sure we can use proxy method to register for messages
      gap2->RegisterForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      std::vector<std::pair<dtGame::GameActorProxy*, std::string > > toFill;
      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be one registered game actor listener for the actor published message", 
         (unsigned int) 1, (unsigned int) toFill.size());

      // And unregister them too
      gap2->UnregisterForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message", toFill.size() == 0);

      // now use the Register For Self behavior - they only go on the actor itself
      gap2->RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_ACTOR_PUBLISHED, 
         dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);

      // There shouldnt be any listeners on the GM 
      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap2->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should not be any listeners for the actor published message this time", toFill.size() == 0);
      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should not be any listeners for the actor published message this time", toFill.size() == 0);

      mManager->PublishActor(*gap1);
      mManager->PublishActor(*gap2);

      dtCore::System::Instance()->Step();

      // One publish message should have been received. So, count should be 2
      CPPUNIT_ASSERT_EQUAL_MESSAGE("We should only have gotten 1 publish, so count shoudl be 2.",
         (unsigned int) 2, (unsigned int) static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestGlobalInvokableMessageRegistration()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<dtDAL::ActorType> actor2Type = mManager->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy1 = mManager->CreateActor(*actor1Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*>(proxy1.get());

      dtCore::RefPtr<dtDAL::ActorProxy> proxy2 = mManager->CreateActor(*actor2Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*>(proxy2.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap2 != NULL);

      dtGame::Invokable* iTestListener = gap2->GetInvokable("Test Message Listener");

      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named \"Test Message Listener\"",iTestListener != NULL);

      mManager->AddActor(*gap1, false, false);
      mManager->AddActor(*gap2, false, false);

      mManager->RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      std::vector<std::pair<dtGame::GameActorProxy*, std::string > > toFill;

      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered global listener for the Map Loaded message", toFill.size() == 1);

      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message", toFill.size() == 1);

      mManager->UnregisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->UnregisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered global listener for the Map Loaded message", toFill.size() == 0);

      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message", toFill.size() == 0);

      mManager->RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      //std::cout << gap1->GetId() << std::endl;

      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered global listener for the Map Loaded message", toFill.size() == 1);

      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message", toFill.size() == 1);

      mManager->PublishActor(*gap1);
      mManager->SendMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
      mManager->DeleteActor(*gap1);

      //actors are not removed immediately
      CPPUNIT_ASSERT(mManager->FindGameActorById(gap1->GetId()) != NULL);

      SLEEP(10);
      dtCore::System::Instance()->Step();

      //Actor should be removed by now.
      CPPUNIT_ASSERT(mManager->FindGameActorById(gap1->GetId()) == NULL);

      //A publish and map loaded message should have been received, but no listener was added for delete.
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 0);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 1);

      dtCore::RefPtr<dtDAL::ActorProxy> proxy3 = mManager->CreateActor(*actor1Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap3 = dynamic_cast<dtGame::GameActorProxy*>(proxy3.get());

      //add global and actor-specific delete listeners
      mManager->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_DELETED, gap3->GetId(), *gap2, iTestListener->GetName());
      mManager->RegisterForMessages(dtGame::MessageType::INFO_ACTOR_DELETED, *gap2, iTestListener->GetName());
      //take off the publish listener so that when the message shows up, it will not be passed on.
      mManager->UnregisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      //add and publish the actor.
      mManager->AddActor(*gap3, false, true);
      //delete it
      mManager->DeleteActor(*gap3);
      //add a load map message again.
      mManager->SendMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));

      SLEEP(10);
      dtCore::System::Instance()->Step();

      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);

      //test removing the map loaded message and then send one
      mManager->UnregisterForMessages(
         dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->SendMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
      SLEEP(10);
      dtCore::System::Instance()->Step();

      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestSetEnvironmentActor()
{
   try
   {
      dtCore::RefPtr<dtCore::Camera> camera = new dtCore::Camera;
      camera->SetScene(&mManager->GetScene());
      //One cannot enable paging without a window.
      //mManager->GetScene().EnablePaging();
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      dtCore::RefPtr<TestComponent> tc = new TestComponent;
      CPPUNIT_ASSERT(tc.valid());
      mManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      dtCore::RefPtr<dtDAL::ActorType> type = mManager->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test environment actor in the test game library", type.valid());
      dtCore::RefPtr<dtDAL::ActorProxy> ap = mManager->CreateActor(*type);
      CPPUNIT_ASSERT_MESSAGE("The game manager should have been able to create the test environment actor", ap.valid());

      dtCore::RefPtr<TestGameEnvironmentActorProxy> eap = dynamic_cast<TestGameEnvironmentActorProxy*>(ap.get());
      CPPUNIT_ASSERT_MESSAGE("The dynamic cast should not have returned NULL", eap != NULL);
      dtCore::RefPtr<TestGameEnvironmentActor> ea = dynamic_cast<TestGameEnvironmentActor*>(eap->GetActor());
      CPPUNIT_ASSERT_MESSAGE("Should have been able to cast the environment proxy's actor to an environment actor", ea != NULL);

      std::vector<dtDAL::ActorProxy*> actors;
      std::vector<dtCore::RefPtr<dtCore::DeltaDrawable> > drawables;
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());

      type = mManager->FindActorType("ExampleActors", "TestPlayer");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test player actor in the game manager", type.valid());
      const unsigned int numActors = 20;
      for(unsigned int i = 0; i < numActors; i++)
      {
         ap = mManager->CreateActor(*type);
         CPPUNIT_ASSERT_MESSAGE("The game manager should be able to create the test player actor", ap.valid());
         mManager->AddActor(*ap);
      }

      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("The game manager should have the correct number of actors in it", drawables.size() - 1 == numActors);

      // SET ENVIRONMENT ACTOR TESTS
      mManager->SetEnvironmentActor(eap.get());
      //One cannot enable paging without a window.
      //bool shouldBeTrue = mManager->GetScene().IsPagingEnabled();
      //CPPUNIT_ASSERT(shouldBeTrue);
      SLEEP(2);
      dtCore::System::Instance()->Step();
      std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = tc->GetReceivedProcessMessages();
      bool wasMessage = false;
      for(unsigned int i = 0; i < msgs.size(); i++)
      {
         if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
            wasMessage = true;
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should have been sent", wasMessage);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("An environment actor was added.  The game manager should only have "
         " the environment actor and the default scene light.", drawables.size() == 2);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor should now have the number of actors in it", actors.size() == numActors);
      mManager->SetEnvironmentActor(NULL);
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      //mManager->GetScene().DisablePaging();
      msgs.clear();
      tc->reset();
      dtCore::System::Instance()->Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for(unsigned int i = 0; i < msgs.size(); i++)
      {
         if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
            wasMessage = true;
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent", !wasMessage);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Set the environment actor to NULL. The game manager should now have the correct "
            "number of actors in it", drawables.size()-1 == numActors);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor should not have any actors in it", actors.empty());
      mManager->SetEnvironmentActor(NULL);
      //CPPUNIT_ASSERT(!mManager->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::Instance()->Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for(unsigned int i = 0; i < msgs.size(); i++)
      {
         if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
            wasMessage = true;
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent, it was a no-op", !wasMessage);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment from NULL to NULL should not have done anything", actors.empty());
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment from NULL to NULL should not have done anything", drawables.size()-1 == numActors);

      type = mManager->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT(type.valid());
      ap = mManager->CreateActor(*type);
      CPPUNIT_ASSERT(ap.valid());
      dtCore::RefPtr<dtGame::EnvironmentActorProxy> eap2 = dynamic_cast<dtGame::EnvironmentActorProxy*>(ap.get());
      CPPUNIT_ASSERT(eap2 != NULL);
      dtCore::RefPtr<dtGame::EnvironmentActor>      ea2  = dynamic_cast<dtGame::EnvironmentActor*>(eap2->GetActor());
      CPPUNIT_ASSERT(ea2 != NULL);
      mManager->SetEnvironmentActor(eap.get());
      //One cannot enable paging without a window.
      //CPPUNIT_ASSERT(!mManager->GetScene().IsPagingEnabled());
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Once again, the environment actor should now have all the actors", actors.size() == numActors);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Once again, the game manager should have no actors in the scene except the environment actor", drawables.size() == 2);
      //One cannot enable paging without a window.
      //mManager->GetScene().EnablePaging();
      mManager->SetEnvironmentActor(eap2.get());
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::Instance()->Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for(unsigned int i = 0; i < msgs.size(); i++)
      {
         if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
            wasMessage = true;
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should have been sent", wasMessage);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment actor on the game manager from one to another should NOT add any actors to the game manager", drawables.size() == 2);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor was set to another, this environment actor should NOT have any actors", actors.empty());
      ea2->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Since this environment actor was set, it should now have all the actors", actors.size() == numActors);
      mManager->SetEnvironmentActor(NULL);
      //One cannot enable paging without a window.
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::Instance()->Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for(unsigned int i = 0; i < msgs.size(); i++)
      {
         if(msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
            wasMessage = true;
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent", !wasMessage);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());
      ea2->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The previous environment actor should not have any actors", actors.empty());
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("The game manager should now have all the actors", drawables.size() - 1 == numActors);
      mManager->DeleteAllActors(true);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT(drawables.empty());
   }
   catch(const dtUtil::Exception &e)
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
   dtCore::RefPtr<dtDAL::ActorType> types[size] = 
   { 
      TestGameActorLibrary::TEST1_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST2_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_PLAYER_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_TASK_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_COUNTER_TASK_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_TANK_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_JET_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_HELICOPTER_GAME_ACTOR_PROXY_TYPE/*,
      TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE*/
   };
   for(unsigned int i = 0; i < size; i++)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*types[i]);
      CPPUNIT_ASSERT_MESSAGE("The proxy should not be NULL", proxy.valid());
      CPPUNIT_ASSERT_MESSAGE("The proxy's actor should not be NULL", proxy->GetActor() != NULL);
   }
}

void GameActorTests::TestEnvironmentTimeConversions()
{
   dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE);
   CPPUNIT_ASSERT(proxy.valid());
   dtCore::RefPtr<TestGameEnvironmentActor> envActor = dynamic_cast<TestGameEnvironmentActor*>(proxy->GetActor());
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
