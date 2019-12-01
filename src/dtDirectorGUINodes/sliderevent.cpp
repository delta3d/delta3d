/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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

#include <dtDirectorGUINodes/sliderevent.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/intactorproperty.h>
#include <dtCore/stringselectoractorproperty.h>

#include <dtGUI/gui.h>
#include <dtGUI/scriptmodule.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUISlider.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   SliderEvent::SliderEvent()
       : EventNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   SliderEvent::~SliderEvent()
   {
      DestroyConnection(mValueChangedConnection);
      DestroyConnection(mThumbTrackStartedConnection);
      DestroyConnection(mThumbTrackEndedConnection);
   }

   /////////////////////////////////////////////////////////////////////////////
   void SliderEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      EventNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Value Changed", "Activated when a slider value has changed."));
      mOutputs.push_back(OutputLink(this, "Thumb Track Started", "Activated when a slider thumb track has started to change."));
      mOutputs.push_back(OutputLink(this, "Thumb Track Ended", "Activated when the GUI slider has stopped changing."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SliderEvent::OnStart()
   {
      GUINodeManager::GetLayout(GetString("Layout"));
      RefreshConnections();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SliderEvent::OnFinishedLoading()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SliderEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* layoutProp =
         new dtCore::StringSelectorActorProperty(
         "Layout", "Layout",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SliderEvent::SetLayout),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SliderEvent::GetLayout),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &SliderEvent::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);

      dtCore::StringSelectorActorProperty* sliderProp =
         new dtCore::StringSelectorActorProperty(
         "Slider", "Slider",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SliderEvent::SetSlider),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SliderEvent::GetSlider),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &SliderEvent::GetSliderList),
         "The Slider.", "", true);
      AddProperty(sliderProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, sliderProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SliderEvent::UsesActorFilters()
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void SliderEvent::OnLinkValueChanged(const std::string& linkName)
   {
      EventNode::OnLinkValueChanged(linkName);

      if (!GetDirector()->IsLoading())
      {
         UpdateName();

         if (linkName == "Layout")
         {
            std::string layoutName = GetString("Layout");
            GUINodeManager::GetLayout(layoutName);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SliderEvent::UpdateName()
   {
      std::string layoutName = GetString("Layout");
      std::string sliderName = GetString("Slider");

      if (layoutName.empty() && sliderName.empty())
      {
         mName.clear();
      }
      else if (sliderName.empty())
      {
         mName = "Layout: " + layoutName;
      }
      else if (layoutName.empty())
      {
         mName = "Slider: " + sliderName;
      }
      else
      {
         mName = layoutName + "::" + sliderName;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SliderEvent::RefreshConnections()
   {
      DestroyConnection(mValueChangedConnection);
      DestroyConnection(mThumbTrackStartedConnection);
      DestroyConnection(mThumbTrackEndedConnection);

      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         std::string sliderName = GetString("Slider");

         if (!sliderName.empty())
         {
            mValueChangedConnection = gui->SubscribeEvent(sliderName,
               CEGUI::Slider::EventValueChanged.c_str(),
               dtGUI::GUI::Subscriber(&SliderEvent::OnValueChanged, this));
            mThumbTrackStartedConnection = gui->SubscribeEvent(sliderName,
               CEGUI::Slider::EventThumbTrackStarted.c_str(),
               dtGUI::GUI::Subscriber(&SliderEvent::OnThumbTrackStarted, this));
            mThumbTrackEndedConnection = gui->SubscribeEvent(sliderName,
               CEGUI::Slider::EventThumbTrackEnded.c_str(),
               dtGUI::GUI::Subscriber(&SliderEvent::OnThumbTrackEnded, this));
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SliderEvent::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SliderEvent::GetLayout()
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SliderEvent::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SliderEvent::SetSlider(const std::string& value)
   {
      mSlider = value;

      UpdateName();

      if (GetDirector()->HasStarted())
      {
         RefreshConnections();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SliderEvent::GetSlider()
   {
      return mSlider;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SliderEvent::GetSliderList()
   {
      std::vector<std::string> stringList;

      CEGUI::Window* layout = GUINodeManager::GetLayout(GetString("Layout"));
      RecurseSliderList(stringList, layout);

      return stringList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SliderEvent::RecurseSliderList(std::vector<std::string>& sliderList, CEGUI::Window* parent)
   {
      if (!parent)
      {
         return;
      }

      size_t count = parent->getChildCount();
      for (size_t index = 0; index < count; ++index)
      {
         CEGUI::Window* child = parent->getChildAtIdx(index);
         CEGUI::Slider* slider = dynamic_cast<CEGUI::Slider*>(child);

         if (slider)
         {
            sliderList.push_back(slider->getName().c_str());
         }

         RecurseSliderList(sliderList, child);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SliderEvent::OnValueChanged(const CEGUI::EventArgs& e)
   {
      Trigger("Value Changed");
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SliderEvent::OnThumbTrackStarted(const CEGUI::EventArgs& e)
   {
      Trigger("Thumb Track Started");
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SliderEvent::OnThumbTrackEnded(const CEGUI::EventArgs& e)
   {
      Trigger("Thumb Track Ended");
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SliderEvent::DestroyConnection(CEGUI::Event::Connection& connection)
   {
      if (connection.isValid())
      {
         connection->disconnect();
         connection = NULL;
      }
   }
}
