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
#include <dtCore/refptr.h>
#include <dtCore/physical.h>
#include <dtCore/scene.h>
#include <dtCore/odebodywrap.h>
#include <dtCore/odegeomwrap.h>
#include <dtCore/transform.h>

class PhysicalTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(PhysicalTests);
      CPPUNIT_TEST(TestGeomInit);
      CPPUNIT_TEST(TestBodyInit);
      CPPUNIT_TEST(TestGeomPosition);
      CPPUNIT_TEST(TestBodyPosition);
      CPPUNIT_TEST(TestBodyAndGeomPosition);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp() {};
   void tearDown() {};
   void TestGeomInit();
   void TestBodyInit();
   void TestGeomPosition();
   void TestBodyPosition();
   void TestBodyAndGeomPosition();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PhysicalTests);

//////////////////////////////////////////////////////////////////////////
void PhysicalTests::TestGeomInit()
{
   //verify some initial conditions with the collision detection geom.
   using namespace dtCore;
   RefPtr<Transformable> trans = new Transformable("testTrans");

   RefPtr<Scene> scene = new Scene();
   scene->AddDrawable(trans.get());

   trans->SetCollisionSphere(1.f);
   trans->SetCollisionDetection(true);

   const dGeomID geomID = trans->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE("Physical has no geom ID", geomID != 0);

   Transform physXform;
   trans->GetTransform(physXform);

   Transform geomXform;
   trans->GetGeomWrapper()->GetGeomTransform(geomXform);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Geom Transform wasn't initialized the same as the Physical Transform",
                                 true, physXform.EpsilonEquals(geomXform));

   scene->RemoveDrawable(trans.get());
}

//////////////////////////////////////////////////////////////////////////
void PhysicalTests::TestBodyInit()
{
   //verify some initial conditions with the body.
   using namespace dtCore;
   RefPtr<Physical> phys = new Physical("testPhys");

   RefPtr<Scene> scene = new Scene();
   scene->AddDrawable(phys.get());

   Transform physXform;
   phys->GetTransform(physXform);

   phys->EnableDynamics(true);

   const dBodyID bodyID = phys->GetBodyID();
   CPPUNIT_ASSERT_MESSAGE("Physical has no body ID", bodyID != 0);

   Transform bodyXform;
   phys->GetBodyWrapper()->GetBodyTransform(bodyXform);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Body Transform wasn't initialized the same as the Physical Transform",
                                 true, physXform.EpsilonEquals(bodyXform));

   scene->RemoveDrawable(phys.get());
}

//////////////////////////////////////////////////////////////////////////
void PhysicalTests::TestGeomPosition()
{
   using namespace dtCore;
   RefPtr<Transformable> trans = new Transformable("testTrans");

   RefPtr<Scene> scene = new Scene();
   scene->AddDrawable(trans.get());

   trans->SetCollisionSphere(1.f);
   trans->SetCollisionDetection(true);

   Transform startXform;
   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   const osg::Vec3 startHPR(10.f, 20.f, 30.f);
   startXform.SetTranslation(startXYZ);
   startXform.SetRotation(startHPR);

   trans->SetTransform(startXform);

   Transform geomXform;
   trans->GetGeomWrapper()->GetGeomTransform(geomXform);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Geom Transform didn't get set when Collision enabled, Dynamics not",
                                 true, startXform.EpsilonEquals(geomXform));

   scene->RemoveDrawable(trans.get());
}

//////////////////////////////////////////////////////////////////////////
void PhysicalTests::TestBodyPosition()
{
   //verify the ode body matches the Physical's Transform
   using namespace dtCore;
   RefPtr<Physical> phys = new Physical("testPhys");

   RefPtr<Scene> scene = new Scene();
   scene->AddDrawable(phys.get());

   phys->EnableDynamics(true);

   Transform startXform;
   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   const osg::Vec3 startHPR(10.f, 20.f, 30.f);
   startXform.SetTranslation(startXYZ);
   startXform.SetRotation(startHPR);

   phys->SetTransform(startXform);

   Transform bodyXform;
   phys->GetBodyWrapper()->GetBodyTransform(bodyXform);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Body Transform didn't get set when Dynamics are enabled, collision not enabled",
                                true, startXform.EpsilonEquals(bodyXform));

   scene->RemoveDrawable(phys.get());
}

//////////////////////////////////////////////////////////////////////////
void PhysicalTests::TestBodyAndGeomPosition()
{
   //verify the ode body and geom matches the Physical's Transform
   using namespace dtCore;
   RefPtr<Physical> phys = new Physical("testPhys");

   RefPtr<Scene> scene = new Scene();
   scene->AddDrawable(phys.get());

   phys->SetCollisionSphere(1.f);
   phys->SetCollisionDetection(true);
   phys->EnableDynamics(true);

   Transform startXform;
   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   const osg::Vec3 startHPR(10.f, 20.f, 30.f);
   startXform.SetTranslation(startXYZ);
   startXform.SetRotation(startHPR);

   phys->SetTransform(startXform);

   Transform geomXform;
   phys->GetGeomWrapper()->GetGeomTransform(geomXform);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Geom Transform didn't get set when Collision and Dynamics are enabled",
                                 true, startXform.EpsilonEquals(geomXform));

   Transform bodyXform;
   phys->GetBodyWrapper()->GetBodyTransform(bodyXform);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Body Transform didn't get set when Collision and Dynamics are enabled",
                                 true, startXform.EpsilonEquals(bodyXform));

   scene->RemoveDrawable(phys.get());
}

