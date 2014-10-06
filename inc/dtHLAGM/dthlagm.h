/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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

#ifndef DT_HLAGM
#define DT_HLAGM

/** 
 * @namespace dtHLAGM
 *
 * This namespace contains the functionality to provide network communications
 * from an HLA network to the GameManager.
 */
namespace dtHLAGM
{
}

#ifdef _MSC_VER

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4

#if defined(_DEBUG)

   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dthlagmd.lib"
   #endif
   
   #define _AUTOLIBNAME2  "osgSimd.lib"
   #define _AUTOLIBNAME3  "ws2_32.lib"
   
#else

   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dthlagm.lib"
   #endif
   
   #define _AUTOLIBNAME2  "osgSim.lib"
   #define _AUTOLIBNAME3  "ws2_32.lib"
   
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
#pragma message( "Will automatically link with " _AUTOLIBNAME )
#pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DT_HLAGM
