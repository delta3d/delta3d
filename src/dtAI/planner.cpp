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
 * Bradley Anderegg 07/24/2006
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
      FreeMem();
   }

   struct PlannerDeleteFunc
   {
      template<class _Type>
         void operator()(_Type p1)
      {
         delete p1->mState;
         delete p1;
      }
   };

   void Planner::FreeMem()
   {
      std::for_each(mOpen.begin(), mOpen.end(), PlannerDeleteFunc());
      mOpen.clear();
      mConfig.mResult.clear();
   }

   void Planner::Reset(const PlannerConfig& pConfig)
   {
      FreeMem();
      mConfig = pConfig;

      PlannerNodeLink* pNodeLink = new PlannerNodeLink();
      pNodeLink->mState = new WorldState(*mHelper->GetCurrentState());

      mOpen.push_back(pNodeLink);
   }


   void Planner::Reset(const PlannerHelper* pHelper)
   {
      FreeMem();
      mHelper = pHelper;

      PlannerNodeLink* pNodeLink = new PlannerNodeLink();
      pNodeLink->mState = new WorldState(*mHelper->GetCurrentState());

      mOpen.push_back(pNodeLink);
   }

   const PlannerNodeLink* Planner::FindLowestCost()
   {
      PlannerContainer::const_iterator iter = mOpen.begin();
      PlannerContainer::const_iterator endOfList = mOpen.end();

      const PlannerNodeLink* pLowest = *iter;

      while (iter != endOfList)
      {
         const PlannerNodeLink* pNLIter = (*iter);
         if (pNLIter->operator<(*pLowest))
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

   std::list<const Operator*> Planner::GetPlan() const
   {
      return mConfig.mResult;
   }

   std::vector<const Operator*> Planner::GetPlanAsVector() const
   {
      return OperatorVector(GetPlan().begin(), GetPlan().end());
   }

   PlannerConfig& Planner::GetConfig()
   {
      return mConfig;
   }

   const PlannerConfig& Planner::GetConfig() const
   {
      return mConfig;
   }

   bool Planner::CanApplyOperator(const Operator* pOperator, const WorldState* pState)
   {
      Operator::ConditionalList::const_iterator iter =  pOperator->GetPreConditions().begin();
      Operator::ConditionalList::const_iterator endOfList = pOperator->GetPreConditions().end();
      while (iter != endOfList)
      {
         if (!((*iter)->Evaluate(pState)))
         {
            return false;
         }
         ++iter;
      }
      return true;
   }

   void Planner::GetTraversableStates(const WorldState* pCurrentState, const std::list<Operator*>& pOperators, std::list<Operator*>& pOperatorListIn)
   {
      std::list<Operator*>::const_iterator iter = pOperators.begin();
      std::list<Operator*>::const_iterator endOfList = pOperators.end();
      while (iter != endOfList)
      {
         if (CanApplyOperator(*iter, pCurrentState))
         {
            pOperatorListIn.push_back(*iter);
         }
         ++iter;
      }

   }

   Planner::PlannerResult Planner::GeneratePlan()
   {
      mConfig.mTimer.Update();

      for (;;)
      {

         if (mOpen.empty())
         {
            return NO_PLAN;
         }

         mConfig.mCurrentElapsedTime += mConfig.mTimer.GetDT();
         mConfig.mTimer.Update();

         const PlannerNodeLink* pCurrent = FindLowestCost();

         bool pReachedGoal = mHelper->IsDesiredState(pCurrent->mState);

         //we have found our desired state
         if (pReachedGoal || mConfig.mCurrentElapsedTime >= mConfig.mMaxTimePerIteration)
         {
            while (pCurrent->mOperator)
            {
               mConfig.mResult.push_front(pCurrent->mOperator);
               pCurrent = pCurrent->mParent;
            }

            mConfig.mTotalElapsedTime += mConfig.mCurrentElapsedTime;
            mConfig.mCurrentElapsedTime = 0.0;

            if (pReachedGoal) return PLAN_FOUND;
            else return PARTIAL_PLAN;
         }
         else
         {
            Remove(pCurrent);

            std::list<Operator*> pTraverse;
            GetTraversableStates(pCurrent->mState, mHelper->GetOperators(), pTraverse);

            std::list<Operator*>::iterator iter = pTraverse.begin();
            std::list<Operator*>::iterator endOfList = pTraverse.end();

            while (iter != endOfList)
            {
               WorldState* pWS = new WorldState(*(pCurrent->mState));
               PlannerNodeLink* pnl = new PlannerNodeLink();

               (*iter)->Apply(pWS);

               pnl->mOperator = *iter;
               pnl->mState = pWS;
               pnl->mParent = pCurrent;
               pnl->mGCost = pWS->GetCost();
               pnl->mHCost = mHelper->RemainingCost(pWS);

               mOpen.push_back(pnl);

               ++iter;
            }

         }

      }

      return NO_PLAN;
   }

} // namespace dtAI
