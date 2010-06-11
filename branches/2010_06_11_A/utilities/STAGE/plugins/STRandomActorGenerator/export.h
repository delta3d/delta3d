#ifndef STAGE_RANDOM_ACTOR_GENERATORS_EXPORT
#define STAGE_RANDOM_ACTOR_GENERATORS_EXPORT

/**
* This is modeled from the DT_EXPORT macro found in dtCore/export.h.  
* We define another due to conflicts with using the DT_EXPORT while  
* trying to import Delta3D symbols.  The STAGE_RANDOM_ACTOR_GENERATOR_EXPORT macro should be used
* in front of any classes that are to be exported from the testGameActorLibrary.
* Also note that STAGE_RANDOM_ACTOR_GENERATOR_PLUGIN should be defined in the compiler 
* preprocessor #defines.
*/
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef STAGE_RANDOM_ACTOR_GENERATOR_PLUGIN
#    define STAGE_RANDOM_ACTOR_GENERATOR_EXPORT __declspec(dllexport)
#  else
#    define STAGE_RANDOM_ACTOR_GENERATOR_EXPORT __declspec(dllimport)
#  endif
#else
#  ifdef STAGE_RANDOM_ACTOR_GENERATOR_PLUGIN
#    define STAGE_RANDOM_ACTOR_GENERATOR_EXPORT  __attribute__ ((visibility("default")))
#  else
#    define STAGE_RANDOM_ACTOR_GENERATOR_EXPORT
#  endif
#endif

#endif
