#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/odebodywrap.h>
#include <dtCore/refptr.h>
#include <dtUtil/mathdefines.h>
#include <ode/objects.h>

class ODEPhysicsTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ODEPhysicsTests);
      CPPUNIT_TEST(TestEnablingWithoutBody);
      CPPUNIT_TEST(TestEnablingBeforeBodyAssignment);
      CPPUNIT_TEST(TestEnablingAfterBodyAssignment);
      CPPUNIT_TEST(TestSettingMassBeforeBodyAssignment);
      CPPUNIT_TEST(TestSettingThePosition);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp() {};
   void tearDown() {};

   void TestEnablingWithoutBody();
   void TestEnablingBeforeBodyAssignment();
   void TestEnablingAfterBodyAssignment();
   void TestSettingMassBeforeBodyAssignment();
   void TestSettingThePosition();
   void TestSettingTheCoG();	
};

CPPUNIT_TEST_SUITE_REGISTRATION(ODEPhysicsTests);

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestEnablingWithoutBody()
{
   using namespace dtCore;

   RefPtr<ODEBodyWrap> wrap = new ODEBodyWrap();

   wrap->EnableDynamics(false);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Dynamics should be false",
                                false, wrap->DynamicsEnabled());

   wrap->EnableDynamics(true);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Dynamics should be true",
                                 true, wrap->DynamicsEnabled());
}

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestEnablingBeforeBodyAssignment()
{
   using namespace dtCore;
   dWorldID worldID = dWorldCreate();

   RefPtr<ODEBodyWrap> wrap = new ODEBodyWrap();

   dBodyID id = dBodyCreate(worldID);
   wrap->EnableDynamics(true);
   wrap->SetBodyID(id);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("ODE Body should be enabled",
                                1, dBodyIsEnabled(id));

   wrap = NULL;
   dWorldDestroy(worldID);
}

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestEnablingAfterBodyAssignment()
{
   using namespace dtCore;
   dWorldID worldID = dWorldCreate();

   RefPtr<ODEBodyWrap> wrap = new ODEBodyWrap();

   dBodyID id = dBodyCreate(worldID);
   wrap->SetBodyID(id);
   wrap->EnableDynamics(true);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("ODE Body should be enabled",
                                 1, dBodyIsEnabled(id));

   wrap = NULL;
   dWorldDestroy(worldID);
}

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestSettingMassBeforeBodyAssignment()
{
   //Ensure the body gets the Mass when the body ID has been
   //assigned after setting the Mass.

   using namespace dtCore;
   dWorldID worldID = dWorldCreate();

   RefPtr<ODEBodyWrap> wrap = new ODEBodyWrap();

   dMass startMass;
   dMassSetSphere(&startMass, 10.0, 10.0);
   wrap->SetMass(&startMass);

   dBodyID id = dBodyCreate(worldID);
   wrap->SetBodyID(id);

   dMass returnedMass;

   dBodyGetMass(id, &returnedMass);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Supplied Mass was not returned",
                                returnedMass.mass, startMass.mass);

   wrap = NULL;
   dWorldDestroy(worldID);
}

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestSettingThePosition()
{
   using namespace dtCore;
   dWorldID worldID = dWorldCreate();

   RefPtr<ODEBodyWrap> wrap = new ODEBodyWrap();

   dBodyID id = dBodyCreate(worldID);
   wrap->SetBodyID(id);

   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   wrap->SetPosition(startXYZ);

   const dReal *pos = dBodyGetPosition(wrap->GetBodyID());
   const osg::Vec3 endXYZ(pos[0], pos[1], pos[2]);

   CPPUNIT_ASSERT_MESSAGE("Supplied position doesn't match the returned position",
                           dtUtil::Equivalent(startXYZ, endXYZ, 1e-5f));

   wrap = NULL;
   dWorldDestroy(worldID);
}

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestSettingTheCoG()
{
   using namespace dtCore;
   dWorldID worldID = dWorldCreate();

   RefPtr<ODEBodyWrap> wrap = new ODEBodyWrap();

   dBodyID id = dBodyCreate(worldID);
   wrap->SetBodyID(id);

   const osg::Vec3 startCog(10.f, 10.f, 10.f);
   wrap->SetCenterOfGravity(startCog);

   osg::Vec3 endCog;
   wrap->GetCenterOfGravity(endCog);

   CPPUNIT_ASSERT_MESSAGE("Supplied COG doesn't match the returned COG",
                           dtUtil::Equivalent(startCog, endCog, 1e-5f));

   dMass mass;
   dBodyGetMass(id, &mass);
   const osg::Vec3 bodyCog(mass.c[0], mass.c[1], mass.c[2]);

   CPPUNIT_ASSERT_MESSAGE("Supplied COG doesn't match the body's COG",
                           dtUtil::Equivalent(startCog, bodyCog, 1e-5f));

   wrap = NULL;
   dWorldDestroy(worldID);

}
