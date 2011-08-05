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
 * Bradley Anderegg 08/10/2006
 */

#include <dtAI/basenpc.h>
#include <dtAI/npcparser.h>
#include <dtAI/npcstate.h>
#include <dtUtil/log.h>
#include <dtUtil/command.h>

namespace dtAI
{
   BaseNPC::BaseNPC(const std::string& pName)
      : mSleeping(true)
      , mName(pName)
      , mStateMachine()
      , mPlanner()
      , mHelper(PlannerHelper::RemainingCostFunctor(this, &BaseNPC::RemainingCost),
                PlannerHelper::DesiredStateFunctor(this, &BaseNPC::IsDesiredState))
      , mWSTemplate()
      , mGoals()
      , mCurrentGoal(NULL)
      , mCurrentPlan()
   {
   }

   BaseNPC::~BaseNPC()
   {
      mCurrentPlan.clear();
      mGoals.clear();
   }

   const std::string& BaseNPC::GetName() const
   {
      return mName;
   }

   void BaseNPC::SetWSTemplate(const WorldState& pWS)
   {
      mWSTemplate = pWS;
   }

   void BaseNPC::InitNPC()
   {
      OnInit();
      InitializeFSM();
      RegisterActions();
   }

   void BaseNPC::OnInit()
   {

   }

   bool BaseNPC::LoadNPCScript(const std::string& pFileName)
   {
      NPCParser parser;
      return parser.LoadScript(pFileName, this);
   }

   void BaseNPC::SpawnNPC()
   {
      mSleeping = false;
      mHelper.SetCurrentState(mWSTemplate);
      if (!mCurrentGoal && !mGoals.empty())
      {
         SetGoal(mGoals.begin()->first);
      }

      mStateMachine.MakeCurrent(&NPCStateTypes::NPC_STATE_SPAWN);
   }

   void BaseNPC::KillNPC()
   {
      mStateMachine.MakeCurrent(&NPCStateTypes::NPC_STATE_DIE);
   }

   void BaseNPC::SetSleeping(bool pIsSleeping)
   {
      mSleeping = pIsSleeping;
   }

   void BaseNPC::Update(double dt)
   {
      if (!mSleeping)
      {
         ExecutePlan(dt);

         mStateMachine.Update(dt);
      }
   }

   bool BaseNPC::SetGoal(const std::string& pGoal)
   {
      GoalMap::iterator iter = mGoals.find(pGoal);

      if (iter != mGoals.end())
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

   const Planner::OperatorList& BaseNPC::GetPlan() const
   {
      return mCurrentPlan;
   }

   void BaseNPC::RegisterActions()
   {

   }

   void BaseNPC::RegisterAction(const std::string& pName, Action pAction)
   {
      ActionMap::iterator iter = mActions.find(pName);
      if (iter == mActions.end())
      {
         mActions.insert(std::pair<std::string, Action>(pName, pAction));
      }
      else
      {
         LOG_WARNING("BaseNPC: " + mName + " attempting to re-register action " + pName);
      }
   }

   float BaseNPC::RemainingCost(const WorldState* pWS) const
   {
      return 1.0f;
   }

   bool BaseNPC::IsDesiredState(const WorldState* pWS) const
   {
      // if we return false we we be in an endless loop
      // perhaps print a warning
      if (!mCurrentGoal)
      {
         return true;
      }

      return mCurrentGoal->second->Evaluate(pWS);
   }


   void BaseNPC::AddOperator(Operator* pOperator)
   {
      mHelper.AddOperator(pOperator);
   }

   const WorldState& BaseNPC::GetWSTemplate() const
   {
      return mWSTemplate;
   }

   WorldState& BaseNPC::GetWSTemplate()
   {
      return mWSTemplate;
   }

   void BaseNPC::AddGoal(const std::string& pName, Goal* pGoal)
   {
      mGoals.insert(std::make_pair(pName, pGoal));
   }

   void BaseNPC::InitializeFSM()
   {
      NPCState* state_default = mStateMachine.AddState(&NPCStateTypes::NPC_STATE_DEFAULT);
      NPCState* state_spawn   = mStateMachine.AddState(&NPCStateTypes::NPC_STATE_SPAWN);
      NPCState* state_idle    = mStateMachine.AddState(&NPCStateTypes::NPC_STATE_IDLE);
      NPCState* state_die     = mStateMachine.AddState(&NPCStateTypes::NPC_STATE_DIE);

      mStateMachine.AddTransition(&NPCEvent::NPC_EVENT_SPAWN, state_default->GetType(), state_spawn->GetType());
      mStateMachine.AddTransition(&NPCEvent::NPC_EVENT_DIE, state_idle->GetType(), state_die->GetType());

      //when spawn first updates, it will call the virtual spawn function
      //set mSleeping to false and select a new state through the
      //virtual select state mechanism on its first update
      mStateMachine.GetState(&NPCStateTypes::NPC_STATE_SPAWN)->AddEntryCommand(new dtUtil::Command1<void, bool>(dtUtil::Functor<void, TYPELIST_1(bool)>(this, &BaseNPC::SetSleeping), false));
      mStateMachine.GetState(&NPCStateTypes::NPC_STATE_SPAWN)->SetUpdate(NPCState::UpdateFunctor(this, &BaseNPC::SelectState));

      //when npc is killed it will automatically go to sleep
      mStateMachine.GetState(&NPCStateTypes::NPC_STATE_DIE)->AddEntryCommand(new dtUtil::Command1<void, bool>(dtUtil::Functor<void, TYPELIST_1(bool)>(this, &BaseNPC::SetSleeping), true));

      //this one is for the user to override
      OnInitializeFSM();

   }

   void BaseNPC::OnInitializeFSM()
   {

   }

   void BaseNPC::SelectState(double dt)
   {
      mStateMachine.MakeCurrent(&NPCStateTypes::NPC_STATE_IDLE);
   }

   bool BaseNPC::ExecuteAction(const std::string& pAction, double dt, WorldState* pWS)
   {
      ActionMap::iterator iter = mActions.find(pAction);
      if (iter != mActions.end())
      {
         return ((*iter).second)(dt, pWS);
      }

      LOG_ERROR("Attempting to execute an action not registered with BaseNPC: " + mName)

      //note we are returning true which will imply that the action has completed
      // if we return false it will keep trying to execute this action which appears
      // not to exist
      return true;
   }

   void BaseNPC::ExecutePlan(double dt)
   {
      Planner::OperatorList::iterator pPlanIter = mCurrentPlan.begin();

      if (pPlanIter != mCurrentPlan.end())
      {
         const Operator* pOperator = *pPlanIter;

         //ExecuteAction will return true when it completes so we can pop that action
         //of the plan... if the action generates a new plan it needs to return false
         //so that we dont loose the first operator of the new plan
         //we should probably make this a critical section or something to prevent this
         if (ExecuteAction(pOperator->GetName(), dt, mHelper.GetCurrentState()))
         {
            mCurrentPlan.pop_front();
         }
         //check if we hit any interrupts
         const NPCOperator* pNPCOperator = dynamic_cast<const NPCOperator*>(pOperator);
         if (pNPCOperator && !pNPCOperator->CheckInterrupts(mHelper.GetCurrentState()))
         {
            //if so, clear the current plan and generate a new one
            GeneratePlan();
         }
      }

   }

} // namespace dtAI
