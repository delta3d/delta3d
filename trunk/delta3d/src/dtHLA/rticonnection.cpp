// rticonnection.cpp: Implementation of the RTIConnection class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <memory.h>
#include <stdlib.h>

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include <sys/socket.h>
#include <linux/in.h>
#endif

#include "ul.h"
#include "sg.h"
#include "tinyxml.h"

#include <osg/Material>
#include <osg/StateSet>

#include "osgDB/FileUtils"

#include "osgUtil/IntersectVisitor"

#include "dtCore/notify.h"
#include "dtHLA/rticonnection.h"
#include "dtCore/system.h"

using namespace dtCore;
using namespace dtHLA;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(RTIConnection)


/**
 * The heartbeat interval, in seconds.
 */
const double heartbeatInterval = 1.0;

/**
 * The length of the semi-major axis, in meters (WGS 84).
 */
const double semiMajorAxis = 6378137.0;

/**
 * The reciprocal of the flattening parameter (WGS 84).
 */
const double flatteningReciprocal = 298.257223563;

/**
 * Flags nodes as entities, which should not be included in the ground clamping
 * intersection test.
 */
const osg::Node::NodeMask entityMask = 0x01;


/**
 * Constructor.
 *
 * @param name the instance name
 */
RTIConnection::RTIConnection(string name)
   : Base(name),
     mScene(NULL),
     mEffectManager(NULL),
     mGlobeModeEnabled(false),
     mUTMModeEnabled(false),
     mGlobeRadius(100.0f),
     mGroundClampMode(NO_CLAMP),
     mEffectClampMode(true),
     mLocalIPAddress(0x7F000001)
{
   RegisterInstance(this);
   
   AddSender(System::GetSystem());

   SetGeoOrigin(0, 0, 0);

   mSiteIdentifier = (unsigned short)(1 + (rand() % 65535));
   mApplicationIdentifier = (unsigned short)(1 + (rand() % 65535)); 
  
   SOCKET some_socket = socket(AF_INET, SOCK_DGRAM, 0);
   
   //
   // Code from http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1047083789&id=1045780608
   //

   int len;
   sockaddr_in other, me;

   memset(&other, 0, sizeof(other));

   other.sin_family = AF_INET;
   other.sin_port = 8192;

   #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   other.sin_addr.S_un.S_addr = 0x7F000001;
   #else
   other.sin_addr.s_addr = 0x7F000001;
   #endif
   
   if(connect(some_socket, (sockaddr*)&other, sizeof(other)) == 0)
   {

      if(getsockname(some_socket, (sockaddr*)&me, (socklen_t*)&len) == 0)
      {
         #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
         mLocalIPAddress = me.sin_addr.S_un.S_addr;
         #else
         mLocalIPAddress = me.sin_addr.s_addr;
         #endif
         
         if(ulIsLittleEndian)
         {
            ulEndianSwap(&mLocalIPAddress);
         }

         #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
         mSiteIdentifier = me.sin_addr.S_un.S_un_w.s_w1;
         #else
         win_addr temp_addr;
         memcpy(&temp_addr,&me.sin_addr,sizeof(win_addr));
         mSiteIdentifier = temp_addr.S_un.S_un_w.s_w1;
         #endif
      }
   }
}

/**
 * Destructor.
 */
RTIConnection::~RTIConnection()
throw (RTI::FederateInternalError)        
{
   for(  std::map<RTI::ObjectHandle, GhostData>::iterator it = 
         mObjectHandleGhostDataMap.begin();
         it != mObjectHandleGhostDataMap.end();
         it++ )
   {
      mScene->RemoveDrawable( (*it).second.mEntity.get() ); 
   }
   mObjectHandleGhostDataMap.clear();

   RemoveSender(System::GetSystem());
   
   DeregisterInstance(this);
} 

/**
 * Creates/joins a federation execution.
 *
 * @param executionName the name of the federation execution to join
 * @param fedFilename the fed filename
 * @param federateName the name of this federate
 */
void RTIConnection::JoinFederationExecution(string executionName,
                                            string fedFilename,
                                            string federateName)
{
   try
   {
      mRTIAmbassador.createFederationExecution(
         executionName.c_str(),
         osgDB::findDataFile(fedFilename).c_str()
      );
   }
   catch(RTI::FederationExecutionAlreadyExists feae)
   {}
   
   mIgnoreEffect = false;
   mEntityIdentifierCounter = 1;
   mEventIdentifierCounter = 1;
   
   mObjectHandleMasterDataMap.clear();
   mObjectHandleGhostDataMap.clear();
   mNewlyDiscoveredObjects.clear();
   mObjectsToUpdate.clear();
   
   mRTIAmbassador.joinFederationExecution(
      federateName.c_str(), executionName.c_str(), this     
   );

   mExecutionName = executionName;
   
   mBaseEntityClassHandle = 
      mRTIAmbassador.getObjectClassHandle("BaseEntity");

   mPhysicalEntityClassHandle =
      mRTIAmbassador.getObjectClassHandle("BaseEntity.PhysicalEntity");

   mPlatformClassHandle =
      mRTIAmbassador.getObjectClassHandle("BaseEntity.PhysicalEntity.Platform");

   mAircraftClassHandle =
      mRTIAmbassador.getObjectClassHandle("BaseEntity.PhysicalEntity.Platform.Aircraft");

   mGroundVehicleClassHandle =
      mRTIAmbassador.getObjectClassHandle("BaseEntity.PhysicalEntity.Platform.GroundVehicle");
      
   mLifeFormClassHandle =
      mRTIAmbassador.getObjectClassHandle("BaseEntity.PhysicalEntity.LifeForm");

   mHumanClassHandle =
      mRTIAmbassador.getObjectClassHandle("BaseEntity.PhysicalEntity.LifeForm.Human");

   mAccelerationVectorAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "AccelerationVector",
      mBaseEntityClassHandle
   );
   
   mAngularVelocityVectorAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "AngularVelocityVector",
      mBaseEntityClassHandle
   );
   
   mDeadReckoningAlgorithmAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "DeadReckoningAlgorithm",
      mBaseEntityClassHandle
   );

   mEntityIdentifierAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "EntityIdentifier",
      mBaseEntityClassHandle
   );

   mEntityTypeAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "EntityType",
      mBaseEntityClassHandle
   );

   mWorldLocationAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "WorldLocation",
      mBaseEntityClassHandle
   );

   mOrientationAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "Orientation",
      mBaseEntityClassHandle
   );
    
   mVelocityVectorAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "VelocityVector",
      mBaseEntityClassHandle
   );
   
   mArticulatedParametersArrayAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "ArticulatedParametersArray",
      mPhysicalEntityClassHandle
   );
   
   mDamageStateAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "DamageState",
      mPhysicalEntityClassHandle
   );

   mForceIdentifierAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "ForceIdentifier",
      mPhysicalEntityClassHandle
   );

   mMarkingAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "Marking",
      mPhysicalEntityClassHandle
   );

   mFirePowerDisabledAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "FirePowerDisabled",
      mPhysicalEntityClassHandle
   );

   mImmobilizedAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "Immobilized",
      mPhysicalEntityClassHandle
   );

   mCamouflageTypeAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "CamouflageType",
      mPhysicalEntityClassHandle
   );

   mIsConcealedAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "IsConcealed",
      mPhysicalEntityClassHandle
   );

   mTrailingEffectsCodeAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "TrailingEffectsCode",
      mPhysicalEntityClassHandle
   );

   mPowerPlantOnAttributeHandle = mRTIAmbassador.getAttributeHandle(
      "PowerPlantOn",
      mPhysicalEntityClassHandle
   );
