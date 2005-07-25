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

#ifndef DTGUI_H
#define DTGUI_H

#include <dtGUI/ceuidrawable.h>


#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME

#if defined(_DEBUG)
   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME "dtguid.lib"
   #endif
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME "dtgui.lib"
   #endif
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment(lib, _AUTOLIBNAME)

#endif //win32

#endif //DTGUI_H
