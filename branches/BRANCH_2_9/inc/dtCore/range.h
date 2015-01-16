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
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<typename T_Num>
   struct ValueHelperT
   {
      template<typename T_Result>
      static T_Result ConverTo(T_Num n)
      {
         return T_Result(n);
      }
      
      template<>
      static osg::Vec2 ConverTo<osg::Vec2>(T_Num n)
      {
         return osg::Vec2(n,n);
      }

      template<>
      static osg::Vec3 ConverTo<osg::Vec3>(T_Num n)
      {
         return osg::Vec3(n,n,n);
      }

      template<>
      static osg::Vec4 ConverTo<osg::Vec4>(T_Num n)
      {
         return osg::Vec4(n,n,n,n);
      }

      template<typename T_Num>
      static T_Num GetMin(T_Num a, T_Num b)
      {
         return a < b ? a : b;
      }

      template<>
      static osg::Vec2 GetMin(osg::Vec2 a, osg::Vec2 b)
      {
         osg::Vec2 result;
         result.x() = GetMin(a.x(), b.x());
         result.y() = GetMin(a.y(), b.y());
         return result;
      }

      template<>
      static osg::Vec3 GetMin(osg::Vec3 a, osg::Vec3 b)
      {
         osg::Vec3 result;
         result.x() = GetMin(a.x(), b.x());
         result.y() = GetMin(a.y(), b.y());
         result.z() = GetMin(a.z(), b.z());
         return result;
      }

      template<>
      static osg::Vec4 GetMin(osg::Vec4 a, osg::Vec4 b)
      {
         osg::Vec4 result;
         result.x() = GetMin(a.x(), b.x());
         result.y() = GetMin(a.y(), b.y());
         result.z() = GetMin(a.z(), b.z());
         result.w() = GetMin(a.w(), b.w());
         return result;
      }

      template<typename T_Num>
      static T_Num GetMax(T_Num a, T_Num b)
      {
         return a > b ? a : b;
      }

      template<>
      static osg::Vec2 GetMax(osg::Vec2 a, osg::Vec2 b)
      {
         osg::Vec2 result;
         result.x() = GetMax(a.x(), b.x());
         result.y() = GetMax(a.y(), b.y());
         return result;
      }

      template<>
      static osg::Vec3 GetMax(osg::Vec3 a, osg::Vec3 b)
      {
         osg::Vec3 result;
         result.x() = GetMax(a.x(), b.x());
         result.y() = GetMax(a.y(), b.y());
         result.z() = GetMax(a.z(), b.z());
         return result;
      }

      template<>
      static osg::Vec4 GetMax(osg::Vec4 a, osg::Vec4 b)
      {
         osg::Vec4 result;
         result.x() = GetMax(a.x(), b.x());
         result.y() = GetMax(a.y(), b.y());
         result.z() = GetMax(a.z(), b.z());
         result.w() = GetMax(a.w(), b.w());
         return result;
      }

   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<typename T_Num>
   class RangeT
   {
   public:
      typedef T_Num ValueType;
      typedef ValueHelperT<ValueType> ValueHelper;

      static const ValueType ZERO;
      static const ValueType ONE;

      RangeT()
      {
         Reset();
      }

      RangeT(ValueType maxNum)
      {
         Reset();
         
         mMax = maxNum;
      }

      RangeT(ValueType minNum, ValueType maxNum)
      {
         mMin = minNum;
         mMax = maxNum;
         mValue = minNum;
      }

      RangeT(ValueType minNum, ValueType maxNum, ValueType value)
      {
         mMin = minNum;
         mMax = maxNum;
         mValue = value;

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
            ClampValue();
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
            ClampValue();
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
         ValueType result = ValueHelper::GetMax(val, mMin);
         result = ValueHelper::GetMin(result, mMax);

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
            result = ValueHelper::GetMax(result, ZERO);
            result = ValueHelper::GetMin(result, ONE);
         }

         return result;
      }

      ValueType GetRatio() const
      {
         return GetRatio(mValue);
      }

      ValueType GetValueAtRatio(float ratio) const
      {
         return (GetSpan() * ratio) + mMin;
      }

      void Reset()
      {
         mMin = ZERO;
         mMax = ZERO;
         mValue = ZERO;
      }

   protected:

      ValueType mMin;
      ValueType mMax;
      ValueType mValue;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   template<typename T_Num>
   const T_Num RangeT<T_Num>::ZERO = ValueHelperT<int>::ConverTo<T_Num>(0);
   template<typename T_Num>
   const T_Num RangeT<T_Num>::ONE = ValueHelperT<int>::ConverTo<T_Num>(1);



   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef RangeT<float> Range;
   typedef RangeT<double> RangeD;
   typedef RangeT<int> RangeInt;
   typedef RangeT<osg::Vec2> RangeVec2;
   typedef RangeT<osg::Vec3> RangeVec3;
   typedef RangeT<osg::Vec4> RangeVec4;
}

#endif
