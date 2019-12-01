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

#ifndef __DELTA_BASENPC_H__
#define __DELTA_BASENPC_H__

#include <dtAI/export.h>
#include <dtAI/basenpcutils.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/worldstate.h>
#include <dtAI/planner.h>
#include <dtAI/fsm.h>

#include <dtUtil/functor.h>

#include <osg/Referenced>

#include <map>
#include <string>


namespace dtAI
{
   /**
    * This represents the base delta class used for AI agents having embedded support for the planning architecture
    * and a generic FSM.
    */
   class DT_AI_EXPORT BaseNPC: public osg::Referenced
   {
   public:
      // A Goal is essentially a conditional, but one representing our desired state
      // so if a goal evaluates to true then we have theoretically completed a task of interest
      // if the user overrides IsDesiredState() then they are responsible for either evaluating
      // Goals themselves or potentially not using them at all
      typedef std::map<std::string, Goal*> GoalMap;
      typedef GoalMap::allocator_type::value_type GoalMapping;

      // An action is a function pointer that takes a double representing the frame time
      // and the current world state to modify
      // it returns true when that action has completed
      // NOTE: if the action generates a new plan for this NPC it MUST return false
      // we should add a critical section in ExecutePlan to prevent this mistake
      typedef dtUtil::Functor<bool,TYPELIST_2(double, WorldState*)> Action;
      typedef std::map<std::string, Action> ActionMap;

   public:
      BaseNPC(const std::string& pName);

   protected:
      virtual ~BaseNPC();

   public:
      /**
       * InitNPC() must be called before spawn
       * use OnInitNPC() for derived initialize behavior
       * calls InitializeFSM() and RegisterActions()
       */
      void InitNPC();

      /**
       * SpawnNPC() sets the WorldState of the NPC to be equal to the WorldState template (SetWSTemplate())
       * it then sets a goal from the goal list if it does not already have one
       * and changes the state to NPC_STATE_SPAWN
       */
      void SpawnNPC();

      /**
       * KillNPC() puts the character in the sleep state so it no longer does processing on update
       * and changes the state to NPC_STATE_DIE
       */
      void KillNPC();

      /**
       * Sets the NPC into the Sleep state which does nothing on update
       */
      void SetSleeping(bool pIsSleeping);

      /**
       * Update should be called every frame
       * calls ExecutePlan which will run a plan if one has been generated
       * Updates the state machine which may change states based on input
       */
      virtual void Update(double dt);

      /**
       * Loads Operators, Goals and Conditionals from an NPCScript file
       * see UnitTestsDtAI for an example
       */
      bool LoadNPCScript(const std::string& pFileName);

      /**
       * Sets the current goal by name, the goal is used during GeneratePlan if the user
       * has not overridden IsDesiredState()
       */
      bool SetGoal(const std::string& pGoal);

      /**
       * GeneratePlan() uses the Planner to generate a plan.
       * The plan is a list of Operators which can be added manually or
       * added through LoadNPCScript with a valid script file.
       * The plan is created to be of minimal cost (uses RemainingCost()) and satisfy the
       * IsDesiredState() function
       * @see AddOperator()
       * @see LoadNPSScript()
       * @see RemainingCost()
       */
      void GeneratePlan();

      /**
       * Actions are function pointers called by name during ExecutePlan()
       * there must be an action registered for every Operator with the same name
       * returned by the Operators GetName() function
       */
      void RegisterAction(const std::string& pName, Action pAction);

      /**
       * Operators define what the NPC can do and how it affects the NPC's state
       * Operators are used to chain together in order to satisfy a Goal
       * they can be added manually or added by LoadNPCScript()
       */
      void AddOperator(Operator* pOperator);

      /**
       * Goals are used to control an NPC's behavior they evaluate a WorldState
       * and return whether or not the WorldState completes the Goal
       * Goals can be added manually or added through LoadNPCScript()
       */
      void AddGoal(const std::string& pName, Goal* pGoal);


      /**
       * GetPlan() returns the last plan generated through GeneratePlan()
       * where a plan is just a list of Operators designed to achieve a specific Goal
       */
      const Planner::OperatorList& GetPlan() const;

      /**
       * GetWSTemplate() returns the template of the WorldState for this NPC
       * a WorldState Template can be thought of as the initial state of the NPC
       */
      const WorldState& GetWSTemplate() const;
      WorldState& GetWSTemplate();

      /**
       * Sets the template used for this NPC's initial state
       * since this wipes out previous state template
       * it is recommended to do a GetWSTemplate() and then
       * add the appropriate state variables
       */
      void SetWSTemplate(const WorldState& pWS);

      /**
       * Returns the name of this NPC
       */
      const std::string& GetName() const;

   protected:
      /**
       * Override this to set the initial state of the NPC after it is spawned
       */
      virtual void SelectState(double dt);

      /**
       * Runs an action of specifed name, where an action is a function that modifies
       * the NPC's WorldState
       */
      bool ExecuteAction(const std::string& pAction, double dt, WorldState* pWS);

      /**
       * Calls GeneratePlan if no plan exists
       * Proceeds to run each Action registered with a specific name of an Operator
       * moving to call the next Action when the current one returns true
       * called on Update()
       */
      virtual void ExecutePlan(double dt);

      /**
       * Sets up the FSM with the basic states DEFAULT, SPAWN, IDLE, DIE
       * as defined in NPCState.h
       */
      void InitializeFSM();

      /**
       * Override OnInitializeFSM() to add states and transitions specific to your NPC
       */
      virtual void OnInitializeFSM();

      /**
       * Called from InitNPC().  Add all the Operators this NPC can perform here.
       * @see AddOperator()
       */
      virtual void OnInit();

      /**
       * Override to register function pointers with strings which correspond to
       * the name of this NPC's Operators using RegisterAction().
       * NOTE: In order to run a generated plan you must override this and register an
       * Action for each potential Operator.
       */
      virtual void RegisterActions();

      /**
       * Used by the planner on GeneratePlan
       * Should estimate the cost remaining to achieve the desired goal.
       * The more accurate this is, the more optimized the algorithm will
       * be able to generate a plan.
       */
      virtual float RemainingCost(const WorldState* pWS) const;

      /**
       * Used by the planner on GeneratePlan
       * returns true when the WorldState passed in is desirable
       * the default implementation uses a specific Goal to evaluate
       * the plan and determine if certain conditionals are met
       */
      virtual bool IsDesiredState(const WorldState* pWS) const;

      bool mSleeping;
      const std::string mName;

      FSM mStateMachine;

      ActionMap mActions;

      Planner mPlanner;
      PlannerHelper mHelper;
      WorldState mWSTemplate;

      GoalMap mGoals;
      GoalMapping* mCurrentGoal;

      Planner::OperatorList mCurrentPlan;
   };

} // namespace dtAI

#endif // __DELTA_BASENPC_H__
