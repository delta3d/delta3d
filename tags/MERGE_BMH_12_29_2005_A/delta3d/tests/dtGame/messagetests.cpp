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
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/fileutils.h>
#include <dtDAL/actorproperty.h>
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
#include <dtGame/defaultmessageprocessor.h>

#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define sleep(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define sleep(milliseconds) usleep(((milliseconds) * 1000))
#endif

class MessageTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(MessageTests);
        
      CPPUNIT_TEST(TestBaseMessages);
      CPPUNIT_TEST(TestMessageFactory);
      CPPUNIT_TEST(TestMessageDelivery);
      CPPUNIT_TEST(TestActorPublish);
      CPPUNIT_TEST(TestPauseResume);
      CPPUNIT_TEST(TestPauseResumeSystem);
      CPPUNIT_TEST(TestRejectMessage);
      CPPUNIT_TEST(TestTimeScaling);
      CPPUNIT_TEST(TestTimeChange);
      CPPUNIT_TEST(TestChangeMap);
      CPPUNIT_TEST(TestChangeMapErrorConditions);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithPauseResumeCommands);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorCreates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithLocalActorCreates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorUpdates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithLocalActorUpdates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorDeletes);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithLocalActorDeletes);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestBaseMessages();
   void TestMessageFactory();
   void TestMessageDelivery();
   void TestActorPublish();
   void TestPauseResume();
   void TestPauseResumeSystem();
   void TestRejectMessage();
   void TestTimeScaling();
   void TestTimeChange();
   void TestChangeMap();
   void TestChangeMapErrorConditions();
   void TestDefaultMessageProcessorWithPauseResumeCommands();
   void TestDefaultMessageProcessorWithRemoteActorCreates();
   void TestDefaultMessageProcessorWithLocalActorCreates();
   void TestDefaultMessageProcessorWithRemoteActorUpdates();
   void TestDefaultMessageProcessorWithLocalActorUpdates();
   void TestDefaultMessageProcessorWithRemoteActorDeletes();
   void TestDefaultMessageProcessorWithLocalActorDeletes();
   
private:
   static char* mTestGameActorLibrary;
   static char* mTestActorLibrary;

   void createActors(dtDAL::Map& map);
   void TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(bool remote);
   void TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(bool remote);
   void TestDefaultMessageProcessorWithLocalOrRemoteActorDeletes(bool remote);
   dtUtil::Log* mLogger;
   
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

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* MessageTests::mTestGameActorLibrary="testGameActorLibraryd";
char* MessageTests::mTestActorLibrary="testActorLibraryd";
#else
char* MessageTests::mTestGameActorLibrary="testGameActorLibrary";
char* MessageTests::mTestActorLibrary="testActorLibrary";
#endif

//////////////////////////////////////////////////////////////////////////
void MessageTests::setUp()
{
   try 
   {
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mLogger = &dtUtil::Log::GetInstance("messagetests.cpp");
      //mLogger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
        
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::GameManager(*scene);
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      mGameManager->LoadActorRegistry(mTestActorLibrary);
      dtCore::System::Instance()->Start();
      
      dtDAL::Project::GetInstance().SetContext("gamemanager/TestGameProject");
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL((std::string("Error: ") + ex.What()).c_str());
   }
      
   //dtGame::MessageFactory& msgF = mGameManager->GetMessageFactory();
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::tearDown()
{
   if (mGameManager.valid())
   {
      try 
      {
         
         dtCore::System::Instance()->SetPause(false);
         dtCore::System::Instance()->Stop();
         
         if (!mGameManager->GetCurrentMap().empty())
         {
            dtDAL::Project::GetInstance().CloseMap(dtDAL::Project::GetInstance().GetMap(mGameManager->GetCurrentMap()), true);
         }
         
         mGameManager->DeleteAllActors();
         mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
         mGameManager->UnloadActorRegistry(mTestActorLibrary);  
         mGameManager = NULL;  
      } 
      catch (const dtUtil::Exception& e) 
      {
            CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
      
   }
   
   try 
   {
      dtDAL::FileUtils::GetInstance().DirDelete("gamemanager/TestGameProject", true);
   }
   catch (const dtUtil::Exception& e) 
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
}

void MessageTests::createActors(dtDAL::Map& map) 
{
   std::vector<dtCore::RefPtr<dtDAL::ActorType> > actors;
   std::vector<dtDAL::ActorProperty *> props;
   
   mGameManager->GetActorTypes(actors);
   
   int nameCounter = 0;
   char nameAsString[21];
   
   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, "Adding one of each proxy type to the map:");
   
   for (unsigned int i=0; i< actors.size(); i++) 
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy;
      
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                         "Creating actor proxy %s with category %s.", actors[i]->GetName().c_str(), actors[i]->GetCategory().c_str());
      
      proxy = mGameManager->CreateActor(*actors[i]);
      snprintf(nameAsString, 21, "%d", nameCounter);
      proxy->SetName(std::string(nameAsString));
      nameCounter++;
      
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                         "Set proxy name to: %s", proxy->GetName().c_str());
      
      
      proxy->GetPropertyList(props);
      for (unsigned int j=0; j<props.size(); j++) {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                            "Property: Name: %s, Type: %s",
                            props[j]->GetName().c_str(), props[j]->GetPropertyType().GetName().c_str());
      }
      
      map.AddProxy(*proxy);
      
      CPPUNIT_ASSERT_MESSAGE("Proxy list has the wrong size.",
                             map.GetAllProxies().size() == i + 1);
      CPPUNIT_ASSERT_MESSAGE("Last proxy in the list should equal the new proxy.",
                             map.GetAllProxies().find(proxy->GetId())->second == proxy.get());
   }
}


