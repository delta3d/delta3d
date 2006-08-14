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
 * @author Bradley Anderegg 08/10/2006
 */

#include <dtAI/basenpc.h>

namespace dtAI
{
   BaseNPC::BaseNPC(const std::string& pName)
      : mSleeping(true)
      , mName(pName)
      , mPlanner()
      , mHelper(PlannerHelper::RemainingCostFunctor(this, &BaseNPC::RemainingCost), PlannerHelper::DesiredStateFunctor(this, &BaseNPC::IsDesiredState))
      , mWSTemplate()      
      , mGoals()
      , mCurrentGoal(0)
      , mCurrentPlan()
   {
   }
   
   BaseNPC::~BaseNPC()
   {
   }


   void BaseNPC::Spawn()
   {
      mSleeping = false;
      mHelper.SetCurrentState(mWSTemplate);
      if(!mCurrentGoal && !mGoals.empty()) SetGoal(mGoals.begin()->first);
      OnSpawn();
   }

   void BaseNPC::OnSpawn()
   {

   }

   void BaseNPC::Kill()
   {
      mSleeping = true;
      OnKill();
   }

   void BaseNPC::OnKill()
   {

   }

   void BaseNPC::Update(double dt)
   {
      if(!mSleeping)
      {
         ExecutePlan();
         OnUpdate(dt);
      }
   }

   void BaseNPC::OnUpdate(double dt)
   {

   }

   bool BaseNPC::SetGoal(const std::string& pGoal)
   {
      GoalMap::iterator iter = mGoals.find(pGoal);

      if(iter != mGoals.end())
      {
         mCurrentGoal = &(*iter);
         return true;
      }
      
      return false;
   }

   void BaseNPC::GeneratePlan()
   {
     mCurrentPlan.clear();
     mPlanner.Reset(&mHelper);
     mPlanner.GeneratePlan();
     mCurrentPlan = mPlanner.GetPlan();
   }

   Planner::OperatorList BaseNPC::GetPlan() const
   {
      return mCurrentPlan;
   }

   float BaseNPC::RemainingCost(const WorldState* pWS) const
   {
      return 1.0f;
   }

   bool BaseNPC::IsDesiredState(const WorldState* pWS) const
   {      
      //if we return false we we be in an endless loop
      //perhaps print a warning
      if(!mCurrentGoal) return true;

      return mCurrentGoal->second->Evaluate(pWS);
   }


   void BaseNPC::AddOperator(NPCOperator* pOperator)
   {
      mHelper.AddOperator(pOperator);
   }

   void BaseNPC::SetWSTemplate(const WorldState& pWS)
   {
      mWSTemplate = pWS;
   }

   void BaseNPC::AddGoal(const std::string& pName, Goal* pGoal)
   {
      mGoals.insert(std::make_pair(pName, pGoal));
   }

   void BaseNPC::ExecutePlan()
   {

   }


}//namespace dtAI
