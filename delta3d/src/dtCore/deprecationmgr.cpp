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
//  DeprecationMgr.cpp
//
//      See DeprecationMgr.h for a detailed description of this class.
//
//      Noel Llopis  --  11/20/2000
//
//-----------------------------------------------------------------------------
#pragma warning( disable:4786 )  

#include "dtCore/deprecationmgr.h"
#include <windows.h>
#include <stdio.h>

//DeprecationMgr::DeprecationMgr ( void )
//{
//}


//DeprecationMgr::~DeprecationMgr ( void )
//{
//	if ( !m_Functions.empty() )
//	{
//		OutputDebugString( "*************************************************************\n" );
//		OutputDebugString( "WARNING. You are using the following deprecated functions:\n" );
//
//		char txt[255];
//		std::map<const char *, DeprecatedFunction>::iterator i;
//		for ( i=m_Functions.begin(); i!=m_Functions.end(); ++i )
//		{
//			DeprecatedFunction * pFunction = &((*i).second);
//
//			sprintf ( txt, "- Function %s called from %i different places.\n",
//							pFunction->OldFunctionName, 
//							pFunction->CalledFrom.size() );
//			OutputDebugString (txt);
//
//			sprintf ( txt, "  Instead use %s.\n", 
//							pFunction->NewFunctionName );
//			OutputDebugString (txt);
//		}
//
//
//		OutputDebugString( "*************************************************************\n" );
//
//		m_Functions.clear();
//	}
//}


//DeprecationMgr * DeprecationMgr::GetInstance ( void )
//{
//	static DeprecationMgr Instance;
//	return &Instance;
//}


//bool DeprecationMgr::AddDeprecatedFunction ( const char * OldFunctionName, 
//											 const char * NewFunctionName,
//											 const void * FramePtr )
//{
//	bool bAddedForFirstTime = false;
//
//	int *pReturn = (int*)FramePtr+1; // usual return address @ [ebp+4]
//
//	int CalledFrom = IsBadReadPtr( pReturn,4 ) ? 0 : *pReturn;
//
//	// Check if this function was already listed as deprecated
//	std::map<const char *, DeprecatedFunction>::iterator ExistingFunc;
//	ExistingFunc = m_Functions.find (OldFunctionName);
//
//	// If it wasn't, make a new entry for it
//	if ( ExistingFunc == m_Functions.end() )
//	{
//		DeprecatedFunction Function;
//		Function.OldFunctionName = OldFunctionName;
//		Function.NewFunctionName = NewFunctionName;
//		Function.CalledFrom.insert ( CalledFrom );
//
//		m_Functions[OldFunctionName] = Function;
//		bAddedForFirstTime = true;
//	}
//
//	// If it was, keep track of where it's called from
//	else
//	{
//		// Since we're keeping track of the addresses this function
//		// was called from in a set, we don't need to check whether we've
//		// already added the address.
//		DeprecatedFunction * pFunction = &((*ExistingFunc).second);
//		pFunction->CalledFrom.insert ( CalledFrom );
//	}
//
//	return bAddedForFirstTime;
//}
