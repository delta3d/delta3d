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

#include <dtDAL/stringactorproperty.h>
#include <dtDAL/stringselectoractorproperty.h>

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
   void SetWidgetText::OnFinishedLoading()
   {
      GUINodeManager::GetLayout(GetString("Layout"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringSelectorActorProperty* layoutProp = new dtDAL::StringSelectorActorProperty(
         "Layout", "Layout",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &SetWidgetText::SetLayout),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &SetWidgetText::GetLayout),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &SetWidgetText::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);
      
      dtDAL::StringSelectorActorProperty* widgetProp = new dtDAL::StringSelectorActorProperty(
         "Widget", "Widget",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &SetWidgetText::SetWidget),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &SetWidgetText::GetWidget),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &SetWidgetText::GetWidgetList),
         "The Widget to set the text on.", "", true);
      AddProperty(widgetProp);

      dtDAL::StringActorProperty* textProp = new dtDAL::StringActorProperty(
         "Text", "Text",
         dtDAL::StringActorProperty::SetFuncType(this, &SetWidgetText::SetText),
         dtDAL::StringActorProperty::GetFuncType(this, &SetWidgetText::GetText),
         "The Text.");
      AddProperty(textProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, widgetProp, false, false, true, false));
      mValues.push_back(ValueLink(this, textProp, false, false, true, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetWidgetText::Update(float simDelta, float delta, int input, bool firstUpdate)
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
         std::string text = GetString("Text");

         CEGUI::Window* widget = gui->GetWidget(widgetName);
         if (widget)
         {
            widget->setText(text.c_str());
         }
      }

      // Fire the "Out" link
      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::UpdateName()
   {
      std::string layoutName = GetString("Layout");
      std::string widgetName = GetString("Widget");
      std::string text = GetString("Text");

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
   void SetWidgetText::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetWidgetText::GetLayout() const
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SetWidgetText::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
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
   std::vector<std::string> SetWidgetText::GetWidgetList()
   {
      std::vector<std::string> list;
      RecurseWidgetList(list, GUINodeManager::GetLayout(GetString("Layout")));
      return list;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetWidgetText::RecurseWidgetList(std::vector<std::string>& widgetList, CEGUI::Window* parent)
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
