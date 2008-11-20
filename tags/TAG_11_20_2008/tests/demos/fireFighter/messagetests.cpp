/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
#include <dtUtil/macros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
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

      bool returnValue = inputComp->HandleKeyPressed(NULL, 'n');
      CPPUNIT_ASSERT_MESSAGE("The current state was not one where the N keypress applied, the state should NOT have changed", 
         inputComp->GetCurrentGameState() == GameState::STATE_MENU);
      CPPUNIT_ASSERT(!returnValue);

      returnValue = inputComp->HandleKeyPressed(NULL, osgGA::GUIEventAdapter::KEY_Escape);
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

      returnValue = inputComp->HandleKeyPressed(NULL, 'n');

      SLEEP(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(returnValue);
      CPPUNIT_ASSERT_MESSAGE("The state should have changed due to the keypress", 
         inputComp->GetCurrentGameState() == GameState::STATE_RUNNING);

      returnValue = inputComp->HandleKeyPressed(NULL, osgGA::GUIEventAdapter::KEY_Escape);
      
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
