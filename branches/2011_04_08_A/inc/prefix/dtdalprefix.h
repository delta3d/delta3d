#ifndef DTDALPREFIX_H__
#define DTDALPREFIX_H__

#ifdef DT_USE_PCH

  #ifdef _MSC_VER
      #include <set>
      #include <osg/Matrix>
      #include <osg/Vec3>
      #include <osg/Group>
      #include <dtUtil/xercesutils.h>
  #else //_MSC_VER
      #include <dtCore/deltadrawable.h>
  #endif //_MSC_VER

#endif //DT_USE_PCH

#endif // DTDALPREFIX_H__
