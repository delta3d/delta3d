#ifndef UNITTESTPREFIX_H__
#define UNITTESTPREFIX_H__

#ifdef DT_USE_PCH
  #ifdef _MSC_VER
    #include <dtUtil/librarysharingmanager.h>
    #include <dtUtil/stringutils.h>
    #include <dtCore/enginepropertytypes.h>
    #include <dtABC/application.h>
    #include <dtCore/export.h>
    #include <osg/Referenced>
    #include <algorithm>
    
  #else
    #include <dtCore/scene.h>
  #endif

  #include <cppunit/extensions/HelperMacros.h>
  #include <osg/GL>

#endif

#endif // UNITTESTPREFIX_H__
