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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "guicomponent.h"
#include "guilistitem.h"
#include "inputcomponent.h"
#include "testappactorregistry.h"
#include "testappconstants.h"
#include "testappgamestates.h"
#include "testappmessages.h"
#include "testappmessagetypes.h"
#include "testapputils.h"

#include <dtABC/application.h>
#include <dtActors/engineactorregistry.h>
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
   const dtUtil::RefString GuiComponent::CHECKBOX_TYPE("WindowsLook/Checkbox");
   const dtUtil::RefString GuiComponent::COMBOBOX_TYPE("WindowsLook/Combobox");
   const dtUtil::RefString GuiComponent::SPINNER_TYPE("WindowsLook/Spinner");
   const dtUtil::RefString GuiComponent::BUTTON_TYPE("WindowsLook/Button");
   const dtUtil::RefString GuiComponent::TESTAPP_BUTTON_TYPE("TestApp/Button");
   const dtUtil::RefString GuiComponent::TESTAPP_BUTTON_PROPERTY_ACTION("Action");
   const dtUtil::RefString GuiComponent::TESTAPP_BUTTON_PROPERTY_TYPE("ButtonType");

   const dtUtil::RefString UI_BACKGROUND("GlobalOverlay_Background");
   const dtUtil::RefString UI_TEXT_MOTION_MODEL("GameScreen_MotionModelType");
   const dtUtil::RefString UI_TEXT_STATUS("GlobalOverlay_Status");

   static const dtUtil::RefString UI_CONTROL_AMBIENCE("GameScreen_AmbienceSpinner");
   static const dtUtil::RefString UI_CONTROL_LAMP("GameScreen_LampSpinner");
   static const dtUtil::RefString UI_CONTROL_LUMINANCE("GameScreen_LuminanceSpinner");
   static const dtUtil::RefString UI_CONTROL_SEA_CHOP("GameScreen_SeaChopSpinner");
   static const dtUtil::RefString UI_CONTROL_SEA_STATE("GameScreen_SeaStateSpinner");
   static const dtUtil::RefString UI_CONTROL_TIME("GameScreen_TimeSpinner");



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

      // Create the factories for custom controls.
      GuiListItem::BindFactory();
   }

   //////////////////////////////////////////////////////////////////////////
   GuiNode* GuiComponent::GetUI(const std::string& uiName)
   {
      GuiNode* guiNode = NULL;

      if (uiName == UINames::UI_HELP)
      {
         guiNode = mHelpOverlay->GetRoot();
      }
      else
      {
         guiNode = mCurrentScreen->GetNode(uiName);
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
         const UIMessage* uiMessage = dynamic_cast<const UIMessage*>(&message);
         HandleUIMessage(*uiMessage);
      }
      else if (msgType == TestAppMessageType::MOTION_MODEL_CHANGED)
      {
         const MotionModelChangedMessage* mmcMessage
            = dynamic_cast<const MotionModelChangedMessage*>(&message);

         HandleMotionModelChanged(mmcMessage->GetNewMotionModelType());
      }
      else if (msgType == dtGame::MessageType::INFO_GAME_STATE_CHANGED)
      {
         const dtGame::GameStateChangedMessage* stateMessage
            = dynamic_cast<const dtGame::GameStateChangedMessage*>(&message);

         HandleGameStateChanged(stateMessage->GetNewState());
      }
      else if (msgType == dtGame::MessageType::INFO_MAP_LOADED)
      {
         dtGame::GameStateComponent* gameStateComp = NULL;
         GetGameManager()->GetComponentByName(dtGame::GameStateComponent::DEFAULT_NAME, gameStateComp);

         gameStateComp->DoStateTransition(&dtGame::EventType::TRANSITION_FORWARD);
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

      bool paused = gameState.IsPausedState();//GetGameManager()->IsPaused();
      std::string statusText(paused?"Paused":"");
      mGlobalOverlay->SetText(UI_TEXT_STATUS.Get(), statusText);
      mGlobalOverlay->SetVisible(UI_BACKGROUND.Get(), paused);

      if (&gameState == &TestAppGameState::STATE_GAME)
      {
         UpdateUIValues(*mCurrentScreen);
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
   void GuiComponent::HandleMotionModelChanged(const dtExample::MotionModelType& motionModelType)
   {
      std::string text(motionModelType.GetName() + " Motion Model");
      mGameScreen->SetText(UI_TEXT_MOTION_MODEL.Get(), text);
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

         // Create screens/overlays that are not tied to specific states.
         mGlobalOverlay = new GuiScreen(*mGUIScene, "Global Overlay", guiDir + "globaloverlay.layout");
         mGlobalOverlay->Setup();

         mHelpOverlay = new GuiScreen(*mGUIScene, dtExample::UINames::UI_HELP, guiDir + "help.layout");
         mHelpOverlay->Setup();

         // Create screen objects associated with specific game states.
         dtCore::RefPtr<GuiScreen> screen = new GuiScreen(*mGUIScene, "Loading Screen", guiDir + "loadingscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_LOADING);

         screen = new GuiScreen(*mGUIScene, "Title Screen", guiDir + "titlescreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_TITLE);

         screen = new GuiScreen(*mGUIScene, "Menu", guiDir + "menuscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_MENU);

         mGameScreen = new GuiScreen(*mGUIScene, "GameScreen", guiDir + "gamescreen.layout");
         mGameScreen->Setup();
         RegisterScreenWithState(*mGameScreen, TestAppGameState::STATE_GAME);

         screen = new GuiScreen(*mGUIScene, "Game Options", guiDir + "gameoptionsscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_GAME_OPTIONS);

         screen = new GuiScreen(*mGUIScene, "Game Quit", guiDir + "gamequitscreen.layout");
         screen->Setup();
         RegisterScreenWithState(*screen, TestAppGameState::STATE_GAME_QUIT);

         // Special setup for the control panel.
         // Some motion models continue to operate even though the UI
         // may consume inputs which should not be processed by motion models.
         // As a work around, the control panel needs to disable motion models
         // when the mouse cursor enters its frame. Certain callback methods
         // need to be registered with events from the control panel regarding
         // the proximity of the mouse cursor.
         GuiNode* cp = mGameScreen->GetNode("GameScreen_ControlPanel");
         cp->subscribeEvent(GuiNode::EventMouseEntersArea,
            CEGUI::Event::Subscriber(&GuiComponent::OnControlPanelFocusGained, this));
         cp->subscribeEvent(GuiNode::EventMouseLeavesArea,
            CEGUI::Event::Subscriber(&GuiComponent::OnControlPanelFocusLost, this));

         // Hide all screens by default.
         GuiScreen * curScreen = NULL;
         GameStateScreenMap::iterator curIter = mScreens.begin();
         GameStateScreenMap::iterator endIter = mScreens.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curScreen = curIter->second.get();

            BindControls(*curScreen->GetRoot());

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
   void GuiComponent::UpdateUIValues(GuiScreen& screen)
   {
      // Update the Attachable Actors list
      UpdateActorList(screen);


      // Update the Sea State spinner.
      InputComponent* comp = GetInputComponent();
      
      GuiSpinner* spinner = dynamic_cast<GuiSpinner*>(screen.GetNode(UI_CONTROL_LUMINANCE.Get()));
      spinner->setCurrentValue(comp->GetLuminance());
      
      spinner = dynamic_cast<GuiSpinner*>(screen.GetNode(UI_CONTROL_AMBIENCE.Get()));
      spinner->setCurrentValue(comp->GetAmbience());

      typedef dtActors::WaterGridActor::SeaState SeaState;
      const SeaState& state = comp->GetSeaState();

      typedef std::vector<dtUtil::Enumeration*> EnumList;
      const EnumList& seaStates = SeaState::Enumerate();

      int index = 0;
      {
         EnumList::const_iterator curIter = seaStates.begin();
         EnumList::const_iterator endIter = seaStates.end();
         for (int i = 0; curIter != endIter; ++curIter, ++i)
         {
            if ((*curIter) == &state)
            {
               index = i;
               break;
            }
         }
      }

      spinner = dynamic_cast<GuiSpinner*>(screen.GetNode(UI_CONTROL_SEA_STATE.Get()));
      spinner->setCurrentValue(index);


      // Update the Sea Chop combobox
      typedef dtActors::WaterGridActor::ChopSettings Chop;
      const EnumList& choppies = Chop::Enumerate();

      Chop& chop = comp->GetWaterChop();

      index = 0;
      {
         EnumList::const_iterator curIter = choppies.begin();
         EnumList::const_iterator endIter = choppies.end();
         for (int i = 0; curIter != endIter; ++curIter, ++i)
         {
            if ((*curIter) == &chop)
            {
               index = i;
               break;
            }
         }
      }

      spinner = dynamic_cast<GuiSpinner*>(screen.GetNode(UI_CONTROL_SEA_CHOP.Get()));
      spinner->setCurrentValue(index);
   }

   //////////////////////////////////////////////////////////////////////////
   // HELPER CLASS
   struct SortActorPred
   {
      bool operator() (dtCore::BaseActorObject* a, dtCore::BaseActorObject* b) const
      {
         return strcmp(a->GetName().c_str(), b->GetName().c_str()) < 0;
      }
   };

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::UpdateActorList(GuiScreen& screen)
   {
      TestAppUtils util;

      std::string listboxName(screen.GetName());
      listboxName += "_ActorList";

      GuiListbox* listbox = screen.GetListbox(listboxName);

      if (listbox != NULL)
      {
         CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

         // Clear the current listing.
         listbox->resetList();

         typedef dtCore::ActorPtrVector ActorList;
         ActorList actors;
         GetGameManager()->GetAllActors(actors);

         SortActorPred pred;
         std::sort(actors.begin(), actors.end(), pred);

         dtCore::BaseActorObject* curActor = NULL;
         ActorList::iterator curIter = actors.begin();
         ActorList::iterator endIter = actors.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curActor = *curIter;

            if ( ! util.IsAttachableActor(*curActor))
            {
               // Skip this actor and go to the next one.
               continue;
            }

            CEGUI::String itemName(curActor->GetId().ToString().c_str());
            CEGUI::String actorName(curActor->GetName().c_str());
            CEGUI::Window* item = wm->createWindow("WindowsLook/StaticText", itemName);

            item->setText(actorName);
            item->setProperty("Font", "DejaVuSans-10");
            item->setProperty("BackgroundEnabled", "False");
            item->setProperty("FrameEnabled", "False");
            item->subscribeEvent( CEGUI::Window::EventMouseClick,
               CEGUI::Event::Subscriber(&GuiComponent::OnListItemClicked, this));

            if (screen.AddListItem(*listbox, *item))
            {
               // DEBUG:
               printf("\n\tItem for actor: %s\n\n", actorName.c_str());
            }
            else
            {
               LOG_ERROR("Could not add item for actor: " + std::string(actorName.c_str()));
            }
         }

         listbox->layoutItemWidgets();

         // DEBUG:
         printf("\n\tActor list count: %zu\n\n", listbox->getItemCount());
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
   void GuiComponent::BindControls(GuiNode& rootWindow)
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
         if(BUTTON_TYPE == guiType
            || TESTAPP_BUTTON_TYPE == guiType)
         {
            button = dynamic_cast<GuiButton*>(curChild);

            BindButton( *button );
         }
         else if(SPINNER_TYPE == guiType)
         {
            GuiSpinner* spinner = dynamic_cast<GuiSpinner*>(curChild);
            
            BindSpinner(*spinner);
         }
         else if (CHECKBOX_TYPE == guiType)
         {
            GuiCheckbox* checkbox = dynamic_cast<GuiCheckbox*>(curChild);
            
            BindCheckbox(*checkbox);
         }
         else if (COMBOBOX_TYPE == guiType)
         {
            GuiCombobox* checkbox = dynamic_cast<GuiCombobox*>(curChild);
            
            BindCombobox(*checkbox);
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
   void GuiComponent::BindCheckbox(GuiCheckbox& checkbox)
   {
      checkbox.subscribeEvent(GuiCheckbox::EventCheckStateChanged,
         CEGUI::Event::Subscriber(&GuiComponent::OnCheckboxChanged, this));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GuiComponent::BindCombobox(GuiCombobox& combobox)
   {
      combobox.subscribeEvent(GuiCombobox::EventListSelectionChanged,
         CEGUI::Event::Subscriber(&GuiComponent::OnComboboxChanged, this));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GuiComponent::BindSpinner(GuiSpinner& spinner)
   {
      spinner.subscribeEvent(GuiSpinner::EventValueChanged,
         CEGUI::Event::Subscriber(&GuiComponent::OnSpinnerChanged, this));
   }

   /////////////////////////////////////////////////////////////////////////////
   const GuiNode* GuiComponent::GetWidgetFromEventArgs(const GuiEventArgs& args) const
   {
      // Cast to WindowEventArgs in order to access the associated CEGUI window.
      const CEGUI::WindowEventArgs& winArgs
         = static_cast<const CEGUI::WindowEventArgs&>(args);

      return winArgs.window;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnControlPanelFocusGained(const GuiEventArgs& args)
   {
      GetInputComponent()->SetMotionModelEnabled(false);
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnControlPanelFocusLost(const GuiEventArgs& args)
   {
      GetInputComponent()->SetMotionModelEnabled(true);
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnButtonClicked(const GuiEventArgs& args)
   {
      const GuiNode* button = GetWidgetFromEventArgs(args);

      if(button != NULL)
      {
         std::string name(button->getName().c_str());

         InputComponent* inputComp = GetInputComponent();
         if (name == "GameScreen_ToggleFireworks")
         {
            inputComp->ToggleFireworks();
         }
         else if (name == "GameScreen_ToggleAIWaypoints")
         {
            inputComp->ToggleAIWaypointDrawMode();
         }
         else if (name == "GameScreen_TogglePhysicsDraw")
         {
            inputComp->TogglePhysicsDrawMode();
         }
         else if (name == "GameScreen_ReloadShaders")
         {
            inputComp->ReloadShaders();
         }
         else
         {
            HandleButton(*button);
         }
      }

      // Let CEGUI know the button has been handled.
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnListItemClicked(const GuiEventArgs& args)
   {
      const GuiNode* item = GetWidgetFromEventArgs(args);

      if (item != NULL)
      {
         //GuiNode* content = NULL;
         dtCore::UniqueId actorID(item->getName().c_str());

         // DEBUG:
         printf("\n\tClicked item: %s\n\n", actorID.ToString().c_str());

         if ( ! actorID.ToString().empty())
         {
            SendRequestAttachMessage(actorID);
         }
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnCheckboxChanged(const GuiEventArgs& args)
   {
      const GuiCheckbox* checkbox = dynamic_cast<const GuiCheckbox*>(GetWidgetFromEventArgs(args));

      if (checkbox != NULL)
      {
         std::string controlName(checkbox->getName().c_str());
         //bool value = checkbox->isSelected();

         // TODO:
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnComboboxChanged(const GuiEventArgs& args)
   {
      const GuiCombobox* combobox = dynamic_cast<const GuiCombobox*>(GetWidgetFromEventArgs(args));

      if (combobox != NULL)
      {
         std::string controlName(combobox->getName().c_str());
         std::string value;

         CEGUI::ListboxItem* item = combobox->getSelectedItem();
         if (item != NULL)
         {
            value = item->getText().c_str();
         }

         // TODO:
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GuiComponent::OnSpinnerChanged(const GuiEventArgs& args)
   {
      const GuiSpinner* spinner = dynamic_cast<const GuiSpinner*>(GetWidgetFromEventArgs(args));

      typedef std::vector<dtUtil::Enumeration*> EnumList;

      if (spinner != NULL)
      {
         std::string controlName(spinner->getName().c_str());
         double value = spinner->getCurrentValue();

         InputComponent* comp = GetInputComponent();

         if (controlName == UI_CONTROL_TIME.Get())
         {
            SendRequestTimeOffsetMessage(value);
         }
         else if (controlName == UI_CONTROL_LAMP.Get())
         {
            comp->SetLampIntensity(value);
         }
         else if (controlName == UI_CONTROL_LUMINANCE.Get())
         {
            comp->SetLuminance(value);
         }
         else if (controlName == UI_CONTROL_AMBIENCE.Get())
         {
            comp->SetAmbience(value);
         }
         else if (controlName == UI_CONTROL_SEA_STATE.Get())
         {
            size_t index = (size_t)value;

            typedef dtActors::WaterGridActor::SeaState SeaState;
            const EnumList& seaStates = SeaState::Enumerate();

            SeaState* state = NULL;
            if (seaStates.size() > index)
            {
               state = dynamic_cast<SeaState*>(seaStates[index]);
            }

            if (state != NULL)
            {
               comp->SetSeaState(*state);
            }
         }
         else if (controlName == UI_CONTROL_SEA_CHOP.Get())
         {
            size_t index = (size_t)value;

            typedef dtActors::WaterGridActor::ChopSettings Chop;
            const EnumList& choppies = Chop::Enumerate();

            Chop* chop = NULL;
            if (choppies.size() > index)
            {
               chop = dynamic_cast<Chop*>(choppies[index]);
            }

            if (chop != NULL)
            {
               comp->SetWaterChop(*chop);
            }
         }
      }

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
         // NOTE: "Action" and "ButtonType" properties are not inherent to CEGUI
         // and were added as custom properties to the testapp.looknfeel file
         if (button.isPropertyPresent(TESTAPP_BUTTON_PROPERTY_ACTION.Get()))
         {
            CEGUI::String actionValue = button.getProperty(GuiComponent::TESTAPP_BUTTON_PROPERTY_ACTION.Get());
            CEGUI::String buttonTypeValue = button.getProperty(GuiComponent::TESTAPP_BUTTON_PROPERTY_TYPE.Get());

            action = std::string(actionValue.c_str());
            buttonType = std::string(buttonTypeValue.c_str());
         }
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

   ////////////////////////////////////////////////////////////////////////
   void GuiComponent::SendRequestTimeOffsetMessage(float offset)
   {
      dtGame::GameManager* gm = GetGameManager();
      dtGame::MessageFactory& factory = gm->GetMessageFactory();
      
      dtCore::RefPtr<dtExample::RequestTimeOffsetMessage> message;
      factory.CreateMessage(dtExample::TestAppMessageType::REQUEST_TIME_OFFSET, message);
      
      message->SetOffset(offset);

      gm->SendMessage(*message);
   }

   ////////////////////////////////////////////////////////////////////////
   void GuiComponent::SendRequestAttachMessage(const dtCore::UniqueId& actorId)
   {
      dtGame::GameManager* gm = GetGameManager();
      dtGame::MessageFactory& factory = gm->GetMessageFactory();
      
      dtCore::RefPtr<dtExample::RequestAttachMessage> message;
      factory.CreateMessage(dtExample::TestAppMessageType::REQUEST_ATTACH, message);
      
      message->SetActorId(actorId);

      gm->SendMessage(*message);
   }

   ////////////////////////////////////////////////////////////////////////
   InputComponent* GuiComponent::GetInputComponent()
   {
      InputComponent* inputComp = NULL;
      GetGameManager()->GetComponentByName(InputComponent::DEFAULT_NAME, inputComp);
      return inputComp;
   }

} // END - namsepace dtExample
