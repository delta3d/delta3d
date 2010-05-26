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
 */
#ifndef DELTA_MATHDEFINES
#define DELTA_MATHDEFINES

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <osg/Math>
#include <dtUtil/mswinmacros.h>


#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

namespace dtUtil
{

   template <typename Real>
   Real Min(Real a, Real b)
   {
      return (a < b) ? a : b;
   }

   template <typename Real>
   Real Max(Real a, Real b)
   {
      return (a > b) ? a : b;
   }

   inline int RandRange(int from, int to)
   {
      return ((from) + (rand() % (((to) - (from)) + 1)));
   }

   inline float RandFloat(float min, float max)
   {
      return ( (min) + (((rand() & RAND_MAX) / ((float)RAND_MAX)) * ( (max) - (min)) )  );
   }

   inline float RandPercent()
   {
      return ((rand() & RAND_MAX) / ((float)RAND_MAX));
   }

   template <typename Real>
   inline Real Abs(Real x)
   {
#ifdef DELTA_WIN32
      return (( x < 0) ? ((Real)(-1.0) * x) : x);
#else
      return std::abs(x);
#endif
   }

   template <typename Real>
   inline void Clamp(Real& x, const Real low, const Real high)
   {
      ClampMin( x, low );
      ClampMax( x, high );
   }

   template <typename Real>
   inline void ClampMax(Real& x, const Real high)
   {
      if (x > high) { x = high; }
   }

   template <typename Real>
   inline void ClampMin(Real& x, const Real low)
   {
      if (x < low) { x = low; }
   }

   /** Apply a linear interpolation between the two supplied numbers using a
     * third percentage value.
     *
     * @param x : specifies the left bound of the range.
     * @param y : specifies the right bound of the range.
     * @param t: the normalized value with respect to the specified range to be interpolated.
     * @return the interpolated value for the coefficient of the range.
     */
   template <typename Real>
   inline Real Lerp(Real x, Real y, Real t)
   {
      return x + t * (y - x);
   }

   template <typename T>
   bool IsFinite(const T value)
   {
     #ifdef DELTA_WIN32
         return _finite(value) ? true : false;
      #else
         return std::isfinite(value) ? true : false;
      #endif
   }

   template <typename VecType>
   bool IsFiniteVec(const VecType value)
   {
      for (size_t i = 0; i < VecType::num_components; ++i)
      {
#ifdef DELTA_WIN32
         if (!_finite(value[i]))
         {
            return false;
         }
#else
         if (!std::isfinite(value[i]))
         {
            return false;
         }
#endif
      }
      return true;
   }

   template <typename T>
   bool IsNAN(const T value)
   {
      return osg::isNaN(value);
   }

   /// Normalizes a value within a specified space range.
   /// Usage:  To find the normalized value for a range:
   /// float nX = CalculateNormal( valueX , xMin , xMax );
   /// @param sX the value with respect to the specified range to be normalized.
   /// @param sMin specifies the left bound of the range.
   /// @param sMax specifies the right bound of the range.
   /// @return the normalized coefficient for the input to the range.
   template<typename T>
   T CalculateNormal(T sX, T sMin, T sMax)
   {
      T delta( sMax - sMin );
      return( (sX-sMin) / delta );
   }


   /// Calculates the corresponding value for a mirrored space.
   /// Given defined ranges for X space and Y space, and a known value in X space,
   /// where X space and Y space are linearly related, find the corresponding value in Y space.
   /// Usage: float y = MapRangeValue(x,xMin,xMax,yMin,yMax);
   /// @param sX the value with respect to the X range to be transformed into the Y range.
   /// @param xMin specifies the left bound of the X range.
   /// @param xMax specifies the right bound of the X range.
   /// @param yMin specifies the left bound of the Y range.
   /// @param yMax specifies the right bound of the Y range.
   template<typename T>
   T MapRangeValue(T sX, T xMin, T xMax, T yMin, T yMax)
   {
      return( Lerp( yMin, yMax, CalculateNormal(sX,xMin,xMax)) );
   }

   /**
    * This does a relative comparison of floats.  This is a SAFE comparison
    * that doesn't use cheesy 0.0001 type epsilon values.  The epsilon is scaled
    * based on the precision of the numbers passed in.  This was taken from
    * Christer Ericson's GDC '07 presentation:
    * http://realtimecollisiondetection.net/pubs/GDC06_Ericson_Physics_Tutorial_Numerical_Robustness.ppt
    * Note - This should be used when comparing very large and/or very small numbers.
    * @param float1 The first float
    * @param float2 The second float
    * @return True if the values are equal within the relative precision of their values.
    */
   inline bool Equivalent(float float1, float float2, float baseEpsilon = FLT_EPSILON)
   {
      return (Abs(float1 - float2) <= baseEpsilon * Max(1.0f, Max(float1, float2)));
   }

   /**
   * This does a relative comparison of doubles.  This is a SAFE comparison
   * that doesn't use cheesy 0.0001 type epsilon values.  The epsilon is scaled
   * based on the precision of the numbers passed in.  This was taken from
   * Christer Ericson's GDC '07 presentation:
   * http://realtimecollisiondetection.net/pubs/GDC06_Ericson_Physics_Tutorial_Numerical_Robustness.ppt
   * Note - This should be used when comparing very large and/or very small numbers.
   * @param double1 The first value
   * @param double2 The second value
   * @return True if the values are equal within the relative precision of their values.
   */
   inline bool Equivalent(double double1, double double2, double baseEpsilon = DBL_EPSILON)
   {
      return (Abs(double1 - double2) <= baseEpsilon * Max(1.0, Max(double1, double2)));
   }

   /**
    * Does an epsilon equals on an any osg::Vec# or array
    * @param lhs The first vector.
    * @param rhs The second vector.
    * @param size The size or the vec.
    * @param epsilon the epsilon to use in the compare.
    */
   template <typename TVec, typename Real>
   inline bool Equivalent(const TVec& lhs, const TVec& rhs, size_t size, Real epsilon)
   {
      for (size_t i = 0; i < size; i++)
      {
         if (!osg::equivalent(lhs[i], rhs[i], epsilon))
            return false;
      }
      return true;
   }

   /**
    * Does an epsilon equals on an any osg::Vec#
    * @param lhs The first vector.
    * @param rhs The second vector.
    * @param epsilon the epsilon to use in the compare.
    */
   template <typename TVec, typename Real>
   inline bool Equivalent(const TVec& lhs, const TVec& rhs, Real epsilon)
   {
      return Equivalent(lhs, rhs, TVec::num_components, epsilon);
   }

   /**
    * Does an epsilon equals on an any osg::Vec#, but auto calculates the epsilon per comparison
    * @param lhs The first vector.
    * @param rhs The second vector.
    */
   template <typename TVec>
   inline bool Equivalent(const TVec& lhs, const TVec& rhs)
   {
      for (size_t i = 0; i < TVec::num_components; i++)
      {
         if (!Equivalent(lhs[i], rhs[i])) {return false;}
      }

      return true;
   }


   template <typename Real>
   inline bool WithinRange(Real lhs, Real rhs, Real epsilon)
   {
      return (Abs(lhs - rhs) <= epsilon);
   }
}


#endif //MATHLIB_H
