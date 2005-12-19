#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <plib/ul.h>

#if defined(__APPLE__)
#include <sys/socket.h>
#include <netinet/in.h>
#elif !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include <sys/socket.h>
#include <linux/in.h>
#endif

#include <cal3d/tinyxml.h>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>
#include <osg/Material>
#include <osg/StateSet>

#include "osgDB/FileUtils"

#include "osgUtil/IntersectVisitor"

#include "dtCore/system.h"
#include "dtUtil/matrixutil.h"
#include "dtUtil/coordinates.h"

/**
 * The length of the semi-major axis, in meters (WGS 84).
 */
const double semiMajorAxis = 6378137.0;

/**
 * The reciprocal of the flattening parameter (WGS 84).
 */
const double flatteningReciprocal = 298.257223563;

/* Ellipsoid Parameters, default to WGS 84  */
static double TranMerc_f = 1 / 298.257223563;      /* Flattening of ellipsoid  */
static double TranMerc_ebs = 0.0067394967565869;   /* Second Eccentricity squared */

static double UTM_a = 6378137.0;         /* Semi-major axis of ellipsoid in meters  */
static double UTM_f = 1 / 298.257223563; /* Flattening of ellipsoid                 */
//static long   UTM_Override = 0;          /* Zone override flag                      */
/* Transverse_Mercator projection Parameters */
static double TranMerc_Origin_Lat = 0.0;           /* Latitude of origin in radians */
static double TranMerc_Origin_Long = 0.0;          /* Longitude of origin in radians */
static double TranMerc_False_Northing = 0.0;       /* False northing in meters */
static double TranMerc_False_Easting = 0.0;        /* False easting in meters */
static double TranMerc_Scale_Factor = 1.0;         /* Scale factor  */

/* Maximum variance for easting and northing values for WGS 84. */
static double TranMerc_Delta_Easting = 40000000.0;
static double TranMerc_Delta_Northing = 40000000.0;

/* Ellipsoid parameters, default to WGS 84 */
static double Geocent_a = 6378137.0;          /* Semi-major axis of ellipsoid in meters */
static double Geocent_f = 1 / 298.257223563;  /* Flattening of ellipsoid           */

static double Geocent_e2 = 0.0066943799901413800;   /* Eccentricity squared  */
static double Geocent_ep2 = 0.00673949675658690300; /* 2nd eccentricity squared */

namespace dtUtil
{
   /**
    * Sets the location of the origin in geodetic coordinates.
    *
    * @param latitude the latitude of the origin
    * @param longitude the longitude of the origin
    * @param elevation the elevation of the origin
    */
   void Coordinates::SetGeoOrigin(double latitude, double longitude, double elevation)
   {
      GeodeticToGeocentric(latitude, 
                           longitude, 
                           elevation,
                           mLocationOffset,
                           mLocationOffset + 1,
                           mLocationOffset + 2);
      
      osg::Vec3 xVec ( 1, 0, 0 );
      osg::Vec3 zVec( 0, 0, 1 );
      
      mRotationOffset.makeRotate(osg::DegreesToRadians(90.0f - latitude), xVec);
      
      osg::Matrix mat;
      
      mat.makeRotate(osg::DegreesToRadians(longitude + 90.0f), zVec);
      
      mRotationOffset = mat * mRotationOffset;
      
      mRotationOffsetInverse.invert(mRotationOffset);
   }
  
   /**
    * Sets the location of the origin in geocentric coordinates.
    *
    * @param x the x coordinate of the location offset
    * @param y the y coordinate of the location offset
    * @param z the z coordinate of the location offset
    */
   void Coordinates::SetOriginLocation(double x, double y, double z)
   {
      mLocationOffset[0] = x;
      mLocationOffset[1] = y;
      mLocationOffset[2] = z;
   }
  
