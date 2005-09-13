#ifndef DTNET_INCLUDE
#define DTNET_INCLUDE

#include "dtNet/netmgr.h"


#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1

#if defined(_DEBUG)
   #define _AUTOLIBNAME1   "dtNetD.lib"
#else 
   #define _AUTOLIBNAME1   "dtNet.lib"  
#endif

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
#endif

#pragma  comment( lib, _AUTOLIBNAME1 )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif    //DTNET_INCLUDE