#ifndef DELTA_DTCHAR
#define DELTA_DTCHAR


#include "character.h"

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1

#if defined(_DEBUG)
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtChard.lib"
   #endif
   #define _AUTOLIBNAME1 "ReplicantBodyd.lib"
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtChar.lib"
   #endif
   #define _AUTOLIBNAME1 "ReplicantBody.lib"
#endif


#ifndef _NOAUTOLIBMSG

   #ifndef DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )

#endif

#ifndef DT_LIBRARY
   #pragma comment (lib, _AUTOLIBNAME)
#endif

#pragma comment (lib, _AUTOLIBNAME1)


#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_DTCHAR
