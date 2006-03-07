#include "dtUtil/matrixutil.h"
#include "dtUtil/coordinates.h"
#include "dtUtil/log.h"
#include "dtUtil/stringutils.h"
#include <dtUtil/deprecationmgr.h>

#include <stdio.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #define snprintf _snprintf
#endif

namespace dtUtil
{     
   IMPLEMENT_ENUM(IncomingCoordinateType);
   const IncomingCoordinateType IncomingCoordinateType::GEOCENTRIC("Geocentric");
   const IncomingCoordinateType IncomingCoordinateType::GEODETIC("Geodetic");
   const IncomingCoordinateType IncomingCoordinateType::UTM("UTM");

   IMPLEMENT_ENUM(LocalCoordinateType);
   const LocalCoordinateType LocalCoordinateType::GLOBE("World Coordinate");
   const LocalCoordinateType LocalCoordinateType::CARTESIAN("Euler Angle");


   Coordinates::Coordinates(): mLocalCoordinateType(&LocalCoordinateType::CARTESIAN), 
      mIncomingCoordinateType(&IncomingCoordinateType::UTM), 
      mZone(1), mGlobeRadius(0.0)
   {
      mLogger = &Log::GetInstance("coordinates.cpp");
      
      mLocationOffset[0] = 0.0;
      mLocationOffset[1] = 0.0;
      mLocationOffset[2] = 0.0;

      mRotationOffset.makeIdentity();
      mRotationOffsetInverse.makeIdentity();
            
      TranMerc_a = 6378137.0;         /* Semi-major axis of ellipsoid i meters */
      TranMerc_es = 0.0066943799901413800; /* Eccentricity (0.08181919084262188000) squared */
      
      /* Isometeric to geodetic latitude parameters, default to WGS 84 */
      TranMerc_ap = 6367449.1458008;
      TranMerc_bp = 16038.508696861;
      TranMerc_cp = 16.832613334334;
      TranMerc_dp = 0.021984404273757;
      TranMerc_ep = 3.1148371319283e-005;
   
      /* Transverse_Mercator projection Parameters */
      TranMerc_Origin_Lat = 0.0;           /* Latitude of origin in radians */
      TranMerc_Origin_Long = 0.0;          /* Longitude of origin in radians */
      TranMerc_False_Northing = 0.0;       /* False northing in meters */
      TranMerc_False_Easting = 0.0;        /* False easting in meters */
      TranMerc_Scale_Factor = 1.0;         /* Scale factor  */
      
      /* Maximum variance for easting and northing values for WGS 84. */
      TranMerc_Delta_Easting = 40000000.0;
      TranMerc_Delta_Northing = 40000000.0;
   
      /* Ellipsoid Parameters, default to WGS 84  */
      TranMerc_f = Geocent_f;      /* Flattening of ellipsoid  */
      TranMerc_ebs = 0.0067394967565869;   /* Second Eccentricity squared */
   }
   
   Coordinates::~Coordinates()
   {  
   }

   Coordinates& Coordinates::operator = (const Coordinates &rhs) 
   {
      if(this == &rhs)
         return *this;

      mLogger                 = rhs.mLogger;
      mLocalCoordinateType    = rhs.mLocalCoordinateType;
      mIncomingCoordinateType = rhs.mIncomingCoordinateType;
      TranMerc_a              = rhs.TranMerc_a;
      TranMerc_es             = rhs.TranMerc_es;
      TranMerc_ap             = rhs.TranMerc_ap;
      TranMerc_bp             = rhs.TranMerc_bp;
      TranMerc_cp             = rhs.TranMerc_cp;
      TranMerc_dp             = rhs.TranMerc_dp;
      TranMerc_ep             = rhs.TranMerc_ep;
      TranMerc_Origin_Lat     = rhs.TranMerc_Origin_Lat;
      TranMerc_Origin_Long    = rhs.TranMerc_Origin_Long;
      TranMerc_False_Northing = rhs.TranMerc_False_Northing;
      TranMerc_False_Easting  = rhs.TranMerc_False_Easting;
      TranMerc_Scale_Factor   = rhs.TranMerc_Scale_Factor;
      TranMerc_Delta_Easting  = rhs.TranMerc_Delta_Easting;
      TranMerc_Delta_Northing = rhs.TranMerc_Delta_Northing;
      TranMerc_f              = rhs.TranMerc_f;
      TranMerc_ebs            = rhs.TranMerc_ebs;
      mZone                   = rhs.mZone;
      mHemisphere             = rhs.mHemisphere;
      mEasting                = rhs.mEasting;
      mNorthing               = rhs.mNorthing;
      mGlobeRadius            = rhs.mGlobeRadius;

      memcpy(mLocationOffset, rhs.mLocationOffset, sizeof(double) * 3);

      mRotationOffset         = rhs.mRotationOffset;
      mRotationOffsetInverse  = rhs.mRotationOffsetInverse;

      return *this;
   }