//Detonation
   mMunitionDetonationClassHandle =
      mRTIAmbassador.getInteractionClassHandle("MunitionDetonation");

   mArticulatedPartDataHandle = mRTIAmbassador.getParameterHandle( //added by mark
      "ArticulatedPartData",
      mMunitionDetonationClassHandle
   );

   mDetonationLocationParameterHandle = mRTIAmbassador.getParameterHandle(
      "DetonationLocation",
      mMunitionDetonationClassHandle
   );

   mDetonationResultCodeParameterHandle = mRTIAmbassador.getParameterHandle(
      "DetonationResultCode",
      mMunitionDetonationClassHandle
   );

   mEventIdentifierParameterHandle = mRTIAmbassador.getParameterHandle(
      "EventIdentifier",
      mMunitionDetonationClassHandle
   );
   mFiringObjectIdentifierHandle = mRTIAmbassador.getParameterHandle(
      "FiringObjectIdentifier",
      mMunitionDetonationClassHandle
   );

   mFinalVelocityVectorHandle = mRTIAmbassador.getParameterHandle(
      "FinalVelocityVector",
      mMunitionDetonationClassHandle
   );

   mFuseTypeParameterHandle = mRTIAmbassador.getParameterHandle(
      "FuseType",
      mMunitionDetonationClassHandle
   ); 

   mMunitionObjectIdentifierHandle = mRTIAmbassador.getParameterHandle(
      "MunitionObjectIdentifier",
      mMunitionDetonationClassHandle
   ); 

   mMunitionTypeParameterHandle = mRTIAmbassador.getParameterHandle(
      "MunitionType",
      mMunitionDetonationClassHandle
   );

    mQuantityFiredParameterHandle = mRTIAmbassador.getParameterHandle(
      "QuantityFired",
      mMunitionDetonationClassHandle
   );
   mRateOfFireHandle = mRTIAmbassador.getParameterHandle(
      "RateOfFire",
      mMunitionDetonationClassHandle
   );

  mRelativeDetonationLocationHandle = mRTIAmbassador.getParameterHandle(
      "RelativeDetonationLocation",
      mMunitionDetonationClassHandle
   );

   mTargetObjectIdentifierHandle = mRTIAmbassador.getParameterHandle(
      "TargetObjectIdentifier",
      mMunitionDetonationClassHandle
   );


   mWarheadTypeParameterHandle = mRTIAmbassador.getParameterHandle(
      "WarheadType",
      mMunitionDetonationClassHandle
   );

   RTI::AttributeHandleSet* ahs = 
      RTI::AttributeHandleSetFactory::create(17);//was4

   ahs->add(mEntityIdentifierAttributeHandle);
   ahs->add(mEntityTypeAttributeHandle);
   ahs->add(mWorldLocationAttributeHandle);   
   ahs->add(mOrientationAttributeHandle);
   ahs->add(mDeadReckoningAlgorithmAttributeHandle);
   ahs->add(mVelocityVectorAttributeHandle);
   ahs->add(mAccelerationVectorAttributeHandle);
   ahs->add(mAngularVelocityVectorAttributeHandle);
   ahs->add(mArticulatedParametersArrayAttributeHandle);
   ahs->add(mDamageStateAttributeHandle);
   ahs->add(mForceIdentifierAttributeHandle);
   ahs->add(mMarkingAttributeHandle);
   ahs->add(mFirePowerDisabledAttributeHandle);
   ahs->add(mImmobilizedAttributeHandle);
   ahs->add(mCamouflageTypeAttributeHandle);
   ahs->add(mIsConcealedAttributeHandle);
   ahs->add(mTrailingEffectsCodeAttributeHandle);
   ahs->add(mPowerPlantOnAttributeHandle);

   mRTIAmbassador.subscribeObjectClassAttributes(
      mPhysicalEntityClassHandle,
      *ahs
   );

   mRTIAmbassador.publishObjectClass(
      mPhysicalEntityClassHandle,
      *ahs
   );

   mRTIAmbassador.publishObjectClass(
      mHumanClassHandle,
      *ahs
   );

   mRTIAmbassador.publishObjectClass(
      mAircraftClassHandle,
      *ahs
   );

   mRTIAmbassador.publishObjectClass(
      mGroundVehicleClassHandle,
      *ahs
   );
   
   delete ahs;
   
   mRTIAmbassador.publishInteractionClass(
      mMunitionDetonationClassHandle
   );

   mRTIAmbassador.subscribeInteractionClass(
      mMunitionDetonationClassHandle
   );

   for(set< RefPtr<Entity> >::iterator it = mMasterEntities.begin();
       it != mMasterEntities.end();
       it++)
   {
      RegisterMasterEntity(const_cast<Entity*>((*it).get()));
   }
}

/**
 * Leaves/destroys the joined execution.
 */
void RTIConnection::LeaveFederationExecution()  //this is kind of broken
{
   try
   {
      mRTIAmbassador.resignFederationExecution(   
         RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES
         
      );
   }
   catch(RTI::RTIinternalError ine)
   {
     
   }

   try
   {
      
      mRTIAmbassador.destroyFederationExecution(
         mExecutionName.c_str()
      );
      

   }
   catch(RTI::FederatesCurrentlyJoined fcj)
   {
      //std::cout<<"Problem DestroyingFed: " << fcj <<std::endl;
   }
   
   mExecutionName = "";
}

/**
 * Returns the name of the joined execution, or "" if unjoined.
 *
 * @return the execution name
 */
string RTIConnection::GetExecutionName()
{
   return mExecutionName;
}

/**
 * Sets the scene in which ghost entities are created.
 *
 * @param scene the target scene
 */
void RTIConnection::SetScene(Scene* scene)
{
   mScene = scene;
}

/**
 * Returns the scene in which ghost entities are created.
 *
 * @return the target scene
 */
Scene* RTIConnection::GetScene()
{
   return mScene.get();
}

/**
 * Sets the effect manager.
 *
 * @param effectManager the effect manager
 */
void RTIConnection::SetEffectManager(EffectManager* effectManager)
{
   if(mEffectManager != NULL)
   {
      mEffectManager->RemoveEffectListener(this);
   }

   mEffectManager = effectManager;

   if(mEffectManager != NULL)
   {
      mEffectManager->AddEffectListener(this);
   }
}

/**
 * Returns the effect manager.
 *
 * @return the effect manager
 */
EffectManager* RTIConnection::GetEffectManager()
{
   return mEffectManager.get();
}

/**
 * Enables or disables globe mode, where entities are rendered as icons at
 * scaled-down geocentric coordinates.
 *
 * @param enable true to enable globe mode, false to disable it
 */
void RTIConnection::EnableGlobeMode(bool enable)
{
   mGlobeModeEnabled = enable;
}

/**
 * Enables or disables UTM mode, Automatically converts Geocentric to UTM
 *
 * @param enable true to enable UTM mode, false to disable it
 */
void RTIConnection::EnableUTMMode(bool enable)
{
   mUTMModeEnabled = enable;
}

/**
 * Checks whether or not globe mode is enabled.
 *
 * @return true if globe mode is enabled, false otherwise
 */
bool RTIConnection::GlobeModeEnabled()
{
   return mGlobeModeEnabled;
}

/**
 * Sets the globe radius.
 *
 * @param radius the new radius
 */
void RTIConnection::SetGlobeRadius(float radius)
{
   mGlobeRadius = radius;
}

/**
 * Returns the globe radius.
 *
 * @return the current globe radius
 */
float RTIConnection::GetGlobeRadius()
{
   return mGlobeRadius;
}

/**
 * Sets the location of the origin in geodetic coordinates.
 *
 * @param latitude the latitude of the origin
 * @param longitude the longitude of the origin
 * @param elevation the elevation of the origin
 */
void RTIConnection::SetGeoOrigin(double latitude, double longitude, double elevation)
{
   GeodeticToGeocentric(
      latitude, 
      longitude, 
      elevation,
      mLocationOffset,
      mLocationOffset + 1,
      mLocationOffset + 2
   );
   
   sgVec3 xVec = { 1, 0, 0 },
          zVec = { 0, 0, 1 };
   
   sgMakeRotMat4(mRotationOffset, 90.0 - latitude, xVec);
   
   sgMat4 mat;
   
   sgMakeRotMat4(mat, longitude + 90.0, zVec);
   
   sgPostMultMat4(mRotationOffset, mat);
   
   sgInvertMat4(mRotationOffsetInverse, mRotationOffset);
}

/**
 * Sets the location of the origin in geocentric coordinates.
 *
 * @param x the x coordinate of the location offset
 * @param y the y coordinate of the location offset
 * @param z the z coordinate of the location offset
 */
void RTIConnection::SetOriginLocation(double x, double y, double z)
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
void RTIConnection::GetOriginLocation(double* x, double* y, double* z) const
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
void RTIConnection::SetOriginRotation(float h, float p, float r)
{
   sgMakeRotMat4(mRotationOffset, h, p, r);

   sgInvertMat4(mRotationOffsetInverse, mRotationOffset);
}

/**
 * Retrieves the rotation of the origin relative to geocentric coordinates.
 *
 * @param h the location in which to store the geocentric heading
 * @param p the location in which to store the geocentric pitch
 * @param r the location in which to store the geocentric roll
 */
void RTIConnection::GetOriginRotation(float* h, float* p, float* r) const
{
   sgCoord coord;

   sgSetCoord(&coord, mRotationOffset);

   *h = coord.hpr[0];
   *p = coord.hpr[1];
   *r = coord.hpr[2];
}

/**
 * Sets the DIS/RPR-FOM site identifier.
 *
 * @param siteIdentifier the new site identifier
 */
void RTIConnection::SetSiteIdentifier(unsigned short siteIdentifier)
{
   mSiteIdentifier = siteIdentifier;
}

/**
 * Returns the DIS/RPR-FOM site identifier.
 *
 * @return the site identifier
 */
unsigned short RTIConnection::GetSiteIdentifier() const
{
   return mSiteIdentifier;
}

/**
 * Sets the DIS/RPR-FOM application identifier.
 *
 * @param applicationIdentifier the new application identifier
 */
void RTIConnection::SetApplicationIdentifier(unsigned short applicationIdentifier)
{
   mApplicationIdentifier = applicationIdentifier;
}

/**
 * Returns the DIS/RPR-FOM application identifier.
 *
 * @return the DIS/RPR-FOM application identifier
 */
