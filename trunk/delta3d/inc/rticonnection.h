// rticonnection.h: Declaration of the RTIConnection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DT_HLA_RTI_CONNECTION
#define DT_HLA_RTI_CONNECTION

#include <map>
#include <set>
#include <string>

#include <osgSim/DOFTransform>

#include "Producer/Timer"

#define RTI_USES_STD_FSTREAM

#include "rti.hh"
#include "NullFederateAmbassador.hh"

#include "sg.h"

#include "base.h"
#include "dis_types.h"
#include "entity.h"
#include "scene.h"
#include "effectmanager.h"

#define PI           3.14159265358979323e0    /* PI                        */
#define MIN_LAT      ( (-80.5 * PI) / 180.0 ) /* -80.5 degrees in radians    */
#define MAX_LAT      ( (84.5 * PI) / 180.0 )  /* 84.5 degrees in radians     */
#define MIN_EASTING  100000
#define MAX_EASTING  900000
#define MIN_NORTHING 0
#define MAX_NORTHING 10000000
#define MAX_DELTA_LONG  ((PI * 90)/180.0)    /* 90 degrees in radians */
#define MIN_SCALE_FACTOR  0.3
#define MAX_SCALE_FACTOR  3.0
#define PI_OVER_2  (PI / 2.0e0)
#define COS_67P5   0.38268343236508977  /* cosine of 67.5 degrees */
#define AD_C       1.0026000            /* Toms region 1 constant */


#define SPHTMD(Latitude) ((double) (TranMerc_ap * Latitude \
      - TranMerc_bp * sin(2.e0 * Latitude) + TranMerc_cp * sin(4.e0 * Latitude) \
      - TranMerc_dp * sin(6.e0 * Latitude) + TranMerc_ep * sin(8.e0 * Latitude) ) )

#define SPHSN(Latitude) ((double) (TranMerc_a / sqrt( 1.e0 - TranMerc_es * \
      pow(sin(Latitude), 2))))

#define SPHSR(Latitude) ((double) (TranMerc_a * (1.e0 - TranMerc_es) / \
    pow(DENOM(Latitude), 3)))

#define DENOM(Latitude) ((double) (sqrt(1.e0 - TranMerc_es * pow(sin(Latitude),2))))

         /* Ellipsoid Parameters, default to WGS 84  */
static double TranMerc_a = 6378137.0;              /* Semi-major axis of ellipsoid i meters */
static double TranMerc_f = 1 / 298.257223563;      /* Flattening of ellipsoid  */
static double TranMerc_es = 0.0066943799901413800; /* Eccentricity (0.08181919084262188000) squared */
static double TranMerc_ebs = 0.0067394967565869;   /* Second Eccentricity squared */

static double UTM_a = 6378137.0;         /* Semi-major axis of ellipsoid in meters  */
static double UTM_f = 1 / 298.257223563; /* Flattening of ellipsoid                 */
static long   UTM_Override = 0;          /* Zone override flag                      */

/* Transverse_Mercator projection Parameters */
static double TranMerc_Origin_Lat = 0.0;           /* Latitude of origin in radians */
static double TranMerc_Origin_Long = 0.0;          /* Longitude of origin in radians */
static double TranMerc_False_Northing = 0.0;       /* False northing in meters */
static double TranMerc_False_Easting = 0.0;        /* False easting in meters */
static double TranMerc_Scale_Factor = 1.0;         /* Scale factor  */

/* Isometeric to geodetic latitude parameters, default to WGS 84 */
static double TranMerc_ap = 6367449.1458008;
static double TranMerc_bp = 16038.508696861;
static double TranMerc_cp = 16.832613334334;
static double TranMerc_dp = 0.021984404273757;
static double TranMerc_ep = 3.1148371319283e-005;

/* Maximum variance for easting and northing values for WGS 84. */
static double TranMerc_Delta_Easting = 40000000.0;
static double TranMerc_Delta_Northing = 40000000.0;

/* Ellipsoid parameters, default to WGS 84 */
static double Geocent_a = 6378137.0;          /* Semi-major axis of ellipsoid in meters */
static double Geocent_f = 1 / 298.257223563;  /* Flattening of ellipsoid           */

