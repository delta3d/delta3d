//////////////////////////////////////////////////////////////////////////////
//
//      File Name:      isense.h
//      Description:    Header File for InterSense DLL
//      Created:        12/4/98
//      Author:         Yury Altshuler
//
//      Copyright:      InterSense 2002 - All rights Reserved.
//
//      Comments:       This dynamic link library is provided to simplify
//                      communications with all models of InterSense tracking devices.
//                      Currently, these include IS-300, IS-600, IS-900, InertiaCube2 and
//                      InterTrax. It can detect, configure, and get data from up to 8 trackers.
//
//                      DLL provides two methods of configuring the Precision Series trackers.
//                      You can use the provided function calls and hardcode the
//                      settings in your application, or use the isenseX.ini files.
//                      Second method is strongly recommended, as it provides you with
//                      ability to change the configuration without recompiling your
//                      application. In the isenseX.ini file name X is a number, starting
//                      at 1, identifying one tracking system in the order of
//                      initialization. Use included isense1.ini file as example.
//                      Only enter the settings you want the program to change.
//
//                      NOTE: Configuration files are not supported on UNIX in this release.
//
//                      InterTrax requires no configuration. All function calls and
//                      settings in the isenseX.ini files are ignored.
//
//
//////////////////////////////////////////////////////////////////////////////
#ifndef _ISD_isenseh
#define _ISD_isenseh


#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
    #include <windows.h>
    #include "types.h"
#else
    #include "types.h"
    #ifndef UNIX
        #define UNIX
    #endif
#endif

#ifndef TRUE
    #define TRUE    1
    #define FALSE   0
#endif

//////////////////////////////////////////////////////////////////////////////
//
// This defines the calling conventions of the entry points to the
// DLL. This varies depending upon the operating system.
//
//////////////////////////////////////////////////////////////////////////////

#if defined _LIB // static library
    #define DLLEXPORT
    #define DLLENTRY __cdecl
    typedef void (* DLL_EP)(void);
    #define DLL_EP_PTR __cdecl *
#else

#if defined UNDER_RTSS
    #define DLLEXPORT __declspec(dllexport)
    #define DLLENTRY _stdcall
    typedef void (* DLL_EP)(void);
    #define DLL_EP_PTR _stdcall *
#elif defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
    #define DLLEXPORT __declspec(dllexport)
    #define DLLENTRY __cdecl
    typedef void (* DLL_EP)(void);
    #define DLL_EP_PTR __cdecl *
#else
    #define DLLEXPORT
    #define DLLENTRY
    typedef void (* DLL_EP)(void);
    #define DLL_EP_PTR *
#endif
#endif

