//==========================================================================================
//
//    File Name:      isense
//    Description:    DLL access point
//    Created:        12/7/98
//    Author:         Yury Altshuler
//
//    Copyright:      InterSense 2003 - All rights Reserved.
//
//
//==========================================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "isense.h"

//==========================================================================================
// Library file name

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#define ISD_LIB_NAME "isense"
#elif defined MACOSX
#include "dlcompat.h"
#define ISD_LIB_NAME "libisense"
#elif defined HP || defined HPUX
#include <dl.h>
#define ISD_LIB_NAME "libisense"
#else
#include <dlfcn.h>
#define ISD_LIB_NAME "libisense"
#endif


typedef void DLL;
static DLL *hLib = NULL;


static DLL_EP  dll_entrypoint( DLL *dll, const char *name );
static DLL    *dll_load( const char *name );
static void    dll_unload( DLL *dll );

static void ISD_FreeLib( void );


//==========================================================================================
// Function Prototypes

typedef ISD_TRACKER_HANDLE (*ISD_OPEN_FN)           ( Hwnd, DWORD, Bool, Bool );
typedef DWORD              (*ISD_OPEN_ALL_FN)       ( Hwnd, ISD_TRACKER_HANDLE *, Bool, Bool );
typedef Bool               (*ISD_COMMAND_FN)        ( ISD_TRACKER_HANDLE );
typedef Bool               (*ISD_COMM_INFO_FN)      ( ISD_TRACKER_HANDLE, ISD_TRACKER_INFO_TYPE * );
typedef Bool               (*ISD_SYSTEM_CONFIG_FN)  ( ISD_TRACKER_HANDLE, ISD_TRACKER_INFO_TYPE *, Bool );
typedef Bool               (*ISD_STATION_CONFIG_FN) ( ISD_TRACKER_HANDLE, ISD_STATION_INFO_TYPE *, WORD, Bool );
typedef Bool               (*ISD_DATA_FN)           ( ISD_TRACKER_HANDLE, ISD_TRACKING_DATA_TYPE * );
typedef Bool               (*ISD_CAMERA_DATA_FN)    ( ISD_TRACKER_HANDLE, ISD_CAMERA_DATA_TYPE * );
typedef Bool               (*ISD_SCRIPT_FN)         ( ISD_TRACKER_HANDLE, char * );
typedef Bool               (*ISD_COUNT_FN)          ( WORD * );
typedef Bool               (*ISD_RESET_HEADING_FN)  ( ISD_TRACKER_HANDLE, WORD );
typedef Bool               (*ISD_BORESIGHT_FN)      ( ISD_TRACKER_HANDLE, WORD, Bool );
typedef Bool               (*ISD_BORESIGHT_REF_FN)  ( ISD_TRACKER_HANDLE, WORD, float, float, float );
typedef float              (*ISD_GET_TIME)          ( void );
typedef Bool               (*ISD_CONFIG_FILE_FN)    ( ISD_TRACKER_HANDLE, char *, Bool );
typedef Bool               (*ISD_AUX_OUTPUT_FN)     ( ISD_TRACKER_HANDLE, WORD, BYTE *, WORD );
typedef Bool               (*ISD_UDP_BROADCAST_FN)  ( ISD_TRACKER_HANDLE, DWORD, ISD_TRACKING_DATA_TYPE *, ISD_CAMERA_DATA_TYPE * );
typedef Bool               (*ISD_SYS_INFO_FN)       ( ISD_TRACKER_HANDLE, ISD_HARDWARE_INFO_TYPE * );
typedef Bool               (*ISD_GET_HARDW_INFO_FN) ( ISD_TRACKER_HANDLE, ISD_STATION_HARDWARE_INFO_TYPE *, WORD );
typedef Bool               (*ISD_SET_RBUFFER_FN)    ( ISD_TRACKER_HANDLE, WORD, ISD_STATION_DATA_TYPE *, DWORD );
typedef Bool               (*ISD_RBUFFER_FN)        ( ISD_TRACKER_HANDLE, WORD );
typedef Bool               (*ISD_QRY_RBUFFER_FN)    ( ISD_TRACKER_HANDLE, WORD, ISD_STATION_DATA_TYPE *, DWORD *, DWORD * );


