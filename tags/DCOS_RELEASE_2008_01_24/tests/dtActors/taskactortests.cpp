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
 * Matthew W. Campbell, Curtiss Murphy
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtActors/taskactor.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/taskactorrollup.h>
#include <dtActors/taskactorordered.h>

#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/basemessages.h>

#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/namedparameter.h>

#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

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
      CPPUNIT_TEST(TestGroupPropertySubTasks);
      CPPUNIT_TEST(TestTaskReparentOnAdd);
      CPPUNIT_TEST(TestGameEventTaskActor);
      CPPUNIT_TEST(TestRollupTaskActor);
      CPPUNIT_TEST(TestOrderedTaskActor);
      CPPUNIT_TEST(TestFailedAndComplete);
      CPPUNIT_TEST(TestMutable);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestTaskActorDefaultValues();
      void TestTaskSubTasks();
      void TestGroupPropertySubTasks();
      void TestTaskReparentOnAdd();
      void TestGameEventTaskActor();
      void TestRollupTaskActor();
      void TestOrderedTaskActor();
      void TestFailedAndComplete();
      void TestMutable();

   private:
      void CreateParentChildProxies();
      dtDAL::GameEventManager *mEventMgr;
      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      static const std::string mTestGameActorLibrary;
      static const std::string mTestActorLibrary;
      dtCore::RefPtr<dtActors::TaskActorProxy> mParentProxy;
      dtCore::RefPtr<dtActors::TaskActorProxy> mChildProxy1;
      dtCore::RefPtr<dtActors::TaskActorProxy> mChildProxy2;
      dtCore::RefPtr<dtActors::TaskActorProxy> mChildProxy3;
};

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TaskActorTests);

