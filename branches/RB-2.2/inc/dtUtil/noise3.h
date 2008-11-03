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

#ifndef __NOISE_3_H__
#define __NOISE_3_H__

#include "dtUtil/mathdefines.h"

namespace dtUtil
{

   /**
   * An implementation of 3D Gradient Noise
   *
   */

template <class Real, class Vector>
class Noise3
{
public:
   Noise3(unsigned int seed = 1023058);
   ~Noise3();

   void Reseed(unsigned int seed);
   Real GetNoise(const Vector& vect_in);

private:

   void BuildTable();
   void BuildCoefs(const Vector& vect_in);
   int Fold(const Vector& vect_in);
   Real Interp(Real t);


   static const int TABLE_SIZE = 256;

   Vector   m_vCoef[8];
   int      m_iCoef[8];

   int      m_iPerm[TABLE_SIZE * 2];
   Vector   m_gTable[TABLE_SIZE];
};



//inline


template <class Real, class Vector>
Noise3<Real, Vector>::Noise3(unsigned int seed)
{
   Reseed(seed);
}

template <class Real, class Vector>
Noise3<Real, Vector>::~Noise3()
{


}

template <class Real, class Vector>
void Noise3<Real, Vector>::Reseed(unsigned int pSeed)
{
   srand(pSeed);
   BuildTable();
}

template <class Real, class Vector>
void Noise3<Real, Vector>::BuildTable()
{
   for (int i = 0; i < TABLE_SIZE; i++)
   {
      Real x = 1 - ( 2.0f * dtUtil::RandPercent() );
      Real y = 1 - ( 2.0f * dtUtil::RandPercent() );
      Real z = 1 - ( 2.0f * dtUtil::RandPercent() );

      if (((x * x) + (y * y) + (z * z)) < 1.0f)
      {
         m_gTable[i] = Vector(x,  y,  z);
         m_gTable[i].normalize();
      }
      else
      {
         --i;
      }
   }

   for (int j = 0; j < TABLE_SIZE; j++)
   {
      int num = dtUtil::RandRange(0, TABLE_SIZE - 1);
      m_gTable[j] = m_gTable[num];
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
void Noise3<Real, Vector>::BuildCoefs(const Vector& vect_in)
{
   Real iX, iY, iZ, iX1, iY1, iZ1;
   iX = floor(vect_in[0]);
   iY = floor(vect_in[1]);
   iZ = floor(vect_in[2]);

   iX1 = (iX + Real(1.0));
   iY1 = (iY + Real(1.0));
   iZ1 = (iZ + Real(1.0));


   m_vCoef[0] = Vector(iX, iY, iZ);
   m_vCoef[1] = Vector(iX1, iY, iZ);

   m_vCoef[2] = Vector(iX, iY1, iZ);
   m_vCoef[3] = Vector(iX1, iY1, iZ);

   m_vCoef[4] = Vector(iX, iY, iZ1);
   m_vCoef[5] = Vector(iX1, iY, iZ1);

   m_vCoef[6] = Vector(iX, iY1, iZ1);
   m_vCoef[7] = Vector(iX1, iY1, iZ1);

   for (int i = 0; i < 8; i++)
   {
      m_iCoef[i] = Fold(m_vCoef[i]);
   }
}


template <class Real, class Vector>
int Noise3<Real, Vector>::Fold(const Vector& vect_in)
{
   int x = int(vect_in[0]) & (TABLE_SIZE - 1);
   int y = int(vect_in[1]) & (TABLE_SIZE - 1);
   int z = int(vect_in[2]) & (TABLE_SIZE - 1);

   return m_iPerm[x + m_iPerm[ y + m_iPerm[z]]];
}

//Ken Perlin's new interpolation function
//n = 6t^5 - 15t^4 + 10t^3
template <class Real, class Vector>
Real Noise3<Real, Vector>::Interp(Real t)
{
   return Real((Real(6.0) * pow(t, Real(5.0))) - (Real(15.0) * pow(t, Real(4.0))) + (Real(10.0) * pow(t, Real(3.0))));
}


template <class Real, class Vector>
Real Noise3<Real, Vector>::GetNoise(const Vector& vect_in)
{
   BuildCoefs(vect_in);

   Real gradientValues[8];

   for (int i = 0; i < 8; i++)
   {
      gradientValues[i] = (vect_in - m_vCoef[i]) * m_gTable[m_iCoef[i]];
   }

   Real u = Interp(vect_in[0] - m_vCoef[0][0]);
   Real v = Interp(vect_in[1] - m_vCoef[0][1]);
   Real w = Interp(vect_in[2] - m_vCoef[0][2]);

   Real sumX1 = Lerp(gradientValues[0], gradientValues[1], u);
   Real sumX2 = Lerp(gradientValues[2], gradientValues[3], u);
   Real sumX3 = Lerp(gradientValues[4] ,gradientValues[5], u);
   Real sumX4 = Lerp(gradientValues[6], gradientValues[7], u);

   Real sumY1 = Lerp(sumX1, sumX2, v);
   Real sumY2 = Lerp(sumX3, sumX4, v);

   Real sumZ1 = Lerp(sumY1, sumY2, w);

   return sumZ1;
}



}//dtCore


#endif

