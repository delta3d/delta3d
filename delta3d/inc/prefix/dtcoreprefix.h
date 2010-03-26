#ifndef DTCORE_PREFIX
#define DTCORE_PREFIX

#ifdef DT_USE_PCH
  #ifdef _MSC_VER
     #include <set>
     #include <osg/Export>
     #include <osg/MatrixTransform>
     #include <osg/GraphicsContext>
     #include <osgViewer/ViewerBase>
     #include <osgDB/ReadFile>
  #else //_MSC_VER   
  #endif //_MSC_VER

  #include <osg/Camera>

#endif //DT_USE_PCH

#endif //DTCORE_PREFIX
