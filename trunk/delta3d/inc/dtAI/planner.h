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

#ifndef __DELTA_PLANNER_H__
#define __DELTA_PLANNER_H__

#include <dtAI/export.h>
#include <dtAI/plannerastarutils.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/plannerconfig.h>
#include <dtAI/worldstate.h>

#include <list>

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
         typedef std::list<const NPCOperator*> OperatorList;
     
      public:
   
         Planner();
         virtual ~Planner();     

         void Reset(const PlannerConfig& pConfig);
         void Reset(const PlannerHelper* pHelper);

         PlannerResult GeneratePlan();

         OperatorList GetPlan() const;

         PlannerConfig& GetConfig();
         const PlannerConfig& GetConfig() const;         
         
      private:
         void FreeMem();
         
         const PlannerNodeLink* FindLowestCost();
         void Remove(const PlannerNodeLink* pNodeLink);
         bool Contains(PlannerNodeLink* pNodeLink);

         const PlannerHelper* mHelper;
         PlannerConfig mConfig;    
         PlannerContainer mOpen;
   
   };
}//namespace dtAI

#endif // __DELTA_PLANNER_H__
