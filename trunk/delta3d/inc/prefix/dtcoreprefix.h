#ifndef DTCORE_PREFIX
#define DTCORE_PREFIX

#ifdef DT_USE_PCH
  #ifdef _MSC_VER
     #include <set>
     #include <osg/Export>
     #include <osg/MatrixTransform>
     #include <osg/GraphicsContext>
     #include <osg/Camera>
     #include <osgViewer/ViewerBase>
     #include <osgDB/ReadFile>
  #else //_MSC_VER
     #include <osg/Vec3f>
     #include <osg/Vec3d>
     #include <osg/Matrixf>
     #include <osg/Matrixd>
     #include <osg/ref_ptr>
     #include <iosfwd>
     #include <osg/Referenced>
     #include <osg/Config>
     #include <ode/common.h>
     #include <memory>
  #endif //_MSC_VER

#endif //DT_USE_PCH

#endif //DTCORE_PREFIX
