#include <prefix/dtutilprefix.h>
#include <cmath>
#include <cstdio>
#include <cfloat>

#include <dtUtil/matrixutil.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mathdefines.h>

namespace dtUtil
{
   /////////////////////////////////////////////////////////////////////////////
   UTMParameters::UTMParameters()
   {
      TranMerc_a = 6378137.0;         // Semi-major axis of ellipsoid i meters
      TranMerc_es = 0.0066943799901413800; // Eccentricity (0.08181919084262188000) squared

      // Isometeric to geodetic latitude parameters, default to WGS 84
      TranMerc_ap = 6367449.1458008;
      TranMerc_bp = 16038.508696861;
      TranMerc_cp = 16.832613334334;
      TranMerc_dp = 0.021984404273757;
      TranMerc_ep = 3.1148371319283e-005;

      // Transverse_Mercator projection Parameters
      TranMerc_Origin_Lat = 0.0;           // Latitude of origin in radians
      TranMerc_Origin_Long = 0.0;          // Longitude of origin in radians
      TranMerc_False_Northing = 0.0;       // False northing in meters
      TranMerc_False_Easting = 0.0;        // False easting in meters
      TranMerc_Scale_Factor = 1.0;         // Scale factor

      // Maximum variance for easting and northing values for WGS 84.
      TranMerc_Delta_Easting = 40000000.0;
      TranMerc_Delta_Northing = 40000000.0;

      // Ellipsoid Parameters, default to WGS 84
      TranMerc_f = Geocent_f;      // Flattening of ellipsoid
      TranMerc_ebs = 0.0067394967565869;   // Second Eccentricity squared
   }

   /////////////////////////////////////////////////////////////////////////////
   void UTMParameters::CalcTransverseMercatorParameters(double a, double f, double Origin_Latitude,
                                                     double Central_Meridian, double False_Easting,
                                                     double False_Northing, double Scale_Factor)
   { // BEGIN Set_Tranverse_Mercator_Parameters
      double tn;        // True Meridianal distance constant
      double tn2;
      double tn3;
      double tn4;
      double tn5;
      double dummy_northing;
      double TranMerc_b; // Semi-minor axis of ellipsoid, in meters

      TranMerc_a = a;
      TranMerc_f = f;
      TranMerc_Origin_Lat = 0;
      TranMerc_Origin_Long = 0;
      TranMerc_False_Northing = 0;
      TranMerc_False_Easting = 0;
      TranMerc_Scale_Factor = 1;

      // Eccentricity Squared
      TranMerc_es = 2 * TranMerc_f - TranMerc_f * TranMerc_f;
      // Second Eccentricity Squared
      TranMerc_ebs = (1 / (1 - TranMerc_es)) - 1;

      TranMerc_b = TranMerc_a * (1 - TranMerc_f);
      // True meridianal constants
      tn = (TranMerc_a - TranMerc_b) / (TranMerc_a + TranMerc_b);
      tn2 = tn * tn;
      tn3 = tn2 * tn;
      tn4 = tn3 * tn;
      tn5 = tn4 * tn;

      TranMerc_ap = TranMerc_a * (1.e0 - tn + 5.e0 * (tn2 - tn3) / 4.e0
                                  + 81.e0 * (tn4 - tn5)/64.e0);
      TranMerc_bp = 3.e0 * TranMerc_a * (tn - tn2 + 7.e0 * (tn3 - tn4)
                                         /8.e0 + 55.e0 * tn5 / 64.e0) / 2.e0;
      TranMerc_cp = 15.e0 * TranMerc_a * (tn2 - tn3 + 3.e0 * (tn4 - tn5) / 4.e0) / 16.0;
      TranMerc_dp = 35.e0 * TranMerc_a * (tn3 - tn4 + 11.e0 * tn5 / 16.e0) / 48.e0;
      TranMerc_ep = 315.e0 * TranMerc_a * (tn4 - tn5) / 512.e0;
      Coordinates::ConvertGeodeticToTransverseMercator(*this, MAX_LAT, MAX_DELTA_LONG, TranMerc_Delta_Easting,
                                                       TranMerc_Delta_Northing);
      Coordinates::ConvertGeodeticToTransverseMercator(*this, 0, MAX_DELTA_LONG, TranMerc_Delta_Easting,
                                                       dummy_northing);
      TranMerc_Origin_Lat = Origin_Latitude;
      if (Central_Meridian > osg::PI)
      {
         Central_Meridian -= (2*osg::PI);
      }
      TranMerc_Origin_Long = Central_Meridian;
      TranMerc_False_Northing = False_Northing;
      TranMerc_False_Easting = False_Easting;
      TranMerc_Scale_Factor = Scale_Factor;
   }  // END of Set_Transverse_Mercator_Parameters

   /////////////////////////////////////////////////////////////////////////////
   double UTMParameters::SPHTMD(double Latitude) const
   {
      return ((double) (TranMerc_ap * Latitude
         - TranMerc_bp * sin(2.e0 * Latitude) + TranMerc_cp * sin(4.e0 * Latitude)
         - TranMerc_dp * sin(6.e0 * Latitude) + TranMerc_ep * sin(8.e0 * Latitude)));
   }

   /////////////////////////////////////////////////////////////////////////////
   double UTMParameters::SPHSN(double Latitude) const
   {
      return ((double) (TranMerc_a / sqrt(1.e0 - TranMerc_es * pow(sin(Latitude), 2))));
   }

   /////////////////////////////////////////////////////////////////////////////
   double UTMParameters::DENOM(double Latitude) const
   {
      return ((double) (sqrt(1.e0 - TranMerc_es * pow(sin(Latitude),2))));
   }

   /////////////////////////////////////////////////////////////////////////////
   double UTMParameters::SPHSR(double Latitude) const
   {
      return ((double) (TranMerc_a * (1.e0 - TranMerc_es) / pow(DENOM(Latitude), 3)));
   }

   IMPLEMENT_ENUM(IncomingCoordinateType)
   const IncomingCoordinateType IncomingCoordinateType::GEOCENTRIC("Geocentric");
   const IncomingCoordinateType IncomingCoordinateType::GEODETIC("Geodetic");
   const IncomingCoordinateType IncomingCoordinateType::UTM("UTM");

   IMPLEMENT_ENUM(LocalCoordinateType)
   const LocalCoordinateType LocalCoordinateType::GLOBE("Globe");
   const LocalCoordinateType LocalCoordinateType::CARTESIAN_UTM("Cartesian UTM");
   const LocalCoordinateType LocalCoordinateType::CARTESIAN_FLAT_EARTH("Cartesian Flat Earth");
   const LocalCoordinateType LocalCoordinateType::CARTESIAN("Cartesian");

   /////////////////////////////////////////////////////////////////////////////
   Coordinates::Coordinates()
      : mLocalCoordinateType(&LocalCoordinateType::CARTESIAN_UTM)
      , mIncomingCoordinateType(&IncomingCoordinateType::UTM)
      , mUTMZone(1)
      , mUTMHemisphere('N')
      , mGlobeRadius(0.0)
      , mMagneticNorthOffset(0.0)
      , mConvergence(0.0)
      , mApplyRotationConversionMatrix(true)
      , mRotationDirty(true)
   {
      mLogger = &Log::GetInstance("coordinates.cpp");

      mRotationOffset.makeIdentity();
      mRotationOffsetInverse.makeIdentity();
   }

