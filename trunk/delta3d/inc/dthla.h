#ifndef DT_HLA
#define DT_HLA

#include "rticonnection.h"

#ifdef _WIN32

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#if defined(_DEBUG)
   #define _AUTOLIBNAME  "libRTI-NGd.lib"
#else
   #define _AUTOLIBNAME  "libRTI-NG.lib"
#endif

#define _AUTOLIBNAME1  "tinyxml.lib"
#define _AUTOLIBNAME3  "osgSim.lib"
#define _AUTOLIBNAME2  "ws2_32.lib"

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#pragma message( "Will automatically link with " _AUTOLIBNAME1 )
#pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)

#endif // _WIN32

#endif // DT_HLA
