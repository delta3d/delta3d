/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
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
 * Bradley Anderegg 08/10/2006
 */

#ifndef __DELTA_PLANNERNODELINK_H__
#define __DELTA_PLANNERNODELINK_H__

namespace dtAI
{
   class WorldState;
   class Operator;

   /**
    * The data structure used by the planner for navigating through a plan graph
    */

   class PlannerNodeLink
   {
   public:
      PlannerNodeLink(): mState(0), mOperator(0), mParent(0), mGCost(0), mHCost(0){}

      bool operator <(const PlannerNodeLink& pnl) const
      {
         return (mGCost + mHCost) < (pnl.mGCost + pnl.mHCost);
      }

      const WorldState* mState;
      const Operator* mOperator;

      const PlannerNodeLink* mParent;
      float mGCost, mHCost;
   };

} // namespace dtAI

#endif // __DELTA_PLANNERNODELINK_H__
