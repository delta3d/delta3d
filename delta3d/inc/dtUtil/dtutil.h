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

#ifndef DTUTIL_INCLUDE
#define DTUTIL_INCLUDE

#if defined (WIN32) || defined(__BORLANDC__)
   #define __STR2__(x) #x
   #define __STR1__(x) __STR2__(x)
   #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
   #pragma message (__LOC__"This header is deprecated. Please #include the individual header files.")
#elif defined(__GNUC__) || defined(__HP_aCC) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
   #warning "This header is deprecated. Please #include the individual header files."
#endif

#include "dtUtil/deprecationmgr.h"
#include "dtUtil/enumeration.h"
#include "dtUtil/fractal.h"
#include "dtUtil/fileutils.h"
#include "dtUtil/keyframedecoder.h"
#include "dtUtil/log.h"
#include "dtUtil/mathdefines.h"
#include "dtUtil/matrixutil.h"
#include "dtUtil/noise1.h"
#include "dtUtil/noise2.h"
#include "dtUtil/noise3.h"
#include "dtUtil/noisetexture.h"
#include "dtUtil/noiseutility.h"
#include "dtUtil/objectfactory.h"
#include "dtUtil/polardecomp.h"
#include "dtUtil/seamlessnoise.h"
#include "dtUtil/serializer.h"
#include "dtUtil/stringutils.h"
#include "dtUtil/xerceserrorhandler.h"
#include "dtUtil/xerceswriter.h"
#include "dtUtil/xercesparser.h"
#include "dtUtil/xercesutils.h"

/** Contains generic, reusable features which are useful for most applications.
*/
namespace dtUtil
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4

#if defined(_DEBUG)
   #ifndef DT_UTIL_LIBRARY
      #define _AUTOLIBNAME  "dtUtild.lib"
   #endif
   #define _AUTOLIBNAME2 "ul_d.lib"
   #define _AUTOLIBNAME3 "osgd.lib"
   #define _AUTOLIBNAME4 "xerces-c_2D.lib"
#else
   #ifndef DT_UTIL_LIBRARY
      #define _AUTOLIBNAME  "dtUtil.lib"
   #endif
   #define _AUTOLIBNAME2 "ul.lib"
   #define _AUTOLIBNAME3 "osg.lib"
   #define _AUTOLIBNAME4 "xerces-c_2.lib"
#endif


#ifndef _NOAUTOLIBMSG
   #ifndef DT_UTIL_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
#endif

#ifndef DT_UTIL_LIBRARY
   #pragma comment(lib, _AUTOLIBNAME)
#endif
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)

#endif //win32
#endif //DTUTIL_INCLUDE
