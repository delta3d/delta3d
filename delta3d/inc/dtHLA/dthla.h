/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#ifndef DT_HLA
#define DT_HLA

#include <dtHLA/rticonnection.h>

/** Contains the functionality to connect to HLA networks.
*
*/
namespace dtHLA
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4

#if defined(_DEBUG)

   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dtHLAd.lib"
   #endif
   
   #define _AUTOLIBNAME3  "osgSimd.lib"
   #define _AUTOLIBNAME4  "ws2_32.lib"
   
#else

   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dtHLA.lib"
   #endif
   
   #define _AUTOLIBNAME3  "osgSim.lib"
   #define _AUTOLIBNAME4  "ws2_32.lib"
   
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#pragma message( "Will automatically link with " _AUTOLIBNAME4 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DT_HLA
