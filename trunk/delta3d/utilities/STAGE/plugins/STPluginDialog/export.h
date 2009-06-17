#ifndef DELTA_EXAMPLE_EXPORT
#define DELTA_EXAMPLE_EXPORT

/**
* This is modeled from the DT_EXPORT macro found in dtCore/export.h.  
* We define another due to conflicts with using the DT_EXPORT while  
* trying to import Delta3D symbols.  The DT_EXAMPLE_EXPORT macro should be used
* in front of any classes that are to be exported from the testGameActorLibrary.
* Also note that DT_EXAMPLE_STAGE_PLUGIN should be defined in the compiler 
* preprocessor #defines.
*/
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef DT_PLUGIN_MANAGER_PLUGIN
#    define DT_PLUGIN_MANAGER_EXPORT __declspec(dllexport)
#  else
#    define DT_PLUGIN_MANAGER_EXPORT __declspec(dllimport)
#  endif
#else
#  ifdef DT_PLUGIN_MANAGER_PLUGIN
#    define DT_PLUGIN_MANAGER_EXPORT  __attribute__ ((visibility("default")))
#  else
#    define DT_PLUGIN_MANAGER_EXPORT
#  endif
#endif

#endif
