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

#include <dtDirectorGUINodes/loadguischeme.h>
#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtCore/stringactorproperty.h>

#include <dtGUI/gui.h>
#include <CEGUI/CEGUIExceptions.h>

#include <dtDirector/director.h>

#include <osgDB/FileNameUtils>


namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   LoadGUIScheme::LoadGUIScheme()
      : ActionNode()
      , mScheme("WindowsLook.scheme")
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   LoadGUIScheme::~LoadGUIScheme()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void LoadGUIScheme::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.push_back(OutputLink(this, "Failed", "Activates if the GUI scheme could not be loaded."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void LoadGUIScheme::OnFinishedLoading()
   {
      std::string schemeFile = GetString("Scheme");
      if (!schemeFile.empty())
      {
         if (GetDirector()->GetGameManager())
         {
            std::string scheme = osgDB::getNameLessExtension(schemeFile);

            dtGUI::GUI* gui = GUINodeManager::GetGUI();
            if (gui && !gui->IsSchemePresent(scheme))
            {
               try
               {
                  gui->LoadScheme(schemeFile);
               }
               catch(CEGUI::Exception& e)
               {
                  LOG_ERROR(std::string("Error loading cegui scheme: ") + e.getMessage().c_str() + " " + e.getFileName().c_str() + ":" + dtUtil::ToString(e.getLine()));
               }
            }
         }
      }

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LoadGUIScheme::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtCore::StringActorProperty* schemeProp =
         new dtCore::StringActorProperty(
         "Scheme", "Scheme",
         dtCore::StringActorProperty::SetFuncType(this, &LoadGUIScheme::SetScheme),
         dtCore::StringActorProperty::GetFuncType(this, &LoadGUIScheme::GetScheme),
         "The Scheme.");
      AddProperty(schemeProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, schemeProp, false, true, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool LoadGUIScheme::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      //std::string schema = GetString("Scheme");
      //if (!schema.empty())
      //{
      //   dtGUI::GUI* gui = GUINodeManager::GetGUI();
      //   if (gui)
      //   {
      //      gui->LoadScheme(schema);
            return ActionNode::Update(simDelta, delta, input, firstUpdate);
      //   }
      //}

      //ActivateOutput("Failed");
      //return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LoadGUIScheme::UpdateName()
   {
      std::string scheme = GetString("Scheme");

      if (scheme.empty())
      {
         mName.clear();
      }
      else
      {
         mName = "Scheme: " + scheme;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void LoadGUIScheme::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      if (!GetDirector()->IsLoading())
      {
         if (linkName == "Scheme")
         {
            UpdateName();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void LoadGUIScheme::SetScheme(const std::string& value)
   {
      mScheme = value;

      UpdateName();
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string LoadGUIScheme::GetScheme()
   {
      return mScheme;
   }
}

////////////////////////////////////////////////////////////////////////////////
