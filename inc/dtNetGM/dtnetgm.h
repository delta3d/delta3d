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

#ifndef DTNETGM_INCLUDE
#define DTNETGM_INCLUDE

#include <dtNetGM/machineinfomessage.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/clientnetworkcomponent.h>
#include <dtNetGM/servernetworkcomponent.h>

/** The dtNetGM namespace contains networking classes.  dtNetGM uses
 *  the <A HREF="http://www.gillius.org/gne/">Game Network Engine</A> and
 *  <A HREF="http://www.hawksoft.com/hawknl/">HawkNL</A> for backbone functionality.
 */
namespace dtNetGM
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIBMSG */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4

#if defined(_DEBUG)
   #ifndef DT_NETGM_LIBRARY
      #define _AUTOLIBNAME1 "dtNetGMd.lib"
   #endif
   #define _AUTOLIBNAME2 "NLstaticD.lib"
   #define _AUTOLIBNAME3 "gned.lib"
   #define _AUTOLIBNAME4 "Ws2_32.lib"
#else
   #ifndef DT_NETGM_LIBRARY
      #define _AUTOLIBNAME1 "dtNetGM.lib"
   #endif
   #define _AUTOLIBNAME2 "NLstatic.lib"
   #define _AUTOLIBNAME3 "gne.lib"
   #define _AUTOLIBNAME4 "Ws2_32.lib"
#endif

#ifndef _NOAUTOLIBMSG
   #ifndef DT_NETGM_LIBRARY
      #pragma message("Will automatically link with " _AUTOLIBNAME1)
   #endif
   #pragma message("Will automatically link with " _AUTOLIBNAME2)
   #pragma message("Will automatically link with " _AUTOLIBNAME3)
   #pragma message("Will automatically link with " _AUTOLIBNAME4)
#endif

#ifndef DT_NETGM_LIBRARY
   #pragma  comment( lib, _AUTOLIBNAME1 )
#endif

#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif    DTNETGM_INCLUDE
