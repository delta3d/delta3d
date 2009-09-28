/*
 * Delta3D Open Source Game and Simulation Engine
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
 * Bradley Anderegg 01/17/2008
 */

#ifndef DELTA_SENSOR
#define DELTA_SENSOR

#include <osg/Referenced>
#include <dtUtil/typetraits.h>
#include <dtUtil/templateutility.h>

namespace dtAI
{
   /**
    * A very simple utility with templated update, compare, and report functionality.
    * It's purpose is to serve a generic method of generating alerts for ai agents
    */

   // This functor is used to perform the compare
   template <typename ReportFunctor, typename ReportData, typename ArgType1, typename ArgType2>
   struct CompareFunctor
   {
      bool operator()(ReportFunctor func, ReportData data, ArgType1 arg1, ArgType2 arg2)
      {
         return func(data, arg1, arg2);
      }
   };

   template <typename ReportFunctor, typename ReportData, typename ArgType1, typename ArgType2>
   struct CompareFunctor<ReportFunctor*, ReportData, ArgType1, ArgType2>
   {
      bool operator()(ReportFunctor* func, ReportData data, ArgType1 arg1, ArgType2 arg2)
      {
         return func->operator()(data, arg1, arg2);
      }
   };

   // a base class will allow us to hold a container of these
   template <typename ReportData>
   class SensorBase: public osg::Referenced
   {
   public:
      SensorBase(){}

      virtual ReportData Evaluate() = 0;

   protected:
      ~SensorBase(){}
   };

   template <typename Type1, typename Type2, typename EvaluateFunc1, typename EvaluateFunc2, typename CompareFunc, typename ReportFunc, typename ReportData>
   class Sensor: public SensorBase<ReportData>
   {
   public:
      Sensor(Type1 t1, Type2 t2, EvaluateFunc1 eval1, EvaluateFunc2 eval2, CompareFunc cmp, ReportFunc rpt)
         : mElement1(t1)
         , mElement2(t2)
         , mEval1(eval1)
         , mEval2(eval2)
         , mReport(rpt)
         , mCompare(cmp)
      {
      }

      void Set(Type1 t1, Type2 t2, EvaluateFunc1 eval1, EvaluateFunc2 eval2, CompareFunc cmp, ReportFunc rpt)
      {
         mElement1 = t1;
         mElement2 = t2;
         mEval1    = eval1;
         mEval2    = eval2;
         mCompare  = cmp;
         mReport   = rpt;
      }

      /**
       * This simple function is the basis of the Sensor
       */
      typename dtUtil::TypeTraits<ReportData>::param_type Evaluate()
      {
         typedef typename dtUtil::TypeTraits<Type1>::reference Traits1Ref;
         typedef typename dtUtil::TypeTraits<Type2>::reference Traits2Ref;
         typedef typename dtUtil::TypeTraits<ReportData>::reference ReportTraitsRef;

         dtUtil::EvaluateFunctor<EvaluateFunc1, Traits1Ref> eval1;
         eval1(mEval1, mElement1);

         dtUtil::EvaluateFunctor<EvaluateFunc2, Traits2Ref> eval2;
         eval2(mEval2, mElement2);

         CompareFunctor<CompareFunc, ReportTraitsRef, Traits1Ref, Traits2Ref> genericCompare;
         if (genericCompare(mCompare, mReportData, mElement1, mElement2))
         {
            dtUtil::EvaluateFunctor<ReportFunc, ReportTraitsRef> invokeReport;
            invokeReport(mReport, mReportData);
         }
         return mReportData;
      }

      /**
       * This function makes us play friendly with the generic functor interface
       */
      typename dtUtil::TypeTraits<ReportData>::param_type operator()()
      {
         return Evaluate();
      }

      /**
       * Allows sensor to work with SteeringBehavior error handling
       */
      typename dtUtil::TypeTraits<ReportData>::param_type operator()(typename dtUtil::TypeTraits<ReportData>::reference result)
      {
         return result = Evaluate();
      }

      typename dtUtil::TypeTraits<Type1>::return_type GetFirstElement()
      {
         return mElement1;
      }

      typename dtUtil::TypeTraits<Type2>::return_type GetSecondElement()
      {
         return mElement2;
      }

      typename dtUtil::TypeTraits<ReportData>::return_type GetReportData()
      {
         return mReportData;
      }

      typename dtUtil::TypeTraits<Type1>::const_return_type GetFirstElement() const
      {
         return mElement1;
      }

      typename dtUtil::TypeTraits<Type2>::const_return_type GetSecondElement() const
      {
         return mElement2;
      }

      typename dtUtil::TypeTraits<ReportData>::const_return_type GetReportData() const
      {
         return mReportData;
      }

   protected:
      /*virtual*/ ~Sensor(){}

      Type1 mElement1;
      Type2 mElement2;
      EvaluateFunc1 mEval1;
      EvaluateFunc2 mEval2;

      ReportFunc mReport;
      ReportData mReportData;

      CompareFunc mCompare;
   };

} // namespace dtAI

#endif // DELTA_SENSOR