//////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C"
{
#endif


// tracking system type
typedef enum
{
    ISD_NONE = 0,           // none found, or can't identify
    ISD_PRECISION_SERIES,   // InertiaCube2, IS-300, IS-600, IS-900 and IS-1200
    ISD_INTERTRAX_SERIES    // InterTrax
}
ISD_SYSTEM_TYPE;


// tracking system model
typedef enum
{
    ISD_UNKNOWN = 0,
    ISD_IS300,          // 3DOF system
    ISD_IS600,          // 6DOF system
    ISD_IS900,          // 6DOF system
    ISD_INTERTRAX,      // InterTrax (Serial)
    ISD_INTERTRAX_2,    // InterTrax (USB)
    ISD_INTERTRAX_LS,   // InterTraxLS, verification required
    ISD_INTERTRAX_LC,   // InterTraxLC
    ISD_ICUBE2,         // InertiaCube2
    ISD_ICUBE2_PRO,     // InertiaCube2 Pro
    ISD_IS1200,         // 6DOF system
    ISD_ICUBE3,         // InertiaCube3
    ISD_ICUBE4,         // InertiaCube4
    ISD_INTERTRAX_3,    // InterTrax3
    ISD_IMUK,           // K-Sensor
    ISD_ICUBE2B_PRO     // InertiaCube2B Pro
}
ISD_SYSTEM_MODEL;


typedef enum
{
    ISD_INTERFACE_UNKNOWN = 0,
    ISD_INTERFACE_SERIAL,
    ISD_INTERFACE_USB,
    ISD_INTERFACE_ETHERNET_UDP,
    ISD_INTERFACE_ETHERNET_TCP,
    ISD_INTERFACE_IOCARD,
    ISD_INTERFACE_PCMCIA,
    ISD_INTERFACE_FILE
}
ISD_INTERFACE_TYPE;


#if defined ISENSE_LIMITED
#define ISD_MAX_TRACKERS        2
#define ISD_MAX_STATIONS        4
#else
#define ISD_MAX_TRACKERS        32
#define ISD_MAX_STATIONS        8
#endif

// orientation format
#define ISD_EULER               1
#define ISD_QUATERNION          2

// Coordinate frame in which position and orientation data is reported
#define ISD_DEFAULT_FRAME       1    // InterSense default
#define ISD_VSET_FRAME          2    // Virtual set frame, use for camera tracker only

// number of supported stylus buttons
#define ISD_MAX_BUTTONS         8

// hardware is limited to 10 analog/digital input channels per station
#define ISD_MAX_CHANNELS        10

// maximum supported number of bytes for auxiliary input data
#define ISD_MAX_AUX_INPUTS      4

// maximum supported number of bytes for auxiliary output data
#define ISD_MAX_AUX_OUTPUTS     4

typedef int ISD_TRACKER_HANDLE;


///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    // Following item are for information only and should not be changed

    float  LibVersion;     // InterSense Library version

    DWORD  TrackerType;    // IS Precision series or InterTrax.
                           // TrackerType can be:
                           // ISD_PRECISION_SERIES for IS-300, IS-600, IS-900 and IS-1200 model trackers,
                           // ISD_INTERTRAX_SERIES for InterTrax, or
                           // ISD_NONE if tracker is not initialized

    DWORD  TrackerModel;   // ISD_UNKNOWN, ISD_IS300, ISD_IS600, ISD_IS900, ISD_INTERTRAX

    DWORD  Port;           // Number of the rs232 port. Starts with 1.

    // Communications statistics. For information only.

    DWORD  RecordsPerSec;
    float  KBitsPerSec;

    // Following items are used to configure the tracker and can be set in
    // the isenseX.ini file

    DWORD  SyncState;   // 4 states: 0 - OFF, system is in free run
                        //           1 - ON, hardware genlock frequency is automatically determined
                        //           2 - ON, hardware genlock frequency is specified by the user
                        //           3 - ON, no hardware signal, lock to the user specified frequency

    float  SyncRate;    // Sync frequency - number of hardware sync signals per second,
                        // or, if SyncState is 3 - data record output frequency

    DWORD  SyncPhase;   // 0 to 100 per cent

    DWORD  Interface;   // hardware interface, read-only

    DWORD  UltTimeout;  // IS-900 only, ultrasonic timeout (sampling rate)
    DWORD  UltVolume;   // IS-900 only, ultrasonic speaker volume
    DWORD  dwReserved4;

    float  FirmwareRev; // Firmware revision
    float  fReserved2;
    float  fReserved3;
    float  fReserved4;

    Bool   LedEnable;   // IS-900 only, blue led on the SoniDiscs enable flag
    Bool   bReserved2;
    Bool   bReserved3;
    Bool   bReserved4;
}
ISD_TRACKER_INFO_TYPE;



///////////////////////////////////////////////////////////////////////////////

// ISD_STATION_INFO_TYPE can only be used with IS Precision Series tracking devices.
// If passed to ISD_SetStationConfig or ISD_GetStationConfig with InterTrax, FALSE is returned.