//////////////////////////////////////////////////////////////////////////
void MessageTests::TestBaseMessages()
{
   try
   {
      dtCore::RefPtr<dtGame::TickMessage> tickMsg = new dtGame::TickMessage();
      dtCore::RefPtr<dtGame::TimeChangeMessage> timeChangeMessage = new dtGame::TimeChangeMessage();
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
      tickMsg->SetSimulationTime(103330.000);
      tickMsg->SetSimTimeScale(6.32f);

      CPPUNIT_ASSERT_MESSAGE("Tick message causing message should be NULL.", tickMsg->GetCausingMessage() == NULL);
      tickMsg->SetCausingMessage(timerMsg.get());

      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the delta real time", tickMsg->GetDeltaRealTime() == 1.34f);
      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the delta sim time", tickMsg->GetDeltaSimTime() == 2.56f);
      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the sim time of day", osg::equivalent(tickMsg->GetSimulationTime(), 103330.000, 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Tick message should be able to set the sim time scale", tickMsg->GetSimTimeScale() == 6.32f);
      CPPUNIT_ASSERT_MESSAGE("Tick message causing message should be the timer message in this test.", tickMsg->GetCausingMessage() == timerMsg.get());
      tickMsg->SetCausingMessage(NULL);
      CPPUNIT_ASSERT_MESSAGE("Tick message causing message should be NULL.", tickMsg->GetCausingMessage() == NULL);

      timeChangeMessage->SetTimeScale(1.8f);
      timeChangeMessage->SetSimulationTime(1.99);
      timeChangeMessage->SetSimulationClockTime(382930);
 
      CPPUNIT_ASSERT(osg::equivalent(timeChangeMessage->GetTimeScale(), 1.8f, 1e-1f));
      CPPUNIT_ASSERT(osg::equivalent(timeChangeMessage->GetSimulationTime(), 1.99, 1e-2));
      CPPUNIT_ASSERT(timeChangeMessage->GetSimulationClockTime() == 382930);
   
      CPPUNIT_ASSERT(timerMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT(timerMsg->GetAboutActorId().ToString().empty());
      CPPUNIT_ASSERT(timerMsg->GetSendingActorId().ToString().empty());

      timerMsg->SetLateTime(1.0f);
      timerMsg->SetTimerName("Bob");
      
      CPPUNIT_ASSERT_MESSAGE("Timer message should be able to set the late time", timerMsg->GetLateTime() == 1.0f);
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
   
      dtCore::RefPtr<dtGame::ActorUpdateMessage> aum1 = new dtGame::ActorUpdateMessage;
      dtCore::RefPtr<dtGame::ActorUpdateMessage> aum2 = new dtGame::ActorUpdateMessage;
      aum1->SetAboutActorId(dtCore::UniqueId());
      dtGame::DataStream stream;
      
      dtGame::MessageParameter *o = aum1->AddUpdateParameter("param1", dtDAL::DataType::FLOAT);
      dtGame::MessageParameter *t = aum1->AddUpdateParameter("param2", dtDAL::DataType::INT);
      dtGame::MessageParameter *h = aum1->AddUpdateParameter("param3", dtDAL::DataType::DOUBLE);
      dtGame::MessageParameter *f = aum1->AddUpdateParameter("param4", dtDAL::DataType::BOOLEAN);
      
      CPPUNIT_ASSERT_MESSAGE("MessageParameter o should not be NULL", o != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter t should not be NULL", t != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter h should not be NULL", h != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter f should not be NULL", f != NULL);

      static_cast<dtGame::FloatMessageParameter*>(o)->SetValue(1.0f);
      static_cast<dtGame::IntMessageParameter*>(t)->SetValue(5);
      static_cast<dtGame::DoubleMessageParameter*>(h)->SetValue(1.5);
      static_cast<dtGame::BooleanMessageParameter*>(f)->SetValue(true);

      aum1->ToDataStream(stream);
      
      aum2->FromDataStream(stream);

      dtGame::MessageParameter *one   = aum2->GetUpdateParameter("param1");
      dtGame::MessageParameter *two   = aum2->GetUpdateParameter("param2");
      dtGame::MessageParameter *three = aum2->GetUpdateParameter("param3");
      dtGame::MessageParameter *four  = aum2->GetUpdateParameter("param4");

      CPPUNIT_ASSERT_MESSAGE("MessageParameter 1 should not be NULL", one != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter 2 should not be NULL", two != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter 3 should not be NULL", three != NULL);
      CPPUNIT_ASSERT_MESSAGE("MessageParameter 4 should not be NULL", four != NULL);

      CPPUNIT_ASSERT_MESSAGE("MessageParameter 1 should equal the value",
         osg::equivalent(static_cast<dtGame::FloatMessageParameter*>(one)->GetValue(),
         static_cast<dtGame::FloatMessageParameter*>(o)->GetValue(), 1e-2f));
      CPPUNIT_ASSERT_MESSAGE("MessageParameter 2 should equal the value", 
         static_cast<dtGame::IntMessageParameter*>(three)->GetValue() ==
         static_cast<dtGame::IntMessageParameter*>(h)->GetValue());      
      CPPUNIT_ASSERT_MESSAGE("MessageParameter 3 should equal the value", 
         osg::equivalent(static_cast<dtGame::DoubleMessageParameter*>(three)->GetValue(),
         static_cast<dtGame::DoubleMessageParameter*>(h)->GetValue(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("MessageParameter 4 should equal the value", 
         static_cast<dtGame::BooleanMessageParameter*>(three)->GetValue() ==
         static_cast<dtGame::BooleanMessageParameter*>(h)->GetValue() );

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   //catch (const std::exception &e)
   //{
   //   CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   //}
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestMessageFactory()
{
   try
   {
      dtGame::MessageFactory& factory = mGameManager->GetMessageFactory();
      
      factory.GetMessageTypeById(dtGame::MessageType::TICK_LOCAL.GetId());
      factory.GetMessageTypeById(dtGame::MessageType::TICK_REMOTE.GetId());
      factory.GetMessageTypeById(dtGame::MessageType::INFO_TIMER_ELAPSED.GetId());
      factory.GetMessageTypeById(dtGame::MessageType::INFO_MAP_LOADED.GetId());
      factory.GetMessageTypeById(dtGame::MessageType::NETSERVER_REJECT_CONNECTION.GetId());
      factory.GetMessageTypeById(dtGame::MessageType::COMMAND_RESTART.GetId());
      factory.GetMessageTypeById(dtGame::MessageType::SERVER_REQUEST_REJECTED.GetId());

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

      static_cast<dtGame::TickMessage*>(tickMsg.get())->SetDeltaRealTime(1.34f);
      static_cast<dtGame::TickMessage*>(tickMsg.get())->SetDeltaSimTime(2.56f);
      static_cast<dtGame::TickMessage*>(tickMsg.get())->SetSimulationTime(103330.000);
      static_cast<dtGame::TickMessage*>(tickMsg.get())->SetSimTimeScale(6.32f);

      static_cast<dtGame::TimerElapsedMessage*>(timerMsg.get())->SetLateTime(1.0f);
      static_cast<dtGame::TimerElapsedMessage*>(timerMsg.get())->SetTimerName("Bob");

      tickMsg->SetCausingMessage(timerMsg.get());
      timerMsg->SetCausingMessage(serverMsg.get());
      
      dtCore::RefPtr<dtGame::Message> tickMsgClone = factory.CloneMessage(*tickMsg);
      
      CPPUNIT_ASSERT_MESSAGE("Tick message's type should have been set correctly", tickMsgClone->GetMessageType() == dtGame::MessageType::TICK_REMOTE);
      CPPUNIT_ASSERT_MESSAGE("Tick message clone should have the proper the delta real time", static_cast<dtGame::TickMessage*>(tickMsgClone.get())->GetDeltaRealTime() == 1.34f);
      CPPUNIT_ASSERT_MESSAGE("Tick message clone should have the proper the delta sim time", static_cast<dtGame::TickMessage*>(tickMsgClone.get())->GetDeltaSimTime() == 2.56f);
      CPPUNIT_ASSERT_MESSAGE("Tick message clone should have the proper the sim time of day", osg::equivalent(static_cast<dtGame::TickMessage*>(tickMsgClone.get())->GetSimulationTime(), 103330.000, 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Tick message clone should have the proper the sim time scale", static_cast<dtGame::TickMessage*>(tickMsgClone.get())->GetSimTimeScale() == 6.32f);

      dtCore::RefPtr<const dtGame::Message> timerMsgClone = tickMsgClone->GetCausingMessage();
      
      CPPUNIT_ASSERT_MESSAGE("Timer elapsed message clone should not be NULL", timerMsgClone != NULL);
      CPPUNIT_ASSERT_MESSAGE("Timer elapsed message's type should have been set correctly", timerMsgClone->GetMessageType() == dtGame::MessageType::INFO_TIMER_ELAPSED);
      CPPUNIT_ASSERT_MESSAGE("Timer message clone should have the proper the late time", static_cast<const dtGame::TimerElapsedMessage*>(timerMsgClone.get())->GetLateTime() == 1.0f);
      CPPUNIT_ASSERT_MESSAGE("Timer message clone should have the proper timer name", static_cast<const dtGame::TimerElapsedMessage*>(timerMsgClone.get())->GetTimerName() == "Bob");

      dtCore::RefPtr<const dtGame::Message> serverMsgClone = timerMsgClone->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Server message rejected clone should not be NULL", serverMsgClone != NULL);
      CPPUNIT_ASSERT_MESSAGE("Server message rejected message's type should have been set correctly", serverMsg->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED);
      
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

//////////////////////////////////////////////////////////////////////////
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
      
      mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 1.5, mGameManager->GetSimulationClockTime());
      
      tc->reset();
      sleep(10);
      dtCore::System::Instance()->Step();
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

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have GetDestination matching the GameManager.", tickMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have GetDestination matching the GameManager.", tickRemoteMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have GetDestination matching the GameManager.", actorCreatedMsg->GetDestination() == NULL);

      mGameManager->RemoveComponent(*tc);
      CPPUNIT_ASSERT(tc->GetGameManager() == NULL);      
   }
   catch (const dtUtil::Exception &e) 
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch (const std::exception &e) 
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

//////////////////////////////////////////////////////////////////////////
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

      mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 1.5, mGameManager->GetSimulationClockTime());
      tc->reset();
      sleep(10);
      dtCore::System::Instance()->Step();
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
      CPPUNIT_ASSERT_MESSAGE("The Actor Created message should have GetDestination of NULL.", actorCreatedMsg->GetDestination() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The Actor Published message should have GetDestination matching the GameManager.", publishedMsg->GetDestination() == &mGameManager->GetMachineInfo());

      dtCore::RefPtr<const dtGame::Message> sendActorCreateMsg = tc->FindSendMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      dtCore::RefPtr<const dtGame::Message> sendPublishedMsg = tc->FindSendMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);
      
      CPPUNIT_ASSERT(sendActorCreateMsg == NULL);
      CPPUNIT_ASSERT(sendPublishedMsg == NULL);
      
      //Another Frame...
      sleep(10);
      dtCore::System::Instance()->Step();
      
      CPPUNIT_ASSERT(tc->GetReceivedSendMessages().size() > 0);
      for (unsigned i = 0; i < tc->GetReceivedSendMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedSendMessages()[i].valid());
         //std::cout << tc->GetReceivedSendMessages()[i]->GetMessageType().GetName();
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
//   catch (const std::exception &e) 
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestPauseResume()
{
   TestComponent* tc = new TestComponent();
   mGameManager->AddComponent(*tc);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());
   
   mGameManager->SetPaused(true);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", mGameManager->IsPaused());
   CPPUNIT_ASSERT_MESSAGE("System should be paused.", dtCore::System::Instance()->GetPause());
   
   sleep(10);
   dtCore::System::Instance()->Step();
   
   dtCore::RefPtr<const dtGame::Message> processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
   dtCore::RefPtr<const dtGame::Message> processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);
   
   CPPUNIT_ASSERT_MESSAGE("A paused message should have been processed.", processPausedMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A resumed message should not have been processed.", !processResumedMsg.valid());

   dtCore::Timer_t oldSimClockTime = mGameManager->GetSimulationClockTime();
   double oldSimTime = mGameManager->GetSimulationTime();

   sleep(10);
   dtCore::System::Instance()->Step();

   dtCore::RefPtr<const dtGame::Message> processTickLocalMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
   dtCore::RefPtr<const dtGame::Message> processTickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);
   
   CPPUNIT_ASSERT_MESSAGE("A tick local message should have been processed.", processTickLocalMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A tick remote message should not have been processed.", processTickRemoteMsg.valid());

   const dtGame::TickMessage* tickLocal = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 
   const dtGame::TickMessage* tickRemote = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 

   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be 0", osg::equivalent(tickLocal->GetDeltaSimTime(), 0.0f, 1e-9f));
   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be 0", osg::equivalent(tickRemote->GetDeltaSimTime(), 0.0f, 1e-9f));

   CPPUNIT_ASSERT_MESSAGE("Simulation time should be unchanged", osg::equivalent(tickLocal->GetSimulationTime(), oldSimTime, 1e-9));
   CPPUNIT_ASSERT_MESSAGE("Simulation time should be unchanged", osg::equivalent(tickRemote->GetSimulationTime(), oldSimTime, 1e-9));

   CPPUNIT_ASSERT_MESSAGE("Simulation time should be unchanged on GM", osg::equivalent(oldSimTime, mGameManager->GetSimulationTime(), 1e-9));
   CPPUNIT_ASSERT_MESSAGE("Simulation clock time should be unchanged on GM", oldSimClockTime == mGameManager->GetSimulationClockTime());

   tc->reset();

   mGameManager->SetPaused(false);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", !mGameManager->IsPaused());
   CPPUNIT_ASSERT_MESSAGE("System should be paused.", !dtCore::System::Instance()->GetPause());
   
   sleep(10);
   dtCore::System::Instance()->Step();
   
   processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
   processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);
   
   CPPUNIT_ASSERT_MESSAGE("A paused message should not have been processed.", !processPausedMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A resumed message should have been processed.", processResumedMsg.valid());
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestPauseResumeSystem()
{
   TestComponent* tc = new TestComponent();
   mGameManager->AddComponent(*tc);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());
   
   dtCore::System::Instance()->SetPause(true);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", mGameManager->IsPaused());
   CPPUNIT_ASSERT_MESSAGE("System should be paused.", dtCore::System::Instance()->GetPause());
   
   sleep(10);
   dtCore::System::Instance()->Step();
   
   dtCore::RefPtr<const dtGame::Message> processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
   dtCore::RefPtr<const dtGame::Message> processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);
   
   CPPUNIT_ASSERT_MESSAGE("A paused message should have been processed.", processPausedMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A resumed message should not have been processed.", !processResumedMsg.valid());

   tc->reset();

   dtCore::System::Instance()->SetPause(false);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", !mGameManager->IsPaused());
   CPPUNIT_ASSERT_MESSAGE("System should be paused.", !dtCore::System::Instance()->GetPause());
   
   sleep(10);
   dtCore::System::Instance()->Step();
   
   processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
   processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);
   
   CPPUNIT_ASSERT_MESSAGE("A paused message should not have been processed.", !processPausedMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A resumed message should have been processed.", processResumedMsg.valid());

}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestRejectMessage()
{
   TestComponent* tc = new TestComponent();
   mGameManager->AddComponent(*tc);
   tc->reset();

   // reject some message - any ole message will do.

   dtCore::RefPtr<dtGame::Message> msg = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_PAUSE);
   mGameManager->RejectMessage(*msg, "test reason");
   sleep(10);
   dtCore::System::Instance()->Step();

   // message should have been processed (not sent)

   dtCore::RefPtr<const dtGame::Message> processedRejection1 = tc->FindProcessMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED);
   dtCore::RefPtr<const dtGame::Message> sentRejection1 = tc->FindSendMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED); 
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have been processed.", processedRejection1.valid());
   CPPUNIT_ASSERT_MESSAGE("Local reject message should NOT have been sent.", !sentRejection1.valid());

   // processed message should have the right data 

   const dtGame::ServerMessageRejected *pMsg1 = static_cast<const dtGame::ServerMessageRejected*> (processedRejection1.get());
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have the reason.", pMsg1->GetCause() == "test reason");
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have our machine.", pMsg1->GetSource() == mGameManager->GetMachineInfo());
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have the destination.", *pMsg1->GetDestination() == mGameManager->GetMachineInfo());
   const dtGame::Message *causeMsg1 = pMsg1->GetCausingMessage();
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have the causing message.", 
      causeMsg1->GetMessageType() == dtGame::MessageType::REQUEST_PAUSE);

   tc->reset();

   // Now, reject a message that goes to a different machine info

   dtCore::RefPtr<dtGame::Message> msg2 = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_PAUSE);
   dtCore::RefPtr<dtGame::MachineInfo> testMachine = new dtGame::MachineInfo(); // has a unique id
   //msg2->SetDestination(&(*testMachine));
   msg2->SetSource(*testMachine);
   mGameManager->RejectMessage(*msg2, "test reason2");
   sleep(10);
   dtCore::System::Instance()->Step();

   // message should have been sent (not processed)

   dtCore::RefPtr<const dtGame::Message> processedRejection2 = tc->FindProcessMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED);
   dtCore::RefPtr<const dtGame::Message> sentRejection2 = tc->FindSendMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED); 
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should NOT have been processed.", !processedRejection2.valid());
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have been sent.", sentRejection2.valid());

   // sent message should have the right data 

   const dtGame::ServerMessageRejected *pMsg2 = static_cast<const dtGame::ServerMessageRejected*> (sentRejection2.get());
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the reason.", pMsg2->GetCause() == "test reason2");
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have our machine.", pMsg2->GetSource() == mGameManager->GetMachineInfo());
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the destination.", *pMsg2->GetDestination() == *testMachine);
   const dtGame::Message *causeMsg2 = pMsg2->GetCausingMessage();
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the causing message.", 
      causeMsg2->GetMessageType() == dtGame::MessageType::REQUEST_PAUSE);
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the source machine info on the cause message.", 
      causeMsg2->GetSource() == *testMachine);

}

