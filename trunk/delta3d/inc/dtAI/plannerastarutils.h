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
 * @author Bradley Anderegg 07/24/2006
 */

#ifndef __DELTA_PLANNERASTARUTILS_H__
#define __DELTA_PLANNERASTARUTILS_H__

#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <list>

namespace dtAI
{
   /**
    * Utilities for using AStar with our Planner
    */


   namespace PlannerUtils
   {

      bool CanApplyOperator(const NPCOperator* pOperator, const WorldState* pState)
      {
         NPCOperator::ConditionalList::const_iterator iter =  pOperator->GetPreConditions().begin();
         NPCOperator::ConditionalList::const_iterator endOfList = pOperator->GetPreConditions().end();
         while(iter != endOfList)
         {
            if(!((*iter)->Evaluate(pState)))
            {
               return false;
            }
            ++iter;
         }
         return true;
      }

      void GetTraversableStates(const WorldState* pCurrentState, const std::list<NPCOperator*>& pOperators, std::list<NPCOperator*>& pOperatorListIn)
      {
         std::list<NPCOperator*>::const_iterator iter = pOperators.begin();
         std::list<NPCOperator*>::const_iterator endOfList = pOperators.end();
         while(iter != endOfList)
         {
            if(CanApplyOperator(*iter, pCurrentState))
            {
               pOperatorListIn.push_back(*iter);
            }
            ++iter;
         }

      }

   };

   class PlannerNodeLink
   {
      public:   
         
         PlannerNodeLink(): mState(0), mOperator(0), mParent(0), mGCost(0), mHCost(0){}

         bool operator <(const PlannerNodeLink& pnl) const
         {
            return (mGCost + mHCost) < (pnl.mGCost + pnl.mHCost);
         }

         const WorldState* mState;
         const NPCOperator* mOperator;

         const PlannerNodeLink* mParent;
         float mGCost, mHCost;
   };

   //class WorldStateIter
   //{
   //public:
   //   typedef std::list<WorldState>::iterator iterator;
   //   
   //   WorldStateIter(const iterator& pIter): mIter(pIter){}  
   //   WorldStateIter(const WorldStateIter& pIter){mIter = pIter.container();}
   //   WorldStateIter& operator=(WorldStateIter& pIter){mIter = pIter.container(); return *this;}

   //   bool WorldStateIter::operator !=(const WorldStateIter& pIter) const
   //   {
   //      return mIter != pIter.container();
   //   }

   //   const WorldStateIter* operator*() const
   //   {
   //      return *mIter;
   //   }

   //   WorldStateIter& operator++()
   //   {
   //      ++mIter;
   //      return *this;
   //   }

   //   iterator container()const{return mIter;}

   //private:
   //   iterator mIter;

   //};


   //class WorldStateNode: public AStarNode<WorldStateNode, PlannerNodeLink, WorldStateIter, float>
   //{
   //   WorldStateNode(node_type* pParent, PlannerNodeLink pWorldState, cost_type pGn, cost_type pHn): BaseType(pParent, pWorldState, pGn, pHn){}

   //   /*virtual*/ iterator begin() const 
   //   {
   //      return iterator();
   //   }

   //   /*virtual*/ iterator end() const
   //   {
   //      return iterator();
   //   }

   //private:
   //   std::list<WorldState*> 

   //};


   //class WorldStateCostFunc: public AStarCostFunc<PlannerNodeLink, float>
   //{
   //public:
   //   float operator()(PlannerNodeLink pFrom, PlannerNodeLink pTo) const
   //   {
   //      pTo.mOperator->Evaluate(pFrom.mState, pTo.mState);         
   //      return pTo.mState->GetCost();
   //   }
   //};


   //class PlannerTimer
   //{
   //public:
   //   void Update(){ mTime = dtCore::Timer::Instance()->Tick();}
   //   double GetDT(){return dtCore::Timer::Instance()->DeltaMil(mTime, dtCore::Timer::Instance()->Tick());}

   //private:
   //   dtCore::Timer_t mTime;
   //};
   //



   //typedef AStar<WorldStateNode, WorldStateCostFunc, std::list<PlannerNodeLink>, PlannerTimer> PlannerAStar;


}//namespace dtAI

#endif // __DELTA_PLANNERASTARUTILS_H__
