/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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

#include "waypointselection.h"

WaypointSelection* WaypointSelection::sInstance = NULL;

///////////////////////////////////////////////////////////////////////////////
WaypointSelection& WaypointSelection::GetInstance()
{
   if (WaypointSelection::sInstance == NULL)
   {
      WaypointSelection::sInstance = new WaypointSelection();
   }

   return *WaypointSelection::sInstance;
}

////////////////////////////////////////////////////////////////////////////////
bool WaypointSelection::HasWaypoint(dtAI::WaypointInterface* waypoint)
{
   return std::find(mSelectedWaypointList.begin(), mSelectedWaypointList.end(), waypoint) != 
                    mSelectedWaypointList.end();
}

////////////////////////////////////////////////////////////////////////////////
void WaypointSelection::AddWaypointToSelection(dtAI::WaypointInterface* waypoint)
{
   if (HasWaypoint(waypoint))
   {
      return;
   }
   
   mSelectedWaypointList.push_back(waypoint);
   emit WaypointSelectionChanged(mSelectedWaypointList);
}

////////////////////////////////////////////////////////////////////////////////
void WaypointSelection::AddWaypointListToSelection(const std::vector<dtAI::WaypointInterface*>& waypointList)
{
   std::vector<dtAI::WaypointInterface*>::const_iterator itr = waypointList.begin();

   //ensure the supplied waypoints aren't aready selected
   std::vector<dtAI::WaypointInterface*> prunedList;
   while (itr != waypointList.end())
   {
      if (HasWaypoint(*itr) == false)
      {
         prunedList.push_back((*itr));
      }
      ++itr;
   }

   std::copy(prunedList.begin(), prunedList.end(), std::back_inserter(mSelectedWaypointList));
   emit WaypointSelectionChanged(mSelectedWaypointList);
}

////////////////////////////////////////////////////////////////////////////////
void WaypointSelection::DeselectWaypoint(dtAI::WaypointInterface* waypoint)
{
   mSelectedWaypointList.erase(std::find(mSelectedWaypointList.begin(), mSelectedWaypointList.end(), waypoint));
   emit WaypointSelectionChanged(mSelectedWaypointList);
}

////////////////////////////////////////////////////////////////////////////////
void WaypointSelection::DeselectAllWaypoints()
{
   mSelectedWaypointList.clear();
   emit WaypointSelectionChanged(mSelectedWaypointList);
}

////////////////////////////////////////////////////////////////////////////////
void WaypointSelection::ToggleWaypointSelection(dtAI::WaypointInterface* waypoint)
{
   std::vector<dtAI::WaypointInterface*>::iterator currentSelection =
      std::find(mSelectedWaypointList.begin(), mSelectedWaypointList.end(), waypoint);

   if (currentSelection != mSelectedWaypointList.end())
   {
      // In order to avoid calling std::find again, do the erase here instead of DeselectWaypoint
      mSelectedWaypointList.erase(currentSelection);
      emit WaypointSelectionChanged(mSelectedWaypointList);
   }
   else
   {
      AddWaypointToSelection(waypoint);
   }
}

////////////////////////////////////////////////////////////////////////////////
size_t WaypointSelection::GetNumberSelected()
{
   return mSelectedWaypointList.size();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<dtAI::WaypointInterface*>& WaypointSelection::GetWaypointList() 
{
   return mSelectedWaypointList;
}

////////////////////////////////////////////////////////////////////////////////
void WaypointSelection::SetWaypointSelectionList(std::vector<dtAI::WaypointInterface*>& waypointList)
{
   mSelectedWaypointList.clear();
   mSelectedWaypointList = waypointList;

   emit WaypointSelectionChanged(mSelectedWaypointList);
}
