#ifndef  DELTA_DTAUDIO
#define  DELTA_DTAUDIO

#include <listener.h>
#include <audiomanager.h>



/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3



#if   defined(_DEBUG)
   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtaudioD.lib"
   #endif
#else
   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtaudio.lib"
   #endif
#endif



#define  _AUTOLIBNAME2  "OpenAL32.lib" // Note: Not Debug
#define  _AUTOLIBNAME3  "ALut.lib"     // Note: Not Debug



#if   !  defined(_NOAUTOLIBMSG)
   #ifndef  DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif



#ifndef  DT_LIBRARY
   #pragma  comment( lib, _AUTOLIBNAME1 )
#endif

#pragma  comment( lib, _AUTOLIBNAME2 )
#pragma  comment( lib, _AUTOLIBNAME3 )



#endif   // DELTA_DTAUDIO
