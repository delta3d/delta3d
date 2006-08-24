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
      pElement = NULL;
   }
};




template<class _NodeType, class _CostFunc, class _Container, class _Timer>
AStar<_NodeType, _CostFunc, _Container, _Timer>::AStar()
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
   
   std::for_each(mOpen.begin(), mOpen.end(), deleteFunc());
   std::for_each(mClosed.begin(), mClosed.end(), deleteFunc());

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
   if(pFrom.empty() || pTo.empty()) return;

   FreeMem();
   mConfig.Reset(pFrom[0], pTo);

   typename std::vector<data_type>::const_iterator iter = pFrom.begin();
   typename std::vector<data_type>::const_iterator endOfList = pFrom.end();

   //we start from index 1
   ++iter;

   while(iter != endOfList)
   {
      mOpen.push_back(new node_type(0, *iter, mCostFunc(pFrom[0], *iter), mCostFunc(*iter, pTo[0]) ));
      ++iter;
   }
   
}

template<class _NodeType, class _CostFunc, class _Container, class _Timer>
void AStar<_NodeType, _CostFunc, _Container, _Timer>::AddNodeLink(node_type* pParent, data_type pData)
{
   if(!pParent)
   {
      mOpen.push_back(new node_type(0, pData, 0, mCostFunc(mConfig.Start(), mConfig.Finish())));   
   }
   else
   {
      mOpen.push_back(new node_type(pParent, pData, pParent->GetCostToNode() + mCostFunc(pParent->GetData(), pData), mCostFunc(pData, mConfig.Finish())));
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

   node_type* pLink = NULL;

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
   //increment our iteration
   //reset our constraint bookkeeping vars
   ++mConfig.mNumIterations;
   mConfig.mTimeSpent = 0;
   mConfig.mNodesExplored = 0;
   
   mTimer.Update();

   for(;;)
   {
      if(mOpen.empty())
      {
         return NO_PATH;
      }

      //start with the node of lowest cost in the open list
      node_type* pStart = FindLowestCost(mOpen);     
      
      //check if we found a path to the end or if we have exceeded a constraint
      cost_type pCost = (pStart->GetCostToNode() + pStart->GetCostToGoal());
      bool pExceededMaxCost(pCost >= mConfig.mMaxCost);
      bool pHasPathToFinish = mConfig.AtFinish(pStart->GetData());

      mTimer.Update();
      mConfig.mTimeSpent += mTimer.GetDT();
      bool pHasExceededTimeLimit(mConfig.mTimeSpent > mConfig.mMaxTime);

      //if we have exceeded a constraint or found a path to the end return
      if(pHasPathToFinish || pExceededMaxCost || pHasExceededTimeLimit || (mConfig.mNodesExplored >= mConfig.mMaxNodesExplored))
      {
         //\todo combine partial lists instead of clearing them
         mConfig.mResult.clear();

         while(pStart)
         {            
            mConfig.mResult.push_front(pStart->GetData());            
            pStart = pStart->GetParent();
         }

         //log our total costs in the config
         mConfig.mTotalCost = pCost;
         mConfig.mTotalNodesExplored += mConfig.mNodesExplored;
         mConfig.mTotalTime += mConfig.mTimeSpent;
         
         if(pHasPathToFinish) return PATH_FOUND;
         else return PARTIAL_PATH;
      }
      else
      {
         ++mConfig.mNodesExplored;

         //remove this node from the open list
         mOpen.remove(pStart);

         //add it onto the closed list
         mClosed.push_back(pStart);
         
         //we will iterate through the potential places this node can take us
         typename node_type::iterator iter = pStart->begin();
         typename node_type::iterator endOfList = pStart->end();

         //\todo refactor this with a better container type
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
                  //create a new path in the open list
                  AddNodeLink(pStart, pNode);
               }
               else //lets see if we can get there for cheaper
               {
                  //compute cost to pNode from pStart
                  cost_type pNewCost = pStart->GetCostToNode() + mCostFunc(pStart->GetData(), pNode);
                  
                  //if the new g(n) cost is cheaper then the old one delete the old one
                  //and add the new one as the best potential path to pNode
                  if(pNewCost < (pLink->GetCostToNode() + pLink->GetCostToGoal()))
                  {
                     delete pLink;
                     AddNodeLink(pStart, pNode);                     
                  }
                  else
                  {
                     //put the node back in the list since we can't get there for cheaper
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

