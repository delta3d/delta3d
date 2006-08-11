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

#ifndef __DELTA_BASENPC_H__
#define __DELTA_BASENPC_H__

#include <dtAI/export.h>
#include <dtAI/basenpcutils.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/worldstate.h>
#include <dtAI/planner.h>

#include <map>
#include <string>


namespace dtAI
{
   /**
    * This represents the base class used for an ai agent having embedded support for the planning architecture
    */
   class DT_AI_EXPORT BaseNPC
   {
      public:

         typedef std::map<std::string, Goal*> GoalMap;
         typedef GoalMap::allocator_type::value_type GoalMapping;

      public:
   
         BaseNPC(const std::string& pName);
         virtual ~BaseNPC();
   
         void Spawn();
         virtual void OnSpawn();

         void Kill();
         virtual void OnKill();

         void Update(double dt);
         virtual void OnUpdate(double dt);

         bool SetGoal(const std::string& pGoal);
         void GeneratePlan();

         virtual float RemainingCost(const WorldState* pWS) const;
         virtual bool IsDesiredState(const WorldState* pWS) const;


         void AddOperator(NPCOperator* pOperator);
         void SetWSTemplate(const WorldState& pWS);
         void AddGoal(const std::string& pName, Goal* pGoal);

      private:

         void ExecutePlan();

         bool mSleeping;
         const std::string mName;

         Planner mPlanner;
         PlannerHelper mHelper;
         WorldState mWSTemplate;

         GoalMap mGoals;        
         GoalMapping* mCurrentGoal;
         Planner::OperatorList mCurrentPlan;
   
   };
}//namespace dtAI

#endif // __DELTA_BASENPC_H__
