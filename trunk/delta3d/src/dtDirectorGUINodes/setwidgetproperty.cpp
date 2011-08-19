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

#include <dtDirectorGUINodes/setwidgetproperty.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/stringactorproperty.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIExceptions.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetWidgetProperty::SetWidgetProperty()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetWidgetProperty::~SetWidgetProperty()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetProperty::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if Widget could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetProperty::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringActorProperty* widgetProp = new dtCore::StringActorProperty(
         "Widget", "Widget",
         dtCore::StringActorProperty::SetFuncType(this, &SetWidgetProperty::SetWidget),
         dtCore::StringActorProperty::GetFuncType(this, &SetWidgetProperty::GetWidget),
         "The Widget to set the text on.");
      AddProperty(widgetProp);

      dtCore::StringActorProperty* propertyProp = new dtCore::StringActorProperty(
         "Property", "Property",
         dtCore::StringActorProperty::SetFuncType(this, &SetWidgetProperty::SetProperty),
         dtCore::StringActorProperty::GetFuncType(this, &SetWidgetProperty::GetProperty),
         "The property to set.");
      AddProperty(propertyProp);

      dtCore::StringActorProperty* valueProp = new dtCore::StringActorProperty(
         "Value", "Value",
         dtCore::StringActorProperty::SetFuncType(this, &SetWidgetProperty::SetValue),
         dtCore::StringActorProperty::GetFuncType(this, &SetWidgetProperty::GetValue),
         "The value to set.");
      AddProperty(valueProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
      mValues.push_back(ValueLink(this, propertyProp));
      mValues.push_back(ValueLink(this, valueProp, false, false, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetWidgetProperty::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         std::string widgetName = GetString("Widget");
         std::string prop = GetString("Property");
         std::string val = GetString("Value");

         CEGUI::Window* widget = gui->GetWidget(widgetName);
         if (widget)
         {
            try
            {
               widget->setProperty(prop, val);
            }
            catch (CEGUI::UnknownObjectException e)
            {
               LOG_ERROR("There is no property named " + prop);
               ActivateOutput("Failed");
               return false;
            }
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
   void SetWidgetProperty::UpdateName()
   {
      mName.clear();

      std::string widgetName = GetString("Widget");
      if (!widgetName.empty())
      {
         mName = widgetName;
      }

      std::string prop = GetString("Property");
      if (!prop.empty())
      {
         if (!mName.empty())
         {
            mName += " - ";
         }
         mName += prop;
      }

      std::string val = GetString("Value");
      if (!val.empty())
      {
         if (!mName.empty())
         {
            mName += " = ";
         }
         mName += val;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SetWidgetProperty::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (!GetDirector()->IsLoading())
      {
         if (linkName == "Widget" || linkName == "Property" || linkName == "Value")
         {
            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetProperty::SetWidget(const std::string& value)
   {
      mWidget = value;
      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetProperty::GetWidget() const
   {
      return mWidget;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetProperty::SetProperty(const std::string& value)
   {
      mProperty = value;
      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetProperty::GetProperty() const
   {
      return mProperty;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetProperty::SetValue(const std::string& value)
   {
      mValue = value;
      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetProperty::GetValue() const
   {
      return mValue;
   }
}

////////////////////////////////////////////////////////////////////////////////
