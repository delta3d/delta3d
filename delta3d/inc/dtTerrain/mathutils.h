/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* Teague Coonan
*/

#ifndef _MATHUTILS_H
#define _MATHUTILS_H

#include <osg/Math>
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{
   class DT_TERRAIN_EXPORT MathUtils
   {
      public:
      
         //Linear interpolation.
         template <typename Real>
         static Real Lerp(Real t, Real v1, Real v2)
         {
            return v1 + (v2-v1)*t;
         }
         
         ///Fixed point math functions.
         static int I2F(int a) { return a << 12; }
         static int F2I(int a) { return a >> 12; }
         static float F2F(int a) { return a * (1.0f/4096.0f); }
         static int IMul(int a, int b) { return (a*b) >> 12; }
         static int ILerp(int t, int a, int b) { return a + IMul(t,(b-a)); }
         static int Fade(int t) 
         {
            return IMul(IMul(IMul(t,t),t), (IMul(IMul(I2F(6), t)-(I2F(15)), t)+(I2F(10))));
         }
   };
}
#endif
