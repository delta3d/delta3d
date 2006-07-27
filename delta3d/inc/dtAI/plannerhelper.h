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

#ifndef __DELTA_PLANNERHELPER_H__
#define __DELTA_PLANNERHELPER_H__

#include <dtAI/export.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>

namespace dtAI
{
   /**
    * A class used to interface with the planner 
    */
   class DT_AI_EXPORT PlannerHelper
   {
      public:
         typedef std::list<NPCOperator*> OperatorList; 

      public:
   
         PlannerHelper();
         virtual ~PlannerHelper();
   
         void AddOperator(NPCOperator* pOperator);
         const OperatorList& GetOperators() const;

         void SetCurrentState(WorldState* pNewState);
         const WorldState* GetCurrentState() const;


      private:

         OperatorList mOperators;
         WorldState* mCurrentState;
   
   };
}//namespace dtAI

#endif // __DELTA_PLANNERHELPER_H__