static double Geocent_e2 = 0.0066943799901413800;   /* Eccentricity squared  */
static double Geocent_ep2 = 0.00673949675658690300; /* 2nd eccentricity squared */


namespace dtHLA
{
   class DetonationListener;

   struct MasterData;
   struct GhostData;


   /**
    * Represents a connection to the HLA run-time infrastructure.
    */
   class RTIConnection : public dtCore::Base,
                         public dtCore::EffectListener,
                         public NullFederateAmbassador
   {
      DECLARE_MANAGEMENT_LAYER(RTIConnection)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         RTIConnection(std::string name = "RTIConnection");

         /**
          * Destructor.
          */
         virtual ~RTIConnection();

         /**
          * Creates/joins a federation execution.
          *
          * @param executionName the name of the federation execution to join
          * @param fedFilename the fed filename
          * @param federateName the name of this federate
          */
         void JoinFederationExecution(std::string executionName = "dtCore",
                                      std::string fedFilename = "RPR-FOM.fed",
                                      std::string federateName = "Participant");
         
         /**
          * Leaves/destroys the joined execution.
          */
         void LeaveFederationExecution();
         
         /**
          * Returns the name of the joined execution, or "" if unjoined.
          *
          * @return the execution name
          */
         std::string GetExecutionName();
         
         /**
          * Sets the scene in which ghost entities are created.
          *
          * @param scene the target scene
          */
         void SetScene(dtCore::Scene* scene);

         /**
          * Returns the scene in which ghost entities are created.
          *
          * @return the target scene
          */
         dtCore::Scene* GetScene();

         /**
          * Sets the effect manager.
          *
          * @param effectManager the effect manager
          */
         void SetEffectManager(dtCore::EffectManager* effectManager);

         /**
          * Returns the effect manager.
          *
          * @return the effect manager
          */
         dtCore::EffectManager* GetEffectManager();
         
         /**
          * Enables or disables globe mode, where entities are rendered as icons at
          * scaled-down geocentric coordinates.
          *
          * @param enable true to enable globe mode, false to disable it
          */
         void EnableGlobeMode(bool enable = true);

         /**
          * Enables or disables UTM mode, Automatically converts Geocentric to UTM
          *
          * @param enable true to enable UTM mode, false to disable it
          */
         void EnableUTMMode(bool enable = true);
         
         /**
          * Checks whether or not globe mode is enabled.
          *
          * @return true if globe mode is enabled, false otherwise
          */
         bool GlobeModeEnabled();
         
         /**
          * Sets the globe radius.
          *
          * @param radius the new radius
          */
         void SetGlobeRadius(float radius);
         
         /**
          * Returns the globe radius.
          *
          * @return the current globe radius
          */
         float GetGlobeRadius();
         
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
          * Sets the DIS/RPR-FOM site identifier.
          *
          * @param siteIdentifier the new site identifier
          */
         void SetSiteIdentifier(unsigned short siteIdentifier);

         /**
          * Returns the DIS/RPR-FOM site identifier.
          *
          * @return the site identifier
          */
         unsigned short GetSiteIdentifier() const;

         /**
          * Sets the DIS/RPR-FOM application identifier.
          *
          * @param applicationIdentifier the new application identifier
          */
         void SetApplicationIdentifier(unsigned short applicationIdentifier);

         /**
          * Returns the DIS/RPR-FOM application identifier.
          *
          * @return the DIS/RPR-FOM application identifier
          */
         unsigned short GetApplicationIdentifier() const;

         /** 
          * Registers a locally owned master entity, announcing its presence
          * to the federation.
          *
          * @param entity the entity to register
          */
         void RegisterMasterEntity(Entity* entity);

         /**
          * Deregisters a locally owned master entity.
          *
          * @param entity the entity to deregister
          */
         void DeregisterMasterEntity(Entity* entity);

         /**
          * Returns the number of active master entities.
          *
          * @return the number of master entities
          */
         int GetMasterEntityCount();
         
         /**
          * Returns the master entity at the specified index.
          *
          * @param index the index
          * @return the master entity at the specified index
          */
         Entity* GetMasterEntity(int index);
         
         /**
          * Returns the number of active ghost entities.
          *
          * @return the number of ghost entities
          */
         int GetGhostEntityCount();
         
         /**
          * Returns the ghost entity at the specified index.
          *
          * @param index the index
          * @return the ghost entity at the specified index
          */
         Entity* GetGhostEntity(int index);
         
         /**
          * Maps the specified entity type to the given filename.
          *
          * @param entityType the entity type to map
          * @param modelFilename the entity model filename
          * @param iconFilename the entity icon filename
          */
         void AddEntityTypeMapping(const EntityType& entityType,
                                   std::string modelFilename,
                                   std::string iconFilename = "");

         /**
          * Maps the specified entity type to the given filename.
          *
          * @param entityType the entity type to map
          * @param modelFilename the entity model filename
          * @param articulatedPartClassNameMap the map from articulated part class IDs
          * to names of DOFTransform nodes
          * @param iconFilename the entity icon filename
          */
         void AddEntityTypeMapping(const EntityType& entityType,
                                   std::string modelFilename,
                                   const std::map<unsigned int, std::string>&
                                    articulatedPartClassNameMap,
                                   std::string iconFilename = "");
                                   
         /**
          * Removes the mapping for the given entity type.
          *
          * @param entityType the entity type to unmap
          */
         void RemoveEntityTypeMapping(const EntityType& entityType);

         /**
          * Loads a set of entity type mappings from an XML file.
          *
          * @param filename the name of the file to load
          * @return true if mappings successfully loaded, false otherwise
          */
         bool LoadEntityTypeMappings(std::string filename);

         /**
          * Ground clamp modes.
          */
         enum GroundClampMode
         {
            NO_CLAMP,
            CLAMP_ELEVATION,
            CLAMP_ELEVATION_AND_ROTATION
         };
         
         /**
          * Sets the ground clamp mode.
          *
          * @param mode the new ground clamp mode
          */
         void SetGroundClampMode(GroundClampMode mode);
         
         /**
          * Returns the ground clamp mode.
          *
          * @return the current ground clamp mode
          */
         GroundClampMode GetGroundClampMode();
         
         /**
          * Adds a detonation listener.
          *
          * @param listener the listener to add
          */
         void AddDetonationListener(DetonationListener* listener);
         
         /**
          * Removes a detonation listener.
          *
          * @param listener the listener to remove
          */
         void RemoveDetonationListener(DetonationListener* listener);
         
         
         /**
          * Processes a received message.
          *
          * @param data the message structure
          */
         virtual void OnMessage(MessageData *data);



         /**
          * Invoked by the RTI ambassador to notify the federate of a new object
          * instance.
          *
          * @param theObject the handle of the discovered object
          * @param theObjectClass the handle of the discovered object's class
          * @param theObjectName the name of the discovered object
          */
         virtual void discoverObjectInstance(
            RTI::ObjectHandle theObject,
            RTI::ObjectClassHandle theObjectClass,
            const char* theObjectName
         );

         /**
          * Invoked by the RTI ambassador to request that the federate provide
          * updated attribute values for the specified object.
          *
          * @param theObject the handle of the object of interest
          * @param theAttributes the set of attributes to update
          */
         virtual void provideAttributeValueUpdate(
            RTI::ObjectHandle theObject,
            const RTI::AttributeHandleSet& theAttributes
         );

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
         virtual void reflectAttributeValues(
            RTI::ObjectHandle theObject,
            const RTI::AttributeHandleValuePairSet& theAttributes,
            const RTI::FedTime& theTime,
            const char *theTag,
            RTI::EventRetractionHandle theHandle
         );

         /**
          * Invoked by the RTI ambassador to notify the federate of updated object
          * attribute values.
          *
          * @param theObject the handle of the modified object
          * @param theAttributes the new attribute values
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void reflectAttributeValues(
            RTI::ObjectHandle theObject,
            const RTI::AttributeHandleValuePairSet& theAttributes,
            const char *theTag
         );

         /**
          * Invoked by the RTI ambassador to notify the federate of a deleted object
          * instance.
          *
          * @param theObject the handle of the removed object
          * @param theTime the event timestamp
          * @param theTag the user-supplied tag associated with the event
          * @param theHandle the event retraction handle
          */
         virtual void removeObjectInstance(
            RTI::ObjectHandle theObject,
            const RTI::FedTime& theTime,
            const char *theTag,
            RTI::EventRetractionHandle theHandle
         );

         /**
          * Invoked by the RTI ambassador to notify the federate of a removed object
          * instance.
          *
          * @param theObject the handle of the removed object
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void removeObjectInstance(
            RTI::ObjectHandle theObject,
            const char *theTag
         );

         /**
          * Invoked by the RTI ambassador to notify the federate of a received
          * interaction.
          *
          * @param theInteraction the handle of the received interaction
          * @param theParameters the parameters of the interaction
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void receiveInteraction(
            RTI::InteractionClassHandle theInteraction,
            const RTI::ParameterHandleValuePairSet& theParameters,
            const char *theTag
         );

         /**
          * Called when an effect is added to the manager.
          *
          * @param effectManager the effect manager that generated
          * the event
          * @param effect the effect object
          */
         virtual void EffectAdded(
            dtCore::EffectManager* effectManager,
            dtCore::Effect* effect
         );


      private:

         /**
          * Creates a 4x4 rotation matrix from a set of DIS/RPR-FOM Euler angles.
          *
          * @param dst the destination matrix
          * @param psi the psi angle
          * @param theta the theta angle
          * @param phi the phi angle
          */
         static void EulersToMatrix(sgMat4 dst, float psi, float theta, float phi);

         /**
          * Returns the DIS/RPR-FOM Euler angles corresponding to the given rotation
          * matrix.
          *
          * @param src the source matrix
          * @param psi the location in which to store the psi angle
          * @param theta the location in which to store the theta angle
          * @param phi the location in which to store the phi angle
          */
         static void MatrixToEulers(sgMat4 src, float* psi, float* theta, float* phi);

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
          * Clamps the specified entity to the ground using the active ground clamp mode.
          *
          * @param entity the entity to clamp
          */
         void ClampToGround(Entity* entity);
         
         /**
          * The RTI ambassador.
          */
         RTI::RTIambassador mRTIAmbassador;
         //NullFederateAmbassador mRTIAmbassador;

         /**
          * The named of the joined execution.
          */
         std::string mExecutionName;

         /**
          * The base entity class handle.
          */
         RTI::ObjectClassHandle mBaseEntityClassHandle;

         /**
          * The physical entity class handle.
          */
         RTI::ObjectClassHandle mPhysicalEntityClassHandle;

         /**
          * The platform class handle.
          */
         RTI::ObjectClassHandle mPlatformClassHandle;

         /**
          * The aircraft entity class handle.
          */
         RTI::ObjectClassHandle mAircraftClassHandle;

         /**
          * The ground vehicle entity class handle.
          */
         RTI::ObjectClassHandle mGroundVehicleClassHandle;
         
         /**
          * The life form entity class handle.
          */
         RTI::ObjectClassHandle mLifeFormClassHandle;

         /**
          * The human entity class handle.
          */
         RTI::ObjectClassHandle mHumanClassHandle;

         /**
          * The acceleration vector attribute handle.
          */
         RTI::AttributeHandle mAccelerationVectorAttributeHandle;
         
         /**
          * The angular velocity attribute handle.
          */
         RTI::AttributeHandle mAngularVelocityVectorAttributeHandle;
         
         /**
          * The dead reckoning algorithm attribute handle.
          */
         RTI::AttributeHandle mDeadReckoningAlgorithmAttributeHandle;

         /**
          * The entity identifier attribute handle.
          */
         RTI::AttributeHandle mEntityIdentifierAttributeHandle;

         /**
          * The entity type attribute handle.
          */
         RTI::AttributeHandle mEntityTypeAttributeHandle;

         /**
          * The world location attribute handle.
          */
         RTI::AttributeHandle mWorldLocationAttributeHandle;

         /**
          * The orientation attribute handle.
          */
         RTI::AttributeHandle mOrientationAttributeHandle;

         /**
          * The velocity vector attribute handle.
          */
         RTI::AttributeHandle mVelocityVectorAttributeHandle;

         /**
          * The articulated parameters array attribute handle.
          */
         RTI::AttributeHandle mArticulatedParametersArrayAttributeHandle;
         
         /**
          * The damage-state attribute handle.
          */
         RTI::AttributeHandle mDamageStateAttributeHandle;

         /**
          * The force-identifier attribute handle.
          */
         RTI::AttributeHandle mForceIdentifierAttributeHandle;

         /**
          * The marking-attribute attribute handle.
          */
         RTI::AttributeHandle mMarkingAttributeHandle;

         /**
          * The fire-power-disabled attribute handle.
          */
         RTI::AttributeHandle mFirePowerDisabledAttributeHandle;

         /**
          * The immobilized attribute handle.
          */
         RTI::AttributeHandle mImmobilizedAttributeHandle;

         /**
          * The camouflage-type attribute handle.
          */
         RTI::AttributeHandle mCamouflageTypeAttributeHandle;

         /**
          * The is-concealed attribute handle.
          */
         RTI::AttributeHandle mIsConcealedAttributeHandle;

         /**
          * The trailing-effects-code attribute handle.
          */
         RTI::AttributeHandle mTrailingEffectsCodeAttributeHandle;

         /**
          * The power-plant-on attribute handle.
          */
         RTI::AttributeHandle mPowerPlantOnAttributeHandle;

         /**
          * The munition detonation class handle.
          */
         RTI::InteractionClassHandle mMunitionDetonationClassHandle;

         /**
          * The detonation location parameter handle.
          */
         RTI::ParameterHandle mDetonationLocationParameterHandle;

         /**
          * The event identifier parameter handle.
          */
         RTI::ParameterHandle mEventIdentifierParameterHandle;

         /**
          * The fuse type parameter handle.
          */
         RTI::ParameterHandle mFuseTypeParameterHandle;

         /**
          * The munition type parameter handle.
          */
         RTI::ParameterHandle mMunitionTypeParameterHandle;

         /**
          * The warhead type parameter handle.
          */
         RTI::ParameterHandle mWarheadTypeParameterHandle;

         /**
          * The detonation result code parameter handle.
          */
         RTI::ParameterHandle mDetonationResultCodeParameterHandle;

         /**
          * The quantity fired parameter handle.
          */
         RTI::ParameterHandle mQuantityFiredParameterHandle;

         /**
         * The Final Velocity Vector Handle
         */

         RTI::ParameterHandle mFinalVelocityVectorHandle;
         /**
         * The Articulated Part Data Handle
         */
         RTI::ParameterHandle mArticulatedPartDataHandle;

         /**
         * The Firing Object Identifier Handle
         */

         RTI::ParameterHandle mFiringObjectIdentifierHandle;
         
         /**
         * The Munition Object Identifier Handle
         */

         RTI::ParameterHandle mMunitionObjectIdentifierHandle;

         /**
         * The Rate Of Fire Handle
         */

         RTI::ParameterHandle mRateOfFireHandle;

         /**
         * The Relative Detonation Location Handle
         */

         RTI::ParameterHandle mRelativeDetonationLocationHandle;

         /**
         * The Target Object Identifier Handle
         */

         RTI::ParameterHandle mTargetObjectIdentifierHandle;
         /**
          * The scene in which to create ghost entities.
          */
         osg::ref_ptr<dtCore::Scene> mScene;

         /**
          * The effect manager.
          */
         osg::ref_ptr<dtCore::EffectManager> mEffectManager;

         /**
          * Whether or not globe mode is enabled.
          */
         bool mGlobeModeEnabled;
         
         /**
          * Whether or not UTM mode is enabled.
          */
         bool mUTMModeEnabled;

         /**
          * The radius of the globe.
          */
         float mGlobeRadius;
         
         /**
          * Whether or not to ignore effect events.
          */
         bool mIgnoreEffect;

         /**
          * The location of the origin in geocentric coordinates.
          */
         double mLocationOffset[3];

         /**
          * The rotation offset matrix.
          */
         sgMat4 mRotationOffset;

         /**
          * The rotation offset matrix inverse.
          */
         sgMat4 mRotationOffsetInverse;

         /**
          * The IP address of the local machine.
          */
         unsigned int mLocalIPAddress;
         
         /**
          * The DIS/RPR-FOM site identifier.
          */
         unsigned short mSiteIdentifier;

         /**
          * The DIS/RPR-FOM application identifier.
          */
         unsigned short mApplicationIdentifier;

         /**
          * The DIS/RPR-FOM entity identifier counter.
          */
         unsigned short mEntityIdentifierCounter;

         /**
          * The DIS/RPR-FOM event identifier counter.
          */
         unsigned short mEventIdentifierCounter;

         

         long     mZone;
         char     mHemisphere;
         double   mEasting;
         double   mNorthing;
         double    mLat;
         double   mLong;
         double   mElevation;



         /**
          * Information associated with an entity type.
          */

         struct EntityTypeMapping
         {
            /**
             * The filename of the entity's model.
             */
            std::string mModelFilename;
            
            /**
             * Maps articulated part classes to DOFTransform names.
             */
            std::map<unsigned int, std::string> mArticulatedPartClassNameMap;
            
            /**
             * The filename of the entity's icon.
             */
            std::string mIconFilename;
         };
         
         /**
          * Maps entity types to associated information.
          */
         std::map<EntityType, EntityTypeMapping> mEntityTypeMappings;

         /**
          * Finds the best (most specific) mapping for the specified
          * entity type.
          *
          * @param entityType the entity type to map
          * @return the best mapping, or NULL if no mapping could be found
          */
         EntityTypeMapping* FindBestMapping(const EntityType& entityType);
         
         /**
          * An internal structure for data associated with master entities.
          */
         struct MasterData
         {
            /**
             * The dtCore entity.
             */
            osg::ref_ptr<Entity> mEntity;

            /**
             * The last transform transmitted.
             */
            dtCore::Transform mTransform;
         };

         /**
          * Maps object instance handles to master entity data.
          */
         std::map<RTI::ObjectHandle, MasterData> mObjectHandleMasterDataMap;

         /**
          * The set of master entities.
          */
         std::set< osg::ref_ptr<Entity> > mMasterEntities;
         
         /**
          * An internal structure for data associated with ghost entities.
          */
         struct GhostData
         {
            /**
             * The dtCore entity.
             */
            osg::ref_ptr<Entity> mEntity;
            
            /**
             * Maps articulated part transforms to DOFTransform nodes.
             */
            std::map<unsigned int, osgSim::DOFTransform*>
               mArticulatedPartClassTransformMap;
         };

         /**
          * Maps object instance handles to ghost entity data.
          */
         std::map<RTI::ObjectHandle, GhostData> mObjectHandleGhostDataMap;

         /**
          * The ground clamp mode.
          */
         GroundClampMode mGroundClampMode;
         
         /**
          * The set of detonation listeners.
          */
         std::set<DetonationListener*> mDetonationListeners;
         

         /**
          * A Producer timer object.
          */
         Producer::Timer mTimer;

         /**
          * The time at which the last heartbeat was issued.
          */
         Producer::Timer_t mLastHeartbeatTime;

         /**
          * The set of object handles discovered in the last tick.
          */
         std::set<RTI::ObjectHandle> mNewlyDiscoveredObjects;

         /**
          * The set of objects for which to provide updates.
          */
         std::set<RTI::ObjectHandle> mObjectsToUpdate;
   };


