#ifndef UNITTESTPREFIX_H__
#define UNITTESTPREFIX_H__

#ifdef DT_USE_PCH
#ifdef _MSC_VER
#include <dtUtil/librarysharingmanager.h>
#include <dtUtil/stringutils.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtABC/application.h>
#endif
#include <dtDAL/export.h>
#include <osg/GL>
#include <osg/Referenced>
#include <algorithm>
#include <cppunit/extensions/HelperMacros.h>
#endif

#endif // UNITTESTPREFIX_H__
