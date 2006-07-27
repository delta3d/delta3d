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

#ifndef __DELTA_PLANNERCONFIG_H__
#define __DELTA_PLANNERCONFIG_H__


namespace dtAI
{
   /**
    * 
    */
   class PlannerConfig
   {
      public:


         const WorldState* mGoal;
         const WorldState* mAcceptedFinalState;
         std::list<const NPCOperator*> mResult;
         
   
   };
}//namespace dtAI

#endif // __DELTA_PLANNERCONFIG_H__
