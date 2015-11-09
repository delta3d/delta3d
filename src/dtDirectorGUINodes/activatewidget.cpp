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

#include <dtCore/stringactorproperty.h>

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
   void ActivateWidget::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringActorProperty* widgetProp = new dtCore::StringActorProperty(
         "Widget", "Widget",
         dtCore::StringActorProperty::SetFuncType(this, &ActivateWidget::SetWidget),
         dtCore::StringActorProperty::GetFuncType(this, &ActivateWidget::GetWidget),
         "The Widget to activate.");
      AddProperty(widgetProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool ActivateWidget::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         std::string widgetName = GetString("Widget");

         CEGUI::Window* widget = gui->GetWidget(widgetName);
         if (widget)
         {
            widget->activate();
         }
         else
         {
            ActivateOutput("Failed");
            return false;
         }
      }

      // Fire the "Out" link
      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActivateWidget::UpdateName()
   {
      std::string widgetName = GetString("Widget");
      if (!widgetName.empty())
      {
         mName = widgetName;
      }
      else
      {
         mName.clear();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActivateWidget::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (!GetDirector()->IsLoading())
      {
         if (linkName == "Widget")
         {
            UpdateName();
         }
      }
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
}
////////////////////////////////////////////////////////////////////////////////
