/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright 2013,  David Guthrie
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
 * David Guthrie
 */

#include <prefix/unittestprefix.h>
//#include <prefix/dtgameprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/deltawin.h>
#include <dtCore/keyboard.h>
#include <dtCore/system.h>
#include <dtGUI/gui.h>
#include <dtUtil/exception.h>
#include <CEGUIWindow.h>

#include <dtABC/application.h>



namespace dtTest
{
   ////////////////////////////////////////////////////////////////////////////
   // HELPER TEST CLASS
   ////////////////////////////////////////////////////////////////////////////
   class TestGUITask : public dtGUI::GUI::GUITask
   {
   public:
      int mUpdateCount;
      std::string mName;

      TestGUITask(const std::string& name)
         : mUpdateCount(0)
         , mName(name)
      {}

      virtual void Update(float dt)
      {
         ++mUpdateCount;
      }

   protected:
      virtual ~TestGUITask()
      {}
   };

   typedef dtGUI::GUI::GUITaskArray GUITaskArray;
   typedef dtGUI::GUI::GUITask GUITask;
   typedef dtCore::RefPtr<GUITask> GUITaskPtr;
   typedef dtCore::RefPtr<TestGUITask> TestTaskPtr;



   /// unit tests for dtGUI::GUI
   class GUITests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( GUITests );
      CPPUNIT_TEST( TestCreateMultiple );
      CPPUNIT_TEST( TestTaskManagement );
      CPPUNIT_TEST( TestTaskUpdate );
      CPPUNIT_TEST( TestTaskUpdateByMessage );
      CPPUNIT_TEST_SUITE_END();

      dtCore::RefPtr<dtABC::Application> mApp;
      dtCore::RefPtr<dtGUI::GUI> mGUI;
      dtCore::RefPtr<dtCore::Keyboard> mKeyboard;

   public:
      void setUp()
      {
         dtCore::System& system = dtCore::System::GetInstance();
         system.SetSystemStages(dtCore::System::STAGE_PREFRAME);
         system.SetUseFixedTimeStep(false);
         system.SetShutdownOnWindowClose(false);
         system.SetPause(false);
         system.SetFrameRate(60.0f);
         system.Start();
      }

      void tearDown()
      {
         mGUI = NULL;
         mKeyboard = NULL;
         mApp = NULL;

         dtCore::System::GetInstance().Stop();
         dtCore::System::Destroy();
      }

      void SetupGUIManually()
      {
         mApp = new dtABC::Application;
         mApp->GetWindow()->SetPosition(0, 0, 50, 50);
         mApp->Config();

         mKeyboard = mApp->GetKeyboard();

         mGUI = new dtGUI::GUI(mApp->GetCamera(), mKeyboard.get(), mApp->GetMouse());
      }