   /////////////////////////////////////////////////////////////////////////////
   Coordinates::~Coordinates()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   Coordinates& Coordinates::operator = (const Coordinates &rhs)
   {
      if (this == &rhs)
      {
         return *this;
      }

      mLogger                 = rhs.mLogger;
      mLocalCoordinateType    = rhs.mLocalCoordinateType;
      mIncomingCoordinateType = rhs.mIncomingCoordinateType;
      mUTMZone                = rhs.mUTMZone;
      mUTMHemisphere          = rhs.mUTMHemisphere;
      mGlobeRadius            = rhs.mGlobeRadius;
      mMagneticNorthOffset    = rhs.mMagneticNorthOffset;

      mLocalOffset            = rhs.mLocalOffset;
      mFlatEarthOrigin         = rhs.mFlatEarthOrigin;
      mConvergence            = rhs.mConvergence;

      mRotationOffset         = rhs.mRotationOffset;
      mRotationOffsetInverse  = rhs.mRotationOffsetInverse;
      mApplyRotationConversionMatrix = rhs.mApplyRotationConversionMatrix;

      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Coordinates::operator == (const Coordinates& rhs) const
   {
      if (this == &rhs)
      {
         return true;
      }

      if (mLogger != rhs.mLogger)
      {
         return false;
      }
      if (mLocalCoordinateType != rhs.mLocalCoordinateType)
      {
         return false;
      }
      if (mIncomingCoordinateType != rhs.mIncomingCoordinateType)
      {
         return false;
      }
      if (mUTMZone != rhs.mUTMZone)
      {
         return false;
      }
      if (mUTMHemisphere != rhs.mUTMHemisphere)
      {
         return false;
      }
      if (mGlobeRadius != rhs.mGlobeRadius)
      {
         return false;
      }
      if (mMagneticNorthOffset != rhs.mMagneticNorthOffset)
      {
         return false;
      }

      if (!dtUtil::Equivalent(mLocalOffset, rhs.mLocalOffset, 0.0001))
      {
         return false;
      }

      if (!dtUtil::Equivalent(mFlatEarthOrigin, rhs.mFlatEarthOrigin, 0.0001))
      {
         return false;
      }

      if (!osg::equivalent(mConvergence, rhs.mConvergence, 0.0001))
      {
         return false;
      }

      if (mRotationOffset != rhs.mRotationOffset)
      {
         return false;
      }

      if (mRotationOffsetInverse != rhs.mRotationOffsetInverse)
      {
         return false;
      }

      if (mApplyRotationConversionMatrix != rhs.mApplyRotationConversionMatrix)
      {
         return false;
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ReconfigureRotationMatrix()
   {
      if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC ||
               *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
      {
         if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_FLAT_EARTH)
         {
            // The incoming origin is the lat lon that is the point of reference, so it should be
            // good for this
            CalculateLocalRotationMatrixLL(osg::DegreesToRadians(mFlatEarthOrigin[0]), osg::DegreesToRadians(mFlatEarthOrigin[1]));
         }
         else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_UTM)
         {
            double phi, lambda;
            //Use the configured utm zone, and local offset values to get a better approximation.
            ConvertUTMToGeodetic(mUTMZone, mUTMHemisphere, mLocalOffset.x(), mLocalOffset.y(), phi, lambda);
            CalculateLocalRotationMatrixLL(phi, lambda);
         }
         else if (*mLocalCoordinateType == LocalCoordinateType::GLOBE)
         {
            mRotationOffset.makeIdentity();
            mRotationOffsetInverse.makeIdentity();
         }
      }
      else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
      {
         mRotationOffset.makeIdentity();
         mRotationOffsetInverse.makeIdentity();
      }

      mRotationDirty = false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetUTMLocalOffsetAsLatLon(const osg::Vec3d& lle)
   {
      unsigned zone;
      char nsZone;

      double latitude = lle[0];
      double longitude = lle[1];

      CalculateUTMZone(latitude, longitude, zone, nsZone);
      SetUTMZone(zone);
      if (nsZone >= 'N')
      {
         SetUTMHemisphere('N');
      }
      else
      {
         SetUTMHemisphere('S');
      }

      double phi = osg::DegreesToRadians(latitude);
      double lambda = osg::DegreesToRadians(longitude);
      ConvertGeodeticToUTM(phi, lambda, mUTMZone, mUTMHemisphere, mLocalOffset[0], mLocalOffset[1]);

      mLocalOffset[2] = lle[2];
      CalculateLocalRotationMatrixLL(phi, lambda);
      /// We just recomputed the rotation, so its not dirty.
      mRotationDirty = false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetLocalOffset(const osg::Vec3d& offset)
   {
      mLocalOffset = offset;
      mRotationDirty = true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::GetLocalOffset(osg::Vec3d& offset) const
   {
      offset = mLocalOffset;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetFlatEarthOrigin(const osg::Vec2d& origin)
   {
      mFlatEarthOrigin = origin;
      mConvergence = CalculateConvergencParamForFlatEarth(origin[0]);
      mRotationDirty = true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::GetFlatEarthOrigin(osg::Vec2d& originOut) const
   {
      originOut = mFlatEarthOrigin;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetUTMZone(unsigned zone)
   {
      Clamp(zone, unsigned(1), unsigned(60));
      mUTMZone = zone;
      mRotationDirty = true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetUTMHemisphere(char hemisphere)
   {
      if (hemisphere == 's' || hemisphere == 'S')
      {
         mUTMHemisphere = 'S';
      }
      else
      {
         mUTMHemisphere = 'N';
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::CalculateLocalRotationMatrixLL(double phi, double lambda)
   {
      double sin_lat = sin(phi);
      double cos_lat = cos(phi);
      double sin_lon = sin(lambda);
      double cos_lon = cos(lambda);

      mRotationOffset(0,0) = -sin_lon;
      mRotationOffset(0,1) = -sin_lat * cos_lon;
      mRotationOffset(0,2) =  cos_lat * cos_lon;
      mRotationOffset(0,3) =  0.0;

      mRotationOffset(1,0) =  cos_lon;
      mRotationOffset(1,1) = -sin_lat * sin_lon;
      mRotationOffset(1,2) =  cos_lat * sin_lon;
      mRotationOffset(1,3) =  0.0;

      mRotationOffset(2,0) =  0.0;
      mRotationOffset(2,1) =  cos_lat;
      mRotationOffset(2,2) =  sin_lat;
      mRotationOffset(2,3) =  0.0;

      mRotationOffset.setTrans(0.0, 0.0, 0.0);
      mRotationOffset(3,3) =  1.0;

      mRotationOffsetInverse.invert(mRotationOffset);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetRemoteReferenceForOriginRotationMatrix(const osg::Vec3d& translation)
   {
      if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC ||
               *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
      {
         double phi, lambda, elevation;

         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            ConvertGeocentricToGeodetic(translation[0], translation[1], translation[2],
                     phi, lambda, elevation);
         }
         else
         {
            phi = osg::DegreesToRadians(translation[0]);
            lambda = osg::DegreesToRadians(translation[1]);
         }

         if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_FLAT_EARTH ||
                  *mLocalCoordinateType == LocalCoordinateType::CARTESIAN_UTM)
         {
            CalculateLocalRotationMatrixLL(phi, lambda);
         }
         else if (*mLocalCoordinateType == LocalCoordinateType::GLOBE)
         {
            // No conversion is done at all on globe, so we just make them indentity.
            mRotationOffset.makeIdentity();
            mRotationOffsetInverse.makeIdentity();
         }
      }
      else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
      {
         // UTM incoming to globe isn't supported right now, so this doesn't do anything for that.
         mRotationOffset.makeIdentity();
         mRotationOffsetInverse.makeIdentity();
      }
      mRotationDirty = false;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Matrix& Coordinates::GetOriginRotationMatrix() const
   {
      return mRotationOffset;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Matrix& Coordinates::GetOriginRotationMatrixInverse() const
   {
      return mRotationOffsetInverse;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetIncomingCoordinateType(const IncomingCoordinateType& incomingCoordType)
   {
      mIncomingCoordinateType = &incomingCoordType;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetLocalCoordinateType(const LocalCoordinateType& localCoordType)
   {
      if (localCoordType == LocalCoordinateType::CARTESIAN)
      {
         LOGN_ERROR("coordinates.cpp", "Setting LocalCoordinateType to CARTESIAN_UTM,"
                  " CARTESIAN was passed in, but it is replaced by CARTESIAN_UTM.")
         mLocalCoordinateType = &LocalCoordinateType::CARTESIAN_UTM;
      }
      else
      {
         mLocalCoordinateType = &localCoordType;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::SetGlobeRadius(float radius)
   {
      mGlobeRadius = radius;
   }

   /////////////////////////////////////////////////////////////////////////////
   float Coordinates::GetGlobeRadius() const
   {
      return mGlobeRadius;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec3 Coordinates::ConvertToLocalTranslation(const osg::Vec3d& loc)
   {
      if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
      {
         mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Converting coordinates.  Incoming coordinates are %lf, %lf, %lf.",
            loc[0], loc[1], loc[2]);
      }

      osg::Vec3 position;

      if (*mLocalCoordinateType == LocalCoordinateType::GLOBE)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            position[0] = (loc[0] / semiMajorAxis) * GetGlobeRadius();
            position[1] = (loc[1] / semiMajorAxis) * GetGlobeRadius();
            position[2] = (loc[2] / semiMajorAxis) * GetGlobeRadius();
         }
         else
         {
            LOGN_ERROR("coordinates.cpp", "With local coordinates in globe mode, only GEOCENTRIC coordinates types are supported.");
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_UTM)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            double lat, lon, elevation, easting, northing;

            ConvertGeocentricToGeodetic(loc[0], loc[1], loc[2],lat,lon,elevation);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Incoming lat lon: %lf, %lf",
                  osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon));
            }

            ConvertGeodeticToUTM(lat, lon, mUTMZone, mUTMHemisphere, easting, northing);

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            position[0] = easting - localOffset.x();
            position[1] = northing - localOffset.y();
            position[2] = elevation - localOffset.z();

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            // This code is not yet unit tested
            double easting, northing;

            ConvertGeodeticToUTM(osg::DegreesToRadians(loc[0]), osg::DegreesToRadians(loc[1]),
                     mUTMZone, mUTMHemisphere, easting, northing);

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            position[0] = easting - localOffset.x();
            position[1] = northing - localOffset.y();
            position[2] = loc[2] - localOffset.z();

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            position[0] = loc[0] - localOffset.x();
            position[1] = loc[1] - localOffset.y();
            position[2] = loc[2] - localOffset.z();
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_FLAT_EARTH)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            double lat, lon, elevation;
            ConvertGeocentricToGeodetic(loc[0], loc[1], loc[2],lat,lon,elevation);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Incoming lat lon: %lf, %lf",
                  osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon));
            }

            osg::Vec3d xyz;
            ConvertLatLonToFlatEarth(xyz, osg::Vec3d(osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon), elevation), mFlatEarthOrigin, mConvergence);
            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            position[0] = xyz.x() - localOffset.x();
            position[1] = xyz.y() - localOffset.y();
            position[2] = xyz.z() - localOffset.z();

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            osg::Vec3d xyz;
            ConvertLatLonToFlatEarth(xyz, loc, mFlatEarthOrigin, mConvergence);
            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            position[0] = xyz.x() - localOffset.x();
            position[1] = xyz.y() - localOffset.y();
            position[2] = xyz.z() - localOffset.z();
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
            double lat, lon;
            ConvertUTMToGeodetic(mUTMZone, mUTMHemisphere, loc[0], loc[1], lat, lon);
            osg::Vec3d xyz;
            ConvertLatLonToFlatEarth(xyz, osg::Vec3d(osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon), loc[2]), mFlatEarthOrigin, mConvergence);

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            position[0] = xyz.x() - localOffset.x();
            position[1] = xyz.y() - localOffset.y();
            position[2] = xyz.z() - localOffset.z();
         }
      }
      else
      {
         LOGN_ERROR("coordinates.cpp", "Unsupported local coordinate mode: " + mLocalCoordinateType->GetName());
      }

      for (unsigned i = 0; i < 3; ++i)
      {
         if (!IsFinite(position[i]))
         {
            position[i] = 0.0f;
         }
      }

      if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
      {
         mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Converting coordinates.  Resulting coordinates are %f, %f, %f.",
            position[0], position[1], position[2]);
      }
      return position;
   }

   /////////////////////////////////////////////////////////////////////////////
   double Coordinates::CalculateConvergencParamForFlatEarth(double latitude)
   {
      return cos(latitude * osg::PI / 180.0);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertLatLonToFlatEarth(osg::Vec3d& xyz, const osg::Vec3d& lle, const osg::Vec2d& originll, double convergenceParam)
   {
      xyz[0] = (lle[1] - originll[1]) * METERS_PER_DEGREE * convergenceParam;
      xyz[1] = (lle[0] - originll[0]) * METERS_PER_DEGREE;
      xyz[2] = lle[2];
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertFlatEarthToLatLon(osg::Vec3d& lle, const osg::Vec3d& xyz, const osg::Vec2d& originll, double convergenceParam)
   {
      lle[0] = originll[0] + xyz[1]/METERS_PER_DEGREE;
      lle[1] = originll[1] + xyz[0]/convergenceParam/METERS_PER_DEGREE;
      lle[2] = xyz[2];
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec3d Coordinates::ConvertToRemoteTranslation(const osg::Vec3& translation)
   {
      if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
      {
         mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Converting to remote coordinates.  Local coordinates are %lf, %lf, %lf.",
            translation[0], translation[1], translation[2]);
      }

      osg::Vec3d remoteLoc;

      if (*mLocalCoordinateType == LocalCoordinateType::GLOBE)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            remoteLoc[0] = (translation[0]/GetGlobeRadius())*semiMajorAxis;
            remoteLoc[1] = (translation[1]/GetGlobeRadius())*semiMajorAxis;
            remoteLoc[2] = (translation[2]/GetGlobeRadius())*semiMajorAxis;
         }
         else
         {
            LOGN_ERROR("coordinates.cpp", "With local coordinates in globe mode, only GEOCENTRIC coordinates types are supported.");
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_UTM)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            double lat, lon;
            double x, y, z;

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);

            ConvertUTMToGeodetic(mUTMZone, mUTMHemisphere, translation[0] + localOffset.x(), translation[1] + localOffset.y(), lat, lon);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Outgoing lat lon: %lf, %lf",
                  osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon));
            }