unsigned short RTIConnection::GetApplicationIdentifier() const
{
   return mApplicationIdentifier;
}

/**
 * Creates a 4x4 rotation matrix from a set of DIS/RPR-FOM Euler angles.
 *
 * @param dst the destination matrix
 * @param psi the psi angle
 * @param theta the theta angle
 * @param phi the phi angle
 */
void RTIConnection::EulersToMatrix(sgMat4 dst, float psi, float theta, float phi)
{
   sgMakeRotMat4(
      dst, 
      -psi * SG_RADIANS_TO_DEGREES - 90.0f, 
      theta * SG_RADIANS_TO_DEGREES,
      phi * SG_RADIANS_TO_DEGREES
   );
   
   dst[0][1] = -dst[0][1];
   dst[1][1] = -dst[1][1];
   dst[2][1] = -dst[2][1];
   
   dst[0][2] = -dst[0][2];
   dst[1][2] = -dst[1][2];
   dst[2][2] = -dst[2][2];
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
void RTIConnection::MatrixToEulers(sgMat4 src, float* psi, float* theta, float* phi)
{
   sgMat4 mat;
   
   sgCopyMat4(mat, src);
   
   mat[0][1] = -mat[0][1];
   mat[1][1] = -mat[1][1];
   mat[2][1] = -mat[2][1];
   
   mat[0][2] = -mat[0][2];
   mat[1][2] = -mat[1][2];
   mat[2][2] = -mat[2][2];
   
   sgCoord coord;

   sgSetCoord(&coord, mat);

   *psi = (-coord.hpr[0] - 90.0f) * SG_DEGREES_TO_RADIANS;
   *theta = coord.hpr[1] * SG_DEGREES_TO_RADIANS;
   *phi = coord.hpr[2] * SG_DEGREES_TO_RADIANS;
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
void RTIConnection::GeocentricToGeodetic(double x, double y, double z,
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
                   
   *latitude = atan(
      (z + epsqu * b * pow(sin(theta), 3)) /
      (p - esqu * a * pow(cos(theta), 3))
   );
                
   *longitude = atan2(y, x);
                
   *elevation = p/cos(*latitude) - 
                a/sqrt(1.0-esqu*pow(sin(*latitude), 2.0));
        
   *latitude *= SG_RADIANS_TO_DEGREES;
   *longitude *= SG_RADIANS_TO_DEGREES;
}

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
void RTIConnection::ConvertGeodeticToUTM (double Latitude, double Longitude,
                                             long   *Zone, char   *Hemisphere, double *Easting, double *Northing)
{ 


  long Lat_Degrees;
  long Long_Degrees;
  long temp_zone;
  //long Error_Code = UTM_NO_ERROR;
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
      ConvertGeodeticToTransverseMercator(Latitude, Longitude, Easting,
                                              Northing);
      
} /* END OF Convert_Geodetic_To_UTM */

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

void RTIConnection::ConvertGeocentricToGeodetic (double X, double Y, double Z, double *Latitude,
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

void RTIConnection::SetTransverseMercatorParameters(double a, double f, double Origin_Latitude,
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
  double inv_f = 1 / f;
  //long Error_Code = TRANMERC_NO_ERROR;

  
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


void RTIConnection::ConvertGeodeticToTransverseMercator (double Latitude,
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
void RTIConnection::GeodeticToGeocentric(double latitude, double longitude, double elevation,
                                         double* x, double* y, double* z)
{
   double rlatitude = latitude * SG_DEGREES_TO_RADIANS,
          rlongitude = longitude * SG_DEGREES_TO_RADIANS,
          a = semiMajorAxis,
          f = 1.0/flatteningReciprocal,
          esqu = 2.0*f - f*f,
          n = a/sqrt(1.0-esqu*pow(sin(rlatitude), 2.0));
                   
   *x = (n + elevation)*cos(rlatitude)*cos(rlongitude);
            
   *y = (n + elevation)*cos(rlatitude)*sin(rlongitude);
            
   *z = (n*(1.0-esqu) + elevation)*sin(rlatitude);
}

/**
 * Clamps the specified entity to the ground using the active ground clamp mode.
 *
 * @param entity the entity to clamp
 */
void RTIConnection::ClampToGround(Entity* entity)
{  
   bool groundBased = 
      (entity->GetEntityType().GetKind() == PlatformKind ||
       entity->GetEntityType().GetKind() == LifeFormKind) &&
      (entity->GetEntityType().GetDomain() == LandPlatformDomain ||
       entity->GetEntityType().GetDomain() == SurfacePlatformDomain);
   
   bool destroyed = (entity->GetDamageState() == Destroyed);
   
   if((groundBased && mGroundClampMode != NO_CLAMP) || destroyed)
   {
      Transform transform;
   
      entity->GetTransform(&transform, Transformable::REL_CS);
   
      sgVec3 xyz, groundNormal = {0, 0, 1};
      float HOT = 0.0f;
      
      transform.GetTranslation(xyz);
      
      osgUtil::IntersectVisitor iv;
   
      RefPtr<osg::LineSegment> segDown = new osg::LineSegment;
   
      segDown->set(
         osg::Vec3(xyz[0], xyz[1], 10000.f),
         osg::Vec3(xyz[0], xyz[1], -10000.f)
      );
      
      iv.addLineSegment(segDown.get());
   
      iv.setTraversalMask(~entityMask);
      
      mScene->GetSceneNode()->accept(iv);
   
      if (iv.hits())
      {
         osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(segDown.get());
         
         if (!hitList.empty())
         {
            osg::Vec3 ip = hitList.front().getWorldIntersectPoint();
            osg::Vec3 np = hitList.front().getWorldIntersectNormal();
            
            HOT = ip.z();
            
            groundNormal[0] = np.x();
            groundNormal[1] = np.y();
            groundNormal[2] = np.z();
         }
      }
      
      xyz[2] = HOT;
      
      transform.SetTranslation(xyz);
      
      if(groundBased && mGroundClampMode == CLAMP_ELEVATION_AND_ROTATION && !destroyed)
      {
         sgVec3 oldNormal = { 0, 0, 1 };
         sgMat4 rotMat;
         
         transform.GetRotation(rotMat);
         
         sgXformVec3(oldNormal, rotMat);
         
         sgVec3 axis;
         
         sgVectorProductVec3(axis, oldNormal, groundNormal);
         
         float angle = sgASin(sgLengthVec3(axis));
         
         if(angle > 0.0001f)
         {
            sgNormalizeVec3(axis);
            
            sgMat4 deltaRot;
            
            sgMakeRotMat4(deltaRot, angle, axis);
            
            sgPostMultMat4(rotMat, deltaRot);
            
            transform.SetRotation(rotMat);
         }
      }
      
      entity->SetTransform(&transform, Transformable::REL_CS);
   }
}

/**
 * Finds the best (most specific) mapping for the specified
 * entity type.
 *
 * @param entityType the entity type to map
 * @return the best mapping, or NULL if no mapping could be found
 */
RTIConnection::EntityTypeMapping* RTIConnection::FindBestMapping(const EntityType& entityType)
{
   EntityTypeMapping* bestMapping = NULL;
   int bestRank = -1;
   
   for(map<EntityType, EntityTypeMapping>::iterator it = mEntityTypeMappings.begin();
       it != mEntityTypeMappings.end();
       it++)
   {
      int rank = (*it).first.RankMatch(entityType);
      
      if(rank > bestRank)
      {
         bestRank = rank;
         bestMapping = &(*it).second;
      }
   }
   
   return bestMapping;
}

/**
 * Registers a locally owned master entity, announcing its presence
 * to the federation.
 *
 * @param entity the entity to register
 */
void RTIConnection::RegisterMasterEntity(Entity* entity)
{
   mMasterEntities.insert(entity);
   
   if(mExecutionName != "")
   {
      EntityType entityType = entity->GetEntityType();

      RTI::ObjectHandle handle;
      
      EntityIdentifier entityIdentifier(
         mSiteIdentifier,
         mApplicationIdentifier,
         mEntityIdentifierCounter++
      );

      if(mEntityIdentifierCounter == 0)
      {
         mEntityIdentifierCounter = 1;
      }

      entity->SetEntityIdentifier(entityIdentifier);

      char name[128];
      
      sprintf(
         name, 
         "Delta3D/%x/%hu/%hu", 
         mLocalIPAddress,
         entityIdentifier.GetApplicationIdentifier(),
         entityIdentifier.GetEntityIdentifier()
      );
      
      if(entityType.GetKind() == PlatformKind &&
         entityType.GetDomain() == AirPlatformDomain)
      {
         handle = mRTIAmbassador.registerObjectInstance(
            mAircraftClassHandle,
            name
         );
      }
      else if(entityType.GetKind() == PlatformKind &&
              entityType.GetDomain() == LandPlatformDomain)
      {
         handle = mRTIAmbassador.registerObjectInstance(
            mGroundVehicleClassHandle,
            name
         );
      }
      else if(entityType.GetKind() == LifeFormKind)
      {
         handle = mRTIAmbassador.registerObjectInstance(
            mHumanClassHandle,
            name
         );
      }
      else
      {
         handle = mRTIAmbassador.registerObjectInstance(
            mPhysicalEntityClassHandle,
            name
         );
      }

      MasterData masterData;

      masterData.mEntity = entity;

      mObjectHandleMasterDataMap[handle] = masterData;
   }
}

/**
 * Deregisters a locally owned master entity.
 *
 * @param entity the entity to deregister
 */
void RTIConnection::DeregisterMasterEntity(Entity* entity)
{
   mMasterEntities.erase(entity);
   
   for(map<RTI::ObjectHandle, MasterData>::iterator it = 
         mObjectHandleMasterDataMap.begin();
       it != mObjectHandleMasterDataMap.end();
       it++)
   { 
      if( (*it).second.mEntity == entity )
      {
         mRTIAmbassador.deleteObjectInstance(
            (*it).first,
            ""
         );
         
         mObjectHandleMasterDataMap.erase((*it).first);
         
         return;
      }
   }
}

/**
 * Returns the number of active master entities.
 *
 * @return the number of master entities
 */
int RTIConnection::GetMasterEntityCount()
{
   return mMasterEntities.size();
}

/**
 * Returns the master entity at the specified index.
 *
 * @param index the index
 * @return the master entity at the specified index
 */
Entity* RTIConnection::GetMasterEntity(int index)
{
   for(set< RefPtr<Entity> >::iterator it = mMasterEntities.begin();
       it != mMasterEntities.end();
       it++)
   {
      if(index-- == 0)
      {
          return const_cast<Entity*>((*it).get());
      }
   }
   
   return NULL;
}

/**
 * Returns the number of active ghost entities.
 *
 * @return the number of ghost entities
 */
int RTIConnection::GetGhostEntityCount()
{
   return mObjectHandleGhostDataMap.size();
}

/**
 * Returns the ghost entity at the specified index.
 *
 * @param index the index
 * @return the ghost entity at the specified index
 */
Entity* RTIConnection::GetGhostEntity(int index)
{
   for(map<RTI::ObjectHandle, GhostData>::iterator it = mObjectHandleGhostDataMap.begin();
       it != mObjectHandleGhostDataMap.end();
       it++)
   {
      if(index-- == 0)
      {
         return (*it).second.mEntity.get();
      }
   }
   
   return NULL;
}
         
/**
 * Maps the specified entity type to the given filename.
 *
 * @param entityType the entity type to map
 * @param modelFilename the entity model filename
 * @param iconFilename the entity icon filename
 */
void RTIConnection::AddEntityTypeMapping(const EntityType& entityType,
                                         string modelFilename,
                                         string iconFilename)
{
   mEntityTypeMappings[entityType].mModelFilename = modelFilename;
   mEntityTypeMappings[entityType].mIconFilename = iconFilename;
}

/**
 * Maps the specified entity type to the given filename.
 *
 * @param entityType the entity type to map
 * @param modelFilename the entity model filename
 * @param articulatedPartClassNameMap the map from articulated part class IDs
 * to names of DOFTransform nodes
 * @param iconFilename the entity icon filename
 */
void RTIConnection::AddEntityTypeMapping(const EntityType& entityType,
                                         string modelFilename,
                                         const map<unsigned int, string>&
                                          articulatedPartClassNameMap,
                                         string iconFilename)
{
   mEntityTypeMappings[entityType].mModelFilename = modelFilename;
   
   mEntityTypeMappings[entityType].mArticulatedPartClassNameMap = 
      articulatedPartClassNameMap;
      
   mEntityTypeMappings[entityType].mIconFilename = iconFilename;
}
                                   
/**
 * Removes the mapping for the given entity type.
 *
 * @param entityType the entity type to unmap
 */
void RTIConnection::RemoveEntityTypeMapping(const EntityType& entityType)
{
   mEntityTypeMappings.erase(entityType);
}

/**
 * Loads a set of entity type mappings from an XML file.
 *
 * @param filename the name of the file to load
 * @return true if mappings successfully loaded, false otherwise
 */
bool RTIConnection::LoadEntityTypeMappings(string filename)
{
   TiXmlDocument doc(osgDB::findDataFile(filename).c_str());
   
   if(doc.LoadFile())
   {
      mEntityTypeMappings.clear();
      
      for(TiXmlElement* e = 
            doc.RootElement()->FirstChildElement("EntityTypeMapping");
          e != NULL;
          e = e->NextSiblingElement("EntityTypeMapping"))
      {
         EntityType et;
         
         int i;
         const char* val;
         
         if(e->Attribute("kind", &i) != NULL)
         {
            et.SetKind((unsigned char)i);
         }
         if(e->Attribute("domain", &i) != NULL)
         {
            et.SetDomain((unsigned char)i);
         }
         if(e->Attribute("country", &i) != NULL)
         {
            et.SetCountry((unsigned short)i);
         }
         if(e->Attribute("category", &i) != NULL)
         {
            et.SetCategory((unsigned char)i);
         }
         if(e->Attribute("subcategory", &i) != NULL)
         {
            et.SetSubcategory((unsigned char)i);
         }
         if(e->Attribute("specific", &i) != NULL)
         {
            et.SetSpecific((unsigned char)i);
         }
         if(e->Attribute("extra", &i) != NULL)
         {
            et.SetExtra((unsigned char)i);
         }
         
         if((val = e->Attribute("model")) != NULL)
         {
            mEntityTypeMappings[et].mModelFilename = val;
         }
         if((val = e->Attribute("icon")) != NULL)
         {
            mEntityTypeMappings[et].mIconFilename = val;
         }
         
         for(TiXmlElement* se = 
               e->FirstChildElement("ArticulatedPart");
             se != NULL;
             se = e->NextSiblingElement("ArticulatedPart"))
         {
            if(se->Attribute("class", &i) != NULL)
            {
               if((val = se->Attribute("name")) != NULL)
               {
                  mEntityTypeMappings[et].mArticulatedPartClassNameMap[i] =
                     val;   
               }
            }   
         }
      }
      
      return true;
   }
   else
   {
      return false;
   }
}

/**
 * Sets the ground clamp mode.
 *
 * @param mode the new ground clamp mode
 */
void RTIConnection::SetGroundClampMode(RTIConnection::GroundClampMode mode)
{
   mGroundClampMode = mode;
}

/**
 * Returns the ground clamp mode.
 *
 * @return the current ground clamp mode
 */
RTIConnection::GroundClampMode RTIConnection::GetGroundClampMode()
{
   return mGroundClampMode;
}

/**
* Sets the effect clamp mode.
*
* @param mode the new effect clamp mode
*/
void RTIConnection::SetEffectClampMode(bool mode)
{
   mEffectClampMode = mode;
}

/**
* Returns the effect clamp mode.
*
* @return the current effect clamp mode
*/
bool RTIConnection::GetEffectClampMode()
{
   return mEffectClampMode;
}
         
/**
 * Adds a detonation listener.
 *
 * @param listener the listener to add
 */
void RTIConnection::AddDetonationListener(DetonationListener* listener)
{
   mDetonationListeners.insert(listener);
}

/**
 * Removes a detonation listener.
 *
 * @param listener the listener to remove
 */
void RTIConnection::RemoveDetonationListener(DetonationListener* listener)
{
   mDetonationListeners.erase(listener);
}

/**
 * Processes a received message.
 *
 * @param data the message structure
 */
void RTIConnection::OnMessage(MessageData *data)
{
   if(data->message == "preframe" && mExecutionName != "")
   {
      double dt = *(double*)data->userData;
      
      bool doHeartbeat = false;

      Producer::Timer_t currentTime = mTimer.tick();

      if(mTimer.delta_s(mLastHeartbeatTime, currentTime) > heartbeatInterval)
      {
         doHeartbeat = true;

         mLastHeartbeatTime = currentTime;
      }


      RTI::AttributeHandleValuePairSet* theAttributes =
         RTI::AttributeSetFactory::create(15);

      WorldCoordinate worldLocation;

      EulerAngles orientation;

      VelocityVector velocityVector;

      Transform transform;
   
      sgVec3 vec;

      sgMat4 mat;


      char encodedEntityIdentifier[6],
           encodedEntityType[8],
           encodedWorldLocation[24],
           encodedOrientation[12],
           encodedVelocityVector[12],
           encodedDeadReckoningAlgorithm[1],
           encodedDamageState[4],
           encodedForceIdentifier[1],
           encodedMarking[12],
           encodedFirePowerDisabled[1],
           encodedImmobilized[1],
           encodedCamouflageType[4],
           encodedIsConcealed[1],
           encodedTrailingEffectsCode[4],
           encodedPowerPlantOn[1];


      for(map<RTI::ObjectHandle, MasterData>::iterator m =
            mObjectHandleMasterDataMap.begin();
          m != mObjectHandleMasterDataMap.end();
          m++)
      {
         Entity* master = (*m).second.mEntity.get();

         theAttributes->empty();

         if(doHeartbeat || mObjectsToUpdate.count((*m).first) > 0)
         {
            master->GetEntityIdentifier().Encode(encodedEntityIdentifier);

            theAttributes->add(
               mEntityIdentifierAttributeHandle,
               encodedEntityIdentifier,
               6
            );

            master->GetEntityType().Encode(encodedEntityType);

            theAttributes->add(
               mEntityTypeAttributeHandle,
               encodedEntityType,
               8
            );

            velocityVector.Encode(encodedVelocityVector);

            theAttributes->add(
               mVelocityVectorAttributeHandle,
               encodedVelocityVector,
               12
            );

            encodedDeadReckoningAlgorithm[0] = 2; // DRM(F, P, W)

            theAttributes->add(
               mDeadReckoningAlgorithmAttributeHandle,
               encodedDeadReckoningAlgorithm,
               1
            );

            *((int*)encodedDamageState) = master->GetDamageState();
            
            if(ulIsLittleEndian)
            {
               ulEndianSwap((int*)encodedDamageState);
            }

            theAttributes->add(
               mDamageStateAttributeHandle,
               encodedDamageState,
               4
            );

            *((char*)encodedForceIdentifier) = 0; // Other

            theAttributes->add(
               mForceIdentifierAttributeHandle,
               encodedForceIdentifier,
               1
            );

            encodedMarking[0] = 1; // ASCII
            
            memset(encodedMarking + 1, 0, 11);
                  
            theAttributes->add(
               mMarkingAttributeHandle,
               encodedMarking,
               12
            );

            *((char*)encodedFirePowerDisabled) = 0; // False

            theAttributes->add(
               mFirePowerDisabledAttributeHandle,
               encodedFirePowerDisabled,
               1
            );

            *((char*)encodedImmobilized) = 0; // False

            theAttributes->add(
               mImmobilizedAttributeHandle,
               encodedImmobilized,
               1
            );

            *((int*)encodedCamouflageType) = 0; // None

            theAttributes->add(
               mCamouflageTypeAttributeHandle,
               encodedCamouflageType,
               4
            );

            *((char*)encodedIsConcealed) = 0; // False

            theAttributes->add(
               mIsConcealedAttributeHandle,
               encodedIsConcealed,
               1
            );

            *((int*)encodedTrailingEffectsCode) = 0; // No trail

            theAttributes->add(
               mTrailingEffectsCodeAttributeHandle,
               encodedTrailingEffectsCode,
               4
            );

            *((char*)encodedPowerPlantOn) = 0; // Whatever

            theAttributes->add(
               mPowerPlantOnAttributeHandle,
               encodedPowerPlantOn,
               1
            );
         }

         master->GetTransform(&transform, Transformable::REL_CS);

         if(doHeartbeat || !(*m).second.mTransform.EpsilonEquals(&transform))
         {
            transform.GetTranslation(vec);
            transform.GetRotation(mat);

            sgXformVec3(vec, mRotationOffset);

            worldLocation.SetX(vec[0] + mLocationOffset[0]);
            worldLocation.SetY(vec[1] + mLocationOffset[1]);
            worldLocation.SetZ(vec[2] + mLocationOffset[2]);

            worldLocation.Encode(encodedWorldLocation);

            theAttributes->add(
               mWorldLocationAttributeHandle,
               encodedWorldLocation,
               24
            );

            sgPostMultMat4(mat, mRotationOffset);

            float psi, theta, phi;

            MatrixToEulers(mat, &psi, &theta, &phi);

            orientation.SetPsi(psi);

            orientation.SetTheta(theta);

            orientation.SetPhi(phi);
        
            orientation.Encode(encodedOrientation);

            theAttributes->add(
               mOrientationAttributeHandle,
               encodedOrientation,
               12
            );

            (*m).second.mTransform = transform;
         }

         if(theAttributes->size() > 0)
         {
            mRTIAmbassador.updateAttributeValues(
               (*m).first,
               *theAttributes,
               ""
            );
         }
      }

      mObjectsToUpdate.clear();

      delete theAttributes;

      
      for(map<RTI::ObjectHandle, GhostData>::iterator g =
            mObjectHandleGhostDataMap.begin();
          g != mObjectHandleGhostDataMap.end();
          g++)
      {
         GhostData& gd = (*g).second;
         Entity* ghost = (*g).second.mEntity.get();
         
         UpdateGhostPosition(dt, gd, ghost);
         
         const vector<ArticulatedParameter>& params =
            ghost->GetArticulatedParametersArray();
         
         map<unsigned int, map<unsigned int, float> > classTypeValueMap;
         
         vector<ArticulatedParameter> newParams;
         
         for(vector<ArticulatedParameter>::const_iterator p = params.begin();
             p != params.end();
             p++)
         {
            const ParameterValue& pv = (*p).GetParameterValue();
                        
            if(pv.GetArticulatedParameterType() == ArticulatedPart)
            {
               const ArticulatedParts& ap = pv.GetArticulatedParts();
               
               switch(ap.GetTypeMetric())
               {
                  case AzimuthMetric:
                     if(classTypeValueMap[ap.GetClass()].count(AzimuthMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][AzimuthMetric] = ap.GetValue();
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][AzimuthMetric] += ap.GetValue();
                     }
                     break;
                     
                  case AzimuthRateMetric:
                     if(classTypeValueMap[ap.GetClass()].count(AzimuthMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][AzimuthMetric] = ap.GetValue()*dt;
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][AzimuthMetric] += ap.GetValue()*dt;
                     }
                     newParams.push_back(*p);
                     break;
                     
                  case ElevationMetric:
                     if(classTypeValueMap[ap.GetClass()].count(ElevationMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][ElevationMetric] = ap.GetValue();
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][ElevationMetric] += ap.GetValue();
                     }
                     break;
                     
                  case ElevationRateMetric:
                     if(classTypeValueMap[ap.GetClass()].count(ElevationMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][ElevationMetric] = ap.GetValue()*dt;
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][ElevationMetric] += ap.GetValue()*dt;
                     }
                     newParams.push_back(*p);
                     break;
                     
                  case RotationMetric:
                     if(classTypeValueMap[ap.GetClass()].count(RotationMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][RotationMetric] = ap.GetValue();
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][RotationMetric] += ap.GetValue();
                     }
                     break;
                     
                  case RotationRateMetric:
                     if(classTypeValueMap[ap.GetClass()].count(RotationMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][RotationMetric] = ap.GetValue()*dt;
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][RotationMetric] += ap.GetValue()*dt;
                     }
                     newParams.push_back(*p);
                     break;
                     
                  case XMetric:
                     if(classTypeValueMap[ap.GetClass()].count(XMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][XMetric] = ap.GetValue();
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][XMetric] += ap.GetValue();
                     }
                     break;
                  
                  case XRateMetric:
                     if(classTypeValueMap[ap.GetClass()].count(XMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][XMetric] = ap.GetValue()*dt;
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][XMetric] += ap.GetValue()*dt;
                     }
                     newParams.push_back(*p);
                     break;
                     
                  case YMetric:
                     if(classTypeValueMap[ap.GetClass()].count(YMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][YMetric] = ap.GetValue();
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][YMetric] += ap.GetValue();
                     }
                     break;
                  
                  case YRateMetric:
                     if(classTypeValueMap[ap.GetClass()].count(YMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][YMetric] = ap.GetValue()*dt;
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][YMetric] += ap.GetValue()*dt;
                     }
                     newParams.push_back(*p);
                     break;
                     
                  case ZMetric:
                     if(classTypeValueMap[ap.GetClass()].count(ZMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][ZMetric] = ap.GetValue();
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][ZMetric] += ap.GetValue();
                     }
                     break;
                     
                  case ZRateMetric:
                     if(classTypeValueMap[ap.GetClass()].count(ZMetric) == 0)
                     {
                        classTypeValueMap[ap.GetClass()][ZMetric] = ap.GetValue()*dt;
                     }
                     else
                     {
                        classTypeValueMap[ap.GetClass()][ZMetric] += ap.GetValue()*dt;
                     }
                     newParams.push_back(*p);
                     break;
               
                  default:
                     newParams.push_back(*p);
                     break;
               }
            }
            else
            {
               newParams.push_back(*p);
            }
         }
         
         for(map<unsigned int, map<unsigned int, float> >::iterator ctvm =
               classTypeValueMap.begin();
             ctvm != classTypeValueMap.end();
             ctvm++)
         {
            osgSim::DOFTransform* transform = NULL;
            osg::Vec3 hpr, translate;
            
            if(gd.mArticulatedPartClassTransformMap.count((*ctvm).first) > 0)
            {
               transform = gd.mArticulatedPartClassTransformMap[(*ctvm).first];
            
               hpr = transform->getCurrentHPR();
               translate = transform->getCurrentTranslate();
            }
            
            if((*ctvm).second.count(AzimuthMetric) > 0)
            {
               hpr[0] = -(*ctvm).second[AzimuthMetric];

               newParams.push_back(
                  ArticulatedParameter(
                     0, 0,
                     ParameterValue(
                        ArticulatedParts(
                           (*ctvm).first,
                           AzimuthMetric,
                           (*ctvm).second[AzimuthMetric]
                        )
                     )
                  )
               );
            }
            if((*ctvm).second.count(ElevationMetric) > 0)
            {
               hpr[1] = (*ctvm).second[ElevationMetric];
               
               newParams.push_back(
                  ArticulatedParameter(
                     0, 0,
                     ParameterValue(
                        ArticulatedParts(
                           (*ctvm).first,
                           ElevationMetric,
                           (*ctvm).second[ElevationMetric]
                        )
                     )
                  )
               );
            }
            if((*ctvm).second.count(RotationMetric) > 0)
            {
               hpr[2] = (*ctvm).second[RotationMetric];
               
               newParams.push_back(
                  ArticulatedParameter(
                     0, 0,
                     ParameterValue(
                        ArticulatedParts(
                           (*ctvm).first,
                           RotationMetric,
                           (*ctvm).second[RotationMetric]
                        )
                     )
                  )
               );
            }
            
            if((*ctvm).second.count(XMetric) > 0)
            {
               translate[0] = (*ctvm).second[XMetric];
               
               newParams.push_back(
                  ArticulatedParameter(
                     0, 0,
                     ParameterValue(
                        ArticulatedParts(
                           (*ctvm).first,
                           XMetric,
                           (*ctvm).second[XMetric]
                        )
                     )
                  )
               );
            }
            if((*ctvm).second.count(YMetric) > 0)
            {
               translate[1] = (*ctvm).second[YMetric];
               
               newParams.push_back(
                  ArticulatedParameter(
                     0, 0,
                     ParameterValue(
                        ArticulatedParts(
                           (*ctvm).first,
                           YMetric,
                           (*ctvm).second[YMetric]
                        )
                     )
                  )
               );
            }
            if((*ctvm).second.count(ZMetric) > 0)
            {
               translate[2] = -(*ctvm).second[ZMetric];
               
               newParams.push_back(
                  ArticulatedParameter(
                     0, 0,
                     ParameterValue(
                        ArticulatedParts(
                           (*ctvm).first,
                           ZMetric,
                           (*ctvm).second[ZMetric]
                        )
                     )
                  )
               );
            }
            
            if(transform != NULL)
            {
               transform->setCurrentHPR(hpr);
               transform->setCurrentTranslate(translate);
            }
         }
         
         ghost->SetArticulatedParametersArray(newParams);
      }
      
            
      mRTIAmbassador.tick();     
      
      
      // Request types of newly discovered objects

      RTI::AttributeHandleSet* requiredAttributes =
         RTI::AttributeHandleSetFactory::create(2);
      
      requiredAttributes->add(mEntityIdentifierAttributeHandle);
      requiredAttributes->add(mEntityTypeAttributeHandle);

      for(set<RTI::ObjectHandle>::iterator objs =
            mNewlyDiscoveredObjects.begin();
          objs != mNewlyDiscoveredObjects.end();
          objs++)
      {
         mRTIAmbassador.requestObjectAttributeValueUpdate(
            *objs,
            *requiredAttributes
         );
      }

      mNewlyDiscoveredObjects.clear();

      delete requiredAttributes;
   }
}