void MessageTests::TestTimeScaling()
{
   TestComponent* tc = new TestComponent();
   mGameManager->AddComponent(*tc);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());
   
   double oldSimTime = mGameManager->GetSimulationTime();
   mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 4.0f, mGameManager->GetSimulationClockTime());
   CPPUNIT_ASSERT_MESSAGE("The time scale should be 4.0.", osg::equivalent(4.0f, mGameManager->GetTimeScale()));
   sleep(10);
   dtCore::System::Instance()->Step();

   dtCore::RefPtr<const dtGame::Message> processTickLocalMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
   dtCore::RefPtr<const dtGame::Message> processTickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);
   
   CPPUNIT_ASSERT_MESSAGE("A tick local message should have been processed.", processTickLocalMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A tick remote message should have been processed.", processTickRemoteMsg.valid());

   const dtGame::TickMessage* tickLocal = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 
   const dtGame::TickMessage* tickRemote = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 

   CPPUNIT_ASSERT_MESSAGE("The time scale should be 4.0.", osg::equivalent(4.0f, tickLocal->GetSimTimeScale()));
   CPPUNIT_ASSERT_MESSAGE("The time scale should be 4.0.", osg::equivalent(4.0f, tickRemote->GetSimTimeScale()));

   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be 4 times the real time", 
                          osg::equivalent(tickLocal->GetDeltaSimTime(), 
                          tickLocal->GetDeltaRealTime() * tickLocal->GetSimTimeScale(), 1e-8f));
   CPPUNIT_ASSERT_MESSAGE("Simulation time should have changed by the delta sim time amount", 
                          osg::equivalent(oldSimTime + tickLocal->GetDeltaSimTime(), tickLocal->GetSimulationTime(), 1e-8));
   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be 4 times the real time", 
                          osg::equivalent(tickRemote->GetDeltaSimTime(), tickRemote->GetDeltaRealTime() * tickRemote->GetSimTimeScale(), 1e-8f));
   CPPUNIT_ASSERT_MESSAGE("Simulation time should have changed by the delta sim time amount", 
                          osg::equivalent(oldSimTime + tickRemote->GetDeltaSimTime(), tickRemote->GetSimulationTime(), 1e-8));
   
   tc->reset();

   mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 1.9f, mGameManager->GetSimulationClockTime());
   sleep(10);
   dtCore::System::Instance()->Step();
   
   processTickLocalMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
   processTickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);
   
   CPPUNIT_ASSERT_MESSAGE("A tick local message should have been processed.", processTickLocalMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A tick remote message should have been processed.", processTickRemoteMsg.valid());

   tickLocal = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 
   tickRemote = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 

   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be half the real time", osg::equivalent(tickLocal->GetDeltaSimTime(), tickLocal->GetDeltaRealTime() * 1.9f, 1e-8f));
   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be half the real time", osg::equivalent(tickRemote->GetDeltaSimTime(), tickRemote->GetDeltaRealTime() * 1.9f, 1e-8f));

   tc->reset();
}

