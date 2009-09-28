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

#ifndef DELTA_WAYPOINTGRAPHASTAR_H
#define DELTA_WAYPOINTGRAPHASTAR_H

#include <dtAI/export.h>
#include <dtAI/astar.h>
#include <dtAI/astarwaypointutils.h>
#include <dtAI/waypointgraph.h>
#include <dtAI/waypointcollection.h>

namespace dtAI
{
   
   class WaypointGraphNode: public AStarNode<WaypointGraphNode, const WaypointInterface*, WaypointGraph::ConstWaypointArray::const_iterator, float>
   {
   public:
      //we must supply astar with a create function to use this constructor    
      WaypointGraphNode(WaypointGraph& wpGraph, node_type* pParent, const WaypointInterface* pWaypoint, cost_type pGn, cost_type pHn)
         : BaseType(pParent, pWaypoint, pGn, pHn)
         , mWPGraph(&wpGraph)
      {
         mWaypoints.clear();
         mWPGraph->GetAllEdgesFromWaypoint(pWaypoint->GetID(), mWaypoints);
      }      

      //this constructor will be used for constrained searches
      WaypointGraphNode(WaypointGraph& wpGraph, const NavMesh& nm, node_type* pParent, const WaypointInterface* pWaypoint, cost_type pGn, cost_type pHn)
         : BaseType(pParent, pWaypoint, pGn, pHn)
         , mWPGraph(&wpGraph)
         , mSearchSpace(&nm)
      {
         mWaypoints.clear();
         
         //copy paths
         NavMesh::NavMeshContainer::const_iterator nm_iter = mSearchSpace->begin(pWaypoint);
         NavMesh::NavMeshContainer::const_iterator nm_iterEnd = mSearchSpace->end(pWaypoint);

         for(;nm_iter != nm_iterEnd; ++nm_iter)
         {
            mWaypoints.push_back( (*nm_iter).second->GetWaypointTo() );
         }
      }      


      //we need this one just to compile, probably a good sign there is some refactoring to be done
      WaypointGraphNode(node_type* pParent, const WaypointInterface* pWaypoint, cost_type pGn, cost_type pHn)
         : BaseType(pParent, pWaypoint, pGn, pHn)
         , mWPGraph(NULL)
      {
      }  

      /*virtual*/ iterator begin() const
      {         
         return mWaypoints.begin();
      }

      /*virtual*/ iterator end() const
      {
         return mWaypoints.end();
      }

   private:
      WaypointGraph* mWPGraph;
      dtCore::RefPtr<const NavMesh> mSearchSpace;
      WaypointGraph::ConstWaypointArray mWaypoints;

   };


   typedef AStar<WaypointGraphNode, WaypointCostFunc, std::list<const WaypointInterface*>, AStarTimer > WaypointGraphAStarBase;


   class DT_AI_EXPORT WaypointGraphAStar: public WaypointGraphAStarBase
   {
      public:
         typedef WaypointGraphAStarBase::CreateNodeFunctor WaypointGraphAStarCreateFunctor;

         WaypointGraphAStar(WaypointGraph& wpGraph);
         virtual ~WaypointGraphAStar();

         PathFindResult HierarchicalFindPath(WaypointID from, WaypointID to, WaypointGraph::ConstWaypointArray& result);

         PathFindResult FindSingleLevelPath(WaypointID from, WaypointID to, WaypointGraph::ConstWaypointArray& result);

         //const WaypointInterface* FindNext(WaypointID from, WaypointID to);

         WaypointGraphNode* CreateNode(WaypointGraphNode* pParent, const WaypointInterface* pWaypoint, float pGn, float pHn);
    
      private: 
         typedef std::vector<WaypointID> WaypointIDArray;

         void CreateSearchSpace(const WaypointGraph::ConstWaypointArray& pathParents, NavMesh& result);

         void AddPaths(const WaypointCollection* from, const WaypointIDArray& wps, NavMesh& nm);
         void FillIDArray(const WaypointGraph::ConstWaypointArray& wps, WaypointIDArray& idArray);

         PathFindResult FindSingleLevelPath(const WaypointInterface* from, const WaypointInterface* to, WaypointGraph::ConstWaypointArray& result);
         PathFindResult HierarchicalFindPath(const WaypointInterface* from, const WaypointInterface* to, WaypointGraph::ConstWaypointCollectionArray& lhs, WaypointGraph::ConstWaypointCollectionArray& rhs, WaypointGraph::ConstWaypointArray& lastPath);

         //void ResolvePathAtNextLowerLevel(const WaypointInterface* from, const WaypointInterface* to, WaypointGraph::ConstWaypointArray& result);
         //bool FindNextEdge(WaypointID from, WaypointID to, WaypointCollection::ChildEdge& result);

         bool mUseConstrainedSearch;
         WaypointGraph& mWPGraph;
         dtCore::RefPtr<NavMesh> mSearchSpace;

   };


} // namespace dtAI

#endif // DELTA_WAYPOINTGRAPHASTAR_H
