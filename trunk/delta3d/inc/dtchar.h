#ifndef DT_CHAR
#define DT_CHAR 

#include "character.h"


#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1

#if defined(_DEBUG)
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtChard.lib"
   #endif
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtChar.lib"
   #endif
#endif

#define _AUTOLIBNAME1 "ReplicantBody.lib"

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

#endif // DT_CHAR