    /**
    * An interface for objects interested in detonation events.
    */
   class DetonationListener
   {
      public:
      
         /**
          * Called when a detonation occurs.
          *
          * @param detonationLocation the location of the detonation in geocentric coordinates
          * @param eventIdentifier the identifier of the detonation event
          * @param fuseType the type of fuse
          * @param munitionType the type of munition detonated
          * @param warheadType the type of warhead
          * @param detonationResultCode the detonation result
          * @param quantityFired the quantity fired
          */
         virtual void DetonationOccurred(
            WorldCoordinate detonationLocation,
            EventIdentifier eventIdentifier,
            unsigned short fuseType,
            EntityType munitionType,
            unsigned short warheadType,
            unsigned char detonationResultCode,
            unsigned short quantityFired
         ) {}
   };
};

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#if defined(_DEBUG)
#define _AUTOLIBNAME  "libRTI-NGd.lib"
#define _AUTOLIBNAME1  "tinyxmld.lib"
#define _AUTOLIBNAME2  "ws2_32.lib"
#else
#define _AUTOLIBNAME  "libRTI-NG.lib"
#define _AUTOLIBNAME1  "tinyxml.lib"
#define _AUTOLIBNAME2  "ws2_32.lib"
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#pragma message( "Will automatically link with " _AUTOLIBNAME1 )
#pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)

#endif // DT_HLA_RTI_CONNECTION
