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

#include <dtAI/plannerhelper.h>
#include <algorithm>

namespace dtAI
{

   PlannerHelper::PlannerHelper(const RemainingCostFunctor& pRCF, const DesiredStateFunctor& pDSF)
      : mOperators()
      , mCurrentState(new WorldState())
      , mRemainingCost(pRCF)
      , mDesiredState(pDSF)
   {
   }

   struct PlannerHelperDeleteFunc
   {
      template<class _Type>
         void operator()(_Type p1)
      {
         delete p1;
      }
   };

   PlannerHelper::~PlannerHelper()
   {
      //\fix me- this crashes boost python... define ownership of operators
      std::for_each(mOperators.begin(), mOperators.end(), PlannerHelperDeleteFunc());
      mOperators.clear();

      delete mCurrentState;
      mCurrentState = 0;
   }

   void PlannerHelper::AddOperator(Operator* pOperator)
   {
      mOperators.push_back(pOperator);
   }

   void PlannerHelper::RemoveOperator(Operator* pOperator)
   {
      mOperators.remove(pOperator);
   }

   const PlannerHelper::OperatorList& PlannerHelper::GetOperators() const
   {
      return mOperators;
   }

   WorldState* PlannerHelper::GetCurrentState()
   {
       return mCurrentState;
   }

   const WorldState* PlannerHelper::GetCurrentState() const
   {
      return mCurrentState;
   }

   void PlannerHelper::SetCurrentState(const WorldState& pNewState)
   {
      mCurrentState->operator =(pNewState);
   }


   float PlannerHelper::RemainingCost(const WorldState* pWS) const
   {
      return mRemainingCost(pWS);
   }

   bool PlannerHelper::IsDesiredState(const WorldState* pWS) const
   {
      return mDesiredState(pWS);
   }

   void PlannerHelper::SetDesiredStateFunc(const DesiredStateFunctor& pFunc)
   {
      mDesiredState = pFunc;
   }

   void PlannerHelper::SetRemainingCostFunc(const RemainingCostFunctor& pFunc)
   {
      mRemainingCost = pFunc;
   }

   const Operator* PlannerHelper::GetOperator(const std::string& pName) const
   {
      OperatorList::const_iterator iter = mOperators.begin();
      OperatorList::const_iterator endOfList = mOperators.end();

      while (iter != endOfList)
      {
         if ((*iter)->GetName() == pName)
         {
            return *iter;
         }
         ++iter;
      }
      return 0;
   }

} // namespace dtAI
