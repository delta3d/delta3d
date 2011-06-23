/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005 MOVES Institute 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
*/

#ifndef DTNET_INCLUDE
#define DTNET_INCLUDE

#if defined (WIN32) || defined(__BORLANDC__)
   #define __STR2__(x) #x
   #define __STR1__(x) __STR2__(x)
   #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
   #pragma message (__LOC__"This header is deprecated. Please #include the individual header files.")
#elif defined(__GNUC__) || defined(__HP_aCC) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
   #warning "This header is deprecated. Please #include the individual header files."
#endif

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

   #ifndef DT_NET_LIBRARY  
      #define _AUTOLIBNAME1 "dtNetD.lib"
   #endif

   #define _AUTOLIBNAME2 "NLstaticD.lib"
   #define _AUTOLIBNAME3 "gned.lib"
#else 

   #ifndef DT_NET_LIBRARY  
      #define _AUTOLIBNAME1 "dtNet.lib"
   #endif

   #define _AUTOLIBNAME2 "NLstatic.lib"
   #define _AUTOLIBNAME3 "gne.lib"
#endif

#ifndef _NOAUTOLIBMSG

   #ifndef DT_NET_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif

#ifndef DT_NET_LIBRARY
   #pragma  comment( lib, _AUTOLIBNAME1 )
#endif

#pragma  comment( lib, _AUTOLIBNAME2 )
#pragma  comment( lib, _AUTOLIBNAME3 )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif    //DTNET_INCLUDE
