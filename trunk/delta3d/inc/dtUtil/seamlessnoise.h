#ifndef __SEAMLESS_NOISE_H__
#define __SEAMLESS_NOISE_H__

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
* @author Bradley Anderegg
*/


namespace dtUtil
{


   template <class Real, class Vector, int NumComponents, class Noise>
   class SeamlessNoise
   {

   public:
      
      SeamlessNoise(){}     
      ~SeamlessNoise(){}

      void Initialize(const Vector& pStart, const Vector& pDiff){ mStart = pStart; mDiff = pDiff;}
    
      Real GetNoise(const Vector& vect_in);
     

   public:

      //data members

      Vector mStart, mDiff;

      Noise mNoise;


   };


   template<class Real, class Vector, int NumComponents, class Noise>
   Real SeamlessNoise<Real, Vector, NumComponents, Noise>::GetNoise(const Vector& vect_in)
   {
      
      Vector vect = vect_in;
      
      for(int i = 0; i < NumComponents; ++i)
      {
         while(vect[i] > ((mStart[i] + mDiff[i]) - 1.0))
            vect[i] -= mDiff[i];
      }

      return mNoise.GetNoise(vect);
    
   }


}

#endif //__SEAMLESS_NOISE_H__

