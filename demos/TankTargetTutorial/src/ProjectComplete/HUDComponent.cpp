/* -*-c++-*-
 * TutorialLibrary - This source file (.h & .cpp) - Using 'The MIT License'
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
 * @author Curtiss Murphy
 */

#include "HUDComponent.h"
#include <dtABC/baseabc.h>

#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/logcontroller.h>

#include <dtGUI/gui.h>

#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUIExceptions.h>
//////////////////////////////////////////////////////////////////////////
HUDComponent::HUDComponent(dtABC::BaseABC& app,
                           const std::string& name)
   : dtGame::GMComponent(name)
   , mUnHandledMessages(0)
{
   SetupGUI(app);
}

//////////////////////////////////////////////////////////////////////////
HUDComponent::~HUDComponent()
{
}

//////////////////////////////////////////////////////////////////////////
void HUDComponent::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
   {
      UpdateSimTime(GetGameManager()->GetSimulationTime());

      // Update AAR text
      if (!mLogController.valid())
      {
         UpdateStaticText(mStateText, "", 1.0f, 1.0f, 1.0f);
      }
      else if (dtGame::LogStateEnumeration::LOGGER_STATE_IDLE == mLogController->GetLastKnownStatus().GetStateEnum())
      {
         UpdateStaticText(mStateText, "IDLE", 1.0f, 1.0f, 1.0f);
      }
      else if (dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK == mLogController->GetLastKnownStatus().GetStateEnum())
      {
         UpdateStaticText(mStateText, "PLAYBACK", 0.1f, 0.1f, 1.0f);
      }
      else // if (dtGame::LogStateEnumeration::LOGGER_STATE_RECORD == mLogController->GetLastKnownStatus().GetStateEnum())
      {
         UpdateStaticText(mStateText, "RECORD", 1.0f, 0.1f, 0.1f);
      }
   }
   else if (message.GetMessageType() == dtGame::MessageType::TICK_REMOTE)
   {
      // Do nothing when we get remote tick
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_RESTARTED)
   {
      mLogController = dynamic_cast<dtGame::LogController*> (GetGameManager()->
         GetComponentByName("LogController"));
   }
   // sum up all the unhandled messages
   else
   {
      mUnHandledMessages++;
      UpdateNumMessages(mUnHandledMessages);

      UpdateLastMessageName(message.GetMessageType().GetName());
   }
}

//////////////////////////////////////////////////////////////////////////
void HUDComponent::UpdateSimTime(double newTime)
{
   char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print

   // Sim Time
   snprintf(clin, HUDCONTROLMAXTEXTSIZE, "SimTime: %.2f", newTime);
   UpdateStaticText(mSimTimeText, clin);
}

//////////////////////////////////////////////////////////////////////////
void HUDComponent::UpdateNumMessages(int messageCount)
{
   char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print

   // Num Messages
   snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Num Msgs: %lu", mUnHandledMessages);
   UpdateStaticText(mNumMessagesText, clin);
}

//////////////////////////////////////////////////////////////////////////
void HUDComponent::UpdateLastMessageName(const std::string& messageName)
{
   char clin[HUDCONTROLMAXTEXTSIZE]; // general buffer to print

   // Last Message
   snprintf(clin, HUDCONTROLMAXTEXTSIZE, "Last Msg: %s", messageName.c_str());
   UpdateStaticText(mLastMessageText, clin);
}


//////////////////////////////////////////////////////////////////////////
void HUDComponent::SetupGUI(dtABC::BaseABC& app)
{
   try
   {
      // Initialize CEGUI
      mGUI = new dtGUI::GUI(app.GetCamera(), app.GetKeyboard(), app.GetMouse());

      // BEGIN - MAKE THIS PART OF BaseHUDComponent ???
      // probably have params for the scheme, the default font, and main win name

      // get our scheme path
      mGUI->LoadScheme("WindowsLook.scheme");

      mMainWindow = mGUI->GetRootSheet();

      // END - MAKE THIS PART OF BaseHUDComponent ???


      // main HUD window for drawing, covers full window size
      mOverlay = static_cast<CEGUI::Window*>(
         mGUI->CreateWidget(mMainWindow, "WindowsLook/StaticImage", "Main Overlay"));
      mOverlay->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0)));
      mOverlay->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
      mOverlay->setProperty("FrameEnabled", "false");
      mOverlay->setProperty("BackgroundEnabled", "false");

      // Sim Time
      mSimTimeText = CreateText("Sim Time", mOverlay, "",
         5.0f, 20.0f, 600.0f, 60.0f);
      mSimTimeText->setProperty("TextColours",
         CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 1.0f, 1.0f)));

      // Num Messages
      mNumMessagesText = CreateText("Num Messages", mOverlay, "",
         5.0f, 45.0f, 600.0f, 60.0f);
      mNumMessagesText->setProperty("TextColours",
         CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 1.0f, 1.0f)));

      // Last Message
      mLastMessageText = CreateText("Last Message", mOverlay, "Last Msg:",
         5.0f, 70.0f, 600.0f, 60.0f);
      mLastMessageText->setProperty("TextColours",
         CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 1.0f, 1.0f)));

      // AAR playback/idle state
      mStateText = CreateText("State Text", mOverlay, "",
         5.0f, 100.0f, 600.0f, 60.0f);
      mStateText->setProperty("TextColours",
         CEGUI::PropertyHelper::colourToString(CEGUI::colour(1.0f, 0.1f, 0.1f)));
      //mStateText->setFont("Tahoma-14");
   }
   catch(CEGUI::Exception& e)
   {
      std::ostringstream oss;
      oss << "CEGUI while setting up GUI: " << e.getMessage().c_str();
      throw dtGame::GeneralGameManagerException(
         oss.str(), __FILE__, __LINE__);
   }

}

//////////////////////////////////////////////////////////////////////////
void HUDComponent::UpdateStaticText(CEGUI::Window* textControl, const char* newText,
                                  float red, float blue, float green, float x, float y)
{
   if (textControl != NULL)
   {
      // text and color
      if (newText != NULL && textControl->getText() != std::string(newText))
      {
         textControl->setText(newText);
         if (red >= 0.00 && blue >= 0.0 && green >= 0.0)
         {
            textControl->setProperty("TextColours",
               CEGUI::PropertyHelper::colourToString(CEGUI::colour(red, green, blue)));
         }
      }
      // position
      if (x > 0.0 && y > 0.0)
      {
         CEGUI::UVector2 position = textControl->getPosition();
         CEGUI::UVector2 newPos(cegui_absdim(x), cegui_absdim(y));
         if (position != newPos)
         {
            textControl->setPosition(newPos);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
CEGUI::Window* HUDComponent::CreateText(const std::string& name, CEGUI::Window* parent, const std::string& text,
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
   result->show();

   return result;
}

