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
 * Bradley Anderegg 06/28/2006
 */

struct delete_func
{
   template<class T>
      void operator()(T* pElement)
   {
      delete pElement;
      pElement = NULL;
   }
};


struct node_type_gtr_func
{
   template<class T>
   bool operator()(T& pElement1, T& pElement2)
   {
      return pElement2->operator<(*pElement1);
   }
};


template<class T, class E>
struct node_type_comp_func
{
   node_type_comp_func(const T data): mData(data){}

   bool operator()(const E* pElement)
   {
      return pElement->GetData() == mData;
   }
private:
   const T mData;
};


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::AStar()
: mFuncCreateNode(this, &AStar<_NodeType, _CostFunc, _Container, _Timer>::CreateNode)
{

}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::AStar(CreateNodeFunctor createFunc)
: mFuncCreateNode(createFunc)
{
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::AStar(const config_type& pConfig):
   mConfig(pConfig)
{
   AddNodeLink(0, pConfig.mStart);
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::~AStar()
{
   FreeMem();
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::FreeMem()
{

   std::for_each(mOpen.begin(), mOpen.end(), delete_func());
   std::for_each(mClosed.begin(), mClosed.end(), delete_func());

   mOpen.clear();
   mClosed.clear();
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Reset(const config_type& pConfig)
{
   FreeMem();
   mConfig = pConfig;
   AddNodeLink(0, mConfig.Start());
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Reset(data_type pFrom, data_type pTo)
{
   FreeMem();
   mConfig.Reset(pFrom, pTo);
   AddNodeLink(0, mConfig.Start());
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Reset(const std::vector<data_type>& pFrom, const std::vector<data_type>& pTo)
{
   if (pFrom.empty() || pTo.empty()) { return; }

   FreeMem();
   mConfig.Reset(pFrom[0], pTo);

   typename std::vector<data_type>::const_iterator iter = pFrom.begin();
   typename std::vector<data_type>::const_iterator endOfList = pFrom.end();

   // we start from index 1
   ++iter;

   while (iter != endOfList)
   {
      mOpen.push_back(mFuncCreateNode(NULL, *iter, mCostFunc(pFrom[0], *iter), mCostFunc(*iter, pTo[0]) ));
      ++iter;
   }

   std::make_heap(mOpen.begin(), mOpen.end(), node_type_gtr_func());

}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::AddNodeLink(node_type* pParent, data_type pData)
{
   if (!pParent)
   {
      Insert(mOpen, mFuncCreateNode(NULL, pData, 0, mCostFunc(mConfig.Start(), mConfig.Finish())));
   }
   else
   {
      cost_type costFromParent = mCostFunc(pParent->GetData(), pData);
      cost_type costToFinish   = mCostFunc(pData, mConfig.Finish());
      Insert(mOpen, mFuncCreateNode(pParent, pData, pParent->GetCostToNode() + costFromParent, costToFinish));
   }
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Insert(AStarContainer& pCont, node_type* pNode)
{
   pCont.push_back(pNode);
   std::push_heap(pCont.begin(), pCont.end(), node_type_gtr_func());
}

// This needs to be optimized once we have a more suitable container implementation
template<class _NodeType, class _CostFunc, class _Container, class _Timer>
typename std::vector<_NodeType*>::iterator AStar<_NodeType, _CostFunc, _Container, _Timer>::Contains(AStarContainer& pCont, data_type pNode)
{
   return std::find_if (pCont.begin(), pCont.end(), node_type_comp_func<data_type, node_type>(pNode));
}

// This needs to be optimized once we have a more suitable container implementation
template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Remove(AStarContainer& pCont, typename AStarContainer::iterator iterToErase)
{
   delete_func d;
   d(*iterToErase);

   pCont.erase(iterToErase, iterToErase + 1);
   std::make_heap(mOpen.begin(), mOpen.end(), node_type_gtr_func());
}


// This needs to be optimized once we have a more suitable container implementation
template<class _NodeType, class _CostFunc, class _Container, class _Timer>
_NodeType* AStar<_NodeType, _CostFunc, _Container, _Timer>::FindLowestCost(AStarContainer& pCont)
{
   if (pCont.empty())
   {
      return 0;
   }
   else
   {
      node_type* node = pCont.front();
      std::pop_heap(pCont.begin(), pCont.end(), node_type_gtr_func());
      pCont.pop_back();
      return node;
   }
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
_NodeType* AStar<_NodeType, _CostFunc, _Container, _Timer>::CreateNode(node_type* pParent, data_type datatype, cost_type pGn, cost_type pHn)
{
   return new node_type(pParent, datatype, pGn, pHn);
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
PathFindResult AStar<_NodeType, _CostFunc, _Container, _Timer>::FindPath()
{
   // increment our iteration
   // reset our constraint bookkeeping vars
   ++mConfig.mNumIterations;
   mConfig.mTimeSpent = 0;
   mConfig.mNodesExplored = 0;

   mTimer.Update();

   for (;;)
   {
      if (mOpen.empty())
      {
         return NO_PATH;
      }

      // start with the node of lowest cost in the open list
      node_type* pStart = FindLowestCost(mOpen);

      // check if we found a path to the end or if we have exceeded a constraint
      cost_type pCost = (pStart->GetCostToNode() + pStart->GetCostToGoal());
      bool pExceededMaxCost(pCost >= mConfig.mMaxCost);
      bool pHasPathToFinish = mConfig.AtFinish(pStart->GetData());

      mConfig.mTimeSpent += mTimer.GetDT();
      mTimer.Update();
      bool pHasExceededTimeLimit(mConfig.mTimeSpent > mConfig.mMaxTime);
      bool pAtOrExceedingMaxDepth(pStart->GetDepth() >= mConfig.mMaxDepth);

      // if we have exceeded a constraint or found a path to the end return
      if (pHasPathToFinish || pExceededMaxCost || pHasExceededTimeLimit || pAtOrExceedingMaxDepth || (mConfig.mNodesExplored >= mConfig.mMaxNodesExplored))
      {
         // we simply add it back to the container so it will be deleted later
         mClosed.push_back(pStart);

         // \todo combine partial lists instead of clearing them
         mConfig.mResult.clear();

         while (pStart)
         {
            mConfig.mResult.push_front(pStart->GetData());
            pStart = pStart->GetParent();
         }

         //log our total costs in the config
         mConfig.mTotalCost = pCost;
         mConfig.mTotalNodesExplored += mConfig.mNodesExplored;
         mConfig.mTotalTime += mConfig.mTimeSpent;

         if (pHasPathToFinish) return PATH_FOUND;
         else return PARTIAL_PATH;
      }
      else
      {
         ++mConfig.mNodesExplored;

         // add it onto the closed list
         Insert(mClosed, pStart);

         // we will iterate through the potential places this node can take us
         typename node_type::iterator iter = pStart->begin();
         typename node_type::iterator endOfList = pStart->end();

         // \todo refactor this with a better container type
         while (iter != endOfList)
         {
            data_type pNode = *iter;
            // if its not in the closed list
            if (!mConfig.mCheckClosedList || Contains(mClosed, pNode) == mClosed.end())
            {
               // if it isnt in the open list
               typename AStarContainer::iterator pNodeLinkIter = Contains(mOpen, pNode);
               if (pNodeLinkIter == mOpen.end())
               {
                  // create a new path in the open list
                  AddNodeLink(pStart, pNode);
               }
               else // lets see if we can get there for cheaper
               {
                  // compute cost to pNode from pStart
                  cost_type pNewCost = pStart->GetCostToNode() + mCostFunc(pStart->GetData(), pNode);

                  // if the new g(n) cost is cheaper then the old one delete the old one
                  // and add the new one as the best potential path to pNode
                  if (pNewCost < (*pNodeLinkIter)->GetCostToNode())
                  {
                     Remove(mOpen, pNodeLinkIter);
                     AddNodeLink(pStart, pNode);
                  }

               }
            }
            ++iter;
         }
      }

   }

   return NO_PATH;
}