void MessageTests::TestTimeChange()
{
   TestComponent* tc = new TestComponent();
   mGameManager->AddComponent(*tc);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());
   
   const double newTime = 222233.03;
   dtCore::Timer_t oldSimClock = mGameManager->GetSimulationClockTime();
   
   mGameManager->ChangeTimeSettings(newTime, 0.5f, oldSimClock);
   sleep(10);
   dtCore::System::Instance()->Step();

   dtCore::RefPtr<const dtGame::Message> processTickLocalMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
   dtCore::RefPtr<const dtGame::Message> processTickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);

   dtCore::RefPtr<const dtGame::Message> processTimeChangeMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_TIME_CHANGED);
   
   CPPUNIT_ASSERT_MESSAGE("A tick local message should have been processed.", processTickLocalMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A tick remote message should have been processed.", processTickRemoteMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A time changed message should have been processed.", processTimeChangeMsg.valid());

   const dtGame::TickMessage* tickLocal = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 
   const dtGame::TickMessage* tickRemote = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get()); 
   const dtGame::TimeChangeMessage* timeChange = static_cast<const dtGame::TimeChangeMessage*>(processTimeChangeMsg.get()); 

   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be half the real time", osg::equivalent(tickLocal->GetDeltaSimTime(), tickLocal->GetDeltaRealTime() * 0.5f, 1e-8f));
   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be half the real time", osg::equivalent(tickRemote->GetDeltaSimTime(), tickLocal->GetDeltaRealTime() * 0.5f, 1e-8f));

   CPPUNIT_ASSERT_MESSAGE("Simulation time should be the set time plus the delta sim time", osg::equivalent(tickRemote->GetDeltaSimTime() + newTime,  tickLocal->GetSimulationTime(), 1e-8));


   CPPUNIT_ASSERT_MESSAGE("The time change message should have the proper sim time.", osg::equivalent(newTime,  timeChange->GetSimulationTime(), 1e-8));
   CPPUNIT_ASSERT_MESSAGE("The time change message should have the proper sim clock time.", oldSimClock == timeChange->GetSimulationClockTime());
   CPPUNIT_ASSERT_MESSAGE("The time change message should have the proper time scale.", osg::equivalent(0.5f,  timeChange->GetTimeScale(), 1e-2f));
   
   //std::cout << mGameManager->GetSimulationTime() << std::endl;
   //std::cout << tickLocal->GetDeltaSimTime() << std::endl;
   
   CPPUNIT_ASSERT_MESSAGE("The GM should have the proper sim time.", osg::equivalent(newTime + (double)tickLocal->GetDeltaSimTime(),  mGameManager->GetSimulationTime()  , 1e-8));
   CPPUNIT_ASSERT_MESSAGE("The GM should have the proper sim clock time.", oldSimClock + (dtCore::Timer_t)(1000000.0 * (double)tickLocal->GetDeltaSimTime()) == mGameManager->GetSimulationClockTime());
   CPPUNIT_ASSERT_MESSAGE("The GM should have the proper time scale.", osg::equivalent(0.5f,  mGameManager->GetTimeScale(), 1e-2f));
   tc->reset();

}

