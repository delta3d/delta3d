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

#ifndef __DELTA_ASTARTESTUTILS_H__
#define __DELTA_ASTARTESTUTILS_H__

#include <dtAI/astar.h>
#include <dtAI/astarnode.h>
#include <dtAI/astarcostfunc.h>
#include <dtAI/astarwaypointutils.h>

#include <vector>
#include <list>


namespace dtAI
{

   class AStarTest_PathData
   {
   public:

      AStarTest_PathData(unsigned pNumPaths): mPathData(pNumPaths) {}

      void AddPath(unsigned pPathNum, const std::list<unsigned>& pPath)
      {
         mPathData[pPathNum] = pPath;
      }

      std::list<unsigned>& GetPath(unsigned pPathNum)
      {
         return mPathData[pPathNum];
      }


      static AStarTest_PathData* sPathData;

   private:                 

      std::vector< std::list<unsigned> > mPathData;
   };


   class TestNode: public AStarNode<TestNode, unsigned, std::list<unsigned>::iterator, unsigned>
   {
   public:      
      TestNode(node_type* pParent, unsigned pData, cost_type pGn, cost_type pHn): BaseType(pParent, pData, pGn, pHn){}

      /*virtual*/ iterator begin() const 
      {
         return AStarTest_PathData::sPathData->GetPath(mData).begin();
      }

      /*virtual*/ iterator end() const
      {
         return AStarTest_PathData::sPathData->GetPath(mData).end();
      }

   };


   class PathCostData
   {
   public:
      struct PathCost
      {
         PathCost(unsigned pCost, unsigned pWay1, unsigned pWay2): mCost(pCost), mWaypoints(pWay1, pWay2){}
         unsigned mCost;
         std::pair<unsigned, unsigned> mWaypoints;
      };

   public:


      unsigned GetCost(unsigned pFrom, unsigned pTo)
      {
         if(pFrom == pTo)
         {
            return 0;
         }

         std::vector<PathCost>::iterator iter = mCostData.begin();
         std::vector<PathCost>::iterator endOfList = mCostData.end();
         while(iter != endOfList)
         {
            PathCost* pCost = &*iter;
            if(pCost->mWaypoints.first == pFrom && pCost->mWaypoints.second == pTo)
            {
               return pCost->mCost;
            }
            ++iter;
         }
         return 100;
      }

      void SetCost(unsigned pathCost, unsigned pFrom, unsigned pTo)
      {
         //check if the path cost has already been assigned
         std::vector<PathCost>::iterator iter = mCostData.begin();
         std::vector<PathCost>::iterator endOfList = mCostData.end();
         while(iter != endOfList)
         {
            PathCost* pCost = &*iter;
            if(pCost->mWaypoints.first == pFrom && pCost->mWaypoints.second == pTo)
            {
              pCost->mCost = pathCost;
               return;
            }
            ++iter;
         }

         //else add a new path cost
         mCostData.push_back(PathCost(pathCost, pFrom, pTo));
      }

      static PathCostData* sCostData;

   private:
      std::vector<PathCost> mCostData;
   };



   class TestCostFunc: public AStarCostFunc<unsigned, unsigned>
   {
   public:
      unsigned operator()(unsigned pIn1, unsigned pIn2) const
      {         
         return PathCostData::sCostData->GetCost(pIn1, pIn2);
      }
   };


   typedef std::list<unsigned> TestContainer;

   typedef AStar<TestNode, TestCostFunc, TestContainer, AStarTimer > TestAStar;


}//namespace dtAI

#endif // __DELTA_ASTARTESTUTILS_H__
