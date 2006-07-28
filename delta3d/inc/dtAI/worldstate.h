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

#ifndef __DELTA_WORLDSTATE_H__
#define __DELTA_WORLDSTATE_H__

#include <dtAI/export.h>
#include <dtAI/statevariable.h>

#include <dtUtil/functor.h>

#include <map>
#include <string>

namespace dtAI
{
   /**
    * 
    */
   class DT_AI_EXPORT WorldState
   {
      public:
         typedef dtUtil::Functor<float, TYPELIST_1(const WorldState*)> RemainingCostFunctor;
         typedef dtUtil::Functor<bool, TYPELIST_1(const WorldState*)> DesiredStateFunctor;
         typedef std::pair<std::string, IStateVariable*> StringStateMapping;
         typedef std::map<std::string, IStateVariable*> StateVarMapping;

      public:
         WorldState();
         WorldState(const RemainingCostFunctor& pRCF, const DesiredStateFunctor& pDSF);
         WorldState(const WorldState& pWS);         
         WorldState& operator=(const WorldState& pWS);

         virtual ~WorldState();
   
         float GetCost() const;
         void AddCost(float pCost);         
                  
         void AddState(const std::string& pName, IStateVariable* pStateVar);
         IStateVariable* GetState(const std::string& pState);         
         const IStateVariable* GetState(const std::string& pState) const;         

         float RemainingCost() const;
         bool IsDesiredState() const;

      private:
         void FreeMem();
   
         float mCost;
         StateVarMapping mStateVariables;
         RemainingCostFunctor mRemainingCost;
         DesiredStateFunctor mDesiredState;

   };
}//namespace dtAI

#endif // __DELTA_WORLDSTATE_H__
