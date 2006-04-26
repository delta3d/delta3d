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

#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class GameActorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(GameActorTests);
        
      CPPUNIT_TEST(TestGameActor);
      CPPUNIT_TEST(TestGameActorProxy);
      CPPUNIT_TEST(TestInvokables);
      CPPUNIT_TEST(TestInvokableMessageRegistration);
      CPPUNIT_TEST(TestGlobalInvokableMessageRegistration);     
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   void TestGameActor();
   void TestGameActorProxy();
   void TestInvokables();
   void TestInvokableMessageRegistration();
   void TestGlobalInvokableMessageRegistration();

private:
   static char* mTestGameActorLibrary;
   static char* mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameActorTests);

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* GameActorTests::mTestGameActorLibrary="testGameActorLibraryd";
char* GameActorTests::mTestActorLibrary="testActorLibraryd";
#else
char* GameActorTests::mTestGameActorLibrary="testGameActorLibrary";
char* GameActorTests::mTestActorLibrary="testActorLibrary";
#endif

void GameActorTests::setUp()
{
   try 
   {
      dtUtil::Log* logger;
      logger = &dtUtil::Log::GetInstance("MessageParameter");
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      //logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
       
      dtCore::Scene* scene = new dtCore::Scene();
      mManager = new dtGame::GameManager(*scene);
      mManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::Instance()->Start();
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
      mManager->DeleteAllActors();
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
      mManager->ProcessMessage(*message);

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
      
      mManager->RegisterGlobalMessageListener(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->RegisterGameActorMessageListener(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());
      
      std::vector<std::pair<dtGame::GameActorProxy*, std::string > > toFill;
      
      mManager->GetGlobalMessageListeners(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered global listener for the Map Loaded message", toFill.size() == 1);
      
      mManager->GetGameActorMessageListeners(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message", toFill.size() == 1);
      
      mManager->UnregisterGlobalMessageListener(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->UnregisterGameActorMessageListener(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      mManager->GetGlobalMessageListeners(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered global listener for the Map Loaded message", toFill.size() == 0);
      
      mManager->GetGameActorMessageListeners(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message", toFill.size() == 0);
            
      mManager->RegisterGlobalMessageListener(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->RegisterGameActorMessageListener(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      //std::cout << gap1->GetId().ToString() << std::endl;
      
      mManager->GetGlobalMessageListeners(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered global listener for the Map Loaded message", toFill.size() == 1);
      
      mManager->GetGameActorMessageListeners(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message", toFill.size() == 1);

      mManager->PublishActor(*gap1);
      mManager->ProcessMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
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
      mManager->RegisterGameActorMessageListener(dtGame::MessageType::INFO_ACTOR_DELETED, gap3->GetId(), *gap2, iTestListener->GetName());
      mManager->RegisterGlobalMessageListener(dtGame::MessageType::INFO_ACTOR_DELETED, *gap2, iTestListener->GetName());
      //take off the publish listener so that when the message shows up, it will not be passed on.
      mManager->UnregisterGameActorMessageListener(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      //add and publish the actor.
      mManager->AddActor(*gap3, false, true);
      //delete it
      mManager->DeleteActor(*gap3);
      //add a load map message again.
      mManager->ProcessMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));

      SLEEP(10);
      dtCore::System::Instance()->Step();

      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);
      
      //test removing the map loaded message and then send one
      mManager->UnregisterGlobalMessageListener(
         dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->ProcessMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
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
