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

#include <dtDirectorGUINodes/setguicursor.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/stringactorproperty.h>

#include <dtGUI/gui.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetGUICursor::SetGUICursor()
      : ActionNode()
      , mImageSet("WindowsLook")
      , mImage("MouseArrow")
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetGUICursor::~SetGUICursor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetGUICursor::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if the GUI cursor could not be set."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetGUICursor::OnFinishedLoading()
   {
      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetGUICursor::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringActorProperty* imageSetProp =
         new dtCore::StringActorProperty(
         "Image Set", "Image Set",
         dtCore::StringActorProperty::SetFuncType(this, &SetGUICursor::SetImageSet),
         dtCore::StringActorProperty::GetFuncType(this, &SetGUICursor::GetImageSet),
         "The image set.");
      AddProperty(imageSetProp);
      
      dtCore::StringActorProperty* imageProp =
         new dtCore::StringActorProperty(
         "Image", "Image",
         dtCore::StringActorProperty::SetFuncType(this, &SetGUICursor::SetImage),
         dtCore::StringActorProperty::GetFuncType(this, &SetGUICursor::GetImage),
         "The arrow image file.");
      AddProperty(imageProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, imageSetProp, false, true, true, false));
      mValues.push_back(ValueLink(this, imageProp, false, true, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetGUICursor::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      std::string imageSet = GetString("Image Set");
      std::string image = GetString("Image");
      dtGUI::GUI* gui = GUINodeManager::GetGUI();
      if (gui)
      {
         gui->SetMouseCursor(imageSet, image);
         return ActionNode::Update(simDelta, delta, input, firstUpdate);
      }

      ActivateOutput("Failed");
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetGUICursor::UpdateName()
   {
      std::string imageSet = GetString("Image Set");
      std::string image    = GetString("Image");

      mName.clear();

      if (!imageSet.empty())
      {
         mName = imageSet;

         if (!image.empty())
         {
            mName += "::";
         }
      }

      if (!image.empty())
      {
         mName += image;
      }

      if (mName.empty())
      {
         mName = "<None>";
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SetGUICursor::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (!GetDirector()->IsLoading())
      {
         if (linkName == "Image Set" ||
            linkName == "Image")
         {
            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetGUICursor::SetImageSet(const std::string& value)
   {
      mImageSet = value;

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetGUICursor::GetImageSet()
   {
      return mImageSet;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetGUICursor::SetImage(const std::string& value)
   {
      mImage = value;

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string SetGUICursor::GetImage()
   {
      return mImage;
   }
}

////////////////////////////////////////////////////////////////////////////////
