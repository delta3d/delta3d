/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */
#include <dtHLAGM/ddmcameracalculatorgeographic.h>

#include <dtCore/camera.h>
#include <dtCore/transform.h>

#include <dtCore/datatype.h>
#include <dtCore/floatactorproperty.h>

#include <dtHLAGM/ddmgeographicregiondata.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmutil.h>

#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec3d>

namespace dtHLAGM
{
   const std::string DDMCameraCalculatorGeographic::PROP_X_SUBSCRIPTION_RANGE("X Subscription Range");
   const std::string DDMCameraCalculatorGeographic::PROP_Y_SUBSCRIPTION_RANGE("Y Subscription Range");
   const std::string DDMCameraCalculatorGeographic::PROP_MIN_TIME_BETWEEN_UPDATES("Min Time Between Updates");

   //////////////////////////////////////////////////////////////
   DDMCameraCalculatorGeographic::DDMCameraCalculatorGeographic(): BaseClass(), mXRange(100), mYRange(100), mMinTimeBetweenUpdates(0.25)
   {
      AddProperty(new dtCore::FloatActorProperty(PROP_X_SUBSCRIPTION_RANGE, PROP_X_SUBSCRIPTION_RANGE,
            dtCore::FloatActorProperty::SetFuncType(this, &DDMCameraCalculatorGeographic::SetXSubscriptionRange),
            dtCore::FloatActorProperty::GetFuncType(this, &DDMCameraCalculatorGeographic::GetXSubscriptionRange)
            ));
      AddProperty(new dtCore::FloatActorProperty(PROP_Y_SUBSCRIPTION_RANGE, PROP_Y_SUBSCRIPTION_RANGE,
            dtCore::FloatActorProperty::SetFuncType(this, &DDMCameraCalculatorGeographic::SetYSubscriptionRange),
            dtCore::FloatActorProperty::GetFuncType(this, &DDMCameraCalculatorGeographic::GetYSubscriptionRange)
            ));
      AddProperty(new dtCore::FloatActorProperty(PROP_MIN_TIME_BETWEEN_UPDATES, PROP_MIN_TIME_BETWEEN_UPDATES,
            dtCore::FloatActorProperty::SetFuncType(this, &DDMCameraCalculatorGeographic::SetMinTimeBetweenUpdates),
            dtCore::FloatActorProperty::GetFuncType(this, &DDMCameraCalculatorGeographic::GetMinTimeBetweenUpdates)
            ));
   }

   //////////////////////////////////////////////////////////////
   DDMCameraCalculatorGeographic::~DDMCameraCalculatorGeographic()
   {
   }

   //////////////////////////////////////////////////////////////
   double DDMCameraCalculatorGeographic::FixLongitudeRange(double longitude) const
   {
      if (longitude < 0)
         longitude += 360.0;
      else if (longitude > 360.0)
         longitude -= 360.0;

      return longitude;
   }

   //////////////////////////////////////////////////////////////
   bool DDMCameraCalculatorGeographic::UpdateRegionData(DDMRegionData& ddmData) const
   {
      if (!BaseClass::UpdateRegionData(ddmData))
         return false;

      if (!mCamera.valid())
         return false;

      dtUtil::Log& logger = dtUtil::Log::GetInstance("ddmcameracalculatorgeographic.cpp");

      DDMGeographicRegionData* ddmGeoData = dynamic_cast<DDMGeographicRegionData*>(&ddmData);
      if (ddmGeoData == NULL)
         return false;

      DDMCalculatorGeographic::DDMForce& force = ddmGeoData->GetForce();
      DDMCalculatorGeographic::DDMObjectKind& kind = GetCalculatorObjectKind();

      std::pair<DDMCalculatorGeographic::RegionCalculationType*, long> appSpacePair = GetAppSpaceValues(force, kind);
      unsigned int mappedValue = MapAppSpaceValue(appSpacePair.second);

      if (logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Using app space number "
               "%d on for type %s %s.  It maps to %u", appSpacePair.second, force.GetName().c_str(), kind.GetName().c_str(), mappedValue);
      }

      if (*appSpacePair.first == DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY && ddmData.GetNumberOfExtents() > 1)
      {
         //We're changing from 3 to 1.  This really shouldn't happen in practice, but it could.
         ddmData.ClearDimensions();
      }

      bool updated = UpdateDimension(ddmData, 0, GetFirstDimensionName(), mappedValue, mappedValue);

      if (*appSpacePair.first == DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE)
      {
         dtCore::Transform xform;
         mCamera->GetTransform(xform, dtCore::Transformable::ABS_CS);

         osg::Vec3 pos;
         xform.GetTranslation(pos);

         osg::Vec3 lowerBound(pos.x() - mXRange/2.0, pos.y() - mYRange/2.0, pos.z());
         osg::Vec3 upperBound(pos.x() + mXRange/2.0, pos.y() + mYRange/2.0, pos.z());

         const osg::Vec3d latLonElevLower = mCoordinates.ConvertToRemoteTranslation(lowerBound);
         const osg::Vec3d latLonElevUpper = mCoordinates.ConvertToRemoteTranslation(upperBound);

         if (UpdateDimension(ddmData, 1, GetSecondDimensionName(),
               DDMUtil::MapLinear(latLonElevLower.x() , -75.0, 75.0),
               DDMUtil::MapLinear(latLonElevUpper.x() , -75.0, 75.0)))
         {
            updated = true;
         }

         double lonLower = FixLongitudeRange(latLonElevLower.y());
         double lonUpper = FixLongitudeRange(latLonElevUpper.y());

         if (UpdateDimension(ddmData, 2, GetThirdDimensionName(),
               DDMUtil::MapLinear(lonLower, 0.0, 360.0),
               DDMUtil::MapLinear(lonUpper, 0.0, 360.0)))
         {
            updated = true;
         }

         if (updated && logger.IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Lat lon range of camera calculator named %s is "
                  "%lf %lf to %lf %lf", GetName().c_str(), latLonElevLower.x(), latLonElevLower.y(), latLonElevUpper.x(), latLonElevUpper.y());
         }
      }

      return updated;
   }

   //////////////////////////////////////////////////////////////
   void DDMCameraCalculatorGeographic::SetCamera(dtCore::Camera* camera)
   {
      mCamera = camera;
   }

   //////////////////////////////////////////////////////////////
   dtCore::Camera* DDMCameraCalculatorGeographic::GetCamera()
   {
      return mCamera.get();
   }

   //////////////////////////////////////////////////////////////
   void DDMCameraCalculatorGeographic::SetXSubscriptionRange(float range)
   {
      mXRange = range;
   }

   //////////////////////////////////////////////////////////////
   void DDMCameraCalculatorGeographic::SetYSubscriptionRange(float range)
   {
      mYRange = range;
   }

   //////////////////////////////////////////////////////////////
   void DDMCameraCalculatorGeographic::SetMinTimeBetweenUpdates(float minTime)
   {
      mMinTimeBetweenUpdates = minTime;
   }
}