      void TestCreateMultiple()
      {
         // start an application with GUI support
         dtCore::RefPtr<dtABC::Application> app( new dtABC::Application );
         app->GetWindow()->SetPosition(0, 0, 50, 50);
         app->Config();

         dtCore::RefPtr<dtCore::Keyboard> kb = app->GetKeyboard();

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() == NULL);
         // create 2 guis to make sure it doesn't crash.
         dtCore::RefPtr<dtGUI::GUI> gui = new dtGUI::GUI(app->GetCamera(), kb, app->GetMouse());

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() != NULL);
         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr()->getRenderer() != NULL);

         dtCore::RefPtr<dtGUI::GUI> gui2 = new dtGUI::GUI(app->GetCamera(), kb, app->GetMouse());

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() != NULL);
         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr()->getRenderer() != NULL);

         gui = NULL;

         CPPUNIT_ASSERT_MESSAGE("Deleting 1 GUI but leaving another around should not cause the system to be deleted.", CEGUI::System::getSingletonPtr() != NULL);
         CPPUNIT_ASSERT_MESSAGE("Deleting 1 GUI but leaving another around should not cause the renderer.", CEGUI::System::getSingletonPtr()->getRenderer() != NULL);

         gui2 = NULL;

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() == NULL);
      }

      void TestTaskManagement()
      {
         try
         {
            SetupGUIManually();

            const std::string NAME1("TaskA");
            const std::string NAME2("TaskB");
            const std::string NAME3("TaskC");

            TestTaskPtr task1 = new TestGUITask(NAME1);
            TestTaskPtr task2 = new TestGUITask(NAME2);
            TestTaskPtr task3 = new TestGUITask(NAME3);

            // Ensure the GUI has no tasks currently.
            CPPUNIT_ASSERT(mGUI.valid());
            CPPUNIT_ASSERT(mGUI->GetTasks().empty());
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 0);
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task3));

            // Add a single task.
            CPPUNIT_ASSERT(mGUI->AddTask(*task1));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 1);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 1);
            CPPUNIT_ASSERT(mGUI->HasTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task3));

            // Add all the tasks.
            CPPUNIT_ASSERT(mGUI->AddTask(*task2));
            CPPUNIT_ASSERT(mGUI->AddTask(*task3));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 3);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 3);
            CPPUNIT_ASSERT(mGUI->HasTask(*task1));
            CPPUNIT_ASSERT(mGUI->HasTask(*task2));
            CPPUNIT_ASSERT(mGUI->HasTask(*task3));

            // Test that adding a task fails when it is an existing task in the GUI.
            CPPUNIT_ASSERT( ! mGUI->AddTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->AddTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->AddTask(*task3));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 3);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 3);

            // Remove a single task.
            CPPUNIT_ASSERT(mGUI->RemoveTask(*task2));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 2);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 2);
            CPPUNIT_ASSERT(mGUI->HasTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task2));
            CPPUNIT_ASSERT(mGUI->HasTask(*task3));

            // Test removing the rest of the tasks.
            CPPUNIT_ASSERT(mGUI->RemoveTask(*task1));
            CPPUNIT_ASSERT(mGUI->RemoveTask(*task3));
            CPPUNIT_ASSERT(mGUI->GetTasks().empty());
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 0);
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task3));

            // Test that removing a task fails if it is not in the GUI.
            CPPUNIT_ASSERT( ! mGUI->RemoveTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->RemoveTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->RemoveTask(*task3));
            CPPUNIT_ASSERT(mGUI->GetTasks().empty());
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 0);
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task3));
         }
         catch (dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

      void TestTaskUpdate()
      {
         try
         {
            SetupGUIManually();

            const std::string NAME1("TaskA");
            const std::string NAME2("TaskB");
            const std::string NAME3("TaskC");

            TestTaskPtr task1 = new TestGUITask(NAME1);
            TestTaskPtr task2 = new TestGUITask(NAME2);
            TestTaskPtr task3 = new TestGUITask(NAME3);

            // Add a single task.
            CPPUNIT_ASSERT(mGUI->AddTask(*task1));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 1);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 1);
            CPPUNIT_ASSERT(mGUI->HasTask(*task1));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task2));
            CPPUNIT_ASSERT( ! mGUI->HasTask(*task3));

            CPPUNIT_ASSERT(task1->mUpdateCount == 0);
            CPPUNIT_ASSERT(task2->mUpdateCount == 0);
            CPPUNIT_ASSERT(task3->mUpdateCount == 0);

            // Test that only added tasks are updated.
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 1);
            CPPUNIT_ASSERT(task1->mUpdateCount == 1);
            CPPUNIT_ASSERT(task2->mUpdateCount == 0);
            CPPUNIT_ASSERT(task3->mUpdateCount == 0);
            
            // --- Update a couple more times.
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 1);
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 1);
            CPPUNIT_ASSERT(task1->mUpdateCount == 3);
            CPPUNIT_ASSERT(task2->mUpdateCount == 0);
            CPPUNIT_ASSERT(task3->mUpdateCount == 0);


            // Test update all tasks.
            CPPUNIT_ASSERT(mGUI->AddTask(*task2));
            CPPUNIT_ASSERT(mGUI->AddTask(*task3));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 3);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 3);

            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 3);
            CPPUNIT_ASSERT(task1->mUpdateCount == 4);
            CPPUNIT_ASSERT(task2->mUpdateCount == 1);
            CPPUNIT_ASSERT(task3->mUpdateCount == 1);
            
            // --- Update a couple more times.
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 3);
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 3);
            CPPUNIT_ASSERT(task1->mUpdateCount == 6);
            CPPUNIT_ASSERT(task2->mUpdateCount == 3);
            CPPUNIT_ASSERT(task3->mUpdateCount == 3);


            // Test that removed tasks are not updated.
            task1->mUpdateCount = 0;
            task2->mUpdateCount = 0;
            task3->mUpdateCount = 0;
            CPPUNIT_ASSERT(mGUI->RemoveTask(*task2));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 2);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 2);
            
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 2);
            CPPUNIT_ASSERT(task1->mUpdateCount == 1);
            CPPUNIT_ASSERT(task2->mUpdateCount == 0);
            CPPUNIT_ASSERT(task3->mUpdateCount == 1);

            // --- Update a couple more times.
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 2);
            CPPUNIT_ASSERT(mGUI->UpdateTasks() == 2);
            CPPUNIT_ASSERT(task1->mUpdateCount == 3);
            CPPUNIT_ASSERT(task2->mUpdateCount == 0);
            CPPUNIT_ASSERT(task3->mUpdateCount == 3);
         }
         catch (dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

      void TestTaskUpdateByMessage()
      {
         try
         {
            SetupGUIManually();

            dtCore::System& system = dtCore::System::GetInstance();

            std::string taskName("TestTask");
            TestTaskPtr task = new TestGUITask(taskName);

            CPPUNIT_ASSERT(mGUI->AddTask(*task));
            CPPUNIT_ASSERT(mGUI->GetTasks().size() == 1);
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 1);

            // Test updates by a simulated preframe message.
            system.Step();
            CPPUNIT_ASSERT(task->mUpdateCount == 1);

            // --- Update a couple more times.
            system.Step();
            system.Step();
            CPPUNIT_ASSERT(task->mUpdateCount == 3);

            
            // Remove the task and make sure it does not update.
            task->mUpdateCount = 0;
            CPPUNIT_ASSERT(mGUI->RemoveTask(*task));
            CPPUNIT_ASSERT(mGUI->GetTasks().empty());
            CPPUNIT_ASSERT(mGUI->GetTaskCount() == 0);

            system.Step();
            system.Step();
            CPPUNIT_ASSERT(task->mUpdateCount == 0);
         }
         catch (dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString());
         }
      }

   private:
   };
   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( GUITests );

}

