/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright 2008, Alion Science and Technology
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
 * David Guthrie 02/04/2008
 */

#ifndef DELTA_DISTANCE_SENSOR
#define DELTA_DISTANCE_SENSOR

#include <cmath>

#include <osg/Vec3>
#include <osg/Matrix>
#include <dtAI/sensor.h>

#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <osg/MatrixTransform>

#include <dtUtil/typetraits.h>

namespace dtAI
{

   template <typename ReportData = float, typename VecType = osg::Vec3>
   class CompareDistanceFunc
   {
   public:
      CompareDistanceFunc(ReportData minDistanceToReport)
         : mCallbackDistance2(minDistanceToReport* minDistanceToReport)
      {}

      bool operator()(ReportData& distance2, const VecType& first, const VecType& second)
      {
         distance2 = (second - first).length2();
         if (distance2 <= mCallbackDistance2)
         {
            distance2 = std::sqrt(distance2);
            return true;
         }
         return false;
      }

      void SetCallbackDistance(ReportData newCallbackDistance)
      {
         mCallbackDistance2 = newCallbackDistance * newCallbackDistance;
      }

      typename dtUtil::TypeTraits<ReportData>::return_type GetCallbackDistance() const
      {
         return std::sqrt(mCallbackDistance2);
      }

      typename dtUtil::TypeTraits<ReportData>::return_type GetCallbackDistanceSquared() const
      {
         return mCallbackDistance2;
      }

   private:
      ReportData mCallbackDistance2;
   };

   template <typename EvaluateFunc1, typename EvaluateFunc2,
      typename ReportFunc, typename ReportData = float, typename VecType = osg::Vec3>
   class DistanceSensor : public Sensor<VecType, VecType, EvaluateFunc1, EvaluateFunc2,
         CompareDistanceFunc<ReportData, VecType>, ReportFunc, ReportData>
   {
   public:
      typedef Sensor<VecType, VecType, EvaluateFunc1, EvaluateFunc2,
         CompareDistanceFunc<ReportData, VecType>, ReportFunc, ReportData> BaseClass;

      DistanceSensor(EvaluateFunc1 eval1, EvaluateFunc2 eval2, ReportData callbackDistance, ReportFunc rpt)
         : BaseClass(VecType(), VecType(), eval1, eval2,
                     CompareDistanceFunc<ReportData, VecType>(callbackDistance) , rpt)
      {
      }

      virtual ~DistanceSensor() {}

      typename dtUtil::TypeTraits<ReportData>::return_type GetDistance()
      {
         return std::sqrt(BaseClass::GetReportData());
      }

      typename dtUtil::TypeTraits<ReportData>::return_type GetDistanceSquared()
      {
         return BaseClass::GetReportData();
      }

      void SetCallbackDistance(typename dtUtil::TypeTraits<ReportData>::const_reference newValue)
      {
         return BaseClass::mCompare.SetCallbackDistance(newValue);
      }

      typename dtUtil::TypeTraits<ReportData>::return_type GetCallbackDistance()
      {
         return BaseClass::mCompare.GetCallbackDistance();
      }

      typename dtUtil::TypeTraits<ReportData>::return_type GetCallbackDistanceSquared()
      {
         return BaseClass::mCompare.GetCallbackDistanceSquared();
      }

   private:
   };

   template <typename VecType = osg::Vec3>
   class EvaluateTransformablePosition
   {
   public:
      EvaluateTransformablePosition(const dtCore::Transformable& transformable,
               dtCore::Transformable::CoordSysEnum frameOfRef = dtCore::Transformable::ABS_CS):
                  mTransformable(&transformable), mFrameOfRef(frameOfRef)
      {}

      void operator()(VecType& vec)
      {
         dtCore::Transform xform;
         mTransformable->GetTransform(xform, mFrameOfRef);
         xform.GetTranslation(vec);
      }

   private:
      dtCore::RefPtr<const dtCore::Transformable> mTransformable;
      dtCore::Transformable::CoordSysEnum mFrameOfRef;
   };

   template <typename VecType = osg::Vec3>
   class EvaluateMatrixPosition
   {
   public:
      EvaluateMatrixPosition(const osg::MatrixTransform& transform)
         : mTransform(&transform)
      {}

      void operator()(VecType& vec)
      {
         osg::Matrix m;
         osg::NodePathList nodePathList = mTransform->getParentalNodePaths();

         if ( !nodePathList.empty() )
         {
            osg::NodePath nodePath = nodePathList[0];
            m.set( osg::computeLocalToWorld(nodePath) );
         }
         else
         {
            m = mTransform->getMatrix();
         }

         vec = m.getTrans();
      }

   private:
      dtCore::RefPtr<const osg::MatrixTransform> mTransform;
   };

   template <typename VecType = osg::Vec3>
   class EvaluateVec3
   {
   public:
      EvaluateVec3(const VecType& sourceVec)
         : mVec(sourceVec)
      {}

      void operator()(VecType& vec)
      {
         vec = mVec;
      }
   private:
      const VecType& mVec;
   };

} // namespace dtAI

#endif // DISTANCESENSOR_H_
