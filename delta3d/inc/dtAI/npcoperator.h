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

#ifndef __DELTA_NPCOPERATOR_H__
#define __DELTA_NPCOPERATOR_H__

#include <dtAI/export.h>
#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>

#include <dtUtil/functor.h>

#include <list>
#include <string>

namespace dtAI
{
   /**
    * 
    */
   class DT_AI_EXPORT NPCOperator
   {
      public:
         typedef std::list<IConditional*> ConditionalList;
         typedef dtUtil::Functor<bool, TYPELIST_2(const WorldState*, WorldState*)> ApplyOperatorFunctor;

      public:
   
         NPCOperator(const std::string& pName, const ApplyOperatorFunctor& pEvalFunc);
         virtual ~NPCOperator();

         bool operator==(const NPCOperator& pRHS) const;

         const std::string& GetName() const;

         bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const;

         void AddPreCondition(IConditional* pCondIn);
         void AddEffect(IConditional* pCondIn);
         void AddInterrupt(IConditional* pCondIn);
   
         void RemovePreCondition(IConditional* pConditional);
         void RemoveEffect(IConditional* pConditional);
         void RemoveInterrupt(IConditional* pConditional);

         const ConditionalList& GetPreConditions() const;
         const ConditionalList& GetEffects() const;
         const ConditionalList& GetInterrupts() const;

      private:
         NPCOperator(const NPCOperator&);             //not implemented by design
         NPCOperator& operator=(const NPCOperator&);  //not implemented by design

         std::string mName;

         ConditionalList mPreConditionals;
         ConditionalList mEffects;
         ConditionalList mInterrupts;

         ApplyOperatorFunctor mApplyFunctor;
   
   };
}//namespace dtAI

#endif // __DELTA_NPCOPERATOR_H__
