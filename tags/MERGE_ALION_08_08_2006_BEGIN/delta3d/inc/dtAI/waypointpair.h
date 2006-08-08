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

#ifndef __DELTA_WAYPOINTPAIR_H__
#define __DELTA_WAYPOINTPAIR_H__


#include "export.h"
#include "waypoint.h"

namespace dtAI
{
   /**
    * A WaypointPair is used to represent a pair of waypoints where
    * the path between them is traversable.  Also used as a helper
    * for computing costs between waypoints.
    */
   class DT_AI_EXPORT WaypointPair
   {
   public:
      /**
      * constructor assigns the two waypoints, calculates slope and 2D Distance
      */
      WaypointPair(const Waypoint* pFrom, const Waypoint* pTo);      
      WaypointPair();
      virtual ~WaypointPair();

      /**
      * returns the closest point to the path between these two 
      * waypoints, currently unused and unimplemented, but seems
      * like it could be useful.
      */
      osg::Vec3 ClosestPointTo(const osg::Vec3& pPoint);

      /**
      * calculates distance on the X-Y Plane between the two points
      */
      void Calculate2DDistance();
      float Get2DDistance() const;

      /**
      * Calculates the slope in the Z Plane
      */
      void CalculateSlope();
      float GetSlope() const;

      /**
      * Setters for the From and To waypoints
      * if this is used remember to call Calculate2DDistance()
      * and CalculateSlope()
      */
      void SetFrom(const Waypoint* pWaypoint);
      void SetTo(const Waypoint* pWaypoint);

      /**
      * Getters for the waypoints
      */
      const Waypoint* GetWaypointFrom() const;
      const Waypoint* GetWaypointTo() const;

      /**
      * Getters that return a reference the the actual position
      * just for convenience really
      */
      const osg::Vec3& GetFrom() const;
      const osg::Vec3& GetTo() const;

   private:

      const Waypoint* mFrom;
      const Waypoint* mTo;

      float m2DDistance;
      float mSlope;
   
   };
}//namespace dtAI

#endif // __DELTA_WAYPOINTPAIR_H__
