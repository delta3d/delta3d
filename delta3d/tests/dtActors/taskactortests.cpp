/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
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
 * Matthew W. Campbell, Curtiss Murphy
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtABC/application.h>

#include <dtActors/taskactor.h>
#include <dtActors/taskactorgameevent.h>
#include <dtActors/taskactorrollup.h>
#include <dtActors/taskactorordered.h>
#include <dtActors/engineactorregistry.h>

#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <dtCore/actoridactorproperty.h>
#include <dtCore/arrayactorproperty.h>
#include <dtCore/doubleactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/gameeventmanager.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/actorfactory.h>
#include <dtCore/namedarrayparameter.h>
#include <dtCore/stringactorproperty.h>

#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>

#include <vector>
#include "../dtGame/basegmtests.h"
#include <dtGame/testcomponent.h>

/**
 * This test suite tests the base task actor proxy as well as the different
 * task subclasses.
 */
class TaskActorTests : public dtGame::BaseGMTestFixture
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
      CPPUNIT_TEST(TestNestedMutable);
   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp() override;
      void tearDown() override;

      // Helper Methods
      int GetEventCount(const dtCore::GameEvent& gameEvent);
      bool SetNotifyEventsOnTask(dtActors::TaskActorProxy& taskProxy);

      // Test Methods
      void TestTaskActorDefaultValues();
      void TestTaskSubTasks();
      void TestGroupPropertySubTasks();
      void TestTaskReparentOnAdd();
      void TestGameEventTaskActor();
      void TestRollupTaskActor();
      void TestOrderedTaskActor();
      void TestFailedAndComplete();
      void TestMutable();
      void TestNestedMutable();

   private:
      void CreateParentChildProxies();
      dtCore::GameEventManager* mEventMgr;
      dtCore::RefPtr<dtActors::TaskActorProxy> mParentProxy;
      dtCore::RefPtr<dtActors::TaskActorProxy> mChildProxy1;
      dtCore::RefPtr<dtActors::TaskActorProxy> mChildProxy2;
      dtCore::RefPtr<dtActors::TaskActorProxy> mChildProxy3;
      dtCore::RefPtr<dtCore::GameEvent> mNotifyCompletedEvent;
      dtCore::RefPtr<dtCore::GameEvent> mNotifyFailedEvent;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TaskActorTests);
