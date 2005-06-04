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

#ifndef  DELTA_DTSOARX
#define  DELTA_DTSOARX

#include <soarx/soarxterrain.h>

#ifdef _WIN32

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

#if   defined(_DEBUG)

   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtsoarxd.lib"
   #endif
   
   #define _AUTOLIBNAME2 "gdal_iD.lib"
   
#else

   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtsoarx.lib"
   #endif
   
   #define _AUTOLIBNAME2 "gdal_i.lib"
   
#endif

#if   !  defined(_NOAUTOLIBMSG)
   #ifndef  DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#endif



#ifndef  DT_LIBRARY
   #pragma  comment( lib, _AUTOLIBNAME1 )
#endif

#pragma  comment( lib, _AUTOLIBNAME2 )

#endif // _WIN32

#endif   // DELTA_DTSOARX
