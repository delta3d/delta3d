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
#ifndef DELTA_FIXEDPOINTNOISE
#define DELTA_FIXEDPOINTNOISE

namespace dtTerrain
{
   
   /**
    * This is a fixed point implementation of an improved Ken Perlin noise
    * generator.  This code comes primarily from an implementation found
    * in the SoarX terrain rendering architecture developed by Andras Balogh 
    * and can be found at http://web.interware.hu/bandi/ranger.html.
    */
   class FixedPointNoise
   {
      public:
         
         ///3D noise.
         int operator()(int x, int y, int z);
         
         ///2D noise.
         int operator()(int x, int y);
         
         //1D noise.
         int operator()(int x);
          
      private:
      
         ///
         int Grad(int h, int x, int y, int z)
         {
            h &= 15;
            int u = (h & 8) ? y : x;
            int v = (h < 4) ? y : (((h & 14) == 12) ? x : z);
            return ((h & 2) ? -u : u) + ((h & 1) ? -v : v);
         }
         
         ///
         int Grad(int h, int x, int y)
         {
            h &= 15;
            int u = (h & 8) ? y : x;
            int v = (h < 4) ? y : (((h & 14) == 12) ? x : 0);
            return ((h & 2) ? -u : u) + ((h & 1) ? -v : v);
         }
         
         ///
         int Grad(int h, int x)
         {
            h &= 15;
            int u = (h & 8) ? 0 : x;
            int v = (h < 4) ? 0 : (((h & 14) == 12) ? x : 0);
            return ((h & 2) ? -u : u) + ((h & 1) ? -v : v);
         }
      
         ///Gets a permutation value at the given index.
         int Permutation(int i)
         {
            static int p[] = 
            { 
               151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
               142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
               203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,
               175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,
               230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,
               76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,
               173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,
               206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
               70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
               178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
               81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,
               121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,
               78,66,215,61,156,180,151
            };

            return p[i];
         }      
   }; 
   
}

#endif 
