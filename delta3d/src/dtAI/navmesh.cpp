/** Delta3D Open Source Game and Simulation Engine
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

#include <dtAI/navmesh.h>
#include <dtAI/waypoint.h>
#include <dtAI/waypointpair.h>

#include <algorithm>


namespace dtAI
{

   NavMesh::NavMesh()
   {

   }

   struct NavMeshDeleteFunc
   {
      template<class _Type>
         void operator()(_Type p1)
      {
         delete p1.second; 
      }
   };

   NavMesh::~NavMesh()
   {
      Clear();
   }

   void NavMesh::Clear()
   {
      std::for_each(mNavMesh.begin(), mNavMesh.end(), NavMeshDeleteFunc());
      mNavMesh.clear();
   }  

   void NavMesh::AddPathSegment(const Waypoint* pFrom, const Waypoint* pTo)
   {
      mNavMesh.insert( NavMeshPair(pFrom->GetID(), new WaypointPair(pFrom, pTo)));
   }

   const NavMesh::NavMeshContainer& NavMesh::GetNavMesh() const
   {
      return mNavMesh;
   }


   NavMesh::NavMeshContainer::iterator NavMesh::begin(const Waypoint* pPtr) 
   {
      return mNavMesh.lower_bound(pPtr->GetID());
   }


   NavMesh::NavMeshContainer::iterator NavMesh::end(const Waypoint* pPtr) 
   {
      return mNavMesh.upper_bound(pPtr->GetID());
   }

   NavMesh::NavMeshContainer::const_iterator NavMesh::begin(const Waypoint* pPtr) const
   {
      return mNavMesh.lower_bound(pPtr->GetID());
   }


   NavMesh::NavMeshContainer::const_iterator NavMesh::end(const Waypoint* pPtr) const
   {
      return mNavMesh.upper_bound(pPtr->GetID());
   }
  
   bool NavMesh::IsOneWay(WaypointPair* pPair) const
   {
      NavMeshContainer::const_iterator iter = begin(pPair->GetWaypointTo());
      NavMeshContainer::const_iterator endOfList = end(pPair->GetWaypointTo());

      WaypointID id = pPair->GetWaypointFrom()->GetID();
      while(iter != endOfList)
      {
         if(iter->second->GetWaypointTo()->GetID() == id)
         {
            return false;
         }
         ++iter;
      }

      return true;
   }

}//namespace dtAI
