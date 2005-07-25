#ifndef _CPP_UNIT_TEST_H_
#define _CPP_UNIT_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/dt.h>
#if defined (WIN32) || defined (_Win32) || defined (__WIN32__)
    #include <soarx/dtsoarx.h>
#endif
#include "dtDAL/librarymanager.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/datatype.h"

using namespace dtDAL;
using namespace dtCore;

std::ostream& operator << (std::ostream &o, const osg::Vec3 &vec)
{
    o << vec[0] << ' ' << vec[1] << ' ' << vec[2];
    return o;
}
std::ostream& operator << (std::ostream &o, const osg::Vec4 &vec)
{
    o << vec[0] << ' ' << vec[1] << ' ' << vec[2] << ' ' << vec[3];
    return o;
}
std::ostream& operator << (std::ostream &o, const osg::Vec2 &vec)
{
    o << vec[0] << ' ' << vec[1];
    return o;
}

class ProxyTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(ProxyTest);

        CPPUNIT_TEST(testProxies);

    CPPUNIT_TEST_SUITE_END();

private:
    LibraryManager &libMgr;
    std::vector<osg::ref_ptr<ActorType> > actors;

    void testProps(ActorProxy& proxy);
    void compareProxies(ActorProxy& ap1, ActorProxy& ap2);

public:

    ProxyTest();

    virtual ~ProxyTest();

    virtual void setUp();

    virtual void tearDown();

    void testProxies();
};

#endif
