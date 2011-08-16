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

#include <dtDirectorGUINodes/setwidgettext.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/stringactorproperty.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetWidgetText::SetWidgetText()
      : ActionNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetWidgetText::~SetWidgetText()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if Widget could not be found."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();
      
      dtCore::StringActorProperty* widgetProp = new dtCore::StringActorProperty(
         "Widget", "Widget",
         dtCore::StringActorProperty::SetFuncType(this, &SetWidgetText::SetWidget),
         dtCore::StringActorProperty::GetFuncType(this, &SetWidgetText::GetWidget),
         "The Widget to set the text on.");
      AddProperty(widgetProp);

      dtCore::StringActorProperty* textProp = new dtCore::StringActorProperty(
         "Text", "Text",
         dtCore::StringActorProperty::SetFuncType(this, &SetWidgetText::SetText),
         dtCore::StringActorProperty::GetFuncType(this, &SetWidgetText::GetText),
         "The Text.");
      AddProperty(textProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
      mValues.push_back(ValueLink(this, textProp, false, false, true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetWidgetText::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         std::string widgetName = GetString("Widget");
         std::string text = GetString("Text");

         CEGUI::Window* widget = gui->GetWidget(widgetName);
         if (widget)
         {
            widget->setText(text.c_str());
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
   void SetWidgetText::UpdateName()
   {
      mName.clear();

      std::string widgetName = GetString("Widget");
      if (!widgetName.empty())
      {
         mName = widgetName;
      }

      std::string text = GetString("Text");
      if (!text.empty())
      {
         if (!mName.empty())
         {
            mName += " - ";
         }
         mName += "\"" + text + "\"";
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SetWidgetText::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (!GetDirector()->IsLoading())
      {
         if (linkName == "Widget" || linkName == "Text")
         {
            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::SetWidget(const std::string& value)
   {
      mWidget = value;
      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetText::GetWidget() const
   {
      return mWidget;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::SetText(const std::string& value)
   {
      mText = value;
      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetText::GetText() const
   {
      return mText;
   }
}

////////////////////////////////////////////////////////////////////////////////
