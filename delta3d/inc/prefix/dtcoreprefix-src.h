#ifdef DELTA_PCH

#ifndef DTCORE_PREFIX
#define DTCORE_PREFIX

#include <prefix/dtutilprefix-src.h>

#include <dtCore/base.h>
#include <dtCore/macros.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/transformable.h>

//for stage
#ifndef NO_DT_WIN_PCH_HEADER

#if !defined (WIN32) && !defined (_WIN32) && !defined (__WIN32__)
   #include <dtCore/mouse.h>
   #include <dtCore/keyboard.h>
   #include <dtCore/deltawin.h>
#endif

#endif

#include <dtCore/uniqueid.h>
#include <dtCore/sigslot.h>

#endif
#endif
