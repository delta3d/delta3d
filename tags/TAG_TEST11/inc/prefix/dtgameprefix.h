#ifndef DTGAMEPREFIX_H__
#define DTGAMEPREFIX_H__

#ifdef DT_USE_PCH

   #ifdef _MSC_VER
      #include <set>
      #include <dtDAL/physicalactorproxy.h>
      #include <dtDAL/namedparameter.h>
      #include <osg/Vec2>
      #include <osgGA/GUIEventAdapter>
   #else //_MSC_VER
     #include <dtUtil/refstring.h>
     #include <dtDAL/propertycontainer.h>
     #include <dtDAL/physicalactorproxy.h>
   #endif //_MSC_VER

#endif //DT_USE_PCH

#endif // DTGAMEPREFIX_H__