//==========================================================================================
// Function Pointers

ISD_OPEN_FN           _ISD_OpenTracker              = NULL;
ISD_OPEN_ALL_FN       _ISD_OpenAllTrackers          = NULL;
ISD_COMMAND_FN        _ISD_CloseTracker             = NULL;
ISD_COMM_INFO_FN      _ISD_GetCommInfo              = NULL;
ISD_SYSTEM_CONFIG_FN  _ISD_GetTrackerConfig         = NULL;
ISD_SYSTEM_CONFIG_FN  _ISD_SetTrackerConfig         = NULL;
ISD_STATION_CONFIG_FN _ISD_GetStationConfig         = NULL;
ISD_STATION_CONFIG_FN _ISD_SetStationConfig         = NULL;
ISD_DATA_FN           _ISD_GetTrackingData          = NULL;
ISD_CAMERA_DATA_FN    _ISD_GetCameraData            = NULL;
ISD_SCRIPT_FN         _ISD_SendScript               = NULL;
ISD_COUNT_FN          _ISD_NumOpenTrackers          = NULL;
ISD_RESET_HEADING_FN  _ISD_ResetHeading             = NULL;
ISD_BORESIGHT_FN      _ISD_Boresight                = NULL;
ISD_BORESIGHT_REF_FN  _ISD_BoresightReferenced      = NULL;
ISD_GET_TIME          _ISD_GetTime                  = NULL;
ISD_CONFIG_FILE_FN    _ISD_ConfigureFromFile        = NULL;
ISD_COMMAND_FN        _ISD_ConfigSave               = NULL;
ISD_AUX_OUTPUT_FN     _ISD_AuxOutput                = NULL;
ISD_UDP_BROADCAST_FN  _ISD_UdpBroadcastData         = NULL;
ISD_SYS_INFO_FN       _ISD_GetSystemHardwareInfo    = NULL;
ISD_GET_HARDW_INFO_FN _ISD_GetStationHardwareInfo   = NULL;
ISD_SET_RBUFFER_FN    _ISD_RingBufferSetup          = NULL;
ISD_RBUFFER_FN        _ISD_RingBufferStart          = NULL;
ISD_RBUFFER_FN        _ISD_RingBufferStop           = NULL;
ISD_QRY_RBUFFER_FN    _ISD_RingBufferQuery          = NULL;


