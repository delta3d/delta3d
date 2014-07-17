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

#ifndef DELTA_TEST_APP_GUI_COMPONENT
#define DELTA_TEST_APP_GUI_COMPONENT

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/message.h>
#include <dtGame/gamestate.h>
#include <dtGame/gmcomponent.h>
#include <osg/Referenced>

//This is for the CEGUI headers.
#ifdef None
#undef None
#endif

#include <dtGUI/gui.h>
#include "export.h"
#include "guiscreen.h"
#include "testappmessages.h"

#define HUDCONTROLMAXTEXTSIZE 100



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtCore
{
   class Keyboard;
   class Mouse;
   class DeltaWin;
}


namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT GuiComponent : public dtGame::GMComponent
   {
      public:

         GuiComponent();

         /*override*/ void OnAddedToGM();

         GuiNode* GetUI(const std::string& uiName);

         /**
          * Get messages from the GM component
          */
         /*override*/ void ProcessMessage(const dtGame::Message& message);

         void HandleGameStateChanged(const dtGame::GameStateType& gameState);

         void HandleUIMessage(const dtExample::UIMessage& uiMessage);

         /**
          * Sets up the basic GUI.
          */
         void SetupGUI(dtCore::Camera& cam, dtCore::Keyboard& keyboard, dtCore::Mouse& mouse);

         void Update(float simTimeDelta, float realTimeDelta);

      protected:

         virtual ~GuiComponent();

         bool RegisterScreenWithState(GuiScreen& screen, const dtGame::GameStateType& gameStateType);

      private:


         dtCore::RefPtr<dtCore::DeltaWin> mWindow;
         dtCore::RefPtr<dtGUI::GUI> mGUIScene;

         typedef std::map<const dtGame::GameStateType*, dtCore::RefPtr<GuiScreen> > GameStateScreenMap;

         GameStateScreenMap mScreens;
         dtCore::RefPtr<GuiScreen> mCurrentScreen;
         dtCore::RefPtr<GuiScreen> mHelpOverlay;
   };

} // END - namespace dtExample

#endif
