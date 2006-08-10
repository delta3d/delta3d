#ifndef DELTA_MATHDEFINES
#define DELTA_MATHDEFINES

#include <cstdlib>

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
   Real Abs(Real x)
   {
      return (( x < 0) ? ((Real)(-1.0) * x) : x);
   }

   template <typename Real>
   void Clamp(Real& x, const Real low, const Real high)
   {
      if (x < low) x = low;
      if (x > high) x = high;
   }

   template <typename Real>
   Real Lerp(Real x, Real y, Real t)
   {
      return x + t * (y - x);
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

   /// Applies the linear transformation of a coefficient for a given range.
   /// Usage:  To map a value in one space to a corresponding value in another space:
   /// float nX = CalculateNormal( valueX , xMin , xMax );
   /// float valueY = CalculateValueForRange(nX,minY,maxY);
   /// @param coefficient the normalized value with respect to the specified range to be transformed.
   /// @param sMin specifies the left bound of the range.
   /// @param sMax specifies the right bound of the range.
   /// @return the mapped value for the coefficient of the range.
   template<typename T>
   T CalculateValueForRange(T coefficient, T sMin, T sMax)
   {
      return( coefficient*(sMax-sMin) + sMin);
   }

   /// Caclulates the corresponding value for a mirrored space.
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
      return( CalculateValueForRange( CalculateNormal(sX,xMin,xMax), yMin, yMax ) );
   }
}


#endif //MATHLIB_H