   bool Coordinates::operator == (const Coordinates &rhs)
   {
      if(mLogger != rhs.mLogger)
         return false;
      if(mLocalCoordinateType != rhs.mLocalCoordinateType)
         return false;
      if(mIncomingCoordinateType != rhs.mIncomingCoordinateType)
         return false;
      if(TranMerc_a != rhs.TranMerc_a)
         return false;
      if(TranMerc_es != rhs.TranMerc_es)
         return false;
      if(TranMerc_ap != rhs.TranMerc_ap)
         return false;
      if(TranMerc_bp != rhs.TranMerc_bp)
         return false;
      if(TranMerc_cp != rhs.TranMerc_cp)
         return false;
      if(TranMerc_dp != rhs.TranMerc_dp)
         return false;
      if(TranMerc_ep != rhs.TranMerc_ep)
         return false;
      if(TranMerc_Origin_Lat != rhs.TranMerc_Origin_Lat)
         return false;
      if(TranMerc_Origin_Long != rhs.TranMerc_Origin_Long)
         return false;
      if(TranMerc_False_Northing != rhs.TranMerc_False_Northing)
         return false;
      if(TranMerc_False_Easting != rhs.TranMerc_False_Easting)
         return false;
      if(TranMerc_Scale_Factor != rhs.TranMerc_Scale_Factor)
         return false;
      if(TranMerc_Delta_Easting != rhs.TranMerc_Delta_Easting)
         return false;
      if(TranMerc_Delta_Northing != rhs.TranMerc_Delta_Northing)
         return false;
      if(TranMerc_f != rhs.TranMerc_f)
         return false;
      if(TranMerc_ebs != rhs.TranMerc_ebs)
         return false;
      if(mZone != rhs.mZone)
         return false;
      if(mHemisphere != rhs.mHemisphere)
         return false;
      if(mEasting != rhs.mEasting)
         return false;
      if(mNorthing != rhs.mNorthing)
         return false;
      if(mGlobeRadius != rhs.mGlobeRadius)
         return false;

      if(mLocationOffset[0] != rhs.mLocationOffset[0] || 
         mLocationOffset[1] != rhs.mLocationOffset[1] ||
         mLocationOffset[2] != rhs.mLocationOffset[2])
         return false;

      if(mRotationOffset != rhs.mRotationOffset)
         return false;
      if(mRotationOffsetInverse != rhs.mRotationOffsetInverse)
         return false;

      return true;
   }
  
   void Coordinates::SetGeoOrigin(double latitude, double longitude, double elevation)
   {
      GeodeticToGeocentric(latitude, 
                           longitude, 
                           elevation,
                           mLocationOffset[0],
                           mLocationOffset[1],
                           mLocationOffset[2]);      
   }
  
   void Coordinates::SetOriginLocation(double x, double y, double z)
   {
      mLocationOffset[0] = x;
      mLocationOffset[1] = y;
      mLocationOffset[2] = z;
   }
  
   void Coordinates::GetOriginLocation(double& x, double& y, double& z) const
   {
      x = mLocationOffset[0];
      y = mLocationOffset[1];
      z = mLocationOffset[2];
   }