void MessageTests::TestChangeMap()
{
   dtDAL::Project& project = dtDAL::Project::GetInstance();
   std::string mapName = "Many Game Actors";
   dtDAL::Map* map = &project.CreateMap(mapName, "mga");
   
   createActors(*map);
   unsigned numActors = map->GetAllProxies().size();
   
   project.SaveMap(*map);
   project.CloseMap(*map);
   
   TestComponent& tc = *new TestComponent();
   mGameManager->AddComponent(tc);
   mGameManager->ChangeMap(mapName, false);
   
   std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > toFill;
   mGameManager->GetAllActors(toFill);
   
   CPPUNIT_ASSERT_MESSAGE("The number of actors in the GM should match the map.", numActors == toFill.size());
      
   sleep(10);
   dtCore::System::Instance()->Step();
   
   CPPUNIT_ASSERT_MESSAGE("The number of actors in the GM should match the map.", numActors == toFill.size());

   dtCore::RefPtr<const dtGame::Message> processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOADED);
   CPPUNIT_ASSERT_MESSAGE("A map loaded message should have been processed.", processMapLoadedMsg.valid());
   const dtGame::MapLoadedMessage* mapLoadedMsg = static_cast<const dtGame::MapLoadedMessage*>(processMapLoadedMsg.get());
   CPPUNIT_ASSERT_MESSAGE("The Map name in the message should be " + mapName,  mapLoadedMsg->GetLoadedMapName() == mapName); 
   
   for (unsigned i = 0; i < toFill.size(); ++i)
   {
      if (toFill[i]->IsGameActorProxy())
      {
         dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(toFill[i].get());
         CPPUNIT_ASSERT(gap != NULL); 
         CPPUNIT_ASSERT_MESSAGE("The game actor proxy should be assigned to the game actor.", &gap->GetGameActor().GetGameActorProxy() == gap.get());
         std::vector<const dtGame::Invokable*> invokables;
         gap->GetInvokables(invokables);
         CPPUNIT_ASSERT_MESSAGE("There should be invokables on the game actor proxies if BuildInvokables was called.", invokables.size() > 0);
      }
   }   
   
}

