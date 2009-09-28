#ifndef TEST_GA_COMPONENTS_EXPORT_H
#define TEST_GA_COMPONENTS_EXPORT_H


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef TEST_GA_COMPONENTS_LIBRARY
#    define TEST_GA_COMPONENTS_EXPORT __declspec(dllexport)
#  else
#    define TEST_GA_COMPONENTS_EXPORT __declspec(dllimport)
#  endif
#else
#   ifdef TEST_GA_COMPONENTS_LIBRARY
#      define TEST_GA_COMPONENTS_EXPORT __attribute__ ((visibility("default")))
#   else
#      define TEST_GA_COMPONENTS_EXPORT
#   endif 
#endif

#endif // TEST_GA_COMPONENTS_EXPORT_H
