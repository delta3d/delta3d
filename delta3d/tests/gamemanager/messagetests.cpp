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
#include <dtDAL/datatype.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actortype.h>
#include <dtGame/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/rulescomponent.h>

#include <cppunit/extensions/HelperMacros.h>

class MessageTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(MessageTests);
        
        CPPUNIT_TEST(TestBaseMessages);
        CPPUNIT_TEST(TestMessageFactory);
        CPPUNIT_TEST(TestMessageDelivery);
        CPPUNIT_TEST(TestActorPublish);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestBaseMessages();
   void TestMessageFactory();
   void TestMessageDelivery();
   void TestActorPublish();

private:
   static char* mTestGameActorLibrary;
   static char* mTestActorLibrary;
   
   dtCore::RefPtr<dtGame::GameManager> mGameManager;
   
};

class TestComponent: public dtGame::GMComponent
{
   public:
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedProcessMessages() 
         { return mReceivedProcessMessages; }
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedSendMessages() 
         { return mReceivedSendMessages; }
      
      virtual void ProcessMessage(const dtGame::Message& msg)
      {
         mReceivedProcessMessages.push_back(&msg);
      }
      virtual void SendMessage(const dtGame::Message& msg)
      {
         mReceivedSendMessages.push_back(&msg);            
      }
      
      void reset() 
      {
         mReceivedSendMessages.clear();
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
      dtCore::RefPtr<const dtGame::Message> FindSendMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedSendMessages.size(); ++i)
         {
            if (mReceivedSendMessages[i]->GetMessageType() == type)
               return mReceivedSendMessages[i];
         }
         return NULL;
      }
   private:
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedProcessMessages;
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedSendMessages;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MessageTests);

#if defined (_DEBUG) && defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
char* MessageTests::mTestGameActorLibrary="testGameActorLibraryd";
char* MessageTests::mTestActorLibrary="testActorLibraryd";
#else
char* MessageTests::mTestGameActorLibrary="testGameActorLibrary";
char* MessageTests::mTestActorLibrary="testActorLibrary";
#endif

void MessageTests::setUp()
{
   dtUtil::Log* logger;
   logger = &dtUtil::Log::GetInstance("MessageParameter");
   logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
     
   dtCore::Scene* scene = new dtCore::Scene();
   mGameManager = new dtGame::GameManager(*scene);
   mGameManager->LoadActorRegistry(mTestGameActorLibrary);
   mGameManager->LoadActorRegistry(mTestActorLibrary);
   
   //dtGame::MessageFactory& msgF = mGameManager->GetMessageFactory();
   
   
}

