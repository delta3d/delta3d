/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
   class Keyboard;
   class Mouse;
}

namespace dtGUI
{
   class ScriptModule;
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
      void InitializeGUI(dtCore::DeltaWin* dwin, 
                        dtCore::Keyboard *keyboard,
                        dtCore::Mouse *mouse,
                        dtGUI::ScriptModule* sm=0);

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
