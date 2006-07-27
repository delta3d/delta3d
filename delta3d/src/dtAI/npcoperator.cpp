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

#include <dtAI/npcoperator.h>

namespace dtAI
{
   NPCOperator::NPCOperator(const std::string& pName, const ApplyOperatorFunctor& pEvalFunc)
      : mName(pName)
      , mPreConditionals()
      , mEffects()
      , mInterrupts()
      , mApplyFunctor(pEvalFunc)
   {
   }
   
   NPCOperator::~NPCOperator()
   {
      mPreConditionals.clear();
      mEffects.clear();
      mInterrupts.clear();
   }


   bool NPCOperator::operator==(const NPCOperator& pRHS) const
   {
      return mName == pRHS.GetName();
   }

   const std::string& NPCOperator::GetName() const
   {
      return mName;
   }

   bool NPCOperator::Apply(const WorldState* pCurrent, WorldState* pWSIn) const
   {
      return mApplyFunctor(pCurrent, pWSIn);
   }

   void NPCOperator::AddPreCondition(IConditional* pCondIn)
   {
      mPreConditionals.push_back(pCondIn);
   }

   void NPCOperator::AddEffect(IConditional* pCondIn)
   {
      mEffects.push_back(pCondIn);
   }

   void NPCOperator::AddInterrupt(IConditional* pCondIn)
   {
      mInterrupts.push_back(pCondIn);
   }

   void NPCOperator::RemovePreCondition(IConditional* pConditional)
   {
      mPreConditionals.remove(pConditional);
   }

   void NPCOperator::RemoveEffect(IConditional* pConditional)
   {
      mEffects.remove(pConditional);
   }

   void NPCOperator::RemoveInterrupt(IConditional* pConditional)
   {
      mInterrupts.remove(pConditional);
   }

   const NPCOperator::ConditionalList& NPCOperator::GetPreConditions() const
   {
      return mPreConditionals;
   }

   const NPCOperator::ConditionalList& NPCOperator::GetEffects() const
   {
      return mEffects;
   }

   const NPCOperator::ConditionalList& NPCOperator::GetInterrupts() const
   {
      return mInterrupts;
   }


}//namespace dtAI
