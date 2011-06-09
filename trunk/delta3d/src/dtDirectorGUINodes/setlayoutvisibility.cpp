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

#include <dtDirectorGUINodes/setlayoutvisibility.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtDAL/stringselectoractorproperty.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetLayoutVisibility::SetLayoutVisibility()
      : ActionNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetLayoutVisibility::~SetLayoutVisibility()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Show"));
      mInputs.push_back(InputLink(this, "Hide"));

      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::OnFinishedLoading()
   {
      GUINodeManager::GetLayout(GetString("Layout"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringSelectorActorProperty* layoutProp =
         new dtDAL::StringSelectorActorProperty(
         "Layout", "Layout",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &SetLayoutVisibility::SetLayout),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &SetLayoutVisibility::GetLayout),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &SetLayoutVisibility::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);
      
      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetLayoutVisibility::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      CEGUI::Window* layout = GUINodeManager::GetLayout(GetString("Layout"));
      if (!layout)
      {
         TriggerOutput("Failed");
         return false;
      }

      switch (input)
      {
      case INPUT_SHOW:
         {
            layout->show();
         }
         break;
      case INPUT_HIDE:
         {
            layout->hide();
         }
         break;
      }

      // Fire the "Out" link
      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::UpdateName()
   {
      std::string layoutName = GetString("Layout");

      if (layoutName.empty())
      {
         mName.clear();
      }
      else
      {
         mName = "Layout: " + layoutName;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::OnLinkValueChanged(const std::string& linkName)
   {
      if (!GetDirector()->IsLoading())
      {
         if (linkName == "Layout")
         {
            std::string layoutName = GetString("Layout");
            GUINodeManager::GetLayout(layoutName);

            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetLayoutVisibility::GetLayout()
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SetLayoutVisibility::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }
}

////////////////////////////////////////////////////////////////////////////////
