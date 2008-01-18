/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2006 MOVES Institute
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

namespace dtAI
{
   /**
    *	A very simple utility with templated update, compare, and report functionality.
    * It's purpose is to serve a generic method of generating alerts for ai agents
    */


   //a base class will allow us to hold a container of these
   template <typename ReportData>
   class SensorBase: public osg::Referenced
   {
   public:
      SensorBase(){}

      virtual ReportData Evaluate() = 0;

   protected:
      ~SensorBase(){}

   };


   template <typename FunctorType, typename ArgType>
   struct EvaluateFunctor
   {
   private:

      //gotta love partial template specialization
      template <typename U, typename T>
      struct InnerEvaluate
      {
         static void Eval(U u, T t)
         {
            u(t);
         }     
      };

      template <typename U, typename T>
      struct InnerEvaluate<U*, T>
      {
         static void Eval(U* u, T t)
         {
            u->operator()(t);
         }         
      };


   public:
      static void Evaluate(FunctorType func, ArgType arg)
      {
         InnerEvaluate<FunctorType, ArgType>::Eval(func, arg);
      }
   };


   template <typename Type1, typename Type2, typename EvaluateType1, typename EvaluateType2, typename CompareType, typename ReportType, typename ReportData>   
   class Sensor: public SensorBase<ReportData>
   {
      public:
         Sensor(Type1 t1, Type2 t2, EvaluateType1 eval1, EvaluateType2 eval2, CompareType cmp, ReportType rpt)
         {
            Set(t1, t2, eval1, eval2, cmp, rpt);
         }

         void Set(Type1 t1, Type2 t2, EvaluateType1 eval1, EvaluateType2 eval2, CompareType cmp, ReportType rpt)
         {
            mElement1 = t1;
            mElement2 = t2;
            mEval1 = eval1;
            mEval2 = eval2;
            mCompare = cmp;
            mReport = rpt;
         }

         /**
          *	This simple function is the basis of the Sensor
          */
         ReportData Evaluate()
         {
            mEval1(mElement1);
            mEval2(mElement2);
            if(mCompare(mReportData, mElement1, mElement2))
            {               
               EvaluateFunctor<ReportType, ReportData>::Evaluate(mReport, mReportData);
            }
            return mReportData;
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
         EvaluateType1 mEval1;
         EvaluateType2 mEval2;

         ReportType mReport;
         ReportData mReportData;

         CompareType mCompare;

   };



}//namespace dtAI

#endif // DELTA_SENSOR
