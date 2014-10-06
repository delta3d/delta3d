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

#include <dtDirectorGUINodes/loadlayout.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>

#include <dtGUI/gui.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   LoadLayout::LoadLayout()
      : ActionNode()
      , mShowImmediately(false)
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   LoadLayout::~LoadLayout()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void LoadLayout::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* layoutProp = new dtCore::StringSelectorActorProperty(
         "Layout", "Layout",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &LoadLayout::SetLayout),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &LoadLayout::GetLayout),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &LoadLayout::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);

      dtCore::BooleanActorProperty* showImmediatelyProp = new dtCore::BooleanActorProperty(
         "ShowImmediately", "Show Immediately",
         dtCore::BooleanActorProperty::SetFuncType(this, &LoadLayout::SetShowImmediately),
         dtCore::BooleanActorProperty::GetFuncType(this, &LoadLayout::GetShowImmediately),
         "Whether we should show the layout immediately or not.");
      AddProperty(showImmediatelyProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, showImmediatelyProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool LoadLayout::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      CEGUI::Window* layout = GUINodeManager::GetLayout(GetString("Layout"));
      if (layout && GetBoolean("ShowImmediately"))
      {
         layout->show();
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoadLayout::UpdateName()
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
   void LoadLayout::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

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
   void LoadLayout::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string LoadLayout::GetLayout() const
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> LoadLayout::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoadLayout::SetShowImmediately(bool value)
   {
      mShowImmediately = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool LoadLayout::GetShowImmediately() const
   {
      return mShowImmediately;
   }
}

////////////////////////////////////////////////////////////////////////////////
