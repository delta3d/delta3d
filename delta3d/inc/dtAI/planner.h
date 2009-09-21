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
 * Bradley Anderegg 07/24/2006
 */

#ifndef __DELTA_PLANNER_H__
#define __DELTA_PLANNER_H__

#include <dtAI/export.h>
#include <dtAI/plannernodelink.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/plannerconfig.h>
#include <dtAI/worldstate.h>

#include <list>
#include <vector>

namespace dtAI
{
   /**
    * A game oriented Planner modeled after Jeff Orkin's F.E.A.R Planner
    */
   class DT_AI_EXPORT Planner
   {
   public:
      enum PlannerResult{NO_PLAN, PLAN_FOUND, PARTIAL_PLAN};

      typedef std::list<const PlannerNodeLink*> PlannerContainer;
      typedef std::list<const Operator*> OperatorList;
      typedef std::vector<const Operator*> OperatorVector;

   public:
      Planner();
      virtual ~Planner();

      void Reset(const PlannerConfig& pConfig);
      void Reset(const PlannerHelper* pHelper);

      PlannerResult GeneratePlan();

      OperatorList GetPlan() const;

      PlannerConfig& GetConfig();
      const PlannerConfig& GetConfig() const;

      OperatorVector GetPlanAsVector() const;

   private:
      void FreeMem();

      const PlannerNodeLink* FindLowestCost();
      void Remove(const PlannerNodeLink* pNodeLink);
      bool Contains(PlannerNodeLink* pNodeLink);
      bool CanApplyOperator(const Operator* pOperator, const WorldState* pState);
      void GetTraversableStates(const WorldState* pCurrentState, const std::list<Operator*>& pOperators, std::list<Operator*>& pOperatorListIn);

      const PlannerHelper* mHelper;
      PlannerConfig mConfig;
      PlannerContainer mOpen;
   };

} // namespace dtAI

#endif // __DELTA_PLANNER_H__
