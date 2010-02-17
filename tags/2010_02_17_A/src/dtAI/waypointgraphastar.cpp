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
 * Bradley Anderegg 06/29/2006
 */


#include <dtAI/waypointgraphastar.h>


namespace dtAI
{


   WaypointGraphAStar::WaypointGraphAStar(WaypointGraph& wpGraph)
      : WaypointGraphAStarBase(WaypointGraphAStarCreateFunctor(this, &WaypointGraphAStar::CreateNode))
      , mUseConstrainedSearch(false)
      , mWPGraph(wpGraph)
      , mSearchSpace(new NavMesh())
   {
   }

   WaypointGraphAStar::~WaypointGraphAStar()
   {
   }

   WaypointGraphNode* WaypointGraphAStar::CreateNode(WaypointGraphNode* pParent, const WaypointInterface* pWaypoint, float pGn, float pHn)
   {
      WaypointGraphNode* wgn = NULL;
      if(mUseConstrainedSearch)
      {
         wgn = new WaypointGraphNode(mWPGraph, *mSearchSpace, pParent, pWaypoint, pGn, pHn);
      }
      else
      {
         wgn = new WaypointGraphNode(mWPGraph, pParent, pWaypoint, pGn, pHn);
      }

      return wgn;
   }

   PathFindResult WaypointGraphAStar::HierarchicalFindPath(WaypointID from, WaypointID to, WaypointGraph::ConstWaypointArray& result)
   {
      const WaypointInterface* fromWP = mWPGraph.FindWaypoint(from);
      const WaypointInterface* toWP = mWPGraph.FindWaypoint(to);

      const WaypointCollection* wcParent = mWPGraph.FindCommonParent(from, to);

      PathFindResult pfr = NO_PATH;

      if(wcParent != NULL && fromWP != NULL && toWP != NULL)
      {
         WaypointGraph::ConstWaypointCollectionArray lhs, rhs, lastPath;         
         
         mWPGraph.GetNodePath(fromWP, wcParent, lhs);
         mWPGraph.GetNodePath(toWP, wcParent, rhs);

         //if result is not empty this will most likely fail in an unexpected way
         result.clear();
         result.push_back(wcParent);
         
         //we set this flag so if this is used as a base AStar class it work as expected.
         //It changes how the create node function gets its list of waypoints to search on.         
         mUseConstrainedSearch = true;        
         pfr = HierarchicalFindPath(fromWP, toWP, lhs, rhs, result);         
         mUseConstrainedSearch = false;
      }

      return pfr;
   }

   dtAI::PathFindResult WaypointGraphAStar::HierarchicalFindPath(const WaypointInterface* from, const WaypointInterface* to, WaypointGraph::ConstWaypointCollectionArray& lhs, WaypointGraph::ConstWaypointCollectionArray& rhs, WaypointGraph::ConstWaypointArray& lastPath)
   {     
      PathFindResult result = NO_PATH;

      if(lhs.empty() || rhs.empty())
      {
         mSearchSpace->Clear();
         CreateSearchSpace(lastPath, *mSearchSpace);

         lastPath.clear();         
         //this is the final search
         result = FindSingleLevelPath(from, to, lastPath);
      }
      else
      {
         const WaypointCollection* lhsCurNode = lhs.back();
         const WaypointCollection* rhsCurNode = rhs.back();

         lhs.pop_back();
         rhs.pop_back();

         mSearchSpace->Clear();
         CreateSearchSpace(lastPath, *mSearchSpace);

         lastPath.clear();
         lastPath.push_back(lhsCurNode);

         result = FindSingleLevelPath(lhsCurNode, rhsCurNode, lastPath);

         if(result == PATH_FOUND)
         {
            //call recursively until no nodes are left... 
            return HierarchicalFindPath(from, to, lhs, rhs, lastPath);
         }
      }

      return result;      
   }

   PathFindResult WaypointGraphAStar::FindSingleLevelPath(WaypointID from, WaypointID to, WaypointGraph::ConstWaypointArray& result)
   {
      const WaypointInterface* fromWP = mWPGraph.FindWaypoint(from);
      const WaypointInterface* toWP = mWPGraph.FindWaypoint(to);

      PathFindResult pfr = NO_PATH;

      if(fromWP != NULL && toWP != NULL)
      {
         pfr = FindSingleLevelPath(fromWP, toWP, result);
      }

      return pfr;
   }

   PathFindResult WaypointGraphAStar::FindSingleLevelPath(const WaypointInterface* from, const WaypointInterface* to, WaypointGraph::ConstWaypointArray& result)
   {
      WaypointGraphAStarBase::Reset(from, to);
      PathFindResult pathFound = WaypointGraphAStarBase::FindPath();
      if(pathFound != NO_PATH)
      {
         WaypointGraphAStarBase::container_type& wpList = WaypointGraphAStarBase::GetPath();
         std::copy(wpList.begin(), wpList.end(), std::back_inserter(result));
      }

      return pathFound;
   }

   void WaypointGraphAStar::CreateSearchSpace(const WaypointGraph::ConstWaypointArray& pathParents, NavMesh& result)
   {
      WaypointIDArray wpIds;
      FillIDArray(pathParents, wpIds);

      WaypointGraph::ConstWaypointArray::const_iterator iter = pathParents.begin();
      WaypointGraph::ConstWaypointArray::const_iterator iterEnd = pathParents.end();         

      for(;iter != iterEnd; ++iter)
      {
         const WaypointCollection* wc = dynamic_cast<const WaypointCollection*>(*iter);
         if(wc != NULL)
         {
            AddPaths(wc, wpIds, result);           
         }
      }
   }

   void WaypointGraphAStar::AddPaths(const WaypointCollection* from, const WaypointIDArray& wps, NavMesh& nm )
   {
      WaypointIDArray::const_iterator iter = wps.begin();
      WaypointIDArray::const_iterator iterEnd = wps.end();

      for(;iter != iterEnd; ++iter)
      {
         WaypointCollection::ChildEdgeArray result;
         from->GetEdges(*iter, result);

         WaypointCollection::ChildEdgeArray::const_iterator edgeIter = result.begin();
         WaypointCollection::ChildEdgeArray::const_iterator edgeIterEnd = result.end();
         for(;edgeIter != edgeIterEnd; ++edgeIter)
         {
            const WaypointInterface* fromWP = mWPGraph.FindWaypoint(edgeIter->first);
            const WaypointInterface* toWP = mWPGraph.FindWaypoint(edgeIter->second);

            if(fromWP != NULL && toWP != NULL)
            {
               nm.AddEdge(fromWP, toWP);
            }
         }
      }

   }

   void WaypointGraphAStar::FillIDArray(const WaypointGraph::ConstWaypointArray& wps, WaypointIDArray& wpIds)
   {
      WaypointGraph::ConstWaypointArray::const_iterator iter = wps.begin();
      WaypointGraph::ConstWaypointArray::const_iterator iterEnd = wps.end();         

      for(;iter != iterEnd; ++iter)
      {
         wpIds.push_back((*iter)->GetID()); 
      }
   }

} // namespace dtAI

