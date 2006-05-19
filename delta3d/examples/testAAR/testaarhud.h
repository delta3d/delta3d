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
* @author Curtiss Murphy
*/
#ifndef DELTA_TESTAARHUD
#define DELTA_TESTAARHUD


#include <dtABC/application.h>
#include <dtGame/logstatus.h>
#include <dtGame/message.h>
#include <dtGame/gmcomponent.h>
#include <osg/Referenced>

//This is for the CEGUI headers.
#ifdef None
#undef None
#endif
#include <CEGUI/CEGUI.h>
#include <dtGUI/dtgui.h>

#define HUDCONTROLMAXTEXTSIZE 100

namespace dtActors
{
   class TaskActorProxy;
}

namespace dtGame
{
   class ClientGameManager;
   class GameActorProxy;
   class ServerLoggerComponent;
   class LogController;
   class TaskComponent;
}

/**
 * Exception that may be thrown by the HUD.
 */
class ARRHUDException : public dtUtil::Enumeration
{
   DECLARE_ENUM(ARRHUDException);
   public:
      static ARRHUDException INIT_ERROR;
      static ARRHUDException RUNTIME_ERROR;
   private:
      ARRHUDException(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

/**
 * HUD State enumeration - what info is the HUD showing.
 */
class HUDState : public dtUtil::Enumeration
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
class TestAARHUD : public dtGame::GMComponent //public osg::Referenced
{
   public:

      /**
       * Constructs the test application.
       */
      TestAARHUD(dtCore::DeltaWin *win, dtGame::ClientGameManager &clientGM,
                 dtGame::LogController &logController, dtGame::TaskComponent &taskComponent,
                 dtGame::ServerLoggerComponent &serverLogger);

      /**
       * Destroys the test application.
       */
      virtual ~TestAARHUD();

      /**
       * Get messages from the GM component
       */
      virtual void ProcessMessage(const dtGame::Message& message);

      /**
       * Sets up the basic GUI.
       */
      void SetupGUI(dtCore::DeltaWin *win);

      /**
       * Cycles HUD state to the next most data.  From minimal, to moderate, to max,
       * and then back to minimal.
       */
      HUDState &CycleToNextHUDState();

      void SetHUDState(HUDState &newState) { mHUDState = &newState;  UpdateState(); }
      HUDState &GetHUDState() { return *mHUDState; }

      dtCore::RefPtr<dtGUI::CEUIDrawable> GetGUIDrawable() { return mGUI; }

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
      void UpdateStaticText(CEGUI::StaticText *textControl, char *newText,
                            float red = -1.0, float blue = -1.0, float green = -1.0,
                            float x = -1, float y = -1);
private:

      /**
       * During the tickHUD() - update our medium data
       */
      void UpdateMediumDetailData();

      /**
       * During the tickHUD() - update our high data
       */
      void UpdateHighDetailData(int baseWidth, float &curYPos);

      /**
       * Add the tasks to the UI.  The tasks have to be worked on as parent/children - so
       * we have to do it recursively.  The numCompleted is increased (+1) if the taskProxy
       * was complete.
       */
      int RecursivelyAddTasks(std::string indent, int curIndex,
                              const dtActors::TaskActorProxy *taskProxy, int &numCompleted);

      /**
       * Utility method to create text
       */
      CEGUI::StaticText *CreateText(const std::string &name, CEGUI::StaticImage *parent, const std::string &text,
                                     float x, float y, float width, float height);

      HUDState *mHUDState;
      HUDState *mLastHUDStateBeforeHelp;

      dtCore::RefPtr<dtGame::ClientGameManager> mClientGM;
      dtCore::RefPtr<dtGame::LogController> mLogController;
      dtCore::RefPtr<dtGame::TaskComponent> mTaskComponent;
      dtCore::RefPtr<dtGame::ServerLoggerComponent> mServerLoggerComponent;

      dtCore::DeltaWin *mWin;
      CEGUI::Window *mMainWindow;
      dtCore::RefPtr<dtGUI::CEUIDrawable> mGUI;
      CEGUI::StaticImage *mHUDOverlay;
      CEGUI::StaticImage *mHelpOverlay;


      // main info
      CEGUI::StaticText *mStateText;
      CEGUI::StaticText *mSimTimeText;
      CEGUI::StaticText *mSpeedFactorText;
      CEGUI::StaticText *mNumMessagesText;
      CEGUI::StaticText *mRecordDurationText;
      CEGUI::StaticText *mNumTagsText;
      CEGUI::StaticText *mLastTagText;
      CEGUI::StaticText *mNumFramesText;
      CEGUI::StaticText *mLastFrameText;
      CEGUI::StaticText *mCurLogText;
      CEGUI::StaticText *mCurMapText;

      // tips messages
      CEGUI::StaticText *mFirstTipText;
      CEGUI::StaticText *mSecondTipText;

      // help fields
      CEGUI::StaticText *mHelpTipText;
      CEGUI::StaticText *mHelp1Text;
      CEGUI::StaticText *mHelp2Text;
      CEGUI::StaticText *mHelp3Text;
      CEGUI::StaticText *mHelp4Text;
      CEGUI::StaticText *mHelp5Text;
      CEGUI::StaticText *mHelp6Text;
      CEGUI::StaticText *mHelp7Text;
      CEGUI::StaticText *mHelp8Text;
      CEGUI::StaticText *mHelp9Text;
      CEGUI::StaticText *mHelp10Text;
      CEGUI::StaticText *mHelp11Text;
      CEGUI::StaticText *mHelp12Text;
      CEGUI::StaticText *mHelp13Text;
      CEGUI::StaticText *mHelp14Text;
      CEGUI::StaticText *mHelp15Text;
      CEGUI::StaticText *mHelp16Text;
      CEGUI::StaticText *mHelp17Text;
      CEGUI::StaticText *mHelp18Text;

      // task texts
      CEGUI::StaticText *mTasksHeaderText;
      std::vector<CEGUI::StaticText *> mTaskTextList;

      float mRightTextXOffset;
      float mTextYTopOffset;
      float mTextYSeparation;
      float mTextHeight;
};

#endif
