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

#include <dtDirectorGUINodes/getwidgetposition.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/stringactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetWidgetPosition::GetWidgetPosition()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetWidgetPosition::~GetWidgetPosition()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if Widget could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::OnFinishedLoading()
   {
      GUINodeManager::GetLayout(GetString("Layout"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* layoutProp = new dtCore::StringSelectorActorProperty(
         "Layout", "Layout",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &GetWidgetPosition::SetLayout),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &GetWidgetPosition::GetLayout),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &GetWidgetPosition::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);

      dtCore::StringSelectorActorProperty* widgetProp = new dtCore::StringSelectorActorProperty(
         "Widget", "Widget",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &GetWidgetPosition::SetWidget),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &GetWidgetPosition::GetWidget),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &GetWidgetPosition::GetWidgetList),
         "The Widget to set the text on.", "", true);
      AddProperty(widgetProp);

      mpPositionProp = new dtCore::Vec4ActorProperty(
         "Position", "Position",
         dtCore::Vec4ActorProperty::SetFuncType(this, &GetWidgetPosition::SetPosition),
         dtCore::Vec4ActorProperty::GetFuncType(this, &GetWidgetPosition::GetPosition),
         "The position of the widget in Unified Dims (xScale, xDim, yScale, yDim).");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
      mValues.push_back(ValueLink(this, mpPositionProp, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetWidgetPosition::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      CEGUI::Window* layout = GUINodeManager::GetLayout(GetString("Layout"));
      if (!layout)
      {
         ActivateOutput("Failed");
         return false;
      }

      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         CEGUI::Window* widget = gui->GetWidget(GetString("Widget"));
         if (widget)
         {
            CEGUI::UVector2 pos = widget->getPosition();
            SetVec4(osg::Vec4(pos.d_x.d_scale, pos.d_x.d_offset,
               pos.d_y.d_scale, pos.d_y.d_offset), "Position");
         }
      }

      // Fire the "Out" link
      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::UpdateName()
   {
      std::string layoutName = GetString("Layout");
      std::string widgetName = GetString("Widget");

      if (layoutName.empty())
      {
         mName.clear();
      }
      else
      {
         mName = "Layout: " + layoutName;
      }

      if (!widgetName.empty())
      {
         if (!mName.empty())
         {
            mName += "::";
         }
         mName += widgetName;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::OnLinkValueChanged(const std::string& linkName)
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
         else if (linkName == "Widget" || linkName == "Text")
         {
            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string GetWidgetPosition::GetLayout() const
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> GetWidgetPosition::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::SetWidget(const std::string& value)
   {
      mWidget = value;

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string GetWidgetPosition::GetWidget() const
   {
      return mWidget;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> GetWidgetPosition::GetWidgetList()
   {
      std::vector<std::string> list;
      RecurseWidgetList(list, GUINodeManager::GetLayout(GetString("Layout")));
      return list;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::RecurseWidgetList(std::vector<std::string>& widgetList, CEGUI::Window* parent)
   {
      if (!parent)
      {
         return;
      }

      size_t count = parent->getChildCount();
      for (size_t index = 0; index < count; ++index)
      {
         CEGUI::Window* child = parent->getChildAtIdx(index);

         if (child)
         {
            widgetList.push_back(child->getName().c_str());
         }

         RecurseWidgetList(widgetList, child);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetWidgetPosition::SetPosition(const osg::Vec4& value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 GetWidgetPosition::GetPosition() const
   {
      return osg::Vec4();
   }
}

////////////////////////////////////////////////////////////////////////////////
