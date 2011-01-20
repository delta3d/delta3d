#ifndef DTUTIL_PREFIX
#define DTUTIL_PREFIX

#ifdef DT_USE_PCH

   #ifdef _MSC_VER
      #include <map>
      #include <osgDB/FileUtils>
   #else//_MSC_VER
      //no big payoffs here
   #endif//_MSC_VER

#endif //DT_USE_PCH

#endif //DTUTIL_PREFIX
