#ifndef STAGEPREFIX_H__
#define STAGEPREFIX_H__

#include <dtUtil/warningdisable.h>

#ifdef DT_USE_PCH

   #ifdef _MSC_VER
      #include <QtGui/QWidget>
      #include <QtCore/QObject>
      #include <QtCore/QVariant>
      #include <dtCore/transformable.h>
      #include <osg/Group>
   #else //_MSC_VER
      #include <stddef.h>
      #include <QtGui/QWidget>      
   #endif //_MSC_VER

#endif //DT_USE_PCH
#endif // STAGEPREFIX_H__