void RTIConnection::UpdateGhostPosition(const double dt, GhostData &gd, Entity *ghost)
{
   Transform transform;
   WorldCoordinate wc;
   VelocityVector vv;

   ghost->GetTransform(&transform, Transformable::REL_CS);

   wc = ghost->GetWorldLocation();

   vv = ghost->GetVelocityVector();

   wc.SetX(wc.GetX() + vv.GetX()*dt);
   wc.SetY(wc.GetY() + vv.GetY()*dt);
   wc.SetZ(wc.GetZ() + vv.GetZ()*dt);

   ghost->SetWorldLocation(wc);

   sgVec3 position;

   if(mGlobeModeEnabled)
   {
      position[0] = (wc.GetX()/semiMajorAxis)*mGlobeRadius;
      position[1] = (wc.GetY()/semiMajorAxis)*mGlobeRadius;
      position[2] = (wc.GetZ()/semiMajorAxis)*mGlobeRadius;

   }
   else if(mUTMModeEnabled)
   {
      ConvertGeocentricToGeodetic(wc.GetX(),wc.GetY(),wc.GetZ(),&mLat,&mLong,&mElevation);
      ConvertGeodeticToUTM(mLat,mLong,&mZone,&mHemisphere,&mEasting,&mNorthing);
      position[0] = mEasting - mLocationOffset[0];
      position[1] = mNorthing - mLocationOffset[1];
      position[2] = mElevation - mLocationOffset[2];
      sgXformVec3(position, mRotationOffsetInverse); //not sure if I need this

   }
   else
   {
      position[0] = wc.GetX()- mLocationOffset[0];
      position[1] = wc.GetY() - mLocationOffset[1];
      position[2] = wc.GetZ() - mLocationOffset[2];
      sgXformVec3(position, mRotationOffsetInverse);
      //std::cout<<"X: " <<position[0]<<"Y: " <<position[1]<<"Z: " <<position[2]<<std::endl;  //debugging
   }

   transform.SetTranslation(position);

   ghost->SetTransform(&transform, Transformable::REL_CS);

   ClampToGround(ghost);
}

