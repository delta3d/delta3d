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

#ifndef __DELTA_PLANNERCONFIG_H__
#define __DELTA_PLANNERCONFIG_H__

#include <dtCore/timer.h>

// vs thinks std::numeric_limits<>::max() is a macro
#ifdef max
#undef max
#endif
#include <limits>

#include <list>

namespace dtAI
{
   class PlannerTimer
   {
   public:
      void Update(){ mTime = dtCore::Timer::Instance()->Tick();}
      double GetDT(){return dtCore::Timer::Instance()->DeltaSec(mTime, dtCore::Timer::Instance()->Tick());}

   private:
      dtCore::Timer_t mTime;
   };

   /**
    *
    */
   class PlannerConfig
   {
   public:
      PlannerConfig()
         : mResult()
         , mMaxTimePerIteration(std::numeric_limits<double>::max())
         , mCurrentElapsedTime(0.0)
         , mTotalElapsedTime(0.0)
         , mTimer()
      {

      }

      std::list<const Operator*> mResult;

      double mMaxTimePerIteration;
      double mCurrentElapsedTime;
      double mTotalElapsedTime;

      PlannerTimer mTimer;
   };

} // namespace dtAI

#endif // __DELTA_PLANNERCONFIG_H__