void MessageTests::TestChangeMapErrorConditions()
{
   try
   {
      dtDAL::Project::GetInstance().SetContext("");
      mGameManager->ChangeMap("A Testy Map");
      CPPUNIT_FAIL("The game manager should not have been able to change to any map without a context.");
   }
   catch(const dtUtil::Exception &e) 
   {
      // correct
   }

   try
   {
      dtDAL::Project::GetInstance().SetContext("../tests/project/WorkingProject");
      mGameManager->ChangeMap("");
      CPPUNIT_FAIL("The game manager should not have been able to change to an empty string");
   }
   catch(const dtUtil::Exception &e) 
   {
      // correct
   }

   try
   {
      dtDAL::Project::GetInstance().SetContext("../tests/project/WorkingProject");
      mGameManager->ChangeMap("This map does not exist");
      CPPUNIT_FAIL("The game manager should not have been able to set the map");
   }
   catch(const dtUtil::Exception &e) 
   {
      // correct
   }

   try
   {
      dtDAL::Project::GetInstance().SetContext("../tests/project/WorkingProject");
      mGameManager->ChangeMap("../examples/testMap/testMap");
   }
   catch(const dtUtil::Exception &e) 
   {
      // correct 
   }

}


void MessageTests::TestDefaultMessageProcessorWithPauseResumeCommands()
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor);
   
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());

   dtCore::RefPtr<dtGame::Message> pauseCommand = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::COMMAND_PAUSE);
   mGameManager->ProcessMessage(*pauseCommand);
   sleep(10);
   dtCore::System::Instance()->Step();
   
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should now be paused.", mGameManager->IsPaused());
   
   dtCore::RefPtr<dtGame::Message> resumeCommand = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::COMMAND_RESUME);
   mGameManager->ProcessMessage(*resumeCommand);
   sleep(10);
   dtCore::System::Instance()->Step();
   
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should now be resumed.", !mGameManager->IsPaused());
   
}

