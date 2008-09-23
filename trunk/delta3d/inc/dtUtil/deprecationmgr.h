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
/**      \class  DeprecationMgr
//      Tally usage of deprecated functions and report at end of execution.
//
//      Noel Llopis  --  11/20/2000
*/
//-----------------------------------------------------------------------------
#ifndef _DEPRECATIONMGR_H_
#define _DEPRECATIONMGR_H_

#include <dtUtil/macros.h>

#if defined(DELTA_WIN32)
// Identifier was truncated to '255' characters in the debug information
#pragma warning( disable:4786 )
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

#if defined( _DEBUG) && (defined DELTA_WIN32)
   #define DEPRECATE(a,b) { \
      void * fptr;	\
      _asm { mov fptr, ebp }                                       \
		DeprecationMgr::GetInstance().AddDeprecatedFunction(a, b, fptr); \
   }
#else
   #define DEPRECATE(a,b)
#endif


class DeprecationMgr
{
  public:
	static DeprecationMgr& GetInstance()
   {
      static DeprecationMgr Instance;
      return Instance;
   }

	~DeprecationMgr()
   {
      if (!m_Functions.empty())
      {
			#if defined(DELTA_WIN32)
         OutputDebugString( "*************************************************************\n" );
         OutputDebugString( "WARNING. You are using the following deprecated functions:\n" );
			#else
			std::cout << "*************************************************************" << std::endl;
			std::cout << "WARNING. You are using the following deprecated functions:" << std::endl;
         #endif // defined(DELTA_WIN32)

         //char txt[255];
         std::map<const char*, DeprecatedFunction>::iterator i;
         for (i = m_Functions.begin(); i != m_Functions.end(); ++i)
         {
            DeprecatedFunction* pFunction = &((*i).second);

            std::ostringstream oss;
            oss << "- Function " << pFunction->OldFunctionName << " called from " <<
                   pFunction->CalledFrom.size() << " different places.\n";

            /* sprintf ( txt, "- Function %s called from %u different places.\n",
               pFunction->OldFunctionName,
               (unsigned)pFunction->CalledFrom.size() ); */

            #if defined(DELTA_WIN32)
            OutputDebugString(oss.str().c_str());
		      #else
			   std::cout << oss.str();
            #endif // defined(DELTA_WIN32)

            oss.str("");
            oss << "  Instead use " << pFunction->NewFunctionName << ".\n",
            /* sprintf ( txt, "  Instead use %s.\n",
               pFunction->NewFunctionName ); */

            #if defined(DELTA_WIN32)
            OutputDebugString(oss.str().c_str());
		      #else
			   std::cout << oss.str();
            #endif // defined(DELTA_WIN32)

         }


         #if defined(DELTA_WIN32)
			OutputDebugString( "*************************************************************\n" );
		   #else
			std::cout << "*************************************************************" << std::endl;
         #endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

         m_Functions.clear();
      }
   }

	bool AddDeprecatedFunction ( const char * OldFunctionName,
								 const char * NewFunctionName,
								 const void * FramePtr )
   {
      bool bAddedForFirstTime = false;

      int *pReturn = (int*)FramePtr+1; // usual return address @ [ebp+4]

      #if defined(DELTA_WIN32)
      int CalledFrom = IsBadReadPtr( pReturn,4 ) ? 0 : *pReturn;
      #else

      int CalledFrom = 0;

      std::signal(SIGSEGV,SIG_IGN);

      if( pReturn )
         CalledFrom = *pReturn;

      #endif //defined(DELTA_WIN32)

      // Check if this function was already listed as deprecated
      std::map<const char *, DeprecatedFunction>::iterator ExistingFunc;
      ExistingFunc = m_Functions.find (OldFunctionName);

      // If it wasn't, make a new entry for it
      if ( ExistingFunc == m_Functions.end() )
      {
         DeprecatedFunction Function;
         Function.OldFunctionName = OldFunctionName;
         Function.NewFunctionName = NewFunctionName;
         Function.CalledFrom.insert ( CalledFrom );

         m_Functions[OldFunctionName] = Function;
         bAddedForFirstTime = true;
      }

      // If it was, keep track of where it's called from
      else
      {
         // Since we're keeping track of the addresses this function
         // was called from in a set, we don't need to check whether we've
         // already added the address.
         DeprecatedFunction * pFunction = &((*ExistingFunc).second);
         pFunction->CalledFrom.insert ( CalledFrom );
      }

      return bAddedForFirstTime;
   }

  private:
	struct DeprecatedFunction
	{
		const char *  OldFunctionName;
		const char *  NewFunctionName;
		std::set<int> CalledFrom;
	};

   DeprecationMgr() {}

	std::map<const char *, DeprecatedFunction> m_Functions;
};


#endif //_DEPRECATIONMGR_H_
