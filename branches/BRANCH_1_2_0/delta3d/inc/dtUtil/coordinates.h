/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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
 */

#ifndef DELTA_COORDINATES
#define DELTA_COORDINATES

// coordinates.h: Declaration of the coordinates class.
//
//////////////////////////////////////////////////////////////////////

#include <map>
#include <set>
#include <string>

#include <osgSim/DOFTransform>
#include <osg/Matrix>
#include <osg/Math>

#include "dtUtil/export.h"
#include "dtUtil/enumeration.h"
#include "dtUtil/mathdefines.h"

namespace dtUtil
{
   class Log;
   
   const double MIN_LAT = ( (-80.5 * osg::PI) / 180.0 ); /* -80.5 degrees in radians    */
   const double MAX_LAT = ( (84.5 * osg::PI) / 180.0 );  /* 84.5 degrees in radians     */
   const double MIN_EASTING = 100000;
   const double MAX_EASTING = 900000;
   const double MIN_NORTHING = 0;
   const double MAX_NORTHING = 10000000;
   const double MAX_DELTA_LONG  = ((osg::PI * 90)/180.0);    /* 90 degrees in radians */
   const double MIN_SCALE_FACTOR = 0.3;
   const double MAX_SCALE_FACTOR = 3.0;
   const double COS_67P5 = 0.38268343236508977;  /* cosine of 67.5 degrees */
   const double AD_C     =  1.0026000;            /* Toms region 1 constant */

   /**
    * The reciprocal of the flattening parameter (WGS 84).
    */
   const double flatteningReciprocal = 298.257223563;
         
   /**
    * The length of the semi-major axis, in meters (WGS 84).
    */
   const double semiMajorAxis = 6378137.0;

   /* Ellipsoid parameters in WGS 84 */
   const double Geocent_a = semiMajorAxis;  /* Semi-major axis of ellipsoid in meters */
   const double Geocent_f = 1 / flatteningReciprocal; /* Flattening of ellipsoid           */
   
   const double Geocent_e2 = (2.0 - Geocent_f) * Geocent_f;   /* Eccentricity squared  */
   const double Geocent_ep2 = Geocent_e2 / (1.0 - Geocent_e2); /* 2nd eccentricity squared */
   const double Geocent_ef = Geocent_f / (2.0 - Geocent_f);
   const double Geocent_ef_3 = Geocent_ef * Geocent_ef * Geocent_ef;
   const double Geocent_ef_4 = Geocent_ef_3 * Geocent_ef;
   const double Geocent_e2_2 = Geocent_e2 * Geocent_e2;
   const double Geocent_e2_3 = Geocent_e2_2* Geocent_e2;
   
   ///Scale used in UTM calculations.
   const double CentralMeridianScale = 0.9996;
         
   class DT_UTIL_EXPORT IncomingCoordinateType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(IncomingCoordinateType);
      
      public:
         //Defines coordinates as XYZ based on the center of the earth with the z-axis
         //down, x going through the prime meridian in the equatorial plane, and y 90 degrees of it.
         //Rotation is based on the refenence point in the center of the earth.
         static const IncomingCoordinateType GEOCENTRIC;
         
         //Uses the WGS84 flattening parameters and coordinates are in latitde, longitude, and elevation.
         //Rotation is based on the UTM zone.
         static const IncomingCoordinateType GEODETIC;
         
         //Coordinates are base on zones in the earth and offsets within that zone. This is terrain
         //centric coordinates and each zone is treated like it's a flat plate.
         //Rotation is in reference to the terrain.
         static const IncomingCoordinateType UTM;
         
