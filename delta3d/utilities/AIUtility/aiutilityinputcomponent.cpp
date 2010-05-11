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
            mSelectedWaypointList.clear();
         }

         // Try and find the waypoint we clicked on if any
         if (mpAIInterface.valid())
         {
            dtAI::WaypointInterface* waypointInterface = mpAIInterface->GetClosestWaypoint(pickedPosition, 0.5f);
            if (waypointInterface != NULL)
            {
               std::vector<dtAI::WaypointInterface*>::iterator previousSelection =
                  std::find(mSelectedWaypointList.begin(), mSelectedWaypointList.end(), waypointInterface);
               if (previousSelection == mSelectedWaypointList.end())
               {
                  mSelectedWaypointList.push_back(waypointInterface);
               }
               else // Deselect it since it was already in the list
               {
                  mSelectedWaypointList.erase(previousSelection);
               }
            }
         }

         // Update UI property list with our new selection
         emit WaypointSelectionChanged(mSelectedWaypointList);

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
void AIUtilityInputComponent::OnAddEdge()
{
   if(mSelectedWaypointList.size() == 2)
   {
      // Update NavMesh
      dtAI::WaypointInterface* waypointA = mSelectedWaypointList[0];
      dtAI::WaypointInterface* waypointB = mSelectedWaypointList[1];
      mpAIInterface->AddEdge(waypointA->GetID(), waypointB->GetID());

      // Update UI
      emit WaypointSelectionChanged(mSelectedWaypointList);
      mpAIInterface->GetDebugDrawable()->AddEdge(waypointA, waypointB);
   }
   else
   {
      LOG_ERROR("Trying to add edge with too many or too few waypoints selected.");
   }
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityInputComponent::OnRemoveEdge()
{
   if(mSelectedWaypointList.size() == 2)
   {
      // Update NavMesh
      dtAI::WaypointInterface* waypointA = mSelectedWaypointList[0];
      dtAI::WaypointInterface* waypointB = mSelectedWaypointList[1];
      mpAIInterface->RemoveEdge(waypointA->GetID(), waypointB->GetID());

      // Update UI
      emit WaypointSelectionChanged(mSelectedWaypointList);
      mpAIInterface->GetDebugDrawable()->RemoveEdge(waypointA, waypointB);
   }
   else
   {
      LOG_ERROR("Trying to remove edge with too many or too few waypoints selected.");
   }
}

////////////////////////////////////////////////////////////////////////////////

