#ifndef DELTA_EXPORT
#define DELTA_EXPORT

// export.h: Export definitions.  Modeled after osg/Export.
//
//////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
	#  ifdef DT_LIBRARY
	#    define DT_EXPORT   __declspec(dllexport)
	#  else
	#    define DT_EXPORT   __declspec(dllimport)
	#  endif /* DT_LIBRARY */
#else
	#  define DT_EXPORT
#endif


#endif // DELTA_EXPORT