   void Coordinates::SetGeoOriginRotation(double latitude, double longitude)
   {
      unsigned long zone;
      double easting, northing;      
      char hemisphere;
      
      double lat = osg::DegreesToRadians(latitude);

      //find the central lat and lon for the zone of the lat and lon given.
      ConvertGeodeticToUTM(lat, osg::DegreesToRadians(longitude), 
         zone, hemisphere, easting, northing);
      
      SetUTMZone(zone);

      double lon = TranMerc_Origin_Long;

      double sin_lat = sin(lat);
      double cos_lat = cos(lat);
      double sin_lon = sin(lon);
      double cos_lon = cos(lon);

      /* This matrix is used to convert from GCS to GCC.
       * position_gcc = matrix * position_gcs + position_origin_in_gcc
       */
      mRotationOffset.makeIdentity();
      
      mRotationOffset(0,0) = -sin_lon;
      mRotationOffset(0,1) = -sin_lat * cos_lon;
      mRotationOffset(0,2) =  cos_lat * cos_lon;
      mRotationOffset(1,0) =  cos_lon;
      mRotationOffset(1,1) = -sin_lat * sin_lon;
      mRotationOffset(1,2) =  cos_lat * sin_lon;
      mRotationOffset(2,0) =  0;
      mRotationOffset(2,1) =  cos_lat;
      mRotationOffset(2,2) =  sin_lat;

      mRotationOffsetInverse.invert(mRotationOffset);
   }
  
   void Coordinates::SetOriginRotation(float h, float p, float r)
   {
     dtUtil::MatrixUtil::HprToMatrix(mRotationOffset, osg::Vec3(h, p, r)); 
     
     mRotationOffsetInverse.invert(mRotationOffset);
   }
  
   void Coordinates::GetOriginRotation(float& h, float& p, float& r) const
   {
      osg::Vec3 tmp;
      dtUtil::MatrixUtil::MatrixToHpr(tmp, mRotationOffset);
      h = tmp[0]; 
      p = tmp[1]; 
      r = tmp[2];
   }
   
   const osg::Matrix& Coordinates::GetOriginRotationMatrix() const
   {
      return mRotationOffset;
   }
   
   const osg::Matrix& Coordinates::GetOriginRotationMatrixInverse() const
   {
      return mRotationOffsetInverse;
   }
  
   void Coordinates::SetGlobeRadius(float radius)
   {
      mGlobeRadius = radius;
   }
  
