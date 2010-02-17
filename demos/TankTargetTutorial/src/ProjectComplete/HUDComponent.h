/* -*-c++-*-
 * TutorialLibrary - HUDComponent (.h & .cpp) - Using 'The MIT License'
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
#ifndef __GM_TUTORIAL_HUD_COMPONENT__
#define __GM_TUTORIAL_HUD_COMPONENT__


#include "export.h"
#include <dtGame/gmcomponent.h>


// This is for the CEGUI headers.
#ifdef None
#undef None
#endif


#define HUDCONTROLMAXTEXTSIZE 100

namespace dtABC
{
   class BaseABC;
}

namespace dtGUI
{
   class GUI;
}

namespace dtGame
{
   class LogController;
}

namespace CEGUI
{
   class Window;
}

/**
 * This component draws the HUD for the Tutorial app. It draws
 * status information like sim time, speed factor, num messages, etc...
 */
class TUTORIAL_TANK_EXPORT HUDComponent : public dtGame::GMComponent
{
public:
   // Constructor
   HUDComponent(dtABC::BaseABC& app,
                const std::string& name);

protected:
   // Destructor
   virtual ~HUDComponent();

public:
   // Get messages from the Game Manager
   virtual void ProcessMessage(const dtGame::Message& message);

protected:
   /**
    * Utility method to set the text, position, and color of a text control
    * Check to see if the data changed.  The default values for color and position
    * won't do anything since they use a color and position < 0.
    */
   void UpdateStaticText(CEGUI::Window* textControl, const char* newText,
                         float red = -1.0, float blue = -1.0, float green = -1.0,
                         float x = -1, float y = -1);

   // Utility method to create text
   CEGUI::Window* CreateText(const std::string& name, CEGUI::Window* parent,
      const std::string& text, float x, float y, float width, float height);

private:
   void UpdateSimTime(double newTime);
   void UpdateNumMessages(int messageCount);
   void UpdateLastMessageName(const std::string& messageName);

   // Sets up the GUI during construction
   void SetupGUI(dtABC::BaseABC& app);

   dtCore::RefPtr<dtGUI::GUI> mGUI;
   CEGUI::Window* mMainWindow;
   CEGUI::Window* mOverlay;

   // main info
   CEGUI::Window* mNumMessagesText;
   CEGUI::Window* mSimTimeText;
   CEGUI::Window* mLastMessageText;
   CEGUI::Window* mStateText;

   long mUnHandledMessages;

   dtCore::RefPtr<dtGame::LogController> mLogController;
};

#endif // __GM_TUTORIAL_HUD_COMPONENT__
