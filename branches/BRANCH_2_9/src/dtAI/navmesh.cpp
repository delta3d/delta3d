/* Delta3D Open Source Game and Simulation Engine
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
   struct NavMeshDeleteFunc
   {
      template<class _Type>
      void operator()(_Type p1)
      {
         delete p1.second;
      }
   };

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMesh()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMesh(NavMeshContainer::iterator from, NavMeshContainer::iterator to)
   {
      mNavMesh.insert(from, to);
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::~NavMesh()
   {
      Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::Clear()
   {
      if(!mNavMesh.empty())
      {
         std::for_each(mNavMesh.begin(), mNavMesh.end(), NavMeshDeleteFunc());
         mNavMesh.clear();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::InsertCopy(NavMeshContainer::const_iterator from, NavMeshContainer::const_iterator to)
   {
      mNavMesh.insert(from, to);
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::Remove(NavMeshContainer::iterator from, NavMeshContainer::iterator to)
   {
      mNavMesh.erase(from, to);
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::AddEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      //adding check for duplicates
      if(!ContainsEdge(pFrom, pTo))
      {
         mNavMesh.insert( NavMeshPair(pFrom->GetID(), new WaypointPair(pFrom, pTo)));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::AddPathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      AddEdge(pFrom, pTo);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NavMesh::RemoveEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      NavMeshContainer::iterator iter      = begin(pFrom);
      NavMeshContainer::iterator endOfList = end(pFrom);

      WaypointID id = pTo->GetID();
      while (iter != endOfList)
      {
         if (iter->second->GetWaypointTo()->GetID() == id)
         {
            mNavMesh.erase(iter);
            return true;
         }
         ++iter;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::RemovePathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      RemoveEdge(pFrom, pTo);
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::RemoveAllEdges(const WaypointInterface* from)
   {
      WaypointID id = from->GetID();

      //std::pair<NavMeshContainer::iterator, NavMeshContainer::iterator> rangeElements = mNavMesh.equal_range(from->GetID());
      
      //before we delete these paths, lets see if these are bi-directional
      NavMeshContainer::iterator iter = mNavMesh.begin();
      NavMeshContainer::iterator iterEnd = mNavMesh.end();
      for(; iter != iterEnd;)
      {
         if(iter->first == id || iter->second->GetWaypointTo()->GetID() == id)
         {
            NavMeshContainer::iterator iterTmp = iter;
            ++iterTmp;

            mNavMesh.erase(iter);
            iter = iterTmp;
         }
         else
         {
            ++iter;
         }
      }
      
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::RemoveAllEdgesFromWaypoint(const WaypointInterface* from)
   {
      std::pair<NavMeshContainer::iterator, NavMeshContainer::iterator> rangeElements = mNavMesh.equal_range(from->GetID());
      
      mNavMesh.erase(rangeElements.first, rangeElements.second);
   }

   /////////////////////////////////////////////////////////////////////////////
   void NavMesh::RemoveAllPaths(const WaypointInterface* pFrom)
   {
      RemoveAllEdges(pFrom);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NavMesh::ContainsEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo) const
   {
      NavMeshContainer::const_iterator iter = begin(pFrom);
      NavMeshContainer::const_iterator endOfList = end(pFrom);

      WaypointID id = pTo->GetID();
      while (iter != endOfList)
      {
         if (iter->second->GetWaypointTo()->GetID() == id)
         {
            return true;
         }
         ++iter;
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NavMesh::ContainsPath(const WaypointInterface* pFrom, const WaypointInterface* pTo) const
   {
      return ContainsEdge(pFrom, pTo);
   }

   /////////////////////////////////////////////////////////////////////////////
   const NavMesh::NavMeshContainer& NavMesh::GetNavMesh() const
   {
      return mNavMesh;
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMeshContainer::iterator NavMesh::begin(const WaypointInterface* pPtr)
   {
      return mNavMesh.lower_bound(pPtr->GetID());
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMeshContainer::iterator NavMesh::end(const WaypointInterface* pPtr)
   {
      return mNavMesh.upper_bound(pPtr->GetID());
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMeshContainer::const_iterator NavMesh::begin(const WaypointInterface* pPtr) const
   {
      return mNavMesh.lower_bound(pPtr->GetID());
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMeshContainer::const_iterator NavMesh::end(const WaypointInterface* pPtr) const
   {
      return mNavMesh.upper_bound(pPtr->GetID());
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh::NavMeshContainer::size_type NavMesh::size(const WaypointInterface* pPtr) const
   {
      // first pass implementation is potentially slow, but works
      NavMesh::NavMeshContainer::size_type count = 0;
      for (NavMeshContainer::const_iterator adj = begin(pPtr); adj != end(pPtr); ++adj)
      {
         ++count;
      }
      return count;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool NavMesh::IsOneWay(WaypointPair* pPair) const
   {
      NavMeshContainer::const_iterator iter      = begin(pPair->GetWaypointTo());
      NavMeshContainer::const_iterator endOfList = end(pPair->GetWaypointTo());

      WaypointID id = pPair->GetWaypointFrom()->GetID();
      while (iter != endOfList)
      {
         if (iter->second->GetWaypointTo()->GetID() == id)
         {
            return false;
         }
         ++iter;
      }

      return true;
   }

} // namespace dtAI