///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::setUp()
{
   dtGame::BaseGMTestFixture::setUp();
   try
   {
      // Setup the managers.
      mEventMgr = &dtCore::GameEventManager::GetInstance();
      // Create some test events.
      mNotifyCompletedEvent = new dtCore::GameEvent("NotifyCompletedEvent");
      mEventMgr->AddEvent(*mNotifyCompletedEvent);
      mNotifyFailedEvent = new dtCore::GameEvent("NotifyFailedEvent");
      mEventMgr->AddEvent(*mNotifyFailedEvent);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::tearDown()
{
   // Clear all events
   mNotifyCompletedEvent = nullptr;
   mNotifyFailedEvent = nullptr;
   mEventMgr->ClearAllEvents();
   mEventMgr = nullptr;
   dtGame::BaseGMTestFixture::tearDown();
}

///////////////////////////////////////////////////////////////////////////////
int TaskActorTests::GetEventCount(const dtCore::GameEvent& gameEvent)
{
   int total = 0;

   typedef std::vector<dtCore::RefPtr<const dtGame::Message> > MessageList;
   const MessageList& messageList = mTestComp->GetReceivedProcessMessages();

   const dtCore::GameEvent* curEvent = nullptr;
   MessageList::const_iterator curMessage = messageList.begin();
   MessageList::const_iterator endMessageList = messageList.end();
   for (; curMessage != endMessageList; ++curMessage)
   {
      if ((*curMessage)->GetMessageType() == dtGame::MessageType::INFO_GAME_EVENT)
      {
         curEvent = static_cast<const dtGame::GameEventMessage&>(*(*curMessage)).GetGameEvent();
         if (curEvent->GetName() == gameEvent.GetName())
         {
            ++total;
         }
      }
   }

   return total;
}

///////////////////////////////////////////////////////////////////////////////
bool TaskActorTests::SetNotifyEventsOnTask(dtActors::TaskActorProxy& taskProxy)
{
   dtActors::TaskActor* task = nullptr;
   taskProxy.GetDrawable(task);

   task->SetNotifyCompletedEvent(mNotifyCompletedEvent.get());
   task->SetNotifyFailedEvent(mNotifyFailedEvent.get());

   return task->GetNotifyCompletedEvent() == mNotifyCompletedEvent.get()
      && task->GetNotifyFailedEvent() == mNotifyFailedEvent.get();
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskActorDefaultValues()
{
   try
   {
      dtCore::RefPtr<const dtCore::ActorType> taskActorType =
         mGM->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> proxy;
      mGM->CreateActor(*taskActorType, proxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",proxy.valid());
      dtActors::TaskActor* task = nullptr;
      proxy->GetDrawable(task);

      // Make sure the correct properties exist on the proxy.
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a description property.",
         proxy->GetProperty("Description") != nullptr);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a display name property.",
         proxy->GetProperty("DisplayName") != nullptr);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a passing score property.",
         proxy->GetProperty("PassingScore") != nullptr);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a score property.",
         proxy->GetProperty("Score") != nullptr);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a weight property.",
         proxy->GetProperty("Weight") != nullptr);

      // Check the default values of a newly created base task actor.
      dtCore::StringActorProperty* descProp =
         static_cast<dtCore::StringActorProperty*>(proxy->GetProperty("Description"));
      CPPUNIT_ASSERT_MESSAGE("Task description should be empty.",descProp->GetValue().empty());

      dtCore::StringActorProperty* displayNameProp =
         static_cast<dtCore::StringActorProperty*>(proxy->GetProperty("DisplayName"));
      CPPUNIT_ASSERT_MESSAGE("Task display name should be empty.",displayNameProp->GetValue().empty());

      dtCore::FloatActorProperty* valueProp =
         static_cast<dtCore::FloatActorProperty*>(proxy->GetProperty("PassingScore"));
      CPPUNIT_ASSERT_MESSAGE("Task passing score should be 1.0.",valueProp->GetValue() == 1.0);

      valueProp = static_cast<dtCore::FloatActorProperty*>(proxy->GetProperty("Score"));
      CPPUNIT_ASSERT_MESSAGE("Task score should be 0.0.",valueProp->GetValue() == 0.0f);

      valueProp = static_cast<dtCore::FloatActorProperty*>(proxy->GetProperty("Weight"));
      CPPUNIT_ASSERT_MESSAGE("Task weight should be 1.0.",valueProp->GetValue() == 1.0f);

      // Test Notify Completed Event Property
      dtCore::GameEventActorProperty* eventProp = static_cast<dtCore::GameEventActorProperty*>
         (proxy->GetProperty(dtActors::TaskActorProxy::PROPERTY_EVENT_NOTIFY_COMPLETED));
      CPPUNIT_ASSERT_MESSAGE("Task Notify Completed Event should be nullptr",
         eventProp->GetValue() == nullptr);
      task->SetNotifyCompletedEvent(mNotifyCompletedEvent.get());
      CPPUNIT_ASSERT(eventProp->GetValue() == mNotifyCompletedEvent.get());

      // Test Notify Failed Event Property
      eventProp = static_cast<dtCore::GameEventActorProperty*>
         (proxy->GetProperty(dtActors::TaskActorProxy::PROPERTY_EVENT_NOTIFY_FAILED));
      CPPUNIT_ASSERT_MESSAGE("Task Notify Failed Event should be nullptr",
         eventProp->GetValue() == nullptr);
      task->SetNotifyFailedEvent(mNotifyFailedEvent.get());
      CPPUNIT_ASSERT(eventProp->GetValue() == mNotifyFailedEvent.get());

      // --- Ensure the task maintains two different events.
      CPPUNIT_ASSERT(task->GetNotifyCompletedEvent() == mNotifyCompletedEvent.get());
      CPPUNIT_ASSERT(task->GetNotifyFailedEvent() == mNotifyFailedEvent.get());

      dtActors::TaskActor* actor = dynamic_cast<dtActors::TaskActor*>(proxy->GetDrawable());
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
      dtCore::RefPtr<const dtCore::ActorType> taskActorType =
         mGM->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.", taskActorType.valid());

      dtCore::RefPtr<dtActors::TaskActorProxy> mParentProxy;
      mGM->CreateActor(*taskActorType, mParentProxy);
      mGM->AddActor(*mParentProxy, false, false);

      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.", mParentProxy.valid());

      std::vector<dtActors::TaskActorProxy* > children;

      // Create a bunch of actors and add them as children.
      for (unsigned i = 0; i < 25; ++i)
      {
         dtCore::RefPtr<dtActors::TaskActorProxy> childProxy = nullptr;

         mGM->CreateActor(*taskActorType, childProxy);
         CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.", childProxy.valid());
         mGM->AddActor(*childProxy,false, false);

         children.push_back(childProxy.get());

         childProxy->SetName("ChildProxy" + dtUtil::ToString(i));
         mParentProxy->AddSubTask(*childProxy);
      }

      mParentProxy->GetAllSubTasks(children);
      CPPUNIT_ASSERT_MESSAGE("Number of child tasks should have been 25, and the GetAllSubTasks method should clear before filling.",
         children.size() == 25);

      dtCore::ArrayActorProperty<dtCore::UniqueId>* subTaskArray = dynamic_cast<dtCore::ArrayActorProperty<dtCore::UniqueId>*>(mParentProxy->GetProperty("SubTaskList"));
      CPPUNIT_ASSERT(subTaskArray != nullptr);

      std::vector<dtCore::UniqueId> subTasks = subTaskArray->GetValue();

      dtCore::RefPtr<dtCore::NamedArrayParameter> subTaskArrayParam = new dtCore::NamedArrayParameter("TaskParam");
      subTaskArrayParam->SetFromProperty(*subTaskArray);
      CPPUNIT_ASSERT(subTaskArrayParam.valid());
      subTaskArrayParam->ApplyValueToProperty(*subTaskArray);

      CPPUNIT_ASSERT_MESSAGE("Number of child tasks in the group property should be 25.", subTaskArray->GetArraySize() == 25);

      dtCore::ActorIDActorProperty* taskProp = dynamic_cast<dtCore::ActorIDActorProperty*>(subTaskArray->GetArrayProperty());
      CPPUNIT_ASSERT_MESSAGE("The properties in the subtasks array prop should all be of type ActorIDActorProperty.",
         taskProp != nullptr);

      // Make sure the parent was set correctly and that we can find the task.
      for (unsigned i = 0; i < 25; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("Parent was not set correctly.",children[i]->GetParentTask() == mParentProxy.get());
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            mParentProxy->FindSubTask(children[i]->GetId()) != nullptr);
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by name.",
            mParentProxy->FindSubTask(children[i]->GetName()) != nullptr);

         subTaskArray->SetIndex(i);
         CPPUNIT_ASSERT_MESSAGE("The SubTasks in the group property should have the same id's and be in the same order as the list on the proxy.",
            children[i]->GetId() == taskProp->GetValue());
      }

      for (unsigned i = 0; i < 25; i += 2)
      {
         mParentProxy->RemoveSubTask(*children[i]);
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be nullptr after removing it.",
            children[i]->GetParentTask() == nullptr);
      }

      std::vector<const dtActors::TaskActorProxy* > children2;
      mParentProxy->GetAllSubTasks(children2);

      for (unsigned i = 0; i < children2.size(); ++i)
      {
         mParentProxy->RemoveSubTask(children2[i]->GetName());
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be nullptr after removing it by name.",
            children2[i]->GetParentTask() == nullptr);
      }

      CPPUNIT_ASSERT_MESSAGE("There should be no more child tasks left.",
         mParentProxy->GetSubTaskCount() == 0);

      std::vector<dtCore::UniqueId> subTasks2 = subTaskArray->GetValue();
      CPPUNIT_ASSERT_MESSAGE("There should be no more child tasks left when looking in the group property.",
         subTasks2.size() == 0);

      subTaskArray->SetValue(subTasks);
      mParentProxy->GetAllSubTasks(children);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Setting the subtasks via the property should populate the list of children.",
         subTasks.size(), children.size());

      for (unsigned i = 0; i < children.size(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("The SubTasks in the group property should have the same id's and be in the same order as the list on the proxy.",
            children[i]->GetId() == subTasks[i]);
      }

      subTasks2 = subTaskArray->GetValue();
      dtCore::RefPtr<dtCore::NamedArrayParameter> subTaskArrayParam2 = new dtCore::NamedArrayParameter("TaskParam2");
      subTaskArrayParam2->SetFromProperty(*subTaskArray);
      CPPUNIT_ASSERT(subTaskArrayParam2.valid());

      CPPUNIT_ASSERT_MESSAGE("The old and new subtask group parameters should be equal but they are: \n  Old:\n"+
         subTaskArrayParam->ToString() +
         "\nand\n  New:\n" +
         subTaskArrayParam2->ToString(),
         *subTaskArrayParam == *subTaskArrayParam2);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::CreateParentChildProxies()
{
   dtCore::RefPtr<const dtCore::ActorType> taskActorType =
         mGM->FindActorType("dtcore.Tasks","Task Actor");
   CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

   mGM->CreateActor(*taskActorType, mParentProxy);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mParentProxy.valid());

   mGM->CreateActor(*taskActorType, mChildProxy1);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mChildProxy1.valid());

   mGM->CreateActor(*taskActorType, mChildProxy2);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mChildProxy2.valid());

   mGM->CreateActor(*taskActorType, mChildProxy3);
   CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",mChildProxy3.valid());

   mGM->AddActor(*mParentProxy, false, false);
   mGM->AddActor(*mChildProxy1, false, false);
   mGM->AddActor(*mChildProxy2, false, false);
   mGM->AddActor(*mChildProxy3, false, false);
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskReparentOnAdd()
{
   try
   {
      // This tests a nested hierarchy of tasks.
      // parent
      //    --> Child1
      //           --> Child2
      //                  --> Child3

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
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy's parent should be nullptr",
         mChildProxy1->GetParentTask() == nullptr);
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy's should be parent of child 2.",
         mChildProxy2->GetParentTask() == nullptr);
      CPPUNIT_ASSERT_MESSAGE("Child3 proxy's should be parent of child 3.",
         mChildProxy3->GetParentTask() == nullptr);
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
      dtCore::ActorProperty* subTasksProp = mParentProxy->GetProperty("SubTaskList");
      CPPUNIT_ASSERT(subTasksProp != nullptr);
      CPPUNIT_ASSERT(subTasksProp->GetDataType() == dtCore::DataType::ARRAY);

      std::vector<dtCore::UniqueId> subTasks;
      subTasks.push_back(mChildProxy1->GetId());
      subTasks.push_back(mChildProxy2->GetId());
      subTasks.push_back(mChildProxy3->GetId());

      dtCore::ArrayActorProperty<dtCore::UniqueId>* ap = static_cast<dtCore::ArrayActorProperty<dtCore::UniqueId>*>(subTasksProp);
      ap->SetValue(subTasks);

      dtCore::RefPtr<dtCore::NamedArrayParameter> expectedValue = new dtCore::NamedArrayParameter("SubTaskList");
      expectedValue->SetFromProperty(*ap);

      dtCore::RefPtr<dtCore::NamedArrayParameter> actualValue = new dtCore::NamedArrayParameter("SubTaskList2");
      actualValue = expectedValue.get();

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


      /// Now change the value and make sure the removed one's don't have their parent's still set set.
      subTasks.clear();
      subTasks.push_back(mChildProxy1->GetId());
      ap->SetValue(subTasks);

      CPPUNIT_ASSERT_MESSAGE("Resetting the parent of a proxy should leave its parent pointer (mChildProxy1).",
         mChildProxy1->GetParentTask() == mParentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Changing the list of child proxies should set the removed proxy's (mChildProxy2) parent to nullptr.",
         mChildProxy2->GetParentTask() == nullptr);
      CPPUNIT_ASSERT_MESSAGE("Changing the list of child proxies should set the removed proxy's (mChildProxy3) parent to nullptr.",
         mChildProxy3->GetParentTask() == nullptr);

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
      dtCore::RefPtr<const dtCore::ActorType> actorType =
            mGM->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",actorType.valid());

      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy =
            dynamic_cast<dtActors::TaskActorGameEventProxy*>(mGM->CreateActor(*actorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());
      mGM->AddActor(*eventTaskProxy,false,false);

      // Before we can listen for any game event messages, we have to register the event
      // with the event manager since other components rely on this behavior.
      dtCore::GameEvent* gameEvent = new dtCore::GameEvent("TestGameEvent");
      mEventMgr->AddEvent(*gameEvent);
      dtCore::GameEvent* failEvent = new dtCore::GameEvent("TestFailEvent");
      mEventMgr->AddEvent(*failEvent);

      // Set the game event property on the task.
      dtCore::GameEventActorProperty* prop = static_cast<dtCore::GameEventActorProperty*>
         (eventTaskProxy->GetProperty(dtActors::TaskActorGameEventProxy::PROPERTY_EVENT_COMPLETE.Get()));
      CPPUNIT_ASSERT_MESSAGE("Game event task actors should have a GameEvent property.",
         prop != nullptr);

      prop->FromString(gameEvent->GetUniqueId().ToString());
      dtCore::GameEvent* resultEvent = prop->GetValue();
      CPPUNIT_ASSERT(resultEvent != nullptr);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Game event in property was not correct.", resultEvent->GetName(), gameEvent->GetName());

      // Set the fail game event property on the task.
      prop = static_cast<dtCore::GameEventActorProperty*>
         (eventTaskProxy->GetProperty(dtActors::TaskActorGameEventProxy::PROPERTY_EVENT_FAIL.Get()));
      CPPUNIT_ASSERT_MESSAGE("Game event task actors should have a FailGameEvent property.",
         prop != nullptr);

      prop->FromString(failEvent->GetUniqueId().ToString());
      resultEvent = prop->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Game event in property was not correct.",resultEvent == failEvent);

      // Set the minimum number of times the event should be fired.
      dtCore::IntActorProperty* minProp = static_cast<dtCore::IntActorProperty*>
         (eventTaskProxy->GetProperty(dtActors::TaskActorGameEventProxy::PROPERTY_MIN_OCCURANCES.Get()));
      CPPUNIT_ASSERT_MESSAGE("Game event task actor should have a MinOccurances property.",
         minProp != nullptr);

      minProp->SetValue(5);
      CPPUNIT_ASSERT_MESSAGE("Game event min occurances should have been 5.",minProp->GetValue() == 5);

      // Pass a game event message on through the game manager and make sure
      // the game event task sees it and tracks it.
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
            (mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

      eventMsg->SetGameEvent(*gameEvent);
      mGM->SendMessage(*eventMsg);
      dtCore::System::GetInstance().Step();

      // Should be incomplete since the task should have only gotten the event once.
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Task should not yet be complete.",
                             eventTaskProxy->GetProperty("Complete")->ToString(), std::string("false"));

      double currSimTime;
      for (int i = 0; i < 4; ++i)
      {
         mGM->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();
         currSimTime = mGM->GetSimulationTime();
      }

      // Now the task should have been marked complete.
      CPPUNIT_ASSERT_MESSAGE("Task should have been completed.",
                             eventTaskProxy->GetProperty("Complete")->ToString() == "true");

      // Make sure the complete time got marked properly...
      dtCore::DoubleActorProperty* time = static_cast<dtCore::DoubleActorProperty*>(eventTaskProxy->GetProperty("CompleteTime"));
      CPPUNIT_ASSERT_MESSAGE("Task complete time stamp was not correct.",time->GetValue() == currSimTime);

      // Test failing by a fail event.
      dtActors::TaskActorGameEvent* task = nullptr;
      eventTaskProxy->GetDrawable(task);
      task->SetCompletedTimeStamp(-1);
      task->SetComplete(false);
      CPPUNIT_ASSERT(!task->IsComplete());
      CPPUNIT_ASSERT(!task->IsFailed());

      // --- Send the fail event.
      eventMsg->SetGameEvent(*failEvent);
      mGM->SendMessage(*eventMsg);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(!task->IsComplete());
      CPPUNIT_ASSERT_MESSAGE("Game Event Task should fail when receiving a failable event.",
         task->IsFailed());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestRollupTaskActor()
{
   try
   {
      dtCore::RefPtr<dtCore::GameEvent> eventList[5];
      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventProxyList[5];
      int i;

      dtCore::RefPtr<const dtCore::ActorType> gameEventType = mGM->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for game event task.",gameEventType.valid());

      dtCore::RefPtr<const dtCore::ActorType> rollupActorType = mGM->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());

      // Create our test rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy;
      mGM->CreateActor(*rollupActorType, rollupTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.",rollupTaskProxy.valid());
      mGM->AddActor(*rollupTaskProxy,false,false);

      // Create some event tasks and add them as children to the rollup task.
      for (i = 0; i < 5; ++i)
      {
         dtCore::GameEvent* gameEvent = new dtCore::GameEvent();
         mEventMgr->AddEvent(*gameEvent);

         dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy;
         mGM->CreateActor(*gameEventType, eventTaskProxy);
         CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());

         eventTaskProxy->GetProperty("GameEvent")->FromString(gameEvent->GetUniqueId().ToString());
         eventTaskProxy->GetProperty("Weight")->FromString("0.2");

         CPPUNIT_ASSERT_EQUAL(std::string("0.2"), eventTaskProxy->GetProperty("Weight")->ToString());

         rollupTaskProxy->AddSubTask(*eventTaskProxy);
         mGM->AddActor(*eventTaskProxy,false,false);
         eventList[i] = gameEvent;
         eventProxyList[i] = eventTaskProxy;
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have 5 child tasks.",rollupTaskProxy->GetSubTaskCount() == 5);

      // Now we need to send a message for each of the 5 events.  This will cause each of the event tasks to be
      // completed which should inform the rollup task and have it calculate the correct score each time
      // one of its children has a score change.
      dtCore::FloatActorProperty* prop =
            static_cast<dtCore::FloatActorProperty*>(rollupTaskProxy->GetProperty("Score"));
      for (i = 0; i < 5; ++i)
      {
         dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGM->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),prop->GetValue(),(float(i)+1.0f) * 0.2f,0.001f);
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have been marked complete.",
                             rollupTaskProxy->GetProperty("Complete")->ToString() == "true");


      // Reset the event and rollup tasks..
      dtActors::TaskActor* actor = dynamic_cast<dtActors::TaskActor*>(rollupTaskProxy->GetDrawable());
      actor->Reset();
      for (i = 0; i < 5; ++i)
      {
         actor = dynamic_cast<dtActors::TaskActor*>(eventProxyList[i]->GetDrawable());
         actor->Reset();
      }

      // Try it again, only do not fire all the events.  This should cause the rollup task to NOT
      // be complete since all of its subtasks are not complete.
      for (i = 0; i < 3; ++i)
      {
         dtCore::RefPtr<dtGame::GameEventMessage> eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGM->SendMessage(*eventMsg);
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
      dtCore::RefPtr<dtCore::GameEvent> eventList[5];
      dtCore::RefPtr<dtCore::GameEvent> primaryEvent;
      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventProxyList[5];
      dtCore::RefPtr<dtActors::TaskActorGameEventProxy> primaryEventProxy;
      dtCore::RefPtr<dtGame::GameEventMessage> eventMsg;
      int i;

      dtCore::RefPtr<const dtCore::ActorType> gameEventType = mGM->FindActorType("dtcore.Tasks","GameEvent Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for game event task.",gameEventType.valid());

      dtCore::RefPtr<const dtCore::ActorType> rollupActorType = mGM->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());

      dtCore::RefPtr<const dtCore::ActorType> orderedActorType = mGM->FindActorType("dtcore.Tasks","Ordered Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for ordered task.",orderedActorType.valid());

      // Create our test rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy;
      mGM->CreateActor(*rollupActorType, rollupTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.", rollupTaskProxy.valid());
      mGM->AddActor(*rollupTaskProxy,false,false);

      // Create our test ordered task...
      dtCore::RefPtr<dtActors::TaskActorProxy> orderedTaskProxy;
      mGM->CreateActor(*orderedActorType, orderedTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create ordered task actor proxy.", orderedTaskProxy.valid());
      mGM->AddActor(*orderedTaskProxy,false,false);

      // reate our test primary event task...
      primaryEvent = new dtCore::GameEvent("PrimaryEvent");
      mEventMgr->AddEvent(*primaryEvent);

      mGM->CreateActor(*gameEventType, primaryEventProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create primary game event task actor proxy.",primaryEventProxy.valid());
      primaryEventProxy->GetProperty("GameEvent")->FromString(primaryEvent->GetUniqueId().ToString());
      mGM->AddActor(*primaryEventProxy,false,false);

      // Add an event task which must be completed before the rollup task can be completed.
      orderedTaskProxy->AddSubTask(*primaryEventProxy);
      orderedTaskProxy->AddSubTask(*rollupTaskProxy);

      // Create some event tasks and add them as children to the rollup task.
      for (i = 0; i < 5; ++i)
      {
         dtCore::GameEvent* gameEvent = new dtCore::GameEvent();
         mEventMgr->AddEvent(*gameEvent);

         dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTaskProxy;
         mGM->CreateActor(*gameEventType, eventTaskProxy);
         CPPUNIT_ASSERT_MESSAGE("Could not create game event task actor proxy.",eventTaskProxy.valid());

         eventTaskProxy->GetProperty("GameEvent")->FromString(gameEvent->GetUniqueId().ToString());
         eventTaskProxy->GetProperty("Weight")->FromString("0.2");

         CPPUNIT_ASSERT_EQUAL(std::string("0.2"), eventTaskProxy->GetProperty("Weight")->ToString());

         rollupTaskProxy->AddSubTask(*eventTaskProxy);
         mGM->AddActor(*eventTaskProxy,false,false);
         eventList[i] = gameEvent;
         eventProxyList[i] = eventTaskProxy;
      }

      CPPUNIT_ASSERT_MESSAGE("Rollup task should have 5 child tasks.",rollupTaskProxy->GetSubTaskCount() == 5);

      // Now we need to send a message for each of the 5 events.  This will attempt to mark each task
      // complete which should NOT occur until the primary event task has been completed first since that
      // is first in the ordered task's child task list.
      dtCore::FloatActorProperty* prop =
            static_cast<dtCore::FloatActorProperty*>(rollupTaskProxy->GetProperty("Score"));
      for (i = 0; i < 5; ++i)
      {
         eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGM->SendMessage(*eventMsg);
         dtCore::System::GetInstance().Step();

         CPPUNIT_ASSERT_MESSAGE("Event Task: " + dtUtil::ToString(i) + " should not have been complete.",
            eventProxyList[i]->GetProperty("Complete")->ToString() == "false");

         CPPUNIT_ASSERT_MESSAGE("Rollup task score was not calculated correctly.  The score was: " +
               dtUtil::ToString(prop->GetValue()),osg::equivalent(prop->GetValue(),0.0f,0.001f));
      }

      // Now we will complete the primary event which should then allow the rollup task and its children
      // to be evaluated.
      eventMsg = static_cast<dtGame::GameEventMessage*>
            (mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

      eventMsg->SetGameEvent(*primaryEvent);
      mGM->SendMessage(*eventMsg);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("Primary Event Task should have been complete.",
                             primaryEventProxy->GetProperty("Complete")->ToString() == "true");

      // Now the rollup task and its subtasks should be "completable".
      for (i = 0; i < 5; ++i)
      {
         eventMsg = static_cast<dtGame::GameEventMessage*>
               (mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT).get());

         eventMsg->SetGameEvent(*eventList[i]);
         mGM->SendMessage(*eventMsg);
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
      dtCore::RefPtr<const dtCore::ActorType> taskActorType =
         mGM->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<const dtCore::ActorType> rollupActorType =
         mGM->FindActorType("dtcore.Tasks","Rollup Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for rollup task.",rollupActorType.valid());


      dtCore::RefPtr<dtActors::TaskActorProxy> proxy;
      mGM->CreateActor(*taskActorType, proxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",proxy.valid());

      dtActors::TaskActor* actor = dynamic_cast<dtActors::TaskActor*>(proxy->GetDrawable());

      // Add notify events to the root task.
      CPPUNIT_ASSERT(SetNotifyEventsOnTask(*proxy));

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

      // Ensure events were sent.
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Notify Completed Event should NOT have been sent.",
         GetEventCount(*mNotifyCompletedEvent) == 0);
      CPPUNIT_ASSERT_MESSAGE("Notify Failed Event should have been sent only once.",
         GetEventCount(*mNotifyFailedEvent) == 1);
      mTestComp->reset();


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

      // Ensure events were sent.
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Notify Completed Event should have been sent only once.",
         GetEventCount(*mNotifyCompletedEvent) == 1);
      CPPUNIT_ASSERT_MESSAGE("Notify Failed Event should NOT have been sent.",
         GetEventCount(*mNotifyFailedEvent) == 0);
      mTestComp->reset();

      /// TEST the PARENT - CHILD behavior.

      // Create our parent rollup task...
      dtCore::RefPtr<dtActors::TaskActorProxy> rollupTaskProxy;
      mGM->CreateActor(*rollupActorType, rollupTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create rollup task actor proxy.", rollupTaskProxy.valid());
      dtActors::TaskActor* parentActor;
      rollupTaskProxy->GetDrawable(parentActor);
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
      dtCore::RefPtr<const dtCore::ActorType> taskActorType =
         mGM->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());

      dtCore::RefPtr<const dtCore::ActorType> orderedActorType = mGM->FindActorType("dtcore.Tasks","Ordered Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type for ordered task.",orderedActorType.valid());

      // Create our test child task
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy1;
      mGM->CreateActor(*taskActorType, childProxy1);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy1.valid());
      mGM->AddActor(*childProxy1,false,false);

      // Child 2 task
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy2;
      mGM->CreateActor(*taskActorType, childProxy2);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy2.valid());
      mGM->AddActor(*childProxy2,false,false);

      // Create our test ordered task...
      dtCore::RefPtr<dtActors::TaskActorProxy> orderedTaskProxy;
      mGM->CreateActor(*orderedActorType, orderedTaskProxy);
      CPPUNIT_ASSERT_MESSAGE("Could not create ordered task actor proxy.", orderedTaskProxy.valid());
      mGM->AddActor(*orderedTaskProxy,false,false);

      dtActors::TaskActor* childActor1 = dynamic_cast<dtActors::TaskActor*>(childProxy1->GetDrawable());
      dtActors::TaskActor* childActor2 = dynamic_cast<dtActors::TaskActor*>(childProxy2->GetDrawable());

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
      dtActors::TaskActorOrdered* orderedTaskActor = nullptr;
      orderedTaskProxy->GetDrawable(orderedTaskActor);
      orderedTaskActor->SetFailed(true);
      CPPUNIT_ASSERT_MESSAGE("2nd child task should NOT be mutable when ordered parent is failed.", !childProxy2->IsCurrentlyMutable());

      orderedTaskActor->Reset();
      childActor1->Reset();
      childActor2->Reset();

      bool result = childProxy1->IsCurrentlyMutable();

      CPPUNIT_ASSERT_MESSAGE("A blocking ordered task should have a mutable first child", result);

      result = childProxy2->IsCurrentlyMutable();
      CPPUNIT_ASSERT_MESSAGE("A blocking ordered task should NOT have a mutable second child", !result);

      orderedTaskActor->SetFailureType(dtActors::TaskActorOrdered::FailureType::CAUSE_FAILURE);
      CPPUNIT_ASSERT_MESSAGE("A failing ordered task should have mutable children",
         childProxy1->IsCurrentlyMutable());
      CPPUNIT_ASSERT_MESSAGE("A failing ordered task should have mutable children",
         childProxy2->IsCurrentlyMutable());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestNestedMutable()
{
   dtCore::RefPtr<dtActors::TaskActorOrderedProxy> masterTask;
   dtCore::RefPtr<dtActors::TaskActorOrderedProxy> subTaskOne;
   dtCore::RefPtr<dtActors::TaskActorOrderedProxy> subTaskTwo;

   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventSubOne;
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventSubTwo;
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventOne;
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventTwo;
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventThree;
   dtCore::RefPtr<dtActors::TaskActorGameEventProxy> eventFour;

   mGM->CreateActor(*dtActors::EngineActorRegistry::ORDERED_TASK_ACTOR_TYPE, masterTask);
   mGM->CreateActor(*dtActors::EngineActorRegistry::ORDERED_TASK_ACTOR_TYPE, subTaskOne);
   mGM->CreateActor(*dtActors::EngineActorRegistry::ORDERED_TASK_ACTOR_TYPE, subTaskTwo);

   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, eventSubOne);
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, eventSubTwo);
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, eventOne);
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, eventTwo);
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, eventThree);
   mGM->CreateActor(*dtActors::EngineActorRegistry::GAME_EVENT_TASK_ACTOR_TYPE, eventFour);

   mGM->AddActor(*masterTask, false, false);
   mGM->AddActor(*subTaskOne, false, false);
   mGM->AddActor(*subTaskTwo, false, false);

   mGM->AddActor(*eventSubOne, false, false);
   mGM->AddActor(*eventSubTwo, false, false);
   mGM->AddActor(*eventOne, false, false);
   mGM->AddActor(*eventTwo, false, false);
   mGM->AddActor(*eventThree, false, false);
   mGM->AddActor(*eventFour, false, false);

   masterTask->SetName("MasterTask");
   subTaskOne->SetName("SubTaskOne");
   subTaskTwo->SetName("SubTaskTwo");

   eventSubOne->SetName("EventSubOne");
   eventSubTwo->SetName("EventSubTwo");
   eventOne->SetName("EventOne");
   eventTwo->SetName("EventTwo");
   eventThree->SetName("EventThree");
   eventFour->SetName("EventFour");

   CPPUNIT_ASSERT(masterTask.valid());
   CPPUNIT_ASSERT(subTaskOne.valid());
   CPPUNIT_ASSERT(subTaskTwo.valid());

   CPPUNIT_ASSERT(eventSubOne.valid());
   CPPUNIT_ASSERT(eventSubTwo.valid());
   CPPUNIT_ASSERT(eventOne.valid());
   CPPUNIT_ASSERT(eventTwo.valid());
   CPPUNIT_ASSERT(eventThree.valid());
   CPPUNIT_ASSERT(eventFour.valid());

   subTaskOne->AddSubTask(*eventSubOne);
   subTaskOne->AddSubTask(*eventSubTwo);

   subTaskTwo->AddSubTask(*eventOne);
   subTaskTwo->AddSubTask(*eventTwo);
   subTaskTwo->AddSubTask(*eventThree);
   subTaskTwo->AddSubTask(*eventFour);

   masterTask->AddSubTask(*subTaskOne);
   masterTask->AddSubTask(*subTaskTwo);

   bool result = masterTask->IsCurrentlyMutable();
   CPPUNIT_ASSERT(!result);
   CPPUNIT_ASSERT(!subTaskOne->IsCurrentlyMutable());
   CPPUNIT_ASSERT(!subTaskTwo->IsCurrentlyMutable());

   CPPUNIT_ASSERT_MESSAGE("The first event on the first task should be mutable by default",
      eventSubOne->IsCurrentlyMutable());
   CPPUNIT_ASSERT(!eventSubTwo->IsCurrentlyMutable());

   CPPUNIT_ASSERT(!eventOne->IsCurrentlyMutable());
   CPPUNIT_ASSERT(!eventTwo->IsCurrentlyMutable());
   CPPUNIT_ASSERT(!eventThree->IsCurrentlyMutable());
   CPPUNIT_ASSERT(!eventFour->IsCurrentlyMutable());

   dtActors::TaskActor* taskActorOne;
   subTaskOne->GetDrawable(taskActorOne);
   CPPUNIT_ASSERT(taskActorOne != nullptr);

   dtActors::TaskActorGameEvent* eventSubActorOne;
   eventSubOne->GetDrawable(eventSubActorOne);
   CPPUNIT_ASSERT(eventSubActorOne);

   dtActors::TaskActorGameEvent* eventSubActorTwo;
   eventSubTwo->GetDrawable(eventSubActorTwo);
   CPPUNIT_ASSERT(eventSubActorTwo);

   eventSubActorOne->SetComplete(true);
   CPPUNIT_ASSERT_MESSAGE("The second event of the first subtask should now be mutable",
      eventSubTwo->IsCurrentlyMutable());

   eventSubActorTwo->SetComplete(true);

   taskActorOne->SetComplete(true);

   CPPUNIT_ASSERT_MESSAGE("The first event should be mutable since the first subtask is completed",
      eventOne->IsCurrentlyMutable());

   dtActors::TaskActorGameEvent* eventActorOne;
   eventOne->GetDrawable(eventActorOne);
   CPPUNIT_ASSERT(eventActorOne);

   dtActors::TaskActorGameEvent* eventActorTwo;
   eventTwo->GetDrawable(eventActorTwo);
   CPPUNIT_ASSERT(eventActorTwo);

   dtActors::TaskActorGameEvent* eventActorThree;
   eventThree->GetDrawable(eventActorThree);
   CPPUNIT_ASSERT(eventActorThree);

   dtActors::TaskActorGameEvent* eventActorFour;
   eventFour->GetDrawable(eventActorFour);
   CPPUNIT_ASSERT(eventActorFour);

   eventActorOne->SetComplete(true);

   CPPUNIT_ASSERT_MESSAGE("The second event should be mutable since the first event is completed",
      eventTwo->IsCurrentlyMutable());

   eventActorTwo->SetComplete(true);

   result = eventThree->IsCurrentlyMutable();
   CPPUNIT_ASSERT_MESSAGE("The third event should be mutable since the second event is completed",
      result);

   eventActorThree->SetComplete(true);

   CPPUNIT_ASSERT_MESSAGE("The fourth event should be mutable since the third event is completed",
      eventFour->IsCurrentlyMutable());
}

