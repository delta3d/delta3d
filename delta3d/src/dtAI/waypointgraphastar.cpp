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
      , mWPGraph(wpGraph)
   {
   }

   WaypointGraphAStar::~WaypointGraphAStar()
   {
   }

   WaypointGraphNode* WaypointGraphAStar::CreateNode(WaypointGraphNode* pParent, const WaypointInterface* pWaypoint, float pGn, float pHn)
   {
      return new WaypointGraphNode(mWPGraph, pParent, pWaypoint, pGn, pHn);
   }

   PathFindResult WaypointGraphAStar::HierarchicalFindPath(WaypointID from, WaypointID to)
   {
      const WaypointInterface* fromWp = mWPGraph.FindWaypoint(from);
      const WaypointInterface* toWp = mWPGraph.FindWaypoint(to);
      WaypointGraphAStarBase::Reset(fromWp, toWp);
      return WaypointGraphAStarBase::FindPath();
   }


} // namespace dtAI

