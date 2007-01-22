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
 * William E. Johnson II
 */
#include <prefix/dtgameprefix-src.h>
#include <dtABC/application.h>
#include <dtAudio/audiomanager.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtDAL/project.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <fireFighter/messages.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/gamestate.h>
#include <fireFighter/inputcomponent.h>
#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
   #define snprintf _snprintf
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

using dtCore::RefPtr;

class FireFighterMessageTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(FireFighterMessageTests);

      CPPUNIT_TEST(TestGameStateMessages);

   CPPUNIT_TEST_SUITE_END();

   public:
      
      void setUp();
      void tearDown();
      void TestGameStateMessages();

   private:
      
      RefPtr<dtGame::GameManager> mGM;
      RefPtr<dtABC::Application> mApp;
};

CPPUNIT_TEST_SUITE_REGISTRATION(FireFighterMessageTests);

void FireFighterMessageTests::setUp()
{
   dtCore::System::GetInstance().Start();
   mApp = new dtABC::Application;
   mGM = new dtGame::GameManager(*mApp->GetScene());
   mGM->SetApplication(*mApp);
   dtAudio::AudioManager::Instantiate();
   dtAudio::AudioManager::GetInstance().Config();
   mGM->LoadActorRegistry("fireFighter");
   //dtDAL::Project::GetInstance().SetContext("../demos/fireFighter/fireFighterProject");
}

void FireFighterMessageTests::tearDown()
{
   if(mGM.valid())
   {
      mGM->DeleteAllActors(true);
      mGM = NULL;
   }
   mApp = NULL;
   dtAudio::AudioManager::Destroy();
   dtCore::System::GetInstance().Stop();
}

void FireFighterMessageTests::TestGameStateMessages()
{
   try
   {
      RefPtr<InputComponent> inputComp = new InputComponent(InputComponent::NAME);
      RefPtr<dtGame::DefaultMessageProcessor> dmp = new dtGame::DefaultMessageProcessor;
      CPPUNIT_ASSERT(inputComp.valid());
      CPPUNIT_ASSERT(dmp.valid());
      mGM->AddComponent(*inputComp, dtGame::GameManager::ComponentPriority::NORMAL);
      mGM->AddComponent(*dmp, dtGame::GameManager::ComponentPriority::HIGHEST);

      dtGame::MessageFactory &mf = mGM->GetMessageFactory();
      mf.RegisterMessageType<GameStateChangedMessage>(MessageType::GAME_STATE_CHANGED);

      GameState &state = inputComp->GetCurrentGameState();
      CPPUNIT_ASSERT_MESSAGE("The initial game state should be unknown", state == GameState::STATE_UNKNOWN);

      RefPtr<dtGame::Message> msg = mf.CreateMessage(MessageType::GAME_STATE_CHANGED);
      CPPUNIT_ASSERT(msg.valid());

      GameStateChangedMessage *gscm = static_cast<GameStateChangedMessage*>(msg.get());
      gscm->SetOldState(GameState::STATE_UNKNOWN);
      gscm->SetNewState(GameState::STATE_MENU);
      CPPUNIT_ASSERT(gscm->GetOldState() == GameState::STATE_UNKNOWN);
      CPPUNIT_ASSERT(gscm->GetNewState() == GameState::STATE_MENU);
      mGM->SendMessage(*gscm);

      SLEEP(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("The state should have changed to menu", 
         inputComp->GetCurrentGameState() == GameState::STATE_MENU);

      bool returnValue = inputComp->HandleKeyPressed(NULL, Producer::Key_N, Producer::KeyChar_n);
      CPPUNIT_ASSERT_MESSAGE("The current state was not one where the N keypress applied, the state should NOT have changed", 
         inputComp->GetCurrentGameState() == GameState::STATE_MENU);
      CPPUNIT_ASSERT(!returnValue);

      returnValue = inputComp->HandleKeyPressed(NULL, Producer::Key_Escape, Producer::KeyChar_Escape);
      CPPUNIT_ASSERT(!returnValue);
      CPPUNIT_ASSERT_MESSAGE("The current state was not one where the Escape keypress applied, the state shoudl NOT have changed", 
         inputComp->GetCurrentGameState() == GameState::STATE_MENU);

      msg = mf.CreateMessage(MessageType::GAME_STATE_CHANGED);
      CPPUNIT_ASSERT(msg.valid());
      gscm = static_cast<GameStateChangedMessage*>(msg.get());
      gscm->SetOldState(GameState::STATE_MENU);
      gscm->SetNewState(GameState::STATE_INTRO);
      mGM->SendMessage(*gscm);

      SLEEP(10);
      dtCore::System::GetInstance().Step();

      returnValue = inputComp->HandleKeyPressed(NULL, Producer::Key_N, Producer::KeyChar_n);

      SLEEP(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(returnValue);
      CPPUNIT_ASSERT_MESSAGE("The state should have changed due to the keypress", 
         inputComp->GetCurrentGameState() == GameState::STATE_RUNNING);

      returnValue = inputComp->HandleKeyPressed(NULL, Producer::Key_Escape, Producer::KeyChar_Escape);
      
      SLEEP(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(returnValue);
      CPPUNIT_ASSERT_MESSAGE("The state was running, hitting escape should set the state back to the main menu", 
         inputComp->GetCurrentGameState() == GameState::STATE_MENU);
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}