/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2009, Alion Science and Technology Corporation
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
 * @author Eddie Johnson and David Guthrie and Curtiss Murphy
 */

#include <prefix/unittestprefix.h>
#include <iostream>
#include <osg/Math>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

#include <dtActors/engineactorregistry.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/timer.h>

#include <dtCore/datatype.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/actortype.h>
#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/actorproperty.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/gameevent.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/datastream.h>

#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/defaultmessageprocessor.h>

#include <testGameActorLibrary/testgameactorlibrary.h>
#include <testGameActorLibrary/testgameactor.h>

#include <dtGame/testcomponent.h>

#include <cppunit/extensions/HelperMacros.h>

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

class MessageTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(MessageTests);

      CPPUNIT_TEST(TestOperatorEquals);
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
      CPPUNIT_TEST(TestChangeMapGameEvents);
      CPPUNIT_TEST(TestChangeMapErrorConditions);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithPauseResumeRequests);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithMapRequests);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithPauseResumeCommands);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorCreates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithLocalActorCreates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorCreatesNoParent);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorUpdates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithLocalActorUpdates);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithRemoteActorDeletes);
      CPPUNIT_TEST(TestDefaultMessageProcessorWithLocalActorDeletes);
      CPPUNIT_TEST(TestRemoteActorCreatesFromPrototype);
      CPPUNIT_TEST(TestGameEventMessage);
      CPPUNIT_TEST(TestActorEnteredWorldMessage);
      CPPUNIT_TEST(TestPartialUpdateDoesNotCreateActor);
      CPPUNIT_TEST(TestNonPartialUpdateDoesCreateActor);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestOperatorEquals();
   void TestBaseMessages();
   void TestMessageFactory();
   void TestMessageDelivery();
   void TestActorPublish();
   void TestPauseResume();
   void TestPauseResumeSystem();
   void TestRejectMessage();
   void TestTimeScaling();
   void TestTimeChange();
   void TestChangeMapGameEvents();
   void TestChangeMap();
   void TestChangeMapErrorConditions();
   void TestDefaultMessageProcessorWithPauseResumeRequests();
   void TestDefaultMessageProcessorWithMapRequests();
   void TestDefaultMessageProcessorWithPauseResumeCommands();
   void TestDefaultMessageProcessorWithRemoteActorCreates();
   void TestDefaultMessageProcessorWithRemoteActorCreatesNoParent();
   void TestDefaultMessageProcessorWithLocalActorCreates();
   void TestDefaultMessageProcessorWithRemoteActorUpdates();
   void TestDefaultMessageProcessorWithLocalActorUpdates();
   void TestDefaultMessageProcessorWithRemoteActorDeletes();
   void TestDefaultMessageProcessorWithLocalActorDeletes();
   void TestRemoteActorCreatesFromPrototype();
   void TestGameEventMessage();
   void TestActorEnteredWorldMessage();
   void TestPartialUpdateDoesNotCreateActor();
   void TestNonPartialUpdateDoesCreateActor();

private:
   static const char* mTestGameActorLibrary;
   static const char* mTestActorLibrary;

   void createActors(dtCore::Map& map);
   void RemoveOneProxy(dtCore::Map& map);

   void TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(bool remote, bool parent);
   void TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(bool remote, bool partial,
            dtGame::GameActorProxy::LocalActorUpdatePolicy& policy = dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_ALL);
   void TestDefaultMessageProcessorWithLocalOrRemoteActorDeletes(bool remote);
   void CheckMapNames(const dtGame::MapMessage& mapLoadedMsg,
      const dtGame::GameManager::NameVector& mapNames);
   void DoTestOfPartialUpdateDoesNotCreateActor(bool testWithPartial);

   dtUtil::Log* mLogger;

   dtCore::RefPtr<dtGame::GameManager> mGameManager;

};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MessageTests);

const char* MessageTests::mTestGameActorLibrary="testGameActorLibrary";
const char* MessageTests::mTestActorLibrary="testActorLibrary";

//////////////////////////////////////////////////////////////////////////
void MessageTests::setUp()
{
   try
   {
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
      mLogger = &dtUtil::Log::GetInstance("messagetests.cpp");

      mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
      mGameManager->SetApplication(GetGlobalApplication());
      mGameManager->LoadActorRegistry("dtActors");
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      mGameManager->LoadActorRegistry(mTestActorLibrary);
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

      dtCore::Project::GetInstance().CreateContext("data/TestGameProject");
      dtCore::Project::GetInstance().SetContext("data/TestGameProject");
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL((std::string("Error: ") + ex.ToString()).c_str());
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
         dtCore::System::GetInstance().SetPause(false);
         dtCore::System::GetInstance().Stop();

         dtCore::GameEventManager::GetInstance().ClearAllEvents();
         mGameManager->DeleteAllActors(true);

         if (!mGameManager->GetCurrentMap().empty())
         {
            dtCore::Project::GetInstance().CloseMap(dtCore::Project::GetInstance().GetMap(mGameManager->GetCurrentMap()), true);
            dtCore::Project::GetInstance().DeleteMap(dtCore::Project::GetInstance().GetMap(mGameManager->GetCurrentMap()));
         }

         mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
         mGameManager->UnloadActorRegistry(mTestActorLibrary);
         mGameManager->LoadActorRegistry("dtActors");
         mGameManager = NULL;
      }
      catch(const dtUtil::Exception& e)
      {
         if (!mGameManager->GetCurrentMap().empty())
         {
            dtCore::Project::GetInstance().CloseMap(dtCore::Project::GetInstance().GetMap(mGameManager->GetCurrentMap()), true);
            dtCore::Project::GetInstance().DeleteMap(dtCore::Project::GetInstance().GetMap(mGameManager->GetCurrentMap()));
         }
         CPPUNIT_FAIL((std::string("Error: ") + e.ToString()).c_str());
      }
   }

   try
   {
      if (dtUtil::FileUtils::GetInstance().DirExists("data/TestGameProject"))
      {
         dtUtil::FileUtils::GetInstance().DirDelete("data/TestGameProject", true);
      }
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.ToString()).c_str());
   }
}

void MessageTests::createActors(dtCore::Map& map)
{
   map.ClearProxies();

   dtCore::ActorTypeVec actorTypes;

   actorTypes.push_back(dtActors::EngineActorRegistry::TASK_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::ROLL_UP_TASK_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::ORDERED_TASK_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::INFINITE_LIGHT_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::POSITIONAL_LIGHT_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::SPOT_LIGHT_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::PARTICLE_SYSTEM_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::MESH_TERRAIN_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::BEZIER_NODE_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::BEZIER_CONTROL_POINT_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::BEZIER_CONTROLLER_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::TRIGGER_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::TRIPOD_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::COORDINATE_CONFIG_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::PLAYER_START_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::WAYPOINT_VOLUME_ACTOR_TYPE.get());
   actorTypes.push_back(dtActors::EngineActorRegistry::DISTANCE_SENSOR_ACTOR_TYPE.get());

   actorTypes.push_back(TestGameActorLibrary::TEST_TANK_GAME_ACTOR_TYPE.get());
   actorTypes.push_back(TestGameActorLibrary::TEST_GAME_ACTOR_CRASH_TYPE.get());

   for (unsigned int i = 0; i < actorTypes.size(); ++i)
   {
      dtCore::RefPtr<dtCore::BaseActorObject> proxy;

      proxy = mGameManager->CreateActor(*actorTypes[i]);
      proxy->SetName(dtUtil::ToString(i));

      map.AddProxy(*proxy);
   }

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Proxy list has the wrong size.",
                                actorTypes.size(), map.GetAllProxies().size());
}