            GeodeticToGeocentric(lat, lon, translation[2] + localOffset.z(), x, y, z);

            remoteLoc[0] = x;
            remoteLoc[1] = y;
            remoteLoc[2] = z;

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            // This code is not yet unit tested
            double lat, lon;

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);

            ConvertUTMToGeodetic(mUTMZone, mUTMHemisphere, translation[0] + localOffset.x(), translation[1] + localOffset.y(), lat, lon);

            remoteLoc[0] = osg::RadiansToDegrees(lat);
            remoteLoc[1] = osg::RadiansToDegrees(lon);
            remoteLoc[2] = translation[2] + localOffset.z();

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);
            remoteLoc[0] = translation[0] + localOffset.x();
            remoteLoc[1] = translation[1] + localOffset.y();
            remoteLoc[2] = translation[2] + localOffset.z();
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_FLAT_EARTH)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            osg::Vec3d lle;
            double x, y, z;

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);

            ConvertFlatEarthToLatLon(lle, osg::Vec3d(translation) + localOffset, mFlatEarthOrigin, mConvergence);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Outgoing lat lon: %lf, %lf",
                  lle[0], lle[1]);
            }

            GeodeticToGeocentric(osg::DegreesToRadians(lle[0]), osg::DegreesToRadians(lle[1]), lle[2], x, y, z);

            remoteLoc[0] = x;
            remoteLoc[1] = y;
            remoteLoc[2] = z;

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            osg::Vec3d lle;

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);

            ConvertFlatEarthToLatLon(lle, osg::Vec3d(translation) + localOffset, mFlatEarthOrigin, mConvergence);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Outgoing lat lon: %lf, %lf",
                  osg::RadiansToDegrees(lle[0]), osg::RadiansToDegrees(lle[1]));
            }

            remoteLoc[0] = lle[0];
            remoteLoc[1] = lle[1];
            remoteLoc[2] = lle[2];

         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
            osg::Vec3d lle;

            osg::Vec3d localOffset;
            GetLocalOffset(localOffset);

            ConvertFlatEarthToLatLon(lle, osg::Vec3d(translation) + localOffset, mFlatEarthOrigin, mConvergence);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Outgoing lat lon: %lf, %lf",
                  osg::RadiansToDegrees(lle[0]), osg::RadiansToDegrees(lle[1]));
            }

            double easting, northing;
            ConvertGeodeticToUTM(lle[0], lle[1], mUTMZone, mUTMHemisphere, easting, northing);
            remoteLoc[0] = easting;
            remoteLoc[1] = northing;
            remoteLoc[2] = lle[2];
         }
      }
      else
      {
         LOGN_ERROR("coordinates.cpp", "Unsupported local coordinate mode: " + mLocalCoordinateType->GetName());
      }

      if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
      {
         mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Converting coordinates.  Resulting coordinates are %f, %f, %f.",
            remoteLoc[0], remoteLoc[1], remoteLoc[2]);
      }

      for (unsigned i = 0; i < 3; ++i)
      {
         if (!IsFinite(remoteLoc[i]))
         {
            remoteLoc[i] = 0.0;
         }
      }

      return remoteLoc;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec3 Coordinates::ConvertToLocalRotation(double psi, double theta, double phi)
   {
      osg::Matrix rotMat;

      if (mRotationDirty)
      {
         ReconfigureRotationMatrix();
      }

      if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
      {
         mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Converting rotation.  Incoming rotations are %lf, %lf, %lf.",
            psi, theta, phi);
      }

      EulersToMatrix(rotMat, psi, theta, phi);

      if (*mLocalCoordinateType == LocalCoordinateType::GLOBE)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            //do nothing
         }
         else
         {
            LOGN_ERROR("coordinates.cpp", "With local coordinates in globe mode, only GEOCENTRIC and GEODETIC coordinates types are supported.");
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_UTM ||
               *mLocalCoordinateType == LocalCoordinateType::CARTESIAN_FLAT_EARTH)
      {
         if (mApplyRotationConversionMatrix)
         {
            rotMat = osg::Matrix::inverse(rotMat) * GetOriginRotationMatrix();
         }

         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            ZFlop(rotMat);
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM
            || *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            //do nothing
         }
      }
      else
      {
         LOGN_ERROR("coordinates.cpp", "Unsupported local coordinate mode: " + mLocalCoordinateType->GetName());
      }

      osg::Vec3 rotation;
      MatrixUtil::MatrixToHpr(rotation, rotMat);

      if (mLogger->IsLevelEnabled(Log::LOG_DEBUG))
      {
         mLogger->LogMessage(Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Converting rotation.  resulting rotations are %f, %f, %f.",
            rotation[0], rotation[1], rotation[2]);
      }

      for (unsigned i = 0; i < 3; ++i)
      {
         if (!IsFinite(rotation[i]))
         {
            rotation[i] = 0.0f;
         }
      }

      return rotation;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec3d Coordinates::ConvertToRemoteRotation(const osg::Vec3& hpr)
   {
      osg::Matrix rotMat;
      MatrixUtil::HprToMatrix(rotMat, hpr);

      if (mRotationDirty)
      {
         ReconfigureRotationMatrix();
      }

      if (*mLocalCoordinateType == LocalCoordinateType::GLOBE)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            //do nothing
         }
         else
         {
            LOGN_ERROR("coordinates.cpp", "With local coordinates in globe mode, only GEOCENTRIC and GEODETIC coordinates types are supported.");
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN_UTM ||
               *mLocalCoordinateType == LocalCoordinateType::CARTESIAN_FLAT_EARTH)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            ZFlop(rotMat);
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM ||
            *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            //Do nothing
         }
         rotMat = osg::Matrix::inverse(rotMat * GetOriginRotationMatrixInverse());
      }
      else
      {
         LOGN_ERROR("coordinates.cpp", "Unsupported local coordinate mode: " + mLocalCoordinateType->GetName());
      }

      osg::Vec3d rotation;
      float psi, theta, phi;
      MatrixToEulers(rotMat, psi, theta, phi);
      rotation[0] = psi;
      rotation[1] = theta;
      rotation[2] = phi;

      for (unsigned i = 0; i < 3; ++i)
      {
         if (!IsFinite(rotation[i]))
         {
            rotation[i] = 0.0f;
         }
      }

      return rotation;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ZFlop(osg::Matrix& toFlop)
   {
      toFlop.set(toFlop(1,0), toFlop(1,1), toFlop(1,2), toFlop(1,3),
                 toFlop(0,0), toFlop(0,1), toFlop(0,2), toFlop(0,3),
                 -toFlop(2,0), -toFlop(2,1), -toFlop(2,2), toFlop(2,3),
                 toFlop(3,0), toFlop(3,1), toFlop(3,2), toFlop(3,3));
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::EulersToMatrix(osg::Matrix& dst, float psi, float theta, float phi)
   {
      float cos_psi    = cosf(psi);
      float sin_psi    = sinf(psi);
      float cos_theta  = cosf(theta);
      float sin_theta  = sinf(theta);
      float cos_phi    = cosf(phi);
      float sin_phi    = sinf(phi);

      dst.makeIdentity();
      dst(0,0) = cos_psi * cos_theta;
      dst(0,1) = - sin_psi * cos_phi + cos_psi * sin_theta * sin_phi;
      dst(0,2) = sin_psi * sin_phi + cos_psi * sin_theta * cos_phi;
      dst(1,0) = sin_psi * cos_theta;
      dst(1,1) = cos_psi * cos_phi + sin_psi * sin_theta * sin_phi;
      dst(1,2) = - cos_psi * sin_phi + sin_psi * sin_theta * cos_phi;
      dst(2,0) = - sin_theta;
      dst(2,1) = cos_theta * sin_phi;
      dst(2,2) = cos_theta * cos_phi;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::MatrixToEulers(const osg::Matrix& src, float& psi, float& theta, float& phi)
   {
      osg::Vec3 coord;
      osg::Matrix mat = src;

      float cos_theta;
      float sq_cos_theta = 1.0 - mat(2,0) * mat(2,0);
      float sin_psi;
      float sin_phi;

      cos_theta = ((sq_cos_theta) < 0.0 ? (0.0) : (sqrtf(sq_cos_theta)));
      if (cos_theta == 0.0) // Singularity here
      {
         cos_theta = 0.000001f;
      }

      sin_psi = mat(1,0) / cos_theta;
      psi = safeASIN(sin_psi);

      if (mat(0,0) < 0.0)
      {
         if (psi < 0.0)
         {
            psi = -osg::PI - (psi);
         }
         else
         {
            psi = osg::PI - (psi);
         }
      }
      theta = -safeASIN(mat(2,0));

      sin_phi = mat(2,1) / cos_theta;
      phi = safeASIN(sin_phi);
      // Correct for quadrant
      if (mat(2,2) < 0.0)
      {
         if (phi < 0.0)
         {
            phi = -osg::PI - (phi);
         }
         else
         {
            phi = osg::PI - (phi);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::GeocentricToGeodetic(double x, double y, double z,
                                          double& latitude, double& longitude,
                                          double& elevation)
   {
      double p = sqrt(x*x + y*y),
        a = semiMajorAxis,
        f = Geocent_f,
        b = a - a*f,
        theta = atan( (z*a)/(p*b) ),
        epsqu = (a*a - b*b)/(b*b),
        esqu = 2.0*f - f*f;

      latitude = atan((z + epsqu * b * pow(sin(theta), 3)) /
                       (p - esqu * a * pow(cos(theta), 3)));

      longitude = atan2(y, x);

      elevation = p/cos(latitude) -
        a/sqrt(1.0-esqu*pow(sin(latitude), 2.0));

      latitude = osg::RadiansToDegrees(latitude);
      longitude = osg::RadiansToDegrees(longitude);
   }


   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertGeodeticToUTM (double Latitude, double Longitude,
                                           unsigned Zone, char Hemisphere, double& Easting, double& Northing)
   {
      double Origin_Latitude = 0.0;
      double Central_Meridian = 0.0;
      double False_Easting = 500000;
      double False_Northing = 0;

      // no errors
      if (Longitude < 0)
      {
         Longitude += (2*osg::PI) + 1.0e-10;
      }

      //char nsZone;
      //CalculateUTMZone(osg::RadiansToDegrees(Latitude), osg::RadiansToDegrees(Longitude), Zone, nsZone);

      if (Zone >= 31)
      {
         Central_Meridian = osg::DegreesToRadians(double(6 * Zone - 183));
      }
      else
      {
         Central_Meridian = osg::DegreesToRadians(double(6 * Zone + 177));
      }

      // If we are projecting in the southern hemisphere, set the false northing.
      if (Hemisphere == 'S' || Hemisphere == 's')
      {
         False_Northing = 10000000;
      }

      UTMParameters params;
      params.CalcTransverseMercatorParameters(Geocent_a, Geocent_f, Origin_Latitude,
                                      Central_Meridian, False_Easting, False_Northing, CentralMeridianScale);
      ConvertGeodeticToTransverseMercator(params, Latitude, Longitude, Easting, Northing);
   } // END OF Convert_Geodetic_To_UTM

   void Coordinates::ConvertUTMToGeodetic (unsigned zone, char hemisphere, double easting, double northing, double& latitude, double& longitude)
   {
      /*
       * PCREES CONTRIBUTION
       * The function Convert_UTM_To_Geodetic converts UTM projection (zone,
       * hemisphere, easting and northing) coordinates to geodetic(latitude
       * and  longitude) coordinates, according to the current ellipsoid
       * parameters.  If any errors occur, the error code(s) are returned
       * by the function, otherwise UTM_NO_ERROR is returned.
       *
       *    Zone              : UTM zone                               (input)
       *    Hemisphere        : North or South hemisphere              (input)
       *    Easting           : Easting (X) in meters                  (input)
       *    Northing          : Northing (Y) in meters                 (input)
       *    Latitude          : Latitude in radians                    (output)
       *    Longitude         : Longitude in radians                   (output)
       */

      double Origin_Latitude = 0;
      double Central_Meridian = 0;
      double False_Easting = 500000;
      double False_Northing = 0;

      if (zone >= 31)
      {
         Central_Meridian = ((6 * zone - 183) * osg::PI / 180.0 /*+ 0.00000005*/);
      }
      else
      {
         Central_Meridian = ((6 * zone + 177) * osg::PI / 180.0 /*+ 0.00000005*/);
      }

      if (hemisphere == 'S' || hemisphere == 's')
      {
         False_Northing = 10000000;
      }

      UTMParameters params;
      params.CalcTransverseMercatorParameters(Geocent_a, Geocent_f, Origin_Latitude,
                                      Central_Meridian, False_Easting, False_Northing, CentralMeridianScale);

      ConvertTransverseMercatorToGeodetic(params, easting,northing,latitude,longitude);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::CalculateUTMZone(double latitude, double longitude, unsigned& ewZone, char& nsZone)
   {
      Clamp(latitude, -80.0, 84.0);

      long Lat_Degrees = long(latitude);
      long Long_Degrees = long(longitude);


      unsigned long zone;

      if (longitude < 180.0)
      {
         zone = long(31 + (longitude / 6.0));
      }
      else
      {
         zone = long((longitude / 6.0) - 29);
      }

      if (zone > 60)
      {
         zone = 1;
      }

      // UTM special cases
      if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > -1)
          && (Long_Degrees < 3))
      {
         zone = 31;
      }
      else if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > 2)
          && (Long_Degrees < 12))
      {
         zone = 32;
      }
      else if ((Lat_Degrees > 71) && (Long_Degrees > -1) && (Long_Degrees < 9))
      {
         zone = 31;
      }
      else if ((Lat_Degrees > 71) && (Long_Degrees > 8) && (Long_Degrees < 21))
      {
         zone = 33;
      }
      else if ((Lat_Degrees > 71) && (Long_Degrees > 20) && (Long_Degrees < 33))
      {
         zone = 35;
      }
      else if ((Lat_Degrees > 71) && (Long_Degrees > 32) && (Long_Degrees < 42))
      {
         zone = 37;
      }

      ewZone = zone;

      // Calculate UTM zone letter (north-south direction)
      if (latitude < 80.0)
      {
         nsZone = 'C' + (char)floor((latitude + 80.0) / 8.0);
         // Skip the letters I and O. They aren't used.
         if (nsZone >= 'I')
         {
            ++nsZone;
         }
         if (nsZone >= 'O')
         {
            ++nsZone;
         }
      }
      else // 80 to 84 are also grid 'X'
      {
         nsZone = 'X';
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string Coordinates::ConvertUTMToMGRS(double easting, double northing, unsigned eastWestZone,
                                                   char northSouthZone, unsigned resolution)
   {
      static long resolutionDivisor[6] = {100000, 10000, 1000, 100, 10, 1};
      static char gridLetters[24] =  {'A', 'B', 'C', 'D', 'E', 'F',
                                      'G', 'H', 'J', 'K', 'L', 'M',
                                      'N', 'P', 'Q', 'R', 'S', 'T',
                                      'U', 'V', 'W', 'X', 'Y', 'Z'};

      // Intermediate parameters needed to build milgrid string
      char    eastingLetter;
      char    northingLetter;
      char    formatString[20];
      char    mgrsString[255];
      double offset;
      int   eastingNum;
      int   northingNum;

      // resolution must be 0-5
      if (resolution > 5)
      {
         throw CoordinateConversionInvalidInput("The resolution for the mgrs conversion must be between 0 and 5 inclusive.", __FILE__, __LINE__);
      }

      // Calculate east-west 100,000 km square grid designator
      // Note that origin repeats every 3 zones (18 degrees)
      // Note that the 8 results from there being 8*3 or 24 letters!
      int index = (((eastWestZone - 1) % 3) * 8) + (int)(easting / 100000) - 1;
      Clamp(index, 0, 23);
      eastingLetter = gridLetters[index];

      // Calculate north-south 100,000 km square grid designator
      // Note: origin alternates between even/odd UTM zones and repeats
      //       every 2,000,000 meters
      if (eastWestZone % 2)
      {
         offset = fmod(northing, 2000000.0);
      }
      else
      {
         offset = fmod(northing + 500000.0, 2000000.0);
      }

      index = (int)(offset / 100000.0);
      Clamp(index, 0, 23);
      northingLetter = gridLetters[index];

      eastingNum  = (((long)easting)  % 100000) / resolutionDivisor[resolution];
      northingNum = (((long)northing) % 100000) / resolutionDivisor[resolution];

      snprintf(formatString, 20, "%%02d%%c%%c%%c%%0%ud%%0%ud", resolution,
              resolution);
      snprintf(mgrsString, 255, formatString, eastWestZone, northSouthZone,
              eastingLetter, northingLetter, eastingNum, northingNum);

      std::string result(mgrsString);
      Trim(result);
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertMGRSToUTM(unsigned defaultZone, char defaultZoneLetter,
                                      const std::string& mgrs,
                                      unsigned& zone, double& easting, double& northing )
   {
      // Numbers used to scale utm to meters
      static long resolutionDivisor[6] = {100000, 10000, 1000, 100, 10, 1};

      unsigned char z_char;

      // Is it too long?
      if ( mgrs.length() > 15 )
      {
         throw CoordinateConversionInvalidInput("The MGRS string must be no longer that 12 digits.", __FILE__, __LINE__);
      }

      std::string working;
      if ((mgrs.length() % 2) != 0)
      {
         if (!(isdigit(static_cast<unsigned char>(mgrs[0])) &&
               isdigit(static_cast<unsigned char>(mgrs[1])) &&
               isalpha(static_cast<unsigned char>(mgrs[2]))))
         {
            throw CoordinateConversionInvalidInput("The string must begin with 2 digits followed by a letter.", __FILE__, __LINE__);
         }

         zone = 10*(mgrs[0] - '0') + (mgrs[1] - '0');
         z_char = mgrs[2];
         working = mgrs.substr(3);
      }
      else
      {
         zone = defaultZone;
         z_char = defaultZoneLetter;
         working = mgrs;
      }

      // Are the first two characters letters?
      if (!(isalpha(static_cast<unsigned char>(working[0])) &&
          isalpha(static_cast<unsigned char>(working[1]))))
      {
         throw CoordinateConversionInvalidInput("The intra-zone grid designations must be letters.", __FILE__, __LINE__);
      }

      // Are the rest of the characters numbers?
      for (unsigned int i = 2; i < working.length(); ++i)
      {
         if (!isdigit(static_cast<unsigned char>(working[i])))
         {
            throw CoordinateConversionInvalidInput("All characters following the zone designations must be digits.", __FILE__, __LINE__);
         }
      }

      // Passed preliminary error checking, go ahead and parse the string

      // calculate the length of each position number
      size_t numLen = (working.length() - 2) / 2;
      char control[20];
      char e_char, n_char;
      int e_num, n_num;
      // Mac OS g++ complains about size_t not being an unsigned int here.
      sprintf(control, "%%c%%c%%%ud%%%ud", unsigned(numLen), unsigned(numLen));
      int sscanf_return = sscanf(working.c_str(), control, &e_char, &n_char, &e_num, &n_num);
      if (sscanf_return != 4)
      {
         throw CoordinateConversionInvalidInput("Internal error when parsing input.  Check input syntax: " + mgrs, __FILE__, __LINE__);
      }

      // The string has passed error checking.

      // convert lower to upper case for leading chars
      if (islower(static_cast<unsigned char>(z_char)))
      {
         z_char = toupper(z_char);
      }
      if (islower(static_cast<unsigned char>(e_char)))
      {
         e_char = toupper(e_char);
      }
      if (islower(static_cast<unsigned char>(n_char)))
      {
         n_char = toupper(n_char);
      }

      e_num *= resolutionDivisor[numLen];
      n_num *= resolutionDivisor[numLen];

      // If we get here, we're using single world-wide datum (WGS84)
      // Calculate northing
      northing = n_char - 'A';
      if (n_char > 'O')
      {
         --northing;
      }
      if (n_char > 'I')
      {
         --northing;
      }

      northing *= 100000;
      northing += n_num;

      if (!(zone % 2))
      {
         northing -= 500000;

         if (northing < 0)
         {
            northing += 2000000;
         }
      }

      // Things get hokey here, but there's no way around it because of the
      // definition of MilGrid. *sigh*
      float deg_base = float(z_char - 'A' - 3);

      if (z_char > 'O')
      {
         --deg_base;
      }

      if (z_char > 'I')
      {
         --deg_base;
      }

      deg_base *= 8.0;
      deg_base -= 72.0;

      if (deg_base >= 0)
      {
         if (deg_base >= 71.64)
         {
            northing += 8000000;
         }
         else if (deg_base >= 53.91)
         {
            if (deg_base > 63 && northing < 1000000)
            {
               northing += 8000000;
            }
            else
            {
               northing += 6000000;
            }
         }
         else if (deg_base >= 36.02)
         {
            if (deg_base > 45 && northing < 1000000)
            {
               northing += 6000000;
            }
            else
            {
               northing += 4000000;
            }
         }
         else if (deg_base >= 18.03)
         {
            if (deg_base > 28 && northing < 1000000)
            {
               northing += 4000000;
            }
            else
            {
               northing += 2000000;
            }
         }
         else if (deg_base > 10 && northing < 1000000)
         {
            northing += 2000000;
         }
      }
      else
      {
         if (deg_base >= -18.03)
         {
            northing += 8000000;
         }
         else if (deg_base >= -36.02)
         {
            if (deg_base > -28 && northing < 1000000)
            {
               northing += 8000000;
            }
            else
            {
               northing += 6000000;
            }
         }
         else if (deg_base >= -53.91)
         {
            if (deg_base > -45 && northing < 1000000)
            {
               northing += 6000000;
            }
            else
            {
               northing += 4000000;
            }
         }
         else if (deg_base >= -71.64)
         {
            if (deg_base > -63.0 && northing < 1000000)
            {
               northing += 4000000;
            }
            else
            {
               northing += 2000000;
            }
         }
         else if (deg_base > -81 && northing < 1000000)
         {
            northing += 2000000;
         }
      }

      // Calculate easting
      easting = e_char - 'A';

      if (e_char > 'O')
      {
         --easting;
      }
      if (e_char > 'I')
      {
         --easting;
      }
      easting++;
      easting -= (((zone - 1) % 3) * 8);
      easting *= 100000;
      easting += e_num;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertGeocentricToGeodetic (double x, double y, double z,
                                           double& phi, double& lambda, double& elevation)
   { // BEGIN Convert_Geocentric_To_Geodetic
     double W;        // distance from z axis
     double W2;       // square of distance from z axis
     double T0;       // initial estimate of vertical component
     double T1;       // corrected estimate of vertical component
     double S0;       // initial estimate of horizontal component
     double S1;       // corrected estimate of horizontal component
     double Sin_B0;   // sin(B0), B0 is estimate of Bowring aux variable
     double Sin3_B0;  // cube of sin(B0)
     double Cos_B0;   // cos(B0)
     double Sin_p1;   // sin(phi1), phi1 is estimated latitude
     double Cos_p1;   // cos(phi1)
     double Rn;       // Earth radius at location
     double Sum;      // numerator of cos(phi1)
     int At_Pole;     // indicates location is in polar region
     double Geocent_b = Geocent_a * (1 - Geocent_f); // Semi-minor axis of ellipsoid, in meters

     At_Pole = 0;
     if (x != 0.0)
     {
        lambda = atan2(y,x);
     }
     else
     {
        if (y > 0)
        {
           lambda = osg::PI_2;
        }
        else if (y < 0)
        {
           lambda = -osg::PI_2;
        }
        else
        {
           At_Pole = 1;
           lambda = 0.0;
           if (z > 0.0)
           {  // north pole
              phi = osg::PI_2;
           }
           else if (z < 0.0)
           {  // south pole
              phi = -osg::PI_2;
           }
           else
           {  // center of earth
              phi = osg::PI_2;
              elevation = -Geocent_b;
              return;
           }
        }
     }
     W2 = x*x + y*y;
     W = sqrt(W2);
     T0 = z * AD_C;
     S0 = sqrt(T0 * T0 + W2);
     Sin_B0 = T0 / S0;
     Cos_B0 = W / S0;
     Sin3_B0 = Sin_B0 * Sin_B0 * Sin_B0;
     T1 = z + Geocent_b * Geocent_ep2 * Sin3_B0;
     Sum = W - Geocent_a * Geocent_e2 * Cos_B0 * Cos_B0 * Cos_B0;
     S1 = sqrt(T1*T1 + Sum * Sum);
     Sin_p1 = T1 / S1;
     Cos_p1 = Sum / S1;
     Rn = Geocent_a / sqrt(1.0 - Geocent_e2 * Sin_p1 * Sin_p1);
     if (Cos_p1 >= COS_67P5)
     {
        elevation = W / Cos_p1 - Rn;
     }
     else if (Cos_p1 <= -COS_67P5)
     {
        elevation = W / -Cos_p1 - Rn;
     }
     else
     {
        elevation = z / Sin_p1 + Rn * (Geocent_e2 - 1.0);
     }
     if (At_Pole == 0)
     {
        phi = atan(Sin_p1 / Cos_p1);
     }
   } // END OF Convert_Geocentric_To_Geodetic

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertGeodeticToTransverseMercator (const UTMParameters& params,
                                                          double Latitude,
                                                          double Longitude,
                                                          double& Easting,
                                                          double& Northing
                                                          )
   {      // BEGIN Convert_Geodetic_To_Transverse_Mercator
      double c;       // Cosine of latitude
      double c2;
      double c3;
      double c5;
      double c7;
      double dlam;    // Delta longitude - Difference in Longitude
      double eta;     // constant - TranMerc_ebs *c *c
      double eta2;
      double eta3;
      double eta4;
      double s;       // Sine of latitude
      double sn;      // Radius of curvature in the prime vertical
      double t;       // Tangent of latitude
      double tan2;
      double tan3;
      double tan4;
      double tan5;
      double tan6;
      double t1;      // Term in coordinate conversion formula - GP to y
      double t2;      // Term in coordinate conversion formula - GP to y
      double t3;      // Term in coordinate conversion formula - GP to y
      double t4;      // Term in coordinate conversion formula - GP to y
      double t5;      // Term in coordinate conversion formula - GP to y
      double t6;      // Term in coordinate conversion formula - GP to y
      double t7;      // Term in coordinate conversion formula - GP to y
      double t8;      // Term in coordinate conversion formula - GP to y
      double t9;      // Term in coordinate conversion formula - GP to y
      double tmd;     // True Meridional distance
      double tmdo;    // True Meridional distance for latitude of origin
      //double temp_Origin;
      //double temp_Long;

      if (Longitude > osg::PI)
      {
         Longitude -= (2 * osg::PI);
      }
      //if ((Longitude < (params.TranMerc_Origin_Long - MAX_DELTA_LONG))
      //    || (Longitude > (params.TranMerc_Origin_Long + MAX_DELTA_LONG)))
      //{
         //if (Longitude < 0)
         //{
         //   temp_Long = Longitude + 2 * osg::PI;
         //}
         //else
         //{
         //   temp_Long = Longitude;
         //}
         //if (params.TranMerc_Origin_Long < 0)
         //{
         //   temp_Origin = params.TranMerc_Origin_Long + 2 * osg::PI;
         //}
         //else
         //{
         //   temp_Origin = params.TranMerc_Origin_Long;
         //}
      //}

      /*
       *  Delta Longitude
       */
      dlam = Longitude - params.TranMerc_Origin_Long;

      if (dlam > osg::PI)
      {
         dlam -= (2 * osg::PI);
      }
      if (dlam < -osg::PI)
      {
         dlam += (2 * osg::PI);
      }
      if (std::abs(dlam) < 2.e-10)
      {
         dlam = 0.0;
      }

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
      eta = params.TranMerc_ebs * c2;
      eta2 = eta * eta;
      eta3 = eta2 * eta;
      eta4 = eta3 * eta;

      // radius of curvature in prime vertical
      sn = params.SPHSN(Latitude);

      // True Meridianal Distances
      tmd = params.SPHTMD(Latitude);

      //  Origin
      tmdo = params.SPHTMD(params.TranMerc_Origin_Lat);

      // northing
      t1 = (tmd - tmdo) * params.TranMerc_Scale_Factor;
      t2 = sn * s * c * params.TranMerc_Scale_Factor / 2.e0;
      t3 = sn * s * c3 * params.TranMerc_Scale_Factor * (5.e0 - tan2 + 9.e0 * eta
                                                  + 4.e0 * eta2) / 24.e0;

      t4 = sn * s * c5 * params.TranMerc_Scale_Factor * (61.e0 - 58.e0 * tan2
                                                  + tan4 + 270.e0 * eta - 330.e0 * tan2 * eta + 445.e0 * eta2
                                                  + 324.e0 * eta3 -680.e0 * tan2 * eta2 + 88.e0 * eta4
                                                  -600.e0 * tan2 * eta3 - 192.e0 * tan2 * eta4) / 720.e0;

      t5 = sn * s * c7 * params.TranMerc_Scale_Factor * (1385.e0 - 3111.e0 *
                                                  tan2 + 543.e0 * tan4 - tan6) / 40320.e0;

      Northing = params.TranMerc_False_Northing + t1 + pow(dlam,2.e0) * t2
        + pow(dlam,4.e0) * t3 + pow(dlam,6.e0) * t4
        + pow(dlam,8.e0) * t5;

      // Easting
      t6 = sn * c * params.TranMerc_Scale_Factor;
      t7 = sn * c3 * params.TranMerc_Scale_Factor * (1.e0 - tan2 + eta) / 6.e0;
      t8 = sn * c5 * params.TranMerc_Scale_Factor * (5.e0 - 18.e0 * tan2 + tan4
                                              + 14.e0 * eta - 58.e0 * tan2 * eta + 13.e0 * eta2 + 4.e0 * eta3
                                              - 64.e0 * tan2 * eta2 - 24.e0 * tan2 * eta3) / 120.e0;
      t9 = sn * c7 * params.TranMerc_Scale_Factor * (61.e0 - 479.e0 * tan2
                                               + 179.e0 * tan4 - tan6) / 5040.e0;

      Easting = params.TranMerc_False_Easting + dlam * t6 + pow(dlam,3.e0) * t7
        + pow(dlam,5.e0) * t8 + pow(dlam,7.e0) * t9;
   } // END OF Convert_Geodetic_To_Transverse_Mercator

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::ConvertTransverseMercatorToGeodetic(const UTMParameters& params,
            double Easting, double Northing,
            double& Latitude, double& Longitude)
   {      // BEGIN Convert_Transverse_Mercator_To_Geodetic

      /*
       * The function Convert_Transverse_Mercator_To_Geodetic converts Transverse
       * Mercator projection (easting and northing) coordinates to geodetic
       * (latitude and longitude) coordinates, according to the current ellipsoid
       * and Transverse Mercator projection parameters.  If any errors occur, the
       * error code(s) are returned by the function, otherwise TRANMERC_NO_ERROR is
       * returned.
       *
       *    Easting       : Easting/X in meters                         (input)
       *    Northing      : Northing/Y in meters                        (input)
       *    Latitude      : Latitude in radians                         (output)
       *    Longitude     : Longitude in radians                        (output)
       */

      double c;       // Cosine of latitude
      double de;      // Delta easting - Difference in Easting (Easting-Fe)
      double dlam;    // Delta longitude - Difference in Longitude
      double eta;     // constant - TranMerc_ebs *c *c
      double eta2;
      double eta3;
      double eta4;
      double ftphi;   // Footpoint latitude
      int    i;       // Loop iterator
      //double s;       // Sine of latitude
      double sn;      // Radius of curvature in the prime vertical
      double sr;      // Radius of curvature in the meridian
      double t;       // Tangent of latitude
      double tan2;
      double tan4;
      double t10;     // Term in coordinate conversion formula - GP to Y
      double t11;     // Term in coordinate conversion formula - GP to Y
      double t12;     // Term in coordinate conversion formula - GP to Y
      double t13;     // Term in coordinate conversion formula - GP to Y
      double t14;     // Term in coordinate conversion formula - GP to Y
      double t15;     // Term in coordinate conversion formula - GP to Y
      double t16;     // Term in coordinate conversion formula - GP to Y
      double t17;     // Term in coordinate conversion formula - GP to Y
      double tmd;     // True Meridional distance
      double tmdo;    // True Meridional distance for latitude of origin

      // True Meridional Distances for latitude of origin
      tmdo = params.SPHTMD(params.TranMerc_Origin_Lat);

      //  Origin
      tmd = tmdo + (Northing - params.TranMerc_False_Northing) / params.TranMerc_Scale_Factor;

      // First Estimate
      sr = params.SPHSR(0.e0);
      ftphi = tmd / sr;

      for (i = 0; i < 5; ++i)
      {
         t10 = params.SPHTMD(ftphi);
         sr = params.SPHSR(ftphi);
         ftphi = ftphi + (tmd - t10) / sr;
      }

      // Radius of Curvature in the meridian
      sr = params.SPHSR(ftphi);

      // Radius of Curvature in the meridian
      sn = params.SPHSN(ftphi);

      // Sine Cosine terms
      //s = sin(ftphi);
      c = cos(ftphi);

      // Tangent Value
      t = tan(ftphi);
      tan2 = t * t;
      tan4 = tan2 * tan2;
      eta = params.TranMerc_ebs * pow(c,2);
      eta2 = eta * eta;
      eta3 = eta2 * eta;
      eta4 = eta3 * eta;
      de = Easting - params.TranMerc_False_Easting;
      if (fabs(de) < 0.0001)
      {
         de = 0.0;
      }

      // Latitude
      t10 = t / (2.e0 * sr * sn * pow(params.TranMerc_Scale_Factor, 2));
      t11 = t * (5.e0  + 3.e0 * tan2 + eta - 4.e0 * pow(eta,2)
               - 9.e0 * tan2 * eta) / (24.e0 * sr * pow(sn,3)
                        * pow(params.TranMerc_Scale_Factor,4));
      t12 = t * (61.e0 + 90.e0 * tan2 + 46.e0 * eta + 45.E0 * tan4
               - 252.e0 * tan2 * eta  - 3.e0 * eta2 + 100.e0
               * eta3 - 66.e0 * tan2 * eta2 - 90.e0 * tan4
               * eta + 88.e0 * eta4 + 225.e0 * tan4 * eta2
               + 84.e0 * tan2* eta3 - 192.e0 * tan2 * eta4)
               / (720.e0 * sr * pow(sn,5) * pow(params.TranMerc_Scale_Factor, 6));
      t13 = t * ( 1385.e0 + 3633.e0 * tan2 + 4095.e0 * tan4 + 1575.e0
               * pow(t,6)) / (40320.e0 * sr * pow(sn,7) * pow(params.TranMerc_Scale_Factor,8));
      Latitude = ftphi - pow(de,2) * t10 + pow(de,4) * t11 - pow(de,6) * t12
      + pow(de,8) * t13;

      t14 = 1.e0 / (sn * c * params.TranMerc_Scale_Factor);

      t15 = (1.e0 + 2.e0 * tan2 + eta) / (6.e0 * pow(sn,3) * c *
               pow(params.TranMerc_Scale_Factor,3));

      t16 = (5.e0 + 6.e0 * eta + 28.e0 * tan2 - 3.e0 * eta2
               + 8.e0 * tan2 * eta + 24.e0 * tan4 - 4.e0
               * eta3 + 4.e0 * tan2 * eta2 + 24.e0
               * tan2 * eta3) / (120.e0 * pow(sn,5) * c
                        * pow(params.TranMerc_Scale_Factor,5));

      t17 = (61.e0 +  662.e0 * tan2 + 1320.e0 * tan4 + 720.e0
               * pow(t,6)) / (5040.e0 * pow(sn,7) * c
                        * pow(params.TranMerc_Scale_Factor,7));

      // Difference in Longitude
      dlam = de * t14 - pow(de,3) * t15 + pow(de,5) * t16 - pow(de,7) * t17;

      // Longitude
      (Longitude) = params.TranMerc_Origin_Long + dlam;

      if((Longitude) > (osg::PI))
      {
         Longitude -= (2 * osg::PI);
      }

   } // END OF Convert_Transverse_Mercator_To_Geodetic

   /////////////////////////////////////////////////////////////////////////////
   void Coordinates::GeodeticToGeocentric(double phi, double lambda, double elevation,
                                          double& x, double& y, double& z)
   {
      double esqu = 2.0 * Geocent_f - Geocent_f*Geocent_f;
      double n = Geocent_a/sqrt(1.0 - esqu * pow(sin(phi), 2.0));

      x = (n + elevation) * cos(phi) * cos(lambda);

      y = (n + elevation) * cos(phi) * sin(lambda);

      z = (n*(1.0-esqu) + elevation) * sin(phi);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int Coordinates::DegreesToMils(const float degrees)
   {
      float positiveDegrees = degrees < 0 ? degrees + 360.0f : degrees;
      positiveDegrees = 360.0f - positiveDegrees;
      unsigned mils = unsigned(positiveDegrees * (6400.0f / 360.0f));
      return mils;// < 0 ? (6400 + mils) : mils;
   }

   /////////////////////////////////////////////////////////////////////////////
   float Coordinates::MilsToDegrees(const unsigned int mils)
   {
      float positiveMils = mils > 6400 ? 6400.0f : float(mils);
      float degrees = float(positiveMils * (360.0f / 6400.0f));
      return 360.0f - degrees;
   }

   /////////////////////////////////////////////////////////////////////////////
   float Coordinates::CalculateMagneticNorthOffset(const float latitude, const float longitude)
   {
      float phi      = osg::DegreesToRadians(latitude);
      float lambda   = osg::DegreesToRadians(longitude);
      float phiMN    = float(osg::DegreesToRadians(MagneticNorthLatitude));
      float lambdaMN = float(osg::DegreesToRadians(MagneticNorthLongitude));

      float ldiff    = lambdaMN - lambda;
      float cosPhiMN = cosf(phiMN);

      return osg::RadiansToDegrees(atan2(cosPhiMN * sinf(ldiff),
         (cosf(phi) * sinf(phiMN) - sinf(phi) * cosPhiMN * cosf(ldiff))));
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string Coordinates::XYZToMGRS(const osg::Vec3& pos)
   {
      // preserve the old value
      const dtUtil::IncomingCoordinateType& oldType = GetIncomingCoordinateType();

      SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
      osg::Vec3d latLonElev = ConvertToRemoteTranslation(pos);

      unsigned ewZone;
      char nsZone;

      CalculateUTMZone(latLonElev[0], latLonElev[1], ewZone, nsZone);

      SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::UTM);
      osg::Vec3d eastingNorthingElev = ConvertToRemoteTranslation(pos);

      std::string milgrid =
         ConvertUTMToMGRS(eastingNorthingElev.x(), eastingNorthingElev.y(), ewZone, nsZone, 5);

      SetIncomingCoordinateType(oldType);

      return milgrid;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Coordinates::ConvertMGRSToXYZ(const std::string& mgrs)
   {
      const dtUtil::IncomingCoordinateType& oldType = GetIncomingCoordinateType();
      unsigned int zone = 0;
      double easting = 0, northing = 0;

      ConvertMGRSToUTM(0,0, mgrs, zone, easting, northing);
      SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::UTM);
      osg::Vec3d tempVec3;
      tempVec3 = ConvertToLocalTranslation(osg::Vec3d(easting, northing, 0));

      SetIncomingCoordinateType(oldType);

      return tempVec3;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CoordinateConversionInvalidInput::CoordinateConversionInvalidInput(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
} // namespace dtUtil
