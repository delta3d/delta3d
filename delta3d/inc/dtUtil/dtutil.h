#ifndef DTUTIL_INCLUDE
#define DTUTIL_INCLUDE

#include "dtUtil/domain.h"


#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

#if defined(_DEBUG)
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtutild.lib"
   #endif
   #define _AUTOLIBNAME1 "sg_d.lib"
   #define _AUTOLIBNAME2 "ul_d.lib"
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtutil.lib"
   #endif
   #define _AUTOLIBNAME1 "sg.lib"
   #define _AUTOLIBNAME2 "ul.lib"
#endif


#ifndef _NOAUTOLIBMSG
   #ifndef DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#endif

#ifndef DT_LIBRARY
   #pragma comment(lib, _AUTOLIBNAME)
#endif
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)

#endif //win32
#endif //DTUTIL_INCLUDE