void MessageTests::TestOperatorEquals()
{
   dtGame::MessageFactory& factory = mGameManager->GetMessageFactory();

   dtCore::RefPtr<dtGame::MachineInfo> machineInfo1 = new dtGame::MachineInfo;
   dtCore::RefPtr<dtGame::MachineInfo> machineInfo2 = new dtGame::MachineInfo;

   dtCore::RefPtr<dtGame::TickMessage> msg1;
   dtCore::RefPtr<dtGame::TickMessage> msg2;
   dtCore::RefPtr<dtGame::TickMessage> msg3;
   dtCore::RefPtr<dtGame::TickMessage> msg4;
   factory.CreateMessage(dtGame::MessageType::TICK_LOCAL, msg1);
   factory.CreateMessage(dtGame::MessageType::TICK_LOCAL, msg2);
   factory.CreateMessage(dtGame::MessageType::TICK_REMOTE, msg3);
   factory.CreateMessage(dtGame::MessageType::TICK_REMOTE, msg4);

   CPPUNIT_ASSERT(*msg1 == *msg1);
   CPPUNIT_ASSERT(*msg1 == *msg2);
   CPPUNIT_ASSERT(*msg1 != *msg3);

   msg1->SetSource(*machineInfo1);
   msg1->SetDestination(machineInfo2.get());
   CPPUNIT_ASSERT(*msg1 == *msg1);
   CPPUNIT_ASSERT(*msg1 != *msg2);
   msg2->SetSource(*machineInfo1);
   msg2->SetDestination(machineInfo2.get());
   CPPUNIT_ASSERT(*msg1 == *msg2);


   msg1->SetSendingActorId(dtCore::UniqueId());
   CPPUNIT_ASSERT(*msg1 == *msg1);
   CPPUNIT_ASSERT(*msg1 != *msg2);
   msg2->SetSendingActorId(msg1->GetSendingActorId());
   CPPUNIT_ASSERT(*msg1 == *msg2);


   msg1->SetAboutActorId(dtCore::UniqueId());
   CPPUNIT_ASSERT(*msg1 == *msg1);
   CPPUNIT_ASSERT(*msg1 != *msg2);
   msg2->SetAboutActorId(msg1->GetAboutActorId());
   CPPUNIT_ASSERT(*msg1 == *msg2);


   msg1->SetDeltaRealTime(22.343);
   CPPUNIT_ASSERT(*msg1 == *msg1);
   CPPUNIT_ASSERT(*msg1 != *msg2);
   msg2->SetDeltaRealTime(msg1->GetDeltaRealTime());
   CPPUNIT_ASSERT(*msg1 == *msg2);

   msg1->SetCausingMessage(msg3.get());
   CPPUNIT_ASSERT(*msg1 == *msg1);
   CPPUNIT_ASSERT(*msg1 != *msg2);
   msg2->SetCausingMessage(msg1->GetCausingMessage());
   CPPUNIT_ASSERT(*msg1 == *msg2);
   msg2->SetCausingMessage(msg4.get());
   CPPUNIT_ASSERT(*msg1 == *msg2);

}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestBaseMessages()
{
   try
   {
      dtCore::RefPtr<dtGame::TickMessage> tickMsg = new dtGame::TickMessage();
      dtCore::RefPtr<dtGame::TimeChangeMessage> timeChangeMessage = new dtGame::TimeChangeMessage();
      dtCore::RefPtr<dtGame::TimerElapsedMessage> timerMsg = new dtGame::TimerElapsedMessage();
      dtCore::RefPtr<dtGame::MapMessage> mapMsg = new dtGame::MapMessage();
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

      dtGame::GameManager::NameVector mapNames;
      std::ostringstream ss;
      for (unsigned i = 0; i < 20; ++i)
      {
         ss.str("");
         ss << "Some whack map " << i;
         mapNames.push_back(ss.str());
      }

      mapMsg->SetMapNames(mapNames);
      dtGame::GameManager::NameVector mapNamesGet;
      mapMsg->GetMapNames(mapNamesGet);
      ss.str("Map message should be able to set/get the map names");
      for (unsigned i = 0; i < mapNames.size(); ++i)
      {
         ss << mapNames[i] << "\n";
      }
      ss << "\n\n";
      for (unsigned i = 0; i < mapNamesGet.size(); ++i)
      {
         ss << mapNamesGet[i] << "\n";
      }
      CPPUNIT_ASSERT_MESSAGE(ss.str(), mapNames == mapNamesGet);

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
      dtUtil::DataStream stream;

      dtGame::MessageParameter* o = aum1->AddUpdateParameter("param1", dtCore::DataType::FLOAT);
      dtGame::MessageParameter* t = aum1->AddUpdateParameter("param2", dtCore::DataType::INT);
      dtGame::MessageParameter* h = aum1->AddUpdateParameter("param3", dtCore::DataType::DOUBLE);
      dtGame::MessageParameter* f = aum1->AddUpdateParameter("param4", dtCore::DataType::BOOLEAN);

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

      dtGame::MessageParameter* one   = aum2->GetUpdateParameter("param1");
      dtGame::MessageParameter* two   = aum2->GetUpdateParameter("param2");
      dtGame::MessageParameter* three = aum2->GetUpdateParameter("param3");
      dtGame::MessageParameter* four  = aum2->GetUpdateParameter("param4");

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

      dtCore::RefPtr<dtGame::ActorUpdateMessage> actorMsg = new dtGame::ActorUpdateMessage;
      actorMsg->SetActorTypeName("Tripod");
      actorMsg->SetActorTypeCategory("dtcore");
      dtCore::RefPtr<const dtCore::ActorType> type = actorMsg->GetActorType();
      CPPUNIT_ASSERT(type.valid());
      CPPUNIT_ASSERT_MESSAGE("The message type should be correct", type.get() == dtActors::EngineActorRegistry::TRIPOD_ACTOR_TYPE);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
   //catch (const std::exception& e)
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
      factory.GetMessageTypeById(dtGame::MessageType::INFO_PLAYER_ENTERED_WORLD.GetId());

      dtCore::RefPtr<dtGame::TickMessage> tickMsg;
      factory.CreateMessage(dtGame::MessageType::TICK_LOCAL, tickMsg);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a tick local message", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("The Message should have the type TICK_LOCAL",tickMsg->GetMessageType() == dtGame::MessageType::TICK_LOCAL);
      factory.CreateMessage(dtGame::MessageType::TICK_REMOTE, tickMsg);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a tick remote message", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("The Message should have the type TICK_REMOTE",tickMsg->GetMessageType() == dtGame::MessageType::TICK_REMOTE);
      dtCore::RefPtr<dtGame::TimerElapsedMessage> timerMsg;
      factory.CreateMessage(dtGame::MessageType::INFO_TIMER_ELAPSED, timerMsg);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a timer elapsed message", timerMsg != NULL);
      dtCore::RefPtr<dtGame::Message> mapMsg = factory.CreateMessage(dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a map loaded message", mapMsg != NULL);
      dtCore::RefPtr<dtGame::Message> netMsg = factory.CreateMessage(dtGame::MessageType::NETSERVER_REJECT_CONNECTION);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a net server reject connection message", netMsg != NULL);
      dtCore::RefPtr<dtGame::Message> restartMsg = factory.CreateMessage(dtGame::MessageType::COMMAND_RESTART);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a restart message", restartMsg != NULL);
      dtCore::RefPtr<dtGame::Message> serverMsg = factory.CreateMessage(dtGame::MessageType::SERVER_REQUEST_REJECTED);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a server message rejected message", serverMsg != NULL);
      dtCore::RefPtr<dtGame::Message> playerMsg = factory.CreateMessage(dtGame::MessageType::INFO_PLAYER_ENTERED_WORLD);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to create a player entered world message", playerMsg.valid());

      dtCore::RefPtr<dtGame::MapMessage> commandChangeMsg;
      factory.CreateMessage(dtGame::MessageType::COMMAND_LOAD_MAP, commandChangeMsg);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to a COMMAND_LOAD_MAP message", commandChangeMsg.valid());

      dtCore::RefPtr<dtGame::MapMessage> requestChangeMsg;
      factory.CreateMessage(dtGame::MessageType::REQUEST_LOAD_MAP, requestChangeMsg);
      CPPUNIT_ASSERT_MESSAGE("The message factory should be able to a REQUEST_LOAD_MAP message", requestChangeMsg.valid());

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
      for (unsigned int i = 0; i < v.size(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("The vector of supported message types should register as supported in the factory", factory.IsMessageTypeSupported(*v[i]));
      }

      tickMsg->SetDeltaRealTime(1.34f);
      tickMsg->SetDeltaSimTime(2.56f);
      tickMsg->SetSimulationTime(103330.000);
      tickMsg->SetSimTimeScale(6.32f);

      timerMsg->SetLateTime(1.0f);
      timerMsg->SetTimerName("Bob");

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
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestMessageDelivery()
{
   try
   {
      dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent("name");

      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);

      dtCore::RefPtr<dtGame::Message> msg = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED);

      CPPUNIT_ASSERT_MESSAGE("The message created should not be NULL", msg != NULL);

      dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");

      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mGameManager->CreateActor(*type, gap);

      CPPUNIT_ASSERT(gap->IsGameActor());
      mGameManager->AddActor(*gap, false, false);
      std::set<const dtCore::ActorType*> typeSet;
      mGameManager->GetUsedActorTypes(typeSet);
      CPPUNIT_ASSERT_MESSAGE("The amount of actor types supported should not be 0", typeSet.size() != 0);

      mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 1.5, mGameManager->GetSimulationClockTime());

      tc->reset();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();
      for (unsigned i = 0; i < tc->GetReceivedProcessMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedProcessMessages()[i].valid());
         //std::cout << tc->GetReceivedProcessMessages()[i]->GetMessageType()->GetName();
      }

      dtCore::RefPtr<const dtGame::Message> tickMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
      dtCore::RefPtr<const dtGame::Message> tickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);
      dtCore::RefPtr<const dtGame::Message> actorCreatedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      dtCore::RefPtr<const dtGame::Message> publishedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);
      dtCore::RefPtr<const dtGame::Message> tickEndOfFrame = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_END_OF_FRAME);

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have been processed.", tickMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have been processed.", tickRemoteMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have been processed.", actorCreatedMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("No Actor Published message should have been processed.", publishedMsg == NULL);
      CPPUNIT_ASSERT_MESSAGE("A Tick End of Frame message should have been processed.", tickEndOfFrame != NULL);

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have GetSource matching the GameManager.", tickMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have GetSource matching the GameManager.", tickRemoteMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick End Of Frame message should have GetSource matching the GameManager.", tickEndOfFrame->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have GetSource matching the GameManager.", actorCreatedMsg->GetSource() == mGameManager->GetMachineInfo());

      CPPUNIT_ASSERT_MESSAGE("A Tick Local message should have GetDestination matching the GameManager.", tickMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick Remote message should have GetDestination matching the GameManager.", tickRemoteMsg->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("A Tick End Of Frame message should have GetDestination matching the GameManager.", tickEndOfFrame->GetDestination() == &mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("An Actor Created message should have GetDestination matching the GameManager.", actorCreatedMsg->GetDestination() == NULL);

      dtCore::RefPtr<const dtGame::TickMessage> localTickMessage = dynamic_cast<const dtGame::TickMessage*>(tickMsg.get());
      dtCore::RefPtr<const dtGame::TickMessage> remoteTickMessage = dynamic_cast<const dtGame::TickMessage*>(tickRemoteMsg.get());;
      dtCore::RefPtr<const dtGame::TickMessage> endTickMessage = dynamic_cast<const dtGame::TickMessage*>(tickEndOfFrame.get());

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All the tick messages should have the same data.",
               localTickMessage->GetDeltaSimTime(), remoteTickMessage->GetDeltaSimTime(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All the tick messages should have the same data.",
               localTickMessage->GetDeltaSimTime(), endTickMessage->GetDeltaSimTime(), 0.001f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All the tick messages should have the same data.",
               localTickMessage->GetDeltaRealTime(), remoteTickMessage->GetDeltaRealTime(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All the tick messages should have the same data.",
               localTickMessage->GetDeltaRealTime(), endTickMessage->GetDeltaRealTime(), 0.001f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All the tick messages should have the same data.",
               localTickMessage->GetSimTimeScale(), remoteTickMessage->GetSimTimeScale(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("All the tick messages should have the same data.",
               localTickMessage->GetSimTimeScale(), endTickMessage->GetSimTimeScale(), 0.001f);

      mGameManager->RemoveComponent(*tc);
      CPPUNIT_ASSERT(tc->GetGameManager() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestActorPublish()
{
   try
   {
      dtGame::TestComponent* tc = new dtGame::TestComponent("name");
      dtGame::DefaultNetworkPublishingComponent* rc = new dtGame::DefaultNetworkPublishingComponent();

      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*rc, dtGame::GameManager::ComponentPriority::NORMAL);

      CPPUNIT_ASSERT(tc->GetGameManager() != NULL);
      CPPUNIT_ASSERT(rc->GetGameManager() != NULL);

      dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");

      CPPUNIT_ASSERT(type != NULL);
      dtCore::RefPtr<dtCore::BaseActorObject> ap = mGameManager->CreateActor(*type);

      CPPUNIT_ASSERT(ap->IsGameActor());
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
      CPPUNIT_ASSERT(gap != NULL);


      CPPUNIT_ASSERT_THROW_MESSAGE("Adding an actor with both remote and true should fail.",
                                   mGameManager->AddActor(*gap, true, true), dtGame::ActorIsRemoteException);


      //now we really add the actor as published.
      mGameManager->AddActor(*gap, false, true);

      CPPUNIT_ASSERT(!gap->IsRemote());
      CPPUNIT_ASSERT(gap->IsPublished());

      mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 1.5, mGameManager->GetSimulationClockTime());
      tc->reset();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();
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

      dtCore::RefPtr<const dtGame::Message> sendActorCreateMsg = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      dtCore::RefPtr<const dtGame::Message> sendPublishedMsg = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);

      CPPUNIT_ASSERT(sendActorCreateMsg == NULL);
      CPPUNIT_ASSERT(sendPublishedMsg == NULL);

      //Another Frame...
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(tc->GetReceivedDispatchNetworkMessages().size() > 0);
      for (unsigned i = 0; i < tc->GetReceivedDispatchNetworkMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedDispatchNetworkMessages()[i].valid());
         //std::cout << tc->GetReceivedDispatchNetworkMessages()[i]->GetMessageType().GetName();
      }

      sendActorCreateMsg = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::INFO_ACTOR_CREATED);
      sendPublishedMsg = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::INFO_ACTOR_PUBLISHED);

      CPPUNIT_ASSERT_MESSAGE("An actor create message should have been sent.", sendActorCreateMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("No publish message should be been sent.", sendPublishedMsg == NULL);

      CPPUNIT_ASSERT_MESSAGE("The Actor Create message should have GetSource matching the GameManager.", sendActorCreateMsg->GetSource() == mGameManager->GetMachineInfo());
      CPPUNIT_ASSERT_MESSAGE("The Actor Create message should have GetDestination of NULL.", sendActorCreateMsg->GetDestination() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestPauseResume()
{
   dtGame::TestComponent* tc = new dtGame::TestComponent("name");
   mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());

   mGameManager->SetPaused(true);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", mGameManager->IsPaused());
   CPPUNIT_ASSERT_MESSAGE("System should be paused.", dtCore::System::GetInstance().GetPause());

   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

   dtCore::RefPtr<const dtGame::Message> processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
   dtCore::RefPtr<const dtGame::Message> processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);

   CPPUNIT_ASSERT_MESSAGE("A paused message should have been processed.", processPausedMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A resumed message should not have been processed.", !processResumedMsg.valid());

   dtCore::Timer_t oldSimClockTime = mGameManager->GetSimulationClockTime();
   double oldSimTime = mGameManager->GetSimulationTime();

   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

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
   CPPUNIT_ASSERT_MESSAGE("System should be paused.", !dtCore::System::GetInstance().GetPause());

   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

   processPausedMsg  = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
   processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);

   CPPUNIT_ASSERT_MESSAGE("A paused message should not have been processed.", !processPausedMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A resumed message should have been processed.", processResumedMsg.valid());
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestPauseResumeSystem()
{
   try
   {
      dtGame::TestComponent* tc = new dtGame::TestComponent("name");
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());

      dtCore::System::GetInstance().SetPause(true);
      CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", mGameManager->IsPaused());
      CPPUNIT_ASSERT_MESSAGE("System should be paused.", dtCore::System::GetInstance().GetPause());

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      dtCore::RefPtr<const dtGame::Message> processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
      dtCore::RefPtr<const dtGame::Message> processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);

      CPPUNIT_ASSERT_MESSAGE("A paused message should have been processed.", processPausedMsg.valid());
      CPPUNIT_ASSERT_MESSAGE("A resumed message should not have been processed.", !processResumedMsg.valid());

      tc->reset();

      dtCore::System::GetInstance().SetPause(false);
      CPPUNIT_ASSERT_MESSAGE("The Game Manager should be paused.", !mGameManager->IsPaused());
      CPPUNIT_ASSERT_MESSAGE("System should be paused.", !dtCore::System::GetInstance().GetPause());

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      processPausedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_PAUSED);
      processResumedMsg = tc->FindProcessMessageOfType(dtGame::MessageType::INFO_RESUMED);

      CPPUNIT_ASSERT_MESSAGE("A paused message should not have been processed.", !processPausedMsg.valid());
      CPPUNIT_ASSERT_MESSAGE("A resumed message should have been processed.", processResumedMsg.valid());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void MessageTests::TestRejectMessage()
{
   dtGame::TestComponent* tc = new dtGame::TestComponent("name");
   mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
   tc->reset();

   // reject some message - any ole message will do.

   dtCore::RefPtr<dtGame::Message> msg = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_PAUSE);
   mGameManager->RejectMessage(*msg, "test reason");
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

   // message should have been processed (not sent)

   dtCore::RefPtr<const dtGame::Message> processedRejection1 = tc->FindProcessMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED);
   dtCore::RefPtr<const dtGame::Message> sentRejection1 = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED);
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have been processed.", processedRejection1.valid());
   CPPUNIT_ASSERT_MESSAGE("Local reject message should NOT have been sent.", !sentRejection1.valid());

   // processed message should have the right data

   const dtGame::ServerMessageRejected* pMsg1 = static_cast<const dtGame::ServerMessageRejected*> (processedRejection1.get());
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have the reason.", pMsg1->GetCause() == "test reason");
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have our machine.", pMsg1->GetSource() == mGameManager->GetMachineInfo());
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have the destination.", *pMsg1->GetDestination() == mGameManager->GetMachineInfo());
   const dtGame::Message* causeMsg1 = pMsg1->GetCausingMessage();
   CPPUNIT_ASSERT_MESSAGE("Local reject message should have the causing message.",
      causeMsg1->GetMessageType() == dtGame::MessageType::REQUEST_PAUSE);

   tc->reset();

   // Now, reject a message that goes to a different machine info

   dtCore::RefPtr<dtGame::Message> msg2 = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_PAUSE);
   dtCore::RefPtr<dtGame::MachineInfo> testMachine = new dtGame::MachineInfo(); // has a unique id
   //msg2->SetDestination(&(*testMachine));
   msg2->SetSource(*testMachine);
   mGameManager->RejectMessage(*msg2, "test reason2");
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

   // message should have been sent (not processed)

   dtCore::RefPtr<const dtGame::Message> processedRejection2 = tc->FindProcessMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED);
   dtCore::RefPtr<const dtGame::Message> sentRejection2 = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::SERVER_REQUEST_REJECTED);
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should NOT have been processed.", !processedRejection2.valid());
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have been sent.", sentRejection2.valid());

   // sent message should have the right data

   const dtGame::ServerMessageRejected* pMsg2 = static_cast<const dtGame::ServerMessageRejected*> (sentRejection2.get());
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the reason.", pMsg2->GetCause() == "test reason2");
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have our machine.", pMsg2->GetSource() == mGameManager->GetMachineInfo());
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the destination.", *pMsg2->GetDestination() == *testMachine);
   const dtGame::Message* causeMsg2 = pMsg2->GetCausingMessage();
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the causing message.",
      causeMsg2->GetMessageType() == dtGame::MessageType::REQUEST_PAUSE);
   CPPUNIT_ASSERT_MESSAGE("Non-local reject message should have the source machine info on the cause message.",
      causeMsg2->GetSource() == *testMachine);

}

