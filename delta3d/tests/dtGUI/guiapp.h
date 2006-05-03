/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
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
#ifndef _TESTS_GUI_APP_H_
#define _TESTS_GUI_APP_H_

#include <dtABC/application.h>  // for base class
#include <dtCore/refptr.h>      // for member

namespace dtGUI
{
   class GUIManager;
   class ScriptModule;
}

namespace dtTest
{
   class GUIApp : public dtABC::Application
   {
   public:
      typedef dtABC::Application BaseClass;

      GUIApp();

      void Config();

      dtGUI::GUIManager* GetGUIManager();

      dtGUI::ScriptModule* GetScriptModule();

   protected:
      virtual ~GUIApp();

   private:
      dtCore::RefPtr<dtGUI::GUIManager> mGUIManager;
      dtGUI::ScriptModule* mScriptModule;
   };
}

#endif  // _TESTS_GUI_APP_H_
