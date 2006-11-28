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
#ifndef DELTA_FIRE_FIGHTER_HELP_WINDOW
#define DELTA_FIRE_FIGHTER_HELP_WINDOW

#include <fireFighter/export.h>
#include <osg/Referenced>

namespace CEGUI
{
   class PushButton;
   class FrameWindow;
   class Window;
}

class FIRE_FIGHTER_EXPORT HelpWindow : public osg::Referenced
{
   public:

      /// Constructor
      HelpWindow(CEGUI::Window *mainWindow);

      /**
         * Enables and disables the help window
         * @param enable, true to enable, false to disable
         */
      void Enable(bool enable);

      /**
         * Returns if the help window is enabled
         * @return mIsEnabled
         */
      bool IsEnabled() const { return mIsEnabled; }

      CEGUI::PushButton*  GetCloseButton() { return mCloseButton; }
      CEGUI::FrameWindow* GetFrameWindow() { return mOverlay;     }

   protected:

      /// Destructor
      virtual ~HelpWindow();

   private:

      /**
         * Private helper method to save clutter in the constructor
         */
      void InitGui(CEGUI::Window *mainWindow);

      bool mIsEnabled;

      // Internal window handle
      CEGUI::FrameWindow *mOverlay;
      CEGUI::PushButton  *mCloseButton;

      // Internal handle to the main window
      CEGUI::Window *mMainWindow;

      // Internal handle to the help text
      CEGUI::Window 
         *mHeaderText, 
         *mBinocsText, 
         *mLRFText,
         *mCompassText,
         *mGPSText, 
         *mToggleFullScreen,
         *mMagnifyModels;
};


#endif