/**
 * Invoked by the RTI ambassador to notify the federate of a new object
 * instance.
 *
 * @param theObject the handle of the discovered object
 * @param theObjectClass the handle of the discovered object's class
 * @param theObjectName the name of the discovered object
 */
void RTIConnection::discoverObjectInstance(
   RTI::ObjectHandle theObject,
   RTI::ObjectClassHandle theObjectClass,
   const char* theObjectName)
        throw (
                RTI::CouldNotDiscover,
                RTI::ObjectClassNotKnown,
                RTI::FederateInternalError)
{
   Entity* ghost = new Entity(theObjectName);

   ghost->GetOSGNode()->setNodeMask(entityMask);
   
   GhostData ghostData;

   ghostData.mEntity = ghost;

   mObjectHandleGhostDataMap[theObject] = ghostData;

   if(mScene != NULL)
   {
      mScene->AddDrawable(ghost);
   }

   mNewlyDiscoveredObjects.insert(theObject);
}

/**
 * Invoked by the RTI ambassador to request that the federate provide
 * updated attribute values for the specified object.
 *
 * @param theObject the handle of the object of interest
 * @param theAttributes the set of attributes to update
 */
void RTIConnection::provideAttributeValueUpdate(
   RTI::ObjectHandle theObject,
   const RTI::AttributeHandleSet& theAttributes)
