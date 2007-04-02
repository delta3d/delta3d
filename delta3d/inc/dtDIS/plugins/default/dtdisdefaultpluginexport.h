#ifndef _dtdis_default_plugin_export_h_
#define _dtdis_default_plugin_export_h_

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef DT_DIS_DEFAULT_PLUGIN
#    define DT_DIS_DEFAULT_EXPORT __declspec(dllexport)
#  else
#    define DT_DIS_DEFAULT_EXPORT __declspec(dllimport)
#  endif
#else
#  define DT_DIS_DEFAULT_EXPORT
#endif

#endif // _dtdis_default_plugin_export_h_
