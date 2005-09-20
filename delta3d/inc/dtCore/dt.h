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

#ifndef DELTA_DT
#define DELTA_DT

#include <dtCore/camera.h>
#include <dtCore/clouddome.h>
#include <dtCore/cloudplane.h>
#include <dtCore/compass.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/deltawin.h>
#include <dtCore/environment.h>
#include <dtCore/enveffect.h>
#include <dtCore/effectmanager.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/globals.h>
#include <dtCore/infinitelight.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/inputdevice.h>
#include <dtCore/inputmapper.h>
#include <dtCore/isector.h>
#include <dtCore/joystick.h>
#include <dtCore/keyboard.h>
#include <dtCore/light.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/motionmodel.h>
#include <dtCore/mouse.h>
#include <dtCore/notify.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/particlesystem.h>
#include <dtCore/positionallight.h>
#include <dtCore/physical.h>
#include <dtCore/pointaxis.h>
#include <dtCore/recorder.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/skybox.h>
#include <dtCore/skydome.h>
#include <dtCore/spotlight.h>
#include <dtCore/stats.h>
#include <dtCore/terrain.h>
#include <dtCore/tracker.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/tripod.h>
#include <dtCore/system.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/uniqueid.h>
#include <dtCore/walkmotionmodel.h>

/** The dtCore namespace contains basic, low-level functionality which is mostly
* required for all 3D applications.
*/
namespace dtCore
{
}


#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4
#undef _AUTOLIBNAME5
#undef _AUTOLIBNAME6
#undef _AUTOLIBNAME7
#undef _AUTOLIBNAME8
#undef _AUTOLIBNAME10
#undef _AUTOLIBNAME11
#undef _AUTOLIBNAME12
#undef _AUTOLIBNAME13
#undef _AUTOLIBNAME14
#undef _AUTOLIBNAME15
#undef _AUTOLIBNAME18
#undef _AUTOLIBNAME19
#undef _AUTOLIBNAME20
#undef _AUTOLIBNAME21

#if defined(_DEBUG)
   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME5 "dtCored.lib"
   #endif

   #define _AUTOLIBNAME   "Producerd.lib"
   #define _AUTOLIBNAME2  "ul_d.lib"
   #define _AUTOLIBNAME4  "OpenThreadsWin32d.lib"  
   #define _AUTOLIBNAME6  "osgd.lib"
   #define _AUTOLIBNAME7  "osgDBd.lib"
   #define _AUTOLIBNAME8  "osgParticled.lib"
   #define _AUTOLIBNAME10 "osgUtild.lib"
   #define _AUTOLIBNAME11 "oded.lib"
   #define _AUTOLIBNAME13 "osgTextd.lib"
   #define _AUTOLIBNAME15 "osgTerraind.lib"
   #define _AUTOLIBNAME18 "js_d.lib"
   #define _AUTOLIBNAME21 "isensed.lib"
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME5 "dtCore.lib"
   #endif

   #define _AUTOLIBNAME   "Producer.lib"
   #define _AUTOLIBNAME2  "ul.lib"
   #define _AUTOLIBNAME4  "OpenThreadsWin32.lib"  
   #define _AUTOLIBNAME6  "osg.lib"
   #define _AUTOLIBNAME7  "osgDB.lib"
   #define _AUTOLIBNAME8  "osgParticle.lib"
   #define _AUTOLIBNAME10 "osgUtil.lib"
   #define _AUTOLIBNAME11 "ode.lib"
   #define _AUTOLIBNAME13 "osgText.lib"
   #define _AUTOLIBNAME15 "osgTerrain.lib"
   #define _AUTOLIBNAME18 "js.lib"
   #define _AUTOLIBNAME21 "isense.lib"
#endif

#define _AUTOLIBNAME3  "winmm.lib"

#define _AUTOLIBNAME12 "opcode.lib"
#define _AUTOLIBNAME14 "opengl32.lib"
#define _AUTOLIBNAME19 "openal32.lib"
#define _AUTOLIBNAME20 "alut.lib"


/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME6 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME7 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME8 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME10 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME11 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME12 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME13 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME14 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME15 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME18 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME19 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME20 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME21 )

   #ifndef DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME5 )
   #endif
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)
#pragma comment(lib, _AUTOLIBNAME6)
#pragma comment(lib, _AUTOLIBNAME7)
#pragma comment(lib, _AUTOLIBNAME8)
#pragma comment(lib, _AUTOLIBNAME10)
#pragma comment(lib, _AUTOLIBNAME11)
#pragma comment(lib, _AUTOLIBNAME12)
#pragma comment(lib, _AUTOLIBNAME13)
#pragma comment(lib, _AUTOLIBNAME14)
#pragma comment(lib, _AUTOLIBNAME15)
#pragma comment(lib, _AUTOLIBNAME18)
#pragma comment(lib, _AUTOLIBNAME19)
#pragma comment(lib, _AUTOLIBNAME20)
#pragma comment(lib, _AUTOLIBNAME21)

#ifndef DT_LIBRARY
   #pragma comment(lib, _AUTOLIBNAME5)
#endif

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_DT
