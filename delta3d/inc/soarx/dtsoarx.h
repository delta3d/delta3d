#ifndef  DELTA_DTSOARX
#define  DELTA_DTSOARX

#include <soarx/soarxterrain.h>

#ifdef _WIN32

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3



#if   defined(_DEBUG)

   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtsoarxd.lib"
   #endif
   
   #define _AUTOLIBNAME2 "osgGL2d.lib"
   #define _AUTOLIBNAME3 "gdal.lib"
   
#else

   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtsoarx.lib"
   #endif
   
   #define _AUTOLIBNAME2 "osgGL2.lib"
   #define _AUTOLIBNAME3 "gdal.lib"
   
#endif

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

#endif // _WIN32

#endif   // DELTA_DTSOARX
