#if !defined(DELTA_INCLUDE)
#define  DELTA_INCLUDE

#include "camera.h"
#include "clouddome.h"
#include "Environment.h"
#include "effectmanager.h"
#include "globals.h"
#include "infiniteterrain.h"
#include "keyboard.h"
#include "orbitmotionmodel.h"
#include "particlesystem.h"
#include "system.h"
#include "window.h"


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

#if defined(_DEBUG)
   #ifndef DT_LIBRARY  
      #define  _AUTOLIBNAME5 "dtcoreD.lib"
   #endif
#else
   #ifndef DT_LIBRARY
      #define _AUTOLIBNAME5 "dtcore.lib"
   #endif
#endif

#define _AUTOLIBNAME   "Producer.lib"
#define _AUTOLIBNAME1  "sg.lib"
#define _AUTOLIBNAME2  "ul.lib"
#define _AUTOLIBNAME3  "winmm.lib"
#define _AUTOLIBNAME4  "OpenThreadsWin32.lib"  
#define _AUTOLIBNAME6  "osg.lib"
#define _AUTOLIBNAME7  "osgDB.lib"
#define _AUTOLIBNAME8  "osgParticle.lib"
#define _AUTOLIBNAME9  "tinyxml.lib"
#define _AUTOLIBNAME10 "osgUtil.lib"
#define _AUTOLIBNAME11 "ode.lib"
#define _AUTOLIBNAME12 "opcode.lib"
#define _AUTOLIBNAME13 "osgText.lib"
#define _AUTOLIBNAME14 "opengl32.lib"
#define _AUTOLIBNAME15 "osgTerrain.lib"
#define _AUTOLIBNAME16 "glGUI.lib"
#define _AUTOLIBNAME17 "osgGL2.lib"

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

#ifndef DT_LIBRARY
   #pragma comment(lib, _AUTOLIBNAME5)
#endif

#endif //DELTA_INCLUDE