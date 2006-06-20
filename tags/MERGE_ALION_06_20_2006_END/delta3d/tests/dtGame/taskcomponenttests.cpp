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
#include <dtGame/clientgamemanager.h>
#include <dtGame/taskcomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/basemessages.h>
#include <dtCore/system.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

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
      dtCore::RefPtr<dtGame::ClientGameManager> mGameManager;
      static char* mTestGameActorLibrary;
      static char* mTestActorLibrary;
};

CPPUNIT_TEST_SUITE_REGISTRATION(GMTaskComponentTests);
#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* GMTaskComponentTests::mTestGameActorLibrary="testGameActorLibrary";
char* GMTaskComponentTests::mTestActorLibrary="testActorLibrary";
#else
char* GMTaskComponentTests::mTestGameActorLibrary="testGameActorLibrary";
char* GMTaskComponentTests::mTestActorLibrary="testActorLibrary";
#endif

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void GMTaskComponentTests::setUp()
{
   try
   {
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::ClientGameManager(*scene);
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::Instance()->SetShutdownOnWindowClose(false);
      dtCore::System::Instance()->Start();
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
         dtCore::System::Instance()->SetPause(false);
         dtCore::System::Instance()->Stop();
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
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = NULL;
      dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

      mGameManager->AddComponent(*taskComponent,dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);

      dtCore::RefPtr<dtDAL::ActorType> playerType = mGameManager->FindActorType("ExampleActors", "Test1Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find test player actor type.",playerType != NULL);

      dtCore::RefPtr<dtDAL::ActorType> taskType = mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find task actor type.",taskType != NULL);

      //Add and remove a few task actors...
      proxy = mGameManager->CreateActor(*taskType);
      gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);

      mGameManager->AddActor(*gameProxy,false,false);
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Task component should have 1 top level task.",
         taskComponent->GetNumTopLevelTasks() == 1);
      mGameManager->DeleteActor(*gameProxy);
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Task component should have 0 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 0);

      //Create some actors...
      for (i=0; i<100; i++)
      {
         if ((i%2) == 0)
            proxy = mGameManager->CreateActor(*playerType);
         else
            proxy = mGameManager->CreateActor(*taskType);

         proxy->SetName("ActorProxy" + dtUtil::ToString(i));
         gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
         CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);
         mGameManager->AddActor(*gameProxy,false,false);

         if ((i%10) == 0)
            dtCore::System::Instance()->Step();
      }

      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Task component should have 50 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 50);
      CPPUNIT_ASSERT_MESSAGE("Task component should have 50 total tasks.",
         taskComponent->GetNumTasks() == 50);

      //Remove some actors and make sure the removal is tracked..
      mGameManager->DeleteAllActors();

      SLEEP(50);
      dtCore::System::Instance()->Step();

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
      dtCore::RefPtr<dtDAL::ActorType> taskType = mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find task actor type.",taskType != NULL);

      for (unsigned int i=0; i<20; i++)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = NULL;
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

         proxy = mGameManager->CreateActor(*taskType);
         gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

         gameProxy->SetName("TaskProxy" + dtUtil::ToString(i));
         CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);
         mGameManager->AddActor(*gameProxy,false,false);

         dtCore::System::Instance()->Step();
      }

      CPPUNIT_ASSERT_MESSAGE("Task component should have 20 top level tasks.",
         taskComponent->GetNumTopLevelTasks() == 20);
      CPPUNIT_ASSERT_MESSAGE("Task component should have 20 total tasks.",
         taskComponent->GetNumTasks() == 20);

      //This code simulates a map change since we do not actually have a map to change.
      //Still tests the task component logic though.
      mGameManager->DeleteAllActors(true);
      dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
      dtCore::RefPtr<dtGame::MapLoadedMessage> mapLoadMessage =
         (dtGame::MapLoadedMessage *)(msgFactory.CreateMessage(dtGame::MessageType::INFO_MAP_LOADED)).get();
      mGameManager->SendMessage(*mapLoadMessage);
      dtCore::System::Instance()->Step();

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
      dtCore::RefPtr<dtDAL::ActorType> taskType = mGameManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find task actor type.",taskType != NULL);

      for (i=0; i<20; i++)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = NULL;
         dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

         proxy = mGameManager->CreateActor(*taskType);
         gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

         CPPUNIT_ASSERT_MESSAGE("Should have been a game actor.",gameProxy != NULL);
         gameProxy->SetName("TASK" + dtUtil::ToString(i));
         mGameManager->AddActor(*gameProxy,false,false);

         dtCore::System::Instance()->Step();
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

