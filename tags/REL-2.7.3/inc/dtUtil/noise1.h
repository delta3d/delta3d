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

#ifndef __NOISE_1_H__
#define __NOISE_1_H__

#include "dtUtil/mathdefines.h"

namespace dtUtil
{

   /**
   * An implementation of 1D Gradient Noise
   *
   */

template <class Real, class Vector>
class Noise1
{
public:
   Noise1(unsigned int seed = 1023058);
   ~Noise1();

   void Reseed(unsigned int seed);
   Real GetNoise(const Vector vect_in);

private:

   void BuildTable();
   void BuildCoefs(const Vector vect_in);
   int Fold(const Vector vect_in);
   Real Interp(Real t);


   static const int TABLE_SIZE = 256;

   Vector         m_vCoef[2];
   int            m_iCoef[2];

   int            m_iPerm[TABLE_SIZE * 2];
   Vector         m_gTable[TABLE_SIZE];

   Vector start, diff;

};






template <class Real, class Vector>
Noise1<Real, Vector>::Noise1(unsigned int seed)
{
   Reseed(seed);
   start = 25.0f;
   diff = 5.0f;
}

template <class Real, class Vector>
Noise1<Real, Vector>::~Noise1()
{


}

template <class Real, class Vector>
void Noise1<Real, Vector>::Reseed(unsigned int pSeed)
{
   srand(pSeed);
   BuildTable();
}

template <class Real, class Vector>
void Noise1<Real, Vector>::BuildTable()
{
   for (int i = 0; i < TABLE_SIZE; i++)
   {
      m_gTable[i] = 1.0f - ( 2.0f * dtUtil::RandPercent() );
   }


   //create a table of random permuations
   for (int j = 0; j < TABLE_SIZE; ++j)
   {
      m_iPerm[j] = j;
   }

   for (int j = 0; j < TABLE_SIZE; ++j)
   {
      int r = dtUtil::RandRange(0, TABLE_SIZE - 1);
      int temp = m_iPerm[j];
      m_iPerm[j] = m_iPerm[r];
      m_iPerm[r] = temp;
   }

   //duplicate this table for speed
   for (int k = TABLE_SIZE; k < TABLE_SIZE * 2; ++k)
   {
      m_iPerm[k] = m_iPerm[k - TABLE_SIZE];
   }
}


template <class Real, class Vector>
void Noise1<Real, Vector>::BuildCoefs(const Vector vect_in)
{
   int iX = int(floor(vect_in));
   int iX1 = (iX + 1);

   m_vCoef[0] = Real(iX);
   m_vCoef[1] = Real(iX1);

   for (int i = 0; i < 2; ++i)
   {
      m_iCoef[i] = Fold(m_vCoef[i]);
   }

}


template <class Real, class Vector>
int Noise1<Real, Vector>::Fold(const Vector vect_in)
{
   int x = int(vect_in) & (TABLE_SIZE - 1);
   return m_iPerm[x];
}

//Ken Perlin's new interpolation function
//n = 6t^5 - 15t^4 + 10t^3
template <class Real, class Vector>
Real Noise1<Real, Vector>::Interp(Real t)
{
   return Real((Real(6.0) * pow(t, Real(5.0))) - (Real(15.0) * pow(t, Real(4.0))) + (Real(10.0) * pow(t, Real(3.0))));
}


template <class Real, class Vector>
Real Noise1<Real, Vector>::GetNoise(const Vector vect_in)
{

   BuildCoefs(vect_in);

   Real gradientValues[2];

   for (int i = 0; i < 2; i++)
   {
      gradientValues[i] = (vect_in - m_vCoef[i]) * m_gTable[m_iCoef[i]];
   }

   Real u = Interp(vect_in - m_vCoef[0]);
   Real sumX1 = Lerp(gradientValues[0], gradientValues[1], u);

   return Real(sumX1);
}






}//dtCore


#endif