void MessageTests::TestTimeScaling()
{
   dtGame::TestComponent* tc = new dtGame::TestComponent("name");
   mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());

   double oldSimTime = mGameManager->GetSimulationTime();
   mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 4.0f, dtCore::System::GetInstance().GetRealClockTime());
   CPPUNIT_ASSERT_MESSAGE("The time scale should be 4.0.", osg::equivalent(4.0f, mGameManager->GetTimeScale()));
   CPPUNIT_ASSERT_MESSAGE("The simulation clock time should match the realtime clock.",
      mGameManager->GetSimulationClockTime() == dtCore::System::GetInstance().GetRealClockTime());
   dtCore::Timer_t oldSimClockTime = mGameManager->GetSimulationClockTime();
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

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
                          osg::equivalent(oldSimTime + tickLocal->GetDeltaSimTime(), tickLocal->GetSimulationTime(), 1e-6));
   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be 4 times the real time",
                          osg::equivalent(tickRemote->GetDeltaSimTime(), tickRemote->GetDeltaRealTime() * tickRemote->GetSimTimeScale(), 1e-8f));
   CPPUNIT_ASSERT_MESSAGE("Simulation time should have changed by the delta sim time amount",
                          osg::equivalent(oldSimTime + tickRemote->GetDeltaSimTime(), tickRemote->GetSimulationTime(), 1e-6));

   CPPUNIT_ASSERT_MESSAGE("Simulation clock time should have changed by the delta sim time amount",
                          osg::equivalent(double(oldSimClockTime + dtCore::Timer_t(tickLocal->GetDeltaSimTime() * 1e6)), double(mGameManager->GetSimulationClockTime()), 100.0));

   tc->reset();

   mGameManager->ChangeTimeSettings(mGameManager->GetSimulationTime(), 0.5f, mGameManager->GetSimulationClockTime());
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

   processTickLocalMsg  = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);
   processTickRemoteMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_REMOTE);

   CPPUNIT_ASSERT_MESSAGE("A tick local message should have been processed.", processTickLocalMsg.valid());
   CPPUNIT_ASSERT_MESSAGE("A tick remote message should have been processed.", processTickRemoteMsg.valid());

   tickLocal  = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get());
   tickRemote = static_cast<const dtGame::TickMessage*>(processTickLocalMsg.get());

   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be half the real time", osg::equivalent(tickLocal->GetDeltaSimTime(), tickLocal->GetDeltaRealTime() * 0.5f, 1e-8f));
   CPPUNIT_ASSERT_MESSAGE("Simulation elapsed time should be half the real time", osg::equivalent(tickRemote->GetDeltaSimTime(), tickRemote->GetDeltaRealTime() * 0.5f, 1e-8f));

   tc->reset();
}

