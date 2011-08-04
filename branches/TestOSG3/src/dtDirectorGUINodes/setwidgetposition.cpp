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

#include <dtDirectorGUINodes/setwidgetposition.h>
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
   SetWidgetPosition::SetWidgetPosition()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetWidgetPosition::~SetWidgetPosition()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetPosition::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if Widget could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetPosition::OnFinishedLoading()
   {
      GUINodeManager::GetLayout(GetString("Layout"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetPosition::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* layoutProp = new dtCore::StringSelectorActorProperty(
         "Layout", "Layout",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SetWidgetPosition::SetLayout),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SetWidgetPosition::GetLayout),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &SetWidgetPosition::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);

      dtCore::StringSelectorActorProperty* widgetProp = new dtCore::StringSelectorActorProperty(
         "Widget", "Widget",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SetWidgetPosition::SetWidget),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SetWidgetPosition::GetWidget),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &SetWidgetPosition::GetWidgetList),
         "The Widget to set the text on.", "", true);
      AddProperty(widgetProp);

      dtCore::Vec4ActorProperty* valueProp = new dtCore::Vec4ActorProperty(
         "WidgetPosition", "Position",
         dtCore::Vec4ActorProperty::SetFuncType(this, &SetWidgetPosition::SetPosition),
         dtCore::Vec4ActorProperty::GetFuncType(this, &SetWidgetPosition::GetPosition),
         "The position to set on the widget in Unified Dims (xScale, xOffset, yScale, yOffset).");
      AddProperty(valueProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
      mValues.push_back(ValueLink(this, valueProp));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetWidgetPosition::Update(float simDelta, float delta, int input, bool firstUpdate)
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
         std::string widgetName = GetString("Widget");
         osg::Vec4 position = GetVec4("WidgetPosition");

         CEGUI::Window* widget = gui->GetWidget(widgetName);
         if (widget)
         {
            widget->setPosition(CEGUI::UVector2(CEGUI::UDim(position.x(), position.y()),
               CEGUI::UDim(position.z(), position.w())));
         }
      }

      // Fire the "Out" link
      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetWidgetPosition::UpdateName()
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
   void SetWidgetPosition::OnLinkValueChanged(const std::string& linkName)
   {
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
   void SetWidgetPosition::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetPosition::GetLayout() const
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SetWidgetPosition::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetPosition::SetWidget(const std::string& value)
   {
      mWidget = value;

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetPosition::GetWidget() const
   {
      return mWidget;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SetWidgetPosition::GetWidgetList()
   {
      std::vector<std::string> list;
      RecurseWidgetList(list, GUINodeManager::GetLayout(GetString("Layout")));
      return list;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetWidgetPosition::RecurseWidgetList(std::vector<std::string>& widgetList, CEGUI::Window* parent)
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
   void SetWidgetPosition::SetPosition(const osg::Vec4& value)
   {
      mPosition = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 SetWidgetPosition::GetPosition() const
   {
      return mPosition;
   }
}

////////////////////////////////////////////////////////////////////////////////
