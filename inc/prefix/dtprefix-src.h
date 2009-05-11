#ifdef DELTA_PCH

#ifndef DELTA_PREFIX
#define DELTA_PREFIX

#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <map>
//#include <sstream>
#include <algorithm>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #if !defined (WIN32_LEAN_AND_MEAN)
       #define WIN32_LEAN_AND_MEAN
   #endif
   #if !defined (NOMINMAX)
       #define NOMINMAX
   #endif
   #include <windows.h>
#endif

//#include <python/dtpython.h>
#include <osg/Referenced>
#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Math>
#include <osg/io_utils>

//#include <cppunit/extensions/HelperMacros.h>

#endif
#endif
