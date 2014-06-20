/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
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

#include <dtAI/operator.h>
#include <algorithm>

namespace dtAI
{
   Operator::Operator(const std::string& pName, const ApplyOperatorFunctor& pEvalFunc)
      : mName(pName)
      , mPreConditionals()
      , mApplyFunctor(pEvalFunc)
   {
   }
   
   Operator::~Operator()
   {
      mPreConditionals.clear();
   }


   bool Operator::operator==(const Operator& pRHS) const
   {
      return mName == pRHS.GetName();
   }

   const std::string& Operator::GetName() const
   {
      return mName;
   }

   bool Operator::Apply(WorldState* pWSIn) const
   {
      return mApplyFunctor(this, pWSIn);
   }

   void Operator::AddPreCondition(IConditional* pCondIn)
   {
      mPreConditionals.push_back(pCondIn);
   }

   void Operator::RemovePreCondition(IConditional* pConditional)
   {
      mPreConditionals.remove(pConditional);
   }

   const Operator::ConditionalList& Operator::GetPreConditions() const
   {
      return mPreConditionals;
   }


}//namespace dtAI
