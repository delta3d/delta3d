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
 * @author Bradley Anderegg 06/27/2006
 */

#ifndef __DELTA_WaypointPair_H__
#define __DELTA_WaypointPair_H__

#include <dtAI/export.h>
#include <map>

namespace dtAI
{
   class Waypoint;
   class WaypointPair;
   
   /**
    * A NavMesh is a collection of waypoint pairs
    */
   class DT_AI_EXPORT NavMesh
   {
   public:

      typedef std::pair<unsigned, WaypointPair*> NavMeshPair;
      typedef std::multimap<unsigned, WaypointPair*> NavMeshContainer;      

   public:         
         NavMesh();
         ~NavMesh();

         void AddPathSegment(const Waypoint* pFrom, const Waypoint* pTo);

         const NavMeshContainer& GetNavMesh() const;

         NavMeshContainer::iterator begin(const Waypoint* pPtr);
         NavMeshContainer::iterator end(const Waypoint* pPtr);

         void Clear();
   private:

      NavMeshContainer mNavMesh;
   
   };
}//namespace dtAI

#endif // __DELTA_WaypointPair_H__
