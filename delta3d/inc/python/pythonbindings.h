// pythonbindings.h: Python binding declarations.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_PYTHON_BINDINGS
#define DELTA_PYTHON_BINDINGS

#include "dt.h"

extern "C"
{
   void initDelta();
}

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

#if defined(_DEBUG)
   #define _AUTOLIBNAME  "dtpythond.lib"
   #define _AUTOLIBNAME1 "python23.lib"
   #define _AUTOLIBNAME2 "boost_python_debug.lib"
#else
   #define _AUTOLIBNAME  "dtpython.lib"
   #define _AUTOLIBNAME1 "python23.lib"
   #define _AUTOLIBNAME2 "boost_python.lib"
#endif

#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
#endif

#pragma comment (lib, _AUTOLIBNAME)
#pragma comment (lib, _AUTOLIBNAME1)
#pragma comment (lib, _AUTOLIBNAME2)

#endif // DELTA_PYTHON_BINDINGS