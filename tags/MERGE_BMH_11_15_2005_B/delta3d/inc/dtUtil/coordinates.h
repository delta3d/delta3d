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

#include "dtUtil/export.h"

const double PI = 3.14159265358979323e0;         /* PI */
const double MIN_LAT = ( (-80.5 * PI) / 180.0 ); /* -80.5 degrees in radians    */
const double MAX_LAT = ( (84.5 * PI) / 180.0 );  /* 84.5 degrees in radians     */
const double MIN_EASTING = 100000;
const double MAX_EASTING = 900000;
const double MIN_NORTHING = 0;
const double MAX_NORTHING = 10000000;
const double MAX_DELTA_LONG  = ((PI * 90)/180.0);    /* 90 degrees in radians */
const double MIN_SCALE_FACTOR = 0.3;
const double MAX_SCALE_FACTOR = 3.0;
const double PI_OVER_2 = (PI / 2.0e0);
const double COS_67P5 = 0.38268343236508977;  /* cosine of 67.5 degrees */
const double AD_C     =  1.0026000;            /* Toms region 1 constant */

static double TranMerc_a = 6378137.0;         /* Semi-major axis of ellipsoid i meters */
static double TranMerc_es = 0.0066943799901413800; /* Eccentricity (0.08181919084262188000) squared */
/* Isometeric to geodetic latitude parameters, default to WGS 84 */
static double TranMerc_ap = 6367449.1458008;
static double TranMerc_bp = 16038.508696861;
static double TranMerc_cp = 16.832613334334;
static double TranMerc_dp = 0.021984404273757;
static double TranMerc_ep = 3.1148371319283e-005;

inline double SPHTMD(double Latitude)
{
   return ((double) (TranMerc_ap * Latitude 
      - TranMerc_bp * sin(2.e0 * Latitude) + TranMerc_cp * sin(4.e0 * Latitude) 
      - TranMerc_dp * sin(6.e0 * Latitude) + TranMerc_ep * sin(8.e0 * Latitude)));
}

inline double SPHSN(double Latitude) 
{
   return ((double) (TranMerc_a / sqrt( 1.e0 - TranMerc_es * pow(sin(Latitude), 2))));
}

inline double DENOM(double Latitude)
{ 
   return ((double) (sqrt(1.e0 - TranMerc_es * pow(sin(Latitude),2))));
}

inline double SPHSR(double Latitude) 
{
   return ((double) (TranMerc_a * (1.e0 - TranMerc_es) / pow(DENOM(Latitude), 3)));
}

namespace dtUtil
{
   class DT_UTIL_EXPORT Coordinates
   {
      public:
         
         /**
          * Sets the location of the origin in geodetic coordinates.
          *
          * @param latitude the latitude of the origin
          * @param longitude the longitude of the origin
          * @param elevation the elevation of the origin
          */
         void SetGeoOrigin(double latitude, double longitude, double elevation);
         
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
         void GetOriginLocation(double* x, double* y, double* z) const;
         
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
         void GetOriginRotation(float* h, float* p, float* r) const;
         
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
         static void MatrixToEulers(osg::Matrix& src, float* psi, float* theta, float* phi);
         
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
                                          double* latitude, double* longitude, double* elevation);
         
         /*
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
         void ConvertGeodeticToUTM (double Latitude, double Longitude, long   *Zone, 
                                    char   *Hemisphere, double *Easting, double *Northing); 
         
         /*
          * The function ConvertGeocentricToGeodetic converts geocentric
          * coordinates (X, Y, Z) to geodetic coordinates (latitude, longitude, 
          * and height), according to the current ellipsoid parameters.
          * Code taken from http://earth-info.nga.mil/GandG/geotrans/
          *
          * @param   X         : Geocentric X coordinate, in meters.         (input)
          * @param   Y         : Geocentric Y coordinate, in meters.         (input)
          * @param   Z         : Geocentric Z coordinate, in meters.         (input)
          * @param   Latitude  : Calculated latitude value in radians.       (output)
          * @param   Longitude : Calculated longitude value in radians.      (output)
          * @param   Height    : Calculated height value, in meters.         (output)
          *
          * The method used here is derived from 'An Improved Algorithm for
          * Geocentric to Geodetic Coordinate Conversion', by Ralph Toms, Feb 1996
          */
         void ConvertGeocentricToGeodetic (double X, double Y, double Z,
                                           double *Latitude, double *Longitude, double *Height);
         
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
                                                   double *Easting, double *Northing);
         
         /**
          * Converts a set of geodetic coordinates to the equivalent geocentric
          * coordinates.  Uses the formula given at
          * <A HREF="http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html">
          * http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html</A>.
          *
          * @param latitude the geodetic latitude
          * @param longitude the geodetic longitude
          * @param elevation the geodetic elevation
          * @param x the location in which to store the geocentric x coordinate
          * @parma y the location in which to store the geocentric y coordinate
          * @param z the location in which to store the geocentric z coordinate
          */
         static void GeodeticToGeocentric(double latitude, double longitude, double elevation,
                                          double* x, double* y, double* z);
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
         
      private:
         
         long     mZone;
         char     mHemisphere;
         double   mEasting;
         double   mNorthing;
         double   mLat;
         double   mLong;
         double   mElevation;
         
   };
};

#endif // DELTA_RTICONNECTION

