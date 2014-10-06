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
 * Author: Jeff P. Houde
 */

#include <dtDirectorGUINodes/guinodemanager.h>

#include <dtABC/application.h>

#include <dtGUI/scriptmodule.h>
#include <CEGUI/CEGUIExceptions.h>

#include <dtUtil/log.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   GUINodeManager::GUINodeManager()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   GUINodeManager::~GUINodeManager()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   dtGUI::GUI* GUINodeManager::GetGUI()
   {
      if (dtGUI::GUI::GetInstanceCount() == 0)
      {
         try
         {
            dtGUI::GUI::SetDefaultParser("XercesParser");

            dtCore::Camera* camera = NULL;
            dtCore::Keyboard* keyboard = NULL;
            dtCore::Mouse* mouse = NULL;

            dtABC::Application* app = dtABC::Application::GetInstance(0);
            if (app)
            {
               camera = app->GetCamera();
               keyboard = app->GetKeyboard();
               mouse = app->GetMouse();
            }

            return new dtGUI::GUI(camera, keyboard, mouse);
         }
         catch(CEGUI::Exception& e)
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,
               "CEGUI::%s", e.getMessage().c_str() );
         }
      }

      if (dtGUI::GUI::GetInstanceCount() > 0)
      {
         return dtGUI::GUI::GetInstance(0);
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Window* GUINodeManager::GetLayout(const std::string& name)
   {
      dtGUI::GUI* gui = GetGUI();
      if (gui)
      {
         std::map<std::string, CEGUI::Window*>::const_iterator iter = gui->GetLayoutMap().find(name);
         if (iter != gui->GetLayoutMap().end())
         {
            return iter->second;
         }

         try
         {
            CEGUI::Window* layout = gui->LoadLayout(name);

            if (layout)
            {
               layout->hide();
               return layout;
            }
         }
         catch(CEGUI::Exception& e)
         {
            dtUtil::Log::GetInstance().LogMessage(DT_LOG_SOURCE, 
               "CEGUI::" + std::string(e.getMessage().c_str()),
               dtUtil::Log::LOG_WARNING);
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<std::string> GUINodeManager::GetLayoutList()
   {
      std::vector<std::string> list;

      dtGUI::GUI* gui = GetGUI();
      if (gui)
      {
         std::map<std::string, CEGUI::Window*>::const_iterator iter;
         for (iter = gui->GetLayoutMap().begin(); iter != gui->GetLayoutMap().end(); ++iter)
         {
            list.push_back(iter->first);
         }
      }

      return list;
   }
}

//////////////////////////////////////////////////////////////////////////