void MessageTests::TestDefaultMessageProcessorWithRemoteActorUpdates()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(true);
}

void MessageTests::TestDefaultMessageProcessorWithLocalActorUpdates()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false);
}

void MessageTests::TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(bool remote)
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor);
   
   dtCore::RefPtr<dtDAL::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
   
   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtDAL::ActorProxy> ap = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(ap->IsGameActorProxy());
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   CPPUNIT_ASSERT(gap != NULL);
  
   try 
   {
      mGameManager->AddActor(*gap, remote, false);
   } 
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL("Actor should be added with no problems"); 
   }

   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorUpdateMsg = 
      static_cast<dtGame::ActorUpdateMessage*>(mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED).get());
   
   CPPUNIT_ASSERT_MESSAGE("Has Fired should be false.", gap->GetProperty("Has Fired")->GetStringValue() == "false");
   CPPUNIT_ASSERT_MESSAGE("Local Tick Count should be 0.", gap->GetProperty("Local Tick Count")->GetStringValue() == "0");
   CPPUNIT_ASSERT_MESSAGE("Remote Tick Count should be 0.", gap->GetProperty("Remote Tick Count")->GetStringValue() == "0");

   gap->PopulateActorUpdate(*actorUpdateMsg);

   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Name") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Name") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Category") != NULL);

   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Name")->ToString() == gap->GetName());
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Name")->ToString() == gap->GetActorType().GetName());
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Category")->ToString() == gap->GetActorType().GetCategory());

   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("Has Fired") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("Local Tick Count") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("Remote Tick Count") != NULL);
   
   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("Has Fired")->ToString() == "false");
   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("Local Tick Count")->ToString() == "0");
   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("Remote Tick Count")->ToString() == "0");

   actorUpdateMsg->GetUpdateParameter("Has Fired")->FromString("true");
   actorUpdateMsg->GetUpdateParameter("Local Tick Count")->FromString("96");
   actorUpdateMsg->GetUpdateParameter("Remote Tick Count")->FromString("107");

   mGameManager->ProcessMessage(*actorUpdateMsg);
   sleep(10);
   dtCore::System::Instance()->Step();
   if (remote)
   {
      CPPUNIT_ASSERT_MESSAGE("Has Fired should be changed to true.", gap->GetProperty("Has Fired")->GetStringValue() == "true");
      CPPUNIT_ASSERT_MESSAGE("Local Tick Count should be changed to 96.", gap->GetProperty("Local Tick Count")->GetStringValue() == "96");
      CPPUNIT_ASSERT_MESSAGE("Remote Tick Count should be changed to 107.", gap->GetProperty("Remote Tick Count")->GetStringValue() == "107");
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("Has Fired should still be false.", gap->GetProperty("Has Fired")->GetStringValue() == "false");
      CPPUNIT_ASSERT_MESSAGE("Local Tick Count should still be 0.", gap->GetProperty("Local Tick Count")->GetStringValue() == "0");
      CPPUNIT_ASSERT_MESSAGE("Remote Tick Count should still be 0.", gap->GetProperty("Remote Tick Count")->GetStringValue() == "0");
   }
   
}

