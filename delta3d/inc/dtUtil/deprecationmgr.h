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

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
// Identifier was truncated to '255' characters in the debug information
#pragma warning( disable:4786 )  
#include <windows.h>
#else
#include <iostream>
#include <csignal>
#endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#include <string>
#include <map>
#include <set>


#ifdef _DEBUG
#define DEPRECATE(a,b) { \
		void * fptr;	\
		_asm { mov fptr, ebp }	\
		DeprecationMgr::GetInstance()->AddDeprecatedFunction(a, b, fptr); \
	}
#else
#define DEPRECATE(a,b)
#endif


class DeprecationMgr
{
  public:
	static DeprecationMgr * GetInstance ( void )
   {
      static DeprecationMgr Instance;
      return &Instance;
   }

	~DeprecationMgr ( void )
   {
      if ( !m_Functions.empty() )
      {
			#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
         OutputDebugString( "*************************************************************\n" );
         OutputDebugString( "WARNING. You are using the following deprecated functions:\n" );
			#else
			std::cout << "*************************************************************" << std::endl;
			std::cout << "WARNING. You are using the following deprecated functions:" << std::endl;
         #endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

         char txt[255];
         std::map<const char *, DeprecatedFunction>::iterator i;
         for ( i=m_Functions.begin(); i!=m_Functions.end(); ++i )
         {
            DeprecatedFunction * pFunction = &((*i).second);

            sprintf ( txt, "- Function %s called from %i different places.\n",
               pFunction->OldFunctionName, 
               pFunction->CalledFrom.size() );

            #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
            OutputDebugString (txt);         
		      #else
			   std::cout << txt;
            #endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
				

            sprintf ( txt, "  Instead use %s.\n", 
               pFunction->NewFunctionName );

            #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
            OutputDebugString (txt);         
		      #else
			   std::cout << txt;
            #endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
        
         }


         #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
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

      #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
      int CalledFrom = IsBadReadPtr( pReturn,4 ) ? 0 : *pReturn;
      #else

      int CalledFrom = 0;

      std::signal(SIGSEGV,SIG_IGN);
      
      if( pReturn )
         CalledFrom = *pReturn;
      
      #endif //defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

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

   DeprecationMgr ( void ) {};

	std::map<const char *, DeprecatedFunction> m_Functions;
};


#endif //_DEPRECATIONMGR_H_
