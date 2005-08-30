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
      enum NoiseDimension{NOISE_1 = 0, NOISE_2, NOISE_3};

   public:
      
      SeamlessNoise(){}
      ~SeamlessNoise(){}

      void Initialize(const Vector& pResolution, const Vector& pFrom, const Vector& pTo){ mResolution = pResolution; mFrom = pFrom; mTo = pTo; mExtent = mTo - mFrom;}
    
      Real GetNoise(const Vector& vect_in);
      
   private:
      Real Interp(Real x, Real y, Real t){return x + t * (y - x);}


   public:

      //data members

      Vector mFrom, mTo;

      Vector mResolution, mExtent;

      Noise mNoise;


   };


   template<class Real, class Vector, int NumComponents, class Noise>
   Real SeamlessNoise<Real, Vector, NumComponents, Noise>::GetNoise(const Vector& vect_in)
   {
      Vector interpFrom, interpTo;
      Real finalValue;

      /*int numInterpolations = int(powf(2.0f, NumComponents));
      
      Vector blendValues;
      Vector values[numInterpolations];*/


    /* switch (mDimension)
     {
       case NOISE_1:
          {
             Real swValue, seValue;
             swValue = mNoise.GetNoise(vect_in);
             seValue = mNoise.GetNoise(vect_in + mExtent);
             Real xBlend = 1.0 - ((vect_in - mFrom) / mExtent);
             finalValue = Interp(swValue, seValue, xBlend);
          }
          break;

       case NOISE_2:
        {
          Vector values[4];
          values[0] = mNoise.GetNoise(vect_in);
          values[1] = mNoise.GetNoise(Vector(vect_in[0] + mExtent[0], vect_in[1]));
          values[2] = mNoise.GetNoise(Vector(vect_in[0], vect_in[1] + mExtent[1]));
          values[3] = mNoise.GetNoise(Vector(vect_in[0] + mExtent[0], vect_in[1] + mExtent[1]));
          Real xBlend = 1.0 - ((vect_in[0] - mFrom[0]) / mExtent[0]);
          Real zBlend = 1.0 - ((vect_in[1] - mFrom[1]) / mExtent[1]);
          Real z0 = Interp(values[0], values[1], xBlend);
          Real z1 = Interp(values[2], values[3], xBlend);
          finalValue = (float)Interp(z0, z1, zBlend);

          break;
        }
      case NOISE_3
      {*/
       
         Real values[8];

         values[0] = mNoise.GetNoise(Vector(vect_in[0], vect_in[1], vect_in[2]));
         values[1] = mNoise.GetNoise(Vector(vect_in[0] + mExtent[0], vect_in[1], vect_in[2]));

         values[2] = mNoise.GetNoise(Vector(vect_in[0], vect_in[1] + mExtent[1], vect_in[2]));
         values[3] = mNoise.GetNoise(Vector(vect_in[0] + mExtent[0], vect_in[1] + mExtent[1], vect_in[2]));

         values[4] = mNoise.GetNoise(Vector(vect_in[0], vect_in[1], vect_in[2] + mExtent[2]));
         values[5] = mNoise.GetNoise(Vector(vect_in[0] + mExtent[0], vect_in[1], vect_in[2] + mExtent[2]));

         values[6] = mNoise.GetNoise(Vector(vect_in[0], vect_in[1] + mExtent[1], vect_in[2] + mExtent[2]));
         values[7] = mNoise.GetNoise(Vector(vect_in[0] + mExtent[0], vect_in[1] + mExtent[1], vect_in[2] + mExtent[2]));

         Real xBlend = 1.0 - ((vect_in[0] - mFrom[0]) / mExtent[0]);
         Real yBlend = 1.0 - ((vect_in[1] - mFrom[1]) / mExtent[1]);
         Real zBlend = 1.0 - ((vect_in[2] - mFrom[2]) / mExtent[2]);

         Real sumX1 = Interp(values[0], values[1], xBlend);
         Real sumX2 = Interp(values[2], values[3], xBlend);
         Real sumX3 = Interp(values[4] ,values[5], xBlend);
         Real sumX4 = Interp(values[6], values[7], xBlend);

         Real sumY1 = Interp(sumX1, sumX2, yBlend);
         Real sumY2 = Interp(sumX3, sumX4, yBlend);

         finalValue = Interp(sumY1, sumY2, zBlend);

         /*break;
      }

       default:
            
          return 0.0;
         
          break;
     }

     int numInterpolations = */

     return finalValue;
    
   }


}

#endif //__SEAMLESS_NOISE_H__