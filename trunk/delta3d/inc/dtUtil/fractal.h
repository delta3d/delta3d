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
class Fractal: public Noise
{

public:

   Real FBM(Vector vect_in, int octaves = 2, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f);

   Real Turbulence(Vector vect_in, int octaves = 1, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f);

   Real Marble(Vector vect_in, int octaves = 1, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f);

   Real IslandFractal(Vector vect_in, int octaves = 4, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f, Real oscarity = 2.37f);
   
   Real RigidMultiFractal(Vector vect_in, int octaves = 2, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f, Real offset = 0.5f, Real gain = 2.0f);

   Real HeteroFracal(Vector vect_in, int octaves = 2, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f, Real offset = 0.5f);

};



//inline


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::FBM(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity)
{
   Real total = 0.0f;
   Real amplitude = 1.0f;

   for(int i = 0; i < octaves; i++) 
   {
      total += GetNoise(vect_in * freq) * amplitude;
      freq *= lacunarity;
      amplitude *= persistance;		
   }

 
   return total;
}


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::Turbulence(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity)
{
   Real total = 0.0f;
   Real amplitude = 1.0f;

   for(int i = 0; i < octaves; i++) 
   {
      total += amplitude * abs(GetNoise(vect_in * freq));
      freq *= lacunarity;
      amplitude *= persistance;		
   }

   return total;
}


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::Marble(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity)
{
   return sin(vect_in[0] + Turbulence(vect_in, octaves, freq, persistance, lacunarity));
}




template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::RigidMultiFractal(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity, Real offset, Real gain)
{

   Real total = 0.0f;
   Real amplitude = 1.0f;
   Real signal = 0.0f;
   Real weight = 1.0;

   for(int i = 0; i < octaves; i++) 
   {
      signal = weight * (offset - abs(GetNoise(vect_in)));
      weight = signal * gain;

      total += signal * amplitude;

      vect_in *= freq;
      freq *= lacunarity;
      amplitude *= persistance;		
   }


   return total;

}

template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::HeteroFracal(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity, Real offset)
{

   Real total = 1.0f;
   Real amplitude = 1.0f;
   Real signal = 0.0f;
   Real weight = 1.0;

   for(int i = 0; i < octaves; i++) 
   {
      signal = amplitude * (GetNoise(vect_in) + offset);
      
      signal *= total;
      total += signal;

      vect_in *= freq;
      freq *= lacunarity;
      amplitude *= persistance;		
   }


   return (total - 2.0f);

}


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::IslandFractal(Vector vect_in, int octaves /* = 4 */, Real freq /* = 2.0f */, Real persistance /* = 0.5f */, Real lacunarity /* = 2.21731f */, Real oscarity /* = 2.37f */)
{
   Real total = 0.0f;
   Real pers = 1.0f;

   for(int i = 0; i < octaves; i++) 
   {
      total += GetNoise(vect_in * freq) * pers;

      freq *= lacunarity;
      pers *= persistance;
   }

   total *= (abs(total * persistance) + (total * total * oscarity));

   return abs(total);
}




}//dtCore

#endif//__FRACTAL_H__

