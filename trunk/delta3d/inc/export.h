#ifndef DELTA_EXPORT
#define DELTA_EXPORT

// export.h: Export definitions.  Modeled after osg/Export.
//
//////////////////////////////////////////////////////////////////////


#ifdef DT_DLL
	#  ifdef DT_LIBRARY
	#    define DT_EXPORT   __declspec(dllexport)
	#  else
	#    define DT_EXPORT   __declspec(dllimport)
	#  endif /* DT_LIBRARY */
#else
	#  define DT_EXPORT
#endif


#endif // DELTA_EXPORT
