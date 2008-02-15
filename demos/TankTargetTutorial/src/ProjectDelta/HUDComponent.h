/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
* @author Curtiss Murphy
*/
#ifndef __GM_TUTORIAL_HUD_COMPONENT__
#define __GM_TUTORIAL_HUD_COMPONENT__


#include "export.h"
#include <dtGame/message.h>
#include <dtGame/gmcomponent.h>

//This is for the CEGUI headers.
#ifdef None
#undef None
#endif

#include <dtGUI/dtgui.h>
#include <CEGUI/CEGUI.h>

#define HUDCONTROLMAXTEXTSIZE 100

/**
 * This component draws the HUD for the Tutorial app. It draws
 * status information like sim time, speed factor, num messages, etc...
 */
class TUTORIAL_TANK_EXPORT HUDComponent : public dtGame::GMComponent 
{
   public:
      // Constructor
      HUDComponent(dtCore::DeltaWin *win, 
         dtCore::Keyboard *keyboard, 
         dtCore::Mouse *mouse,
         const std::string &name);

   protected:
      // Destructor
      virtual ~HUDComponent();

   public:
      // Get messages from the Game Manager 
      virtual void ProcessMessage(const dtGame::Message& message);

      // returns the main GUI to be added to the scene.
      dtCore::RefPtr<dtGUI::CEUIDrawable> GetGUIDrawable() { return mGUI; }

      void OnAddedToGM();

   protected:
      /**
      * Utility method to set the text, position, and color of a text control
      * Check to see if the data changed.  The default values for color and position
      * won't do anything since they use a color and position < 0.
      */
      void UpdateStaticText(CEGUI::Window *textControl, char *newText,
         float red = -1.0, float blue = -1.0, float green = -1.0,
         float x = -1, float y = -1);

      // Utility method to create text
      CEGUI::Window *CreateText(const std::string &name, CEGUI::Window *parent, 
         const std::string &text, float x, float y, float width, float height);

   private:
      void UpdateSimTime(double newTime);
      void UpdateNumMessages(int messageCount);
      void UpdateLastMessageName(const std::string &messageName);

      // Sets up the GUI during construction
      void SetupGUI(dtCore::DeltaWin *win, 
                    dtCore::Keyboard *keyboard,
                    dtCore::Mouse *mouse);

      dtCore::RefPtr<dtGUI::CEUIDrawable> mGUI;
      CEGUI::Window *mMainWindow;
      CEGUI::Window *mOverlay;

      // main info
      CEGUI::Window *mNumMessagesText;
      CEGUI::Window *mSimTimeText;
      CEGUI::Window *mLastMessageText;

      long mUnHandledMessages;
};

#endif