//==========================================================================================
static DLL *ISD_LoadLib( void )
{
    if (hLib = dll_load( ISD_LIB_NAME ))
    {
        _ISD_OpenTracker            = ( ISD_OPEN_FN )           dll_entrypoint( hLib, "ISD_OpenTracker" );
        _ISD_OpenAllTrackers        = ( ISD_OPEN_ALL_FN )       dll_entrypoint( hLib, "ISD_OpenAllTrackers" );
        _ISD_CloseTracker           = ( ISD_COMMAND_FN )        dll_entrypoint( hLib, "ISD_CloseTracker" );
        _ISD_GetCommInfo            = ( ISD_COMM_INFO_FN )      dll_entrypoint( hLib, "ISD_GetCommInfo" );
        _ISD_GetTrackerConfig       = ( ISD_SYSTEM_CONFIG_FN )  dll_entrypoint( hLib, "ISD_GetTrackerConfig" );
        _ISD_SetTrackerConfig       = ( ISD_SYSTEM_CONFIG_FN )  dll_entrypoint( hLib, "ISD_SetTrackerConfig" );
        _ISD_GetStationConfig       = ( ISD_STATION_CONFIG_FN ) dll_entrypoint( hLib, "ISD_GetStationConfig" );
        _ISD_SetStationConfig       = ( ISD_STATION_CONFIG_FN ) dll_entrypoint( hLib, "ISD_SetStationConfig" );
        _ISD_GetTrackingData        = ( ISD_DATA_FN )           dll_entrypoint( hLib, "ISD_GetTrackingData" );
        _ISD_GetCameraData          = ( ISD_CAMERA_DATA_FN )    dll_entrypoint( hLib, "ISD_GetCameraData" );
        _ISD_SendScript             = ( ISD_SCRIPT_FN )         dll_entrypoint( hLib, "ISD_SendScript" );
        _ISD_NumOpenTrackers        = ( ISD_COUNT_FN )          dll_entrypoint( hLib, "ISD_NumOpenTrackers" );
        _ISD_ResetHeading           = ( ISD_RESET_HEADING_FN )  dll_entrypoint( hLib, "ISD_ResetHeading" );
        _ISD_Boresight              = ( ISD_BORESIGHT_FN )      dll_entrypoint( hLib, "ISD_Boresight" );
        _ISD_BoresightReferenced    = ( ISD_BORESIGHT_REF_FN )  dll_entrypoint( hLib, "ISD_BoresightReferenced" );
        _ISD_GetTime                = ( ISD_GET_TIME )          dll_entrypoint( hLib, "ISD_GetTime" );
        _ISD_ConfigureFromFile      = ( ISD_CONFIG_FILE_FN)     dll_entrypoint( hLib, "ISD_ConfigureFromFile" );
        _ISD_ConfigSave             = ( ISD_COMMAND_FN )        dll_entrypoint( hLib, "ISD_ConfigSave" );
        _ISD_AuxOutput              = ( ISD_AUX_OUTPUT_FN )     dll_entrypoint( hLib, "ISD_AuxOutput" );
        _ISD_UdpBroadcastData       = ( ISD_UDP_BROADCAST_FN )  dll_entrypoint( hLib, "ISD_UdpBroadcastData" );
        _ISD_GetSystemHardwareInfo  = ( ISD_SYS_INFO_FN )       dll_entrypoint( hLib, "ISD_GetSystemHardwareInfo" );
        _ISD_GetStationHardwareInfo = ( ISD_GET_HARDW_INFO_FN ) dll_entrypoint( hLib, "ISD_GetStationHardwareInfo" );
        _ISD_RingBufferSetup        = ( ISD_SET_RBUFFER_FN )    dll_entrypoint( hLib, "ISD_RingBufferSetup" );
        _ISD_RingBufferStart        = ( ISD_RBUFFER_FN )        dll_entrypoint( hLib, "ISD_RingBufferStart" );
        _ISD_RingBufferStop         = ( ISD_RBUFFER_FN )        dll_entrypoint( hLib, "ISD_RingBufferStop" );
        _ISD_RingBufferQuery        = ( ISD_QRY_RBUFFER_FN )    dll_entrypoint( hLib, "ISD_RingBufferQuery" );
    }

    if ( hLib == NULL )
    {
        printf("Could not load %s\n", ISD_LIB_NAME);
    }

    return hLib;
}

//==========================================================================================
static void ISD_FreeLib( void )
{
    _ISD_OpenTracker            = NULL;
    _ISD_OpenAllTrackers        = NULL;
    _ISD_CloseTracker           = NULL;
    _ISD_GetCommInfo            = NULL;
    _ISD_GetTrackerConfig       = NULL;
    _ISD_SetTrackerConfig       = NULL;
    _ISD_GetStationConfig       = NULL;
    _ISD_SetStationConfig       = NULL;
    _ISD_GetTrackingData        = NULL;
    _ISD_GetCameraData          = NULL;
    _ISD_SendScript             = NULL;
    _ISD_NumOpenTrackers        = NULL;
    _ISD_ResetHeading           = NULL;
    _ISD_Boresight              = NULL;
    _ISD_BoresightReferenced    = NULL;
    _ISD_GetTime                = NULL;
    _ISD_ConfigureFromFile      = NULL;
    _ISD_ConfigSave             = NULL;
    _ISD_AuxOutput              = NULL;
    _ISD_UdpBroadcastData       = NULL;
    _ISD_GetSystemHardwareInfo  = NULL;
    _ISD_GetStationHardwareInfo = NULL;
    _ISD_RingBufferSetup        = NULL;
    _ISD_RingBufferStart        = NULL;
    _ISD_RingBufferStop         = NULL;
    _ISD_RingBufferQuery        = NULL;

    if ( hLib )
    {
        dll_unload( hLib );  // free the dll
        hLib = NULL;
    }
}


