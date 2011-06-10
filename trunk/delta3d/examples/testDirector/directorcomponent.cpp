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
 * MG
 */

#include "directorcomponent.h"

#include <dtABC/application.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtCore/deltawin.h>
#include <dtCore/transform.h>
#include <dtCore/collisionmotionmodel.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtActors/engineactorregistry.h>
#include <dtDirector/director.h>

const std::string& DirectorComponent::NAME = "DirectorComponent";

////////////////////////////////////////////////////////////////////////////////

DirectorComponent::DirectorComponent(const std::string& name)
   : dtGame::BaseInputComponent(name)
{
}

////////////////////////////////////////////////////////////////////////////////
DirectorComponent::~DirectorComponent()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorComponent::ProcessMessage(const dtGame::Message& message)
{
   if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
   {
      OnMapLoaded();
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DirectorComponent::HandleKeyPressed(const dtCore::Keyboard* keyBoard, int key)
{
   bool handled = false;

   switch (key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
      {
         GetGameManager()->GetApplication().Quit();
         return true;
      }
   case '~':
      {
         GetGameManager()->GetApplication().SetNextStatisticsType();
         return true;
      }
   default:
      break;
   };

   if (!handled)
   {
      return GetGameManager()->GetApplication().KeyPressed(keyBoard, key);
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorComponent::OnMapLoaded()
{
}

////////////////////////////////////////////////////////////////////////////////