   float Coordinates::GetGlobeRadius() const
   {
      return mGlobeRadius;
   }

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
            position[0] = (loc[0]/semiMajorAxis)*GetGlobeRadius();
            position[1] = (loc[1]/semiMajorAxis)*GetGlobeRadius();
            position[2] = (loc[2]/semiMajorAxis)*GetGlobeRadius();
         }
         else
         {
            LOGN_ERROR("coordinates.cpp", "With local coordinates in globe mode, only GEOCENTRIC coordinates types are supported.");
         }
      }
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            double lat, lon, elevation, easting, northing;      
            unsigned long zone;
            char hemisphere;
            
            ConvertGeocentricToGeodetic(loc[0], loc[1], loc[2],lat,lon,elevation);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Incoming lat lon: %lf, %lf", 
                  osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon));

            ConvertGeodeticToUTM(lat,lon, zone, hemisphere, easting, northing);
            
            double originX,originY,originZ;
            GetOriginLocation(originX,originY,originZ);
            position[0] = easting - originX;
            position[1] = northing - originY;
            position[2] = elevation - originZ;
            
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            //This code is not yet unit tested
            double easting, northing;      
            unsigned long zone;
            char hemisphere;
            
            ConvertGeodeticToUTM(osg::DegreesToRadians(loc[0]), osg::DegreesToRadians(loc[1]), zone, hemisphere, easting, northing);
            
            double originX,originY,originZ;
            GetOriginLocation(originX,originY,originZ);
            position[0] = easting - originX;
            position[1] = northing - originY;
            position[2] = loc[2] - originZ;
            
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
            double originX, originY, originZ;
            GetOriginLocation(originX, originY, originZ);
            position[0] = loc[0] - originX;
            position[1] = loc[1] - originY;
            position[2] = loc[2] - originZ;
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
            position[0], position[1], position[2]);
      }
      return position; 
      
   }

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
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN)
      {
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC)
         {
            double lat, lon;
            double x, y, z;     

            double originX,originY,originZ;
            GetOriginLocation(originX,originY,originZ);
            
            ConvertUTMToGeodetic(mZone, translation[0] + originX, translation[1] + originY, lat, lon);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Outgoing lat lon: %lf, %lf", 
                  osg::RadiansToDegrees(lat), osg::RadiansToDegrees(lon));

            GeodeticToGeocentric(lat, lon, translation[2] + originZ, x, y, z);
            
            remoteLoc[0] = x;
            remoteLoc[1] = y;
            remoteLoc[2] = z;
            
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            //This code is not yet unit tested
            double lat, lon;

            double originX,originY,originZ;
            GetOriginLocation(originX,originY,originZ);
            
            ConvertUTMToGeodetic(mZone, translation[0] + originX, translation[1] + originY, lat, lon);
            
            remoteLoc[0] = osg::RadiansToDegrees(lat);
            remoteLoc[1] = osg::RadiansToDegrees(lon);
            remoteLoc[2] = translation[2] + originZ;
            
         }
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
            double originX, originY, originZ;
            GetOriginLocation(originX, originY, originZ);
            remoteLoc[0] = translation[0] + originX;
            remoteLoc[1] = translation[1] + originY;
            remoteLoc[2] = translation[2] + originZ;
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
      return remoteLoc; 
   }
  
   const osg::Vec3 Coordinates::ConvertToLocalRotation(double psi, double theta, double phi) const
   {
      osg::Matrix rotMat;

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
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN)
      {
         rotMat = osg::Matrix::inverse(rotMat) * GetOriginRotationMatrix();

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
      return rotation;
   }

   const osg::Vec3d Coordinates::ConvertToRemoteRotation(const osg::Vec3& hpr) const
   {
      osg::Matrix rotMat;
      MatrixUtil::HprToMatrix(rotMat, hpr);
            
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
      else if (*mLocalCoordinateType == LocalCoordinateType::CARTESIAN)
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
      return rotation;
   }
  
   void Coordinates::ZFlop(osg::Matrix& toFlop)
   {
      toFlop.set(toFlop(1,0), toFlop(1,1), toFlop(1,2), toFlop(1,3), 
                 toFlop(0,0), toFlop(0,1), toFlop(0,2), toFlop(0,3),
                 -toFlop(2,0), -toFlop(2,1), -toFlop(2,2), toFlop(2,3),
                 toFlop(3,0), toFlop(3,1), toFlop(3,2), toFlop(3,3));
   }
  
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
  
   void Coordinates::MatrixToEulers(const osg::Matrix& src, float& psi, float& theta, float& phi)
   {
      osg::Vec3 coord;
      osg::Matrix mat = src;

      float cos_theta;
      float sq_cos_theta = 1.0 - mat(2,0) * mat(2,0);
      float sin_psi;
      float sin_phi;
      
      cos_theta = ((sq_cos_theta) < 0.0 ? (0.0) : (sqrtf(sq_cos_theta)));
      if (cos_theta == 0.0) /* Singularity here */
        cos_theta = 0.000001;
      
      sin_psi = mat(1,0) / cos_theta;
      psi = safeASIN(sin_psi);
      
      if (mat(0,0) < 0.0)
      {
          if (psi < 0.0)
            psi = -osg::PI - (psi);
          else
            psi = osg::PI - (psi);
      }
      theta = -safeASIN(mat(2,0));
      
      sin_phi = mat(2,1) / cos_theta;
      phi = safeASIN(sin_phi);
      /* Correct for quadrant */
      if (mat(2,2) < 0.0)
      {
          if (phi < 0.0)
            phi = -osg::PI - (phi);
          else
            phi = osg::PI - (phi);
      }
      
   }
  
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
  
   void Coordinates::ConvertGeodeticToUTM (double Latitude, double Longitude,
                                           unsigned long& Zone, char& Hemisphere, double& Easting, double& Northing)
   {
      double Origin_Latitude = 0.0;
      double Central_Meridian = 0.0;
      double False_Easting = 500000;
      double False_Northing = 0;
      
      /* no errors */
      if (Longitude < 0)
        Longitude += (2*osg::PI) + 1.0e-10;
        
      char nsZone;
      CalculateUTMZone(osg::RadiansToDegrees(Latitude), osg::RadiansToDegrees(Longitude), Zone, nsZone);

      if (Zone >= 31)
        Central_Meridian = osg::DegreesToRadians(double(6 * Zone - 183));
      else
        Central_Meridian = osg::DegreesToRadians(double(6 * Zone + 177));
      
      if (Latitude < 0)
      {
         False_Northing = 10000000;
         Hemisphere = 'S';
      }
      else
         Hemisphere = 'N';
         
      SetTransverseMercatorParameters(Geocent_a, Geocent_f, Origin_Latitude,
                                      Central_Meridian, False_Easting, False_Northing, CentralMeridianScale);
      ConvertGeodeticToTransverseMercator(Latitude, Longitude, Easting, Northing);
      
   } /* END OF Convert_Geodetic_To_UTM */

   void Coordinates::ConvertGeodeticToUTM (double Latitude, double Longitude,
      long& Zone, char& Hemisphere, double& Easting, double& Northing)
   {
      DEPRECATE(  "void Coordinates::ConvertGeodeticToUTM (double Latitude, double Longitude, long& Zone, char& Hemisphere, double& Easting, double& Northing)",
                  "void Coordinates::ConvertGeodeticToUTM (double Latitude, double Longitude, unsigned long& Zone, char& Hemisphere, double& Easting, double& Northing)")

      unsigned long tempZone;
      ConvertGeodeticToUTM(Latitude,Longitude,tempZone,Hemisphere,Easting,Northing);
      Zone = tempZone;
   }
  
   void Coordinates::ConvertUTMToGeodetic (long zone, double easting, double northing, double& latitude, double& longitude)
   {
      double phai;               /* resulting latitude in radians           */
      double lamda;              /* resutling longitude in radians          */
      double phai1;              /* footprint latitude                      */
      double lamda0;             /* longitude of central meridian           */
      double mu,M,N,R,T,C,D;     /* basic parameters used for calcualtaions */
      double cos_2mu;
      double cos_phai1;          /* cosine of latitude                      */
      double sin_phai1;          /* sine of latitude                        */
      double tan_phai1;          /* tangent of latitude                     */
      
      /* Equations are taken from "Map Projections - A Working Manual" by
       * John Snyder, USGS Professional Paper 1395, Pages 63 and 64.
       */
    
      double P1 =  1.0 - (Geocent_e2/4.0) - (3.0*Geocent_e2_2/64.0) - (5.0*Geocent_e2_3/256.0);
      double P2 = (3.0 * Geocent_ef/2.0) - (27.0 * Geocent_ef_3/32.0);
      double P3 = (21.0 * Geocent_ef*Geocent_ef / 16.0 ) - (55.0 * Geocent_ef_4 / 32.0); 
      double P4 = 151.0 * Geocent_ef_3 / 96.0;
      double P5 = 1097.0 * Geocent_ef_4 / 512.0;

      // easting minus false easting
      double adj_easting = easting - 500000.0;

      // northing minus false northing
      double adj_northing;
      if (northing > 5000000)
         adj_northing = northing - 10000000.0;
      else
         adj_northing = northing;

      /* Calculate longitude of the central meridian */
      if ( zone < 31 )
         lamda0 = double( ( zone * 6 ) - 183);
      else
         lamda0 = double( ( ( zone - 31 ) * 6 ) + 3 );

      /* convert to radians */
      lamda0 = osg::DegreesToRadians(lamda0);

      /* calculate true distance along central meridian from equator to phi */
      M = adj_northing / CentralMeridianScale;

      /* calculate the footprint latitude, phai1 */
      mu = M / (Geocent_a * P1);

      cos_2mu = cos(2.0*mu);
      phai1 = mu + (sin(2.0*mu) *
        ((P2-P4) +
         cos_2mu * ((2.0*P3-4.0*P5) +
               cos_2mu * (4.0*P4 +
                cos_2mu * 8.0*P5))));

      /* calculate the basic parameters used calculations */
      cos_phai1 = cos(phai1);
      sin_phai1 = sin(phai1);
      tan_phai1 = tan(phai1);
      C = Geocent_ep2 * cos_phai1 * cos_phai1;
      T = tan_phai1 * tan_phai1;
      N = Geocent_a / sqrt( 1.0 - (Geocent_e2 * sin_phai1 * sin_phai1));
      P1 = sin_phai1 * sin_phai1;
      R = ( Geocent_a * ( 1 - Geocent_e2 ) ) /
        ( (1 - Geocent_e2*P1) * sqrt(1 - Geocent_e2*P1) );
      D =  adj_easting / ( N * CentralMeridianScale);

      /* calculate various derived parameters from squaring, cubing, etc. */
      double D2 = D * D ;
      double D3 = D2 * D;
      double D4 = D3 * D;
      double D5 = D4 * D;
      double D6 = D5 * D;
      double C2 =  C * C;
      double T2 =  T * T;

      /* calculate the latitude, phai */
      P1 =   D2/2.0;
      P2 = ( D4/24.0  ) * ( 5.0 + 3.0*T + 10.0*C - 4.0*C2 - 9.0*Geocent_ep2) ;
      P3 = ( D6/720.0 ) * (61.0 + 90.0*T + 298.0*C + 45.0*T2 - 
          252.0 * Geocent_ep2 - 3.0 * C2 );
      phai = phai1 - ( N * tan_phai1 / R ) * ( P1 - P2 + P3 );

      /* calculate the longitude, lambda */
      P1 = ( 1.0 + 2.0*T + C ) * D3 / 6.0;
      P2 = ( 5.0 - 2.0*C + 28.0*T - 3.0*C2 + 8.0*Geocent_ep2 + 24.0*T2 ) * D5 / 120.0;
      lamda = lamda0 + ((D - P1 + P2) / cos_phai1);

      latitude = phai;
      longitude = lamda;
      
   }

   void Coordinates::CalculateUTMZone(double latitude, double longitude, unsigned long& ewZone, char& nsZone)
   {
      CLAMP(latitude, -80.0, 84.0);

      long Lat_Degrees = long(latitude);
      long Long_Degrees = long(longitude);

      
      unsigned long zone;
      
      if (longitude < 180.0)
        zone = long(31 + (longitude / 6.0));
      else
        zone = long((longitude / 6.0) - 29);
      
      if (zone > 60)
        zone = 1;

      /* UTM special cases */      
      if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > -1)
          && (Long_Degrees < 3))
        zone = 31;
      else if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > 2)
          && (Long_Degrees < 12))
        zone = 32;
      else if ((Lat_Degrees > 71) && (Long_Degrees > -1) && (Long_Degrees < 9))
        zone = 31;
      else if ((Lat_Degrees > 71) && (Long_Degrees > 8) && (Long_Degrees < 21))
        zone = 33;
      else if ((Lat_Degrees > 71) && (Long_Degrees > 20) && (Long_Degrees < 33))
        zone = 35;
      else if ((Lat_Degrees > 71) && (Long_Degrees > 32) && (Long_Degrees < 42))
        zone = 37;
        
      ewZone = zone;
      

      /* Calculate UTM zone letter (north-south direction) */
      if (latitude < 80.0)
      {
         nsZone = 'C' + (char)floor((latitude + 80.0) / 8.0);
         //Skip the letters I and O. They isn't used.
         if (nsZone >= 'I')
            nsZone ++;
         if (nsZone >= 'O')
            nsZone ++;
      }
      else /* 80 to 84 are also grid 'X' */
         nsZone = 'X';
            
   }
  
   void Coordinates::ConvertGeocentricToGeodetic (double x, double y, double z,
                                           double& phi, double& lambda, double& elevation)

   { /* BEGIN Convert_Geocentric_To_Geodetic */
     
     double W;        /* distance from z axis */
     double W2;       /* square of distance from z axis */
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
           {  /* north pole */
              phi = osg::PI_2;
           }
           else if (z < 0.0)
           {  /* south pole */
              phi = -osg::PI_2;
           }
           else
           {  /* center of earth */
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
   } /* END OF Convert_Geocentric_To_Geodetic */
  
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
                                          TranMerc_Delta_Easting,
                                          TranMerc_Delta_Northing);
      ConvertGeodeticToTransverseMercator(0,
                                          MAX_DELTA_LONG,
                                          TranMerc_Delta_Easting,
                                          dummy_northing);
      TranMerc_Origin_Lat = Origin_Latitude;
      if (Central_Meridian > osg::PI)
        Central_Meridian -= (2*osg::PI);
      TranMerc_Origin_Long = Central_Meridian;
      TranMerc_False_Northing = False_Northing;
      TranMerc_False_Easting = False_Easting; 
      TranMerc_Scale_Factor = Scale_Factor;
   }  /* END of Set_Transverse_Mercator_Parameters  */
  
   void Coordinates::ConvertGeodeticToTransverseMercator (double Latitude,
                                                          double Longitude,
                                                          double& Easting,
                                                          double& Northing) const
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
      double t1;      /* Term in coordinate conversion formula - GP to y */
      double t2;      /* Term in coordinate conversion formula - GP to y */
      double t3;      /* Term in coordinate conversion formula - GP to y */
      double t4;      /* Term in coordinate conversion formula - GP to y */
      double t5;      /* Term in coordinate conversion formula - GP to y */
      double t6;      /* Term in coordinate conversion formula - GP to y */
      double t7;      /* Term in coordinate conversion formula - GP to y */
      double t8;      /* Term in coordinate conversion formula - GP to y */
      double t9;      /* Term in coordinate conversion formula - GP to y */
      double tmd;     /* True Meridional distance                        */
      double tmdo;    /* True Meridional distance for latitude of origin */
      double temp_Origin;
      double temp_Long;
      
      if (Longitude > osg::PI)
        Longitude -= (2 * osg::PI);
      if ((Longitude < (TranMerc_Origin_Long - MAX_DELTA_LONG))
          || (Longitude > (TranMerc_Origin_Long + MAX_DELTA_LONG)))
      {
         if (Longitude < 0)
           temp_Long = Longitude + 2 * osg::PI;
         else
           temp_Long = Longitude;
         if (TranMerc_Origin_Long < 0)
           temp_Origin = TranMerc_Origin_Long + 2 * osg::PI;
         else
           temp_Origin = TranMerc_Origin_Long;
      }
      
      /* 
       *  Delta Longitude
       */
      dlam = Longitude - TranMerc_Origin_Long;
      
      if (dlam > osg::PI)
        dlam -= (2 * osg::PI);
      if (dlam < -osg::PI)
        dlam += (2 * osg::PI);
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
      
      Northing = TranMerc_False_Northing + t1 + pow(dlam,2.e0) * t2
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
      
      Easting = TranMerc_False_Easting + dlam * t6 + pow(dlam,3.e0) * t7 
        + pow(dlam,5.e0) * t8 + pow(dlam,7.e0) * t9;
      
   } /* END OF Convert_Geodetic_To_Transverse_Mercator */
  
  
   void Coordinates::GeodeticToGeocentric(double phi, double lambda, double elevation,
                                          double& x, double& y, double& z)
   {
      double esqu = 2.0*Geocent_f - Geocent_f*Geocent_f;
      double n = Geocent_a/sqrt(1.0-esqu*pow(sin(phi), 2.0));
      
      x = (n + elevation)*cos(phi)*cos(lambda);
      
      y = (n + elevation)*cos(phi)*sin(lambda);
      
      z = (n*(1.0-esqu) + elevation)*sin(phi);
   }

   unsigned int Coordinates::DegreesToMils(const float degrees)
   {
      float positiveDegrees = degrees < 0 ? degrees + 360.0f : degrees;
      positiveDegrees = 360.0f - positiveDegrees;
      unsigned mils = unsigned(positiveDegrees * (6400.0f / 360.0f));
      return mils;// < 0 ? (6400 + mils) : mils;
   }

   float Coordinates::MilsToDegrees(const unsigned int mils)
   {
      float positiveMils = mils > 6400 ? 6400.0f : float(mils);
      float degrees = float(positiveMils * (360.0f / 6400.0f));
      return 360.0f - degrees;
   }
}