//==========================================================================================
DLLEXPORT ISD_TRACKER_HANDLE DLLENTRY
ISD_OpenTracker(
                Hwnd hParent,
                DWORD commPort,
                Bool infoScreen,
                Bool verbose
                )
{
    if (!_ISD_OpenTracker) // this will be NULL if dll not loaded
    {
        if (!hLib) ISD_LoadLib();

        if (!hLib)  // failed to load dll
        {
            return 0;
        }
    }
    if (_ISD_OpenTracker)
    {
        return((*_ISD_OpenTracker)( hParent, commPort, infoScreen, verbose ));
    }
    return FALSE;
}


//==========================================================================================
DLLEXPORT DWORD DLLENTRY
ISD_OpenAllTrackers(
                    Hwnd hParent,
                    ISD_TRACKER_HANDLE *handle,
                    Bool infoScreen,
                    Bool verbose
                    )
{
    if ( !_ISD_OpenAllTrackers ) // this will be NULL if dll not loaded
    {
        if ( !hLib ) ISD_LoadLib();

        if ( !hLib )  // failed to load dll
        {
            return 0;
        }
    }
    if ( _ISD_OpenAllTrackers )
    {
        return (*_ISD_OpenAllTrackers)( hParent, handle, infoScreen, verbose );
    }
    return FALSE;
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_CloseTracker( ISD_TRACKER_HANDLE handle )
{
    Bool ret;
    WORD num;

    if ( _ISD_CloseTracker )
    {
        ret = (*_ISD_CloseTracker)( handle );

        // if all trackers are closed the dll can be freed
        if ( ISD_NumOpenTrackers( &num ) )
        {
            if ( num == 0 )
            {
                ISD_FreeLib();
            }
        }
        return ret;
    }
    return FALSE;
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_NumOpenTrackers( WORD *num )
{
    if ( !_ISD_NumOpenTrackers ) return FALSE;
    return (*_ISD_NumOpenTrackers)( num );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetCommInfo(
                ISD_TRACKER_HANDLE handle,
                ISD_TRACKER_INFO_TYPE *Tracker
                )
{
    if ( !_ISD_GetCommInfo ) return FALSE;
    return (*_ISD_GetCommInfo)( handle, Tracker );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetTrackerConfig(
                     ISD_TRACKER_HANDLE handle,
                     ISD_TRACKER_INFO_TYPE *Tracker,
                     Bool verbose
                     )
{
    if ( !_ISD_GetTrackerConfig ) return FALSE;
    return (*_ISD_GetTrackerConfig)( handle, Tracker, verbose );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_SetTrackerConfig(
                     ISD_TRACKER_HANDLE handle,
                     ISD_TRACKER_INFO_TYPE *Tracker,
                     Bool verbose
                     )
{
    if (!_ISD_SetTrackerConfig ) return FALSE;
    return (*_ISD_SetTrackerConfig)( handle, Tracker, verbose );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_SetStationConfig(
                     ISD_TRACKER_HANDLE handle,
                     ISD_STATION_INFO_TYPE *Station,
                     WORD stationNum,
                     Bool verbose
                     )
{
    if ( !_ISD_SetStationConfig ) return FALSE;
    return (*_ISD_SetStationConfig)( handle, Station, stationNum, verbose );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetStationConfig(
                     ISD_TRACKER_HANDLE handle,
                     ISD_STATION_INFO_TYPE *Station,
                     WORD stationNum,
                     Bool verbose
                     )
{
    if ( !_ISD_GetStationConfig ) return FALSE;
    return (*_ISD_GetStationConfig)( handle, Station, stationNum, verbose );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetTrackingData(
            ISD_TRACKER_HANDLE handle,
            ISD_TRACKING_DATA_TYPE *Data
            )
{
    if ( !_ISD_GetTrackingData ) return FALSE;
    return (*_ISD_GetTrackingData)( handle, Data );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetCameraData(
                  ISD_TRACKER_HANDLE handle,
                  ISD_CAMERA_DATA_TYPE *Data
                  )
{
    if ( !_ISD_GetCameraData ) return FALSE;
    return (*_ISD_GetCameraData)( handle, Data );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_SendScript(
               ISD_TRACKER_HANDLE handle,
               char *script
               )
{
    if ( !_ISD_SendScript ) return FALSE;
    return (*_ISD_SendScript)( handle, script );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_ResetHeading(
                 ISD_TRACKER_HANDLE handle,
                 WORD stationNum
                 )
{
    if ( !_ISD_ResetHeading ) return FALSE;
    return (*_ISD_ResetHeading)( handle, stationNum );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_Boresight(
              ISD_TRACKER_HANDLE handle,
              WORD stationNum,
              Bool set
              )
{
    if ( !_ISD_Boresight ) return FALSE;
    return (*_ISD_Boresight)( handle, stationNum, set );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_BoresightReferenced(
                        ISD_TRACKER_HANDLE handle,
                        WORD stationNum,
                        float yaw,
                        float pitch,
                        float roll
                        )
{
    if ( !_ISD_BoresightReferenced ) return FALSE;
    return (*_ISD_BoresightReferenced)( handle, stationNum, yaw, pitch, roll );
}


//==========================================================================================
DLLEXPORT float DLLENTRY
ISD_GetTime( void )
{
    if ( !_ISD_GetTime ) return 0.0f;
    return (*_ISD_GetTime)();
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_ConfigureFromFile(
                      ISD_TRACKER_HANDLE handle,
                      char *path,
                      Bool verbose
                      )
{
    if ( !_ISD_ConfigureFromFile ) return FALSE;
    return (*_ISD_ConfigureFromFile)( handle, path, verbose );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_ConfigSave( ISD_TRACKER_HANDLE handle )
{
    if ( !_ISD_ConfigSave ) return FALSE;
    return (*_ISD_ConfigSave)( handle );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_AuxOutput(
              ISD_TRACKER_HANDLE handle,
              WORD stationID,
              BYTE *AuxOutput,
              WORD length
              )
{
    if ( !_ISD_AuxOutput ) return FALSE;
    return (*_ISD_AuxOutput)( handle, stationID, AuxOutput, length );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_UdpBroadcastData(
                     ISD_TRACKER_HANDLE handle,
                     DWORD port,
                     ISD_TRACKING_DATA_TYPE *trackerData,
                     ISD_CAMERA_DATA_TYPE *cameraData
                     )
{
    if ( !_ISD_UdpBroadcastData ) return FALSE;
    return (*_ISD_UdpBroadcastData)( handle, port, trackerData, cameraData );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetSystemHardwareInfo(
                          ISD_TRACKER_HANDLE handle,
                          ISD_HARDWARE_INFO_TYPE *hwInfo
                          )
{
    if ( !_ISD_GetSystemHardwareInfo ) return FALSE;
    return (*_ISD_GetSystemHardwareInfo)( handle, hwInfo );
}


//==========================================================================================
DLLEXPORT Bool DLLENTRY
ISD_GetStationHardwareInfo( ISD_TRACKER_HANDLE handle,
                            ISD_STATION_HARDWARE_INFO_TYPE *info,
                            WORD stationNum )
{
    if ( !_ISD_GetStationHardwareInfo ) return FALSE;
    return (*_ISD_GetStationHardwareInfo)( handle, info, stationNum );
}


//==========================================================================================
DLLEXPORT Bool ISD_RingBufferSetup(
                                   ISD_TRACKER_HANDLE handle,
                                   WORD stationID,
                                   ISD_STATION_DATA_TYPE *dataBuffer,
                                   DWORD samples
                                   )
{
    if ( !_ISD_RingBufferSetup ) return FALSE;
    return (*_ISD_RingBufferSetup)( handle, stationID, dataBuffer, samples );
}


//==========================================================================================
DLLEXPORT Bool ISD_RingBufferStart(
                                   ISD_TRACKER_HANDLE handle,
                                   WORD stationID
                                   )
{
    if ( !_ISD_RingBufferStart ) return FALSE;
    return (*_ISD_RingBufferStart)( handle, stationID );
}


//==========================================================================================
DLLEXPORT Bool ISD_RingBufferStop(
                                  ISD_TRACKER_HANDLE handle,
                                  WORD stationID
                                  )
{
    if ( !_ISD_RingBufferStop ) return FALSE;
    return (*_ISD_RingBufferStop)( handle, stationID );
}


//==========================================================================================
DLLEXPORT Bool ISD_RingBufferQuery(
                                   ISD_TRACKER_HANDLE handle,
                                   WORD stationID,
                                   ISD_STATION_DATA_TYPE *currentData,
                                   DWORD *head,
                                   DWORD *tail
                                   )
{
    if ( !_ISD_RingBufferQuery ) return FALSE;
    return (*_ISD_RingBufferQuery)( handle, stationID, currentData, head, tail );
}

//==========================================================================================
static DLL_EP dll_entrypoint( DLL *dll, const char *name )
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

    FARPROC proc;

    proc = GetProcAddress( (HMODULE) dll, (LPCSTR) name );
    if ( proc == NULL )
    {
        printf( "Failed to load %s. Error code %d\n", name, GetLastError() );
    }
    return (DLL_EP) proc;

#else // UNIX

#if defined LINUX  || defined SUN || defined MACOSX
   void *handle = (void *) dll;
   DLL_EP ep;
   ep = (DLL_EP) dlsym(handle, name);
   return ( dlerror() == 0 ) ? ep : (DLL_EP) NULL;

#elif defined HP || defined HPUX
   shl_t handle = (shl_t) dll;
   DLL_EP ep;
   return shl_findsym(&handle, name, TYPE_PROCEDURE, &ep) == -1 ?
      (DLL_EP) NULL : ep;
#else

   void *handle = (void *) dll;
   DLL_EP ep;
   ep = (DLL_EP) dlsym(handle, name);
   return ( dlerror() == 0 ) ? ep : (DLL_EP) NULL;
#endif
#endif
}


//==========================================================================================
static DLL *dll_load( const char *name )
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

    return (DLL *) LoadLibrary( name );

#else // UNIX
    char dllname[512];
    strcpy(dllname, name);

#if defined MACOSX
    strcat(dllname, ".dylib");
    return (DLL *) dlopen(dllname, RTLD_NOW);

#elif defined LINUX || defined SUN
    strcat( dllname, ".so" );
    return (DLL *) dlopen(dllname, RTLD_NOW);

#elif defined HP || defined HPUX
    strcat(dllname, ".sl");
    return shl_load(dllname, BIND_DEFERRED|DYNAMIC_PATH, 0L);
#else
    strcat( dllname, ".so" );
    return (DLL *) dlopen(dllname, RTLD_NOW);
#endif
#endif
}


//==========================================================================================
static void dll_unload( DLL *dll )
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

    HINSTANCE hInst = (HINSTANCE) dll;
    FreeLibrary( hInst );

#else // UNIX

#if defined(LINUX) || defined(MACOSX)
    void *handle = (void *) dll;
    dlclose( handle );
#elif defined(HP) || defined(HPUX)
    shl_t handle = (shl_t) dll;
    shl_unload( handle );
#else
    void *handle = (void *) dll;
    dlclose( handle );
#endif
#endif
}

