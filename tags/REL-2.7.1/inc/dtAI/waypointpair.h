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

#ifndef __DELTA_WAYPOINTPAIR_H__
#define __DELTA_WAYPOINTPAIR_H__

#include <dtAI/export.h>
#include <dtAI/waypointinterface.h>

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
      WaypointPair(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      WaypointPair();
      virtual ~WaypointPair();

      /**
       * calculates distance between the two points on the XY plane
       */
      void Calculate2DDistance();
      float Get2DDistance() const;

      /**
       * calculates distance between the two points in 3D space
       */
      void Calculate3DDistance();
      float Get3DDistance() const;

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
      void SetFrom(const WaypointInterface* pWaypoint);
      void SetTo(const WaypointInterface* pWaypoint);

      /**
       * Getters for the waypoints
       */
      const WaypointInterface* GetWaypointFrom() const;
      const WaypointInterface* GetWaypointTo() const;

      /**
       * Getters that return a reference the the actual position
       * just for convenience really
       */
      const osg::Vec3& GetFrom() const;
      const osg::Vec3& GetTo() const;

   private:
      const WaypointInterface* mFrom;
      const WaypointInterface* mTo;

      float m2DDistance;
      float m3DDistance;
      float mSlope;
   };

} // namespace dtAI

#endif // __DELTA_WAYPOINTPAIR_H__
