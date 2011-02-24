/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 */

#ifndef DELTA_TIMER
#define DELTA_TIMER

#include <osg/Timer>
#include <dtCore/export.h>

namespace dtCore
{
   typedef osg::Timer_t Timer_t;

   void DT_CORE_EXPORT AppSleep(unsigned int milliseconds);

   /**
    * High-level wrapper for osg::Timer.
    */
   class DT_CORE_EXPORT Timer
   {
   public:
      Timer();
      ~Timer();

      static const Timer* Instance();

      Timer_t Tick() const;

      double DeltaSec(Timer_t t1, Timer_t t2) const;
      double DeltaMil(Timer_t t1, Timer_t t2) const;
      double DeltaMicro(Timer_t t1, Timer_t t2) const;
      double DeltaNano(Timer_t t1, Timer_t t2) const;

      double ElapsedSeconds() const;
      double ElapsedMilliseconds() const;
      double ElapsedMicroseconds() const;
      double ElapsedNanoseconds() const;


      double GetSecondsPerTick() const;

   private:
      osg::Timer mTimer;
   };
} // namespace dtCore

#endif // DELTA_TIMER