   /**
    * Retrieves the location of the origin in geocentric coordinates.
    *
    * @param x the location in which to store the x coordinate
    * @param y the location in which to store the y coordinate
    * @param z the location in which to store the z coordinate
    */
   void Coordinates::GetOriginLocation(double* x, double* y, double* z) const
   {
      *x = mLocationOffset[0];
      *y = mLocationOffset[1];
      *z = mLocationOffset[2];
   }
  
   /**
    * Sets the rotation of the origin relative to geocentric coordinates.
    *
    * @param h the geocentric heading (in degrees)
    * @param p the geocentric pitch (in degrees)
    * @param r the geocentric roll (in degrees)
    */
   void Coordinates::SetOriginRotation(float h, float p, float r)
   {
      //sgMakeRotMat4(mRotationOffset, h, p, r);
     dtUtil::MatrixUtil::HprToMatrix(mRotationOffset, osg::Vec3(h, p, r)); 
     
     //sgInvertMat4(mRotationOffsetInverse, mRotationOffset);
     mRotationOffsetInverse.invert(mRotationOffset);
   }
  
   /**
    * Retrieves the rotation of the origin relative to geocentric coordinates.
    *
    * @param h the location in which to store the geocentric heading
    * @param p the location in which to store the geocentric pitch
    * @param r the location in which to store the geocentric roll
    */
   void Coordinates::GetOriginRotation(float* h, float* p, float* r) const
   {
      osg::Vec3 tmp;
      dtUtil::MatrixUtil::MatrixToHpr(tmp, mRotationOffset);
      *h = tmp[0]; *p = tmp[1]; *r = tmp[2];
   }
  
   /**
    * Creates a 4x4 rotation matrix from a set of DIS/RPR-FOM Euler angles.
    *
    * @param dst the destination matrix
    * @param psi the psi angle
    * @param theta the theta angle
    * @param phi the phi angle
    */
   void Coordinates::EulersToMatrix(osg::Matrix& dst, float psi, float theta, float phi)
   {
      dtUtil::MatrixUtil::HprToMatrix(dst, osg::Vec3(osg::RadiansToDegrees(-psi) - 90.0f, osg::RadiansToDegrees(theta), osg::RadiansToDegrees(phi)));
      
      dst(0,1) = -dst(0,1);
      dst(1,1) = -dst(1,1);
      dst(2,1) = -dst(2,1);
      
      dst(0,2) = -dst(0,2);
      dst(1,2) = -dst(1,2);
      dst(2,2) = -dst(2,2);
   }
  
