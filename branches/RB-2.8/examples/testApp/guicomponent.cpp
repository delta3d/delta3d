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
#include "testappgamestates.h"
#include <dtABC/application.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtCore/deltawin.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gamestatechangedmessage.h>
#include <dtGame/messagetype.h>
#include <dtGame/taskcomponent.h>
#include <dtActors/taskactor.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUIFont.h>



namespace dtExample
{
   ///////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ///////////////////////////////////////////////////////////////////////
   GuiComponent::GuiComponent()
      : dtGame::GMComponent("GuiComponent")
      , mMainWindow(NULL)
      , mGUI(NULL)
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
   void GuiComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         const dtGame::TickMessage* tickMessage
            = dynamic_cast<const dtGame::TickMessage*>(&message);

         Update(tickMessage->GetDeltaSimTime(), tickMessage->GetDeltaRealTime());
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_GAME_STATE_CHANGED)
      {
         const dtGame::GameStateChangedMessage* stateMessage
            = dynamic_cast<const dtGame::GameStateChangedMessage*>(&message);

         HandleGameStateChanged(stateMessage->GetNewState());
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
      {
         mMainWindow->setVisible(true);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::HandleGameStateChanged(const dtGame::GameStateType& gameState)
   {
      LOG_ALWAYS("GuiComponent switching screen for game state: " + gameState.GetName());

      GameStateScreenMap::iterator foundIter = mScreens.find(gameState.GetName());
      
      if (mCurrentScreen.valid())
      {
         LOG_ALWAYS("\tScreen hiding: " + mCurrentScreen->GetName());

         mCurrentScreen->SetVisible(false);
      }

      GuiScreen* nextScreen = NULL;
      if (foundIter != mScreens.end())
      {
         nextScreen = foundIter->second.get();
      }

      mCurrentScreen = nextScreen;

      if (mCurrentScreen.valid())
      {
         LOG_ALWAYS("\tScreen showing: " + mCurrentScreen->GetName());

         mCurrentScreen->SetVisible(true);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::SetupGUI(dtCore::Camera& cam, dtCore::Keyboard& keyboard, dtCore::Mouse& mouse)
   {
      try
      {
         // Initialize CEGUI
         mGUI = new dtGUI::GUI(&cam, &keyboard, &mouse);

         mGUI->LoadScheme("testapp.scheme");

         CEGUI::System::getSingleton().getDefaultFont()->setProperty("PointSize", "14");

         mMainWindow = mGUI->GetRootSheet();

         // Create the screen objects.
         mScreens[TestAppGameState::STATE_GAME.GetName()]
            = new GuiScreen("Game", "game.layout");
         mScreens[TestAppGameState::STATE_HELP.GetName()]
            = new GuiScreen("Help", "help.layout");

         // Setup and hide all screens by default.
         GuiScreen * curScreen = NULL;
         GameStateScreenMap::iterator curIter = mScreens.begin();
         GameStateScreenMap::iterator endIter = mScreens.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curScreen = curIter->second.get();

            curScreen->Setup(mMainWindow);
            curScreen->SetVisible(false);
         }

         mMainWindow->setVisible(false);
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
      // TODO:
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::UpdateStaticText(CEGUI::Window* textControl, const std::string& newText,
      osg::Vec3 color, float x, float y)
   {
      float red = color.x();
      float green = color.y();
      float blue = color.z();

      if (textControl != NULL)
      {
         // text and color
         if (textControl->getText() != newText)
         {
            textControl->setText(newText);
            if (red >= 0.00 && blue >= 0.0 && green >= 0.0)
            {
               textControl->setProperty("TextColours", 
                  CEGUI::PropertyHelper::colourToString(CEGUI::colour(red, green, blue)));
               // how to do it with a string.  Use "FF00FF00" or "FFFFFFFF" for examples
               //String col = PropertyHelper::colourRectToString(ColourRect(PropertyHelper::stringToColour(String(color))));
               //textControl->setProperty("TextColours", col);         }
            }
         }
         // position
         if (x > 0.0 && y > 0.0)
         {
            CEGUI::UVector2 position = textControl->getPosition();
            CEGUI::UVector2 newPos(cegui_absdim(x), cegui_absdim(y));
            if (position != newPos)
               textControl->setPosition(newPos);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GuiComponent::UpdateStaticText(CEGUI::Window* textControl, const std::string& newText,
      float x, float y)
   {
      osg::Vec3 color(1.0f, 1.0f, 1.0f);
      UpdateStaticText(textControl, newText, color, x, y);
   }

   //////////////////////////////////////////////////////////////////////////
   CEGUI::Window* GuiComponent::CreateText(const std::string& name, CEGUI::Window* parent, const std::string& text,
                                    float x, float y, float width, float height)
   {
      // create base window and set our default attribs
      CEGUI::Window* result = mGUI->CreateWidget(parent, "WindowsLook/StaticText", name);
      result->setText(text);
      result->setPosition(CEGUI::UVector2(cegui_absdim(x), cegui_absdim(y)));
      result->setSize(CEGUI::UVector2(cegui_absdim(width), cegui_absdim(height)));
      result->setProperty("FrameEnabled", "false");
      result->setProperty("BackgroundEnabled", "false");
      result->setHorizontalAlignment(CEGUI::HA_LEFT);
      result->setVerticalAlignment(CEGUI::VA_TOP);
      // set default color to white
      result->setProperty("TextColours", 
         CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 1.0f, 1.0f)));
      result->show();

      return result;
   }

} // END - namsepace dtExample
