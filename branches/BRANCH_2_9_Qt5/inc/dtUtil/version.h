/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006 MOVES Institute 
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
 * Robert Osfield (via <osg/Version>)
 * Chris Osborn
 */

#ifndef __DELTA_VERSION__
#define __DELTA_VERSION__

#include <dtUtil/export.h>

extern "C"
{

#define DELTA3D_VERSION_MAJOR 2
#define DELTA3D_VERSION_MINOR 8
#define DELTA3D_VERSION_PATCH 100

/**
  * delta3DGetVersion() returns the library version number.
 *  Numbering convention : Delta3D 2.8.0 will return "2.8.0" from delta3DGetVersion.
  *
  * This C function can be also used to check for the existence of the Delta3D
  * library using autoconf and its m4 macro AC_CHECK_LIB.
  *
  * Here is the code to add to your configure.in:
 \verbatim
 #
 # Check for the Delta3D library
 #
 AC_CHECK_LIB(dtUtil, delta3DGetVersion, ,
    [AC_MSG_ERROR(Delta3D library not found. See http://www.delta3d.org)],)
 \endverbatim
*/
   extern DT_UTIL_EXPORT const char* Delta3DGetVersion();

   /** The osgGetLibraryName() method returns the library name in human-friendly form. */
   extern DT_UTIL_EXPORT const char* Delta3DGetLibraryName();
}

#endif // __DELTA_VERSION__
