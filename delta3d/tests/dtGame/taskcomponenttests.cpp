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
* @author Matthew W. Campbell
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/actortype.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/gamemanager.h>
#include <dtGame/taskcomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtABC/application.h>

#include <dtCore/timer.h>

extern dtABC::Application& GetGlobalApplication();

class GMTaskComponentTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GMTaskComponentTests);
      CPPUNIT_TEST(TestTaskComponentCreate);
      CPPUNIT_TEST(TestTaskComponentTaskTracking);
      CPPUNIT_TEST(TestChangeMap);
      CPPUNIT_TEST(TestGetTasks);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestTaskComponentCreate();
      void TestTaskComponentTaskTracking();
      void TestChangeMap();
      void TestGetTasks();

   private:
      dtCore::RefPtr<dtGame::GameManager> mGameManager;
      static const std::string mTestGameActorLibrary;
      static const std::string mTestActorLibrary;
};

CPPUNIT_TEST_SUITE_REGISTRATION(GMTaskComponentTests);

const std::string GMTaskComponentTests::mTestGameActorLibrary="testGameActorLibrary";
const std::string GMTaskComponentTests::mTestActorLibrary="testActorLibrary";

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void GMTaskComponentTests::setUp()
{
   try
   {
      mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
      mGameManager->SetApplication(GetGlobalApplication());
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }

}

//////////////////////////////////////////////////////////////////////////
void GMTaskComponentTests::tearDown()
{
   if (mGameManager.valid())
   {
      try
      {
         dtCore::System::GetInstance().SetPause(false);
         dtCore::System::GetInstance().Stop();
         mGameManager->DeleteAllActors();
         mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
         mGameManager = NULL;
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   }
}

void GMTaskComponentTests::TestTaskComponentCreate()
{
   try
   {
      dtCore::RefPtr<dtGame::TaskComponent> taskComponent =
         new dtGame::TaskComponent();

      mGameManager->AddComponent(*taskComponent,dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMTaskComponentTests::TestTaskComponentTaskTracking()
{
   try
   {
      unsigned int i;
      dtCore::RefPtr<dtGame::TaskComponent> taskComponent = new dtGame::TaskComponent();
      dtCore::RefPtr<dtCore::BaseActorObject> proxy = NULL;
      dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

      mGameManager->AddComponent(*taskComponent,dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);

      dtCore::RefPtr<const dtCore::ActorType> playerType = mGameManager->FindActorType("ExampleActors", "Test1Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find test player actor type.",playerType != NULL);

      dtCore::RefPtr<const dtCore::ActorType> taskType = mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find task actor type.",taskType != NULL);

      //Add and remove a few task actors...
      proxy = mGameManager->CreateActor(*taskType);
      gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);

      mGameManager->AddActor(*gameProxy,false,false);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Task component should have 1 top level task.",
         taskComponent->GetNumTopLevelTasks() == 1);
      mGameManager->DeleteActor(*gameProxy);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Task component should have 0 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 0);

      //Create some actors...
      for (i=0; i<100; i++)
      {
         if ((i%2) == 0)
            proxy = mGameManager->CreateActor(*playerType);
         else
            proxy = mGameManager->CreateActor(*taskType);

         proxy->SetName("BaseActorObject" + dtUtil::ToString(i));
         gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
         CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);
         mGameManager->AddActor(*gameProxy,false,false);

         if ((i%10) == 0)
            dtCore::System::GetInstance().Step();
      }

      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Task component should have 50 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 50);
      CPPUNIT_ASSERT_MESSAGE("Task component should have 50 total tasks.",
         taskComponent->GetNumTasks() == 50);

      //Remove some actors and make sure the removal is tracked..
      mGameManager->DeleteAllActors();

      dtCore::AppSleep(50);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("Task component should have no top level tasks after deletion.",
         taskComponent->GetNumTopLevelTasks() == 0);
      CPPUNIT_ASSERT_MESSAGE("Task component should have no tasks after deletion.",
         taskComponent->GetNumTasks() == 0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMTaskComponentTests::TestChangeMap()
{
   try
   {
      dtCore::RefPtr<dtGame::TaskComponent> taskComponent =
         new dtGame::TaskComponent();

      mGameManager->AddComponent(*taskComponent,dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);

      //Create some actors...
      dtCore::RefPtr<const dtCore::ActorType> taskType = mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find task actor type.",taskType != NULL);

      for (unsigned int i=0; i<20; i++)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = NULL;
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

         proxy = mGameManager->CreateActor(*taskType);
         gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

         gameProxy->SetName("TaskProxy" + dtUtil::ToString(i));
         CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);
         mGameManager->AddActor(*gameProxy,false,false);

         dtCore::System::GetInstance().Step();
      }

      CPPUNIT_ASSERT_MESSAGE("Task component should have 20 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 20);
      CPPUNIT_ASSERT_MESSAGE("Task component should have 20 total tasks.",
         taskComponent->GetNumTasks() == 20);

      //This code simulates a map change since we do not actually have a map to change.
      //Still tests the task component logic though.
      mGameManager->DeleteAllActors(true);
      dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
      dtCore::RefPtr<dtGame::MapMessage> mapLoadMessage;
      msgFactory.CreateMessage(dtGame::MessageType::INFO_MAP_LOADED, mapLoadMessage);
      mGameManager->SendMessage(*mapLoadMessage);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("Task component should have 0 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 0);
      CPPUNIT_ASSERT_MESSAGE("Task component should have 0 total tasks.",
         taskComponent->GetNumTasks() == 0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMTaskComponentTests::TestGetTasks()
{
   try
   {
      unsigned int i;
      dtCore::RefPtr<dtGame::TaskComponent> taskComponent =
         new dtGame::TaskComponent();

      mGameManager->AddComponent(*taskComponent,dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);

      //Create some actors...
      dtCore::RefPtr<const dtCore::ActorType> taskType = mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find task actor type.",taskType != NULL);

      for (i=0; i<20; i++)
      {
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = NULL;
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

         proxy = mGameManager->CreateActor(*taskType);
         gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

         CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);
         gameProxy->SetName("TASK" + dtUtil::ToString(i));
         mGameManager->AddActor(*gameProxy,false,false);

         dtCore::System::GetInstance().Step();
      }

      std::vector<dtCore::RefPtr<dtGame::GameActorProxy> > allTasks,topTasks;
      std::vector<dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor;
      taskComponent->GetAllTasks(allTasks);
      taskComponent->GetTopLevelTasks(topTasks);

      CPPUNIT_ASSERT_MESSAGE("Should have been 20 top level tasks in returned list.",
         topTasks.size() == 20);
      CPPUNIT_ASSERT_MESSAGE("Should have been 20 tasks in returned list.",
         allTasks.size() == 20);

      for (i=0; i<20; i++)
      {
         std::string name = "TASK" + dtUtil::ToString(i);

         dtCore::RefPtr<dtGame::GameActorProxy> gameProxy =
            taskComponent->GetTaskByName("TASK" + dtUtil::ToString(i));
         CPPUNIT_ASSERT_MESSAGE("Should have found a task of name: " + name,gameProxy != NULL);
         CPPUNIT_ASSERT_MESSAGE("Proxy name should have been: " + name, gameProxy->GetName() == name);
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

