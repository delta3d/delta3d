#ifndef _dtdis_export_h_
#define _dtdis_export_h_

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef DT_DIS_LIBRARY
#    define DT_DIS_EXPORT __declspec(dllexport)
#  else
#    define DT_DIS_EXPORT __declspec(dllimport)
#  endif
#else
#  define DT_DIS_EXPORT
#endif

/// the Delta3D support framework for the DIS network protocol.
namespace dtDIS
{
}

#endif // _dtdis_export_h_