void MessageTests::TestTimeChange()
{
   dtGame::TestComponent* tc = new dtGame::TestComponent("name");
   mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());

   const double newTime = 222233.03;
   dtCore::Timer_t oldSimClock = mGameManager->GetSimulationClockTime();

   mGameManager->ChangeTimeSettings(newTime, 0.5f, oldSimClock);
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

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
   // the divide by 256 is to allow for some rounding error.
   CPPUNIT_ASSERT_MESSAGE("The GM should have the proper sim clock time.", (oldSimClock + (dtCore::Timer_t)(1000000.0 * (double)tickLocal->GetDeltaSimTime())) / 256 == mGameManager->GetSimulationClockTime() / 256);
   CPPUNIT_ASSERT_MESSAGE("The GM should have the proper time scale.", osg::equivalent(0.5f,  mGameManager->GetTimeScale(), 1e-2f));
   tc->reset();

}

void MessageTests::TestChangeMapGameEvents()
{
   try
   {
      // NOTE - This whole test should be with a map or with the GM. It definitely doesn't belong with
      // message tests.

      dtCore::Project& project = dtCore::Project::GetInstance();
      std::string mapName = "Many Game Actors";
      dtCore::Map* map = &project.CreateMap(mapName, "mga");

      dtCore::RefPtr<dtCore::GameEvent> event;

      event = new dtCore::GameEvent("one", "");
      map->GetEventManager().AddEvent(*event);
      event = new dtCore::GameEvent("two", "");
      map->GetEventManager().AddEvent(*event);
      event = new dtCore::GameEvent("three", "");
      map->GetEventManager().AddEvent(*event);

      project.SaveMap(*map);
      project.CloseMap(*map);

      //clear the events just to make sure.
      dtCore::GameEventManager& geMan = dtCore::GameEventManager::GetInstance();
      geMan.ClearAllEvents();

      // add one event that is not in the map - it shouldn't be removed by changing maps.
      event = new dtCore::GameEvent("non-map event", "");
      geMan.AddEvent(*event);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Should be one event.", geMan.GetNumEvents(), (unsigned int) 1);

      mGameManager->ChangeMap(mapName, false);
      //two ticks to finish the change.
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Four events should be in the game event manager singleton.",
         geMan.GetNumEvents(), (unsigned int)4);

      CPPUNIT_ASSERT_MESSAGE("The first game event should be in the Game Event Manager singleton on map change.",
         geMan.FindEvent("one") != NULL);
      CPPUNIT_ASSERT_MESSAGE("The second game event should be in the Game Event Manager singleton on map change.",
         geMan.FindEvent("two") != NULL);
      CPPUNIT_ASSERT_MESSAGE("The third game event should be in the Game Event Manager singleton on map change.",
         geMan.FindEvent("three") != NULL);
      CPPUNIT_ASSERT_MESSAGE("The fourth game event should be in the Game Event Manager singleton on map change.",
         geMan.FindEvent("non-map event") != NULL);

      // test the new flag for removing game events
      mGameManager->CloseCurrentMap();
      CPPUNIT_ASSERT_MESSAGE("Should be paused now", mGameManager->IsPaused());

      //two ticks to finish the change.
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Closing the map should have removed some of the events since we didnt change the flag.",
         geMan.GetNumEvents(), (unsigned int) 1);
      CPPUNIT_ASSERT_MESSAGE("Paused should have cleared.", !mGameManager->IsPaused());

      // re-add the events and try again.
      mGameManager->ChangeMap(mapName, false);
      //two ticks to finish the change.
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Three events should be back in the game event manager singleton.",
         geMan.GetNumEvents(), (unsigned int) 4);
      mGameManager->SetRemoveGameEventsOnMapChange(false);
      mGameManager->CloseCurrentMap();
      //two ticks to finish the change.
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The events should still be in the Game Manager since we changed the flag.",
         geMan.GetNumEvents(), (unsigned int) 4);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void MessageTests::CheckMapNames(const dtGame::MapMessage& mapLoadedMsg,
      const dtGame::GameManager::NameVector& mapNames)
{
   static dtGame::GameManager::NameVector mapNamesGet;
   mapLoadedMsg.GetMapNames(mapNamesGet);
   CPPUNIT_ASSERT(mapNamesGet.size() > 0);
   CPPUNIT_ASSERT_EQUAL(mapNames.size(), mapNamesGet.size());
   CPPUNIT_ASSERT_MESSAGE(
         "The Map name in the \"" + mapLoadedMsg.GetMessageType().GetName() + "\" message should be " + mapNames[0],
         mapNames == mapNamesGet);
}

void MessageTests::RemoveOneProxy(dtCore::Map& map)
{
   dtCore::ActorRefPtrVector toFill;
   map.FindProxies(toFill, "", TestGameActorLibrary::TEST_TANK_GAME_ACTOR_TYPE->GetCategory(),
         TestGameActorLibrary::TEST_TANK_GAME_ACTOR_TYPE->GetName());

   CPPUNIT_ASSERT(!toFill.empty());
   map.RemoveProxy(*toFill[0]);
}

