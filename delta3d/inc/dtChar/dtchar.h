/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

//TODO: Add Cal3D

#ifndef DELTA_DTCHAR
#define DELTA_DTCHAR

#undef Status
#include "dtChar/character.h"

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1

#if defined(_DEBUG)
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtchard.lib"
   #endif
   #define _AUTOLIBNAME1 "ReplicantBodyd.lib"
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME  "dtchar.lib"
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
