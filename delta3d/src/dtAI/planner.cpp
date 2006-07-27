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

#include <dtAI/planner.h>

#include <algorithm>

namespace dtAI
{
   Planner::Planner()
      : mHelper(0)
      , mConfig()
   {
   }
   
   Planner::~Planner()
   {
   }

   void Planner::Reset(const PlannerConfig& pConfig)
   {
      mOpen.clear();
      mConfig.mResult.clear();
      mConfig = pConfig;      
      
      PlannerNodeLink* pNodeLink = new PlannerNodeLink();
      pNodeLink->mState = mHelper->GetCurrentState(); 

      mOpen.push_back(pNodeLink);
   }


   void Planner::Reset(const WorldState* pDesiredState, const PlannerHelper* pHelper)
   {
      mOpen.clear();
      mConfig.mResult.clear();

      mHelper = pHelper;
      mConfig.mGoal = pDesiredState;     


      PlannerNodeLink* pNodeLink = new PlannerNodeLink();
      pNodeLink->mState = mHelper->GetCurrentState(); 

      mOpen.push_back(pNodeLink);
   }

   const PlannerNodeLink* Planner::FindLowestCost()
   {
      PlannerContainer::const_iterator iter = mOpen.begin();
      PlannerContainer::const_iterator endOfList = mOpen.end();

      const PlannerNodeLink* pLowest = *iter;

      while(iter != endOfList)
      {
         const PlannerNodeLink* pNLIter = (*iter);
         if(pNLIter->operator<(*pLowest))
         {
            pLowest = pNLIter;
         }
         ++iter;
      }

      return pLowest;
   }

   void Planner::Remove(const PlannerNodeLink* pNodeLink)
   {
      mOpen.remove(pNodeLink);
   }

   bool Planner::Contains(PlannerNodeLink* pNodeLink)
   {
      return std::find(mOpen.begin(), mOpen.end(), pNodeLink) != mOpen.end();
   }

   PlannerConfig& Planner::GetResult()
   {
      return mConfig;
   }

   Planner::PlannerResult Planner::GeneratePlan()
   {
      for (;;)
      {

         if(mOpen.empty())
         {
            return NO_PLAN;
         }

         const PlannerNodeLink* pCurrent = FindLowestCost();
         
         //we have found our desired state
         if(pCurrent->mState->IsDesiredState())
         {
            while(pCurrent->mOperator)
            {
               mConfig.mResult.push_front(pCurrent->mOperator);
               pCurrent = pCurrent->mParent;
            }
            return PLAN_FOUND;
         }
         else
         {
            Remove(pCurrent);
            
            std::list<NPCOperator*> pTraverse;            
            PlannerUtils::GetTraversableStates(pCurrent->mState, mHelper->GetOperators(), pTraverse);

            std::list<NPCOperator*>::iterator iter = pTraverse.begin();
            std::list<NPCOperator*>::iterator endOfList = pTraverse.end();

            while(iter != endOfList)
            {
               WorldState* pWS = new WorldState(*(pCurrent->mState));
               PlannerNodeLink* pnl = new PlannerNodeLink();

               (*iter)->Apply(pCurrent->mState, pWS);

               pnl->mOperator = *iter;
               pnl->mState = pWS;
               pnl->mParent = pCurrent;
               pnl->mGCost = pWS->GetCost();
               pnl->mHCost = pWS->RemainingCost();

               mOpen.push_back(pnl);

               ++iter;
            }

         }

      }

      return NO_PLAN;
   }

}//namespace dtAI
