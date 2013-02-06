/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Author: Eric R. Heine
 */

#include <dtDirectorGUINodes/toggleguicursor.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtGUI/gui.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   ToggleGUICursor::ToggleGUICursor()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   ToggleGUICursor::~ToggleGUICursor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleGUICursor::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Show", "Shows the CEGUI mouse cursor."));
      mInputs.push_back(InputLink(this, "Hide", "Hides the CEGUI mouse cursor."));

      // Add failed input
      mOutputs.push_back(OutputLink(this, "Failed", "Activates if the GUI could not load."));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ToggleGUICursor::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         switch (input)
         {
         case INPUT_SHOW:
            gui->ShowCursor();
            break;
         case INPUT_HIDE:
            gui->HideCursor();
            break;
         }
         return ActionNode::Update(simDelta, delta, input, firstUpdate);
      }

      ActivateOutput("Failed");
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
