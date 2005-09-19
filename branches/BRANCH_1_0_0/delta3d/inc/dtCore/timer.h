/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004 MOVES Institute 
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
#include "dtCore/export.h"

namespace dtCore
{
   /** Delta3D Implementation of OSG's Timer. osg::Timer does not
   * provide a public API to switch the bool controlling the use
   * of the system clock, and since it's inaccurate, we must flip it.
   */

#if defined(_MSC_VER)
   typedef __int64 Timer_t;
#else
   typedef unsigned long long Timer_t;
#endif

   /** Time stamper. */
   class DT_EXPORT Timer : public osg::Timer
   {

   public:

      Timer();
      ~Timer() {}

      ///get a static reference to the timer
      static const Timer* instance();

      ///updates the timer
      Timer_t tick() const;

      //inline double delta_s( Timer_t t1, Timer_t t2 ) const { return (double)(t2 - t1)*_secsPerTick; }
      //inline double delta_m( Timer_t t1, Timer_t t2 ) const { return delta_s(t1,t2)*1e3; }
      //inline double delta_u( Timer_t t1, Timer_t t2 ) const { return delta_s(t1,t2)*1e6; }
      //inline double delta_n( Timer_t t1, Timer_t t2 ) const { return delta_s(t1,t2)*1e9; }

      //inline double getSecondsPerTick() const { return _secsPerTick; }

   //protected :

      //double _secsPerTick;

   };
};

#endif // DELTA_TIMER
