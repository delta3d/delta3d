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

namespace dtAI
{
   WorldState::WorldState(const RemainingCostFunctor& pRCF, const DesiredStateFunctor& pDSF)
      : mCost(0.0f)
      , mStateVariables()
      , mRemainingCost(pRCF)
      , mDesiredState(pDSF)
   {
   }
   
   WorldState::~WorldState()
   {
   }

   WorldState::WorldState(const WorldState& pWS)
   {
      mCost = pWS.mCost;      
      mRemainingCost = pWS.mRemainingCost;
      mDesiredState = pWS.mDesiredState;

      StateVarList::const_iterator iter = pWS.mStateVariables.begin();
      StateVarList::const_iterator endOfList = pWS.mStateVariables.end();
      while(iter != endOfList)
      {
         mStateVariables.push_back((*iter)->Copy());
         ++iter;
      }

   }

   float WorldState::GetCost() const
   {
      return mCost;
   }

   void WorldState::AddCost(float pCost)
   {
      mCost += pCost;
   }

   void WorldState::AddState(IStateVariable* pStateVar)
   {
      mStateVariables.push_back(pStateVar);
   }

   IStateVariable* WorldState::GetState(const std::string& pState)
   {
      StateVarList::iterator iter = mStateVariables.begin();
      StateVarList::iterator endOfList = mStateVariables.end();

      while(iter != endOfList)
      {
         if((*iter)->GetName() == pState)
         {
            return (*iter);
         }
         ++iter;
      }
      return 0;
   }


   const IStateVariable* WorldState::GetState(const std::string& pState) const
   {
      StateVarList::const_iterator iter = mStateVariables.begin();
      StateVarList::const_iterator endOfList = mStateVariables.end();

      while(iter != endOfList)
      {
         if((*iter)->GetName() == pState)
         {
            return (*iter);
         }
         ++iter;
      }
      return 0;
   }


   const WorldState::StateVarList& WorldState::GetStateVars() const
   {
      return mStateVariables;
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
