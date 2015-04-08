/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2004-2008, MOVES Institute
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author Bradley Anderegg 06/29/2006
 */

#include <prefix/unittestprefix.h>
#ifndef __DELTA_ASTARTESTUTILS_H__
#define __DELTA_ASTARTESTUTILS_H__

////////////////////////////////////////////////////////////////////////////////

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
      AStarTest_PathData(float pNumPaths): mPathData(unsigned(pNumPaths)) {}

      void AddPath(float pPathNum, const std::list<float>& pPath)
      {
         mPathData[unsigned(pPathNum)] = pPath;
      }

      std::list<float>& GetPath(float pPathNum)
      {
         return mPathData[unsigned(pPathNum)];
      }

      static AStarTest_PathData* sPathData;

   private:
      std::vector< std::list<float> > mPathData;
   };


   class TestNode: public AStarNode<TestNode, float, std::list<float>::iterator, float>
   {
   public:
      TestNode(node_type* pParent, float pData, cost_type pGn, cost_type pHn): BaseType(pParent, pData, pGn, pHn){}

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
         PathCost(float pCost, float pWay1, float pWay2): mCost(pCost), mWaypoints(pWay1, pWay2){}
         float mCost;
         std::pair<float, float> mWaypoints;
      };

   public:
      float GetCost(float pFrom, float pTo)
      {
         if (pFrom == pTo)
         {
            return 0;
         }

         std::vector<PathCost>::iterator iter = mCostData.begin();
         std::vector<PathCost>::iterator endOfList = mCostData.end();
         while (iter != endOfList)
         {
            PathCost* pCost = &*iter;
            if (pCost->mWaypoints.first == pFrom && pCost->mWaypoints.second == pTo)
            {
               return pCost->mCost;
            }
            ++iter;
         }
         return 100;
      }

      void SetCost(float pathCost, float pFrom, float pTo)
      {
         //check if the path cost has already been assigned
         std::vector<PathCost>::iterator iter = mCostData.begin();
         std::vector<PathCost>::iterator endOfList = mCostData.end();
         while (iter != endOfList)
         {
            PathCost* pCost = &*iter;
            if (pCost->mWaypoints.first == pFrom && pCost->mWaypoints.second == pTo)
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

   class TestCostFunc: public AStarCostFunc<float, float>
   {
   public:
      float operator()(float pIn1, float pIn2) const
      {
         return PathCostData::sCostData->GetCost(pIn1, pIn2);
      }
   };

   typedef std::list<float> TestContainer;
   typedef AStar<TestNode, TestCostFunc, TestContainer, AStarTimer > TestAStar;
} // namespace dtAI

////////////////////////////////////////////////////////////////////////////////

#endif // __DELTA_ASTARTESTUTILS_H__
