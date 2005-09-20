#ifndef DTNET_INCLUDE
#define DTNET_INCLUDE

#include <dtNet/netmgr.h>
#include <dtNet/connectionserver.h>
#include <dtNet/connectionlistener.h>

/** The dtNet namespace contains networking classes.  dtNet is uses
*  the <A HREF="http://www.gillius.org/gne/">Game Network Engine</A> and 
*  <A HREF="http://www.hawksoft.com/hawknl/">HawkNL</A> for backbone functionality.
*/
namespace dtNet
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIBMSG */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3

#if defined(_DEBUG)

   #ifndef DT_LIBRARY  
      #define _AUTOLIBNAME1 "dtNetD.lib"
   #endif

   #define _AUTOLIBNAME2 "NLstaticD.lib"
   #define _AUTOLIBNAME3 "gned.lib"
#else 

   #ifndef DT_LIBRARY  
      #define _AUTOLIBNAME1 "dtNet.lib"
   #endif

   #define _AUTOLIBNAME2 "NLstatic.lib"
   #define _AUTOLIBNAME3 "gne.lib"
#endif

#ifndef _NOAUTOLIBMSG

   #ifndef DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif

#ifndef DT_LIBRARY
   #pragma  comment( lib, _AUTOLIBNAME1 )
#endif

#pragma  comment( lib, _AUTOLIBNAME2 )
#pragma  comment( lib, _AUTOLIBNAME3 )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif    //DTNET_INCLUDE
