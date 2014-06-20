/* -*-c++-*-
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
////////////////////////////////////////////////////////////////////////////////

#include <map>
#include <set>
#include <string>

#include <osg/Matrix>
#include <osg/Math>
#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec3f>
#include <osg/Matrix>

#include <dtUtil/export.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>
#include <dtUtil/mathdefines.h>

namespace dtUtil
{
   class Log;

   const double METERS_PER_DEGREE = 1852.0*60; /* The number of meters per longitude degree on the equator */

   const double MIN_LAT = ((-80.5 * osg::PI) / 180.0); /* -80.5 degrees in radians    */
   const double MAX_LAT = ((84.5 * osg::PI) / 180.0);  /* 84.5 degrees in radians     */
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

   const double MagneticNorthLatitude  = 82.116;
   const double MagneticNorthLongitude = 114.0666;

   struct DT_UTIL_EXPORT UTMParameters
   {
      UTMParameters();

      /*
       * The function CalcTranverseMercatorParameters receives the ellipsoid
       * parameters and Tranverse Mercator projection parameters as inputs, and
       * sets the corresponding state variables on the struct.
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
      void CalcTransverseMercatorParameters(double a, double f, double Origin_Latitude,
                                            double Central_Meridian, double False_Easting,
                                            double False_Northing, double Scale_Factor);
      double SPHTMD(double Latitude) const;
      double SPHSN(double Latitude) const;
      double DENOM(double Latitude) const;
      double SPHSR(double Latitude) const;

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
   };

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

         ///the terrain is flattened using a flat earth projection
         static const LocalCoordinateType CARTESIAN_FLAT_EARTH;
         ///the terrain is flattened using a UTM projection
         static const LocalCoordinateType CARTESIAN_UTM;

         ///Obsolete, don't use.  It will convert to CARTESIAN_UTM.
         static const LocalCoordinateType CARTESIAN;
      private:
         LocalCoordinateType(const std::string& name): dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
         virtual ~LocalCoordinateType() {}
   };

   ///Coordinate exception
   class CoordinateConversionInvalidInput : public dtUtil::Exception
   {
   public:
      CoordinateConversionInvalidInput(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~CoordinateConversionInvalidInput() {};
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
          * Sets the location of the game space origin in lat lon and converts it to an offset in UTM.
          * It also sets the utm zone and rotation conversion.
          *
          * @param lle latitude, longitude, and elevation.
          */
         void SetUTMLocalOffsetAsLatLon(const osg::Vec3d& lle);

         /**
          * Applies a simple offset to the local coordinates when converting
          *
          * @param offset the new offset
          */
         void SetLocalOffset(const osg::Vec3d& offset);

         /**
          * Retrieves local coordinate offset.
          *
          * @param offsetOut Fill this with the offset
          */
         void GetLocalOffset(osg::Vec3d& offsetOut) const;

         /**
          * Sets an origin point of reference lat lon for the incoming coordinates
          *
          * @param offset the new offset
          */
         void SetFlatEarthOrigin(const osg::Vec2d& originLL);

         /**
          * Retrieves the origin point of references lat lon for the incoming coordinates.
          *
          * @param originOut Fill this with the origin
          */
         void GetFlatEarthOrigin(osg::Vec2d& originLLOut) const;

         /**
          *  By default, the origin rotation matrix is set using the local coordinate space
          *  translation origin converted to remote / incoming translation. That is,
          *  if the remote coordinate system uses a geoid-based cartesian model, and the local uses
          *  a flat cartesian model, then the rotation conversion will drift away from correct
          *  the farther a position is from the point of reference.  If this is set with the remote position
          *  before converting the rotation each time, then no drift will occur.
          *
          *  This takes into account the incoming/remote coordinates type when calculating the matrix.
          * @see #ReconfigureRotationMatrix
          * @see #GetOriginRotationMatrix
          * @see #GetOriginRotationMatrixInverse
          */
         void SetRemoteReferenceForOriginRotationMatrix(const osg::Vec3d& translation);

         /**
          * Set the origin rotation matrix to the default of local coordinate space translation
          * origin converted the remote / incoming translation.
          *
          * This is called whenever the incoming or local coordinate types are changed.
          * The only reason to call this from code is to reset the matrix if
          * SetRemoteReferenceForOriginRotationMatrix is called.
          * @see #SetRemoteReferenceForOriginRotationMatrix
          * @see #GetOriginRotationMatrix
          * @see #GetOriginRotationMatrixInverse
          */
         void ReconfigureRotationMatrix();

         /// @return the rotation matrix that will convert incoming rotations to local rotations.
         const osg::Matrix& GetOriginRotationMatrix() const;

         /// @return the rotation matrix that will convert local rotations to incoming rotations.
         const osg::Matrix& GetOriginRotationMatrixInverse() const;

         /**
          * @return true if when converting the rotation, the origin rotation matrix is applied.
          * Otherwise rotation is assumed to need no conversion.
          */
         bool GetApplyRotationConversionMatrix() const { return mApplyRotationConversionMatrix; }

         /**
          * Sets whether or not the rotation needs to be converted via the origin rotation matrix.
          */
         void SetApplyRotationConversionMatrix(bool applyMatrix) { mApplyRotationConversionMatrix = applyMatrix; }

         const IncomingCoordinateType& GetIncomingCoordinateType() const { return *mIncomingCoordinateType; }

         void SetIncomingCoordinateType(const IncomingCoordinateType& incomingCoordType);

         const LocalCoordinateType& GetLocalCoordinateType() const { return *mLocalCoordinateType; }

         void SetLocalCoordinateType(const LocalCoordinateType& localCoordType);

         /**
          * Sets the globe radius for globe local coordinates.
          *
          * @param radius the new radius
          */
         void SetGlobeRadius(float radius);

         /**
          * returns the globe radius for globe local coordinates.
          *
          * @return the current globe radius
          */
         float GetGlobeRadius() const;

         /**
          * @return the currently set UTM zone.
          */
         unsigned GetUTMZone() const { return mUTMZone; }

         /**
          * Set the UTM zone to be used when converting coodinates from cartesian (Assumed to be UTM) to lat/lon.
          * @param zone the utm zone.  If it's not between 1 and 60 inclusive, it will be clamped.
          */
         void SetUTMZone(unsigned zone);

         /**
          * @return the currently set UTM hemisphere.
          */
         char GetUTMHemisphere() const { return mUTMHemisphere; }

         /**
          * Set the UTM hemishere to be used when converting coodinates from cartesian (Assumed to be UTM) to lat/lon.
          * @param hemisphere the utm hemisphere.  It must be 'N' or 'S' or lowercase.
          * Anything else will just be assigned as 'N'
          */
         void SetUTMHemisphere(char hemisphere);

         /**
          * Converts 3 part remote coordinates to a local translation vector based on the current
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
          * Converts psi theta phi coordinates in radians to a local rotation heading, pitch, roll in degrees
          * based on the current configuration.
          */
         const osg::Vec3 ConvertToLocalRotation(double psi, double theta, double phi);
         const osg::Vec3 ConvertToLocalRotation(const osg::Vec3& psiThetaPhi)
            { return ConvertToLocalRotation(psiThetaPhi[0], psiThetaPhi[1], psiThetaPhi[2]); };

         /**
          * Converts hpr coordinates in degrees to a remote rotation psi, theta, phi in radians
          * based on the current configuration.
          */
         const osg::Vec3d ConvertToRemoteRotation(const osg::Vec3& hpr);

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
          * Non static method to set the magnetic north offset variable on this class
          * @param magNorth The new offset to set
          */
         void SetMagneticNorthOffset(float magNorth) { mMagneticNorthOffset = magNorth; }

         /**
          * Non static accessor to the magentic north offset variable of this class.
          * This is in degrees and should be ADDED to the heading angle.  This is just a point
          * of storage.  None of the methods in this class use this angle.
          * @return mMagneticNorthOffset
          */
         float GetMagneticNorthOffset() const { return mMagneticNorthOffset; }

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
          * override parameters.  The UTM Zone and hemisphere must be passed in so that the code
          * knows which zone to use as the point of reference.
          * Call CalculateUTMZone if you want to know the "home" zone of the lat lon before calling this.
          * Code taken from http://earth-info.nga.mil/GandG/geotrans/
          *
          * @param   Latitude          : Latitude in radians                 (input)
          * @param   Longitude         : Longitude in radians                (input)
          * @param   Zone              : UTM zone                            (input)
          * @param   Hemisphere        : North or South hemisphere           (input)
          * @param   Easting           : Easting (X) in meters               (output)
          * @param   Northing          : Northing (Y) in meters              (output)
          */
         static void ConvertGeodeticToUTM(double Latitude, double Longitude, unsigned Zone,
                                    char Hemisphere, double& Easting, double& Northing);

         /**
          * The function ConvertUTMToGeodetic converts UTM projection (zone, easting and
          * northing) to geodetic (latitude and longitude) coordinates according to the current ellipsoid
          * and UTM zone override parameters.
          *
          * @param   zone              : UTM zone (east west)                (input)
          * @param   hemisphere        : UTM hemisphere ('N' or 'S')         (input)
          * @param   easting           : Easting (X) in meters               (input)
          * @param   northing          : Northing (Y) in meters              (input)
          * @param   latitude          : Latitude in radians                 (output)
          * @param   longitude         : Longitude in radians                (output)
          */
         static void ConvertUTMToGeodetic(unsigned zone, char hemisphere, double easting, double northing, double& latitude, double& longitude);

         /**
          * Calculates the proper UTM zone based on the latitude and longitude.
          *
          * @param latitude the latitude position for the zone.
          * @param longitude the longitude position for the zone.
          * @param ewZone the east west zone number.  This the normal UTM zone used in calculations.
          * @param nsZone the north-south zone letter.
          */
         static void CalculateUTMZone(double latitude, double longitude, unsigned& ewZone, char& nsZone);

         /**
          * Converts UTM coordinates to MGRS coordinates and returns the value as a string.
          *
          * @param easting the UTM easting value
          * @param northing the UTM northing value
          * @param eastWestZone the east/west zone number (1 - 60)
          * @param northSouthZone the north/sound zone letter (A-Z omitting I and O)
          * @param resolution the resolution number.  It should be 0-5 for resolutions
          *        of 100000 meters to 1 meter in powers of 10.
          */
         static const std::string ConvertUTMToMGRS(double easting, double northing, unsigned eastWestZone,
                                                   char northSouthZone, unsigned resolution);


         /**
          * @throws dtUtil::Exception if the string is not in a valid format.
          */
         static void ConvertMGRSToUTM(unsigned defaultZone, char defaultZoneLetter,
                                      const std::string& mgrs, unsigned& zone,
                                      double& easting, double& northing);


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

         static void ConvertTransverseMercatorToGeodetic(const UTMParameters& params,
                  double Easting, double Northing,
                  double &Latitude, double &Longitude);

         /**
          * The function ConvertGeodeticToTransverse_Mercator converts geodetic
          * (latitude and longitude) coordinates to Transverse Mercator projection
          * (easting and northing) coordinates, according to the current ellipsoid
          * and Transverse Mercator projection coordinates.
          *
          * @param   params        : UTM Parameter values.
          * @param   Latitude      : Latitude in radians                         (input)
          * @param   Longitude     : Longitude in radians                        (input)
          * @param   Easting       : Easting/X in meters                         (output)
          * @param   Northing      : Northing/Y in meters                        (output)
          */
         static void ConvertGeodeticToTransverseMercator(const UTMParameters& params, double Latitude, double Longitude,
                                                   double& Easting, double& Northing);


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
          * @param y the location in which to store the geocentric y coordinate
          * @param z the location in which to store the geocentric z coordinate
          */
         static void GeodeticToGeocentric(double phi, double lambda, double elevation,
                                          double& x, double& y, double& z);

         /**
          * Adjusts for magnetic north of the earth
          * @param latitude the latitude in DEGREES
          * @param longitude the longitude in DEGREES
          * @return The rotation offset in DEGREES
          */
         static float CalculateMagneticNorthOffset(const float latitude, const float longitude);

         static double CalculateConvergencParamForFlatEarth(double latitude);
         static void ConvertLatLonToFlatEarth(osg::Vec3d& xyz, const osg::Vec3d& lle, const osg::Vec2d& originll, double convergenceParam);
         static void ConvertFlatEarthToLatLon(osg::Vec3d& lle, const osg::Vec3d& xyz, const osg::Vec2d& originll, double convergenceParam);

         std::string XYZToMGRS(const osg::Vec3 &pos);

         osg::Vec3 ConvertMGRSToXYZ(const std::string& mgrs);

      private:

         /**
          * Calculates the origin rotation matrix from a lat lon.  phi and lambda are
          * lat and lon in radians.
          */
         void CalculateLocalRotationMatrixLL(double phi, double lambda);

         Log* mLogger;

         const LocalCoordinateType* mLocalCoordinateType;
         const IncomingCoordinateType* mIncomingCoordinateType;

         unsigned mUTMZone;
         char mUTMHemisphere;

         ///The radius of the globe if the local coordinates are in globe mode.
         float mGlobeRadius;

         float mMagneticNorthOffset;

         /**
          * Applies an offset to the local coordinates.
          */
         osg::Vec3d mLocalOffset;

         /**
          * Sets an incoming origin reference for flat earth.
          */
         osg::Vec2d mFlatEarthOrigin;

         /**
          * The rotation offset matrix.
          */
         osg::Matrix mRotationOffset;

         /*
          * The rotation offset matrix inverse.
          */
         osg::Matrix mRotationOffsetInverse;

         /// A parameter used in the flat earth calculation
         double mConvergence;

         /// converts rotation using the calculated rotation conversion matrix.
         bool mApplyRotationConversionMatrix;
         /// A flag used to tell it to reconpute the rotation matrix next time it converts.
         bool mRotationDirty;

      public:

         Coordinates& operator = (const Coordinates &rhs);
         bool operator == (const Coordinates &rhs) const;
         Coordinates(const Coordinates &rhs) { *this = rhs; }
   };
}

#endif

