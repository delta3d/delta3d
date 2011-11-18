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

#include <dtDirectorGUINodes/setlayoutvisibility.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/stringselectoractorproperty.h>
#include <dtCore/floatactorproperty.h>

#include <dtUtil/mathdefines.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIWindow.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetLayoutVisibility::SetLayoutVisibility()
      : LatentActionNode()
      , mElapsedTime(0.0f)
      , mFadeTime(0.0f)
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetLayoutVisibility::~SetLayoutVisibility()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      LatentActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Show"));
      mInputs.push_back(InputLink(this, "Hide"));

      mOutputs.push_back(OutputLink(this, "Finished"));
      mOutputs.push_back(OutputLink(this, "Failed"));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::OnFinishedLoading()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::BuildPropertyMap()
   {
      LatentActionNode::BuildPropertyMap();

      dtCore::StringSelectorActorProperty* layoutProp = new dtCore::StringSelectorActorProperty(
         "Layout", "Layout",
         dtCore::StringSelectorActorProperty::SetFuncType(this, &SetLayoutVisibility::SetLayout),
         dtCore::StringSelectorActorProperty::GetFuncType(this, &SetLayoutVisibility::GetLayout),
         dtCore::StringSelectorActorProperty::GetListFuncType(this, &SetLayoutVisibility::GetLayoutList),
         "The Layout.", "", true);
      AddProperty(layoutProp);

      dtCore::FloatActorProperty* fadeProp = new dtCore::FloatActorProperty(
         "Fade Time", "Fade Time",
         dtCore::FloatActorProperty::SetFuncType(this, &SetLayoutVisibility::SetFadeTime),
         dtCore::FloatActorProperty::GetFuncType(this, &SetLayoutVisibility::GetFadeTime),
         "The amount of time to fade the layout in or out of view.");
      AddProperty(fadeProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, layoutProp, false, false, true, false));
      mValues.push_back(ValueLink(this, fadeProp, false, false, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetLayoutVisibility::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      CEGUI::Window* layout = GUINodeManager::GetLayout(GetString("Layout"));
      if (!layout)
      {
         ActivateOutput("Failed");
         return false;
      }

      float fadeTime = GetFloat("Fade Time");

      if (firstUpdate)
      {
         mElapsedTime = 0.0f;

         switch (input)
         {
         case INPUT_SHOW:
            {
               layout->show();
               layout->activate();
               layout->setAlpha(0.0f);
            }
            break;
         case INPUT_HIDE:
            {
               layout->setAlpha(1.0f);
            }
            break;
         }

         LatentActionNode::Update(simDelta, delta, input, firstUpdate);
      }

      mElapsedTime = dtUtil::Min(mElapsedTime + delta, fadeTime);

      switch (input)
      {
      case INPUT_SHOW:
         {
            float alpha = 1.0f;
            if (fadeTime > 0.0f)
            {
               alpha = mElapsedTime / fadeTime;
            }
            layout->setAlpha(alpha);

            if (alpha >= 1.0f)
            {
               ActivateOutput("Finished");
               return false;
            }
         }
         break;
      case INPUT_HIDE:
         {
            float alpha = 0.0f;
            if (fadeTime > 0.0f)
            {
               alpha = 1.0f - (mElapsedTime / fadeTime);
            }
            layout->setAlpha(alpha);

            if (alpha <= 0.0f)
            {
               layout->hide();
               ActivateOutput("Finished");
               return false;
            }
         }
         break;
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::UpdateName()
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
   void SetLayoutVisibility::OnLinkValueChanged(const std::string& linkName)
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
   void SetLayoutVisibility::SetLayout(const std::string& value)
   {
      mLayout = value;

      UpdateName();

      if (!GetDirector()->IsLoading())
      {
         GUINodeManager::GetLayout(mLayout);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetLayoutVisibility::GetLayout() const
   {
      return mLayout;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> SetLayoutVisibility::GetLayoutList()
   {
      return GUINodeManager::GetLayoutList();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetLayoutVisibility::SetFadeTime(float value)
   {
      mFadeTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float SetLayoutVisibility::GetFadeTime() const
   {
      return mFadeTime;
   }
}

////////////////////////////////////////////////////////////////////////////////
