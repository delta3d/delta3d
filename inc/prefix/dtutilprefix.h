#ifndef DTUTIL_PREFIX
#define DTUTIL_PREFIX

#ifdef DT_USE_PCH

   #ifdef _MSC_VER
      #include <map>
      #include <osgDB/FileUtils>
   #else//_MSC_VER
      #include <osg/Referenced> 
      #include <osg/Vec3>
      #include <osg/ref_ptr>
      // Stuff you get from string
      #include <memory>
      #include <algorithm>
      #include <iosfwd>
   #endif//_MSC_VER

#endif //DT_USE_PCH

#endif //DTUTIL_PREFIX
