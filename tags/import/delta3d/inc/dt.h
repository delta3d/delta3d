#if !defined(DELTA_INCLUDE)
#define  DELTA_INCLUDE

#include "system.h"
#include "window.h"
#include "camera.h"
#include "Environment.h"

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4
#undef _AUTOLIBNAME5
#if defined(_DEBUG)
   #define _AUTOLIBNAME  "Producerd.lib"
   #define _AUTOLIBNAME4 "OpenThreadsWin32d.lib"  
   #define  _AUTOLIBNAME5 "dtcoreD.lib"
#else
   #define _AUTOLIBNAME  "Producer.lib"
   #define _AUTOLIBNAME4 "OpenThreadsWin32.lib"  
   #define _AUTOLIBNAME5 "dtcore.lib"
#endif
#define _AUTOLIBNAME1  "sg.lib"
#define _AUTOLIBNAME2  "ul.lib"
#define _AUTOLIBNAME3  "winmm.lib"


/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME5 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)
#pragma comment(lib, _AUTOLIBNAME5)

#endif //DELTA_INCLUDE