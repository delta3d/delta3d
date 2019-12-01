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
   void GetWidgetPosition::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringActorProperty* widgetProp = new dtCore::StringActorProperty(
         "Widget", "Widget",
         dtCore::StringActorProperty::SetFuncType(this, &GetWidgetPosition::SetWidget),
         dtCore::StringActorProperty::GetFuncType(this, &GetWidgetPosition::GetWidget),
         "The Widget to set the text on.");
      AddProperty(widgetProp);

      mpPositionProp = new dtCore::Vec4ActorProperty(
         "Position", "Position",
         dtCore::Vec4ActorProperty::SetFuncType(this, &GetWidgetPosition::SetPosition),
         dtCore::Vec4ActorProperty::GetFuncType(this, &GetWidgetPosition::GetPosition),
         "The position of the widget in Unified Dims (xScale, xDim, yScale, yDim).");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
      mValues.push_back(ValueLink(this, mpPositionProp, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetWidgetPosition::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
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
   void GetWidgetPosition::UpdateName()
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
   void GetWidgetPosition::OnLinkValueChanged(const std::string& linkName)
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