void MessageTests::TestChangeMap()
{
   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();
      dtGame::GameManager::NameVector mapNamesExpected;
      mapNamesExpected.push_back("Many Game Actors");
      mapNamesExpected.push_back("Many Game Actors the second");

      dtCore::RefPtr<dtCore::Map> mapA = &project.CreateMap(mapNamesExpected[0], "mga");
      dtCore::RefPtr<dtCore::Map> mapB = &project.CreateMap(mapNamesExpected[1], "mgb");

      dtGame::GameManager::NameVector mapNames2Expected;
      mapNames2Expected.push_back("Many More Game Actors");
      mapNames2Expected.push_back("Many More Game Actors the second");
      dtCore::RefPtr<dtCore::Map> map2A = &project.CreateMap(mapNames2Expected[0], "mg2");
      dtCore::RefPtr<dtCore::Map> map2B = &project.CreateMap(mapNames2Expected[1], "mg2b");

      createActors(*mapA);
      createActors(*mapB);
      createActors(*map2A);
      createActors(*map2B);

      dtCore::RefPtr<dtCore::GameEvent> reusedEvent = new dtCore::GameEvent("eventX", "Event");
      mapA->GetEventManager().AddEvent(*new dtCore::GameEvent("event1", "Event"));
      mapA->GetEventManager().AddEvent(*new dtCore::GameEvent("event2", "Event"));
      mapA->GetEventManager().AddEvent(*reusedEvent);
      mapB->GetEventManager().AddEvent(*new dtCore::GameEvent("event3", "Event"));
      mapB->GetEventManager().AddEvent(*new dtCore::GameEvent("event4", "Event"));
      mapB->GetEventManager().AddEvent(*reusedEvent);

      mapA->AddLibrary(mTestGameActorLibrary, "1.0");
      mapA->AddLibrary(mTestActorLibrary, "1.0");
      mapB->AddLibrary(mTestGameActorLibrary, "1.0");
      mapB->AddLibrary(mTestActorLibrary, "1.0");

      map2A->GetEventManager().AddEvent(*new dtCore::GameEvent("event5", "Event"));
      map2A->GetEventManager().AddEvent(*new dtCore::GameEvent("event6", "Event"));
      map2B->GetEventManager().AddEvent(*new dtCore::GameEvent("event7", "Event"));
      map2B->GetEventManager().AddEvent(*new dtCore::GameEvent("event8", "Event"));

      map2A->AddLibrary(mTestGameActorLibrary, "1.0");
      map2A->AddLibrary(mTestActorLibrary, "1.0");
      map2B->AddLibrary(mTestGameActorLibrary, "1.0");
      map2B->AddLibrary(mTestActorLibrary, "1.0");

      //remove one proxy to make the maps have different sizes.
      RemoveOneProxy(*map2A);
      RemoveOneProxy(*map2B);

      CPPUNIT_ASSERT(mapA->GetAllProxies().size() != map2A->GetAllProxies().size());

      project.SaveMap(*mapA);
      project.CloseMap(*mapA);

      project.SaveMap(*mapB);
      project.CloseMap(*mapB);

      project.SaveMap(*map2A);
      project.CloseMap(*map2A);

      project.SaveMap(*map2B);
      project.CloseMap(*map2B);

      dtGame::TestComponent& tc = *new dtGame::TestComponent("name");
      mGameManager->AddComponent(tc, dtGame::GameManager::ComponentPriority::NORMAL);

      //change the map set using the first set of Maps
      mGameManager->ChangeMapSet(mapNamesExpected, false);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of actors in the GM should be 0.", size_t(0), mGameManager->GetNumAllActors());

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();
      dtCore::RefPtr<const dtGame::Message> processMapChange = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_CHANGE_BEGIN);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_CHANGE_BEGIN message should have been processed.", processMapChange.valid());
      const dtGame::MapMessage* mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapChange.get());

      CheckMapNames(*mapLoadedMsg, mapNamesExpected);

      dtCore::RefPtr<const dtGame::Message> processMapUnloadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_UNLOADED);
      CPPUNIT_ASSERT_MESSAGE("A map unloaded message should NOT have been processed.", !processMapUnloadedMsg.valid());
      processMapUnloadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_UNLOAD_BEGIN);
      CPPUNIT_ASSERT_MESSAGE("A map unload begin message should NOT have been processed.", !processMapUnloadedMsg.valid());
      dtCore::RefPtr<const dtGame::Message> processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("A map loaded message should not have been processed.", !processMapLoadedMsg.valid());

      processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOAD_BEGIN);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_LOAD_BEGIN message should have been processed.", processMapLoadedMsg.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapLoadedMsg.get());
      CheckMapNames(*mapLoadedMsg, mapNamesExpected);


      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of actors in the GM should still be 0 after INFO_MAP_LOAD_BEGIN.",
                                    size_t(0), mGameManager->GetNumAllActors());

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of Actors in the GM should equal the Proxies in the loaded Maps.",
                                    mapA->GetAllProxies().size() + mapB->GetAllProxies().size() - 2,
                                    mGameManager->GetNumAllActors());

      dtCore::GameEventManager& mainGEM = dtCore::GameEventManager::GetInstance();
      //2 from each map, and one that is shared with the same unique id.
      CPPUNIT_ASSERT_EQUAL(5U, mainGEM.GetNumEvents());
      CPPUNIT_ASSERT(mainGEM.FindEvent("event1") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("event2") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("event3") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("event4") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("eventX") != NULL);

      processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("A map loaded message should have been processed.", processMapLoadedMsg.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapLoadedMsg.get());
      CheckMapNames(*mapLoadedMsg, mapNamesExpected);

      processMapChange = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_CHANGED);
      CPPUNIT_ASSERT_MESSAGE("A INFO_MAP_CHANGED message should have been processed.", processMapChange.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapChange.get());
      CheckMapNames(*mapLoadedMsg, mapNamesExpected);

      std::vector<dtGame::GameActorProxy*> gameActorProxyVec;
      mGameManager->GetAllGameActors(gameActorProxyVec);

      for (unsigned int i = 0; i < gameActorProxyVec.size(); ++i)
      {
         dtGame::GameActorProxy* gap = gameActorProxyVec[i];
         CPPUNIT_ASSERT(gap != NULL);
         // Not all game actors proxys have game actors now.
         if (gap->GetDrawable<dtGame::GameActor>() != NULL)
         {
            CPPUNIT_ASSERT_MESSAGE("The game actor proxy should be assigned to the game actor.", &gap->GetGameActor().GetGameActorProxy() == gap);
         }
         std::vector<const dtGame::Invokable*> invokables;
         gap->GetInvokables(invokables);
         CPPUNIT_ASSERT_MESSAGE("There should be invokables on the game actor proxies if BuildInvokables was called.", invokables.size() > 0);
      }

      gameActorProxyVec.clear();

      tc.reset();

      //change the map set using the second set of Maps
      mGameManager->ChangeMapSet(mapNames2Expected, false);

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      processMapUnloadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_UNLOAD_BEGIN);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_UNLOAD_BEGIN message should have been processed.",
            processMapUnloadedMsg.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapUnloadedMsg.get());
      CheckMapNames(*mapLoadedMsg, mapNamesExpected);

      processMapUnloadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_UNLOADED);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_UNLOADED message should have been processed.",
            processMapUnloadedMsg.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapUnloadedMsg.get());
      CheckMapNames(*mapLoadedMsg, mapNamesExpected);

      processMapChange = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_CHANGE_BEGIN);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_CHANGE_BEGIN message should have been processed.",
            processMapChange.valid());

      processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOAD_BEGIN);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_LOAD_BEGIN message should have been processed.",
            processMapLoadedMsg.valid());

      processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("An INFO_MAP_LOADED message should NOT have been processed.",
            !processMapLoadedMsg.valid());

      processMapChange = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_CHANGED);
      CPPUNIT_ASSERT_MESSAGE("A INFO_MAP_CHANGED message should NOT have been processed.",
            !processMapChange.valid());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of actors in the GM should be 0.", size_t(0), mGameManager->GetNumAllActors());

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of actors in the GM should match the second map minus two for the Crash Actors,"
               " which throw and exception in OnEnteredWorld, so should not end up in the GM.",
                                    map2A->GetAllProxies().size() + map2B->GetAllProxies().size() - 2,
                                    mGameManager->GetNumAllActors());

      // make sure that the events from both maps are in the gem.
      CPPUNIT_ASSERT_EQUAL(4U, mainGEM.GetNumEvents());
      CPPUNIT_ASSERT(mainGEM.FindEvent("event5") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("event6") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("event7") != NULL);
      CPPUNIT_ASSERT(mainGEM.FindEvent("event8") != NULL);

      processMapLoadedMsg = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_LOADED);
      CPPUNIT_ASSERT_MESSAGE("A map loaded message should have been processed.", processMapLoadedMsg.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapLoadedMsg.get());
      CheckMapNames(*mapLoadedMsg, mapNames2Expected);

      processMapChange = tc.FindProcessMessageOfType(dtGame::MessageType::INFO_MAP_CHANGED);
      CPPUNIT_ASSERT_MESSAGE("A INFO_MAP_CHANGED message should have been processed.", processMapChange.valid());
      mapLoadedMsg = static_cast<const dtGame::MapMessage*>(processMapChange.get());
      CheckMapNames(*mapLoadedMsg, mapNames2Expected);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
//   catch(const std::exception& e)
//   {
//      CPPUNIT_FAIL(std::string("Exception: ") + typeid(e).name() +
//                   std::string(" Message: ")  + e.what());
//   }
}

void MessageTests::TestGameEventMessage()
{
   try
   {
      dtCore::GameEventManager& eventMgr = dtCore::GameEventManager::GetInstance();
      dtCore::RefPtr<dtGame::Message> message =
            mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);
      dtGame::GameEventMessage* gameEventMsg = static_cast<dtGame::GameEventMessage*>(message.get());

      eventMgr.AddEvent(*(new dtCore::GameEvent("TestEvent1","This is test event one.")));

      dtCore::GameEvent* event = eventMgr.FindEvent("TestEvent1");
      const dtCore::GameEvent* event2;
      gameEventMsg->SetGameEvent(*event);
      event2 = gameEventMsg->GetGameEvent();

      CPPUNIT_ASSERT_MESSAGE("Game event was invalid.",event2 != NULL);
      CPPUNIT_ASSERT_MESSAGE("Game event names were not equal.",event->GetName() == event2->GetName());
      CPPUNIT_ASSERT_MESSAGE("Game event descriptions were not equal.",event->GetDescription() == event2->GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Game event names were not equal.",event->GetUniqueId() == event2->GetUniqueId());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void MessageTests::TestChangeMapErrorConditions()
{
   try
   {
      CPPUNIT_ASSERT_THROW(mGameManager->ChangeMap(""), dtUtil::Exception);

      CPPUNIT_ASSERT_THROW(mGameManager->ChangeMap("This map does not exist"), dtUtil::Exception);

      CPPUNIT_ASSERT_THROW(mGameManager->ChangeMap("../examples/testMap/testMap"), dtUtil::Exception);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }

}

void MessageTests::TestDefaultMessageProcessorWithPauseResumeRequests()
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   CPPUNIT_ASSERT(defMsgProcessor.GetAcceptTimeRequests());

   dtCore::RefPtr<dtGame::Message> pauseRequest;
   mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_PAUSE, pauseRequest);
   mGameManager->SendMessage(*pauseRequest);

   dtCore::System::GetInstance().Step(0.016);
   CPPUNIT_ASSERT_MESSAGE("The Game Manager should now be paused.", mGameManager->IsPaused());

   defMsgProcessor.SetAcceptTimeRequests(false);
   CPPUNIT_ASSERT(!defMsgProcessor.GetAcceptTimeRequests());

   dtCore::RefPtr<dtGame::Message> resumeRequest;
   mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_RESUME, resumeRequest);
   mGameManager->SendMessage(*resumeRequest);
   dtCore::System::GetInstance().Step(0.016);

   CPPUNIT_ASSERT_MESSAGE("The Game Manager should NOT be resumed because it was disabled.", mGameManager->IsPaused());

   defMsgProcessor.SetAcceptTimeRequests(true);
   mGameManager->SendMessage(*resumeRequest);
   dtCore::System::GetInstance().Step(0.016);

   CPPUNIT_ASSERT_MESSAGE("The Game Manager should now be resumed.", !mGameManager->IsPaused());

   defMsgProcessor.SetAcceptTimeRequests(false);
   mGameManager->SendMessage(*pauseRequest);
   dtCore::System::GetInstance().Step(0.016);

   CPPUNIT_ASSERT_MESSAGE("The Game Manager should NOT be paused.", !mGameManager->IsPaused());
}

