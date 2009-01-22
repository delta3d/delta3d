/* Copyright (C) Noel Llopis, 2001.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */

//-----------------------------------------------------------------------------
//
//  DeprecationMgr.h
//
/*      \class  DeprecationMgr
//      Tally usage of deprecated functions and report at end of execution.
//
//      Noel Llopis  --  11/20/2000
*/
//-----------------------------------------------------------------------------

#ifndef _DEPRECATIONMGR_H_
#define _DEPRECATIONMGR_H_

#include <dtUtil/macros.h>
#include <dtUtil/export.h>

#if defined(DELTA_WIN32)
   // Identifier was truncated to '255' characters in the debug information
   #pragma warning( disable:4786 )
   // disable warning for stl classes "needs to have dll-interface to be used by clients of class"
   #pragma warning(disable : 4251)

   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #undef GetClassName
   #undef SendMessage
#else
   #include <iostream>
   #include <csignal>
#endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#include <string>
#include <map>
#include <set>
#include <sstream>


//This will generate a compile-time warning when a function is prefaced.
#if defined (DELTA_WIN32)
   #define DEPRECATE_FUNC  __declspec(deprecated)
#elif __GNUC__
   #define DEPRECATE_FUNC __attribute__ ((deprecated))
#endif


#if defined( _DEBUG) && (defined DELTA_WIN32)
   #define DEPRECATE(a,b) {                                            \
      void * fptr;                                                     \
      _asm { mov fptr, ebp }                                           \
      DeprecationMgr::GetInstance().AddDeprecatedFunction(a, b, fptr); \
   }
#elif defined (_DEBUG) && defined (__GNUC__)
   static void* GetEBP() 
   {
     void* ebp;
     __asm__ __volatile__(
      "movl (%%ebp), %0"
      : "=a" (ebp)
     );
     return ebp;
   }

   #define DEPRECATE(a,b) {        \
      DeprecationMgr::GetInstance().AddDeprecatedFunction(a,b,GetEBP()); \
   }
#else
   #define DEPRECATE(a,b)
#endif

/**
* Used to deprecate a method.  To deprecate a method use:
* @code
* bool NewMethod(float);
* DEPRECATE_FUNC void OldMethod(int)
* {
*    DEPRECATE("void OldMethod(int)"
*              "bool NewMethod(float)");
*
*    NewMethod(float(int)));
* }
* @endcode
*/
class DT_UTIL_EXPORT DeprecationMgr
{
public:
   static DeprecationMgr& GetInstance();

   ~DeprecationMgr();

   bool AddDeprecatedFunction(const char* OldFunctionName,
                              const char* NewFunctionName,
                              const void* FramePtr);

private:
   struct DeprecatedFunction
   {
      const char*   OldFunctionName;
      const char*   NewFunctionName;
      std::set<int> CalledFrom;
   };

   DeprecationMgr() {}

   std::map<const char*, DeprecatedFunction> m_Functions;
};


#endif //_DEPRECATIONMGR_H_
