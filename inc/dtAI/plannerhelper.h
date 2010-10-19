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

#ifndef __DELTA_PLANNERHELPER_H__
#define __DELTA_PLANNERHELPER_H__

#include <dtAI/export.h>
#include <dtAI/operator.h>
#include <dtAI/worldstate.h>

#include <dtUtil/functor.h>

namespace dtAI
{
   /**
    * A class used to interface with the planner
    */
   class DT_AI_EXPORT PlannerHelper
   {
   public:
      typedef dtUtil::Functor<float, TYPELIST_1(const WorldState*)> RemainingCostFunctor;
      typedef dtUtil::Functor<bool, TYPELIST_1(const WorldState*)> DesiredStateFunctor;
      typedef std::list<Operator*> OperatorList;

   public:
      PlannerHelper(const RemainingCostFunctor& pRCF, const DesiredStateFunctor& pDSF);
      virtual ~PlannerHelper();

      void AddOperator(Operator* pOperator);
      void RemoveOperator(Operator* pOperator);

      const Operator* GetOperator(const std::string& pName) const;
      const OperatorList& GetOperators() const;

      void SetCurrentState(const WorldState& pNewState);

      WorldState* GetCurrentState();
      const WorldState* GetCurrentState() const;

      void SetRemainingCostFunc(const RemainingCostFunctor& pFunc);
      void SetDesiredStateFunc(const DesiredStateFunctor& pFunc);

      float RemainingCost(const WorldState* pWS) const;
      bool IsDesiredState(const WorldState* pWS) const;

   private:
      OperatorList mOperators;
      WorldState* mCurrentState;

      RemainingCostFunctor mRemainingCost;
      DesiredStateFunctor mDesiredState;
   };

} // namespace dtAI

#endif // __DELTA_PLANNERHELPER_H__