void MessageTests::TestDefaultMessageProcessorWithMapRequests()
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   try
   {
      dtCore::Project& project = dtCore::Project::GetInstance();
      dtGame::GameManager::NameVector mapNamesExpected;
      mapNamesExpected.push_back("Many Game Actors");

      dtCore::RefPtr<dtCore::Map> mapA = &project.CreateMap(mapNamesExpected[0], "mga");

      mapA->AddLibrary(mTestGameActorLibrary, "1.0");
      mapA->AddLibrary(mTestActorLibrary, "1.0");

      createActors(*mapA);

      project.SaveMap(*mapA);
      project.CloseMap(*mapA);

      CPPUNIT_ASSERT(mGameManager->GetNumAllActors() == 0);

      defMsgProcessor.SetAcceptMapLoadRequests(false);
      dtCore::RefPtr<dtGame::MapMessage> mapMsg;
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_LOAD_MAP, mapMsg);
      mapMsg->SetMapNames(mapNamesExpected);
      mGameManager->SendMessage(*mapMsg);
      dtCore::System::GetInstance().Step(0.016);

      CPPUNIT_ASSERT_MESSAGE("No map should have loaded because the requests are DISABLED.", mGameManager->GetNumAllActors() == 0);
      CPPUNIT_ASSERT(!project.IsMapOpen(mapNamesExpected[0]));

      defMsgProcessor.SetAcceptMapLoadRequests(true);
      mGameManager->SendMessage(*mapMsg);
      dtCore::System::GetInstance().Step(0.016);

      CPPUNIT_ASSERT_MESSAGE("The map should have loaded because the requests are ENABLED.", mGameManager->GetNumAllActors() > 0);
      CPPUNIT_ASSERT(project.IsMapOpen(mapNamesExpected[0]));

      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_UNLOAD_MAP, mapMsg);
      mapMsg->SetMapNames(mapNamesExpected);
      defMsgProcessor.SetAcceptMapLoadRequests(false);
      mGameManager->SendMessage(*mapMsg);
      dtCore::System::GetInstance().Step(0.016);

      CPPUNIT_ASSERT_MESSAGE("The map should NOT have unloaded because the requests are DISABLED.", mGameManager->GetNumAllActors() > 0);
      CPPUNIT_ASSERT(project.IsMapOpen(mapNamesExpected[0]));

      defMsgProcessor.SetAcceptMapLoadRequests(true);
      mGameManager->SendMessage(*mapMsg);
      dtCore::System::GetInstance().Step(0.016);

      CPPUNIT_ASSERT_MESSAGE("The map should have unloaded because the requests are ENABLED.", mGameManager->GetNumAllActors() == 0);
      CPPUNIT_ASSERT(!project.IsMapOpen(mapNamesExpected[0]));

      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::REQUEST_CHANGE_MAP, mapMsg);
      mapMsg->SetMapNames(mapNamesExpected);

      defMsgProcessor.SetAcceptMapChangeRequests(false);
      mGameManager->SendMessage(*mapMsg);
      // Change map takes a few frames...
      dtCore::System::GetInstance().Step(0.016);
      dtCore::System::GetInstance().Step(0.016);
      dtCore::System::GetInstance().Step(0.016);
      CPPUNIT_ASSERT_MESSAGE("No map should have loaded because the requests are DISABLED.", mGameManager->GetNumAllActors() == 0);
      CPPUNIT_ASSERT(!project.IsMapOpen(mapNamesExpected[0]));

      defMsgProcessor.SetAcceptMapChangeRequests(true);
      mGameManager->SendMessage(*mapMsg);
      // Change map takes a few frames...
      dtCore::System::GetInstance().Step(0.016);
      CPPUNIT_ASSERT_MESSAGE("No map should have loaded because it a map change takes several frames.", mGameManager->GetNumAllActors() == 0);
      dtCore::System::GetInstance().Step(0.016);
      dtCore::System::GetInstance().Step(0.016);
      CPPUNIT_ASSERT_MESSAGE("The map should have loaded after a few steps.", mGameManager->GetNumAllActors() > 0);
      CPPUNIT_ASSERT(project.IsMapOpen(mapNamesExpected[0]));

      mGameManager->CloseCurrentMap();
      dtCore::System::GetInstance().Step(0.016);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }

}

void MessageTests::TestDefaultMessageProcessorWithPauseResumeCommands()
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   CPPUNIT_ASSERT_MESSAGE("The Game Manager should not start out paused.", !mGameManager->IsPaused());

   dtCore::RefPtr<dtGame::Message> pauseCommand;
   mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::COMMAND_PAUSE, pauseCommand);

   mGameManager->SendMessage(*pauseCommand);
   dtCore::System::GetInstance().Step(0.016);

   CPPUNIT_ASSERT_MESSAGE("The Game Manager should now be paused.", mGameManager->IsPaused());

   dtCore::RefPtr<dtGame::Message> resumeCommand;
   mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::COMMAND_RESUME, resumeCommand);
   mGameManager->SendMessage(*resumeCommand);
   dtCore::System::GetInstance().Step(0.016);

   CPPUNIT_ASSERT_MESSAGE("The Game Manager should now be resumed.", !mGameManager->IsPaused());

}

void MessageTests::TestDefaultMessageProcessorWithRemoteActorUpdates()
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);

   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(true, true);
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(true, false);
}

void MessageTests::TestDefaultMessageProcessorWithLocalActorUpdates()
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::HIGHEST);

   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false, true, dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false, false, dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false, true, dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_ALL);
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false, false, dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_ALL);
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false, true, dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_WITH_PROPERTY_FILTER);
   TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(false, false, dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_WITH_PROPERTY_FILTER);
}