typedef struct
{
    DWORD   ID;             // unique number identifying a station. It is the same as that
                            // passed to the ISD_SetStationConfig and ISD_GetStationConfig
                            // functions and can be 1 to ISD_MAX_STATIONS

    Bool    State;          // TRUE if ON, FALSE if OFF

    Bool    Compass;        // 0 or 2 for OFF or ON. Setting 1 is not in use and will
                            // have the same result as ON.
                            // Compass setting is ignored if station is configured for
                            // Fusion Mode operation.

    LONG    InertiaCube;    // InertiaCube associated with this station. If no InertiaCube is
                            // assigned, this number is -1. Otherwise, it is a positive number
                            // 1 to 4

    DWORD   Enhancement;    // levels 0, 1, or 2
    DWORD   Sensitivity;    // levels 1 to 4
    DWORD   Prediction;     // 0 to 50 ms
    DWORD   AngleFormat;    // ISD_EULER or ISD_QUATERNION

    Bool    TimeStamped;    // TRUE if time stamp is requested
    Bool    GetInputs;      // TRUE if button and joystick data is requested
    Bool    GetEncoderData; // TRUE if raw encoder data is requested

    // This setting controls how Magnetic Environment Calibration is applied. This Callibration
    // calculates nominal field strength and dip angle for the environment in which sensor
    // is used. Based on these values system can assign weight to compass measurements,
    // allowing for bad measurements to be rejected. Values from 0 to 3 are accepted.
    // If CompassCompensation is set to 0, the calibration is ignored and all compass data is
    // used. Higher values result in tighter rejection threshold, resulting in more measurements
    // being rejected. If sensor is used in an environment with a lot of magnetic inteference
    // this can result in drift due to insuficient compensation from the compass data. Default
    // setting is 2.
    // -----------------------------------------------------------------------------------------
    BYTE    CompassCompensation;

    // This setting controls how the system deals with sharp changes in IMU data that can
    // be caused by shock or impact. Sensors may experience momentary rotation rates or
    // accelerations that are outside of the specified range, resulting in undesirable
    // behaviour. By turning on shock suppression you can have the system filter out
    // corrupted data. Values 0 (OFF) to 2 are accepted, with higher values resulting in
    // greated filterring.
    // -----------------------------------------------------------------------------------------
    BYTE    ImuShockSuppression;

    // This setting controls the rejection threshold for ultrasonic measurements. Currently
    // implemented only for PC Tracker. Default setting is 4, which results in measurements
    // with range error greater than 4 times the averate to be rejected. Please do not change
    // this setting without consulting with InterSense technical support.
    // -----------------------------------------------------------------------------------------
    BYTE    UrmRejectionFactor;

    BYTE    bReserved2;

    DWORD   CoordFrame;     // coord frame in which position and orientation data is reported

    // AccelSensitivity is used for 3-DOF tracking with InertiaCube products only. It controls how
    // fast tilt correction, using accelerometers, is applied. Valid values are 1 to 4, with 2 as default.
    // Default is best for head tracking in static environment, with user seated.
    // Level 1 reduces the amount of tilt correction during movement. While it will prevent any effect
    // linear accelerations may have on pitch and roll, it will also reduce stability and dynamic accuracy.
    // It should only be used in situations when sensor is not expected to experience a lot of movement.
    // Level 3 allows for more aggressive tilt compensation, appropriate when sensor is moved a lot,
    // for example, when user is walking for long durations of time.
    // Level 4 allows for even greater tilt corrections. It will reduce orientation accuracy by
    // allowing linear accelerations to effect orientation, but increase stability. This level
    // is appropriate for when user is running, or in other situations when sensor experiences
    // a great deal of movement.
    // -----------------------------------------------------------------------------------------
    DWORD   AccelSensitivity;

    DWORD   dwReserved3;
    DWORD   dwReserved4;

    float   TipOffset[3];   // coordinates in station frame of the point being tracked
    float   fReserved4;

    Bool    GetCameraData;  // TRUE to get computed FOV, aperture, etc
    Bool    GetAuxInputs;
    Bool    GetCovarianceData;
    Bool    bReserved4;
}
ISD_STATION_INFO_TYPE;


