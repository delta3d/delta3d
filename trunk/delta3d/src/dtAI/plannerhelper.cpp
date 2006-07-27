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

#include <dtAI/plannerhelper.h>

namespace dtAI
{
   PlannerHelper::PlannerHelper()
      : mOperators(0)
      , mCurrentState(0)
   {
   }
   
   PlannerHelper::~PlannerHelper()
   {
   }

   void PlannerHelper::AddOperator(NPCOperator* pOperator)
   {
      mOperators.push_back(pOperator);
   }

   const PlannerHelper::OperatorList& PlannerHelper::GetOperators() const
   {
      return mOperators;
   }

   const WorldState* PlannerHelper::GetCurrentState() const
   {
      return mCurrentState;
   }

   void PlannerHelper::SetCurrentState(WorldState* pNewState)
   {
      mCurrentState = pNewState;
   }


}//namespace dtAI