void MessageTests::TestDefaultMessageProcessorWithRemoteActorDeletes()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorDeletes(true);
}

void MessageTests::TestDefaultMessageProcessorWithLocalActorDeletes()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorDeletes(false);
}

void MessageTests::TestDefaultMessageProcessorWithLocalOrRemoteActorDeletes(bool remote)
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor);
   
   dtCore::RefPtr<dtDAL::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
   
   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtDAL::ActorProxy> ap = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(ap->IsGameActorProxy());
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   CPPUNIT_ASSERT(gap != NULL);
  
   try 
   {
      mGameManager->AddActor(*gap, remote, false);
   } 
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL("Actor should be added with no problems"); 
   }

   dtCore::RefPtr<dtGame::Message> actorDeleteMsg = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED);

   actorDeleteMsg->SetAboutActorId(gap->GetId());

   mGameManager->ProcessMessage(*actorDeleteMsg);
   sleep(10);
   dtCore::System::Instance()->Step();

   if (remote)
   {
      CPPUNIT_ASSERT_MESSAGE("The actor should have been deleted.", mGameManager->FindGameActorById(gap->GetId()) == NULL);
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("The actor should not have been deleted.", mGameManager->FindGameActorById(gap->GetId()) != NULL);
   }
   
}

void MessageTests::TestDefaultMessageProcessorWithRemoteActorCreates()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(true);
}

void MessageTests::TestDefaultMessageProcessorWithLocalActorCreates()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(false);
}

void MessageTests::TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(bool remote)
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor);
   
   dtCore::RefPtr<dtDAL::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
   
   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtDAL::ActorProxy> ap = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(ap->IsGameActorProxy());
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   CPPUNIT_ASSERT(gap != NULL);
  
   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorCreateMsg = 
      static_cast<dtGame::ActorUpdateMessage*>(mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED).get());
   
   CPPUNIT_ASSERT_MESSAGE("Has Fired should be false.", gap->GetProperty("Has Fired")->GetStringValue() == "false");
   CPPUNIT_ASSERT_MESSAGE("Local Tick Count should be 0.", gap->GetProperty("Local Tick Count")->GetStringValue() == "0");
   CPPUNIT_ASSERT_MESSAGE("Remote Tick Count should be 0.", gap->GetProperty("Remote Tick Count")->GetStringValue() == "0");

   gap->PopulateActorUpdate(*actorCreateMsg);
   if (remote)
   {
      //make it remote
      actorCreateMsg->SetSource(*new dtGame::MachineInfo());
   }

   CPPUNIT_ASSERT(actorCreateMsg->GetParameter("Name") != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter("Actor Type Name") != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter("Actor Type Category") != NULL);

   CPPUNIT_ASSERT(actorCreateMsg->GetParameter("Name")->ToString() == gap->GetName());
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter("Actor Type Name")->ToString() == gap->GetActorType().GetName());
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter("Actor Type Category")->ToString() == gap->GetActorType().GetCategory());

   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("Has Fired") != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("Local Tick Count") != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("Remote Tick Count") != NULL);
   
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("Has Fired")->ToString() == "false");
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("Local Tick Count")->ToString() == "0");
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("Remote Tick Count")->ToString() == "0");

   actorCreateMsg->GetUpdateParameter("Has Fired")->FromString("true");
   actorCreateMsg->GetUpdateParameter("Local Tick Count")->FromString("96");
   actorCreateMsg->GetUpdateParameter("Remote Tick Count")->FromString("107");

   mGameManager->ProcessMessage(*actorCreateMsg);
   
   CPPUNIT_ASSERT(mGameManager->FindGameActorById(gap->GetId()) == NULL);
   
   sleep(10);
   dtCore::System::Instance()->Step();
   
   dtCore::RefPtr<dtGame::GameActorProxy> gapRemote = mGameManager->FindGameActorById(gap->GetId());
   
   if (remote)
   {
      CPPUNIT_ASSERT_MESSAGE("The remote actor should have been created.", gapRemote != NULL);

      CPPUNIT_ASSERT_MESSAGE("The remote actor should have the same actor type as the real actor.", gapRemote->GetActorType() == gap->GetActorType());
      CPPUNIT_ASSERT_MESSAGE("The remote actor should have the same name as the real actor.", gapRemote->GetName() == gap->GetName());

      CPPUNIT_ASSERT_MESSAGE("Has Fired should be changed to true.", gapRemote->GetProperty("Has Fired")->GetStringValue() == "true");
      CPPUNIT_ASSERT_MESSAGE("Local Tick Count should be changed to 96.", gapRemote->GetProperty("Local Tick Count")->GetStringValue() == "96");
      CPPUNIT_ASSERT_MESSAGE("Remote Tick Count should be changed to 107.", gapRemote->GetProperty("Remote Tick Count")->GetStringValue() == "107");

      CPPUNIT_ASSERT_MESSAGE("The created actor should be remote.", gapRemote->IsRemote());
      CPPUNIT_ASSERT_MESSAGE("The created actor should not be published.", !gapRemote->IsPublished());
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("No actor should have been created.", gapRemote == NULL);
   }
   
}
