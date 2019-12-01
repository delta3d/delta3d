/* -*-c++-*-
 * testAAR - testaarhud (.h & .cpp) - Using 'The MIT License'
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
 * Curtiss Murphy
 */

#ifndef DELTA_TEST_AAR_HUD
#define DELTA_TEST_AAR_HUD

#include <dtGame/message.h>
#include <dtGame/gmcomponent.h>
#include <osg/Referenced>

//This is for the CEGUI headers.
#ifdef None
#undef None
#endif

#include <CEGUI/CEGUIWindow.h>
#include <dtGUI/gui.h>
#include "export.h"

#define HUDCONTROLMAXTEXTSIZE 100

namespace dtCore
{
   class Keyboard;
   class Mouse;
   class DeltaWin;
}

namespace dtABC
{
   class BaseABC;
}

namespace dtActors
{
   class TaskActorProxy;
}

namespace dtGame
{
   class ServerLoggerComponent;
   class LogController;
   class TaskComponent;
}

/**
 * HUD State enumeration - what info is the HUD showing.
 */
class TEST_AAR_EXPORT HUDState : public dtUtil::Enumeration
{
   DECLARE_ENUM(HUDState);
   public:
      static HUDState MINIMAL;
      static HUDState MEDIUM;
      static HUDState MAXIMUM;
      static HUDState NONE;
      static HUDState HELP;
   private:
      HUDState(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

/**
 * This class draws the HUD for the testAAR with using CEGUI.  It draws
 * status information like AAR state (record, playback, idle), sim time,
 * speed factor, num messages, and other help info etc...
 */
class TEST_AAR_EXPORT TestAARHUD : public dtGame::GMComponent
{
   public:

      /**
       * Constructs the test application.
       */
      TestAARHUD(dtABC::BaseABC& app,
                 dtGame::LogController& logController, 
                 dtGame::TaskComponent& taskComponent,
                 dtGame::ServerLoggerComponent& serverLogger);
   protected:

      /**
       * Destroys the test application.
       */
      virtual ~TestAARHUD();

   public:

      /**
       * Get messages from the GM component
       */
      virtual void ProcessMessage(const dtGame::Message& message);

      /**
       * Sets up the basic GUI.
       */
      void SetupGUI(dtCore::Camera& cam, dtCore::Keyboard& keyboard, dtCore::Mouse& mouse);

      /**
       * Cycles HUD state to the next most data.  From minimal, to moderate, to max,
       * and then back to minimal.
       */
      HUDState& CycleToNextHUDState();

      void SetHUDState(HUDState& newState) { mHUDState = &newState;  UpdateState(); }
      HUDState& GetHUDState() { return *mHUDState; }

      dtGUI::GUI* GetGUIDrawable() { return mGUI.get(); }

      void TickHUD();

   protected:

      /**
       * Helper method that creates an actor with random movement behavior.
       */
      //void PlaceActor();

      /**
       * Makes sure to enable/disable controls as appropriate for each state.
       */
      void UpdateState();

      /**
       * Utility method to set the text, position, and color of a text control
       * Check to see if the data changed.  The default values for color and position
       * won't do anything since they use a color and position < 0.
       */
      void UpdateStaticText(CEGUI::Window* textControl, const std::string& newText,
         float red = -1.0f, float green = -1.0f, float blue = -1.0f, float x = -1.0f, float y = -1.0f);

   private:

      /**
       * During the tickHUD() - update our medium data
       */
      void UpdateMediumDetailData();

      /**
       * During the tickHUD() - update our high data
       */
      void UpdateHighDetailData(int baseWidth, float& curYPos);

      /**
       * Add the tasks to the UI.  The tasks have to be worked on as parent/children - so
       * we have to do it recursively.  The numCompleted is increased (+1) if the taskProxy
       * was complete.
       */
      int RecursivelyAddTasks(const std::string& indent, int curIndex,
                              const dtActors::TaskActorProxy* taskProxy, int& numCompleted);

      /**
       * Utility method to create text
       */
      CEGUI::Window* CreateText(const std::string& name, CEGUI::Window* parent, const std::string& text,
                                     float x, float y, float width, float height);

      HUDState* mHUDState;
      HUDState* mLastHUDStateBeforeHelp;

      dtCore::RefPtr<dtGame::LogController> mLogController;
      dtCore::RefPtr<dtGame::TaskComponent> mTaskComponent;
      dtCore::RefPtr<dtGame::ServerLoggerComponent> mServerLoggerComponent;

      dtCore::DeltaWin* mWin;
      CEGUI::Window* mMainWindow;
      dtCore::RefPtr<dtGUI::GUI> mGUI;
      CEGUI::Window* mHUDOverlay;
      CEGUI::Window* mHelpOverlay;


      // main info
      CEGUI::Window* mStateText;
      CEGUI::Window* mSimTimeText;
      CEGUI::Window* mSpeedFactorText;
      CEGUI::Window* mNumMessagesText;
      CEGUI::Window* mRecordDurationText;
      CEGUI::Window* mNumTagsText;
      CEGUI::Window* mLastTagText;
      CEGUI::Window* mNumFramesText;
      CEGUI::Window* mLastFrameText;
      CEGUI::Window* mCurLogText;
      CEGUI::Window* mCurMapText;

      // tips messages
      CEGUI::Window* mFirstTipText;
      CEGUI::Window* mSecondTipText;

      // help fields
      CEGUI::Window* mHelpTipText;
      CEGUI::Window* mHelp1Text;
      CEGUI::Window* mHelp2Text;
      CEGUI::Window* mHelp3Text;
      CEGUI::Window* mHelp4Text;
      CEGUI::Window* mHelp5Text;
      CEGUI::Window* mHelp6Text;
      CEGUI::Window* mHelp7Text;
      CEGUI::Window* mHelp8Text;
      CEGUI::Window* mHelp9Text;
      CEGUI::Window* mHelp10Text;
      CEGUI::Window* mHelp11Text;
      CEGUI::Window* mHelp12Text;
      CEGUI::Window* mHelp13Text;
      CEGUI::Window* mHelp14Text;
      CEGUI::Window* mHelp15Text;
      CEGUI::Window* mHelp16Text;
      CEGUI::Window* mHelp17Text;
      CEGUI::Window* mHelp18Text;
      CEGUI::Window* mHelp19Text;

      // task texts
      CEGUI::Window* mTasksHeaderText;
      std::vector<CEGUI::Window*> mTaskTextList;

      float mRightTextXOffset;
      float mTextYTopOffset;
      float mTextHeight;
};

#endif