throw (
            RTI::ObjectNotKnown,
            RTI::AttributeNotKnown,
            RTI::AttributeNotOwned,
            RTI::FederateInternalError
         )
{
   mObjectsToUpdate.insert(theObject);
}

/**
 * Invoked by the RTI ambassador to notify the federate of updated object
 * attribute values.
 *
 * @param theObject the handle of the modified object
 * @param theAttributes the new attribute values
 * @param theTime the event timestamp
 * @param theTag the user-supplied tag associated with the event
 * @param theHandle the event retraction handle
 */
void RTIConnection::reflectAttributeValues(
   RTI::ObjectHandle theObject,
   const RTI::AttributeHandleValuePairSet& theAttributes,
   const RTI::FedTime& theTime,
   const char *theTag,
   RTI::EventRetractionHandle theHandle)
throw (
            RTI::ObjectNotKnown,
            RTI::AttributeNotKnown,
            RTI::FederateOwnsAttributes,
            RTI::InvalidFederationTime,
            RTI::FederateInternalError
         )        
{
   reflectAttributeValues(theObject, theAttributes, theTag);
}

/**
 * Finds a named DOFTransform under the specified parent.
 *
 * @param parent the parent to search
 * @param name the name of the DOFTransform to find
 * @return a pointer to the transform with the specified name, or NULL if no
 * such transform exists
 */
osgSim::DOFTransform* FindNamedTransform(osg::Node* parent, string name)
{
   osgSim::DOFTransform* transform = 
      dynamic_cast<osgSim::DOFTransform*>(parent);
   
   if(transform != NULL && transform->getName() == name)
   {
      return transform;
   }
   else
   {
      osg::Group* group = parent->asGroup();
   
      if(group != NULL)
      {
         for(unsigned int i=0;i<group->getNumChildren();i++)
         {
            transform = FindNamedTransform(group->getChild(i), name);
            
            if(transform != NULL)
            {
               return transform;
            }
         }
      }
      
      return NULL;
   }
}

/**
 * Invoked by the RTI ambassador to notify the federate of updated object
 * attribute values.
 *
 * @param theObject the handle of the modified object
 * @param theAttributes the new attribute values
 * @param theTag the user-supplied tag associated with the event
 */
