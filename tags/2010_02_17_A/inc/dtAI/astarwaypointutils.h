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
    * This is an example of how to use class AStar for generic path finding
    * @see AStar.h
    */

   /**
    * This class wraps a multi-map iterator and abstracts it as an iterator to
    * WaypointInterface list type iterator.  This class is needed for the AStarNode's _Iter
    * template argument.  Notice the operators it uses, !=, *, and ++.  Any STL iterator
    * will work for AStarNode's _Iter template argument provided that operator* will return
    * a DataType.
    */
   class WaypointIter
   {
   public:
      typedef NavMesh::NavMeshContainer::iterator iterator;

   public:
      WaypointIter(const iterator& pIter): mIter(pIter){}
      WaypointIter(const WaypointIter& pIter){mIter = pIter.container();}
      WaypointIter& operator=(WaypointIter& pIter){mIter = pIter.container(); return *this;}

      bool operator !=(const WaypointIter& pIter) const
      {
         return mIter != pIter.container();
      }

      const WaypointInterface* operator*() const
      {
         return (*mIter).second->GetWaypointTo();
      }

      WaypointIter& operator++()
      {
        ++mIter;
        return *this;
      }

   protected:
      iterator container()const{return mIter;}

   private:
      iterator mIter;
   };

   /**
    * This is the NodeType derivative used by the AStar class.  Notice the template arguments
    * it gives to the base class AStarNode.  The first one, WaypointNode, is just itself passed
    * as an argument, this is useful to avoid casting.  The next, const WaypointInterface*, is our DataType
    * this is what we expect for our result path to contain, and what we compute our cost from.
    * The next argument, WaypointIter, is the iterator type we will return on begin() and end(), it
    * is described in more detail above. The last template argument, float, is our CostType, this is
    * defined as being the type used to compute the cost between Waypoints.
    */
   class WaypointNode : public AStarNode<WaypointNode, const WaypointInterface*, WaypointIter, float>
   {
   public:
      WaypointNode(node_type* pParent, const WaypointInterface* pWaypoint, cost_type pGn, cost_type pHn) : BaseType(pParent, pWaypoint, pGn, pHn) {}

      /*virtual*/ iterator begin() const
      {
         return iterator(WaypointManager::GetInstance().GetNavMesh().begin(mData));
      }

      /*virtual*/ iterator end() const
      {
         return iterator(WaypointManager::GetInstance().GetNavMesh().end(mData));
      }
   };

   /**
    * This class is used to compute the cost between two DataType's, for us
    * this DataType is const WaypointInterface*.  As a note, the AStar algorithm will
    * give perfect output when perfect input is provided. This means that the
    * performance of AStar::FindPath() and its accuracy is determined by how
    * well we can compute cost.  This is using 3D distance for cost and can
    * be refined to use slope or even other factors such as gravitational spots
    * reflected places you want your agents to avoid or close in on.
    */
   class WaypointCostFunc : public AStarCostFunc<const WaypointInterface*, float>
   {
   public:
      float operator()(const WaypointInterface* pWaypoint1, const WaypointInterface* pWaypoint2) const
      {
         WaypointPair pPair(pWaypoint1, pWaypoint2);
         return pPair.Get3DDistance();
      }
   };

   /**
    * This is a simple timer classed created with the dtCore::Timer. In order to keep
    * the AStar class from being dependant on a specific Timer implementation we take
    * one as a template argument.  That said here is a default one for use with delta.
    * The usage syntax is Update() - Update() - GetDT(), where GetDT() will determine
    * the amount of time between the two updates.
    */
   class AStarTimer
   {
   public:
      void Update() { mTime = dtCore::Timer::Instance()->Tick(); }
      double GetDT() { return dtCore::Timer::Instance()->DeltaMil(mTime, dtCore::Timer::Instance()->Tick()); }

   private:
      dtCore::Timer_t mTime;
   };

   /**
    * And this is where the magic happens.  A template instantiation of class AStar using
    * WaypointNode as the NodeType, the custom cost function above, a container to hold our result
    * and a timer for statistical tracking and constraints.  Now we can use the AStar API on our
    * custom AStar type WaypointAStar.
    */
   typedef AStar<WaypointNode, WaypointCostFunc, std::list<const WaypointInterface*>, AStarTimer > WaypointAStar;
} // namespace dtAI

#endif // __DELTA_ASTARWAYPOINTUTILS_H__
