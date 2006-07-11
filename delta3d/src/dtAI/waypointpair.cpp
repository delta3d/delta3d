/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * @author Bradley Anderegg 06/28/2006
 */

#include <dtAI/waypointpair.h>
#include <cassert>


namespace dtAI
{
WaypointPair::WaypointPair()
   : mFrom(0)
   , mTo(0)
   , m2DDistance(0)
{
}

WaypointPair::WaypointPair(const Waypoint* pFrom, const Waypoint* pTo)
   : mFrom(pFrom)
   , mTo(pTo)
{
   Calculate2DDistance();
   CalculateSlope();
}

WaypointPair::~WaypointPair()
{
}



osg::Vec3 WaypointPair::ClosestPointTo(const osg::Vec3& pPoint)
{
   //\todo implement me
   assert(0);
   return pPoint;
}

void WaypointPair::Calculate2DDistance()
{
   m2DDistance = sqrtf(osg::square(GetTo()[0] - GetFrom()[0]) + osg::square(GetTo()[1] - GetFrom()[1]));
}


void WaypointPair::CalculateSlope()
{
   if(GetTo()[0] - GetFrom()[0]) mSlope = 0;
   else mSlope = (GetTo()[1] - GetFrom()[1]) / (GetTo()[0] - GetFrom()[0]);
}

float WaypointPair::GetSlope() const
{
   return mSlope;
}

float WaypointPair::Get2DDistance() const
{
   return m2DDistance;
}

void WaypointPair::SetFrom(const Waypoint* pWaypoint)
{
   assert(pWaypoint);
   mFrom = pWaypoint;
}

void WaypointPair::SetTo(const Waypoint* pWaypoint)
{
   assert(pWaypoint);
   mTo = pWaypoint;
}

const Waypoint* WaypointPair::GetWaypointFrom() const
{
   return mFrom;
}

const Waypoint* WaypointPair::GetWaypointTo() const 
{
   return mTo;
}

const osg::Vec3& WaypointPair::GetFrom() const
{
   return mFrom->GetPosition();
}

const osg::Vec3& WaypointPair::GetTo() const
{
   return mTo->GetPosition();
}

}//namespace dtAI
