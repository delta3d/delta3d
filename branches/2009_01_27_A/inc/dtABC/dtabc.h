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

#ifndef DELTA_DTABC
#define DELTA_DTABC

#include <dtABC/action.h>
#include <dtABC/application.h>
#include <dtABC/autotrigger.h>
#include <dtABC/baseabc.h>
#include <dtABC/beziercontroller.h>
#include <dtABC/beziercontrolpoint.h>
#include <dtABC/curvenode.h>
#include <dtABC/event.h>
#include <dtABC/export.h>
#include <dtABC/fl_dt_window.h>
#include <dtABC/motionaction.h>
#include <dtABC/pathpoint.h>
#include <dtABC/pathpointconverter.h>
#include <dtABC/state.h>
#include <dtABC/statemanager.h>
#include <dtABC/trigger.h>
#include <dtABC/weather.h>
#include <dtABC/widget.h>

/** The Application Base Classes library contains high-level classes which can
* be assist greatly in the creation of applications.
*/
namespace dtABC
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME4
#undef _AUTOLIBNAME5
#undef _AUTOLIBNAME6
#undef _AUTOLIBNAME7
#undef _AUTOLIBNAME8

#if defined(_DEBUG)
   #define _AUTOLIBNAME1   "dtABCd.lib"
   #define _AUTOLIBNAME4   "fltkd.lib"
   #define _AUTOLIBNAME8   "xerces-c_2D.lib"
#else 
   #define _AUTOLIBNAME1   "dtABC.lib"  
   #define _AUTOLIBNAME4   "fltk.lib"
   #define _AUTOLIBNAME8   "xerces-c_2.lib"
#endif

#define _AUTOLIBNAME5 "wsock32.lib"
#define _AUTOLIBNAME6 "comctl32.lib"
#define _AUTOLIBNAME7 "winmm.lib"

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME5 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME6 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME7 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME8 )
#endif

#pragma  comment( lib, _AUTOLIBNAME1 )
#pragma  comment( lib, _AUTOLIBNAME4 )
#pragma  comment( lib, _AUTOLIBNAME5 )
#pragma  comment( lib, _AUTOLIBNAME6 )
#pragma  comment( lib, _AUTOLIBNAME7 )
#pragma  comment( lib, _AUTOLIBNAME8 )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_DTABC
