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

#ifndef DELTA_DTSCRIPT
#define DELTA_DTSCRIPT

#if defined (_MSC_VER) || defined(__BORLANDC__)
   #define __STR2__(x) #x
   #define __STR1__(x) __STR2__(x)
   #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
   #pragma message (__LOC__"This header is deprecated. Please #include the individual header files.")
#elif defined(__GNUC__) || defined(__HP_aCC) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
   #warning "This header is deprecated. Please #include the individual header files."
#endif

#include <dtScript/scriptmanager.h>

/** Contains the functionality to call Python scripts from a C++ application.
*/
namespace dtScript
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

#if defined(_DEBUG)
#define _AUTOLIBNAME1   "dtScriptd.lib"
#define _AUTOLIBNAME2   "python24.lib"
#else 
#define _AUTOLIBNAME1   "dtScript.lib"
#define _AUTOLIBNAME2   "python24.lib"
#endif

#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME1 )
#pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#endif

#pragma  comment( lib, _AUTOLIBNAME1 )
#pragma  comment( lib, _AUTOLIBNAME2 )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_DTSCRIPT