///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    BYTE    TrackingStatus;     // tracking status byte
    BYTE    NewData;            // TRUE if data changed since last call to ISD_Get_Data
    BYTE    CommIntegrity;      // Communication integrity of wireless link
    BYTE    BatteryState;       // Wireless devices only 0=n/a, 1=low, 2=ok

    float   Euler[3];           // Orientation in Euler angle format
    float   Quaternion[4];      // Orientation in Quaternion format
    float   Position[3];        // Always in meters
    float   TimeStamp;          // Seconds, reported only if requested
    float   StillTime;          // InertiaCube and PC-Tracker products only
    float   BatteryLevel;       // Battery Voltage, if available
    float   CompassYaw;         // Magnetometer heading, computed based on current orientation.

    Bool    ButtonState[ISD_MAX_BUTTONS];    // Only if requested

    // -----------------------------------------------------------------------------------------
    // Current hardware is limited to 10 channels, only 2 are used.
    // The only device using this is the IS-900 wand that has a built-in
    // analog joystick. Channel 1 is x-axis rotation, channel 2 is y-axis
    // rotation

    short   AnalogData[ISD_MAX_CHANNELS]; // only if requested

    BYTE    AuxInputs[ISD_MAX_AUX_INPUTS];


    float   AngularVelBodyFrame[3]; // rad/sec, in sensor body coordinate frame.
                                    // This is the processed angular rate, with current biases
                                    // removed. This is the angular rate used to produce
                                    // orientation updates.

    float   AngularVelNavFrame[3];  // rad/sec, in world coordinate frame, with boresight and other
                                    // transformations applied.

    float   AccelBodyFrame[3];      // meter^2/sec, in sensor body coordinate frame. This is
                                    // the accelerometer measurements in the sensor body coordinate
                                    // frame. Only factory calibration is applied to this data,
                                    // gravity component is not removed.

    float   AccelNavFrame[3];       // meters/sec^2, in the navigation (earth) coordinate frame.
                                    // This is the accelerometer measurements with calibration,
                                    // and current sensor orientation applied, and gravity
                                    // subtracted. This is the best available estimate of
                                    // acceleration.

    float   VelocityNavFrame[3];    // meters/sec, 6-DOF systems only.

    float   AngularVelRaw[3];       // Raw gyro output, only factory calibration is applied.
                                    // Some errors due to temperature dependant gyro bias
                                    // drift will remain.

    DWORD   Reserved[64];
}
ISD_STATION_DATA_TYPE;


///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    BYTE    TrackingStatus;     // tracking status byte
    BYTE    bReserved1;         // pack to 4 byte boundary
    BYTE    bReserved2;
    BYTE    bReserved3;

    DWORD   Timecode;           // timecode, not implemented yet
    LONG    ApertureEncoder;    // Aperture encoder counts, relative to last reset of power up
    LONG    FocusEncoder;       // Focus encoder counts
    LONG    ZoomEncoder;        // Zoom encoded counts
    DWORD   TimecodeUserBits;   // Time code user bits, not implemented yet

    float   Aperture;           // Computed Aperture value
    float   Focus;              // Computed focus value (mm), not implemented yet
    float   FOV;                // Computed vertical FOV value (degrees)
    float   NodalPoint;         // Nodal point offset due to zoom and focus (mm)

    float   CovarianceOrientation[3];     // Available only for IS-1200
    float   CovariancePosition[3];

    DWORD   dwReserved1;
    DWORD   dwReserved2;

    float   fReserved1;
    float   fReserved2;
    float   fReserved3;
    float   fReserved4;
}
ISD_CAMERA_ENCODER_DATA_TYPE;


typedef struct
{
    ISD_STATION_DATA_TYPE Station[ISD_MAX_STATIONS];
}
ISD_TRACKING_DATA_TYPE;


typedef struct
{
    ISD_CAMERA_ENCODER_DATA_TYPE Camera[ISD_MAX_STATIONS];
}
ISD_CAMERA_DATA_TYPE;


