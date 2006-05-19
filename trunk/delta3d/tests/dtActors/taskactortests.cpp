/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/gameevent.h>
#include <dtActors/taskactor.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/taskactorrollup.h>
#include <dtActors/taskactorordered.h>
#include <dtGame/clientgamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/basemessages.h>
#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtUtil/stringutils.h>

#include <vector>

/**
 * This test suite tests the base task actor proxy as well as the different
 * task subclasses.
 */
class TaskActorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(TaskActorTests);
      CPPUNIT_TEST(TestTaskActorDefaultValues);
      CPPUNIT_TEST(TestTaskSubTasks);
      CPPUNIT_TEST(TestTaskReparentOnAdd);
      CPPUNIT_TEST(TestGameEventTaskActor);
      CPPUNIT_TEST(TestRollupTaskActor);
      CPPUNIT_TEST(TestOrderedTaskActor);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestTaskActorDefaultValues();
      void TestTaskSubTasks();
      void TestTaskReparentOnAdd();
      void TestGameEventTaskActor();
      void TestRollupTaskActor();
      void TestOrderedTaskActor();

   private:
      dtDAL::GameEventManager *mEventMgr;
      dtCore::RefPtr<dtGame::ClientGameManager> mGameManager;
      static char* mTestGameActorLibrary;
      static char* mTestActorLibrary;
};

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TaskActorTests);
#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* TaskActorTests::mTestGameActorLibrary="testGameActorLibraryd";
char* TaskActorTests::mTestActorLibrary="testActorLibraryd";
#else
char* TaskActorTests::mTestGameActorLibrary="testGameActorLibrary";
char* TaskActorTests::mTestActorLibrary="testActorLibrary";
#endif

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::setUp()
{
   try
   {
      mEventMgr = &dtDAL::GameEventManager::GetInstance();
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::ClientGameManager(*scene);
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::Instance()->SetShutdownOnWindowClose(false);
      dtCore::System::Instance()->Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::tearDown()
{
   dtCore::System::Instance()->SetPause(false);
   dtCore::System::Instance()->Stop();
   mGameManager->DeleteAllActors();
   mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
   mGameManager = NULL;
   mEventMgr->ClearAllEvents();
   mEventMgr = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskActorDefaultValues()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<dtDAL::ActorProxy> proxy =
            mGameManager->CreateActor(*taskActorType);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",proxy.valid());

      //Make sure the correct properties exist on the proxy.
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a description property.",
         proxy->GetProperty("Description") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a passing score property.",
         proxy->GetProperty("PassingScore") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a score property.",
         proxy->GetProperty("Score") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a weight property.",
         proxy->GetProperty("Weight") != NULL);

      //Check the default values of a newly created base task actor.
      dtDAL::StringActorProperty *descProp =
         static_cast<dtDAL::StringActorProperty *>(proxy->GetProperty("Description"));
      CPPUNIT_ASSERT_MESSAGE("Task description should be empty.",descProp->GetValue().empty());

      dtDAL::FloatActorProperty *valueProp =
         static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("PassingScore"));
      CPPUNIT_ASSERT_MESSAGE("Task passing score should be 1.0.",valueProp->GetValue() == 1.0);

      valueProp = static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("Score"));
      CPPUNIT_ASSERT_MESSAGE("Task score should be 0.0.",valueProp->GetValue() == 0.0f);

      valueProp = static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("Weight"));
      CPPUNIT_ASSERT_MESSAGE("Task weight should be 1.0.",valueProp->GetValue() == 1.0f);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskSubTasks()
{
   try
   {
      unsigned int i;

      dtCore::RefPtr<dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> parentProxy =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",parentProxy.valid());

      //Create a bunch of actors and add them as children.
      for (i=0; i<25; i++)
      {
         dtCore::RefPtr<dtActors::TaskActorProxy> childProxy = NULL;

         childProxy = dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*taskActorType).get());
         CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy.valid());

         childProxy->SetName("ChildProxy" + dtUtil::ToString(i));
         parentProxy->AddSubTaskProxy(*childProxy);
      }

      std::vector<dtCore::RefPtr<dtActors::TaskActorProxy> > children(parentProxy->GetAllSubTaskProxies());
      CPPUNIT_ASSERT_MESSAGE("Number of child tasks should have been 25.",children.size() == 25);

      //Make sure the parent was set correctly and that we can find the task.
      for (i=0; i<25; i++)
      {
         CPPUNIT_ASSERT_MESSAGE("Parent was not set correctly.",children[i]->GetParentTaskProxy() == parentProxy.get());
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            parentProxy->FindSubTaskProxy(children[i]->GetGameActor().GetUniqueId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            parentProxy->FindSubTaskProxy(children[i]->GetGameActor().GetName()) != NULL);
      }

      for (i=0; i<25; i+=2)
      {
         parentProxy->RemoveSubTaskProxy(*children[i]);
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be NULL after removing it.",
            children[i]->GetParentTaskProxy() == NULL);
      }

      std::vector<dtCore::RefPtr<dtActors::TaskActorProxy> > children2(parentProxy->GetAllSubTaskProxies());
      for (i=0; i<children2.size(); i++)
      {
         parentProxy->RemoveSubTaskProxy(children2[i]->GetGameActor().GetName());
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be NULL after removing it by name.",
            children2[i]->GetParentTaskProxy() == NULL);
      }

      CPPUNIT_ASSERT_MESSAGE("There should be no more child tasks left.",parentProxy->GetNumSubTaskProxies() == 0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskReparentOnAdd()
{
   try
   {
      //This tests a nested hierarchy of tasks.
      //parent
      //   --> Child1
      //          --> Child2
      //                 --> Child3

      dtCore::RefPtr<dtDAL::ActorType> taskActorType =
            mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> parentProxy =
         dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",parentProxy.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy1 =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy1.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy2 =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy2.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy3 =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy3.valid());

      parentProxy->AddSubTaskProxy(*childProxy1);
      childProxy1->AddSubTaskProxy(*childProxy2);
      childProxy2->AddSubTaskProxy(*childProxy3);

      CPPUNIT_ASSERT_MESSAGE("Parent proxy should be parent of child 1.",
         childProxy1->GetParentTaskProxy() == parentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy should be parent of child 2.",
         childProxy2->GetParentTaskProxy() == childProxy1.get());
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy should be parent of child 3.",
         childProxy3->GetParentTaskProxy() == childProxy2.get());

      parentProxy->RemoveSubTaskProxy(*childProxy1);
      childProxy1->RemoveSubTaskProxy(*childProxy2);
      childProxy2->RemoveSubTaskProxy(*childProxy3);
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy's parent should be NULL",
         childProxy1->GetParentTaskProxy() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy's should be parent of child 2.",
         childProxy2->GetParentTaskProxy() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Child3 proxy's should be parent of child 3.",
         childProxy3->GetParentTaskProxy() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestGameEventTaskActor()
{
   try
   {
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()),
                                   dtGame::GameManager::ComponentPriority::HIGHEST);
      dtCore::RefPtr<dtDAL::ActorType> actorType =
            mGameManager->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",actorType.valid());

      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy =
            dynamic_cast<dtActors::TaskActorGameEventProxy *>(mGameManager->CreateActor(*actorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());
      mGameManager->AddActor(*eventTaskProxy,false,false);

      //Before we can listen for any game event messages, we have to register the event
      //with the event manager since other components rely on this behavior.
      dtDAL::GameEvent *gameEvent = new dtDAL::GameEvent("TestGameEvent");
      mEventMgr->AddEvent(*gameEvent);

      //Set the game event property on the task.
      CPPUNIT_ASSERT_MESSAGE("Game event task actors should have a GameEvent property.",
                             eventTaskProxy->GetProperty("GameEvent") != NULL);
      eventTaskProxy->GetProperty("GameEvent")->SetStringValue(gameEvent->GetUniqueId().ToString());

      dtDAL::GameEventActorProperty *prop =
            static_cast<dtDAL::GameEventActorProperty*>(eventTaskProxy->GetProperty("GameEvent"));
      dtDAL::GameEvent *toCheck = prop->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Game event in property was not correct.",toCheck == gameEvent);

      //Set the minimum number of times the event should be fired.
      CPPUNIT_ASSERT_MESSAGE("Game event task actor should have a MinOccurances property.",
                             eventTaskProxy->GetProperty("MinOccurances") != NULL);
      dtDAL::IntActorProperty *minProp =
            static_cast<dtDAL::IntActorProperty*>(eventTaskProxy->GetProperty("MinOccurances"));
      minProp->SetValue(5);
      CPPUNIT_ASSERT_MESSAGE("Game event min occurances should have been 5.",minProp->GetValue() == 5);

      //Pass a game event message on through the game manager and make sure
      //the game event task sees it and tracks it.
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
            (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

      eventMsg->SetGameEvent(*gameEvent);
      mGameManager->ProcessMessage(*eventMsg);
      dtCore::System::Instance()->Step();

      //Should be incomplete since the task should have only gotten the event once.
      CPPUNIT_ASSERT_MESSAGE("Task should not yet be complete.",
                             eventTaskProxy->GetProperty("Complete")->GetStringValue() == "false");

      double currSimTime;
      for (int i=0; i<4; i++)
      {
         mGameManager->ProcessMessage(*eventMsg);
         dtCore::System::Instance()->Step();
         currSimTime = mGameManager->GetSimulationTime();
      }

      //Now the task should have been marked complete.
      CPPUNIT_ASSERT_MESSAGE("Task should have been completed.",
                             eventTaskProxy->GetProperty("Complete")->GetStringValue() == "true");

      //Make sure the complete time got marked properly...
      dtDAL::DoubleActorProperty *time = static_cast<dtDAL::DoubleActorProperty*>(eventTaskProxy->GetProperty("CompleteTime"));
      CPPUNIT_ASSERT_MESSAGE("Task complete time stamp was not correct.",time->GetValue() == currSimTime);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestRollupTaskActor()
{
   try
   {
      dtCore::RefPtr<dtDAL::GameEvent> eventList[5];
      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventProxyList[5];
      int i;

      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()),
                                   dtGame::GameManager::ComponentPriority::HIGHEST);

      dtCore::RefPtr<dtDAL::ActorType> gameEventType = mGameManager->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for game event task.",gameEventType.valid());

      dtCore::RefPtr<dtDAL::ActorType> rollupActorType = mGameManager->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());

      //Create our test rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*rollupActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.",rollupTaskProxy.valid());
      mGameManager->AddActor(*rollupTaskProxy,false,false);

      //Create some event tasks and add them as children to the rollup task.
      for (i=0; i<5; i++)
      {
         dtDAL::GameEvent *gameEvent = new dtDAL::GameEvent();
         mEventMgr->AddEvent(*gameEvent);

         dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy =
            dynamic_cast<dtActors::TaskActorGameEventProxy *>(mGameManager->CreateActor(*gameEventType).get());
         CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());

         eventTaskProxy->GetProperty("GameEvent")->SetStringValue(gameEvent->GetUniqueId().ToString());
         eventTaskProxy->GetProperty("Weight")->SetStringValue("0.2f");

         rollupTaskProxy->AddSubTaskProxy(*eventTaskProxy);
         mGameManager->AddActor(*eventTaskProxy,false,false);
         eventList[i] = gameEvent;
         eventProxyList[i] = eventTaskProxy;
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have 5 child tasks.",rollupTaskProxy->GetNumSubTaskProxies() == 5);

      //Now we need to send a message for each of the 5 events.  This will cause each of the event tasks to be
      //completed which should inform the rollup task and have it calculate the correct score each time
      //one of its children has a score change.
      dtDAL::FloatActorProperty *prop =
            static_cast<dtDAL::FloatActorProperty*>(rollupTaskProxy->GetProperty("Score"));
      for (i=0; i<5; i++)
      {
         dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGameManager->ProcessMessage(*eventMsg);
         dtCore::System::Instance()->Step();

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),((float)i+1.0f) * 0.2f,0.001f));
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have been marked complete.",
                             rollupTaskProxy->GetProperty("Complete")->GetStringValue() == "true");

      //Reset the event and rollup tasks..
      dtActors::TaskActor *actor = dynamic_cast<dtActors::TaskActor*>(rollupTaskProxy->GetActor());
      actor->Reset();
      for (i=0; i<5; i++)
      {
         actor = dynamic_cast<dtActors::TaskActor*>(eventProxyList[i]->GetActor());
         actor->Reset();
      }

      //Try it again, only do not fire all the events.  This should cause the rollup task to NOT
      //be complete since all of its subtasks are not complete.
      for (i=0; i<3; i++)
      {
         dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGameManager->ProcessMessage(*eventMsg);
         dtCore::System::Instance()->Step();

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),((float)i+1.0f) * 0.2f,0.001f));
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should not have been marked complete.",
                             rollupTaskProxy->GetProperty("Complete")->GetStringValue() == "false");
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestOrderedTaskActor()
{
   try
   {
      dtCore::RefPtr<dtDAL::GameEvent> eventList[5];
      dtCore::RefPtr<dtDAL::GameEvent> primaryEvent;
      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventProxyList[5];
      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> primaryEventProxy;
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
      int i;

      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()),
                                   dtGame::GameManager::ComponentPriority::HIGHEST);

      dtCore::RefPtr<dtDAL::ActorType> gameEventType = mGameManager->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for game event task.",gameEventType.valid());

      dtCore::RefPtr<dtDAL::ActorType> rollupActorType = mGameManager->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());

      dtCore::RefPtr<dtDAL::ActorType> orderedActorType = mGameManager->FindActorType("dtcore.Tasks","Ordered Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for ordered task.",orderedActorType.valid());

      //Create our test rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*rollupActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.",rollupTaskProxy.valid());
      mGameManager->AddActor(*rollupTaskProxy,false,false);

      //Create our test ordered task...
      dtCore::RefPtr<dtActors::TaskActorProxy> orderedTaskProxy =
            dynamic_cast<dtActors::TaskActorProxy *>(mGameManager->CreateActor(*orderedActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create ordered task actor proxy.",orderedTaskProxy.valid());
      mGameManager->AddActor(*orderedTaskProxy,false,false);

      //Create our test primary event task...
      primaryEvent = new dtDAL::GameEvent("PrimaryEvent");
      mEventMgr->AddEvent(*primaryEvent);

      primaryEventProxy = dynamic_cast<dtActors::TaskActorGameEventProxy *>(mGameManager->CreateActor(*gameEventType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create primary game event task actor proxy.",primaryEventProxy.valid());
      primaryEventProxy->GetProperty("GameEvent")->SetStringValue(primaryEvent->GetUniqueId().ToString());
      mGameManager->AddActor(*primaryEventProxy,false,false);

      //Add an event task which must be completed before the rollup task can be completed.
      orderedTaskProxy->AddSubTaskProxy(*primaryEventProxy);
      orderedTaskProxy->AddSubTaskProxy(*rollupTaskProxy);

      //Create some event tasks and add them as children to the rollup task.
      for (i=0; i<5; i++)
      {
         dtDAL::GameEvent *gameEvent = new dtDAL::GameEvent();
         mEventMgr->AddEvent(*gameEvent);

         dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy =
               dynamic_cast<dtActors::TaskActorGameEventProxy *>(mGameManager->CreateActor(*gameEventType).get());
         CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());

         eventTaskProxy->GetProperty("GameEvent")->SetStringValue(gameEvent->GetUniqueId().ToString());
         eventTaskProxy->GetProperty("Weight")->SetStringValue("0.2f");

         rollupTaskProxy->AddSubTaskProxy(*eventTaskProxy);
         mGameManager->AddActor(*eventTaskProxy,false,false);
         eventList[i] = gameEvent;
         eventProxyList[i] = eventTaskProxy;
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have 5 child tasks.",rollupTaskProxy->GetNumSubTaskProxies() == 5);

      //Now we need to send a message for each of the 5 events.  This will attempt to mark each task
      //complete which should NOT occur until the primary event task has been completed first since that
      //is first in the ordered task's child task list.
      dtDAL::FloatActorProperty *prop =
            static_cast<dtDAL::FloatActorProperty*>(rollupTaskProxy->GetProperty("Score"));
      for (i=0; i<5; i++)
      {
         eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGameManager->ProcessMessage(*eventMsg);
         dtCore::System::Instance()->Step();

         CPPUNIT_ASSERT_MESSAGE("Event Task: " + dtUtil::ToString(i) + " should not have been complete.",
            eventProxyList[i]->GetProperty("Complete")->GetStringValue() == "false");

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),0.0f,0.001f));
      }

      //Now we will complete the primary event which should then allow the rollup task and its children
      //to be evaluated.
      eventMsg = static_cast<dtGame::GameEventMessage*>
            (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

      eventMsg->SetGameEvent(*primaryEvent);
      mGameManager->ProcessMessage(*eventMsg);
      dtCore::System::Instance()->Step();

      CPPUNIT_ASSERT_MESSAGE("Primary Event Task should have been complete.",
                             primaryEventProxy->GetProperty("Complete")->GetStringValue() == "true");

      //Now the rollup task and its subtasks should be "completable".
      for (i=0; i<5; i++)
      {
         eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGameManager->ProcessMessage(*eventMsg);
         dtCore::System::Instance()->Step();

         CPPUNIT_ASSERT_MESSAGE("Event Task: " + dtUtil::ToString(i) + " should have been complete.",
                                eventProxyList[i]->GetProperty("Complete")->GetStringValue() == "true");

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),((float)i+1.0f) * 0.2f,0.001f));
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have been marked complete.",
                              rollupTaskProxy->GetProperty("Complete")->GetStringValue() == "true");
      CPPUNIT_ASSERT_MESSAGE("Ordered task should have been marked complete.",
                             orderedTaskProxy->GetProperty("Complete")->GetStringValue() == "true");
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}