void MessageTests::tearDown()
{
   try {
      mGameManager->DeleteAllActors();
      mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
      mGameManager->UnloadActorRegistry(mTestActorLibrary);  
      mGameManager = NULL;  
   } catch (const dtUtil::Exception& e) {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

void MessageTests::TestBaseMessages()
{
   try
   {
      dtCore::RefPtr<dtGame::TickMessage> tickMsg = new dtGame::TickMessage();
      dtCore::RefPtr<dtGame::TimerElapsedMessage> timerMsg = new dtGame::TimerElapsedMessage();
      dtCore::RefPtr<dtGame::MapLoadedMessage> mapMsg = new dtGame::MapLoadedMessage();
      dtCore::RefPtr<dtGame::NetServerRejectMessage> netRejectMsg = new dtGame::NetServerRejectMessage();
      dtCore::RefPtr<dtGame::RestartMessage> restartMsg = new dtGame::RestartMessage();
      dtCore::RefPtr<dtGame::ServerMessageRejected> serverMsgRejected = new dtGame::ServerMessageRejected();
      
      CPPUNIT_ASSERT(tickMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT(tickMsg->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(tickMsg->GetSendingActorId().ToString().empty());
      
      tickMsg->SetDeltaRealTime(1.34f);
      tickMsg->SetDeltaSimTime(2.56f);
      tickMsg->SetSimTimeOfDay(100);
      tickMsg->SetSimTimeScale(6.32f);

      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the delta real time", tickMsg->GetDeltaRealTime() == 1.34f);
      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the delta sim time", tickMsg->GetDeltaSimTime() == 2.56f);
      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the sim time of day", tickMsg->GetSimTimeOfDay() == 100);
      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the sim time scale", tickMsg->GetSimTimeScale() == 6.32f);
   
      CPPUNIT_ASSERT(timerMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT(timerMsg->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(timerMsg->GetSendingActorId().ToString().empty());

      timerMsg->SetLateTime(1.0f);
      timerMsg->SetSimTimeOfDay(100);
      timerMsg->SetTimerName("Bob");
      
      CPPUNIT_ASSERT_MESSAGE("Timer message should be able to set the late time", timerMsg->GetLateTime() == 1.0f);
      CPPUNIT_ASSERT_MESSAGE("Timer should be able to set the sime time of day", timerMsg->GetSimTimeOfDay() == 100);
      CPPUNIT_ASSERT_MESSAGE("Timer should be able to the the timer name", timerMsg->GetTimerName() == "Bob");

      CPPUNIT_ASSERT(mapMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT(mapMsg->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(mapMsg->GetSendingActorId().ToString().empty());

      mapMsg->SetLoadedMapName("Some whack map");
      CPPUNIT_ASSERT_MESSAGE("Loaded map message should be able to set the loaded map name", mapMsg->GetLoadedMapName() == "Some whack map");

      CPPUNIT_ASSERT(netRejectMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT(netRejectMsg->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(netRejectMsg->GetSendingActorId().ToString().empty());

      netRejectMsg->SetRejectionMessage("Because it was dumb");
      CPPUNIT_ASSERT_MESSAGE("Net server reject message should be able to set the rejection message", netRejectMsg->GetRejectionMessage() == "Because it was dumb");

      CPPUNIT_ASSERT(restartMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT(restartMsg->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(restartMsg->GetSendingActorId().ToString().empty());

      restartMsg->SetReason("Who cares?");
      CPPUNIT_ASSERT_MESSAGE("Restart message should be able to set its reason", restartMsg->GetReason() == "Who cares?");

      CPPUNIT_ASSERT(serverMsgRejected->GetDestination() == NULL);
      CPPUNIT_ASSERT(serverMsgRejected->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(serverMsgRejected->GetSendingActorId().ToString().empty());

      serverMsgRejected->SetCause("Whack");
      CPPUNIT_ASSERT_MESSAGE("Server message rejected should be able to set its cause", serverMsgRejected->GetCause() == "Whack");
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

void MessageTests::TestMessageFactory()
{
   try
   {
      dtGame::MessageFactory& factory = mGameManager->GetMessageFactory();
      
      CPPUNIT_ASSERT_MESSAGE("Tick local message should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::TICK_LOCAL.GetId()));
      CPPUNIT_ASSERT_MESSAGE("Tick remote message should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::TICK_REMOTE.GetId()));
      CPPUNIT_ASSERT_MESSAGE("Timer elapsed message should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::INFO_TIMER_ELAPSED.GetId()));
      CPPUNIT_ASSERT_MESSAGE("Map loaded message should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::INFO_MAP_LOADED.GetId()));
      CPPUNIT_ASSERT_MESSAGE("Net server reject message message should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::NETSERVER_REJECT_CONNECTION.GetId()));
      CPPUNIT_ASSERT_MESSAGE("Restart message should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::COMMAND_RESTART.GetId()));
      CPPUNIT_ASSERT_MESSAGE("Server message rejected should be in the factory", factory.GetMessageTypeById(dtGame::MessageType::SERVER_REQUEST_REJECTED.GetId()));

      dtCore::RefPtr<dtGame::Message> tickMsg = factory.CreateMessage(dtGame::MessageType::TICK_LOCAL);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a tick local message", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("The Message should have the type TICK_LOCAL",tickMsg->GetMessageType() == dtGame::MessageType::TICK_LOCAL);
      tickMsg = factory.CreateMessage(dtGame::MessageType::TICK_REMOTE);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a tick remote message", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("The Message should have the type TICK_REMOTE",tickMsg->GetMessageType() == dtGame::MessageType::TICK_REMOTE);
      dtCore::RefPtr<dtGame::Message> timerMsg = factory.CreateMessage(dtGame::MessageType::INFO_TIMER_ELAPSED);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a timer elapsed message", timerMsg != NULL);
      dtCore::RefPtr<dtGame::Message> mapMsg = factory.CreateMessage(dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a map loaded message", mapMsg != NULL);
      dtCore::RefPtr<dtGame::Message> netMsg = factory.CreateMessage(dtGame::MessageType::NETSERVER_REJECT_CONNECTION);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a net server reject connection message", netMsg != NULL);
      dtCore::RefPtr<dtGame::Message> restartMsg = factory.CreateMessage(dtGame::MessageType::COMMAND_RESTART);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a restart message", restartMsg != NULL);
      dtCore::RefPtr<dtGame::Message> serverMsg = factory.CreateMessage(dtGame::MessageType::SERVER_REQUEST_REJECTED);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a server message rejected message", serverMsg != NULL);
      
      CPPUNIT_ASSERT_MESSAGE("Tick message's type should have been set correctly", tickMsg->GetMessageType() == dtGame::MessageType::TICK_REMOTE);
      CPPUNIT_ASSERT_MESSAGE("Timer elapsed message's type should have been set correctly", timerMsg->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED);
      CPPUNIT_ASSERT_MESSAGE("Map loaded message's type should have been set correctly", mapMsg->GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("Net server reject connection message's type should have been set correctly", netMsg->GetMessageType() == dtGame::MessageType::NETSERVER_REJECT_CONNECTION);
      CPPUNIT_ASSERT_MESSAGE("Restart message's type should have been set correctly", restartMsg->GetMessageType() == dtGame::MessageType::COMMAND_RESTART);
      CPPUNIT_ASSERT_MESSAGE("Server message rejected message's type should have been set correctly", serverMsg->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED);
   
      std::vector<const dtGame::MessageType*> v;
      factory.GetSupportedMessageTypes(v);

      CPPUNIT_ASSERT_MESSAGE("List of supported message types should not be 0", !v.empty());

      CPPUNIT_ASSERT_MESSAGE("Tick local message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::TICK_LOCAL));
      CPPUNIT_ASSERT_MESSAGE("Tick remote message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::TICK_REMOTE));
      CPPUNIT_ASSERT_MESSAGE("Info timer elapsed message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::INFO_TIMER_ELAPSED));
      CPPUNIT_ASSERT_MESSAGE("Info map loaded message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::INFO_MAP_LOADED));
      CPPUNIT_ASSERT_MESSAGE("Net server rejection message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::NETSERVER_REJECT_CONNECTION));
      CPPUNIT_ASSERT_MESSAGE("Restart message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::COMMAND_RESTART));
      CPPUNIT_ASSERT_MESSAGE("Server request rejected message should be supported", factory.IsMessageTypeSupported(dtGame::MessageType::SERVER_REQUEST_REJECTED));

      // Paranoia, but better safe than sorry
      for(unsigned int i = 0; i < v.size(); i++)
         CPPUNIT_ASSERT_MESSAGE("The vector of supported message types should register as supported in the factory", factory.IsMessageTypeSupported(*v[i]));

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

void MessageTests::TestMessageDelivery()
{
   try
   {
      dtCore::RefPtr<TestComponent> tc = new TestComponent();

      mGameManager->AddComponent(*tc);

      dtCore::RefPtr<dtGame::Message> msg = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED);

      CPPUNIT_ASSERT_MESSAGE("The message created should not be NULL", msg != NULL);      
      
      dtCore::RefPtr<dtDAL::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
      
      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtDAL::ActorProxy> gap = mGameManager->CreateActor(*type);

      CPPUNIT_ASSERT(gap->IsGameActorProxy());
      mGameManager->AddActor(*dynamic_cast<dtGame::GameActorProxy*>(gap.get()), false, false);
      std::vector<const dtDAL::ActorType*> vec;
      mGameManager->GetActorTypes(vec);
      CPPUNIT_ASSERT_MESSAGE("The amount of actor types supported should not be 0", vec.size() != 0);
      
      mGameManager->SetTimeScale(1.5);
      double deltaTime[2] = { 0.233, 0.233 };
      
      tc->reset();
      dtCore::System::Instance()->SendMessage("preframe", &deltaTime);
      for (unsigned i = 0; i < tc->GetReceivedProcessMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedProcessMessages()[i].valid());
         //std::cout << tc->GetReceivedProcessMessages()[i]->GetMessageType()->GetName();
      } 

      dtCore::RefPtr<const dtGame::Message> tickMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
      dtCore::RefPtr<const dtGame::Message> tickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);
      dtCore::RefPtr<const dtGame::Message> actorCreatedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      dtCore::RefPtr<const dtGame::Message> publishedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have been processed.", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have been processed.", tickRemoteMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have been processed.", actorCreatedMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("No Actor Published message should have been processed.", publishedMsg == NULL);

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have GetSource matching the GameManager.", tickMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have GetSource matching the GameManager.", tickRemoteMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have GetSource matching the GameManager.", actorCreatedMsg->GetSource() == mGameManager->GetMachineInfo());

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have GetDestination matching the GameManager.", *tickMsg->GetDestination() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have GetDestination matching the GameManager.", *tickRemoteMsg->GetDestination() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have GetDestination matching the GameManager.", *actorCreatedMsg->GetDestination() == mGameManager->GetMachineInfo());

      mGameManager->RemoveComponent(*tc);
      CPPUNIT_ASSERT(tc->GetGameManager() == NULL);      
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

void MessageTests::TestActorPublish()
{
   try
   {
      TestComponent* tc = new TestComponent();
      dtGame::RulesComponent* rc = new dtGame::RulesComponent();

      mGameManager->AddComponent(*tc);
      mGameManager->AddComponent(*rc);

      CPPUNIT_ASSERT(tc->GetGameManager() != NULL);
      CPPUNIT_ASSERT(rc->GetGameManager() != NULL);

      dtCore::RefPtr<dtDAL::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
      
      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtDAL::ActorProxy> ap = mGameManager->CreateActor(*type);

      CPPUNIT_ASSERT(ap->IsGameActorProxy());
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
      CPPUNIT_ASSERT(gap != NULL);
     
      try 
      {
         mGameManager->AddActor(*gap, true, true);
         CPPUNIT_FAIL("Adding an actor with both remote and true should fail.");
      } 
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_ASSERT(ex.TypeEnum() == dtGame::ExceptionEnum::ACTOR_IS_REMOTE); 
      }
      
      //now we really add the actor as published.
      mGameManager->AddActor(*gap, false, true);

      CPPUNIT_ASSERT(!gap->IsRemote());
      CPPUNIT_ASSERT(gap->IsPublished());

      mGameManager->SetTimeScale(1.5);
      double deltaTime[2] = { 0.233, 0.233 };
      
      tc->reset();
      dtCore::System::Instance()->SendMessage("preframe", &deltaTime);
      for (unsigned i = 0; i < tc->GetReceivedProcessMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedProcessMessages()[i].valid());
         //std::cout << tc->GetReceivedProcessMessages()[i]->GetMessageType().GetName();
      } 

      dtCore::RefPtr<const dtGame::Message> tickMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
      dtCore::RefPtr<const dtGame::Message> tickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);
      dtCore::RefPtr<const dtGame::Message> actorCreatedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      dtCore::RefPtr<const dtGame::Message> publishedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have been processed.", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have been processed.", tickRemoteMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have been processed.", actorCreatedMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("An Actor Published message should have been processed.", publishedMsg != NULL);

      CPPUNIT_ASSERT_MESSAGE("The Tick Local message should have GetSource matching the GameManager.", tickMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Tick Remote message should have GetSource matching the GameManager.", tickRemoteMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Actor Created message should have GetSource matching the GameManager.", actorCreatedMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Actor Published message should have GetSource matching the GameManager.", publishedMsg->GetSource() == mGameManager->GetMachineInfo());

      CPPUNIT_ASSERT_MESSAGE("The Tick Local message should have GetDestination matching the GameManager.", tickMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Tick Remote message should have GetDestination matching the GameManager.", tickRemoteMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Actor Created message should have GetDestination matching the GameManager.", actorCreatedMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Actor Published message should have GetDestination matching the GameManager.", publishedMsg->GetDestination() == &mGameManager->GetMachineInfo());

      dtCore::RefPtr<const dtGame::Message> sendActorCreateMsg = tc->FindSendMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      dtCore::RefPtr<const dtGame::Message> sendPublishedMsg = tc->FindSendMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);
      
      CPPUNIT_ASSERT(sendActorCreateMsg == NULL);
      CPPUNIT_ASSERT(sendPublishedMsg == NULL);
      
      //Another Frame...
      dtCore::System::Instance()->SendMessage("preframe", &deltaTime);
      
      CPPUNIT_ASSERT(tc->GetReceivedSendMessages().size() > 0);
      for (unsigned i = 0; i < tc->GetReceivedSendMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedSendMessages()[i].valid());
         std::cout << tc->GetReceivedSendMessages()[i]->GetMessageType().GetName();
      } 

      sendActorCreateMsg = tc->FindSendMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      sendPublishedMsg = tc->FindSendMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);
      
      CPPUNIT_ASSERT_MESSAGE("An actor create message should have been sent.", sendActorCreateMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("No publish message should be been sent.", sendPublishedMsg == NULL);

      CPPUNIT_ASSERT_MESSAGE("The Actor Create message should have GetSource matching the GameManager.", sendActorCreateMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Actor Create message should have GetDestination of NULL.", sendActorCreateMsg->GetDestination() == NULL);
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
