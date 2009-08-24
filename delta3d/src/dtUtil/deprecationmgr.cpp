#include <dtUtil/deprecationmgr.h>

#if defined(DELTA_WIN32)
   // Identifier was truncated to '255' characters in the debug information
   #pragma warning( disable:4786 )
   // disable warning for stl classes "needs to have dll-interface to be used by clients of class"
   #pragma warning(disable : 4251)

   #include <dtUtil/mswin.h>
#else
   #include <csignal>
   #include <iostream>
#endif

#include <sstream>
#include <map>
#include <set>

struct DeprecatedFunction
{
   const char*   OldFunctionName;
   const char*   NewFunctionName;
   std::set<int> CalledFrom;
};

class DeprecationMgrImpl
{
public:

   std::map<const char*, DeprecatedFunction> mFunctions;

};

//////////////////////////////////////////////////////////////////////////
DeprecationMgr& DeprecationMgr::GetInstance()
{
   static DeprecationMgr Instance;
   return Instance;
}

DeprecationMgr::DeprecationMgr()
: mImpl(new DeprecationMgrImpl)
{

}

//////////////////////////////////////////////////////////////////////////
DeprecationMgr::~DeprecationMgr()
{
   if (!mImpl->mFunctions.empty())
   {
#if defined(DELTA_WIN32)
      OutputDebugString( "*************************************************************\n" );
      OutputDebugString( "WARNING. You are using the following deprecated functions:\n" );
#else
      std::cerr << "*************************************************************" << std::endl;
      std::cerr << "WARNING. You are using the following deprecated functions:" << std::endl;
#endif // defined(DELTA_WIN32)

      //char txt[255];
      std::map<const char*, DeprecatedFunction>::iterator i;
      for (i = mImpl->mFunctions.begin(); i != mImpl->mFunctions.end(); ++i)
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
         std::cerr << oss.str();
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
      std::cerr << "*************************************************************" << std::endl;
#endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

      mImpl->mFunctions.clear();
   }
   delete mImpl;
   mImpl = NULL;
}

//////////////////////////////////////////////////////////////////////////
bool DeprecationMgr::AddDeprecatedFunction(const char* OldFunctionName, const char* NewFunctionName, const void* FramePtr)
{
   bool bAddedForFirstTime = false;

   int* pReturn = (int*)FramePtr+1; // usual return address @ [ebp+4]

#if defined(DELTA_WIN32)
   int CalledFrom = IsBadReadPtr(pReturn, 4) ? 0 : *pReturn;
#else

   int CalledFrom = 0;

   std::signal(SIGSEGV,SIG_IGN);

   if (pReturn)
   {
      CalledFrom = *pReturn;
   }

#endif //defined(DELTA_WIN32)

   // Check if this function was already listed as deprecated
   std::map<const char*, DeprecatedFunction>::iterator ExistingFunc;
   ExistingFunc = mImpl->mFunctions.find (OldFunctionName);

   // If it wasn't, make a new entry for it
   if (ExistingFunc == mImpl->mFunctions.end())
   {
      DeprecatedFunction Function;
      Function.OldFunctionName = OldFunctionName;
      Function.NewFunctionName = NewFunctionName;
      Function.CalledFrom.insert ( CalledFrom );

      mImpl->mFunctions[OldFunctionName] = Function;
      bAddedForFirstTime = true;
   }

   // If it was, keep track of where it's called from
   else
   {
      // Since we're keeping track of the addresses this function
      // was called from in a set, we don't need to check whether we've
      // already added the address.
      DeprecatedFunction* pFunction = &((*ExistingFunc).second);
      pFunction->CalledFrom.insert ( CalledFrom );
   }

   return bAddedForFirstTime;
}
