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

#include <dtDirectorGUINodes/activatewidget.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtDAL/stringactorproperty.h>
#include <dtDAL/stringselectoractorproperty.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   ActivateWidget::ActivateWidget()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   ActivateWidget::~ActivateWidget()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if Widget could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::OnFinishedLoading()
   {
      GUINodeManager::GetLayout(GetString("Layout"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringSelectorActorProperty* layoutProp = new dtDAL::StringSelectorActorProperty(
         "Layout", "Layout",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &ActivateWidget::SetLayout),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &ActivateWidget::GetLayout),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &ActivateWidget::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);

      dtDAL::StringSelectorActorProperty* widgetProp = new dtDAL::StringSelectorActorProperty(
         "Widget", "Widget",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &ActivateWidget::SetWidget),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &ActivateWidget::GetWidget),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &ActivateWidget::GetWidgetList),
         "The Widget to activate.", "", true);
      AddProperty(widgetProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ActivateWidget::Update(float simDelta, float delta, int input, bool firstUpdate)
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

         CEGUI::Window* widget = gui->GetWidget(widgetName);
         if (widget)
         {
            widget->activate();
         }
      }

      // Fire the "Out" link
      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::UpdateName()
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
   void ActivateWidget::OnLinkValueChanged(const std::string& linkName)
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
         else if (linkName == "Widget")
         {
            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ActivateWidget::GetLayout() const
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> ActivateWidget::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::SetWidget(const std::string& value)
   {
      mWidget = value;

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string ActivateWidget::GetWidget() const
   {
      return mWidget;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> ActivateWidget::GetWidgetList()
   {
      std::vector<std::string> list;
      RecurseWidgetList(list, GUINodeManager::GetLayout(GetString("Layout")));
      return list;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::RecurseWidgetList(std::vector<std::string>& widgetList, CEGUI::Window* parent)
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
}

////////////////////////////////////////////////////////////////////////////////
