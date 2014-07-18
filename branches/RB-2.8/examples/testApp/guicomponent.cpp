/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "guicomponent.h"
#include "testappconstants.h"
#include "testappgamestates.h"
#include "testappmessages.h"
#include "testappmessagetypes.h"

#include <dtABC/application.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtCore/deltawin.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gamestatecomponent.h>
#include <dtGame/gamestatemessages.h>
#include <dtGame/messagetype.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIFont.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/elements/CEGUIPushButton.h>



namespace dtExample
{
   ///////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ///////////////////////////////////////////////////////////////////////
   const dtUtil::RefString GuiComponent::BUTTON_TYPE("WindowsLook/Button");
   const dtUtil::RefString GuiComponent::BUTTON_PROPERTY_ACTION("Action");
   const dtUtil::RefString GuiComponent::BUTTON_PROPERTY_TYPE("ButtonType");



   ///////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ///////////////////////////////////////////////////////////////////////
   GuiComponent::GuiComponent()
      : dtGame::GMComponent("GuiComponent")
      , mGUIScene(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   GuiComponent::~GuiComponent()
   {
   }

   /////////////////////////////////////////////////////////////////////////
   void GuiComponent::OnAddedToGM()
   {
      dtABC::BaseABC* app = &GetGameManager()->GetApplication();
      mWindow = app->GetWindow();
      SetupGUI(*app->GetCamera(), *app->GetKeyboard(), *app->GetMouse());
   }

   //////////////////////////////////////////////////////////////////////////
   GuiNode* GuiComponent::GetUI(const std::string& uiName)
   {
      GuiNode* guiNode = NULL;

      if (uiName == UINames::UI_HELP)
      {
         guiNode = mHelpOverlay->GetRoot();
      }

      return guiNode;
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::ProcessMessage(const dtGame::Message& message)
   {
      const dtGame::MessageType& msgType = message.GetMessageType();

      if (msgType == dtGame::MessageType::TICK_LOCAL)
      {
         const dtGame::TickMessage* tickMessage
            = dynamic_cast<const dtGame::TickMessage*>(&message);

         Update(tickMessage->GetDeltaSimTime(), tickMessage->GetDeltaRealTime());
      }
      else if (msgType == TestAppMessageType::UI_HIDE
         || msgType == TestAppMessageType::UI_SHOW
         || msgType == TestAppMessageType::UI_TOGGLE)
      {
         HandleUIMessage(static_cast<const UIMessage&>(message));
      }
      else if (msgType == dtGame::MessageType::INFO_GAME_STATE_CHANGED)
      {
         const dtGame::GameStateChangedMessage* stateMessage
            = dynamic_cast<const dtGame::GameStateChangedMessage*>(&message);

         HandleGameStateChanged(stateMessage->GetNewState());
      }
      else if (msgType == dtGame::MessageType::INFO_MAP_LOADED)
      {
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::HandleGameStateChanged(const dtGame::GameStateType& gameState)
   {
      // DEBUG:
      LOG_ALWAYS("GuiComponent switching screen for game state: " + gameState.GetName());

      GameStateScreenMap::iterator foundIter = mScreens.find(&gameState);
      
      if (mCurrentScreen.valid())
      {
         mCurrentScreen->OnExit();
      }

      GuiScreen* nextScreen = NULL;
      if (foundIter != mScreens.end())
      {
         nextScreen = foundIter->second.get();
      }

      mCurrentScreen = nextScreen;

      if (mCurrentScreen.valid())
      {
         mCurrentScreen->OnEnter();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::HandleUIMessage(const dtExample::UIMessage& uiMessage)
   {
      const dtGame::MessageType& msgType = uiMessage.GetMessageType();

      GuiNode* ui = GetUI(uiMessage.GetUIName());

      if (ui != NULL)
      {
         if (msgType == TestAppMessageType::UI_HIDE)
         {
            ui->setVisible(false);
         }
         else if (msgType == TestAppMessageType::UI_SHOW)
         {
            ui->setVisible(true);
         }
         else if (msgType == TestAppMessageType::UI_TOGGLE)
         {
            ui->setVisible( ! ui->isVisible());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::SetupGUI(dtCore::Camera& cam, dtCore::Keyboard& keyboard, dtCore::Mouse& mouse)
   {
      try
      {
         const std::string guiDir = "gui/layouts/testapp/";
         // Initialize CEGUI
         mGUIScene = new dtGUI::GUI(&cam, &keyboard, &mouse);

         mGUIScene->LoadScheme("testapp.scheme");

         CEGUI::System::getSingleton().getDefaultFont()->setProperty("PointSize", "14");

         // Create screen objects associated with specific game states.
         dtCore::RefPtr<GuiScreen> screen = new GuiScreen(*mGUIScene, "Title Screen", guiDir + "titlescreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_TITLE);

         screen = new GuiScreen(*mGUIScene, "Menu", guiDir + "menuscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_MENU);

         screen = new GuiScreen(*mGUIScene, "Game", guiDir + "gamescreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_GAME);

         screen = new GuiScreen(*mGUIScene, "Game Options", guiDir + "gameoptionsscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_GAME_OPTIONS);

         screen = new GuiScreen(*mGUIScene, "Game Quit", guiDir + "gamequitscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_GAME_QUIT);

         // Create screens/overlays that are not tied to specific states.
         mHelpOverlay = new GuiScreen(*mGUIScene, dtExample::UINames::UI_HELP, guiDir + "help.layout");
         mHelpOverlay->Setup();

         mGlobalOverlay = new GuiScreen(*mGUIScene, "Global Overlay", guiDir + "globaloverlay.layout");
         mGlobalOverlay->Setup();

         // Hide all screens by default.
         GuiScreen * curScreen = NULL;
         GameStateScreenMap::iterator curIter = mScreens.begin();
         GameStateScreenMap::iterator endIter = mScreens.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curScreen = curIter->second.get();

            BindButtons(*curScreen->GetRoot());

            curScreen->SetVisible(false);
         }

         // Ensure the that the first screen enabled is one
         // relevant to the initial game state.
         dtGame::GameStateComponent* gameStateComp
            = dynamic_cast<dtGame::GameStateComponent*>
            (GetGameManager()->GetComponentByName(dtGame::GameStateComponent::DEFAULT_NAME));
         
         if (gameStateComp != NULL)
         {
            const dtGame::StateType* gameState = gameStateComp->GetCurrentState();
            if (gameState != NULL)
            {
               HandleGameStateChanged(*gameState);
            }
            else
            {
               LOG_ERROR("GameStateComponent returned a NULL current state.");
            }
         }
         else
         {
            LOG_ERROR("Could not access the GameStateComponent to determine initial game state.");
         }
      }
      catch (CEGUI::Exception& e)
      {
         std::ostringstream oss;
         oss << "CEGUI while setting up GUI: " << e.getMessage().c_str();
         throw dtGame::GameApplicationConfigException(oss.str(), __FILE__, __LINE__);
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::Update(float simTimeDelta, float realTimeDelta)
   {
      if (mCurrentScreen.valid())
      {
         mCurrentScreen->Update(realTimeDelta);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool GuiComponent::RegisterScreenWithState(GuiScreen& screen, const dtGame::GameStateType& gameStateType)
   {
      bool success = mScreens.insert(std::make_pair(&gameStateType, &screen)).second;

      /*dtGame::GameStateComponent* gameStateComp = GetGameManager()->GetComponentByName(dtGame::GameStateComponent::DEFAULT_NAME);

      if (gameStateComp != NULL)
      {
         dtGame::GameState* gameState = gameStateComp->GetState(gameStateType);

         if (gameState != NULL)
         {
            success = mScreens.insert(std::make_pair(&gameStateType, &screen)).second;
            
            typedef dtUtil::Functor<void,TYPELIST_0()> VoidFunc;

            // Bind Entry method.
            VoidFunc enterFunc(&screen, &GuiScreen::OnEnter);
            dtCore::RefPtr<dtUtil::Command0<void> > comEnter = new dtUtil::Command0<void>(enterFunc);
            gameState->AddEntryCommand(comEnter.get());

            // Bind Exit method.
            VoidFunc exitFunc(&screen, &GuiScreen::OnExit);
            dtCore::RefPtr<dtUtil::Command0<void> > comExit = new dtUtil::Command0<void>(exitFunc);
            gameState->AddExitCommand(comExit);

            // Bind Update Method
            dtGame::GameState::UpdateFunctor updateFunc(&screen, &GuiScreen::Update);
            gameState->SetUpdate(updateFunc);
         }
      }*/

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   void GuiComponent::BindButtons(GuiNode& rootWindow)
   {
      GuiNode* curChild = NULL;
      GuiButton* button = NULL;
      typedef std::vector<GuiNode*> GuiNodeList;
      GuiNodeList childList;

      // Go through all child windows and bind all buttons.
      childList.push_back( &rootWindow );
      while( ! childList.empty() )
      {
         curChild = childList.back();
         childList.pop_back();

         // If this is a button...
         std::string guiType(curChild->getType().c_str());
         if(BUTTON_TYPE == guiType)
         {
            button = dynamic_cast<GuiButton*>(curChild);

            // ...bind it to this component's callback for handling buttons.
            BindButton( *button );
         }
         // ...else if this is a normal widget window...
         else if( curChild->getChildCount() > 0 )
         {
            size_t numChildren = curChild->getChildCount();
            for( size_t i = 0; i < numChildren; ++i )
            {
               childList.push_back( curChild->getChildAtIdx( i ) );
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void GuiComponent::BindButton(GuiButton& button)
   {
      button.subscribeEvent(GuiButton::EventClicked,
         CEGUI::Event::Subscriber(&GuiComponent::OnButtonClicked, this));
   }

   /////////////////////////////////////////////////////////////////////////////
   const GuiNode* GuiComponent::GetWidgetFromEventArgs( const GuiEventArgs& args ) const
   {
      // Cast to WindowEventArgs in order to access the associated CEGUI window.
      const CEGUI::WindowEventArgs& winArgs
         = static_cast<const CEGUI::WindowEventArgs&>(args);

      return winArgs.window;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnButtonClicked( const GuiEventArgs& args )
   {
      const GuiNode* button = GetWidgetFromEventArgs(args);

      if(button != NULL)
      {
         HandleButton(*button);
      }

      // Let CEGUI know the button has been handled.
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void GuiComponent::HandleButton(const GuiNode& button)
   {
      // Prepare to capture the button action name.
      std::string action;
      std::string buttonType;

      // Attempt to access the button's action property.
      try
      {
         CEGUI::String actionValue = button.getProperty(GuiComponent::BUTTON_PROPERTY_ACTION.Get());
         CEGUI::String buttonTypeValue = button.getProperty(GuiComponent::BUTTON_PROPERTY_TYPE.Get());
         action = std::string(actionValue.c_str());
         buttonType = std::string(buttonTypeValue.c_str());
      }
      catch(CEGUI::Exception& ceguiEx)
      {
         std::ostringstream oss;
         oss << "Button \"" << button.getName().c_str()
            << "\" does not have the \"Action\" property.\n"
            << ceguiEx.getMessage().c_str() << std::endl;
         LOG_ERROR(oss.str());
      }

      // Lookup and send the transition type in a message.
      dtGame::EventType* transition = dtGame::EventType::GetValueForName(action);
      if (transition != NULL)
      {
         dtGame::GameManager* gm = GetGameManager();

         dtCore::RefPtr<dtGame::GameStateTransitionRequestMessage> message;
         dtGame::MessageFactory& factory = gm->GetMessageFactory();
         factory.CreateMessage(dtGame::MessageType::REQUEST_GAME_STATE_TRANSITION, message);

         message->SetTransition(*transition);

         gm->SendMessage(*message);
      }
   }

} // END - namsepace dtExample
