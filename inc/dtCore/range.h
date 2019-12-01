/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_RANGE_H
#define DELTA_RANGE_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
/*#include <osg/Referenced>



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<typename T_Num>
   class RangeT
   {
   public:
      typedef T_Num ValueType;

      static const ValueType ZERO;
      static const ValueType ONE;

      RangeT()
         : mMin(ZERO)
         , mMax(ZERO)
         , mValue(ZERO)
      {}

      RangeT(ValueType maxNum)
         : mMin(ZERO)
         , mMax(maxNum)
         , mValue(ZERO)
      {}

      RangeT(ValueType minNum, ValueType maxNum)
         : mMin(minNum)
         , mMax(maxNum)
         , mValue(minNum)
      {}

      RangeT(ValueType minNum, ValueType maxNum, ValueType value)
         : mMin(minNum)
         , mMax(maxNum)
         , mValue(value)
      {
         ClampValue();
      }

      RangeT(const RangeT& range)
         : mMin(range.mMin)
         , mMax(range.mMax)
         , mValue(range.mValue)
      {}

      virtual ~RangeT()
      {}

      void SetValue(ValueType value)
      {
         mValue = ClampValue(value);
      }

      ValueType GetValue() const
      {
         return mValue;
      }

      void SetMin(ValueType value)
      {
         mMin = value;

         if (mMin > mValue)
         {
            SetValue(mMin);
         }
      }

      ValueType GetMin() const
      {
         return mMin;
      }

      void SetMax(ValueType value)
      {
         mMax = value;

         if (mMax < mValue)
         {
            SetValue(mMax);
         }
      }

      ValueType GetMax() const
      {
         return mMax;
      }

      ValueType GetSpan() const
      {
         return mMax - mMin;
      }

      ValueType ClampValue(ValueType val) const
      {
         ValueType result = val;

         if (val < mMin)
         {
            result = mMin;
         }

         if (val > mMax)
         {
            result = mMax;
         }

         return result;
      }

      ValueType ClampValue()
      {
         mValue = ClampValue(mValue);
         return mValue;
      }

      ValueType GetRatio(ValueType val) const
      {
         ValueType result = ZERO;

         ValueType span = mMax - mMin;
         if (span != ZERO)
         {
            result = (val - mMin) / span;

            // Clamp the ratio.
            if (result < ZERO)
            {
               result = ZERO;
            }
            else if (result > ONE)
            {
               result = ONE;
            }
         }

         return result;
      }

      ValueType GetRatio() const
      {
         return GetRatio(mValue);
      }

   protected:

      ValueType mMin;
      ValueType mMax;
      ValueType mValue;
   };



   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef RangeT<float> Range;
   typedef RangeT<double> RangeD;
   typedef RangeT<int> RangeInt;
}*/

#endif
