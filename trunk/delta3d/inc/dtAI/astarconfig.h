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
 * @author Bradley Anderegg 06/30/2006
 */

#ifndef __DELTA_ASTARCONFIG_H__
#define __DELTA_ASTARCONFIG_H__

//vs thinks std::numeric_limits<>::max() is a macro
#ifdef max
#undef max
#endif
#include <limits>

namespace dtAI
{
   /**
    * AStarConfig configures the AStar options
    */
   template <class _DataType, class _CostType, class _ContainerType>
   class AStarConfig
   {
      public:
         typedef _DataType data_type;
         typedef _CostType cost_type;
         //typedef std::numeric_limits<cost_type>::max() max_cost;
         typedef _ContainerType container;


         data_type mStart;
         data_type mFinish;

         cost_type mTotalCost;
         container mResult;


         //these are the max values per iteration
         unsigned mMaxNodesExplored;
         double mMaxTime;
         cost_type mMaxCost;

         //these are for book keeping during a single iteration
         unsigned mIteration;
         unsigned mNodesExplored;         
         double mTimeSpent;

         //these are overall totals for all iterations and may be greater then
         //the max per iteration
         unsigned mTotalNodesExplored;
         double mTotalTime;



      public:
         AStarConfig():
            mStart(),
            mFinish(),
            mTotalCost(0),
            mResult(),
            mMaxNodesExplored(std::numeric_limits<unsigned>::max()),
            mMaxTime(9999.0),
            mMaxCost(std::numeric_limits<cost_type>::max()),
            mIteration(0),
            mNodesExplored(0), 
            mTimeSpent(0.0), 
            mTotalNodesExplored(0),
            mTotalTime(0.0)
         {
         }

         AStarConfig(data_type pFrom, data_type pTo):
            mMaxNodesExplored(std::numeric_limits<unsigned>::max()),
            mMaxCost(std::numeric_limits<cost_type>::max()),
            mMaxTime(9999)
         {
            Reset(pFrom, pTo);
         }
      
         void Reset(data_type pFrom, data_type pTo)
         {
            mStart = pFrom;
            mFinish = pTo;
            mResult.clear();
            mTotalCost = 0;
            mIteration = 0;
            mNodesExplored = 0;
            mTimeSpent = 0;
            mTotalNodesExplored = 0;
            mTotalTime = 0;
         }
   
   };
}//namespace dtAI

#endif // __DELTA_ASTARCONFIG_H__