   /**
    * Returns the DIS/RPR-FOM Euler angles corresponding to the given rotation
    * matrix.
    *
    * @param src the source matrix
    * @param psi the location in which to store the psi angle
    * @param theta the location in which to store the theta angle
    * @param phi the location in which to store the phi angle
    */
   void Coordinates::MatrixToEulers(osg::Matrix& src, float* psi, float* theta, float* phi)
   {
      osg::Vec3 coord;
      osg::Matrix mat = src;
      
      mat(0,1) = -mat(0,1);
      mat(1,1) = -mat(1,1);
      mat(2,1) = -mat(2,1);
      
      mat(0,2) = -mat(0,2);
      mat(1,2) = -mat(1,2);
      mat(2,2) = -mat(2,2);
      
      dtUtil::MatrixUtil::MatrixToHpr(coord, mat);
      
      *psi = (-coord[0] - 90.0f) * osg::DegreesToRadians(1.0f);
      *theta = coord[1] * osg::DegreesToRadians(1.0f);
      *phi = coord[2] * osg::DegreesToRadians(1.0f);
   }
  
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
   void Coordinates::GeocentricToGeodetic(double x, double y, double z,
                                          double* latitude, double* longitude, 
                                          double* elevation)
   {
      double p = sqrt(x*x + y*y),
        a = semiMajorAxis,
        f = 1.0/flatteningReciprocal,
        b = a - a*f,
        theta = atan( (z*a)/(p*b) ),
        epsqu = (a*a - b*b)/(b*b),
        esqu = 2.0*f - f*f;
      
      *latitude = atan((z + epsqu * b * pow(sin(theta), 3)) /
                       (p - esqu * a * pow(cos(theta), 3)));
      
      *longitude = atan2(y, x);
      
      *elevation = p/cos(*latitude) - 
        a/sqrt(1.0-esqu*pow(sin(*latitude), 2.0));
      
      *latitude *= osg::RadiansToDegrees(1.0);
      *longitude *= osg::RadiansToDegrees(1.0);
   }
  
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
   void Coordinates::ConvertGeodeticToUTM (double Latitude, double Longitude,
                                           long   *Zone, char   *Hemisphere, double *Easting, double *Northing)
   {
      long Lat_Degrees;
      long Long_Degrees;
      long temp_zone;
      double Origin_Latitude = 0;
      double Central_Meridian = 0;
      double False_Easting = 500000;
      double False_Northing = 0;
      double Scale = 0.9996;
      
      /* no errors */
      if (Longitude < 0)
        Longitude += (2*PI) + 1.0e-10;
      Lat_Degrees = (long)(Latitude * 180.0 / PI);
      Long_Degrees = (long)(Longitude * 180.0 / PI);
      
      if (Longitude < PI)
        temp_zone = (long)(31 + ((Longitude * 180.0 / PI) / 6.0));
      else
        temp_zone = (long)(((Longitude * 180.0 / PI) / 6.0) - 29);
      if (temp_zone > 60)
        temp_zone = 1;
      /* UTM special cases */
      if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > -1)
          && (Long_Degrees < 3))
        temp_zone = 31;
      if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > 2)
          && (Long_Degrees < 12))
        temp_zone = 32;
      if ((Lat_Degrees > 71) && (Long_Degrees > -1) && (Long_Degrees < 9))
        temp_zone = 31;
      if ((Lat_Degrees > 71) && (Long_Degrees > 8) && (Long_Degrees < 21))
        temp_zone = 33;
      if ((Lat_Degrees > 71) && (Long_Degrees > 20) && (Long_Degrees < 33))
        temp_zone = 35;
      if ((Lat_Degrees > 71) && (Long_Degrees > 32) && (Long_Degrees < 42))
        temp_zone = 37;
      
      if (temp_zone >= 31)
        Central_Meridian = (6 * temp_zone - 183) * PI / 180.0;
      else
        Central_Meridian = (6 * temp_zone + 177) * PI / 180.0;
      *Zone = temp_zone;
      if (Latitude < 0)
      {
         False_Northing = 10000000;
         *Hemisphere = 'S';
      }
      else
        *Hemisphere = 'N';
      SetTransverseMercatorParameters(UTM_a, UTM_f, Origin_Latitude,
                                      Central_Meridian, False_Easting, False_Northing, Scale);
      ConvertGeodeticToTransverseMercator(Latitude, Longitude, Easting, Northing);
      
   } /* END OF Convert_Geodetic_To_UTM */
  
   /**
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
   void Coordinates::ConvertGeocentricToGeodetic (double X, double Y, double Z, double *Latitude,
                                                  double *Longitude, double *Height)
   { /* BEGIN Convert_Geocentric_To_Geodetic */
     
     double W;        /* distance from Z axis */
     double W2;       /* square of distance from Z axis */
     double T0;       /* initial estimate of vertical component */
     double T1;       /* corrected estimate of vertical component */
     double S0;       /* initial estimate of horizontal component */
     double S1;       /* corrected estimate of horizontal component */
     double Sin_B0;   /* sin(B0), B0 is estimate of Bowring aux variable */
     double Sin3_B0;  /* cube of sin(B0) */
     double Cos_B0;   /* cos(B0) */
     double Sin_p1;   /* sin(phi1), phi1 is estimated latitude */
     double Cos_p1;   /* cos(phi1) */
     double Rn;       /* Earth radius at location */
     double Sum;      /* numerator of cos(phi1) */
     int At_Pole;     /* indicates location is in polar region */
     double Geocent_b = Geocent_a * (1 - Geocent_f); /* Semi-minor axis of ellipsoid, in meters */
     
     At_Pole = FALSE;
     if (X != 0.0)
     {
        *Longitude = atan2(Y,X);
     }
     else
     {
        if (Y > 0)
        {
           *Longitude = PI_OVER_2;
        }
        else if (Y < 0)
        {
           *Longitude = -PI_OVER_2;
        }
        else
        {
           At_Pole = TRUE;
           *Longitude = 0.0;
           if (Z > 0.0)
           {  /* north pole */
              *Latitude = PI_OVER_2;
           }
           else if (Z < 0.0)
           {  /* south pole */
              *Latitude = -PI_OVER_2;
           }
           else
           {  /* center of earth */
              *Latitude = PI_OVER_2;
              *Height = -Geocent_b;
              return;
           } 
        }
     }
     W2 = X*X + Y*Y;
     W = sqrt(W2);
     T0 = Z * AD_C;
     S0 = sqrt(T0 * T0 + W2);
     Sin_B0 = T0 / S0;
     Cos_B0 = W / S0;
     Sin3_B0 = Sin_B0 * Sin_B0 * Sin_B0;
     T1 = Z + Geocent_b * Geocent_ep2 * Sin3_B0;
     Sum = W - Geocent_a * Geocent_e2 * Cos_B0 * Cos_B0 * Cos_B0;
     S1 = sqrt(T1*T1 + Sum * Sum);
     Sin_p1 = T1 / S1;
     Cos_p1 = Sum / S1;
     Rn = Geocent_a / sqrt(1.0 - Geocent_e2 * Sin_p1 * Sin_p1);
     if (Cos_p1 >= COS_67P5)
     {
        *Height = W / Cos_p1 - Rn;
     }
     else if (Cos_p1 <= -COS_67P5)
     {
        *Height = W / -Cos_p1 - Rn;
     }
     else
     {
        *Height = Z / Sin_p1 + Rn * (Geocent_e2 - 1.0);
     }
     if (At_Pole == FALSE)
     {
        *Latitude = atan(Sin_p1 / Cos_p1);
     }
   } /* END OF Convert_Geocentric_To_Geodetic */
  
   /**
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
   void Coordinates::SetTransverseMercatorParameters(double a, double f, double Origin_Latitude,
                                                     double Central_Meridian, double False_Easting, 
                                                     double False_Northing, double Scale_Factor)
     
   { /* BEGIN Set_Tranverse_Mercator_Parameters */
     
      double tn;        /* True Meridianal distance constant  */
      double tn2;
      double tn3;
      double tn4;
      double tn5;
      double dummy_northing;
      double TranMerc_b; /* Semi-minor axis of ellipsoid, in meters */
      
      TranMerc_a = a;
      TranMerc_f = f;
      TranMerc_Origin_Lat = 0;
      TranMerc_Origin_Long = 0;
      TranMerc_False_Northing = 0;
      TranMerc_False_Easting = 0; 
      TranMerc_Scale_Factor = 1;
      
      /* Eccentricity Squared */
      TranMerc_es = 2 * TranMerc_f - TranMerc_f * TranMerc_f;
      /* Second Eccentricity Squared */
      TranMerc_ebs = (1 / (1 - TranMerc_es)) - 1;
      
      TranMerc_b = TranMerc_a * (1 - TranMerc_f);    
      /*True meridianal constants  */
      tn = (TranMerc_a - TranMerc_b) / (TranMerc_a + TranMerc_b);
      tn2 = tn * tn;
      tn3 = tn2 * tn;
      tn4 = tn3 * tn;
      tn5 = tn4 * tn;
      
      TranMerc_ap = TranMerc_a * (1.e0 - tn + 5.e0 * (tn2 - tn3)/4.e0
                                  + 81.e0 * (tn4 - tn5)/64.e0 );
      TranMerc_bp = 3.e0 * TranMerc_a * (tn - tn2 + 7.e0 * (tn3 - tn4)
                                         /8.e0 + 55.e0 * tn5/64.e0 )/2.e0;
      TranMerc_cp = 15.e0 * TranMerc_a * (tn2 - tn3 + 3.e0 * (tn4 - tn5 )/4.e0) /16.0;
      TranMerc_dp = 35.e0 * TranMerc_a * (tn3 - tn4 + 11.e0 * tn5 / 16.e0) / 48.e0;
      TranMerc_ep = 315.e0 * TranMerc_a * (tn4 - tn5) / 512.e0;
      ConvertGeodeticToTransverseMercator(MAX_LAT,
                                          MAX_DELTA_LONG,
                                          &TranMerc_Delta_Easting,
                                          &TranMerc_Delta_Northing);
      ConvertGeodeticToTransverseMercator(0,
                                          MAX_DELTA_LONG,
                                          &TranMerc_Delta_Easting,
                                          &dummy_northing);
      TranMerc_Origin_Lat = Origin_Latitude;
      if (Central_Meridian > PI)
        Central_Meridian -= (2*PI);
      TranMerc_Origin_Long = Central_Meridian;
      TranMerc_False_Northing = False_Northing;
      TranMerc_False_Easting = False_Easting; 
      TranMerc_Scale_Factor = Scale_Factor;
   }  /* END of Set_Transverse_Mercator_Parameters  */
  
   /**
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
   void Coordinates::ConvertGeodeticToTransverseMercator (double Latitude,
                                                          double Longitude,
                                                          double *Easting,
                                                          double *Northing)
   {      /* BEGIN Convert_Geodetic_To_Transverse_Mercator */
      double c;       /* Cosine of latitude                          */
      double c2;
      double c3;
      double c5;
      double c7;
      double dlam;    /* Delta longitude - Difference in Longitude       */
      double eta;     /* constant - TranMerc_ebs *c *c                   */
      double eta2;
      double eta3;
      double eta4;
      double s;       /* Sine of latitude                        */
      double sn;      /* Radius of curvature in the prime vertical       */
      double t;       /* Tangent of latitude                             */
      double tan2;
      double tan3;
      double tan4;
      double tan5;
      double tan6;
      double t1;      /* Term in coordinate conversion formula - GP to Y */
      double t2;      /* Term in coordinate conversion formula - GP to Y */
      double t3;      /* Term in coordinate conversion formula - GP to Y */
      double t4;      /* Term in coordinate conversion formula - GP to Y */
      double t5;      /* Term in coordinate conversion formula - GP to Y */
      double t6;      /* Term in coordinate conversion formula - GP to Y */
      double t7;      /* Term in coordinate conversion formula - GP to Y */
      double t8;      /* Term in coordinate conversion formula - GP to Y */
      double t9;      /* Term in coordinate conversion formula - GP to Y */
      double tmd;     /* True Meridional distance                        */
      double tmdo;    /* True Meridional distance for latitude of origin */
      double temp_Origin;
      double temp_Long;
      
      if (Longitude > PI)
        Longitude -= (2 * PI);
      if ((Longitude < (TranMerc_Origin_Long - MAX_DELTA_LONG))
          || (Longitude > (TranMerc_Origin_Long + MAX_DELTA_LONG)))
      {
         if (Longitude < 0)
           temp_Long = Longitude + 2 * PI;
         else
           temp_Long = Longitude;
         if (TranMerc_Origin_Long < 0)
           temp_Origin = TranMerc_Origin_Long + 2 * PI;
         else
           temp_Origin = TranMerc_Origin_Long;
      }
      
      /* 
       *  Delta Longitude
       */
      dlam = Longitude - TranMerc_Origin_Long;
      
      if (dlam > PI)
        dlam -= (2 * PI);
      if (dlam < -PI)
        dlam += (2 * PI);
      if (fabs(dlam) < 2.e-10)
        dlam = 0.0;
      
      s = sin(Latitude);
      c = cos(Latitude);
      c2 = c * c;
      c3 = c2 * c;
      c5 = c3 * c2;
      c7 = c5 * c2;
      t = tan (Latitude);
      tan2 = t * t;
      tan3 = tan2 * t;
      tan4 = tan3 * t;
      tan5 = tan4 * t;
      tan6 = tan5 * t;
      eta = TranMerc_ebs * c2;
      eta2 = eta * eta;
      eta3 = eta2 * eta;
      eta4 = eta3 * eta;
      
      /* radius of curvature in prime vertical */
      sn = SPHSN(Latitude);
      
      /* True Meridianal Distances */
      tmd = SPHTMD(Latitude);
      
      /*  Origin  */
      tmdo = SPHTMD (TranMerc_Origin_Lat);
      
      /* northing */
      t1 = (tmd - tmdo) * TranMerc_Scale_Factor;
      t2 = sn * s * c * TranMerc_Scale_Factor/ 2.e0;
      t3 = sn * s * c3 * TranMerc_Scale_Factor * (5.e0 - tan2 + 9.e0 * eta 
                                                  + 4.e0 * eta2) /24.e0; 
      
      t4 = sn * s * c5 * TranMerc_Scale_Factor * (61.e0 - 58.e0 * tan2
                                                  + tan4 + 270.e0 * eta - 330.e0 * tan2 * eta + 445.e0 * eta2
                                                  + 324.e0 * eta3 -680.e0 * tan2 * eta2 + 88.e0 * eta4 
                                                  -600.e0 * tan2 * eta3 - 192.e0 * tan2 * eta4) / 720.e0;
      
      t5 = sn * s * c7 * TranMerc_Scale_Factor * (1385.e0 - 3111.e0 * 
                                                  tan2 + 543.e0 * tan4 - tan6) / 40320.e0;
      
      *Northing = TranMerc_False_Northing + t1 + pow(dlam,2.e0) * t2
        + pow(dlam,4.e0) * t3 + pow(dlam,6.e0) * t4
        + pow(dlam,8.e0) * t5; 
      
      /* Easting */
      t6 = sn * c * TranMerc_Scale_Factor;
      t7 = sn * c3 * TranMerc_Scale_Factor * (1.e0 - tan2 + eta ) /6.e0;
      t8 = sn * c5 * TranMerc_Scale_Factor * (5.e0 - 18.e0 * tan2 + tan4
                                              + 14.e0 * eta - 58.e0 * tan2 * eta + 13.e0 * eta2 + 4.e0 * eta3 
                                              - 64.e0 * tan2 * eta2 - 24.e0 * tan2 * eta3 )/ 120.e0;
      t9 = sn * c7 * TranMerc_Scale_Factor * ( 61.e0 - 479.e0 * tan2
                                               + 179.e0 * tan4 - tan6 ) /5040.e0;
      
      *Easting = TranMerc_False_Easting + dlam * t6 + pow(dlam,3.e0) * t7 
        + pow(dlam,5.e0) * t8 + pow(dlam,7.e0) * t9;
      
   } /* END OF Convert_Geodetic_To_Transverse_Mercator */
  
  
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
   void Coordinates::GeodeticToGeocentric(double latitude, double longitude, double elevation,
                                          double* x, double* y, double* z)
   {
      double rlatitude = latitude * osg::DegreesToRadians(1.0f),
        rlongitude = longitude * osg::DegreesToRadians(1.0f),
        a = semiMajorAxis,
        f = 1.0/flatteningReciprocal,
        esqu = 2.0*f - f*f,
        n = a/sqrt(1.0-esqu*pow(sin(rlatitude), 2.0));
      
      *x = (n + elevation)*cos(rlatitude)*cos(rlongitude);
      
      *y = (n + elevation)*cos(rlatitude)*sin(rlongitude);
      
      *z = (n*(1.0-esqu) + elevation)*sin(rlatitude);
   }
}
