/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Eric Heine
 */

#include "aiutilityinputcomponent.h"
#include "waypointselection.h"

#include <dtABC/application.h>
#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointgraph.h>

#include <iostream>
////////////////////////////////////////////////////////////////////////////////
AIUtilityInputComponent::AIUtilityInputComponent(const std::string &name /*= "AIUtilityInputComponent"*/) 
: dtGame::BaseInputComponent(name)
{
}

////////////////////////////////////////////////////////////////////////////////
AIUtilityInputComponent::~AIUtilityInputComponent()
{
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityInputComponent::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   bool handled = false;

   switch (button)
   {
      case dtCore::Mouse::LeftButton:
      {
         osg::Vec3f pickedPosition;
         GetGameManager()->GetApplication().GetView()->GetMousePickPosition(pickedPosition);

         // See if we are adding to the selection list or not
         dtCore::Keyboard* keyboard = GetGameManager()->GetApplication().GetKeyboard();
         bool isShiftHeld = keyboard && (keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Shift_L) ||
            keyboard->GetKeyState(osgGA::GUIEventAdapter::KEY_Shift_R));
         if (!isShiftHeld)
         {
            WaypointSelection::GetInstance().DeselectAllWaypoints();
         }

         // Try and find the waypoint we clicked on if any
         if (mpAIInterface.valid())
         {
            dtAI::WaypointInterface* waypointInterface = mpAIInterface->GetClosestWaypoint(pickedPosition, 0.5f);
            if (waypointInterface != NULL)
            {
               WaypointSelection::GetInstance().ToggleWaypointSelection(waypointInterface);
            }
         }

         break;
      }
      default:
         break;
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::SetAIPluginInterface(dtAI::AIPluginInterface* aiInterface)
{
   mpAIInterface = aiInterface;
}

////////////////////////////////////////////////////////////////////////////////

