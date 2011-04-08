/* -*-c++-*-
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
 * William E. Johnson II
 */
#ifndef DELTA_FIRE_FIGHTER_HUD_COMPONENT
#define DELTA_FIRE_FIGHTER_HUD_COMPONENT

#include <dtGame/gmcomponent.h>
#include <dtCore/timer.h>
#include <fireFighter/export.h>
#include <CEGUIUDim.h> // So we get CEGUI::UVector2 without including everything in CEGUI.h

// Forward declarations
namespace CEGUI
{
   class PushButton;
   class Window;
   class EventArgs;
   class UVector2;
}

namespace dtGUI
{
   class GUI;
}

namespace dtABC
{
   class BaseABC;;
}

namespace dtActors
{
   class TaskActorProxy;
}

class GameState;
class GameItemActor;
class HelpWindow;

class FIRE_FIGHTER_EXPORT HUDComponent : public dtGame::GMComponent
{
   public:

      static const std::string NAME;

      /// Constructor
      HUDComponent(dtABC::BaseABC& app,
                  const std::string &name = NAME);

      /// Processes incoming messages
      virtual void ProcessMessage(const dtGame::Message &msg);

   protected:

      /// Destructor
      virtual ~HUDComponent();

      /// Shows the Main menu
      void ShowMainMenu();

      /// Shows the End menu
      void ShowEndMenu();

      /// Displays the HUD
      void ShowHUD();

      /// Hides the currently all menus
      void HideMenus(); 

      /// Shows the intro help
      void ShowIntroMenu();
 
   private:
      
      /**
       * Private helper method to build the main menu
       */
      void BuildMainMenu();

      /**
       * Private helper method to build the HUD
       */
      void BuildHUD();

      /**
       * Private helper method to build the end of game menu
       */
      void BuildEndMenu();

      /**
       * Private helper method to build the intro menu
       */
      void BuildIntroMenu();

      /**
       * Private helper method to init the GUI
       */
      void SetupGUI(dtABC::BaseABC& app);

      /**
       * Callback for when the start button is clicked
       */
      bool OnStartWithObjectives(const CEGUI::EventArgs &e);

      /**
       * Callback for when the start button is clicked
       */
      bool OnStart(const CEGUI::EventArgs &e);

      /**
       * Callback for when the quit button is clicked
       */
      bool OnQuit(const CEGUI::EventArgs &e);

      /**
       * Callback for when the quit button is clicked
       */
      bool OnReturnToMenu(const CEGUI::EventArgs &e);

      /**
       * Callback for when the quit button is clicked
       */
      bool OnHelpWindowClosed(const CEGUI::EventArgs &e);

      /**
       * Helper method to quickly build and send game state changed messages
       * @param oldState the old state
       * @param newState the new state
       */
      void SendGameStateChangedMessage(GameState &oldState, GameState &newState);

      /**
       * Private helper method to refresh the HUDComponent when states change
       */
      void Refresh();

      /**
       * Private helper method to toggle the mouse on and off
       * @param enable True to show the mouse
       */
      void ShowMouse(bool enable);

      /**
       * Shows the "Item of interest" image
       */
      void ShowGameItemImage();

      /**
       * Adds an item to the HUD
       */
      void AddItemToHUD(GameItemActor *item);

      /**
       * Sets the currently selected item
       */
      void SetSelectedItem(GameItemActor *item);

      /**
       * Sets the currently activated item
       */
      void SetActivatedItem(GameItemActor *item);

      /**
       * Sets the currently activated item
       */
      void SetDeactivatedItem(GameItemActor *item);

      /**
       * Private helper method to hide the game item image
       */
      void HideGameItemImage();

      /**
       * Private helper method called once per tick to update 
       * the tasks UI
       */
      void UpdateMediumDetailData(CEGUI::Window *parent);

      /**
       * Recursively adds subtasks to the UI from parent tasks
       */
      unsigned int RecursivelyAddTasks(const std::string &indent, 
                                       unsigned int curIndex,
                                       const dtActors::TaskActorProxy *taskProxy, 
                                       unsigned int &numCompleted, 
                                       CEGUI::Window *parent);

      /**
       * Utility method to set the text, position, and color of a text control
       * Check to see if the data changed.  The default values for color and position
       * won't do anything since they use a color and position < 0.
       */
      void UpdateStaticText(CEGUI::Window *textControl, 
                            const std::string &newText,
                            float red   = -1.0f, 
                            float green = -1.0f, 
                            float blue  = -1.0f,
                            float x     = -1.0f, 
                            float y     = -1.0f);

      /**
       * Private helper method to create text for the task actors
       */
      CEGUI::Window* CreateText(const std::string &name, 
                                CEGUI::Window *parent, 
                                const std::string &text,
                                float x, float y, float width, float height);

      /**
       * Refreshes the debriefing screen
       */
      void RefreshDebriefScreen();

      /**
       * Updates the main background during game play
       */
      void UpdateHUDBackground();

      CEGUI::PushButton *mStartWithObjectives, *mStart, *mQuit, *mReturnToMenu;
      CEGUI::Window *mMainWindow;
      CEGUI::Window *mWindowBackground, *mHUDBackground, *mDebriefBackground, 
                    *mIntroBackground, *mGameItemImage, 
                    *mFireSuitIcon, *mFireHoseIcon, *mSCBAIcon, 
                    *mInventoryUseFireSuitIcon, *mInventoryUseFireHoseIcon, *mInventoryUseSCBAIcon, 
                    *mInventorySelectIcon, 
                    *mTargetIcon;

      CEGUI::Window *mAppHeader, *mDebriefHeaderText, *mIntroText;

      dtCore::RefPtr<dtGUI::GUI> mGUI;

      bool mShowObjectives;
      GameState *mCurrentState;
      CEGUI::UVector2 mFireSuitIconPos, mFireHoseIconPos, mSCBAIconPos; 
      std::vector<CEGUI::Window*> mTaskTextList;
      CEGUI::Window *mTasksHeaderText;
      const unsigned int mNumTasks;
      CEGUI::Window *mMissionCompletedText, *mMissionFailedText;
      bool mMissionComplete, mMissionFailed;
      dtActors::TaskActorProxy *mFailedProxy;
      CEGUI::Window *mCompleteOrFail, *mFailReason;
      std::vector<CEGUI::Window*> mDebriefList;
      CEGUI::Window *mHUDOverlay;
      dtCore::RefPtr<HelpWindow> mHelpWindow;
};

#endif