void MessageTests::TestDefaultMessageProcessorWithLocalOrRemoteActorUpdates(bool remote, bool partial,
         dtGame::GameActorProxy::LocalActorUpdatePolicy& policy)
{

   dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");

   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtCore::BaseActorObject> ap = mGameManager->CreateActor(*type);
   dtCore::RefPtr<dtCore::BaseActorObject> apParent = mGameManager->CreateActor(*type);
   dtCore::RefPtr<dtCore::BaseActorObject> apSecondParent = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(ap->IsGameActor());
   CPPUNIT_ASSERT(apParent->IsGameActor());
   CPPUNIT_ASSERT(apSecondParent->IsGameActor());
    dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   dtCore::RefPtr<dtGame::GameActorProxy> gapParent = dynamic_cast<dtGame::GameActorProxy*>(apParent.get());
   dtCore::RefPtr<dtGame::GameActorProxy> gapSecondParent = dynamic_cast<dtGame::GameActorProxy*>(apSecondParent.get());
   CPPUNIT_ASSERT(gap != NULL);
   CPPUNIT_ASSERT(gapParent != NULL);
   CPPUNIT_ASSERT(gapSecondParent != NULL);

   CPPUNIT_ASSERT_MESSAGE("This property should have been in the accept list", gap->ShouldAcceptPropertyInLocalUpdate("OneIsFired"));
   CPPUNIT_ASSERT_MESSAGE("This property should have been in the accept list", gap->ShouldAcceptPropertyInLocalUpdate("TickRemotes"));
   CPPUNIT_ASSERT_MESSAGE("This property should not be in the accept list because it was added, then removed", !gap->ShouldAcceptPropertyInLocalUpdate("TickLocals"));
   CPPUNIT_ASSERT_MESSAGE("This property should not be in the accept list because it was never added", !gap->ShouldAcceptPropertyInLocalUpdate("TestActorId"));

   gap->SetLocalActorUpdatePolicy(policy);
   gap->SetParentActor(gapParent);
   try
   {
      mGameManager->AddActor(*gap, remote, false);
      mGameManager->AddActor(*gapParent, remote, false);
      mGameManager->AddActor(*gapSecondParent, remote, false);
   }
   catch (const dtUtil::Exception&)
   {
      CPPUNIT_FAIL("Actor should be added with no problems");
   }

   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorUpdateMsg =
      static_cast<dtGame::ActorUpdateMessage*>(mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED).get());

   CPPUNIT_ASSERT(!actorUpdateMsg->IsParentIDSet());

   dtCore::UniqueId oldId(gap->GetProperty("TestActorId")->ToString());
   CPPUNIT_ASSERT_MESSAGE("The test actor id should be empty.", oldId.ToString().empty());
   gap->GetProperty("TestActorId")->FromString("33232");
   oldId = gap->GetProperty("TestActorId")->ToString();
   CPPUNIT_ASSERT_MESSAGE("The test actor id should not be empty.", !oldId.ToString().empty());
   CPPUNIT_ASSERT_MESSAGE("OneIsFired should be false.", gap->GetProperty("OneIsFired")->ToString() == "false");
   CPPUNIT_ASSERT_MESSAGE("TickLocals should be 0.", gap->GetProperty("TickLocals")->ToString() == "0");
   CPPUNIT_ASSERT_MESSAGE("TickRemotes should be 0.", gap->GetProperty("TickRemotes")->ToString() == "0");

   if (partial)
   {
      std::vector<dtUtil::RefString> params;
      params.push_back("OneIsFired");
      gap->PopulateActorUpdate(*actorUpdateMsg, params);
      actorUpdateMsg->SetParentID(dtCore::UniqueId(false));
      CPPUNIT_ASSERT(actorUpdateMsg->IsParentIDSet());
   }
   else
   {
      gap->PopulateActorUpdate(*actorUpdateMsg);
      actorUpdateMsg->SetParentID(gapSecondParent->GetId());
      CPPUNIT_ASSERT(actorUpdateMsg->IsParentIDSet());
   }

   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Name") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Name") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Category") != NULL);

   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Name")->ToString() == gap->GetName());
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Name")->ToString() == gap->GetActorType().GetName());
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Actor Type Category")->ToString() == gap->GetActorType().GetCategory());

   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("OneIsFired") != NULL);
   CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("OneIsFired")->ToString() == "false");
   actorUpdateMsg->GetUpdateParameter("OneIsFired")->FromString("true");

   if (partial)
   {
      CPPUNIT_ASSERT_MESSAGE("TickLocals should not be part of the update.",
         actorUpdateMsg->GetUpdateParameter("TickLocals") == NULL);
      CPPUNIT_ASSERT_MESSAGE("TickRemotes should not be part of the update.",
         actorUpdateMsg->GetUpdateParameter("TickRemotes") == NULL);
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("TickLocals should be part of the update.",
         actorUpdateMsg->GetUpdateParameter("TickLocals") != NULL);
      CPPUNIT_ASSERT_MESSAGE("TickRemotes should be part of the update.",
         actorUpdateMsg->GetUpdateParameter("TickRemotes") != NULL);

      CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("TickLocals")->ToString() == "0");
      CPPUNIT_ASSERT(actorUpdateMsg->GetUpdateParameter("TickRemotes")->ToString() == "0");

      actorUpdateMsg->GetUpdateParameter("TickLocals")->FromString("96");
      actorUpdateMsg->GetUpdateParameter("TickRemotes")->FromString("107");
      actorUpdateMsg->GetUpdateParameter("TestActorId")->FromString("3333");

   }

   if (!remote)
   {
      actorUpdateMsg->SetSource(mGameManager->GetMachineInfo());
      mGameManager->SendMessage(*actorUpdateMsg);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Message Was Not Remote. OneIsFired should still be false.",
                                    gap->GetProperty("OneIsFired")->ToString(), std::string("false"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Message Was Not Remote. TickLocals should still be 0.",
                                    gap->GetProperty("TickLocals")->ToString(), std::string("0"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Message Was Not Remote. TickRemotes should still be 0.",
                                    gap->GetProperty("TickRemotes")->ToString(), std::string("0"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Message Was Not Remote. The new Id should NOT be set.", oldId.ToString(), gap->GetProperty("TestActorId")->ToString());
   }

   actorUpdateMsg->SetSource(*new dtGame::MachineInfo);
   mGameManager->SendMessage(*actorUpdateMsg);
   dtCore::System::GetInstance().Step();

   if (remote || policy != dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL)
   {
      bool acceptWithFilter = !remote && policy == dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_WITH_PROPERTY_FILTER;

      CPPUNIT_ASSERT_EQUAL_MESSAGE("OneIsFired should be changed to true.",
                             gap->GetProperty("OneIsFired")->ToString(), std::string("true"));
      if (partial)
      {
         CPPUNIT_ASSERT_EQUAL_MESSAGE("TickLocals should still be 0.",
                                gap->GetProperty("TickLocals")->ToString(), std::string("0"));
         CPPUNIT_ASSERT_EQUAL_MESSAGE("TickRemotes should still be 0.",
                                gap->GetProperty("TickRemotes")->ToString(), std::string("0"));

         CPPUNIT_ASSERT(gap->GetParentActor() == NULL);
      }
      else
      {
         CPPUNIT_ASSERT_EQUAL_MESSAGE("TickRemotes should be changed to 107.",
                                gap->GetProperty("TickRemotes")->ToString(), std::string("107"));
         if (!acceptWithFilter)
         {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("TickLocals should be changed to 96.",
                                   gap->GetProperty("TickLocals")->ToString(), std::string("96"));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The new Id should be set.", std::string("3333"), gap->GetProperty("TestActorId")->ToString());
         }
         else
         {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("TickLocals should still be 0.",
                                          gap->GetProperty("TickLocals")->ToString(), std::string("0"));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The new Id should NOT be set.", oldId.ToString(), gap->GetProperty("TestActorId")->ToString());
         }

         CPPUNIT_ASSERT(gap->GetParentActor() == gapSecondParent);
      }
   }
   else
   {
      CPPUNIT_ASSERT_EQUAL_MESSAGE("OneIsFired should still be false.",
                                    gap->GetProperty("OneIsFired")->ToString(), std::string("false"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("TickLocals should still be 0.",
                                    gap->GetProperty("TickLocals")->ToString(), std::string("0"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("TickRemotes should still be 0.",
                                    gap->GetProperty("TickRemotes")->ToString(), std::string("0"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The new Id should NOT be set.", oldId.ToString(), gap->GetProperty("TestActorId")->ToString());

      CPPUNIT_ASSERT(gap->GetParentActor() == gapParent);
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
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");

   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtCore::BaseActorObject> ap = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(ap->IsGameActor());
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   CPPUNIT_ASSERT(gap != NULL);

   try
   {
      mGameManager->AddActor(*gap, remote, false);
   }
   catch (const dtUtil::Exception&)
   {
      CPPUNIT_FAIL("Actor should be added with no problems");
   }

   dtCore::RefPtr<dtGame::Message> actorDeleteMsg = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED);

   actorDeleteMsg->SetAboutActorId(gap->GetId());

   mGameManager->SendMessage(*actorDeleteMsg);
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();

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
   TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(true, true);
}

void MessageTests::TestDefaultMessageProcessorWithRemoteActorCreatesNoParent()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(true, false);
}

void MessageTests::TestDefaultMessageProcessorWithLocalActorCreates()
{
   TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(false, true);
}

void MessageTests::TestDefaultMessageProcessorWithLocalOrRemoteActorCreates(bool remote, bool addParent)
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");

   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtCore::BaseActorObject> apParent = mGameManager->CreateActor(*type);
   dtCore::RefPtr<dtCore::BaseActorObject> ap = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(apParent->IsGameActor());
   CPPUNIT_ASSERT(ap->IsGameActor());
   dtCore::RefPtr<dtGame::GameActorProxy> gapParent = dynamic_cast<dtGame::GameActorProxy*>(apParent.get());
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   CPPUNIT_ASSERT(gapParent != NULL);
   CPPUNIT_ASSERT(gap != NULL);
   mGameManager->AddActor(*gapParent, false, true);
   gap->SetParentActor(gapParent);

   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorCreateMsg =
      static_cast<dtGame::ActorUpdateMessage*>(mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED).get());

   CPPUNIT_ASSERT_MESSAGE("OneIsFired should be false.", gap->GetProperty("OneIsFired")->ToString() == "false");
   CPPUNIT_ASSERT_MESSAGE("TickLocals should be 0.", gap->GetProperty("TickLocals")->ToString() == "0");
   CPPUNIT_ASSERT_MESSAGE("TickRemotes should be 0.", gap->GetProperty("TickRemotes")->ToString() == "0");

   gap->PopulateActorUpdate(*actorCreateMsg);
   if (remote)
   {
      //make it remote
      actorCreateMsg->SetSource(*new dtGame::MachineInfo());
   }

   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::NAME_PARAMETER) != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::ACTOR_TYPE_NAME_PARAMETER) != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::ACTOR_TYPE_CATEGORY_PARAMETER) != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::PARENT_ID_PARAMETER) != NULL);

   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::NAME_PARAMETER)->ToString() == gap->GetName());
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::ACTOR_TYPE_NAME_PARAMETER)->ToString() == gap->GetActorType().GetName());
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::ACTOR_TYPE_CATEGORY_PARAMETER)->ToString() == gap->GetActorType().GetCategory());
   CPPUNIT_ASSERT(actorCreateMsg->GetParameter(dtGame::ActorUpdateMessage::PARENT_ID_PARAMETER)->ToString() == gapParent->GetId().ToString());

   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("OneIsFired") != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("TickLocals") != NULL);
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("TickRemotes") != NULL);

   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("OneIsFired")->ToString() == "false");
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("TickLocals")->ToString() == "0");
   CPPUNIT_ASSERT(actorCreateMsg->GetUpdateParameter("TickRemotes")->ToString() == "0");

   actorCreateMsg->GetUpdateParameter("OneIsFired")->FromString("true");
   actorCreateMsg->GetUpdateParameter("TickLocals")->FromString("96");
   actorCreateMsg->GetUpdateParameter("TickRemotes")->FromString("107");

   mGameManager->SendMessage(*actorCreateMsg);

   CPPUNIT_ASSERT(mGameManager->FindGameActorById(gap->GetId()) == NULL);

   dtCore::System::GetInstance().Step(0.01667);


   dtCore::RefPtr<dtGame::GameActorProxy> gapRemote = mGameManager->FindGameActorById(gap->GetId());

   if (remote)
   {
      CPPUNIT_ASSERT_MESSAGE("The remote actor should have been created.", gapRemote != NULL);

      CPPUNIT_ASSERT_MESSAGE("The remote actor should have the same actor type as the real actor.", gapRemote->GetActorType() == gap->GetActorType());
      CPPUNIT_ASSERT_MESSAGE("The remote actor should have the same name as the real actor.", gapRemote->GetName() == gap->GetName());

      CPPUNIT_ASSERT_MESSAGE("OneIsFired should be changed to true.", gapRemote->GetProperty("OneIsFired")->ToString() == "true");
      CPPUNIT_ASSERT_MESSAGE("TickLocals should be changed to 96.", gapRemote->GetProperty("TickLocals")->ToString() == "96");
      CPPUNIT_ASSERT_MESSAGE("TickRemotes should be changed to 107.", gapRemote->GetProperty("TickRemotes")->ToString() == "107");

      CPPUNIT_ASSERT(gapRemote->GetParentActor() == gapParent);

      CPPUNIT_ASSERT_MESSAGE("The created actor should be remote.", gapRemote->IsRemote());
      CPPUNIT_ASSERT_MESSAGE("The created actor should not be published.", !gapRemote->IsPublished());

      if (addParent)
      {
         dtCore::RefPtr<dtGame::ActorUpdateMessage> actorUpdateMsg;
         mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, actorUpdateMsg);

         CPPUNIT_ASSERT(!actorUpdateMsg->IsParentIDSet());

         std::vector<dtUtil::RefString> params;
         params.push_back("OneIsFired");
         gap->PopulateActorUpdate(*actorUpdateMsg, params);
         CPPUNIT_ASSERT(actorUpdateMsg->IsParentIDSet());
         CPPUNIT_ASSERT(actorUpdateMsg->GetParentID() == gapRemote->GetParentActor()->GetId());
         actorUpdateMsg->SetParentIDToUnset();
         CPPUNIT_ASSERT(!actorUpdateMsg->IsParentIDSet());

         dtCore::UniqueId id = gapRemote->GetParentActor()->GetId();
         gapRemote->ApplyActorUpdate(*actorUpdateMsg);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The id should not have changed.", id, gapRemote->GetParentActor()->GetId());
      }

   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("No actor should have been created.", gapRemote == NULL);
   }

}

