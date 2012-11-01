#ifndef __SEAMLESS_NOISE_H__
#define __SEAMLESS_NOISE_H__

#include <osg/Vec3f>

#include "mathdefines.h"


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
* Bradley Anderegg
* @reference http://mrl.nyu.edu/~perlin/noise/
*/

//the majority of this code has been taken from
//http://mrl.nyu.edu/~perlin/noise/


namespace dtUtil
{

   /**
   * SeamlessNoise is a noise class that creates tileable noise
   *
   */
   class SeamlessNoise
   {

   public:

      SeamlessNoise(unsigned int seed = 1023058);
      ~SeamlessNoise();

      /**
      * Regenerates the random numbers using the given seed
      * @param seed new seed value
      */
      void Reseed(unsigned int seed);

      /**
      * @param vect_in a 3D Vector to hash the noise to, for 2D noise pass in a constant z value
      * @param repeat pass in the frequency or the desired resolution to tile the noise in
      *        if this param is not passed in the default value will be used
      * @return a float from -1 to 1 
      */
      float GetNoise(const osg::Vec3f& vect_in, int repeat = -1);


      /**
      * The SetRepeat function will allow a user to change the frequency 
      * the noise function will repeat
      */
      void SetRepeat(int pRepeat){mDefaultRepeat = pRepeat;}

   private:

      void BuildTable(unsigned int seed);
      float Fade(float t){ return t * t * t * (t * (t * 6 - 15) + 10); }
      float Grad(int hash, float x, float y, float z);

      int mDefaultRepeat;

   public:
   


   };


}//namespace dtUtil

#endif //__SEAMLESS_NOISE_H__
