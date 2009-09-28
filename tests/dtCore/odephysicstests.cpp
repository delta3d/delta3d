/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/odebodywrap.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/odecontroller.h>
#include <dtCore/scene.h>
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
      CPPUNIT_TEST(TestODEControllerDestructor);
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
   void TestODEControllerDestructor();
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

//////////////////////////////////////////////////////////////////////////
void ODEPhysicsTests::TestODEControllerDestructor()
{
   CPPUNIT_ASSERT_EQUAL_MESSAGE("1 references should exist for ode including the unit test global unit test application.", 1U, dtCore::ODEController::GetODERefCount());

   dtCore::ObserverPtr<dtCore::ODEController> ctrl = new dtCore::ODEController();
 
   dtCore::ObserverPtr<dtCore::Scene> sceneObserver;
 
   {
      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene(ctrl.get());
      sceneObserver = scene.get();

      ctrl->SetMessageSender(scene.get());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("ODEController should have a reference count of 1.",
                                   1, ctrl->referenceCount());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("2 references should exist for ode.", 2U, dtCore::ODEController::GetODERefCount());
      dtCore::RefPtr<dtCore::Scene> scene2 = new dtCore::Scene();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("3 references should exist for ode.", 3U, dtCore::ODEController::GetODERefCount());
   }

   //scene goes out of scope, should be de-referenced and destroyed
   CPPUNIT_ASSERT_MESSAGE("Scene should have been destroyed, but it didn't.",
                           sceneObserver.get() == NULL);

   //Scene de-referenced ODEController so it should have been destroyed
   CPPUNIT_ASSERT_MESSAGE("ODEController should be deleted when the Scene does.",
                           ctrl.get() == NULL);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("1 reference should exist for ode because of the global unit test application.", 1U, dtCore::ODEController::GetODERefCount());
}