typedef enum
{
    ISD_AUX_SYSTEM_NONE = 0,
    ISD_AUX_SYSTEM_ULTRASONIC,
    ISD_AUX_SYSTEM_OPTICAL,
    ISD_AUX_SYSTEM_MAGNETIC,
    ISD_AUX_SYSTEM_RF,
    ISD_AUX_SYSTEM_GPS
}
ISD_AUX_SYSTEM_TYPE;


typedef struct
{
    Bool    Valid;            // set to TRUE if ISD_GetSystemHardwareInfo succeeded

    DWORD   TrackerType;      // see ISD_SYSTEM_TYPE
    DWORD   TrackerModel;     // see ISD_SYSTEM_MODEL
    DWORD   Port;             // hardware port number (Com1, etc.)
    DWORD   Interface;        // hardware interface (RS232, USB, etc.)
    Bool    OnHost;           // tracking algorithms are executed in the dll
    DWORD   AuxSystem;        // position tracking hardware, see ISD_AUX_SYSTEM_TYPE
    float   FirmwareRev;      // Firmware revision

    char    ModelName[128];

    struct
    {
        Bool    Position;     // can tracker position
        Bool    Orientation;  // can tracker orientation
        Bool    Encoders;     // can support lens encoders
        Bool    Prediction;   // predictive algorithms are available
        Bool    Enhancement;  // enhancement level can be changed
        Bool    Compass;      // compass setting can be changed
        Bool    SelfTest;     // has the self-test capability
        Bool    ErrorLog;     // can keep error log

        Bool    UltVolume;    // can ultrasonic volume be software controled
        Bool    UltGain;      // can microphone sensitivity be software controled
        Bool    UltTimeout;   // can ultrasonic sampling frequency be changed
        Bool    PhotoDiode;   // SoniDiscs support photodiode

        DWORD   MaxStations;  // number of supported stations
        DWORD   MaxImus;      // number of supported IMUs
        DWORD   MaxFPses;     // maximum number of Fixed Position Sensing Elements (constellation)
        DWORD   MaxChannels;  // maximum number of analog channels supported per station
        DWORD   MaxButtons;   // maximum number of digital button inputs per station

        Bool    MeasData;     // can provide measurement data
        Bool    DiagData;     // can provide diag data
        Bool    PseConfig;    // supports PSE configuration/reporting tools
        Bool    ConfigLock;   // supports configuration locking

        float   UltMaxRange;  // maximum ultrasonic range
        float   fReserved2;
        float   fReserved3;
        float   fReserved4;

        Bool    CompassCal;   // supports dynamic compass calibration
        Bool    bReserved2;
        Bool    bReserved3;
        Bool    bReserved4;

        DWORD   dwReserved1;
        DWORD   dwReserved2;
        DWORD   dwReserved3;
        DWORD   dwReserved4;
    }
    Capability;

    Bool    bReserved1;
    Bool    bReserved2;
    Bool    bReserved3;
    Bool    bReserved4;

    DWORD   BaudRate;           // Serial port baud rate
    DWORD   NumTestLevels;      // Number of self test levels
    DWORD   dwReserved3;
    DWORD   dwReserved4;

    float   fReserved1;
    float   fReserved2;
    float   fReserved3;
    float   fReserved4;

    char    cReserved1[128];
    char    cReserved2[128];
    char    cReserved3[128];
    char    cReserved4[128];
}
ISD_HARDWARE_INFO_TYPE;


///////////////////////////////////////////////////////////////////////////////

// Station hardware information.
// This structure provides detailed information on station hardware and
// it's capabilities.

