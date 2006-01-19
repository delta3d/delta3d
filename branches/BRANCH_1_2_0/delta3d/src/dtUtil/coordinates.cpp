#include <dtUtil/matrixutil.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>

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
      mGlobeRadius(0.0)
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
      long zone;
      double easting, northing;      
      char hemisphere;
      
      double lat = osg::DegreesToRadians(latitude);

      //find the central lat and lon for the zone of the lat and lon given.
      ConvertGeodeticToUTM(lat, osg::DegreesToRadians(longitude), 
         zone, hemisphere, easting, northing);

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
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC
         || *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
         {
            const double semiMajorAxis = 6378137.0;
            position[0] = (loc[0]/semiMajorAxis)*GetGlobeRadius();
            position[1] = (loc[1]/semiMajorAxis)*GetGlobeRadius();
            position[2] = (loc[2]/semiMajorAxis)*GetGlobeRadius();
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
            double lat, lon, elevation, easting, northing;      
            long zone;
            char hemisphere;
            
            ConvertGeocentricToGeodetic(loc[0], loc[1], loc[2],lat,lon,elevation);
            ConvertGeodeticToUTM(lat,lon, zone, hemisphere, easting, northing);
            
            double originX,originY,originZ;
            GetOriginLocation(originX,originY,originZ);
            position[0] = easting - originX;
            position[1] = northing - originY;
            position[2] = elevation - originZ;
            
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
      return osg::Vec3d();
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
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC
         || *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
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
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
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
         if (*mIncomingCoordinateType == IncomingCoordinateType::GEOCENTRIC
         || *mIncomingCoordinateType == IncomingCoordinateType::GEODETIC)
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
         else if (*mIncomingCoordinateType == IncomingCoordinateType::UTM)
         {
         }
         rotMat =  osg::Matrix::inverse(rotMat) * GetOriginRotationMatrixInverse();
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
                                           long& Zone, char& Hemisphere, double& Easting, double& Northing)
   {
      long Lat_Degrees;
      long Long_Degrees;
      long temp_zone;
      
      double Origin_Latitude = 0.0;
      double Central_Meridian = 0.0;
      double False_Easting = 500000;
      double False_Northing = 0;
      double Scale = 0.9996;
      
      /* no errors */
      if (Longitude < 0)
        Longitude += (2*osg::PI) + 1.0e-10;
        
      double lat_deg = osg::RadiansToDegrees(Latitude);
      double lon_deg = osg::RadiansToDegrees(Longitude);
      Lat_Degrees = long(lat_deg);
      Long_Degrees = long(lon_deg);
      
      if (Longitude < osg::PI)
        temp_zone = long(31 + (lon_deg / 6.0));
      else
        temp_zone = long((lon_deg / 6.0) - 29);
      
      if (temp_zone > 60)
        temp_zone = 1;

      /* UTM special cases */      
      if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > -1)
          && (Long_Degrees < 3))
        temp_zone = 31;
      else if ((Lat_Degrees > 55) && (Lat_Degrees < 64) && (Long_Degrees > 2)
          && (Long_Degrees < 12))
        temp_zone = 32;
      else if ((Lat_Degrees > 71) && (Long_Degrees > -1) && (Long_Degrees < 9))
        temp_zone = 31;
      else if ((Lat_Degrees > 71) && (Long_Degrees > 8) && (Long_Degrees < 21))
        temp_zone = 33;
      else if ((Lat_Degrees > 71) && (Long_Degrees > 20) && (Long_Degrees < 33))
        temp_zone = 35;
      else if ((Lat_Degrees > 71) && (Long_Degrees > 32) && (Long_Degrees < 42))
        temp_zone = 37;
      
      if (temp_zone >= 31)
        Central_Meridian = osg::DegreesToRadians(double(6 * temp_zone - 183));
      else
        Central_Meridian = osg::DegreesToRadians(double(6 * temp_zone + 177));
      
      Zone = temp_zone;
      
      if (Latitude < 0)
      {
         False_Northing = 10000000;
         Hemisphere = 'S';
      }
      else
         Hemisphere = 'N';
         
      SetTransverseMercatorParameters(UTM_a, UTM_f, Origin_Latitude,
                                      Central_Meridian, False_Easting, False_Northing, Scale);
      ConvertGeodeticToTransverseMercator(Latitude, Longitude, Easting, Northing);
      
   } /* END OF Convert_Geodetic_To_UTM */
  
   void Coordinates::ConvertGeocentricToGeodetic (double X, double Y, double Z, double& Latitude,
                                                  double& Longitude, double& Height)
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
     
     At_Pole = 0;
     if (X != 0.0)
     {
        Longitude = atan2(Y,X);
     }
     else
     {
        if (Y > 0)
        {
           Longitude = osg::PI_2;
        }
        else if (Y < 0)
        {
           Longitude = -osg::PI_2;
        }
        else
        {
           At_Pole = 1;
           Longitude = 0.0;
           if (Z > 0.0)
           {  /* north pole */
              Latitude = osg::PI_2;
           }
           else if (Z < 0.0)
           {  /* south pole */
              Latitude = -osg::PI_2;
           }
           else
           {  /* center of earth */
              Latitude = osg::PI_2;
              Height = -Geocent_b;
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
        Height = W / Cos_p1 - Rn;
     }
     else if (Cos_p1 <= -COS_67P5)
     {
        Height = W / -Cos_p1 - Rn;
     }
     else
     {
        Height = Z / Sin_p1 + Rn * (Geocent_e2 - 1.0);
     }
     if (At_Pole == 0)
     {
        Latitude = atan(Sin_p1 / Cos_p1);
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
  
  
   void Coordinates::GeodeticToGeocentric(double latitude, double longitude, double elevation,
                                          double& x, double& y, double& z)
   {
      double rlatitude = latitude * osg::DegreesToRadians(1.0f),
        rlongitude = longitude * osg::DegreesToRadians(1.0f),
        a = semiMajorAxis,
        f = 1.0/flatteningReciprocal,
        esqu = 2.0*f - f*f,
        n = a/sqrt(1.0-esqu*pow(sin(rlatitude), 2.0));
      
      x = (n + elevation)*cos(rlatitude)*cos(rlongitude);
      
      y = (n + elevation)*cos(rlatitude)*sin(rlongitude);
      
      z = (n*(1.0-esqu) + elevation)*sin(rlatitude);
   }
}
