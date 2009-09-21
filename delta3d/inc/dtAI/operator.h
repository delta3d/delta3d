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

#ifndef __DELTA_OPERATOR_H__
#define __DELTA_OPERATOR_H__

#include <dtAI/export.h>
#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>

#include <dtUtil/functor.h>
#include <dtCore/refptr.h>

#include <list>
#include <string>

namespace dtAI
{
   /**
    *
    */
   class DT_AI_EXPORT Operator
   {
   public:
      typedef std::list<dtCore::RefPtr<IConditional> > ConditionalList;
      typedef dtUtil::Functor<bool, TYPELIST_2(const Operator*, WorldState*)> ApplyOperatorFunctor;

   public:
      Operator(const std::string& pName, const ApplyOperatorFunctor& pEvalFunc);
      virtual ~Operator();

      bool operator==(const Operator& pRHS) const;

      const std::string& GetName() const;

      bool Apply(WorldState* pWSIn) const;

      void AddPreCondition(IConditional* pCondIn);

      void RemovePreCondition(IConditional* pConditional);

      const ConditionalList& GetPreConditions() const;

   protected:
      Operator(const Operator&);             //not implemented by design
      Operator& operator=(const Operator&);  //not implemented by design

      std::string mName;

      ConditionalList mPreConditionals;

      ApplyOperatorFunctor mApplyFunctor;
   };

} // namespace dtAI

#endif // __DELTA_OPERATOR_H__
