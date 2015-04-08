/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef GUI_NODE_MANAGER
#define GUI_NODE_MANAGER

#include <dtDirectorGUINodes/nodelibraryexport.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>

////////////////////////////////////////////////////////////////////////////////
namespace dtGUI
{
   class ScriptModule;
}

namespace dtDirector
{
   class GUI_NODE_LIBRARY_EXPORT GUINodeManager
   {
   public:

      /**
       * Retrieves a GUI manager.
       */
      static dtGUI::GUI* GetGUI();

      /**
       * Retrieves or attempts to create a layout of a given file name.
       */
      static CEGUI::Window* GetLayout(const std::string& name);

      /**
       * Retrieve a list of all Layouts currently loaded.
       */
      static std::vector<std::string> GetLayoutList();

   private:

      /**
       *	Constructor.
       */
      GUINodeManager();
      virtual ~GUINodeManager();
   };
}

#endif // GUI_NODE_MANAGER
