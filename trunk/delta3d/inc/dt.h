#ifndef DELTA_DT
#define DELTA_DT


#include "camera.h"
#include "clouddome.h"
#include "cloudplane.h"
#include "compass.h"
#include "cui_openglrenderer.h"
#include "cui_openglshader.h"
#include "deltadrawable.h"
#include "deltawin.h"
#include "environment.h"
#include "enveffect.h"
#include "effectmanager.h"
#include "flymotionmodel.h"
#include "globals.h"
#include "infiniteterrain.h"
#include "inputdevice.h"
#include "inputmapper.h"
#include "isector.h"
#include "joystick.h"
#include "keyboard.h"
#include "logicalinputdevice.h"
#include "motionmodel.h"
#include "mouse.h"
#include "notify.h"
#include "object.h"
#include "orbitmotionmodel.h"
#include "particlesystem.h"
#include "physical.h"
#include "pnoise.h"
#include "pointaxis.h"
#include "recorder.h"
#include "scene.h"
#include "skybox.h"
#include "skydome.h"
#include "stats.h"
#include "terrain.h"
#include "tracker.h"
#include "transform.h"
#include "transformable.h"
#include "tripod.h"
#include "system.h"
#include "ufomotionmodel.h"
#include "uidrawable.h"
#include "walkmotionmodel.h"


#ifdef _WIN32

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4
#undef _AUTOLIBNAME5
#undef _AUTOLIBNAME6
#undef _AUTOLIBNAME7
#undef _AUTOLIBNAME8
#undef _AUTOLIBNAME9
#undef _AUTOLIBNAME10
#undef _AUTOLIBNAME11
#undef _AUTOLIBNAME12
#undef _AUTOLIBNAME13
#undef _AUTOLIBNAME14
#undef _AUTOLIBNAME15
#undef _AUTOLIBNAME16
#undef _AUTOLIBNAME17
#undef _AUTOLIBNAME18
#undef _AUTOLIBNAME19
#undef _AUTOLIBNAME20
#undef _AUTOLIBNAME21

#if defined(_DEBUG)
   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME5 "dtcoreD.lib"
   #endif

   #define _AUTOLIBNAME   "Producerd.lib"
   #define _AUTOLIBNAME1  "sg_d.lib"
   #define _AUTOLIBNAME2  "ul_d.lib"
   #define _AUTOLIBNAME4  "OpenThreadsWin32d.lib"  
   #define _AUTOLIBNAME6  "osgd.lib"
   #define _AUTOLIBNAME7  "osgDBd.lib"
   #define _AUTOLIBNAME8  "osgParticled.lib"
   #define _AUTOLIBNAME9  "tinyxmld.lib"
   #define _AUTOLIBNAME10 "osgUtild.lib"
   #define _AUTOLIBNAME11 "oded.lib"
   #define _AUTOLIBNAME13 "osgTextd.lib"
   #define _AUTOLIBNAME15 "osgTerraind.lib"
   #define _AUTOLIBNAME16 "glGUI_d.lib"
   #define _AUTOLIBNAME17 "osgGL2d.lib"
   #define _AUTOLIBNAME18 "js_d.lib"
   #define _AUTOLIBNAME21 "isensed.lib"
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME5 "dtcore.lib"
   #endif

   #define _AUTOLIBNAME   "Producer.lib"
   #define _AUTOLIBNAME1  "sg.lib"
   #define _AUTOLIBNAME2  "ul.lib"
   #define _AUTOLIBNAME4  "OpenThreadsWin32.lib"  
   #define _AUTOLIBNAME6  "osg.lib"
   #define _AUTOLIBNAME7  "osgDB.lib"
   #define _AUTOLIBNAME8  "osgParticle.lib"
   #define _AUTOLIBNAME9  "tinyxml.lib"
   #define _AUTOLIBNAME10 "osgUtil.lib"
   #define _AUTOLIBNAME11 "ode.lib"
   #define _AUTOLIBNAME13 "osgText.lib"
   #define _AUTOLIBNAME15 "osgTerrain.lib"
   #define _AUTOLIBNAME16 "glGUI.lib"
   #define _AUTOLIBNAME17 "osgGL2.lib"
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
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME6 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME7 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME8 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME9 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME10 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME11 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME12 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME13 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME14 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME15 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME16 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME17 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME18 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME19 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME20 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME21 )

   #ifndef DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME5 )
   #endif
#endif

#pragma comment(lib, _AUTOLIBNAME)
#pragma comment(lib, _AUTOLIBNAME1)
#pragma comment(lib, _AUTOLIBNAME2)
#pragma comment(lib, _AUTOLIBNAME3)
#pragma comment(lib, _AUTOLIBNAME4)
#pragma comment(lib, _AUTOLIBNAME6)
#pragma comment(lib, _AUTOLIBNAME7)
#pragma comment(lib, _AUTOLIBNAME8)
#pragma comment(lib, _AUTOLIBNAME9)
#pragma comment(lib, _AUTOLIBNAME10)
#pragma comment(lib, _AUTOLIBNAME11)
#pragma comment(lib, _AUTOLIBNAME12)
#pragma comment(lib, _AUTOLIBNAME13)
#pragma comment(lib, _AUTOLIBNAME14)
#pragma comment(lib, _AUTOLIBNAME15)
#pragma comment(lib, _AUTOLIBNAME16)
#pragma comment(lib, _AUTOLIBNAME17)
#pragma comment(lib, _AUTOLIBNAME18)
#pragma comment(lib, _AUTOLIBNAME19)
#pragma comment(lib, _AUTOLIBNAME20)
#pragma comment(lib, _AUTOLIBNAME21)

#ifndef DT_LIBRARY
   #pragma comment(lib, _AUTOLIBNAME5)
#endif

#endif // _WIN32

#endif // DELTA_DT
