/*
 * dofpropertydelegatetests.cpp
 *
 *  Created on: May 29, 2015
 *      Author: david
 */

#include <dtCore/transformpropertydelegate.h>
#include <cppunit/extensions/HelperMacros.h>
#include <osg/io_utils>
#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>

namespace dtCore
{
   class TestContainer : public PropertyContainer
   {
   public:
      TestContainer() : mType(new ObjectType("Test1", "UnitTests", "", nullptr))
      {

      }

      const ObjectType& GetObjectType() const override
      {
         return *mType;
      }
   private:
      RefPtr<ObjectType> mType;
   };

   class TransformPropertyDelegateTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(TransformPropertyDelegateTests);
          CPPUNIT_TEST(TestCreateProperties);
          CPPUNIT_TEST(TestMatrixTransform);
          CPPUNIT_TEST(TestDOF);
       CPPUNIT_TEST_SUITE_END();

   public:
       TransformPropertyDelegateTests() {}
      ~TransformPropertyDelegateTests() {}

      void setUp() override {}
      void tearDown() override {}

      void TestCreateProperties()
      {
         dtCore::RefPtr<osg::MatrixTransform> mt = new osg::MatrixTransform();
         mt->setName("MrBossMan");

         TransformPropertyDelegatePtr tpd = new TransformPropertyDelegate(*mt);

         PropertyContainerPtr pc = new TestContainer;

         tpd->AddProperties(*pc);
         Vec3ActorProperty* testProp = nullptr;
         pc->GetProperty(tpd->GetTranslationPropertyName(), testProp);
         CPPUNIT_ASSERT(testProp != nullptr);
         pc->GetProperty(tpd->GetTranslationPropertyName(), testProp);
         CPPUNIT_ASSERT(testProp != nullptr);

         tpd->RemoveProperties(*pc);
         CPPUNIT_ASSERT(pc->GetProperty(tpd->GetTranslationPropertyName()) == nullptr);
         CPPUNIT_ASSERT(pc->GetProperty(tpd->GetTranslationPropertyName()) == nullptr);
      }

      void TestMatrixTransform()
      {
         dtCore::RefPtr<osg::MatrixTransform> mt = new osg::MatrixTransform();
         mt->setName("MrBossMan");

         TransformPropertyDelegatePtr tpd = new TransformPropertyDelegate(*mt);

         osg::Vec3 trans(3.75f, 2.125f, 9.5f);
         tpd->SetTranslation(trans);

         CPPUNIT_ASSERT_EQUAL(trans, tpd->GetTranslation());
         CPPUNIT_ASSERT_EQUAL(trans, osg::Vec3(mt->getMatrix().getTrans()));

         osg::Vec3 rot(trans);
         tpd->SetRotation(rot);

         CPPUNIT_ASSERT(dtUtil::Equivalent(rot, tpd->GetRotation(), 0.001f));
         osg::Vec3 readRot;
         dtUtil::MatrixUtil::MatrixToHpr(readRot, mt->getMatrix());
         CPPUNIT_ASSERT(dtUtil::Equivalent(rot, readRot, 0.001f));
      }

      void TestDOF()
      {
         dtCore::RefPtr<osgSim::DOFTransform> dof = new osgSim::DOFTransform();
         dof->setName("MrDOFMan");

         TransformPropertyDelegatePtr tpd = new TransformPropertyDelegate(*dof);
         osg::Vec3 trans(3.75f, 2.125f, 9.5f);
         tpd->SetTranslation(trans);

         CPPUNIT_ASSERT_EQUAL(trans, tpd->GetTranslation());
         CPPUNIT_ASSERT_EQUAL(trans, osg::Vec3(dof->getCurrentTranslate()));

         osg::Vec3 rot(trans);
         tpd->SetRotation(rot);

         CPPUNIT_ASSERT(dtUtil::Equivalent(rot, tpd->GetRotation(), 0.001f));
         osg::Vec3 rotDeg = dof->getCurrentHPR();
         for (unsigned i = 0; i < 3; ++i)
         {
            rotDeg[i] = osg::RadiansToDegrees(rotDeg[i]);
         }
         CPPUNIT_ASSERT(dtUtil::Equivalent(rot, rotDeg, 0.001f));
      }
   private:

   };

   CPPUNIT_TEST_SUITE_REGISTRATION(TransformPropertyDelegateTests);

} /* namespace dtCore */
