#ifndef DELTA_DTABC
#define DELTA_DTABC


#include "application.h"
#include "baseabc.h"
#include "weather.h"

#ifdef _WIN32

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3
#undef _AUTOLIBNAME4
#undef _AUTOLIBNAME5
#undef _AUTOLIBNAME6
#undef _AUTOLIBNAME7

#if defined(_DEBUG)
   #define _AUTOLIBNAME1   "dtabcD.lib"
   #define _AUTOLIBNAME2   "gui_fld.lib"
   #define _AUTOLIBNAME4   "fltkd.lib"
   #define _AUTOLIBNAME3   "tinyxmld.lib"
#else 
   #define _AUTOLIBNAME1   "dtabc.lib"  
   #define _AUTOLIBNAME2   "gui_fl.lib"
   #define _AUTOLIBNAME4   "fltk.lib"
   #define _AUTOLIBNAME3   "tinyxml.lib"
#endif

#define _AUTOLIBNAME5 "wsock32.lib"
#define _AUTOLIBNAME6 "comctl32.lib"
#define _AUTOLIBNAME7 "winmm.lib"

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME4 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME5 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME6 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME7 )
#endif

#pragma  comment( lib, _AUTOLIBNAME1 )
#pragma  comment( lib, _AUTOLIBNAME2 )
#pragma  comment( lib, _AUTOLIBNAME3 )
#pragma  comment( lib, _AUTOLIBNAME4 )
#pragma  comment( lib, _AUTOLIBNAME5 )
#pragma  comment( lib, _AUTOLIBNAME6 )
#pragma  comment( lib, _AUTOLIBNAME7 )

#endif // _WIN32

#endif // DELTA_DTABC
