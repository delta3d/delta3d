
#ifndef EntryPointExport_h__
#define EntryPointExport_h__


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#   ifdef ENTRY_POINT_LIBRARY
#      define ENTRY_POINT_EXPORT __declspec(dllexport)
#   else
#      define ENTRY_POINT_EXPORT __declspec(dllimport)
#   endif 
#else
#   define ENTRY_POINT_LIBRARY
#endif

#endif // EntryPointExport_h__
