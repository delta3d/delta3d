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
 * @author Bradley Anderegg 06/28/2006
 */

struct deleteFunc
{
   template<class T>
      void operator()(T* pElement)
   {
      delete pElement;
      pElement = 0;
   }
};




template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::AStar()
{
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::AStar(config_type pConfig):
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
   
   std::for_each(mOpen.begin(), mOpen.end(), deleteFunc());
   std::for_each(mClosed.begin(), mClosed.end(), deleteFunc());

   mOpen.clear();
   mClosed.clear();
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Reset(config_type pConfig)
{
   FreeMem();
   mConfig = pConfig;   
   AddNodeLink(0, pConfig.mStart);
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::Reset(data_type pFrom, data_type pTo)
{
   FreeMem();
   mConfig.Reset(pFrom, pTo);      
   AddNodeLink(0, mConfig.mStart);
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::AddNodeLink(node_type* pParent, data_type pData)
{
   if(!pParent)
   {
      mOpen.push_back(new node_type(0, pData, 0, mCostFunc(mConfig.mStart, mConfig.mFinish)));   
   }
   else
   {
      mOpen.push_back(new node_type(pParent, pData, pParent->GetCostToNode() + mCostFunc(pParent->GetData(), pData), mCostFunc(pData, mConfig.mFinish)));
   }
}


template<class _NodeType, class _CostFunc, class _Container, class _Timer>
bool AStar<_NodeType, _CostFunc, _Container, _Timer>::Contains(const AStarContainer& pCont, data_type pNode)
{
   typename AStarContainer::const_iterator iter = pCont.begin();
   typename AStarContainer::const_iterator endOfList = pCont.end();

   while(iter != endOfList)
   {
      if(pNode == (*iter)->GetData())
      {
         return true;
      }
      ++iter;
   }

   return false;
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
_NodeType* AStar<_NodeType, _CostFunc, _Container, _Timer>::Remove(AStarContainer& pCont, data_type pNode)
{
   typename AStarContainer::iterator iter = pCont.begin();
   typename AStarContainer::iterator endOfList = pCont.end();

   node_type* pLink = 0;

   while(iter != endOfList)
   {
      if(pNode == (*iter)->GetData())
      {
         pLink = (*iter);
         pCont.remove(pLink);
         break;
      }
      ++iter;
   }

   return pLink;
}



template<class _NodeType, class _CostFunc, class _Container, class _Timer>
_NodeType* AStar<_NodeType, _CostFunc, _Container, _Timer>::FindLowestCost(const AStarContainer& pCont)
{
   typename AStarContainer::const_iterator iter = pCont.begin();
   typename AStarContainer::const_iterator endOfList = pCont.end();

   node_type* pLowest = *iter;
   
   while(iter != endOfList)
   {
      node_type* pNLIter = (*iter);
      if(pNLIter->operator<(*pLowest))
      {
         pLowest = pNLIter;
      }
      ++iter;
   }

   return pLowest;
}



template<class _NodeType, class _CostFunc, class _Container, class _Timer>
typename AStar<_NodeType, _CostFunc, _Container, _Timer>::AStarResult AStar<_NodeType, _CostFunc, _Container, _Timer>::FindPath()
{  
   ++mConfig.mIteration;
   mConfig.mTimeSpent = 0;
   mConfig.mNodesExplored = 0;
   
   mTimer.Update();

   for(;;)
   {
      if(mOpen.empty())
      {
         return NO_PATH;
      }

      node_type* pStart = FindLowestCost(mOpen);           
      
      cost_type pCost = (pStart->GetCostToNode() + pStart->GetCostToGoal());
      bool pExceededMaxCost(pCost >= mConfig.mMaxCost);
      bool pHasPathToFinish(pStart->GetData() == mConfig.mFinish);

      mTimer.Update();
      mConfig.mTimeSpent += mTimer.GetDT();
      bool pHasExceededTimeLimit(mConfig.mTimeSpent > mConfig.mMaxTime);
      
      if(pHasPathToFinish || pExceededMaxCost || pHasExceededTimeLimit || (mConfig.mNodesExplored >= mConfig.mMaxNodesExplored))
      {
         //\todo combine partial lists instead of clearing them
         mConfig.mResult.clear();

         while(pStart)
         {            
            mConfig.mResult.push_front(pStart->GetData());            
            pStart = pStart->GetParent();
         }

         mConfig.mTotalCost = pCost;
         mConfig.mTotalNodesExplored += mConfig.mNodesExplored;
         mConfig.mTotalTime += mConfig.mTimeSpent;
         
         if(pHasPathToFinish) return PATH_FOUND;
         else return PARTIAL_PATH;
      }
      else
      {
         ++mConfig.mNodesExplored;

         mOpen.remove(pStart);
         mClosed.push_back(pStart);
         
         typename node_type::iterator iter = pStart->begin();
         typename node_type::iterator endOfList = pStart->end();

         while(iter != endOfList)
         {
            data_type pNode = *iter;
            //if its not in the closed list
            if(!Contains(mClosed, pNode))
            {
               //attempt to remove it from the open list
               node_type* pLink = Remove(mOpen, pNode);

               //if it isnt in the open list
               if(!pLink)
               {
                  AddNodeLink(pStart, pNode);
               }
               else //lets see if we can get there for cheaper
               {
                  cost_type pNewCost = pStart->GetCostToNode() + mCostFunc(pStart->GetData(), pNode);
                  if(pNewCost < (pLink->GetCostToNode() + pLink->GetCostToGoal()))
                  {
                     delete pLink;
                     AddNodeLink(pStart, pNode);                     
                  }
                  else
                  {
                     mOpen.push_back(pLink);
                  }
               }
            }
            ++iter;
         }
      }

   }
  
   /*mConfig.mTotalTime += mConfig.mTimeSpent;
   mConfig.mTotalNodesExplored += mConfig.mNodesExplored;
   return false;*/

   //we should never get here
   assert(0);
   return NO_PATH;
}

