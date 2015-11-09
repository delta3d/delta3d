/*
* Copyright, 2007, Alion Science and Technology Corporation, all rights reserved.
*
* Alion Science and Technology Corporation
* 5365 Robin Hood Road
* Norfolk, VA 23513
* (757) 857-5670, www.alionscience.com
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>

#include <dtPhysics/palutil.h>

namespace dtPhysics
{

   class PalUtilTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(PalUtilTests);
         CPPUNIT_TEST(testInversion);
         CPPUNIT_TEST(testMatrixConversion);
         CPPUNIT_TEST(testTransformConversion);
      CPPUNIT_TEST_SUITE_END();

   public:
      PalUtilTests() {}
      ~PalUtilTests() {}

      void setUp();
      void tearDown();

      void testInversion()
      {
         palMatrix4x4 pM, pMInv;
         mat_identity(&pM);
         mat_set_rotation(&pM, 0.3, 0.1, -1.4);
         mat_set_translation(&pM, -383.8, -1289.90, 0.337);
         mat_invert(&pMInv, &pM);

         dtCore::Transform osgMat, palInvToOsgMat;
         PalMatrixToTransform(osgMat, pM);
         PalMatrixToTransform(palInvToOsgMat, pMInv);
         CPPUNIT_ASSERT(osgMat.Invert());

         CPPUNIT_ASSERT(osgMat.EpsilonEquals(palInvToOsgMat, 0.0001f));
      }

      void testTransformConversion()
      {
         osg::Matrix m;
         for (unsigned i = 0; i < 3; ++i)
         {
            for (unsigned j = 0; j < 3; ++j)
            {
               m(i, j) = i * j;
            }
         }
         palMatrix4x4 pM;
         dtCore::Transform xform;
         xform.Set(m);
         dtCore::Transform xform2;

         TransformToPalMatrix(pM, xform);
         PalMatrixToTransform(xform2, pM);

         CPPUNIT_ASSERT(xform.EpsilonEquals(xform2));
      }

      void testMatrixConversion()
      {
         osg::Matrix m, m2;
         for (unsigned i = 0; i < 3; ++i)
         {
            for (unsigned j = 0; j < 3; ++j)
            {
               m(i, j) = i * j;
            }
         }
         palMatrix4x4 pM;

         TransformToPalMatrix(pM, m);
         PalMatrixToTransform(m2, pM);

         for (unsigned i = 0; i < 3; ++i)
         {
            for (unsigned j = 0; j < 3; ++j)
            {
               CPPUNIT_ASSERT_EQUAL(m(i, j), m2(i, j));
            }
         }
      }

      void testMaterialDefConversion()
      {
         palMaterialDesc desc;
         MaterialDef def, def2;

         def.SetStaticFriction(Real(0.7));
         def.SetKineticFriction(Real(4.7));
         def.SetRestitution(Real(12.0));
         def.SetDisableStrongFriction(true);
         def.SetEnableAnisotropicFriction(true);
         def.SetStaticAnisotropicFriction(osg::Vec3(32.1f, 18.1f, 17.3f));
         def.SetKineticAnisotropicFriction(osg::Vec3(1.3f, 2.9f, 0.7f));
         def.SetDirOfAnisotropy(osg::Vec3(1.0f, 0.5f, 1.2f));

         MatDefToPalMatDesc(desc, def);

         CPPUNIT_ASSERT_EQUAL(def.GetStaticFriction(), desc.m_fStatic);
         CPPUNIT_ASSERT_EQUAL(def.GetKineticFriction(), desc.m_fKinetic);
         CPPUNIT_ASSERT_EQUAL(def.GetRestitution(), desc.m_fRestitution);
         CPPUNIT_ASSERT_EQUAL(def.GetDisableStrongFriction(), desc.m_bDisableStrongFriction);
         CPPUNIT_ASSERT_EQUAL(def.GetEnableAnisotropicFriction(), desc.m_bEnableAnisotropicFriction);
         for (unsigned i=0; i < 3; ++i)
         {
            CPPUNIT_ASSERT_EQUAL(def.GetStaticAnisotropicFriction()[i], desc.m_vStaticAnisotropic[i]);
            CPPUNIT_ASSERT_EQUAL(def.GetKineticAnisotropicFriction()[i], desc.m_vKineticAnisotropic[i]);
            CPPUNIT_ASSERT_EQUAL(def.GetDirOfAnisotropy()[i], desc.m_vDirAnisotropy[i]);
         }
         PalMatDescToMatDef(def2, desc);

         CPPUNIT_ASSERT_EQUAL(def.GetStaticFriction(), def2.GetStaticFriction());
         CPPUNIT_ASSERT_EQUAL(def.GetKineticFriction(), def2.GetKineticFriction());
         CPPUNIT_ASSERT_EQUAL(def.GetRestitution(), def2.GetRestitution());
         CPPUNIT_ASSERT_EQUAL(def.GetDisableStrongFriction(), def2.GetDisableStrongFriction());
         CPPUNIT_ASSERT_EQUAL(def.GetEnableAnisotropicFriction(), def2.GetEnableAnisotropicFriction());
         for (unsigned i=0; i < 3; ++i)
         {
            CPPUNIT_ASSERT_EQUAL(def.GetStaticAnisotropicFriction()[i], def2.GetStaticAnisotropicFriction()[i]);
            CPPUNIT_ASSERT_EQUAL(def.GetKineticAnisotropicFriction()[i], def2.GetKineticAnisotropicFriction()[i]);
            CPPUNIT_ASSERT_EQUAL(def.GetDirOfAnisotropy()[i], def2.GetDirOfAnisotropy()[i]);
         }
      }
   private:

   };

   CPPUNIT_TEST_SUITE_REGISTRATION(PalUtilTests);

   /////////////////////////////////////////////////////////
   void PalUtilTests::setUp()
   {
      //mLogger = &dtUtil::Log::GetInstance("PalUtilTests.cpp");

   }

   /////////////////////////////////////////////////////////
   void PalUtilTests::tearDown()
   {
   }

}
