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
 * @author William E. Johnson II
 */

#include <fireFighter/helpwindow.h>
#include <fireFighter/exception.h>

//solves a problem with the prefix headers
#ifdef None
#undef None
#endif

#include <CEGUI/elements/CEGUIFrameWindow.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/CEGUIExceptions.h>
#include <dtGUI/gui.h>
#include <sstream>

HelpWindow::HelpWindow(dtGUI::GUI& gui, CEGUI::Window* mainWindow)
   : mIsEnabled(false)
   , mOverlay(NULL)
   , mHeaderText(NULL)
   , mBinocsText(NULL)
   , mLRFText(NULL)
   , mCompassText(NULL)
   , mGPSText(NULL)
   , mToggleFullScreen(NULL)
   , mMagnifyModels(NULL)
{
   mMainWindow = NULL;
   InitGui(gui, mainWindow);
}

HelpWindow::~HelpWindow()
{
   mOverlay->removeChildWindow(mHeaderText);
   mOverlay->removeChildWindow(mBinocsText);
   mOverlay->removeChildWindow(mLRFText);
   mOverlay->removeChildWindow(mCompassText);
   mOverlay->removeChildWindow(mGPSText);
   mOverlay->removeChildWindow(mToggleFullScreen);
   mOverlay->removeChildWindow(mMagnifyModels);

   if (mMainWindow != NULL)
   {
      mMainWindow->removeChildWindow(mOverlay);
   }

   mHeaderText->destroy();
   mBinocsText->destroy();
   mLRFText->destroy();
   mCompassText->destroy();
   mGPSText->destroy();
   mToggleFullScreen->destroy();
   mMagnifyModels->destroy();
   mOverlay->destroy();
}

void HelpWindow::Enable(bool enable)
{
   mIsEnabled = enable;

   mIsEnabled ? mOverlay->show() : mOverlay->hide();
}

void HelpWindow::InitGui(dtGUI::GUI& gui, CEGUI::Window* mainWindow)
{
   try
   {
      mMainWindow = mainWindow;
      mOverlay = static_cast<CEGUI::FrameWindow*>(gui.CreateWidget(mMainWindow, "WindowsLook/FrameWindow", "help_window"));
      mOverlay->setProperty("AlwaysOnTop", "True");

      mHeaderText       = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "header_helptext");
      mBinocsText       = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "binocs_helptext");
      mLRFText          = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "lrf_helptext");
      mCompassText      = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "compass_helptext");
      mGPSText          = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "gps_helptext");
      mToggleFullScreen = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "fullscreen_helptext");
      mMagnifyModels    = gui.CreateWidget(mOverlay, "WindowsLook/StaticText", "magnifiy_helptext");

      mOverlay->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
      mOverlay->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
      //mOverlay->setFrameEnabled(false);

      mHeaderText->setFont("DejaVuSans-10");
      mHeaderText->setText("Controls");
      mHeaderText->setPosition(CEGUI::UVector2(cegui_reldim(0.33f), cegui_reldim(0.0f)));
      mHeaderText->setSize(CEGUI::UVector2(cegui_reldim(0.8f), cegui_reldim(0.25f)));
      mHeaderText->setProperty("FrameEnabled", "false");
      mHeaderText->setProperty("BackgroundEnabled", "false");
      mHeaderText->setHorizontalAlignment(CEGUI::HA_CENTRE);

      mBinocsText->setFont("DejaVuSans-10");
      mBinocsText->setText("WASD: Move Forward/Backward/Strafe");
      mBinocsText->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.1f)));
      mBinocsText->setSize(CEGUI::UVector2(cegui_reldim(0.8f), cegui_reldim(0.25f)));
      mBinocsText->setProperty("FrameEnabled", "false");
      mBinocsText->setProperty("BackgroundEnabled", "false");
      mBinocsText->setHorizontalAlignment(CEGUI::HA_LEFT);

      mLRFText->setFont("DejaVuSans-10");
      mLRFText->setText("Mouse: Look");
      mLRFText->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.2f)));
      mLRFText->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(0.25f)));
      mLRFText->setProperty("FrameEnabled", "false");
      mLRFText->setProperty("BackgroundEnabled", "false");
      mLRFText->setHorizontalAlignment(CEGUI::HA_LEFT);

      mCompassText->setFont("DejaVuSans-10");
      mCompassText->setText("F: Activate currently viewed item");
      mCompassText->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.3f)));
      mCompassText->setSize(CEGUI::UVector2(cegui_reldim(0.8f), cegui_reldim(0.25f)));
      mCompassText->setProperty("FrameEnabled", "false");
      mCompassText->setProperty("BackgroundEnabled", "false");
      mCompassText->setHorizontalAlignment(CEGUI::HA_LEFT);

      mGPSText->setFont("DejaVuSans-10");
      mGPSText->setText("[]: Change inventory item");
      mGPSText->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.4f)));
      mGPSText->setSize(CEGUI::UVector2(cegui_reldim(0.8f), cegui_reldim(0.25f)));
      mGPSText->setProperty("FrameEnabled", "false");
      mGPSText->setProperty("BackgroundEnabled", "false");
      mGPSText->setHorizontalAlignment(CEGUI::HA_LEFT);

      mToggleFullScreen->setFont("DejaVuSans-10");
      mToggleFullScreen->setText("Left Click: Use current inventory item");
      mToggleFullScreen->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.5f)));
      mToggleFullScreen->setSize(CEGUI::UVector2(cegui_reldim(0.8f), cegui_reldim(0.25f)));
      mToggleFullScreen->setProperty("FrameEnabled", "false");
      mToggleFullScreen->setProperty("BackgroundEnabled", "false");
      mToggleFullScreen->setHorizontalAlignment(CEGUI::HA_LEFT);

      mMagnifyModels->setFont("DejaVuSans-10");
      mMagnifyModels->setText("Esc: Return to menu");
      mMagnifyModels->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim(0.6f)));
      mMagnifyModels->setSize(CEGUI::UVector2(cegui_reldim(0.8f), cegui_reldim(0.25f)));
      mMagnifyModels->setProperty("FrameEnabled", "false");
      mMagnifyModels->setProperty("BackgroundEnabled", "false");
      mMagnifyModels->setHorizontalAlignment(CEGUI::HA_LEFT);

      mCloseButton = static_cast<CEGUI::PushButton*>(gui.CreateWidget(mOverlay, "WindowsLook/Button", "Close Button"));
      mCloseButton->setText("Close");
      mCloseButton->setSize(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim(0.1f)));
      mCloseButton->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.8f)));
      mCloseButton->setMouseCursor(NULL);
      mCloseButton->setHorizontalAlignment(CEGUI::HA_CENTRE);

      mOverlay->setDragMovingEnabled(false);
      mOverlay->setSizingBorderThickness(0);
      mOverlay->setSizingEnabled(false);

      Enable(false);
   }
   catch(const CEGUI::Exception& e)
   {
      std::ostringstream oss;
      oss << "CEGUI exception caught: " << e.getMessage().c_str();
      throw CEGUIException(oss.str(), __FILE__, __LINE__);
   }
}
