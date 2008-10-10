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
 */

#ifndef __FRACTAL_H__
#define __FRACTAL_H__

namespace dtUtil
{

/**
 * Fractal: This class is made to complement the noise classes
 *           where as the noise class hashes vectors to floats between -1 to 1
 *           this class can be used to make summations of the noise
 *           to use this class I recommend including NoiseUtility.h and
 *           using the appropriate typedefs
 *
 * Definitions:
 *                Octaves: The number of successive steps to accumulate noise
 *                Frequency: The fractal dimension, or the number of noise interpolations per Real resolution
 *                Persistance: This defines the how the octaves will be weighted,
 *                                where the weight at each octave is the persistance ^ octave
 *                Lacunarity: The gap between successive steps, or the rate at which the frequency is changing
 *                Offset: The amount to raise the terrain from sea level
 *                Gain: The scale value for the current noise to multiply into the next octave, used to produce smooth valleys and jagged mountain tops
 *                Oscarity: This convoluted word I made up which appears in IslandFractal, it is used to scale the squared value of the noise function
 *                             higher values make larger differences between peaks and sea level
 *
 *
 *@sa http://www.texturingandmodeling.com/
 *Note: This book is awesome and is where most of this material came from
 *
 */

template <class Real, class Vector, class Noise>
class Fractal: public Noise
{

public:

   ///FBM: The standard Fractal Brownian Motion summation
   Real FBM(Vector vect_in, int octaves = 2, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f);

   ///Turbulence: this should be used for gases, clouds, lava, etc, generates an inconsistent bubbly pattern
   Real Turbulence(Vector vect_in, int octaves = 1, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f);

   ///A basic function that makes a pattern of lines in the x direction that could be interpreted as a marble look
   ///try to make the output of this swirl, for added realism
   Real Marble(Vector vect_in, int octaves = 1, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f);

   ///This one I made myself which basically just scales the output exponentially to make an island look
   Real IslandFractal(Vector vect_in, int octaves = 4, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f, Real oscarity = 2.37f);

   //this is a n F. Kenton Musgrave function taken and modified from Texturing and Modeling: A Procedural Approach
   //it attempts to simulate smooth valleys and sea floors with jagged peaks
   Real RigidMultiFractal(Vector vect_in, int octaves = 2, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f, Real offset = 0.5f, Real gain = 2.0f);

   //another one of F. Kenton Musgrave's from Texturing and Modeling: A Procedural Approach
   //this is like the one above but the smoother areas are only at the lower levels and it doesnt specifically create ridges
   Real HeteroFractal(Vector vect_in, int octaves = 2, Real freq = 1.0f, Real persistance = 0.5f, Real lacunarity = 2.0f, Real offset = 0.5f);

};



//inline


template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::FBM(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity)
{
   Real total = 0.0f;
   Real amplitude = 1.0f;

   for (int i = 0; i < octaves; i++)
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

   for (int i = 0; i < octaves; i++)
   {
      total += amplitude * std::abs(GetNoise(vect_in * freq));
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

   for (int i = 0; i < octaves; i++)
   {
      signal = weight * (offset - std::abs(GetNoise(vect_in)));
      weight = signal * gain;

      total += signal * amplitude;

      vect_in *= freq;
      freq *= lacunarity;
      amplitude *= persistance;
   }


   return total;

}

template <class Real, class Vector, class Noise>
Real Fractal<Real, Vector, Noise>::HeteroFractal(Vector vect_in, int octaves, Real freq, Real persistance, Real lacunarity, Real offset)
{

   Real total = 1.0f;
   Real amplitude = 1.0f;
   Real signal = 0.0f;

   for (int i = 0; i < octaves; i++)
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

   for (int i = 0; i < octaves; i++)
   {
      total += GetNoise(vect_in * freq) * pers;

      freq *= lacunarity;
      pers *= persistance;
   }

   total *= (std::abs(total * persistance) + (total * total * oscarity));

   return std::abs(total);
}




}//dtCore

#endif//__FRACTAL_H__

