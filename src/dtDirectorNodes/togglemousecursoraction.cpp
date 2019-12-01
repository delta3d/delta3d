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
 * Author: Jeff P. Houde
 */

#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/togglemousecursoraction.h>

#include <dtABC/application.h>

#include <dtCore/deltawin.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   ToggleMouseCursorAction::ToggleMouseCursorAction()
      : ActionNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   ToggleMouseCursorAction::~ToggleMouseCursorAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleMouseCursorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Show", "Shows the application's mouse cursor."));
      mInputs.push_back(InputLink(this, "Hide", "Hides the application's mouse cursor."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ToggleMouseCursorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ToggleMouseCursorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtABC::Application* app = dtABC::Application::GetInstance(0);
      if (app)
      {
         switch (input)
         {
         case INPUT_SHOW:
            {
               app->GetWindow()->SetShowCursor(true);
               break;
            }
         case INPUT_HIDE:
            {
               app->GetWindow()->SetShowCursor(false);
               break;
            }
         }
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }
}

////////////////////////////////////////////////////////////////////////////////
