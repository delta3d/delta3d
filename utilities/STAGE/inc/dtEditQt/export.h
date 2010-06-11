#ifndef DELTA_STAGE_EXPORT
#define DELTA_STAGE_EXPORT

// export.h: Export definitions.  Modeled after osg/Export.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#   ifdef DT_EDITQT_LIBRARY
#      define DT_EDITQT_EXPORT __declspec(dllexport)
#   else
#      define DT_EDITQT_EXPORT __declspec(dllimport)
#   endif 
#else
#   ifdef DT_EDITQT_LIBRARY
#      define DT_EDITQT_EXPORT __attribute__ ((visibility("default")))
#   else
#      define DT_EDITQT_EXPORT
#   endif 
#endif


#endif // DELTA_STAGE_EXPORT
