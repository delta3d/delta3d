#ifndef __FRACTAL_H__
#define __FRACTAL_H__

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


template <class Real, class Vector, class Noise>
class Fractal
{

public:


   /** A general turbulance function 
   *
   *
   *@param noiseValueIn : pass in a computed noise value
   *@param exponent : A number from 0-255 representing desired density
   *@param sharpness : A number from 0-1 controlling the sharpness of the noise 
   */
   Real ExponentialFunc(Real noiseValueIn, Real exponent, Real sharpness);


   Real FBM(Vector vect_in, int octaves = 2, Real freq = 0.5f, Real persistance = 2.0f, Real lacunarity = 0.5f);
   Real IslandFractal(Vector vect_in, int octaves = 4, Real freq = 0.25f, Real persistance = 0.65f, Real lacunarity = 2.21731f, Real oscarity = 2.37f);
   

private:
   Vector VecNoise(const Vector point, Real offset);

public:

   //data members
   Noise mNoise;

};



//inline


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::ExponentialFunc(Real noiseValueIn, Real exponent, Real sharpness)
{
   return Real(255.0) - ( pow(sharpness, exponent) * Real(255.0));
}


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::FBM(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity)
{
   Real total = 0.0f;
   Real amplitude = 1.0f;

   for(int i = 0; i < octaves; i++) 
   {
      total += mNoise.GetNoise(vect_in) * amplitude;
      vect_in *= freq;
      freq *= lacunarity;
      amplitude *= persistance;		
   }

 
   return total;
}

template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::IslandFractal(Vector vect_in, int octaves /* = 4 */, Real freq /* = 2.0f */, Real persistance /* = 0.5f */, Real lacunarity /* = 2.21731f */, Real oscarity /* = 2.37f */)
{
   Real total = 0.0f;
   Real pers = 1.0f;

   for(int i = 0; i < octaves; i++) 
   {
      total += mNoise.GetNoise(vect_in * freq) * pers;

      freq *= lacunarity;
      pers *= persistance;
   }

   total *= (abs(total * persistance) + (total * total * oscarity));

   return abs(total);
}




}//dtCore

#endif//__FRACTAL_H__

