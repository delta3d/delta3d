#ifndef DT_HLA
#define DT_HLA

#include "rticonnection.h"

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4

#if defined(_DEBUG)

   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dthlad.lib"
   #endif
   
   #define _AUTOLIBNAME1  "libRTI-NGd.lib"
   #define _AUTOLIBNAME2  "tinyxmld.lib"
   #define _AUTOLIBNAME3  "osgSimd.lib"
   #define _AUTOLIBNAME4  "ws2_32.lib"
   
#else

   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dthla.lib"
   #endif
   
   #define _AUTOLIBNAME1  "libRTI-NG.lib"
   #define _AUTOLIBNAME2  "tinyxml.lib"
   #define _AUTOLIBNAME3  "osgSim.lib"
   #define _AUTOLIBNAME4  "ws2_32.lib"
   
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#pragma message( "Will automatically link with " _AUTOLIBNAME1 )
#pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#pragma message( "Will automatically link with " _AUTOLIBNAME4 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DT_HLA
