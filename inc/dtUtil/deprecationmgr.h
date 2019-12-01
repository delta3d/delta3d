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

#include <dtUtil/export.h>

//This will generate a compile-time warning when a function is prefaced.
#if defined (_MSC_VER)
   #define DEPRECATE_FUNC  __declspec(deprecated)
#elif __GNUC__
   #define DEPRECATE_FUNC __attribute__ ((deprecated))
#endif


#if defined( _DEBUG) && (defined _MSC_VER) && !defined(_WIN64)
   #define DEPRECATE(a,b) {                                            \
      void * fptr;                                                     \
      __asm { mov fptr, ebp }                                           \
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


class DeprecationMgrImpl;

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
   DeprecationMgr();
   DeprecationMgrImpl* mImpl;
};


#endif //_DEPRECATIONMGR_H_
