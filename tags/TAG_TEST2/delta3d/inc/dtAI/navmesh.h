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
 * Bradley Anderegg 06/27/2006
 */

#ifndef __DELTA_NAVMESH_H__
#define __DELTA_NAVMESH_H__

#include <dtAI/export.h>
#include <dtAI/primitives.h>
#include <osg/Referenced>
#include <map>

namespace dtAI
{
   class WaypointInterface;
   class WaypointPair;

   /**
    * A NavMesh is the collection of all WaypointPairs
    * This is used for finding traversal paths between points
    */
   class DT_AI_EXPORT NavMesh : public osg::Referenced
   {
   public:
      typedef std::pair<WaypointID, WaypointPair*> NavMeshPair;
      typedef std::multimap<WaypointID, WaypointPair*> NavMeshContainer;

   public:
      NavMesh();
      NavMesh(NavMeshContainer::iterator from, NavMeshContainer::iterator to);

   protected:
      ~NavMesh();

   public:
      void InsertCopy(NavMeshContainer::const_iterator from, NavMeshContainer::const_iterator to);
      void Remove(NavMeshContainer::iterator from, NavMeshContainer::iterator to);

      /**
       * @note For consistency Path and PathSegment are being replaced by the word Edge,
       *       the Add,Remove and such using the old nomenclature will be deprecated.
       */

      /**
       * Creates a new WaypointPair and adds it to the current set
       */
      void AddEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void AddPathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo);

      /**
       * Removes the path segment between the two waypoints if one exists
       */
      bool RemoveEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void RemovePathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo);

      /**
       * Removes all path segments from and to the specified waypoint
       * @note this used to only remove paths from the specified waypoint
       *        and has been changed to remove paths going to the waypoint as well.
       *        for legacy behavoir use RemoveAllEdgesFromWaypoint().
       */
      void RemoveAllEdges(const WaypointInterface* pFrom);
      void RemoveAllEdgesFromWaypoint(const WaypointInterface* pFrom);
      void RemoveAllPaths(const WaypointInterface* pFrom);


      /**
       * @return whether or not a path exists between the two waypoints
       */
      bool ContainsEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo) const;
      bool ContainsPath(const WaypointInterface* pFrom, const WaypointInterface* pTo) const;

      /**
       * returns a reference to the multimap which contains all WaypointPairs
       * indexed by the starting waypoint
       */
      const NavMeshContainer& GetNavMesh() const;

      /**
       * Returns a multimap iterator to begin starting at the first
       * valid waypoint you can get to from the waypoint specified
       * used by WaypointIter in WaypointUtils.h
       */
      NavMeshContainer::iterator begin(const WaypointInterface* pPtr);
      NavMeshContainer::const_iterator begin(const WaypointInterface* pPtr) const;

      /**
       * Returns a multimap iterator to the end sequence of
       * valid waypoints you can get to from the waypoint specified
       * used by WaypointIter in WaypointUtils.h
       */
      NavMeshContainer::iterator end(const WaypointInterface* pPtr);
      NavMeshContainer::const_iterator end(const WaypointInterface* pPtr) const;

      /**
       * Returns the number of immediately reachable waypoints
       * adjacent to the given waypoint
       */
      NavMeshContainer::size_type size(const WaypointInterface* pPtr) const;

      /**
       * Frees memory and clears all waypoints from the multimap
       */
      void Clear();

      /**
       * Returns whether or not there is a WaypointPair going in the opposite direction
       */
      bool IsOneWay(WaypointPair* pPair) const;

   private:
      NavMeshContainer mNavMesh;
   };

} // namespace dtAI

#endif // __DELTA_NAVMESH_H__
