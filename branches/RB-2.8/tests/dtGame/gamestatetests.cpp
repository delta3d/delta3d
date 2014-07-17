/*
 * Copyright, 2009, Alion Science and Technology Corporation, all rights reserved.
 * 
 * Alion Science and Technology Corporation
 * 5365 Robin Hood Road
 * Norfolk, VA 23513
 * (757) 857-5670, www.alionscience.com
 * 
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * @author Bradley Anderegg
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>

#include <dtABC/application.h>
#include <dtCore/deltawin.h>
#include <dtCore/project.h>
#include <dtCore/system.h>
#include <dtGame/basemessages.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gamestate.h>
#include <dtGame/gamestatecomponent.h>
#include <dtGame/gamestatemessages.h>
#include <dtGame/gmcomponent.h>
#include <dtUtil/command.h>
#include <dtUtil/datapathutils.h>

extern dtABC::Application& GetGlobalApplication();



namespace dtGame
{
   ////////////////////////////////////////////////////////////////////////////////
   // TEST CODE
   ////////////////////////////////////////////////////////////////////////////////
   class GameStateTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(GameStateTests);

      CPPUNIT_TEST(TestLoadTransitionFile);

      CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      void TestLoadTransitionFile();
      
   private:
      void TestTransitionFunc(int i, bool b);

      int mLastFunc, mCurrFunc;
      dtCore::RefPtr<dtABC::Application> mApp;
      dtCore::RefPtr<dtGame::GameManager> mGM;

      // Components
      dtCore::RefPtr<GameStateComponent> mGameStateComponent;
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(GameStateTests);



   ////////////////////////////////////////////////////////////////////////////////
   // Added states for the test.
   class ExtendedStateType : public dtGame::GameStateType
   {
      public:
         static const ExtendedStateType STATE_DEBRIEF;
         static const ExtendedStateType STATE_RUNNING_EXIT_PROMPT;

         ExtendedStateType(const std::string& name)
            : StateType(name,false)
         {
         }

      protected:
         virtual ~ExtendedStateType() {}
   };

   const ExtendedStateType ExtendedStateType::STATE_DEBRIEF("STATE_DEBRIEF");
   const ExtendedStateType ExtendedStateType::STATE_RUNNING_EXIT_PROMPT("STATE_RUNNING_EXIT_PROMPT");



   ////////////////////////////////////////////////////////////////////////////////
   void GameStateTests::setUp()
   {
      mLastFunc = 0;
      mCurrFunc = 0;

      // Initialize the System.
      dtCore::System::GetInstance().Config();
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

      // Initialize the Application.
      mApp = &GetGlobalApplication();

      // Initialize the Game Manager.
      mGM = new dtGame::GameManager( *mApp->GetScene() );
      mGM->SetApplication( *mApp );

      // Create & add components
      mGameStateComponent = new GameStateComponent;
      mGM->AddComponent(*mGameStateComponent, dtGame::GameManager::ComponentPriority::NORMAL);
      mGM->AddComponent(*new dtGame::DefaultMessageProcessor, dtGame::GameManager::ComponentPriority::HIGHEST); 

      // Set the search paths.
      dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");
      std::string filePath = dtUtil::FindFileInPathList("Transitions/TestTransitions.xml");
      CPPUNIT_ASSERT( ! filePath.empty());

      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
      dtCore::System::GetInstance().Step();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameStateTests::tearDown()
   {
      dtCore::System::GetInstance().Stop();

      // --- from Game Manager
      mGM->DeleteAllActors();
      dtCore::System::GetInstance().Step();

      // Delete Components
      mGameStateComponent = NULL;

      mGM->Shutdown();
      // Delete the Game Manager and Application
      mGM = NULL;
      mApp = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameStateTests::TestLoadTransitionFile()
   {
      std::string filePath = dtCore::Project::GetInstance().GetResourcePath(dtCore::ResourceDescriptor("Transitions:TestTransitions.xml"));
      bool loadedFile = mGameStateComponent->LoadTransitions(filePath);
      CPPUNIT_ASSERT_MESSAGE("Unable to load transition file: filePath" , loadedFile);

      CPPUNIT_ASSERT_MESSAGE("We should begin in the intro state", mGameStateComponent->GetCurrentState() == &StateType::STATE_INTRO);

      typedef dtUtil::Command2<void, int, bool> CMD2;
      dtCore::RefPtr<CMD2> cmd1 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 1, true);
      dtCore::RefPtr<CMD2> cmd2 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 2, true);
      dtCore::RefPtr<CMD2> cmd3 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 3, true);
      dtCore::RefPtr<CMD2> cmd4 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 4, true);
      dtCore::RefPtr<CMD2> cmd5 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 5, true);


      dtCore::RefPtr<CMD2> cmd11 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 1, false);
      dtCore::RefPtr<CMD2> cmd21 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 2, false);
      dtCore::RefPtr<CMD2> cmd31 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 3, false);
      dtCore::RefPtr<CMD2> cmd41 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 4, false);
      dtCore::RefPtr<CMD2> cmd51 = new CMD2(CMD2::FunctorType(this, &GameStateTests::TestTransitionFunc), 5, false);


      mGameStateComponent->GetState(&StateType::STATE_MENU)->AddEntryCommand(cmd1.get());
      mGameStateComponent->GetState(&StateType::STATE_MENU)->AddExitCommand(cmd11.get());

      mGameStateComponent->GetState(&StateType::STATE_LOADING)->AddEntryCommand(cmd2.get());
      mGameStateComponent->GetState(&StateType::STATE_LOADING)->AddExitCommand(cmd21.get());

      mGameStateComponent->GetState(&StateType::STATE_RUNNING)->AddEntryCommand(cmd3.get());
      mGameStateComponent->GetState(&StateType::STATE_RUNNING)->AddExitCommand(cmd31.get());

      mGameStateComponent->GetState(&ExtendedStateType::STATE_DEBRIEF)->AddEntryCommand(cmd4.get());
      mGameStateComponent->GetState(&ExtendedStateType::STATE_DEBRIEF)->AddExitCommand(cmd41.get());

      mGameStateComponent->GetState(&ExtendedStateType::STATE_RUNNING_EXIT_PROMPT)->AddEntryCommand(cmd5.get());
      mGameStateComponent->GetState(&ExtendedStateType::STATE_RUNNING_EXIT_PROMPT)->AddExitCommand(cmd51.get());

      mGameStateComponent->DoStateTransition(&EventType::TRANSITION_OCCURRED);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the entry functor to STATE_MENU, thus setting the mCurrFunc to 1", 1, mCurrFunc);
      CPPUNIT_ASSERT_MESSAGE("The Current State should be STATE_MENU", mGameStateComponent->GetCurrentState() == &StateType::STATE_MENU);
      CPPUNIT_ASSERT(mGameStateComponent->IsInState(StateType::STATE_MENU));

      mGameStateComponent->DoStateTransition(&EventType::TRANSITION_OCCURRED);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the entry functor to STATE_LOADING, thus setting the mCurrFunc to 2", 2, mCurrFunc);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the exit functor of STATE_MENU, thus setting the mLastFunc to 1", 1, mLastFunc);
      CPPUNIT_ASSERT_MESSAGE("The Current State should be STATE_LOADING", mGameStateComponent->GetCurrentState() == &StateType::STATE_LOADING);
      CPPUNIT_ASSERT(mGameStateComponent->IsInState(StateType::STATE_LOADING));

      mGameStateComponent->DoStateTransition(&EventType::TRANSITION_OCCURRED);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the entry functor to STATE_RUNNING, thus setting the mCurrFunc to 3", 3, mCurrFunc);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the exit functor of STATE_LOADING, thus setting the mLastFunc to 2", 2, mLastFunc);
      CPPUNIT_ASSERT_MESSAGE("The Current State should be STATE_RUNNING", mGameStateComponent->GetCurrentState() == &StateType::STATE_RUNNING);
      CPPUNIT_ASSERT(mGameStateComponent->IsInState(StateType::STATE_RUNNING));

      mGameStateComponent->DoStateTransition(&EventType::TRANSITION_OCCURRED);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the entry functor to STATE_DEBRIEF, thus setting the mCurrFunc to 4", 4, mCurrFunc);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the exit functor of STATE_RUNNING, thus setting the mLastFunc to 3", 3, mLastFunc);
      CPPUNIT_ASSERT_MESSAGE("The Current State should be STATE_DEBRIEF", mGameStateComponent->GetCurrentState() == &ExtendedStateType::STATE_DEBRIEF);
      CPPUNIT_ASSERT(mGameStateComponent->IsInState(ExtendedStateType::STATE_DEBRIEF));

      mGameStateComponent->DoStateTransition(&EventType::TRANSITION_OCCURRED);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the entry functor to STATE_RUNNING_EXIT_PROMPT, thus setting the mCurrFunc to 5", 5, mCurrFunc);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("This Transition should have fired the exit functor of STATE_DEBRIEF, thus setting the mLastFunc to 4", 4, mLastFunc);
      CPPUNIT_ASSERT_MESSAGE("The Current State should be STATE_RUNNING_EXIT_PROMPT", mGameStateComponent->GetCurrentState() == &ExtendedStateType::STATE_RUNNING_EXIT_PROMPT);
      CPPUNIT_ASSERT(mGameStateComponent->IsInState(ExtendedStateType::STATE_RUNNING_EXIT_PROMPT));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GameStateTests::TestTransitionFunc(int i, bool b)
   {
      if(b)
         mCurrFunc = i;
      else
         mLastFunc = i;
   }

} // END - namespace dtGame
