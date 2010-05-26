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
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/genericmouselistener.h>
#include <dtCore/infinitelight.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/inputdevice.h>
#include <dtCore/inputmapper.h>
#include <dtCore/isector.h>
#include <dtCore/keyboard.h>
#include <dtCore/light.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/motionmodel.h>
#include <dtCore/mouse.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/particlesystem.h>
#include <dtCore/positionallight.h>
#include <dtCore/physical.h>
#include <dtCore/pointaxis.h>
#include <dtCore/recorder.h>
#include <dtCore/refptr.h>
#include <dtCore/rtsmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/skybox.h>
#include <dtCore/skydome.h>
#include <dtCore/spotlight.h>
#include <dtCore/stats.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/tripod.h>
#include <dtCore/system.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/uniqueid.h>
#include <dtCore/walkmotionmodel.h>
#include <dtCore/collisionmotionmodel.h>

/** 
 * @namespace dtCore
 *
 * The dtCore namespace contains basic, low-level functionality which is mostly
 * required for all 3D applications.
 */
namespace dtCore
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

// Automatic library inclusion macros that use the #pragma/lib feature
#undef DT_H_OPENTHREADS_LIB
#undef DT_H_DTCORE_LIB
#undef DT_H_OSG_LIB
#undef DT_H_OSGDB_LIB
#undef DT_H_OSGPARTICLE_LIB
#undef DT_H_OSGUTIL_LIB
#undef DT_H_ODE_LIB
#undef DT_H_OSGTEXT_LIB
#undef DT_H_OPENGL_LIB

// Setup debug vs. release library names
#if defined(_DEBUG)
   #ifndef DT_CORE_LIBRARY  
      #define  DT_H_DTCORE_LIB "dtCored.lib"
   #endif
   #define DT_H_OPENTHREADS_LIB  "OpenThreadsd.lib"  
   #define DT_H_OSG_LIB  "osgd.lib"
   #define DT_H_OSGDB_LIB  "osgDBd.lib"
   #define DT_H_OSGPARTICLE_LIB  "osgParticled.lib"
   #define DT_H_OSGUTIL_LIB "osgUtild.lib"

// dTriIndex is a macro hack in delta for ODE < 0.10. For 0.10 it's a typedef, so this will only
// exist for 0.10 and later.
#ifndef dTriIndex
   #define DT_H_ODE_LIB "ode_singled.lib"
#else
   #define DT_H_ODE_LIB "oded.lib"
#endif

   #define DT_H_OSGTEXT_LIB "osgTextd.lib"

#else
   #ifndef DT_CORE_LIBRARY
      #define DT_H_DTCORE_LIB "dtCore.lib"
   #endif
   #define DT_H_OPENTHREADS_LIB  "OpenThreads.lib"  
   #define DT_H_OSG_LIB  "osg.lib"
   #define DT_H_OSGDB_LIB  "osgDB.lib"
   #define DT_H_OSGPARTICLE_LIB  "osgParticle.lib"
   #define DT_H_OSGUTIL_LIB "osgUtil.lib"

#ifndef dTriIndex
   #define DT_H_ODE_LIB "ode_single.lib"
#else
   #define DT_H_ODE_LIB "ode.lib"
#endif

   #define DT_H_OSGTEXT_LIB "osgText.lib"
#endif

#define DT_H_OPENGL_LIB "opengl32.lib"

// You may turn off this include message by defining _NOAUTOLIB 
#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " DT_H_OPENTHREADS_LIB )
   #pragma message( "Will automatically link with " DT_H_OSG_LIB )
   #pragma message( "Will automatically link with " DT_H_OSGDB_LIB )
   #pragma message( "Will automatically link with " DT_H_OSGPARTICLE_LIB )
   #pragma message( "Will automatically link with " DT_H_OSGUTIL_LIB )
   #pragma message( "Will automatically link with " DT_H_ODE_LIB )
   #pragma message( "Will automatically link with " DT_H_OSGTEXT_LIB )
   #pragma message( "Will automatically link with " DT_H_OPENGL_LIB )
   #ifndef DT_CORE_LIBRARY
      #pragma message( "Will automatically link with " DT_H_DTCORE_LIB )
   #endif
#endif

// Actually do the linking
#pragma comment(lib, DT_H_OPENTHREADS_LIB)
#pragma comment(lib, DT_H_OSG_LIB)
#pragma comment(lib, DT_H_OSGDB_LIB)
#pragma comment(lib, DT_H_OSGPARTICLE_LIB)
#pragma comment(lib, DT_H_OSGUTIL_LIB)
#pragma comment(lib, DT_H_ODE_LIB)
#pragma comment(lib, DT_H_OSGTEXT_LIB)
#pragma comment(lib, DT_H_OPENGL_LIB)
#ifndef DT_CORE_LIBRARY
   #pragma comment(lib, DT_H_DTCORE_LIB)
#endif

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_DT