      private:
         IncomingCoordinateType(const std::string& name): dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
         virtual ~IncomingCoordinateType() {}
   };
   
   class DT_UTIL_EXPORT LocalCoordinateType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(LocalCoordinateType);
      
      public:
         ///The local terrain is a globe or part of a globe so that the XYZ coordinates should be mapped around it.
         static const LocalCoordinateType GLOBE;
         
         ///the terrain is a flat plain
         static const LocalCoordinateType CARTESIAN;
         
      private:
         LocalCoordinateType(const std::string& name): dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
         virtual ~LocalCoordinateType() {}
   };

   template <typename T>
   inline T safeASIN(T x)
   {
      return ((x) < -1.0 ? (-0.5*osg::PI) : ((x) > 1.0) ? (0.5*osg::PI) : (asin(x)));
   }
   
   class DT_UTIL_EXPORT Coordinates
   {
      public:
      
         Coordinates();
         virtual ~Coordinates();
         
         
         /**
          * Sets the location of the origin in geodetic coordinates.
          *
          * @param latitude the latitude of the origin
          * @param longitude the longitude of the origin
          * @param elevation the elevation of the origin
          */
         void SetGeoOrigin(double latitude, double longitude, double elevation);

         /**
          * Creates a rotation offset matrix used when converting rotations
          * from geocentric one relative to the zone of the given latitude and longitude.
          * The given parameters, expected to be in degrees, will be used to find out the zone of
          * the terrain which will be used as the actual point of reference. 
          */
         void SetGeoOriginRotation(double latitude, double longitude);
         
         /**
          * Sets the location of the origin in geocentric coordinates.
          *
          * @param x the x coordinate of the location offset
          * @param y the y coordinate of the location offset
          * @param z the z coordinate of the location offset
          */
         void SetOriginLocation(double x, double y, double z);
         
         
         /**
          * Retrieves the location of the origin in geocentric coordinates.
          *
          * @param x the location in which to store the x coordinate
          * @param y the location in which to store the y coordinate
          * @param z the location in which to store the z coordinate
          */
         void GetOriginLocation(double& x, double& y, double& z) const;
         
         /**
          * Sets the rotation of the origin relative to geocentric coordinates.
          *
          * @param h the geocentric heading (in degrees)
          * @param p the geocentric pitch (in degrees)
          * @param r the geocentric roll (in degrees)
          */
         void SetOriginRotation(float h, float p, float r);
         
         /**
          * Retrieves the rotation of the origin relative to geocentric coordinates.
          *
          * @param h the location in which to store the geocentric heading
          * @param p the location in which to store the geocentric pitch
          * @param r the location in which to store the geocentric roll
          */
         void GetOriginRotation(float& h, float& p, float& r) const;
         
         const osg::Matrix& GetOriginRotationMatrix() const;
         
         const osg::Matrix& GetOriginRotationMatrixInverse() const;

         const IncomingCoordinateType& GetIncomingCoordinateType() { return *mIncomingCoordinateType; }

         void SetIncomingCoordinateType(const IncomingCoordinateType& incomingCoordType)
         {
            mIncomingCoordinateType = &incomingCoordType;  
         }
         
         const LocalCoordinateType& GetLocalCoordinateType() { return *mLocalCoordinateType; }

         void SetLocalCoordinateType(const LocalCoordinateType& localCoordType)
         {
            mLocalCoordinateType = &localCoordType;  
         }
         
         /* Sets the globe radius.
          *
          * @param radius the new radius
          */
         void SetGlobeRadius(float radius);
         
         /**
          * Returns the globe radius.
          *
          * @return the current globe radius
          */
         float GetGlobeRadius() const;
         
         /**
          * @return the currently set UTM zone.
          */
         unsigned long GetUTMZone() { return mZone; }
         
         /**
          * Set the UTM zone to be used when converting coodinates from cartesian (Assumed to be UTM) to lat/lon.
          * @param zone the utm zone.  If it's not between 1 and 60 inclusive, it will be clamped. 
          */
         void SetUTMZone(unsigned long zone) { CLAMP(zone, 1L, 60L); mZone = zone; };
         
         /**
          * Converts XYZ coordinates to a local translation vector based on the current
          * configuration.
          * @param loc the location as 3 doubles.
          * @return the x y z location is local space.
          */
         const osg::Vec3 ConvertToLocalTranslation(const osg::Vec3d& loc);

         /**
          * Converts XYZ coordinates to a remote location vector based on the current
          * configuration.
          * @param translation the local x,y,z translation to convert.
          * @return the location as a vec3d
          */
         const osg::Vec3d ConvertToRemoteTranslation(const osg::Vec3& translation);

         /**
          * Converts php coordinates in radians to a local rotation heading, pitch, roll in degrees
          * based on the current configuration.
          */
         const osg::Vec3 ConvertToLocalRotation(double psi, double theta, double phi) const;

         /**
          * Converts hpr coordinates in degrees to a remote rotation psi, theta, phi in radians
          * based on the current configuration.
          */
         const osg::Vec3d ConvertToRemoteRotation(const osg::Vec3& hpr) const;
         
         /**
          * Creates a 4x4 rotation matrix from a set of DIS/RPR-FOM Euler angles.
          *
          * @param dst the destination matrix
          * @param psi the psi angle
          * @param theta the theta angle
          * @param phi the phi angle
          */
         static void EulersToMatrix(osg::Matrix& dst, float psi, float theta, float phi);
         
         /**
          * Returns the DIS/RPR-FOM Euler angles corresponding to the given rotation
          * matrix.
          *
          * @param src the source matrix
          * @param psi the location in which to store the psi angle
          * @param theta the location in which to store the theta angle
          * @param phi the location in which to store the phi angle
          */
         static void MatrixToEulers(const osg::Matrix& src, float& psi, float& theta, float& phi) ;
         
         /*
          * invert the z axis
          */
         static void ZFlop(osg::Matrix& toFlop);

         /**
          * Converts a set of geocentric coordinates to the equivalent geodetic
          * coordinates.  Uses the formula given at
          * <A HREF="http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html">
          * http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html</A>.
          *
          * @param x the geocentric x coordinate
          * @param y the geocentric y coordinate
          * @param z the geocentric z coordinate
          * @param latitude the location in which to store the geodetic latitude
          * @param longitude the location in which to store the geodetic longitude
          * @param elevation the location in which to store the geodetic elevation
          */
         static void GeocentricToGeodetic(double x, double y, double z,
                                          double& latitude, double& longitude, double& elevation);
         
         /**
          * The function ConvertGeodeticToUTM converts geodetic (latitude and
          * longitude) coordinates to UTM projection (zone, hemisphere, easting and
          * northing) coordinates according to the current ellipsoid and UTM zone
          * override parameters.  Code taken from http://earth-info.nga.mil/GandG/geotrans/
          *
          * @param   Latitude          : Latitude in radians                 (input)
          * @param   Longitude         : Longitude in radians                (input)
          * @param   Zone              : UTM zone                            (output)
          * @param   Hemisphere        : North or South hemisphere           (output)
          * @param   Easting           : Easting (X) in meters               (output)
          * @param   Northing          : Northing (Y) in meters              (output)
          */
         void ConvertGeodeticToUTM(double Latitude, double Longitude, unsigned long& Zone, 
                                    char  &Hemisphere, double& Easting, double& Northing); 

         /**
          * The function ConvertUTMToGeodetic converts UTM projection (zone, easting and
          * northing) to geodetic (latitude and longitude) coordinates according to the current ellipsoid 
          * and UTM zone override parameters.
          *
          * @param   zone              : UTM zone                            (input)
          * @param   easting           : Easting (X) in meters               (input)
          * @param   northing          : Northing (Y) in meters              (input)
          * @param   latitude          : Latitude in radians                 (output)
          * @param   longitude         : Longitude in radians                (output)
          */
         static void ConvertUTMToGeodetic(long zone, double easting, double northing, double& latitude, double& longitude); 
         
         /**
          * Calculates the proper UTM zone based on the latitude and longitude.
          * 
          * @param latitude the latitude position for the zone.
          * @param longitude the longitude position for the zone.
          * @param ewZone the east west zone number.  This the normal UTM zone used in calculations.
          * @praam nsZone the north-south zone letter.
          */
         static void CalculateUTMZone(double latitude, double longitude, unsigned long& ewZone, char& nsZone);
         
         /**
          * The function ConvertGeocentricToGeodetic converts geocentric
          * coordinates (X, Y, Z) to geodetic coordinates (latitude, longitude, 
          * and height), according to the current ellipsoid parameters.
          * Code taken from http://earth-info.nga.mil/GandG/geotrans/
          *
          * @param   x         : Geocentric X coordinate, in meters.         (input)
          * @param   y         : Geocentric Y coordinate, in meters.         (input)
          * @param   z         : Geocentric Z coordinate, in meters.         (input)
          * @param   phi       : Calculated latitude value in radians.       (output)
          * @param   lambda    : Calculated longitude value in radians.      (output)
          * @param   elevation : Calculated height value, in meters.         (output)
          *
          * The method used here is derived from 'An Improved Algorithm for
          * Geocentric to Geodetic Coordinate Conversion', by Ralph Toms, Feb 1996
          */
         static void ConvertGeocentricToGeodetic (double x, double y, double z,
                                           double& phi, double& lambda, double& elevation);
         
         /*
          * The function SetTranverseMercatorParameters receives the ellipsoid
          * parameters and Tranverse Mercator projection parameters as inputs, and
          * sets the corresponding state variables. 
          * Code taken from http://earth-info.nga.mil/GandG/geotrans/
          *
          * @param   a                 : Semi-major axis of ellipsoid, in meters    (input)
          * @param   f                 : Flattening of ellipsoid                    (input)
          * @param   Origin_Latitude   : Latitude in radians at the origin of the   (input)
          *                         projection
          * @param   Central_Meridian  : Longitude in radians at the center of the  (input)
          *                         projection
          * @param   False_Easting     : Easting/X at the center of the projection  (input)
          * @param   False_Northing    : Northing/Y at the center of the projection (input)
          * @param   Scale_Factor      : Projection scale factor                    (input) 
          */
         void SetTransverseMercatorParameters(double a, double f, double Origin_Latitude,
                                              double Central_Meridian, double False_Easting,
                                              double False_Northing, double Scale_Factor);
         
         /*
          * The function ConvertGeodeticToTransverse_Mercator converts geodetic
          * (latitude and longitude) coordinates to Transverse Mercator projection
          * (easting and northing) coordinates, according to the current ellipsoid
          * and Transverse Mercator projection coordinates.  
          *
          * @param   Latitude      : Latitude in radians                         (input)
          * @param   Longitude     : Longitude in radians                        (input)
          * @param   Easting       : Easting/X in meters                         (output)
          * @param   Northing      : Northing/Y in meters                        (output)
          */
         void ConvertGeodeticToTransverseMercator (double Latitude, double Longitude,
                                                   double& Easting, double& Northing) const;
         
         /**
          * Converts a set of geodetic coordinates to the equivalent geocentric
          * coordinates.  Uses the formula given at
          * <A HREF="http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html">
          * http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html</A>.
          *
          * @param phi the geodetic latitude in radians
          * @param lambda the geodetic longitude in radians
          * @param elevation the geodetic elevation
          * @param x the location in which to store the geocentric x coordinate
          * @parma y the location in which to store the geocentric y coordinate
          * @param z the location in which to store the geocentric z coordinate
          */
         static void GeodeticToGeocentric(double phi, double lambda, double elevation,
                                          double& x, double& y, double& z);

         /**
          * Converts degrees to mils
          * @param degrees The degrees to convert
          */
         static unsigned int DegreesToMils(const float degrees);

         /**
          * Converts mils to degrees
          * @param mils The mils to convert
          * @note If mils is greater than 6400 then it will be clamped
          * to 6400 implicitly
          */
         static float MilsToDegrees(const unsigned int mils);
         
      private:
         
         Log* mLogger;
         
         const LocalCoordinateType* mLocalCoordinateType;
         const IncomingCoordinateType* mIncomingCoordinateType;
         
         double TranMerc_a;         /* Semi-major axis of ellipsoid i meters */
         double TranMerc_es; /* Eccentricity (0.08181919084262188000) squared */
      
         /* Isometeric to geodetic latitude parameters, default to WGS 84 */
         double TranMerc_ap;
         double TranMerc_bp;
         double TranMerc_cp;
         double TranMerc_dp;
         double TranMerc_ep;
      
         /* Transverse_Mercator projection Parameters */
         double TranMerc_Origin_Lat;           /* Latitude of origin in radians */
         double TranMerc_Origin_Long;          /* Longitude of origin in radians */
         double TranMerc_False_Northing;       /* False northing in meters */
         double TranMerc_False_Easting;        /* False easting in meters */
         double TranMerc_Scale_Factor;         /* Scale factor  */
         
         /* Maximum variance for easting and northing values for WGS 84. */
         double TranMerc_Delta_Easting;
         double TranMerc_Delta_Northing;
      
         /* Ellipsoid Parameters, default to WGS 84  */
         double TranMerc_f;      /* Flattening of ellipsoid  */
         double TranMerc_ebs;   /* Second Eccentricity squared */

         long     mZone;
         char     mHemisphere;
         double   mEasting;
         double   mNorthing;
         
         ///The radius of the globe if the local coordinates are in globe mode.
         float mGlobeRadius;
  
         /**
          * The location of the origin in geocentric coordinates.
          */
         double mLocationOffset[3];
         
         /**
          * The rotation offset matrix.
          */
         osg::Matrix mRotationOffset;
         
         /*
          * The rotation offset matrix inverse.
          */
         osg::Matrix mRotationOffsetInverse;
                  
         double SPHTMD(double Latitude) const;         
         double SPHSN(double Latitude) const; 
         double DENOM(double Latitude) const;
         double SPHSR(double Latitude) const; 
         
      public:

         Coordinates& operator = (const Coordinates &rhs);
         bool operator == (const Coordinates &rhs);
         Coordinates(const Coordinates &rhs) { *this = rhs; }
   };

   inline double Coordinates::SPHTMD(double Latitude) const
   {
      return ((double) (TranMerc_ap * Latitude 
         - TranMerc_bp * sin(2.e0 * Latitude) + TranMerc_cp * sin(4.e0 * Latitude) 
         - TranMerc_dp * sin(6.e0 * Latitude) + TranMerc_ep * sin(8.e0 * Latitude)));
   }
   
   inline double Coordinates::SPHSN(double Latitude) const
   {
      return ((double) (TranMerc_a / sqrt( 1.e0 - TranMerc_es * pow(sin(Latitude), 2))));
   }
   
   inline double Coordinates::DENOM(double Latitude) const
   { 
      return ((double) (sqrt(1.e0 - TranMerc_es * pow(sin(Latitude),2))));
   }
   
   inline double Coordinates::SPHSR(double Latitude) const
   {
      return ((double) (TranMerc_a * (1.e0 - TranMerc_es) / pow(DENOM(Latitude), 3)));
   }
};

#endif