void RTIConnection::reflectAttributeValues(
   RTI::ObjectHandle theObject,
   const RTI::AttributeHandleValuePairSet& theAttributes,
   const char *theTag)
       throw (
            RTI::ObjectNotKnown,
            RTI::AttributeNotKnown,
            RTI::FederateOwnsAttributes,
            RTI::FederateInternalError
         ) 
{
   GhostData& ghostData = mObjectHandleGhostDataMap[theObject];

   Entity* ghost = ghostData.mEntity.get();

   Transform transform;

   unsigned int damageAttribute;

   ghost->GetTransform(&transform, Transformable::REL_CS);

   

   for(unsigned int i=0;i<theAttributes.size();i++)
   {
      RTI::AttributeHandle handle = theAttributes.getHandle(i);

      if(handle == mEntityIdentifierAttributeHandle)
      {
         unsigned long length;

         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 6)
         {
            EntityIdentifier entityIdentifier;

            entityIdentifier.Decode(buf);

            ghost->SetEntityIdentifier(entityIdentifier);
         }
      }
      else if(handle == mEntityTypeAttributeHandle)
      {
         unsigned long length;

         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 8)
         {
            EntityType entityType;

            entityType.Decode(buf);

            ghost->SetEntityType(entityType);

            EntityTypeMapping* mapping = FindBestMapping(entityType);
            
            string filename;
            
            if(mapping != NULL)
            {
               if(mGlobeModeEnabled)
               {
                  filename = mapping->mIconFilename;
               }
               else
               {
                  filename = mapping->mModelFilename;
               }
            }
            
            if(filename != ghost->GetFilename())
            {
               ghost->LoadFile(filename);
               
               if(mapping != NULL)
               {
                  for(map<unsigned int, string>::iterator it =
                        mapping->mArticulatedPartClassNameMap.begin();
                      it != mapping->mArticulatedPartClassNameMap.end();
                      it++)
                  {
                     osgSim::DOFTransform* transform = 
                        FindNamedTransform(ghost->GetOSGNode(), (*it).second);
                        
                     if(transform != NULL)
                     {
                        transform->setAnimationOn(false);
                        
                        ghostData.mArticulatedPartClassTransformMap[(*it).first] =
                           transform;
                     }
                  }
               }
            }
         }
      }
      else if(handle == mWorldLocationAttributeHandle)
      {
         unsigned long length;

         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 24)
         {
            WorldCoordinate worldCoordinate;

            worldCoordinate.Decode(buf);

            ghost->SetWorldLocation(worldCoordinate);
            
            sgVec3 position;
            
            if(mGlobeModeEnabled)
            {
               position[0] = (worldCoordinate.GetX()/semiMajorAxis)*mGlobeRadius;
               position[1] = (worldCoordinate.GetY()/semiMajorAxis)*mGlobeRadius;
               position[2] = (worldCoordinate.GetZ()/semiMajorAxis)*mGlobeRadius;
            }
            else if(mUTMModeEnabled)
            {
            
               ConvertGeocentricToGeodetic(worldCoordinate.GetX(),worldCoordinate.GetY(),worldCoordinate.GetZ(),&mLat,&mLong,&mElevation);
               ConvertGeodeticToUTM(mLat,mLong,&mZone,&mHemisphere,&mEasting,&mNorthing);
               position[0] = mEasting - mLocationOffset[0];
               position[1] = mNorthing - mLocationOffset[1];
               position[2] = mElevation - mLocationOffset[2];
               sgXformVec3(position, mRotationOffsetInverse); //not sure if I need this
            

            }
            else
            {
               position[0] = worldCoordinate.GetX() - mLocationOffset[0];
               position[1] = worldCoordinate.GetY() - mLocationOffset[1];
               position[2] = worldCoordinate.GetZ() - mLocationOffset[2];

               sgXformVec3(position, mRotationOffsetInverse);
            }
            
            transform.SetTranslation(position);
         }
      }
      else if(handle == mOrientationAttributeHandle)
      {
         unsigned long length;

         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 12)
         {
            EulerAngles eulerAngles;

            eulerAngles.Decode(buf);

            ghost->SetOrientation(eulerAngles);
            
            sgMat4 rotMat;
            
            EulersToMatrix(
               rotMat, 
               eulerAngles.GetPsi(), 
               eulerAngles.GetTheta(), 
               eulerAngles.GetPhi()
            );
            

            if(!mGlobeModeEnabled)
            {
               sgPostMultMat4(rotMat, mRotationOffsetInverse);
            }

            if(mUTMModeEnabled) //Added by Mark
            {
               sgCopyMat4(rotMat, mRotationOffset);
            }
            
            transform.SetRotation(rotMat);
         }
      }
      else if(handle == mVelocityVectorAttributeHandle)
      {
         unsigned long length;
         
         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 12)
         {
            VelocityVector velocityVector;

            velocityVector.Decode(buf);

            ghost->SetVelocityVector(velocityVector);
         }
      }
      else if(handle == mAccelerationVectorAttributeHandle)
      {
         unsigned long length;
         
         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 12)
         {
            VelocityVector velocityVector;

            velocityVector.Decode(buf);

            ghost->SetAccelerationVector(velocityVector);
         }
      }
      else if(handle == mAngularVelocityVectorAttributeHandle)
      {
         unsigned long length;
         
         char* buf = theAttributes.getValuePointer(i, length);

         if(length == 12)
         {
            VelocityVector velocityVector;

            velocityVector.Decode(buf);

            ghost->SetAngularVelocityVector(velocityVector);
         }
      }
      else if(handle == mArticulatedParametersArrayAttributeHandle)
      {
         unsigned long length;
         
         char* buf = theAttributes.getValuePointer(i, length);
         
         int numParams = length/20;
         
         vector<ArticulatedParameter> params;
         
         params.resize(numParams);
         
         for(int i=0;i<numParams;i++)
         {
            params[i].Decode(&buf[i*20]);
         }
         
         ghost->SetArticulatedParametersArray(params);
         
         for(vector<ArticulatedParameter>::iterator p = params.begin();
             p != params.end();
             p++)
         {
            const ParameterValue& pv = (*p).GetParameterValue();
                        
            if(pv.GetArticulatedParameterType() == ArticulatedPart)
            {
               const ArticulatedParts& ap = pv.GetArticulatedParts();
               
               if(ghostData.mArticulatedPartClassTransformMap.count(ap.GetClass()) > 0)
               {
                  osgSim::DOFTransform* transform = 
                     ghostData.mArticulatedPartClassTransformMap[ap.GetClass()];
                     
                  osg::Vec3 hpr = transform->getCurrentHPR(),
                            translate = transform->getCurrentTranslate();
                            
                  switch(ap.GetTypeMetric())
                  {
                     case AzimuthMetric:
                        hpr[0] = -ap.GetValue();
                        break;
                        
                     case ElevationMetric:
                        hpr[1] = ap.GetValue();
                        break;
                        
                     case RotationMetric:
                        hpr[2] = ap.GetValue();
                        break;
                        
                     case XMetric:
                        translate[0] = ap.GetValue();
                        break;
                        
                     case YMetric:
                        translate[1] = ap.GetValue();
                        break;
                        
                     case ZMetric:
                        translate[2] = -ap.GetValue();
                        break;
                  }
                  
                  transform->setCurrentHPR(hpr);
                  transform->setCurrentTranslate(translate);
               }
            }
         }
      }
      else if(handle == mDamageStateAttributeHandle)
      {
         sgVec3 position;
         transform.GetTranslation(position);
         unsigned long length;
         char* buf = theAttributes.getValuePointer(i, length);
         damageAttribute = *(unsigned int*)(&buf[0]);

         if(ulIsLittleEndian)
         {
            ulEndianSwap(&damageAttribute);
            
         }

         ghost->SetDamageState((DamageState)damageAttribute);
         
         if(damageAttribute!=0)
         {
            if(mEffectManager != NULL)
            {   
               mIgnoreEffect = true;
               mEffectManager->AddDetonation(
                  position,//position,
                  SmokeDetonation,
                  60.0
               );
               mIgnoreEffect = false;
            }
         }

         osg::StateSet* ss = ghost->GetOSGNode()->getOrCreateStateSet();
                  
         osg::Material* mat = 
            (osg::Material*)ss->getAttribute(osg::StateAttribute::MATERIAL);
         
         if(mat == NULL)
         {
            mat = new osg::Material;
            
            mat->setDiffuse(
               osg::Material::FRONT_AND_BACK,
               osg::Vec4(0, 0, 0, 1)
            );
         }
         
         ss->setAttributeAndModes(
            mat, 
            damageAttribute == Destroyed ?
               (osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE) :
               osg::StateAttribute::OFF
         );
               
         //std::cout<<"Got Damage update type: "<<damageAttribute<<std::endl;
      }
   }

   ghost->SetTransform(&transform, Transformable::REL_CS);
   
   ClampToGround(ghost);
}

/**
 * Invoked by the RTI ambassador to notify the federate of a deleted object
 * instance.
 *
 * @param theObject the handle of the removed object
 * @param theTime the event timestamp
 * @param theTag the user-supplied tag associated with the event
 * @param theHandle the event retraction handle
 */