typedef struct
{
    Bool    Valid;             // set to TRUE if ISD_GetStationHardwareInfo succeeded

    DWORD   ID;                // unique number identifying a station. It is the same as that
                               // passed to the ISD_SetStationConfig and ISD_GetStationConfig
                               // functions and can be 1 to ISD_MAX_STATIONS

    char    DescVersion[20];   // Station Descriptor version

    float   FirmwareRev;       // Station firmware revision.
    DWORD   SerialNum;         // Serial number
    char    CalDate[20];       // Cal date (mm/dd/yyyy)
    DWORD   Port;              // Hardware port number

    struct
    {
        Bool    Position;      // TRUE if station can track position
        Bool    Orientation;   // TRUE if station can track orientation
        DWORD   Encoders;      // number lens encoders, is 0 then none are available
        DWORD   NumChannels;   // number of analog channels supported by this station, wand has 2 (joystick)
        DWORD   NumButtons;    // number of digital button inputs supported by this station
        DWORD   AuxInputs;     // number of auxiliary input channels (OEM products)
        DWORD   AuxOutputs;    // number of auxiliary output channels (OEM products)
        Bool    Compass;       // TRUE is station has compass

        Bool    bReserved1;
        Bool    bReserved2;
        Bool    bReserved3;
        Bool    bReserved4;

        DWORD   dwReserved1;
        DWORD   dwReserved2;
        DWORD   dwReserved3;
        DWORD   dwReserved4;
    }
    Capability;

    Bool    bReserved1;
    Bool    bReserved2;
    Bool    bReserved3;
    Bool    bReserved4;

    DWORD   Type;           // station type
    DWORD   DeviceID;
    DWORD   dwReserved3;
    DWORD   dwReserved4;

    float   fReserved1;
    float   fReserved2;
    float   fReserved3;
    float   fReserved4;

    char    cReserved1[128];
    char    cReserved2[128];
    char    cReserved3[128];
    char    cReserved4[128];
}
ISD_STATION_HARDWARE_INFO_TYPE;


// Returns -1 on failure. To detect tracker automatically specify 0 for commPort.
// hParent parameter to ISD_OpenTracker is optional and should only be used if
// information screen or tracker configuration tools are to be used when available
// in the future releases. If you would like a tracker initialization window to be
// displayed, specify TRUE value for the infoScreen parameter (not implemented in
// this release).
// ----------------------------------------------------------------------------
DLLEXPORT ISD_TRACKER_HANDLE DLLENTRY ISD_OpenTracker(
                                                      Hwnd hParent,
                                                      DWORD commPort,
                                                      Bool infoScreen,
                                                      Bool verbose
                                                      );

DLLEXPORT DWORD DLLENTRY ISD_OpenAllTrackers(
                                             Hwnd hParent,
                                             ISD_TRACKER_HANDLE *handle,
                                             Bool infoScreen,
                                             Bool verbose
                                             );

// This function call deinitializes the tracker, closes communications port and
// frees the resources associated with this tracker. If 0 is passed, all currently
// open trackers are closed. When last tracker is closed, program frees the DLL.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_CloseTracker( ISD_TRACKER_HANDLE handle );


// Get general tracker information, such as type, model, port, etc.
// Also retrieves genlock synchronization configuration, if available.
// See ISD_TRACKER_INFO_TYPE structure definition above for complete list of items
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetTrackerConfig(
                                             ISD_TRACKER_HANDLE handle,
                                             ISD_TRACKER_INFO_TYPE *Tracker,
                                             Bool verbose
                                             );


// When used with IS Precision Series (IS-300, IS-600, IS-900, IS-1200) tracking devices
// this function call will set genlock synchronization  parameters, all other fields
// in the ISD_TRACKER_INFO_TYPE structure are for information purposes only
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_SetTrackerConfig(
                                             ISD_TRACKER_HANDLE handle,
                                             ISD_TRACKER_INFO_TYPE *Tracker,
                                             Bool verbose
                                             );


// Get RecordsPerSec and KBitsPerSec without requesting genlock settings from the tracker.
// Use this instead of ISD_GetTrackerConfig to prevent your program from stalling while
// waiting for the tracker response.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetCommInfo(
                                        ISD_TRACKER_HANDLE handle,
                                        ISD_TRACKER_INFO_TYPE *Tracker
                                        );