/////////////////////////////////////////////////////////////////////
void MessageTests::TestRemoteActorCreatesFromPrototype()
{

   // This test focuses only on the prototype part since the tests above hit the essentials.
   // We are just testing that an actor create message will create an remote actor using the prototype.

   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
   CPPUNIT_ASSERT(type != NULL);

   // Create a prototype actor to work with.
   dtCore::RefPtr<TestGameActor1> prototypeActor;
   mGameManager->CreateActor(*type.get(), prototypeActor);
   CPPUNIT_ASSERT(prototypeActor != NULL);
   prototypeActor->SetName("Test1Prototype");
   prototypeActor->SetTickLocals(59);
   prototypeActor->SetTickRemotes(41);
   dtCore::UniqueId prototypeId = dtCore::UniqueId("ABCDEFG");
   prototypeActor->SetId(prototypeId);
   mGameManager->AddActorAsAPrototype(*prototypeActor);

   // Now we need to actually create an actor from our prototype. Use that to populate a create message.
   // Note, we don't actually add our temp actor to the GM. It acts like a 'remote' actor on another system.
   dtCore::RefPtr<dtCore::BaseActorObject> tempBogusPrototype = mGameManager->CreateActorFromPrototype(prototypeId);
   dtCore::RefPtr<dtGame::GameActorProxy> tempBogusGameProxy = dynamic_cast<dtGame::GameActorProxy*>(tempBogusPrototype.get());
   dtCore::RefPtr<TestGameActor1> tempBogusActor = dynamic_cast<TestGameActor1*>(tempBogusGameProxy.get());
   tempBogusActor->SetTickLocals(11);
   tempBogusActor->SetName("MyUpdateActor");
   dtCore::UniqueId createdId = dtCore::UniqueId("1234567890");
   tempBogusActor->SetId(createdId);

   // Create an actor update message to simulate the creation process from a remote system
   // To do that, we populate the TickLocal property (plus name and Id).
   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorCreateMsg =
      static_cast<dtGame::ActorUpdateMessage*>(mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED).get());
   std::vector<dtUtil::RefString> params;
   params.push_back("TickLocals");
   tempBogusGameProxy->PopulateActorUpdate(*actorCreateMsg, params);
   //make it remote
   actorCreateMsg->SetSource(*new dtGame::MachineInfo());


   // Send it!
   mGameManager->SendMessage(*actorCreateMsg);
   dtCore::AppSleep(10);
   dtCore::System::GetInstance().Step();


   // The create message had a 'local tick' on it, but nothing else. So, when we send it, we should get a
   // new actor with the right name & actorid. The tick remote should be what was on the prototype (41),
   // but the tick local we set manually to 11.


   dtCore::RefPtr<dtGame::GameActorProxy> gapRemote = mGameManager->FindGameActorById(createdId);
   dtCore::RefPtr<TestGameActor1> gapRemoteActor = dynamic_cast<TestGameActor1*> (gapRemote.get());

   CPPUNIT_ASSERT_MESSAGE("The remote actor should have been created.", gapRemote != NULL);
   CPPUNIT_ASSERT_MESSAGE("The remote actor should have the same actor type as our prototype.",
      gapRemote->GetActorType() == prototypeActor->GetActorType());

   CPPUNIT_ASSERT_MESSAGE("The create message should not have affected our prototype - name. ",
      prototypeActor->GetName() == "Test1Prototype");
   CPPUNIT_ASSERT_MESSAGE("The remote actor should have the value from the update message - name.",
      gapRemote->GetName() == "MyUpdateActor");

   CPPUNIT_ASSERT_MESSAGE("The create message should not have affected our prototype - uniqueid. ",
      prototypeActor->GetId() == prototypeId);
   CPPUNIT_ASSERT_MESSAGE("The remote actor should have the value from the update message - uniqueid.",
      gapRemote->GetId() == createdId);

   CPPUNIT_ASSERT_MESSAGE("The create message should not have affected our prototype - tick locals. ",
      prototypeActor->GetTickLocals() == 59);
   CPPUNIT_ASSERT_MESSAGE("The remote actor should have the value from the update message - tick locals.",
      gapRemoteActor->GetTickLocals() == 11);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("The remote actor should have the value from the prototype - tick remotes. ",
      prototypeActor->GetTickRemotes(), gapRemoteActor->GetTickRemotes());

   CPPUNIT_ASSERT_MESSAGE("The created actor should be remote.", gapRemote->IsRemote());
   CPPUNIT_ASSERT_MESSAGE("The created actor should not be published.", !gapRemote->IsPublished());
}


void MessageTests::TestActorEnteredWorldMessage()
{
   dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent("name");
   CPPUNIT_ASSERT(tc.valid());
   mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::HIGHEST);
   std::vector<dtCore::RefPtr<const dtGame::Message> > msgs;

   dtCore::RefPtr<dtCore::BaseActorObject> proxy = mGameManager->CreateActor("ExampleActors", "Test1Actor");
   CPPUNIT_ASSERT(proxy.valid());
   dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
   CPPUNIT_ASSERT_MESSAGE("A Test1Actor actor was created. The dynamic_cast to a GameActorProxy should not be NULL", gap != NULL);
   dtCore::Scene* scene = gap->GetDrawable()->GetSceneParent();
   CPPUNIT_ASSERT_MESSAGE("The game actor proxy has not yet been added to the game manager, its scene parent pointer should be NULL", scene == NULL);

   mGameManager->AddActor(*gap, false, false);
   dtCore::System::GetInstance().Step();
   msgs = tc->GetReceivedProcessMessages();
   bool receivedCreateMsg = false;
   for (unsigned int i = 0; i < msgs.size(); ++i)
   {
      if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ACTOR_CREATED)
      {
         receivedCreateMsg = true;
      }
   }

   CPPUNIT_ASSERT_MESSAGE("An actor created message should have been sent", receivedCreateMsg);
   scene = gap->GetDrawable()->GetSceneParent();
   CPPUNIT_ASSERT_MESSAGE("Now that the game actor proxy was added to the game manager, its scene parent should not be NULL", scene != NULL);
}

///////////////////////////////////////////////////////////////////////
void MessageTests::TestPartialUpdateDoesNotCreateActor()
{
   DoTestOfPartialUpdateDoesNotCreateActor(true);
}

void MessageTests::TestNonPartialUpdateDoesCreateActor()
{
   DoTestOfPartialUpdateDoesNotCreateActor(false);
}

void MessageTests::DoTestOfPartialUpdateDoesNotCreateActor(bool testWithPartial)
{
   dtGame::DefaultMessageProcessor& defMsgProcessor = *new dtGame::DefaultMessageProcessor();
   mGameManager->AddComponent(defMsgProcessor, dtGame::GameManager::ComponentPriority::NORMAL);

   dtCore::RefPtr<const dtCore::ActorType> type = mGameManager->FindActorType("ExampleActors","Test1Actor");
   CPPUNIT_ASSERT(type != NULL);
   dtCore::RefPtr<dtCore::BaseActorObject> ap = mGameManager->CreateActor(*type);

   CPPUNIT_ASSERT(ap->IsGameActor());
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(ap.get());
   CPPUNIT_ASSERT(gap != NULL);

   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorUpdateMsg =
      static_cast<dtGame::ActorUpdateMessage*>(mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED).get());

   CPPUNIT_ASSERT_MESSAGE("Partial should default to false.", !actorUpdateMsg->IsPartialUpdate());
   actorUpdateMsg->SetPartialUpdate(testWithPartial);
   CPPUNIT_ASSERT_MESSAGE("Partial should now be set.", testWithPartial == actorUpdateMsg->IsPartialUpdate());

   gap->PopulateActorUpdate(*actorUpdateMsg);
   //make it remote
   actorUpdateMsg->SetSource(*new dtGame::MachineInfo());
   CPPUNIT_ASSERT(actorUpdateMsg->GetParameter("Is Partial Update") != NULL);

   mGameManager->SendMessage(*actorUpdateMsg);

   dtCore::AppSleep(5);
   dtCore::System::GetInstance().Step();

   dtCore::RefPtr<dtGame::GameActorProxy> gapRemote = mGameManager->FindGameActorById(gap->GetId());
   if (testWithPartial)
   {
      CPPUNIT_ASSERT_MESSAGE("With partial - actor should NOT have been created.", gapRemote == NULL);
   }
   else
   {
      CPPUNIT_ASSERT_MESSAGE("Not partial - actor should exist.", gapRemote != NULL);
   }
}