void RTIConnection::removeObjectInstance(
   RTI::ObjectHandle theObject,
   const RTI::FedTime& theTime,
   const char *theTag,
   RTI::EventRetractionHandle theHandle)
throw (
            RTI::ObjectNotKnown,
            RTI::InvalidFederationTime,
            RTI::FederateInternalError
         )        
{
   removeObjectInstance(theObject, theTag);
}

/**
 * Invoked by the RTI ambassador to notify the federate of a removed object
 * instance.
 *
 * @param theObject the handle of the removed object
 * @param theTag the user-supplied tag associated with the event
 */
void RTIConnection::removeObjectInstance(
   RTI::ObjectHandle theObject,
   const char *theTag)
throw (
            RTI::ObjectNotKnown,
            RTI::FederateInternalError
         )        
{
   mScene->RemoveDrawable( mObjectHandleGhostDataMap[ theObject ].mEntity.get() ); //ADDED BY OSB
   mObjectHandleGhostDataMap.erase( theObject );
}

/**
 * Invoked by the RTI ambassador to notify the federate of a received
 * interaction.
 *
 * @param theInteraction the handle of the received interaction
 * @param theParameters the parameters of the interaction
 * @param theTag the user-supplied tag associated with the event
 */
void RTIConnection::receiveInteraction(
   RTI::InteractionClassHandle theInteraction,
   const RTI::ParameterHandleValuePairSet& theParameters,
   const char *theTag)
throw (
            RTI::InteractionClassNotKnown,
            RTI::InteractionParameterNotKnown,
            RTI::FederateInternalError
         )        
{
   if(theInteraction == mMunitionDetonationClassHandle)
   {
      WorldCoordinate detonationLocation;
      EventIdentifier eventIdentifier;
      unsigned short fuseType;
      EntityType munitionType;
      unsigned short warheadType;
      unsigned char detonationResultCode;
      unsigned short quantityFired;
      
      
      sgVec3 position;
      
      for(unsigned int i=0;i<theParameters.size();i++)
      {
         RTI::ParameterHandle handle = theParameters.getHandle(i);

         if(handle == mDetonationLocationParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 24)
            {
               detonationLocation.Decode(buf);

               position[0] = detonationLocation.GetX() - mLocationOffset[0];
               position[1] = detonationLocation.GetY() - mLocationOffset[1];
               position[2] = detonationLocation.GetZ() - mLocationOffset[2];

               sgXformPnt3(position, mRotationOffsetInverse);

               if(mEffectClampMode)
                  position[2] = mScene->GetHeightOfTerrain( &(position[0]), &(position[1])); 
            }
         }
         else if(handle == mEventIdentifierParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 5)
            {
               eventIdentifier.Decode(buf);
            }
         }
         else if(handle == mFuseTypeParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 2)
            {
               fuseType = *(unsigned short*)(&buf[0]);

               if(ulIsLittleEndian)
               {
                  ulEndianSwap(&fuseType);
               }
            }
         }
         else if(handle == mMunitionTypeParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 8)
            {
               munitionType.Decode(buf);
            }
         }
         else if(handle == mWarheadTypeParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 2)
            {
               warheadType = *(unsigned short*)(&buf[0]);

               if(ulIsLittleEndian)
               {
                  ulEndianSwap(&warheadType);
               }
            }
         }
         else if(handle == mDetonationResultCodeParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 1)
            {
               detonationResultCode = *(unsigned char*)(&buf[0]);
            }
         }
         else if(handle == mQuantityFiredParameterHandle)
         {
            unsigned long length;

            char* buf = theParameters.getValuePointer(i, length);

            if(length == 2)
            {
               quantityFired = *(unsigned short*)(&buf[0]);

               if(ulIsLittleEndian)
               {
                  ulEndianSwap(&quantityFired);
               }
            }
         }
      }

      for(set<DetonationListener*>::iterator it = mDetonationListeners.begin();
          it != mDetonationListeners.end();
          it++)
      {
         (*it)->DetonationOccurred(
            detonationLocation,
            eventIdentifier,
            fuseType,
            munitionType,
            warheadType,
            detonationResultCode,
            quantityFired
         );
      }
      
      if(mEffectManager != NULL)
      {

			unsigned int munType = munitionType.GetSpecific();
			unsigned int  expType;

			switch (munType)
			{
				case 2: //HE
					{
						if((unsigned int)munitionType.GetExtra() == 1)
							expType = 4000L;
						else
							expType = 1000L;

						break;
					}
				case 13: //SMOKE
					{
						expType = 3000L;

						break;
					}
				case 14: //M825
				case 18:
					{
						expType = 6000L;
						break;
					}
				case 1: //ICM					
				case 3: //ICM
					{
						expType = 5000L;
						break;
					}
				default:
					expType = 1000L;
					break;
			}
         mIgnoreEffect = true;

         mEffectManager->AddDetonation(
            position,
            (DetonationType)expType
         );

         mIgnoreEffect = false;
      }
   }
}

/**
 * Called when an effect is added to the manager.
 *
 * @param effectManager the effect manager that generated
 * the event
 * @param effect the effect object
 */
void RTIConnection::EffectAdded(
   EffectManager* effectManager,
   Effect* effect)
{
   if( !mIgnoreEffect && IS_A(effect, Detonation*) )
   {
      Detonation* detonation = (Detonation*)effect;

      RTI::ParameterHandleValuePairSet* theParameters =
         RTI::ParameterSetFactory::create(8);

      WorldCoordinate detonationLocation;
      VelocityVector  finalVelocity;
      EventIdentifier eventIdentifier;

      unsigned short warheadType;


      char encodedDetonationLocation[24],
           encodedEventIdentifier[5], 
           encodedWarheadType[2],
           encodedFuseType[2],
           encodedMunitionType[8],
           encodedDetonationResultCode[1],
           encodedQuantityFired[2],
           encodedFinalVelocity[12];
           

      sgVec3 vec;

      detonation->GetPosition(vec);

      sgXformVec3(vec, mRotationOffset);

      detonationLocation.SetX(vec[0] + mLocationOffset[0]);
      detonationLocation.SetY(vec[1] + mLocationOffset[1]);
      detonationLocation.SetZ(vec[2] + mLocationOffset[2]);
     

      detonationLocation.Encode(encodedDetonationLocation);

      theParameters->add(
         mDetonationLocationParameterHandle,
         encodedDetonationLocation,
         24
      );

     finalVelocity.SetX(0);  //test this
     finalVelocity.SetY(0);  //test this
     finalVelocity.SetZ(900);  //test this

     finalVelocity.Encode(encodedFinalVelocity);
     
     theParameters->add(
        mFinalVelocityVectorHandle,
        encodedFinalVelocity,
        12
     );

     
      eventIdentifier.SetEventIdentifier(mEventIdentifierCounter++);

      if(mEventIdentifierCounter == 0)
      {
         mEventIdentifierCounter = 1;
      }

      eventIdentifier.Encode(encodedEventIdentifier);

      theParameters->add(
         mEventIdentifierParameterHandle,
         encodedEventIdentifier,
         5  //changed this because of error
      );

      warheadType = 1000; // clamp to HE warhead

      if(ulIsLittleEndian)
      {
         ulEndianSwap(&warheadType);
      }

      *(unsigned short*)(&encodedWarheadType[0]) = warheadType;

      theParameters->add(
         mWarheadTypeParameterHandle,
         encodedWarheadType,
         2
      );

      *(unsigned short*)(&encodedFuseType[0]) = 1000; // 0 Other

      theParameters->add(
         mFuseTypeParameterHandle,
         encodedFuseType,
         2
      );

		unsigned short munType = (unsigned short)detonation->GetType();
      EntityType munitionType(2,9,255,2,14,2,0); //Default

		if (munType == 1000)
		{
			//munitionType.SetExtra(2);
			//std::cout<<"HE"<<std::endl;
		}
		else if(munType == 2000)
		{
			munitionType.SetSpecific(13);
		}
		else if(munType == 3000)
		{
			munitionType.SetSpecific(18);
		}
		else if(munType == 4000)
		{
         munitionType.SetExtra(1);
		}
		else if(munType == 5000)
		{
			munitionType.SetSpecific(3);
		}
		else if(munType == 6000)
		{
			munitionType.SetSpecific(18);
		}
      
      munitionType.Encode(encodedMunitionType);

      theParameters->add(
         mMunitionTypeParameterHandle,
         encodedMunitionType,
         8
      );

      encodedDetonationResultCode[0] = 5; // Detonation

      theParameters->add(
         mDetonationResultCodeParameterHandle,
         encodedDetonationResultCode,
         1
      );

      encodedQuantityFired[0] = 0;
      encodedQuantityFired[1] = 1;

      theParameters->add(
         mQuantityFiredParameterHandle,
         encodedQuantityFired,
         2
      );

      mRTIAmbassador.sendInteraction(
         mMunitionDetonationClassHandle,
         *theParameters,
         ""
      );
   }
}
