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
 * @author Bradley Anderegg 06/29/2006
 */

#ifndef __DELTA_ASTARWAYPOINTUTILS_H__
#define __DELTA_ASTARWAYPOINTUTILS_H__

#include <dtAI/astar.h>
#include <dtAI/astarnode.h>
#include <dtAI/astarcostfunc.h>
#include <dtAI/waypointmanager.h>
#include <dtAI/waypointpair.h>
#include <dtAI/waypoint.h>
#include <dtAI/navmesh.h>

#include <dtCore/timer.h>
#include <list>

namespace dtAI
{
   /**
    * AStarWaypointUtils is an implementation of the AStar utilities 
    */

   class WaypointIter
   {
   public:
      typedef NavMesh::NavMeshContainer::iterator iterator;

   public:
      WaypointIter(const iterator& pIter): mIter(pIter){}  
      WaypointIter(const WaypointIter& pIter){mIter = pIter.container();}
      WaypointIter& operator=(WaypointIter& pIter){mIter = pIter.container(); return *this;}

      bool WaypointIter::operator !=(const WaypointIter& pIter) const
      {
         return mIter != pIter.container();
      }

      const Waypoint* operator*() const
      {
         return (*mIter).second->GetWaypointTo();
      }

      WaypointIter& operator++()
      {
        ++mIter;
        return *this;
      }

      iterator container()const{return mIter;}
   private:
      iterator mIter;
   };


   class WaypointNode: public AStarNode<WaypointNode, const Waypoint*, WaypointIter, float>
   {
   public:      
      WaypointNode(node_type* pParent, const Waypoint* pWaypoint, cost_type pGn, cost_type pHn): BaseType(pParent, pWaypoint, pGn, pHn){}

      /*virtual*/ iterator begin() const 
      {
         return iterator(WaypointManager::GetInstance()->GetNavMesh().begin(mData));
      }

      /*virtual*/ iterator end() const
      {
         return iterator(WaypointManager::GetInstance()->GetNavMesh().end(mData));
      }
      
   };


   class WaypointCostFunc: public AStarCostFunc<const Waypoint*, float>
   {
   public:
      float operator()(const Waypoint* pWaypoint1, const Waypoint* pWaypoint2) const
      {
         WaypointPair pPair(pWaypoint1, pWaypoint2);
         return pPair.Get2DDistance();
      }
   };


   class AStarTimer
   {
   public:
      void Update(){ mTime = dtCore::Timer::Instance()->Tick();}
      double GetDT(){return dtCore::Timer::Instance()->DeltaMil(mTime, dtCore::Timer::Instance()->Tick());}

   private:
      dtCore::Timer_t mTime;
   };
   

   typedef AStar<WaypointNode, WaypointCostFunc, std::list<const Waypoint*>, AStarTimer > WaypointAStar;


}//namespace dtAI

#endif // __DELTA_ASTARWAYPOINTUTILS_H__
