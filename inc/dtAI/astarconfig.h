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
 * Bradley Anderegg 06/30/2006
 */

#ifndef __DELTA_ASTARCONFIG_H__
#define __DELTA_ASTARCONFIG_H__

// vs thinks std::numeric_limits<>::max() is a macro
#ifdef max
#undef max
#endif
#include <limits>

#include <vector>

namespace dtAI
{
   /**
    * AStarConfig contains the basic data that AStar operates on.
    * @see AStar.h
    */
   template <class _DataType, class _CostType, class _ContainerType>
   class AStarConfig
   {
   public:
      typedef _DataType data_type;
      typedef _CostType cost_type;
      typedef _ContainerType container;

      AStarConfig()
         : mResult()
         , mTotalCost(0)
         , mTotalNodesExplored(0)
         , mTotalTime(0.0)
         , mNumIterations(0)
         , mMaxNodesExplored(std::numeric_limits<unsigned int>::max())
         , mMaxTime(std::numeric_limits<double>::max())
         , mMaxCost(std::numeric_limits<cost_type>::max())
         , mMaxDepth(std::numeric_limits<unsigned int>::max())
         , mCheckClosedList(true)
         , mStart()
         , mFinish()
         , mNodesExplored(0)
         , mTimeSpent(0.0)
      {
      }

      AStarConfig(data_type pFrom, data_type pTo)
         : mMaxNodesExplored(std::numeric_limits<unsigned int>::max())
         , mMaxCost(std::numeric_limits<cost_type>::max())
         , mMaxTime(std::numeric_limits<double>::max())
      {
         Reset(pFrom, pTo);
      }


   public:
      /// the resulting path, and the total cost to it
      container mResult;
      cost_type mTotalCost;

      /// these are used as statistical output
      unsigned int mTotalNodesExplored;
      double mTotalTime;
      unsigned int mNumIterations;

      /// these are the max values per iteration
      /// and should be set for dealing with constraints
      unsigned int mMaxNodesExplored;
      double mMaxTime;
      cost_type mMaxCost;
      unsigned int mMaxDepth;

      /// when true, nodes already on closed list are not processed
      /// should be set false for inconsistent heuristic functions
      /// see Norvig and Russell, 2nd Edition, p. 99
      bool mCheckClosedList;


   //private:

      /// these are for usage by AStar.h and should not be used by clients of AStar.h
      void Reset(data_type pFrom, data_type pTo)
      {
         mStart              = pFrom;
         mFinish.clear();
         mFinish.push_back(pTo);
         mResult.clear();
         mTotalCost          = 0;
         mNumIterations      = 0;
         mNodesExplored      = 0;
         mTimeSpent          = 0;
         mTotalNodesExplored = 0;
         mTotalTime          = 0;
      }

      void Reset(data_type pFrom, const std::vector<data_type>& pTo)
      {
         mStart              = pFrom;
         mFinish.clear();
         mFinish             = pTo;
         mResult.clear();
         mTotalCost          = 0;
         mNumIterations      = 0;
         mNodesExplored      = 0;
         mTimeSpent          = 0;
         mTotalNodesExplored = 0;
         mTotalTime          = 0;
      }

      bool AtFinish(data_type pTest)
      {
         typename std::vector<data_type>::const_iterator iter = mFinish.begin();
         typename std::vector<data_type>::const_iterator endOfList = mFinish.end();

         while (iter != endOfList)
         {
            if (pTest == (*iter))
            {
               return true;
            }
            ++iter;
         }

         return false;
      }

      data_type Start()
      {
         return mStart;
      }

      data_type Finish()
      {
         return mFinish[0];
      }

      /// these are for usage by AStar.h and should not be used by clients of AStar.h
      data_type mStart;

      std::vector<data_type> mFinish;

      // these are for book keeping during a single iteration
      /// these are for usage by AStar.h and should not be used by clients of AStar.h
      unsigned int mNodesExplored;
      /// these are for usage by AStar.h and should not be used by clients of AStar.h
      double mTimeSpent;
   };

} // namespace dtAI

#endif // __DELTA_ASTARCONFIG_H__
