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

#include <dtAI/worldstate.h>
#include <algorithm>

namespace dtAI
{
   WorldState::WorldState()
      : mCost(0.0f)
      , mStateVariables()
      , mRemainingCost()
      , mDesiredState()
   {

   }

   WorldState::WorldState(const RemainingCostFunctor& pRCF, const DesiredStateFunctor& pDSF)
      : mCost(0.0f)
      , mStateVariables()
      , mRemainingCost(pRCF)
      , mDesiredState(pDSF)
   {
   }
   
   struct deleteFunc
   {
      template<class _Type>
         void operator()(_Type p1)
      {
         delete p1.second; 
      }
   };

   WorldState::~WorldState()
   {
      FreeMem();
   }

   void WorldState::FreeMem()
   {
      std::for_each(mStateVariables.begin(), mStateVariables.end(), deleteFunc());
      mStateVariables.clear();
   }

   WorldState::WorldState(const WorldState& pWS)
   {
      mCost = pWS.mCost;      
      mRemainingCost = pWS.mRemainingCost;
      mDesiredState = pWS.mDesiredState;

      StateVarMapping::const_iterator iter = pWS.mStateVariables.begin();
      StateVarMapping::const_iterator endOfList = pWS.mStateVariables.end();
      while(iter != endOfList)
      {
         mStateVariables.insert(StringStateMapping((*iter).first , (*iter).second->Copy()));
         ++iter;
      }

   }

   WorldState& WorldState::operator =(const WorldState& pWS)
   {
      FreeMem();

      mCost = pWS.mCost;      
      mRemainingCost = pWS.mRemainingCost;
      mDesiredState = pWS.mDesiredState;

      StateVarMapping::const_iterator iter = pWS.mStateVariables.begin();
      StateVarMapping::const_iterator endOfList = pWS.mStateVariables.end();
      while(iter != endOfList)
      {
         mStateVariables.insert(StringStateMapping((*iter).first , (*iter).second->Copy()));
         ++iter;
      }
      return *this;
   }

   float WorldState::GetCost() const
   {
      return mCost;
   }

   void WorldState::AddCost(float pCost)
   {
      mCost += pCost;
   }

   void WorldState::AddState(const std::string& pName, IStateVariable* pStateVar)
   {
      StateVarMapping::iterator iter = mStateVariables.find(pName);
      if(iter == mStateVariables.end()) 
      {
         mStateVariables.insert(StringStateMapping(pName, pStateVar));
      }
   }

   IStateVariable* WorldState::GetState(const std::string& pState)
   {
      StateVarMapping::iterator iter = mStateVariables.find(pState);
      if(iter == mStateVariables.end()) return 0;
      else return (*iter).second;
   }


   const IStateVariable* WorldState::GetState(const std::string& pState) const
   {
      StateVarMapping::const_iterator iter = mStateVariables.find(pState);
      if(iter == mStateVariables.end()) return 0;
      else return (*iter).second;
   }

   float WorldState::RemainingCost() const
   {
      return mRemainingCost(this);
   }

   bool WorldState::IsDesiredState() const
   {
      return mDesiredState(this);
   }


}//namespace dtAI
