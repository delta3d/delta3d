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
* Matthew W. Campbell
*/
#include "dtTerrain/fixedpointnoise.h"
#include "dtTerrain/mathutils.h"

namespace dtTerrain
{
   const int I = (1 << 12);
   
   ////////////////////////////////////////////////////////////////////////// 
   int FixedPointNoise::operator()(int x, int y, int z)
   {
      unsigned char gx = x >> 12;
      unsigned char gy = y >> 12;
      unsigned char gz = z >> 12;

      x &= I-1;
      y &= I-1;
      z &= I-1;

      int fx = MathUtils::Fade(x);
      int fy = MathUtils::Fade(y);
      int fz = MathUtils::Fade(z);

      unsigned char p0 = Permutation(gx)+gy;
      unsigned char p1 = Permutation(gx+1)+gy;

      unsigned char p00 = Permutation(p0)+gz;
      unsigned char p01 = Permutation(p0+1)+gz;
      unsigned char p10 = Permutation(p1)+gz;
      unsigned char p11 = Permutation(p1+1)+gz;

      unsigned char p000 = Permutation(p00);
      unsigned char p001 = Permutation(p00+1);
      unsigned char p010 = Permutation(p01);
      unsigned char p011 = Permutation(p01+1);
      unsigned char p100 = Permutation(p10);
      unsigned char p101 = Permutation(p10+1);
      unsigned char p110 = Permutation(p11);
      unsigned char p111 = Permutation(p11+1);

      
      int noise = MathUtils::ILerp(fz,
                     MathUtils::ILerp(fy,
                        MathUtils::ILerp(fx,Grad(p000,x,y,z),Grad(p100,x-I,y,z)),
                        MathUtils::ILerp(fx,Grad(p010,x,y-I,z),Grad(p110,x-I,y-I,z))),
                     MathUtils::ILerp(fy,
                        MathUtils::ILerp(fx,Grad(p001,x,y,z-I),Grad(p101,x-I,y,z-I)),
                        MathUtils::ILerp(fx,Grad(p011,x,y-I,z-I),Grad(p111,x-I,y-I,z-I))));
         
      return noise;
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   int FixedPointNoise::operator()(int x, int y)
   {      
      unsigned char gx = x >> 12;
      unsigned char gy = y >> 12;

      x &= I-1;
      y &= I-1;

      int fx = MathUtils::Fade(x);
      int fy = MathUtils::Fade(y);

      unsigned char p0 = Permutation(gx)+gy;
      unsigned char p1 = Permutation(gx+1)+gy;

      unsigned char p00 = Permutation(p0);
      unsigned char p01 = Permutation(p0+1);
      unsigned char p10 = Permutation(p1);
      unsigned char p11 = Permutation(p1+1);

      return MathUtils::ILerp(fy, MathUtils::ILerp(fx,Grad(p00,x,y),Grad(p10, x-I, y)),
               MathUtils::ILerp(fx,Grad(p01, x, y-I),Grad(p11, x-I, y-I)));
   }
   
   //////////////////////////////////////////////////////////////////////////       
   int FixedPointNoise::operator()(int x)
   {
      unsigned char gx = x >> 12;

      x &= I-1;
      int fx = MathUtils::Fade(x);

      unsigned char p0 = Permutation(gx);
      unsigned char p1 = Permutation(gx+1);

      return MathUtils::ILerp(fx,Grad(p0, x),Grad(p1, x-I));    
   }
   
}