const std::string TaskActorTests::mTestGameActorLibrary="testGameActorLibrary";
const std::string TaskActorTests::mTestActorLibrary="testActorLibrary";

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::setUp()
{
   try
   {
      mEventMgr = &dtDAL::GameEventManager::GetInstance();
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::GameManager(*scene);
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::tearDown()
{
   dtCore::System::GetInstance().SetPause(false);
   dtCore::System::GetInstance().Stop();
   if(mGameManager.valid())
   {
      mGameManager->DeleteAllActors();
      mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
      mGameManager = NULL;
   }
   mEventMgr->ClearAllEvents();
   mEventMgr = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskActorDefaultValues()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> proxy;
      mGameManager->CreateActor(*taskActorType, proxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",proxy.valid());

      //Make sure the correct properties exist on the proxy.
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a description property.",
         proxy->GetProperty("Description") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a display name property.",
         proxy->GetProperty("DisplayName") != NULL);
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

      dtDAL::StringActorProperty *displayNameProp =
         static_cast<dtDAL::StringActorProperty *>(proxy->GetProperty("DisplayName"));
      CPPUNIT_ASSERT_MESSAGE("Task display name should be empty.",displayNameProp->GetValue().empty());

      dtDAL::FloatActorProperty *valueProp =
         static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("PassingScore"));
      CPPUNIT_ASSERT_MESSAGE("Task passing score should be 1.0.",valueProp->GetValue() == 1.0);

      valueProp = static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("Score"));
      CPPUNIT_ASSERT_MESSAGE("Task score should be 0.0.",valueProp->GetValue() == 0.0f);

      valueProp = static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("Weight"));
      CPPUNIT_ASSERT_MESSAGE("Task weight should be 1.0.",valueProp->GetValue() == 1.0f);

      dtActors::TaskActor *actor = dynamic_cast<dtActors::TaskActor*>(proxy->GetActor());
      CPPUNIT_ASSERT_MESSAGE("Complete should be false.", !actor->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("Failed should be false.", !actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("Initial completed time should be -1.0.", actor->GetCompletedTimeStamp() == -1.0);

      CPPUNIT_ASSERT_MESSAGE("Should be mutable at first.", proxy->IsCurrentlyMutable());
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
      dtCore::RefPtr<const dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.", taskActorType.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> mParentProxy;
      mGameManager->CreateActor(*taskActorType, mParentProxy);
      mGameManager->AddActor(*mParentProxy, false, false);
      
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.", mParentProxy.valid());

      std::vector<dtActors::TaskActorProxy* > children;      

      //Create a bunch of actors and add them as children.
      for (unsigned i=0; i<25; i++)
      {
         dtCore::RefPtr<dtActors::TaskActorProxy> childProxy = NULL;

         mGameManager->CreateActor(*taskActorType, childProxy);
         CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.", childProxy.valid());
         mGameManager->AddActor(*childProxy,false, false);
         
         children.push_back(childProxy.get());

         childProxy->SetName("ChildProxy" + dtUtil::ToString(i));
         mParentProxy->AddSubTask(*childProxy);
      }

      mParentProxy->GetAllSubTasks(children);
      CPPUNIT_ASSERT_MESSAGE("Number of child tasks should have been 25, and the GetAllSubTasks method should clear before filling.", 
         children.size() == 25);

      dtDAL::GroupActorProperty* subTasksGroup = dynamic_cast<dtDAL::GroupActorProperty*>(mParentProxy->GetProperty("SubTasks"));
      CPPUNIT_ASSERT(subTasksGroup != NULL);
      dtCore::RefPtr<dtDAL::NamedGroupParameter> subTasksGroupParam = subTasksGroup->GetValue();
      CPPUNIT_ASSERT(subTasksGroupParam.valid());
   
      CPPUNIT_ASSERT_MESSAGE("Number of child tasks in the group property should be 25.", subTasksGroupParam->GetParameterCount() == 25);
      std::vector<dtDAL::NamedParameter*> subTaskActorIds;
      subTasksGroupParam->GetParameters(subTaskActorIds);

      //Make sure the parent was set correctly and that we can find the task.
      for (unsigned i = 0; i < 25; i++)
      {
         CPPUNIT_ASSERT_MESSAGE("Parent was not set correctly.",children[i]->GetParentTask() == mParentProxy.get());
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            mParentProxy->FindSubTask(children[i]->GetId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            mParentProxy->FindSubTask(children[i]->GetName()) != NULL);
         
         dtDAL::NamedActorParameter* subTaskActorParam = dynamic_cast<dtDAL::NamedActorParameter*>(subTaskActorIds[i]);
         CPPUNIT_ASSERT_MESSAGE("The parameters in the subtasks group prop should all be NamedActorParameters.",
            subTaskActorParam != NULL);
         CPPUNIT_ASSERT_MESSAGE("The SubTasks in the group property should have the same id's and be in the same order as the list on the proxy.",
            children[i]->GetId() == subTaskActorParam->GetValue());
      }

      for (unsigned i = 0; i < 25; i += 2)
      {
         mParentProxy->RemoveSubTask(*children[i]);
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be NULL after removing it.",
            children[i]->GetParentTask() == NULL);
      }

      std::vector<const dtActors::TaskActorProxy* > children2;
      mParentProxy->GetAllSubTasks(children2);
      
      for (unsigned i = 0; i < children2.size(); i++)
      {
         mParentProxy->RemoveSubTask(children2[i]->GetGameActor().GetName());
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be NULL after removing it by name.",
            children2[i]->GetParentTask() == NULL);
      }

      CPPUNIT_ASSERT_MESSAGE("There should be no more child tasks left.",
         mParentProxy->GetSubTaskCount() == 0);
      
      dtCore::RefPtr<dtDAL::NamedGroupParameter> subTasksGroupParam2 = subTasksGroup->GetValue();
      CPPUNIT_ASSERT(subTasksGroupParam2.valid());
      CPPUNIT_ASSERT_MESSAGE("There should be no more child tasks left when looking in the group property.", 
         subTasksGroupParam2->GetParameterCount() == 0);
      
      subTasksGroup->SetValue(*subTasksGroupParam);
      mParentProxy->GetAllSubTasks(children);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Setting the subtasks via the property should populate the list of children.", 
         subTasksGroupParam->GetParameterCount(), (unsigned int) children.size());
      
      for (unsigned i = 0; i < children.size(); i++)
      {
         dtDAL::NamedActorParameter* subTaskActorParam = dynamic_cast<dtDAL::NamedActorParameter*>(subTaskActorIds[i]);
         CPPUNIT_ASSERT_MESSAGE("The SubTasks in the group property should have the same id's and be in the same order as the list on the proxy.",
            children[i]->GetId() == subTaskActorParam->GetValue());
      }

      subTasksGroupParam2 = subTasksGroup->GetValue();
      CPPUNIT_ASSERT_MESSAGE("The old and new subtask group parameters should be equal but they are: \n  Old:\n"+
         subTasksGroupParam->ToString() +
         "\nand\n  New:\n" +
         subTasksGroupParam2->ToString(),
         *subTasksGroupParam == *subTasksGroupParam2);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::CreateParentChildProxies()
{
   dtCore::RefPtr<const dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
   CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

   mGameManager->CreateActor(*taskActorType, mParentProxy);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mParentProxy.valid());

   mGameManager->CreateActor(*taskActorType, mChildProxy1);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mChildProxy1.valid());

   mGameManager->CreateActor(*taskActorType, mChildProxy2);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mChildProxy2.valid());

   mGameManager->CreateActor(*taskActorType, mChildProxy3);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mChildProxy3.valid());
   
   mGameManager->AddActor(*mParentProxy, false, false);
   mGameManager->AddActor(*mChildProxy1, false, false);
   mGameManager->AddActor(*mChildProxy2, false, false);
   mGameManager->AddActor(*mChildProxy3, false, false);
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

      CreateParentChildProxies();

      mParentProxy->AddSubTask(*mChildProxy1);
      mChildProxy1->AddSubTask(*mChildProxy2);
      mChildProxy2->AddSubTask(*mChildProxy3);

      CPPUNIT_ASSERT_MESSAGE("Parent proxy should be parent of child 1.",
         mChildProxy1->GetParentTask() == mParentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy should be parent of child 2.",
         mChildProxy2->GetParentTask() == mChildProxy1.get());
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy should be parent of child 3.",
         mChildProxy3->GetParentTask() == mChildProxy2.get());

      mParentProxy->RemoveSubTask(*mChildProxy1);
      mChildProxy1->RemoveSubTask(*mChildProxy2);
      mChildProxy2->RemoveSubTask(*mChildProxy3);
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy's parent should be NULL",
         mChildProxy1->GetParentTask() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy's should be parent of child 2.",
         mChildProxy2->GetParentTask() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Child3 proxy's should be parent of child 3.",
         mChildProxy3->GetParentTask() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestGroupPropertySubTasks()
{
   try
   {
      CreateParentChildProxies();
      dtDAL::ActorProperty* subTasksProp = mParentProxy->GetProperty("SubTasks");
      CPPUNIT_ASSERT(subTasksProp != NULL);
      CPPUNIT_ASSERT(subTasksProp->GetDataType() == dtDAL::DataType::GROUP);
      
      dtCore::RefPtr<dtDAL::NamedGroupParameter> expectedValue = new dtDAL::NamedGroupParameter("SubTasks"); 
      
      dtCore::RefPtr<dtDAL::NamedActorParameter> temp;
      temp = new dtDAL::NamedActorParameter("0", mChildProxy1->GetId());
      expectedValue->AddParameter(*temp);
      temp = new dtDAL::NamedActorParameter("1", mChildProxy2->GetId());
      expectedValue->AddParameter(*temp);
      temp = new dtDAL::NamedActorParameter("2", mChildProxy3->GetId());
      expectedValue->AddParameter(*temp);
      
      dtDAL::GroupActorProperty* gap = static_cast<dtDAL::GroupActorProperty*>(subTasksProp);
      gap->SetValue(*expectedValue);
      
      dtCore::RefPtr<dtDAL::NamedGroupParameter> actualValue = gap->GetValue();
      std::ostringstream ss;
      ss << "Setting the list of task children and then getting it should yield the same list: \nActual:\n";
      ss << actualValue->ToString();
      ss << "\nExpected:\n";
      ss << expectedValue->ToString();
      CPPUNIT_ASSERT_MESSAGE(ss.str(),
         *actualValue == *expectedValue);

      CPPUNIT_ASSERT_MESSAGE("Setting the parent of a proxy should set its parent pointer (mChildProxy1).", 
         mChildProxy1->GetParentTask() == mParentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Setting the parent of a proxy should set its parent pointer (mChildProxy2).", 
         mChildProxy2->GetParentTask() == mParentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Setting the parent of a proxy should set its parent pointer (mChildProxy3).", 
         mChildProxy3->GetParentTask() == mParentProxy.get());


      ///Now change the value and make sure the removed one's don't have their parent's still set set.
      expectedValue = new dtDAL::NamedGroupParameter("test"); 
      
      temp = new dtDAL::NamedActorParameter("1", mChildProxy1->GetId());
      expectedValue->AddParameter(*temp);
      gap->SetValue(*expectedValue);

      CPPUNIT_ASSERT_MESSAGE("Resetting the parent of a proxy should leave its parent pointer (mChildProxy1).", 
         mChildProxy1->GetParentTask() == mParentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Changing the list of child proxies should set the removed proxy's (mChildProxy2) parent to NULL.", 
         mChildProxy2->GetParentTask() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Changing the list of child proxies should set the removed proxy's (mChildProxy3) parent to NULL.", 
         mChildProxy3->GetParentTask() == NULL);

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
      dtCore::RefPtr<const dtDAL::ActorType> actorType =
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
      eventTaskProxy->GetProperty("GameEvent")->FromString(gameEvent->GetUniqueId().ToString());

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
      mGameManager->SendMessage(*eventMsg);
      dtCore::System::GetInstance().Step();

      //Should be incomplete since the task should have only gotten the event once.
      CPPUNIT_ASSERT_MESSAGE("Task should not yet be complete.",
                             eventTaskProxy->GetProperty("Complete")->ToString() == "false");

      double currSimTime;
      for (int i=0; i<4; i++)
      {
         mGameManager->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();
         currSimTime = mGameManager->GetSimulationTime();
      }

      //Now the task should have been marked complete.
      CPPUNIT_ASSERT_MESSAGE("Task should have been completed.",
                             eventTaskProxy->GetProperty("Complete")->ToString() == "true");

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

      dtCore::RefPtr<const dtDAL::ActorType> gameEventType = mGameManager->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for game event task.",gameEventType.valid());

      dtCore::RefPtr<const dtDAL::ActorType> rollupActorType = mGameManager->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());

      //Create our test rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy;
      mGameManager->CreateActor(*rollupActorType, rollupTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.",rollupTaskProxy.valid());
      mGameManager->AddActor(*rollupTaskProxy,false,false);

      //Create some event tasks and add them as children to the rollup task.
      for (i=0; i<5; i++)
      {
         dtDAL::GameEvent *gameEvent = new dtDAL::GameEvent();
         mEventMgr->AddEvent(*gameEvent);

         dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy;
         mGameManager->CreateActor(*gameEventType, eventTaskProxy);
         CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());

         eventTaskProxy->GetProperty("GameEvent")->FromString(gameEvent->GetUniqueId().ToString());
         eventTaskProxy->GetProperty("Weight")->FromString("0.2f");

         rollupTaskProxy->AddSubTask(*eventTaskProxy);
         mGameManager->AddActor(*eventTaskProxy,false,false);
         eventList[i] = gameEvent;
         eventProxyList[i] = eventTaskProxy;
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have 5 child tasks.",rollupTaskProxy->GetSubTaskCount() == 5);

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
         mGameManager->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),((float)i+1.0f) * 0.2f,0.001f));
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have been marked complete.",
                             rollupTaskProxy->GetProperty("Complete")->ToString() == "true");


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
         mGameManager->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),((float)i+1.0f) * 0.2f,0.001f));
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should not have been marked complete.",
                             rollupTaskProxy->GetProperty("Complete")->ToString() == "false");
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

      dtCore::RefPtr<const dtDAL::ActorType> gameEventType = mGameManager->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for game event task.",gameEventType.valid());

      dtCore::RefPtr<const dtDAL::ActorType> rollupActorType = mGameManager->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());

      dtCore::RefPtr<const dtDAL::ActorType> orderedActorType = mGameManager->FindActorType("dtcore.Tasks","Ordered Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for ordered task.",orderedActorType.valid());

      //Create our test rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy; 
      mGameManager->CreateActor(*rollupActorType, rollupTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.", rollupTaskProxy.valid());
      mGameManager->AddActor(*rollupTaskProxy,false,false);

      //Create our test ordered task...
      dtCore::RefPtr<dtActors::TaskActorProxy> orderedTaskProxy;
      mGameManager->CreateActor(*orderedActorType, orderedTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create ordered task actor proxy.", orderedTaskProxy.valid());
      mGameManager->AddActor(*orderedTaskProxy,false,false);

      //Create our test primary event task...
      primaryEvent = new dtDAL::GameEvent("PrimaryEvent");
      mEventMgr->AddEvent(*primaryEvent);

      mGameManager->CreateActor(*gameEventType, primaryEventProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create primary game event task actor proxy.",primaryEventProxy.valid());
      primaryEventProxy->GetProperty("GameEvent")->FromString(primaryEvent->GetUniqueId().ToString());
      mGameManager->AddActor(*primaryEventProxy,false,false);

      //Add an event task which must be completed before the rollup task can be completed.
      orderedTaskProxy->AddSubTask(*primaryEventProxy);
      orderedTaskProxy->AddSubTask(*rollupTaskProxy);

      //Create some event tasks and add them as children to the rollup task.
      for (i=0; i<5; i++)
      {
         dtDAL::GameEvent *gameEvent = new dtDAL::GameEvent();
         mEventMgr->AddEvent(*gameEvent);

         dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy;
         mGameManager->CreateActor(*gameEventType, eventTaskProxy);
         CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());

         eventTaskProxy->GetProperty("GameEvent")->FromString(gameEvent->GetUniqueId().ToString());
         eventTaskProxy->GetProperty("Weight")->FromString("0.2f");

         rollupTaskProxy->AddSubTask(*eventTaskProxy);
         mGameManager->AddActor(*eventTaskProxy,false,false);
         eventList[i] = gameEvent;
         eventProxyList[i] = eventTaskProxy;
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have 5 child tasks.",rollupTaskProxy->GetSubTaskCount() == 5);

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
         mGameManager->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_MESSAGE("Event Task: " + dtUtil::ToString(i) + " should not have been complete.",
            eventProxyList[i]->GetProperty("Complete")->ToString() == "false");

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),0.0f,0.001f));
      }

      //Now we will complete the primary event which should then allow the rollup task and its children
      //to be evaluated.
      eventMsg = static_cast<dtGame::GameEventMessage*>
            (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

      eventMsg->SetGameEvent(*primaryEvent);
      mGameManager->SendMessage(*eventMsg);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("Primary Event Task should have been complete.",
                             primaryEventProxy->GetProperty("Complete")->ToString() == "true");

      //Now the rollup task and its subtasks should be "completable".
      for (i=0; i<5; i++)
      {
         eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGameManager->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_MESSAGE("Event Task: " + dtUtil::ToString(i) + " should have been complete.",
                                eventProxyList[i]->GetProperty("Complete")->ToString() == "true");

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),((float)i+1.0f) * 0.2f,0.001f));
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have been marked complete.",
                              rollupTaskProxy->GetProperty("Complete")->ToString() == "true");
      CPPUNIT_ASSERT_MESSAGE("Ordered task should have been marked complete.",
                             orderedTaskProxy->GetProperty("Complete")->ToString() == "true");
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestFailedAndComplete()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<const dtDAL::ActorType> rollupActorType = 
         mGameManager->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());


      dtCore::RefPtr<dtActors::TaskActorProxy> proxy;
      mGameManager->CreateActor(*taskActorType, proxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",proxy.valid());

      dtActors::TaskActor *actor = dynamic_cast<dtActors::TaskActor*>(proxy->GetActor());

      double completeTime = actor->GetCompletedTimeStamp();

      /////  TEST SETTING TO FAIL

      // Basic setting to FAIL .  
      actor->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("Should be failed 1.", actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("TimeStamp should have changed when marked to fail 1.", 
         actor->GetCompletedTimeStamp() != completeTime);
      CPPUNIT_ASSERT_MESSAGE("Should not be complete 1.", !actor->IsComplete());

      // Try to mark complete while already failed
      actor->SetCompletedTimeStamp(992.589); // set to something testable
      actor->SetComplete(true);
      CPPUNIT_ASSERT_MESSAGE("Should still be failed 2.", actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("Should not be complete 2.", !actor->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("Time stamp should not have changed 2.", actor->GetCompletedTimeStamp() == 992.589);

      // Try to set to failed again, should not change timestamp.
      actor->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("Should be failed 3.", actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("Should not be complete 3.", !actor->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("Time stamp should not have changed 3.", actor->GetCompletedTimeStamp() == 992.589);
      CPPUNIT_ASSERT_MESSAGE("A Score Change Request on a failed task should be denied", 
         !proxy->RequestScoreChange(*proxy, *proxy));

      // Test Mutable
      CPPUNIT_ASSERT_MESSAGE("We are failed, so should not be mutable", !proxy->IsCurrentlyMutable());


      /////  TEST SETTING TO COMPLETE

      // RESET and test setting to Complete
      actor->Reset(); 
      completeTime = actor->GetCompletedTimeStamp();
      actor->SetComplete(true);
      CPPUNIT_ASSERT_MESSAGE("Should be complete 4.", actor->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("TimeStamp should have changed when marked to complete 4.", 
         actor->GetCompletedTimeStamp() != completeTime);
      CPPUNIT_ASSERT_MESSAGE("Should not be failed 4.", !actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("A Score Change Request on a complete task should be allowed", 
         proxy->RequestScoreChange(*proxy, *proxy));

      // Try to mark failed while already complete
      actor->SetCompletedTimeStamp(8484.233); // set to something testable
      actor->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("Should still be complete 5.", actor->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("Should not be failed 5.", !actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("Time stamp should not have changed 5.", actor->GetCompletedTimeStamp() == 8484.233);


      // Try to set to complete again, should not change timestamp.
      actor->SetComplete(true);
      CPPUNIT_ASSERT_MESSAGE("Should still be complete 6.", actor->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("Should not be failed 6.", !actor->IsFailed());
      CPPUNIT_ASSERT_MESSAGE("Time stamp should not have changed 6.", actor->GetCompletedTimeStamp() == 8484.233);

      // Test Mutable
      CPPUNIT_ASSERT_MESSAGE("We are complete, so should not be mutable", !proxy->IsCurrentlyMutable());

      /// TEST the PARENT - CHILD behavior.

      //Create our parent rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy; 
      mGameManager->CreateActor(*rollupActorType, rollupTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.", rollupTaskProxy.valid());
      dtActors::TaskActor *parentActor;
      rollupTaskProxy->GetActor(parentActor);
      // add the child
      rollupTaskProxy->AddSubTask(*proxy);

      // test a complete parent
      CPPUNIT_ASSERT_MESSAGE("RequestScoreChange should be ok if parent is not failed.", 
         proxy->RequestScoreChange(*proxy,*proxy));

      // Test a failed parent
      parentActor->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("RequestScoreChange should fail if parent is failed.", 
         !proxy->RequestScoreChange(*proxy,*proxy));

      // Also, since we have a rollup task (below tests use ordered task), go ahead and test SetFailed().
      CPPUNIT_ASSERT_MESSAGE("Child Task should NOT be mutable when rollup parent is failed.", !proxy->IsCurrentlyMutable());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestMutable()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> taskActorType =
         mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<const dtDAL::ActorType> orderedActorType = mGameManager->FindActorType("dtcore.Tasks","Ordered Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for ordered task.",orderedActorType.valid());

      // Create our test child task
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy1;
      mGameManager->CreateActor(*taskActorType, childProxy1);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy1.valid());
      mGameManager->AddActor(*childProxy1,false,false);

      // Child 2 task
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy2;
      mGameManager->CreateActor(*taskActorType, childProxy2);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy2.valid());
      mGameManager->AddActor(*childProxy2,false,false);

      //Create our test ordered task...
      dtCore::RefPtr<dtActors::TaskActorProxy> orderedTaskProxy;
      mGameManager->CreateActor(*orderedActorType, orderedTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create ordered task actor proxy.", orderedTaskProxy.valid());
      mGameManager->AddActor(*orderedTaskProxy,false,false);

      dtActors::TaskActor *childActor1 = dynamic_cast<dtActors::TaskActor*>(childProxy1->GetActor());
      dtActors::TaskActor *childActor2 = dynamic_cast<dtActors::TaskActor*>(childProxy2->GetActor());

      CPPUNIT_ASSERT_MESSAGE("Should be mutable on creation.", childProxy1->IsCurrentlyMutable());

      // Set to Failed, should not be mutable
      childActor1->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("Failed should make it not mutable.", !childProxy1->IsCurrentlyMutable());

      childActor1->Reset();
      CPPUNIT_ASSERT_MESSAGE("Double checking we're mutable again.", childProxy1->IsCurrentlyMutable());

      // Test Complete making us not mutable
      childActor1->SetComplete(true);
      CPPUNIT_ASSERT_MESSAGE("Complete should make it not mutable.", !childProxy1->IsCurrentlyMutable());

      childActor1->Reset();

      // Test mutable working on an ORDERED Task. 
      // Add child 1 and child 2, then try to manipulate 2 before setting 1.
      orderedTaskProxy->AddSubTask(*childProxy1);
      orderedTaskProxy->AddSubTask(*childProxy2);

      // 1 should be mutable
      CPPUNIT_ASSERT_MESSAGE("1st child task should be mutable.", childProxy1->IsCurrentlyMutable());

      // 2 should not be mutable.
      CPPUNIT_ASSERT_MESSAGE("2nd child task should make it not mutable until 1 is complete.", !childProxy2->IsCurrentlyMutable());

      // Make 1 complete and check 2 again. It should now be mutable
      childActor1->SetComplete(true);
      CPPUNIT_ASSERT_MESSAGE("2nd child task should now be mutable since 1 is complete.", childProxy2->IsCurrentlyMutable());
      CPPUNIT_ASSERT_MESSAGE("1st child task should NOT be mutable since it is already complete.", !childProxy1->IsCurrentlyMutable());

      // Fail #2 and it should not be mutable anymore
      childActor2->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("2nd child task should not be mutable if it's failed.", !childProxy2->IsCurrentlyMutable());
      childActor2->SetFailed(false);

      // Fail the parent and child 2 should not be mutable anymore
      dtActors::TaskActor *orderedTaskActor;
      orderedTaskProxy->GetActor(orderedTaskActor);
      orderedTaskActor->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("2nd child task should NOT be mutable when ordered parent is failed.", !childProxy2->IsCurrentlyMutable());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


