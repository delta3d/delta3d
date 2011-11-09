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
#ifndef DELTA_GEOCOORDINATES
#define DELTA_GEOCOORDINATES

#include <osg/Vec3>
#include <osg/Vec3d>
#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>
#include <dtTerrain/terrain_export.h>

namespace dtTerrain
{
   class GeoCoordinatesOutOfBoundsException : public dtUtil::Exception
   {
   public:
   	GeoCoordinatesOutOfBoundsException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~GeoCoordinatesOutOfBoundsException() {};
   };
   
   /**
    * This class is intended to represent cartesian and geographic coordinates.
    * The conversion between latitude and longitude to cartesian is rudimentary
    * at best.  Altitude is arbitrary.  Future versions of this class should 
    * use a proper geodetic representation of latitude, longitude, and altitude
    * but for now, this will suffice.
    */
   class DT_TERRAIN_EXPORT GeoCoordinates 
   {
      public:
         
         ///Equatorial Radius in meters. (WGS84)         
         static const double EQUATORIAL_RADIUS;
         
         ///Polar Radius in meters. (WGS84)
         static const double POLAR_RADIUS;
         
         ///Flattening coefficient in meters. (WGS84)
         static const double FLATTENING;
         
         /**
          * Constructs the coordinate system.
          */
         GeoCoordinates();
         
         /**
          * Compares the two coordinate's cartesian location.
          * @return True if they are equal, false otherwise.
          */
         bool operator==(const GeoCoordinates &rhs) const
         {
            //Before we compare, make sure we sync our geographic and
            //cartesian coordinates.  I realize const_cast is ugly and 
            //a hack but I wanted lazy updating of coordinate conversions.
            const_cast<GeoCoordinates*>(this)->GetCartesianPoint();           
            const_cast<GeoCoordinates &>(rhs).GetCartesianPoint();
            return mCartesianPoint == rhs.mCartesianPoint;
         }
         
         /**
          * Performs a less than comparison on this coordinate's cartesian
          * location with that of the other.
          * @param rhs The other coordinate the check against.
          * @return True if this is less than the other.
          */
         bool operator<(const GeoCoordinates &rhs) const
         {
            //Before we compare, make sure we sync our geographic and
            //cartesian coordinates.  I realize const_cast is ugly and 
            //a hack but I wanted lazy updating of coordinate conversions.
            const_cast<GeoCoordinates*>(this)->GetCartesianPoint();           
            const_cast<GeoCoordinates &>(rhs).GetCartesianPoint();
            return mCartesianPoint < rhs.mCartesianPoint;
         }
         
         /**
          * Sets the latitude of this coordinate.  This is stored
          * internally as decimal degrees.
          * @param degrees Latitude degrees ranges from -90 (south pole) to
          *    +90 (north pole).  If degrees is out of this range, an 
          *    OUT_OF_BOUNDS exception is thrown.
          * @param minutes Ranges from 0 - 59.  If minutes is out of this range, an 
          *    OUT_OF_BOUNDS exception is thrown.
          * @param seconds Ranges from 0 - 59.  If seconds is out of this range, an 
          *    OUT_OF_BOUNDS exception is thrown.
          */
         void SetLatitude(int degrees, int minutes = 0, int seconds = 0);
         
         /**
          * Sets the latitude of this coordinate.
          * @param decimalDegrees The new latitude in decimal degrees.
          */
         void SetLatitude(double decimalDegrees)
         {
            mLatitude = decimalDegrees;
            mUpdateCartesianPoint = true;
         }
         
         /**
          * Gets the latitude origin of this coordinate.
          * @return The latitude in decimal degrees.
          */
         double GetLatitude() const { return mLatitude; }
         
         /**
          * Gets the latitude origin of this coordinate.
          * @param degrees Stores the latitude degrees.
          * @param minutes Stores the latitude minutes.
          * @param seconds Stores the latitude seconds.
          */
         void GetLatitude(int &degrees, int &minutes, int &seconds);
         
         /**
          * Sets the longitude of this coordinate.  This is stored
          * internally as decimal degrees.
          * @param degrees Longitude degrees ranges from -180 (180 degrees west) to
          *    +180 (180 degrees east).  If degrees is out of this range, an 
          *    OUT_OF_BOUNDS exception is thrown.
          * @param minutes Ranges from 0 - 59.  If minutes is out of this range, an 
          *    OUT_OF_BOUNDS exception is thrown.
          * @param seconds Ranges from 0 - 59.  If seconds is out of this range, an 
          *    OUT_OF_BOUNDS exception is thrown.
          */
         void SetLongitude(int degrees, int minutes = 0, int seconds = 0);
         
         /**
          * Sets the longitude of this coordinate.
          * @param decimalDegrees The new longitude in decimal degrees.
          */
         void SetLongitude(double decimalDegrees)
         {
            mLongitude = decimalDegrees;
            mUpdateCartesianPoint = true;
         }
         
         /**
          * Gets the longitude origin of this coordinate.
          * @return The longitude in decimal degrees.
          */
         double GetLongitude() const { return mLongitude; }
         
         /**
          * Gets the longitude origin of this coordinate.
          * @param degrees Stores the latitude degrees.
          * @param minutes Stores the latitude minutes.
          * @param seconds Stores the latitude seconds.
          */
         void GetLongitude(int &degrees, int &minutes, int &seconds);
         
         /**
          * Sets the altitude of this coordinate.
          * @param alt Altitude.  The interpretation of this value is
          *    application specific for the time being.  Future versions
          *    of this class should represent altitude using as the 
          *    height above the reference (WGS84) ellipsoid.
          */
         void SetAltitude(double alt)
         {
            mAltitude = alt;
            mUpdateCartesianPoint = true;
         }            
         
         /**
          * Gets this coordinate's altitude.
          * @return The current altitude.
          */
         double GetAltitude() const { return mAltitude; }
         
         /**
          * Sets the location of this coordinate in cartesian space.
          * @param newLocation The new location in cartesian coordinates.
          * @note Internally, this origin in cartesian space is also converted
          *    to latitude, longitude coordinates.
          */
         void SetCartesianPoint(const osg::Vec3 &newLocation);
         
         /**
          * Gets this coordinate's location in cartesian space.
          * @return A 3D point in cartesian space. (meters).
          * @note If the origin was previously set via any of the geocentric
          *    methods, the origin in cartesian space is calculated and cached
          *    as required.
          */
         const osg::Vec3& GetCartesianPoint();
         void GetCartesianPoint(osg::Vec3& point); 
        
         static void SetOrigin(const GeoCoordinates& geoOrigin);
         static void GetOrigin(GeoCoordinates& geoOrigin);

         std::string ToString() const; // GeoCoord(lat,long,alt)
         std::string ToStringAll() const; // GeoCoord(geo: lat,long,alt cart:x,y,z)

      protected:
      
         ///Cached version of this coordinate sytems's origin in cartesian space.
         osg::Vec3 mCartesianPoint;
         
         ///Latitude origin in decimal degrees.
         double mLatitude;
         
         ///Longitude origin in decimal degrees.
         double mLongitude;
         
         ///Elevation origin.
         double mAltitude;
         
         ///Flag to indicate the cartesian origin needs to be updated.
         bool mUpdateCartesianPoint;

         void GetRawCartesianPoint( osg::Vec3d &point ); 
         static osg::Vec3d gOriginOffset;
         static GeoCoordinates geoOrigin;
   };
   
}         

#endif 
