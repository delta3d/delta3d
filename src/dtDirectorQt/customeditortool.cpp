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

#include <dtDirectorQt/customeditortool.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////////
   CustomEditorTool::CustomEditorTool(const std::string& name)
      : mIsOpen(false)
      , mEditor(NULL)
      , mGraph(NULL)
   {
      mToolName = name;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CustomEditorTool::~CustomEditorTool()
   {
      if (mEditor)
      {
         mEditor->UnRegisterCustomEditorTool(this);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void CustomEditorTool::SetEditor(DirectorEditor* editor)
   {
      mEditor = editor;

      // Register this tool with the main editor.
      if (mEditor)
      {
         mEditor->RegisterCustomEditorTool(this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CustomEditorTool::Open(DirectorGraph* graph)
   {
      Close();

      mGraph = graph;
      mIsOpen = true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CustomEditorTool::Close()
   {
      mGraph = NULL;
      mIsOpen = false;
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