// Configure station as specified in the ISD_STATION_INFO_TYPE structure. Before
// this function is called, all elements of the structure must be assigned a value.
// stationID is a number from 1 to ISD_MAX_STATIONS. Should only be used with
// IS Precision Series tracking devices, not valid for InterTrax.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_SetStationConfig(
                                             ISD_TRACKER_HANDLE handle,
                                             ISD_STATION_INFO_TYPE *Station,
                                             WORD stationID,
                                             Bool verbose
                                             );


// Fills the ISD_STATION_INFO_TYPE structure with current settings. Function
// requests configuration records from the tracker and waits for the response.
// If communications are interrupted, it will stall for several seconds while
// attempting to recover the settings. Should only be used with IS Precision Series
// tracking devices, not valid for InterTrax.
// stationID is a number from 1 to ISD_MAX_STATIONS
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetStationConfig(
                                             ISD_TRACKER_HANDLE handle,
                                             ISD_STATION_INFO_TYPE *Station,
                                             WORD stationID,
                                             Bool verbose
                                             );

// Not supported on UNIX in this release
// When a tracker is first opened, library automatically looks for a configuration
// file in current directory of the application. File name convention is
// isenseX.ini where X is a number, starting at 1, identifying one tracking
// system in the order of initialization. This function provides for a way to
// manually configure the tracker using a different configuration file.
// ----------------------------------------------------------------------------
DLLEXPORT Bool ISD_ConfigureFromFile(
                                     ISD_TRACKER_HANDLE handle,
                                     char *path,
                                     Bool verbose
                                     );


// Save tracker configuration. For devices with on-host processing,
// like PC-Tracker model, this will write to isenseX.cfg file.
// Serial port devices like IS-300, IS-600 and IS-900 save configuration
// in the base unit, and this call will just send command to commit the
// changes to permanent storage.

DLLEXPORT Bool ISD_ConfigSave( ISD_TRACKER_HANDLE handle );


// Get data from all configured stations. Data is places in the ISD_TRACKING_DATA_TYPE
// structure.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetTrackingData(
                                    ISD_TRACKER_HANDLE handle,
                                    ISD_TRACKING_DATA_TYPE *Data
                                    );


// Get camera encode and other data for all configured stations. Data is places
// in the ISD_CAMERA_DATA_TYPE structure. This function does not service serial
// port, so ISD_GetData must be called prior to this.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetCameraData(
                                          ISD_TRACKER_HANDLE handle,
                                          ISD_CAMERA_DATA_TYPE *Data
                                          );


// By default, ISD_GetData processes all the records available from the tracker
// and only returns the latest data. As the result, data samples can be lost.
// If all the data samples are required, you can use a ring buffer to store them.
// ISD_SetupRingBuffer accepts the pointer to the ring buffer, and it's size.
// Once activated, all processed data samples are stored in the buffer for use
// by the application.
//
// ISD_GetData can still be used to read the data, but it would return the
// oldest saved data sample, then remove it from the buffer (first in - first out).
// By repeatedly calling ISD_GetData all samples are retrieved, the latest
// coming last. All consecutive calls to ISD_GetData will return the last
// sample, but the NewData flag will be FALSE to indicate that buffer has
// been emptied.
// ----------------------------------------------------------------------------
DLLEXPORT Bool ISD_RingBufferSetup(
                                   ISD_TRACKER_HANDLE handle,
                                   WORD stationID,
                                   ISD_STATION_DATA_TYPE *dataBuffer,
                                   DWORD samples
                                   );

// Activate the ring buffer. While active, all data samples are stored in the
// buffer. Because this is a ring buffer, it will only store the number of samples
// specified in the call to ISD_SetupRingBuffer, so the oldest samples can be
// overwritten.
// ----------------------------------------------------------------------------
DLLEXPORT Bool ISD_RingBufferStart(
                                   ISD_TRACKER_HANDLE handle,
                                   WORD stationID
                                   );

