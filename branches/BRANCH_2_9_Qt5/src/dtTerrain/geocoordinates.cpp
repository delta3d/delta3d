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
*
* Matthew W. Campbell
*/
#include "dtTerrain/geocoordinates.h"
#include "dtUtil/exception.h"
#include <osg/Math>

#include <iostream>
#include <sstream>

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   //These constants are hardcoded from the WGS84 standard.
   const double GeoCoordinates::EQUATORIAL_RADIUS = 6378137.0;
   const double GeoCoordinates::POLAR_RADIUS = 6356752.3142451794975639668;
   const double GeoCoordinates::FLATTENING  = 1.0/298.257223563;

   osg::Vec3d GeoCoordinates::gOriginOffset;
   GeoCoordinates GeoCoordinates::geoOrigin;

   //////////////////////////////////////////////////////////////////////////
   GeoCoordinates::GeoCoordinates()
   {
      mUpdateCartesianPoint = false;
      mLatitude = mLongitude = mAltitude = 0.0 ;
      mCartesianPoint = osg::Vec3(0,0,0);
   }

   //////////////////////////////////////////////////////////////////////////
   void GeoCoordinates::SetLatitude(int degrees, int minutes, int seconds)
   {
      if (degrees < -90 || degrees > 90)
         throw dtTerrain::GeoCoordinatesOutOfBoundsException(
         "Latitude degrees must be in the range of -90 to 90 inclusive.", __FILE__, __LINE__);

      if (minutes < 0 || minutes > 59)
         throw dtTerrain::GeoCoordinatesOutOfBoundsException(
         "Latitude minutes must be in the range of 0 to 59.", __FILE__, __LINE__);

      if (seconds < 0 || seconds > 59)
         throw dtTerrain::GeoCoordinatesOutOfBoundsException(
         "Latitude seconds must be in the range of 0 to 59.", __FILE__, __LINE__);

      if (degrees < 0)
      {
         mLatitude = -(osg::absolute((double)degrees) + ((double)minutes/60.0) + 
            ((double)seconds / 3600.0));
      }
      else
      {
         mLatitude = degrees + ((double)minutes/60.0) + ((double)seconds/3600.0);
      }

      mUpdateCartesianPoint = true;
   }   

   //////////////////////////////////////////////////////////////////////////
   void GeoCoordinates::GetLatitude(int &degrees, int &minutes, int &seconds)
   {
      degrees = (int)floor(mLatitude);
      minutes = (int)floor((mLatitude-degrees) * 60.0);
      seconds = (int)floor((((mLatitude-degrees)*60.0)-minutes) * 60.0);
   }

   //////////////////////////////////////////////////////////////////////////
   void GeoCoordinates::SetLongitude(int degrees, int minutes, int seconds)
   {
      if (degrees < -180 || degrees > 180)
         throw dtTerrain::GeoCoordinatesOutOfBoundsException(
         "Longitude degrees must be in the range of -180 to 180 inclusive.", __FILE__, __LINE__);

      if (minutes < 0 || minutes > 59)
         throw dtTerrain::GeoCoordinatesOutOfBoundsException(
         "Longitude minutes must be in the range of 0 to 59.", __FILE__, __LINE__);

      if (seconds < 0 || seconds > 59)
         throw dtTerrain::GeoCoordinatesOutOfBoundsException(
         "Longitude seconds must be in the range of 0 to 59.", __FILE__, __LINE__);

      if (degrees < 0)
      {
         mLongitude = -(osg::absolute((double)degrees) + ((double)minutes/60.0) + 
            ((double)seconds / 3600.0));
      }
      else
      {
         mLongitude = degrees + ((double)minutes/60.0) + ((double)seconds/3600.0);
      }

      mUpdateCartesianPoint = true;
   }   

   //////////////////////////////////////////////////////////////////////////
   void GeoCoordinates::GetLongitude(int &degrees, int &minutes, int &seconds)
   {
      degrees = (int)floor(mLongitude);
      minutes = (int)floor((mLongitude-degrees) * 60.0);
      seconds = (int)floor((((mLongitude-degrees)*60.0)-minutes) * 60.0);
   }   

      // helper func that should be somewhere
   std::string vec3dToString(const osg::Vec3d &pt)
   {
      std::ostringstream ost;
      ost << "Vec3d("<< pt.x()<< ", " << pt.y()<< ", " << pt.z()<< " )";
      return ost.str();
   }

   // helper func that should be somewhere
   std::string vec3ToString(const osg::Vec3 &pt)
   {
      std::ostringstream ost;
      ost << "Vec3 ("<< pt.x()<< ", " << pt.y()<< ", " << pt.z()<< " )";
      return ost.str();
   }

   //////////////////////////////////////////////////////////////////////////
   void GeoCoordinates::SetCartesianPoint(const osg::Vec3 &point)
   {
      // need to do conversions in dbl precision
      osg::Vec3d tmp(
         (double)point.x() + gOriginOffset.x(),
         (double)point.y() + gOriginOffset.y(),
         (double)point.z() + gOriginOffset.z()
         );
      // azuma: removed negation on mLatitude, so that as latitude increases, Y increases
      mLatitude = (tmp.y() / EQUATORIAL_RADIUS) * osg::RadiansToDegrees(1.0);
      mLongitude = (tmp.x() / EQUATORIAL_RADIUS) * osg::RadiansToDegrees(1.0);
      mAltitude = tmp.z();
      
      //Finally, make sure we keep the cartesian origin around.
      mCartesianPoint = point;
   }

   //////////////////////////////////////////////////////////////////////////
   const osg::Vec3 &GeoCoordinates::GetCartesianPoint()
   {
      if (mUpdateCartesianPoint)
      {
         osg::Vec3d tmp = osg::Vec3d();
         GetRawCartesianPoint(tmp);
         mCartesianPoint = tmp - gOriginOffset;
         mUpdateCartesianPoint = false;
      }

      return mCartesianPoint;
   } 

   // need this 'cause most Delta3d & esp. python uses Vec3 not Vec3d
   void GeoCoordinates::GetCartesianPoint( osg::Vec3& point )
   {
      osg::Vec3 tmp ;
      tmp = GetCartesianPoint();
      point.set( tmp.x(), tmp.y(), tmp.z() );
   }

   void GeoCoordinates::SetOrigin(const GeoCoordinates &geo)
   {
      geoOrigin = geo;
      geoOrigin.GetRawCartesianPoint(gOriginOffset);

      LOG_INFO("SetGeoOrigin: "+geoOrigin.ToStringAll());
      LOG_INFO("   offset:" +vec3dToString(gOriginOffset));
   }

   void GeoCoordinates::GetOrigin(GeoCoordinates &geo){
      geo = geoOrigin;
      LOG_DEBUG("getOrigin: "+geoOrigin.ToStringAll() + " offset: "+
         vec3dToString(gOriginOffset));
   }

   void GeoCoordinates::GetRawCartesianPoint(osg::Vec3d &pt)
   {
      pt.x() = (mLongitude * EQUATORIAL_RADIUS) * osg::DegreesToRadians(1.0);
      // azuma: removed negation on mLatitude, so that as latitude increases, Y increases
      pt.y() = (mLatitude * EQUATORIAL_RADIUS) * osg::DegreesToRadians(1.0);
      pt.z() = mAltitude;
   }

   std::string GeoCoordinates::ToString() const
   {
      std::ostringstream ost;
      ost << "GeoCoord(" << GetLatitude() << ", " << GetLongitude() << ", " << GetAltitude() << ")";
      return ost.str();
   }

   std::string GeoCoordinates::ToStringAll() const
   {
      std::ostringstream ost;
      ost << "GeoCoord(geo: " << GetLatitude() << ", " << GetLongitude() << ", " << GetAltitude() << 
         " cart: "<< mCartesianPoint.x()<< ", " << mCartesianPoint.y()<< ", " << mCartesianPoint.z()<< " )";
      return ost.str();
   }


   ////////////////////////////////////////////////////////////////////////////////
   GeoCoordinatesOutOfBoundsException::GeoCoordinatesOutOfBoundsException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
}
