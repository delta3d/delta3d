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
   /* Delta3D Implementation of OSG's Timer. osg::Timer does not
   * provide a public API to switch the bool controlling the use
   * of the system clock, and since it's inaccurate, we must flip it.
   */

   class DT_EXPORT Timer : public osg::Timer
   {

   #ifdef WIN32
   public:
         Timer();
   #endif //WIN32

   };
};

#endif // DELTA_TIMER
