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
 * Bradley Anderegg 06/28/2006
 */

#include <dtAI/waypointpair.h>
#include <osg/Vec2>
#include <cassert>

namespace dtAI
{

   WaypointPair::WaypointPair()
      : mFrom(0)
      , mTo(0)
      , m2DDistance(0)
      , m3DDistance(0)
   {
   }

   WaypointPair::WaypointPair(const WaypointInterface* pFrom, const WaypointInterface* pTo)
      : mFrom(pFrom)
      , mTo(pTo)
   {
      Calculate2DDistance();
      Calculate3DDistance();
      CalculateSlope();
   }

   WaypointPair::~WaypointPair()
   {
   }

   void WaypointPair::Calculate2DDistance()
   {
      m2DDistance = (
         osg::Vec2(mTo->GetPosition().x(), mTo->GetPosition().y()) -
         osg::Vec2(mFrom->GetPosition().x(), mFrom->GetPosition().y())
         ).length();
   }

   float WaypointPair::Get2DDistance() const
   {
      return m2DDistance;
   }

   void WaypointPair::Calculate3DDistance()
   {
      m3DDistance = osg::Vec3(mTo->GetPosition() - mFrom->GetPosition()).length();
   }

   float WaypointPair::Get3DDistance() const
   {
      return m3DDistance;
   }

   void WaypointPair::CalculateSlope()
   {
      if (GetTo()[0] - GetFrom()[0])
      {
         mSlope = 0;
      }
      else
      {
         mSlope = (GetTo()[1] - GetFrom()[1]) / (GetTo()[0] - GetFrom()[0]);
      }
   }

   float WaypointPair::GetSlope() const
   {
      return mSlope;
   }

   void WaypointPair::SetFrom(const WaypointInterface* pWaypoint)
   {
      assert(pWaypoint);
      mFrom = pWaypoint;
   }

   void WaypointPair::SetTo(const WaypointInterface* pWaypoint)
   {
      assert(pWaypoint);
      mTo = pWaypoint;
   }

   const WaypointInterface* WaypointPair::GetWaypointFrom() const
   {
      return mFrom;
   }

   const WaypointInterface* WaypointPair::GetWaypointTo() const
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

} // namespace dtAI