// Stop collection. The data will continue to be processed, but the contents of
// the ring buffer will not be altered.
// ----------------------------------------------------------------------------
DLLEXPORT Bool ISD_RingBufferStop(
                                  ISD_TRACKER_HANDLE handle,
                                  WORD stationID
                                  );


// Queries the DLL for the latest data without removing it from the buffer or
// affecting the NewData flag. It also returns the indexes of the newest and the
// oldest samples in the buffer. User program can use these to parse the buffer.
// ----------------------------------------------------------------------------
DLLEXPORT Bool ISD_RingBufferQuery(
                                   ISD_TRACKER_HANDLE handle,
                                   WORD stationID,
                                   ISD_STATION_DATA_TYPE *currentData,
                                   DWORD *head,
                                   DWORD *tail
                                   );

// Reset heading to zero
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_ResetHeading(
                                         ISD_TRACKER_HANDLE handle,
                                         WORD stationID
                                         );


// Works with all IS-X00 series products and InertiaCube2, and InterTraxLC.
// For InterTrax30 and InterTrax2 behaves like ISD_ResetHeading.
// Boresight station using specific reference angles. This is useful when
// you need to apply a specific offset to system output. For example, if
// a sensor is mounted at 40 degrees relative to the HMD, you can
// enter 0, 40, 0 to get the system to output zero when HMD is horizontal.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_BoresightReferenced(
                                                ISD_TRACKER_HANDLE handle,
                                                WORD stationID,
                                                float yaw,
                                                float pitch,
                                                float roll
                                                );

// Works with all IS-X00 series products and InertiaCube2, and InterTraxLC.
// For InterTrax30 and InterTrax2 behaves like ISD_ResetHeading.
// Boresight, or unboresight a station. If 'set' is TRUE, all angles
// are reset to zero. Otherwise, all boresight settings are cleared,
// including those set by ISD_ResetHeading and ISD_BoresightReferenced
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_Boresight(
                                      ISD_TRACKER_HANDLE handle,
                                      WORD stationID,
                                      Bool set
                                      );



// Send a configuration script to the tracker. Script must consist of valid
// commands as described in the interface protocol. Commands in the script
// should be terminated by the New Line character '\n'. Line Feed character '\r'
// is added by the function and is not required.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_SendScript(
                                       ISD_TRACKER_HANDLE handle,
                                       char *script
                                       );


// Sends up to 4 output bytes to the auxiliary interface of the station
// specified. The number of bytes should match the number the auxiliary outputs
// interface is set up to expect. If too many are specified, the extra bytes
// are ignored.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_AuxOutput(
                                       ISD_TRACKER_HANDLE handle,
                                       WORD stationID,
                                       BYTE *AuxOutput,
                                       WORD length
                                       );

// Number of currently opened trackers is stored in the parameter passed to this
// functions
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_NumOpenTrackers( WORD *num );


// Platform independent time
// ----------------------------------------------------------------------------
DLLEXPORT float DLLENTRY ISD_GetTime( void );


// Broadcast tracker data over the network
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_UdpDataBroadcast(
                                             ISD_TRACKER_HANDLE handle,
                                             DWORD port,
                                             ISD_TRACKING_DATA_TYPE *trackingData,
                                             ISD_CAMERA_DATA_TYPE *cameraData
                                             );

// System hardware information.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetSystemHardwareInfo(
                                                  ISD_TRACKER_HANDLE handle,
                                                  ISD_HARDWARE_INFO_TYPE *hwInfo
                                                  );


// Station hardware information.
// ----------------------------------------------------------------------------
DLLEXPORT Bool DLLENTRY ISD_GetStationHardwareInfo(
                                                   ISD_TRACKER_HANDLE handle,
                                                   ISD_STATION_HARDWARE_INFO_TYPE *info,
                                                   WORD stationID
                                                   );


#ifdef __cplusplus
}
#endif

#endif

