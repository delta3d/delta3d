#ifndef DELTA_MATHDEFINES
#define DELTA_MATHDEFINES


//math defines

///\todo put this in dtUtil namespace!
///returns absolute value
template <typename num>
num ABS(num x)
{
    return (( x < 0) ? ((num)(-1.0) * x) : x);
}

///min and max defines
#ifndef MIN
#define MIN(a,b) (( (a) < (b)) ? (a) : (b))
#endif // MIN

#ifndef MAX
#define MAX(a,b) (( (a) > (b) ) ? (a) : (b))
#endif // MAX

///\todo put this in dtUtil namespace!
template <typename num, typename rangeNum>
num& CLAMP(num& x, const rangeNum low, const rangeNum high)
{
   if (x < (num)(low)) x = (num)(low);
   if (x > (num)(high)) x = (num)(high);
   
   return x;
}

///a random integer within a specified range
#define RAND_RANGE(min, max) ((min) + (rand() % (((max) - (min)) + 1)))

///a random float within range
#define RAND_FLOAT(min, max) ( (min) + (((rand() & 0x7FFF) / ((float)0x7FFF)) * ( (max) - (min)) )  )

/// Returns a random float between 0 - 1
#define RAND_PERCENT() ((rand() & 0x7FFF) / ((float)0x7FFF))


///\todo put this in dtUtil namespace!
template <typename Real>
Real Lerp(Real x, Real y, Real t)
{
   return x + t * (y - x);
}


///these are taken from
///http://developer.nvidia.com/object/fast_math_routines.html

#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000


// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
   int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
   r = *(float *)&_i;                                                       \
   r = r * (2.0f - (p) * r);                                                \
}

#define FP_EXP(e,p)                                                          \
{                                                                            \
   int _i;                                                                  \
   e = -1.44269504f * (float)0x00800000 * (p);                              \
   _i = (int)e + 0x3F800000;                                                \
   e = *(float *)&_i;                                                       \
}

//this floors the int
//__forceinline void FloatToInt(int *int_pointer, float f) 
//{
//      __asm  fld  f
//      __asm  mov  edx,int_pointer
//      __asm  FRNDINT
//      __asm  fistp dword ptr [edx];
//
//}

namespace dtUtil
{
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
