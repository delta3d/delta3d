/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute 
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
 * @author John K. Grant
 */
#ifndef _DELTA_GUI_MGR_H_
#define _DELTA_GUI_MGR_H_

#include <CEGUI/CEGUILogger.h>
#include <dtCore/base.h>           // for base class
#include <string>                  // for params
#include <dtCore/refptr.h>         // for member
#include <map>                     // for typedef, member
#include <CEGUI/CEGUIEvent.h>            // for CEGUI::Event::Connection

namespace CEGUI
{
   class Window;
   class Imageset;
   class Scheme;
}

namespace dtCore
{
   class DeltaWin;
}

namespace dtGUI
{
   class BaseScriptModule;
   class CEUIDrawable;

   /** A class responsible for drawing a GUI
     * and loading new GUI files.
     */
   class GUIManager : public dtCore::Base
   {
   public:
      typedef dtCore::Base BaseClass;

      GUIManager();

      /** Initializes the GUI system and renderer.
        * @param dwin The instance pointer of the DeltaWin being used by the application.
        * @param sm The (optional) ScriptModule to be used by the CEGUI::System.
        */
      void InitializeGUI(dtCore::DeltaWin* dwin, dtGUI::BaseScriptModule* sm=0);

      ///Returns the instance of the class responsible for drawing the GUI scene.
      dtGUI::CEUIDrawable* GetGUIDrawable();

      // These functions not actually used, so it's #ifdef'd out
      // for now to avoid linker warnings. -osb
      #if 0
      CEGUI::Scheme* LoadScheme(const std::string& scheme);
      CEGUI::Window* LoadLayout(const std::string& layout);
      #endif // 0

      CEGUI::Imageset* CreateImageset(const std::string& name, void* buffer, int width, int height);

      // the following will not compile with vc7.1 2003
      //CEGUI::Window* GUIManager::CreateWindow(const std::string& wtype, const std::string& wname)
      static CEGUI::Window* CreateGUIWindow(const std::string& wtype, const std::string& wname);

      /** Assigns the widget tree to be viewed.
        * \param widget The parent widget for all GUI elements to be drawn.
        */
      static void SetLayout(CEGUI::Window* widget);

      void SetGUILogLevel(CEGUI::LoggingLevel);

      static CEGUI::Window* GetWidgetByName(const std::string &name);

      static CEGUI::Event::Connection SubscribeEvent(CEGUI::Window *win, const CEGUI::String &eventName,CEGUI::Event::Subscriber subscriber);
      static CEGUI::Event::Connection SubscribeEvent(const std::string &winName, const CEGUI::String &eventName,CEGUI::Event::Subscriber subscriber);

      static void ProducerToNormalized(float& x, float& y);

      void ShutdownGUI();

   protected:
      ~GUIManager();

   private:
      dtCore::RefPtr<dtGUI::CEUIDrawable> mDrawable;

      typedef std::map<std::string,CEGUI::Window*> LoadedLayoutMap;
      LoadedLayoutMap mLoadedLayouts;
   };
}

#endif // _DELTA_GUI_MGR_H_
