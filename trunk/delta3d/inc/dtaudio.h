#ifndef  DELTA_DTAUDIO
#define  DELTA_DTAUDIO

#include <listener.h>
#include <audiomanager.h>



/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3



#if   defined(_DEBUG)
   #define  _AUTOLIBNAME1  "dtabcd.lib"
   #define  _AUTOLIBNAME2  "OpenAL32.lib" // Note: Not Debug
   #define  _AUTOLIBNAME3  "ALut.lib"     // Note: Not Debug
#else 
   #define  _AUTOLIBNAME1  "dtabc.lib"
   #define  _AUTOLIBNAME2  "OpenAL32.lib"
   #define  _AUTOLIBNAME3  "ALut.lib"
#endif



#if   !  defined(_NOAUTOLIBMSG)
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif



#pragma  comment( lib, _AUTOLIBNAME1 )
#pragma  comment( lib, _AUTOLIBNAME2 )
#pragma  comment( lib, _AUTOLIBNAME3 )



#endif   // DELTA_DTAUDIO
