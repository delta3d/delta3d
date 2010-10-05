/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2010, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtHLAGM/environmentprocessrecordlist.h>
#include <dtDAL/namedgroupparameter.inl>
#include <dtUtil/mathdefines.h>
#include <dtUtil/coordinates.h>
#include <osg/Vec3d>
#include <osg/Vec3f>

namespace dtHLAGM
{

   class EnvironmentProcessRecordListTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(EnvironmentProcessRecordListTests);

      CPPUNIT_TEST(TestGetSet);
      CPPUNIT_TEST(TestEncodeDecodeBoundingSphere);
      CPPUNIT_TEST(TestEncodeDecodeSphere1);
      CPPUNIT_TEST(TestEncodeDecodeSphere2);
      CPPUNIT_TEST(TestEncodeDecodeRectangle2);
      CPPUNIT_TEST(TestEncodeDecodeRectangle1);
      CPPUNIT_TEST(TestEncodeDecodeRectangle2);
      CPPUNIT_TEST(TestEncodeDecodeEllipse1);
      CPPUNIT_TEST(TestEncodeDecodeEllipse2);
      CPPUNIT_TEST(TestEncodeDecodeGaussPuff);

      CPPUNIT_TEST_SUITE_END();

   public:

      EnvironmentProcessRecordListTests()
      : mEnvRecList(mCoord)
      {
      }

      void setUp()
      {
      }

      void tearDown()
      {
         mEnvRecList.GetRecords().clear();
      }

      void TestGetSet()
      {
         CPPUNIT_ASSERT_EQUAL(size_t(0U), mEnvRecList.GetNumRecords());

         EnvironmentProcessRecord& rec = mEnvRecList.AddRecord();
         rec.SetIndex(14U);
         CPPUNIT_ASSERT_EQUAL(14U, rec.GetIndex());

         CPPUNIT_ASSERT_EQUAL(size_t(1U), mEnvRecList.GetNumRecords());
         CPPUNIT_ASSERT(mEnvRecList.RemoveRecord(14));
         CPPUNIT_ASSERT_EQUAL(size_t(0U), mEnvRecList.GetNumRecords());

         EnvironmentProcessRecord& rec2 = mEnvRecList.AddRecord();
         EnvironmentProcessRecord& rec3 = mEnvRecList.AddRecord();
         rec2.SetIndex(11);
         rec3.SetIndex(1);
         CPPUNIT_ASSERT_EQUAL(size_t(2U), mEnvRecList.GetNumRecords());
         CPPUNIT_ASSERT(!mEnvRecList.RemoveRecord(14));
         CPPUNIT_ASSERT_EQUAL(size_t(2U), mEnvRecList.GetNumRecords());
         CPPUNIT_ASSERT(mEnvRecList.RemoveRecord(1));
         CPPUNIT_ASSERT_EQUAL(size_t(1U), mEnvRecList.GetNumRecords());
         CPPUNIT_ASSERT_EQUAL(11U, mEnvRecList.GetRecords().front()->GetIndex());
         CPPUNIT_ASSERT(mEnvRecList.RemoveRecord(11));
         CPPUNIT_ASSERT_EQUAL(size_t(0U), mEnvRecList.GetNumRecords());
      }

      void TestEncodeDecodeBoundingSphere()
      {
         TestEncodeDecodeSphere(EnvironmentProcessRecord::BoundingSphereRecordType, 2U, 32U);
      }

      void TestEncodeDecodeSphere1()
      {
         TestEncodeDecodeSphere(EnvironmentProcessRecord::SphereRecord1Type, 2U, 32U);
      }

      void TestEncodeDecodeSphere2()
      {
         TestEncodeDecodeSphere(EnvironmentProcessRecord::SphereRecord2Type, 2U, 56U);
      }

      void TestEncodeDecodeRectangle1()
      {
         TestEncodeDecodeEllipseOrGaussPuff(EnvironmentProcessRecord::RectangularVolRecord1Type, 2U, 48U);
      }

      void TestEncodeDecodeRectangle2()
      {
         TestEncodeDecodeEllipseOrGaussPuff(EnvironmentProcessRecord::RectangularVolRecord2Type, 4U, 88U);
      }

      void TestEncodeDecodeEllipse1()
      {
         TestEncodeDecodeEllipseOrGaussPuff(EnvironmentProcessRecord::EllipsoidRecord1Type, 1U, 48U);
      }

      void TestEncodeDecodeEllipse2()
      {
         TestEncodeDecodeEllipseOrGaussPuff(EnvironmentProcessRecord::EllipsoidRecord2Type, 11U, 88U);
      }

      void TestEncodeDecodeGaussPuff()
      {
         TestEncodeDecodeEllipseOrGaussPuff(EnvironmentProcessRecord::GaussianPuffRecordType, 27U, 112U);
      }

   private:

      void TestEncodeDecodeEllipseOrGaussPuff(EnvironmentProcessRecord::EnvironmentRecordTypeCode typeCode,
               unsigned int index, size_t recVariantSize)
      {
         EnvironmentProcessRecord& rec = mEnvRecList.AddRecord();
         rec.SetIndex(index);
         rec.SetTypeCode(typeCode);
         CPPUNIT_ASSERT_EQUAL(recVariantSize + size_t(16), rec.GetEncodedSize());
         CPPUNIT_ASSERT_EQUAL(size_t(rec.GetEncodedSize() + mEnvRecList.GetBaseSize()), mEnvRecList.GetEncodedSize());
         char bufferLarge[512];

         AddDataForElipsesAndPuffs(rec);
         TestEncodeData(rec, bufferLarge, 512U);

         EnvironmentProcessRecordList decodedRecList(mCoord);

         CPPUNIT_ASSERT(decodedRecList.Decode(bufferLarge, 512U));

         CPPUNIT_ASSERT_EQUAL(size_t(1), decodedRecList.GetNumRecords());

         EnvironmentProcessRecord& recCopy = *decodedRecList.GetRecords().front();

         CPPUNIT_ASSERT_EQUAL(unsigned(index), recCopy.GetIndex());
         CPPUNIT_ASSERT_EQUAL(unsigned(typeCode), recCopy.GetTypeCode());

         dtDAL::NamedGroupParameter& groupParam = recCopy.GetRecordData();

         CPPUNIT_ASSERT(dtUtil::Equivalent(mTestPos1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_LOCATION, osg::Vec3d()), 0.01));
         CPPUNIT_ASSERT_MESSAGE("Because it went through the coordinate converter, it should not be bit for bit equal",
                  mTestPos1 != groupParam.GetValue(EnvironmentProcessRecord::PARAM_LOCATION, osg::Vec3d()));
         CPPUNIT_ASSERT_EQUAL(mTestVec1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_DIMENSION, osg::Vec3f()));
         CPPUNIT_ASSERT(dtUtil::Equivalent(mTestOrient, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ORIENTATION, osg::Vec3f()), 0.01f));

         if (typeCode == EnvironmentProcessRecord::GaussianPuffRecordType)
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(mTestPos2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION, osg::Vec3d()), 0.01));
            CPPUNIT_ASSERT_MESSAGE("Because it went through the coordinate converter, it should not be bit for bit equal",
                     mTestPos2 != groupParam.GetValue(EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION, osg::Vec3d()));
            CPPUNIT_ASSERT_EQUAL(mTestHeight, groupParam.GetValue(EnvironmentProcessRecord::PARAM_CENTROID_HEIGHT, float(0.0f)));
         }
         else
         {
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION)  == NULL);
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_CENTROID_HEIGHT)  == NULL);
         }

         if (typeCode == EnvironmentProcessRecord::GaussianPuffRecordType || typeCode == EnvironmentProcessRecord::EllipsoidRecord2Type ||
                  typeCode == EnvironmentProcessRecord::RectangularVolRecord2Type)
         {
            CPPUNIT_ASSERT_EQUAL(mTestVec2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_DIMENSION_RATE, osg::Vec3f()));
            CPPUNIT_ASSERT(dtUtil::Equivalent(mTestVec1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_VELOCITY, osg::Vec3f()), 0.01f));
            CPPUNIT_ASSERT_EQUAL(mTestVec2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, osg::Vec3f()));
         }
         else
         {
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_DIMENSION_RATE)  == NULL);
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_VELOCITY)  == NULL);
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY)  == NULL);
         }

      }

      void TestEncodeDecodeSphere(EnvironmentProcessRecord::EnvironmentRecordTypeCode typeCode,
               unsigned int index, size_t recVariantSize)
      {
         EnvironmentProcessRecord& rec = mEnvRecList.AddRecord();
         rec.SetIndex(index);
         rec.SetTypeCode(typeCode);
         CPPUNIT_ASSERT_EQUAL(recVariantSize + size_t(16), rec.GetEncodedSize());
         CPPUNIT_ASSERT_EQUAL(size_t(rec.GetEncodedSize() + mEnvRecList.GetBaseSize()), mEnvRecList.GetEncodedSize());
         char bufferLarge[512];

         AddDataForSpheres(rec);
         TestEncodeData(rec, bufferLarge, 512U);

         EnvironmentProcessRecordList decodedRecList(mCoord);

         CPPUNIT_ASSERT(decodedRecList.Decode(bufferLarge, 512U));

         CPPUNIT_ASSERT_EQUAL(size_t(1), decodedRecList.GetNumRecords());

         EnvironmentProcessRecord& recCopy = *decodedRecList.GetRecords().front();

         CPPUNIT_ASSERT_EQUAL(unsigned(index), recCopy.GetIndex());
         CPPUNIT_ASSERT_EQUAL(unsigned(typeCode), recCopy.GetTypeCode());

         dtDAL::NamedGroupParameter& groupParam = recCopy.GetRecordData();

         CPPUNIT_ASSERT(dtUtil::Equivalent(mTestPos1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_LOCATION, osg::Vec3d()), 0.01));
         CPPUNIT_ASSERT_MESSAGE("Because it went through the coordinate converter, it should not be bit for bit equal",
                  mTestPos1 != groupParam.GetValue(EnvironmentProcessRecord::PARAM_LOCATION, osg::Vec3d()));
         CPPUNIT_ASSERT_EQUAL(mTestHeight, groupParam.GetValue(EnvironmentProcessRecord::PARAM_RADIUS, float(0.0f)));

         if (typeCode == EnvironmentProcessRecord::SphereRecord2Type)
         {
            CPPUNIT_ASSERT_EQUAL(mTestRate, groupParam.GetValue(EnvironmentProcessRecord::PARAM_RADIUS_RATE, float(0.0f)));
            CPPUNIT_ASSERT(dtUtil::Equivalent(mTestVec1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_VELOCITY, osg::Vec3f()), 0.01f));
            CPPUNIT_ASSERT_EQUAL(mTestVec2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, osg::Vec3f()));
         }
         else
         {
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_RADIUS_RATE)  == NULL);
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_VELOCITY)  == NULL);
            CPPUNIT_ASSERT(groupParam.GetParameter(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY)  == NULL);
         }

      }

      void TestEncodeData(EnvironmentProcessRecord& rec, char* bufferLarge, size_t size)
      {
         char bufferSmall[30];
         // test that encoding in a buffer that is too small will not crash
         size_t sizeOut = mEnvRecList.Encode(bufferSmall, 30);
         CPPUNIT_ASSERT(sizeOut > 30U);
         CPPUNIT_ASSERT_EQUAL(mEnvRecList.GetEncodedSize(), sizeOut);
         // Test with a buffer just one byte too small as a boundary case.
         CPPUNIT_ASSERT_EQUAL(mEnvRecList.GetEncodedSize(), mEnvRecList.Encode(bufferLarge, sizeOut - 1));

         CPPUNIT_ASSERT_EQUAL(mEnvRecList.GetEncodedSize(), mEnvRecList.Encode(bufferLarge, 512U));
      }

      void AddDataForElipsesAndPuffs(EnvironmentProcessRecord& rec)
      {
         dtDAL::NamedGroupParameter& groupParam = rec.GetRecordData();
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_LOCATION, mTestPos1);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION, mTestPos2);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_DIMENSION, mTestVec1);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_DIMENSION_RATE, mTestVec2);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_ORIENTATION, mTestOrient);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_VELOCITY, mTestVec1);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, mTestVec2);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_CENTROID_HEIGHT, mTestHeight);
      }

      void AddDataForSpheres(EnvironmentProcessRecord& rec)
      {
         dtDAL::NamedGroupParameter& groupParam = rec.GetRecordData();
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_LOCATION, mTestPos1);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_RADIUS, mTestHeight);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_RADIUS_RATE, mTestRate);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_VELOCITY, mTestVec1);
         groupParam.SetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, mTestVec2);
      }


      dtUtil::Coordinates mCoord;
      EnvironmentProcessRecordList mEnvRecList;

      static const osg::Vec3d mTestPos1;
      static const osg::Vec3d mTestPos2;
      static const osg::Vec3f mTestVec1;
      static const osg::Vec3f mTestVec2;

      static const osg::Vec3f mTestOrient;

      static const float mTestHeight;
      static const float mTestRate;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(EnvironmentProcessRecordListTests);
   const osg::Vec3d EnvironmentProcessRecordListTests::mTestPos1(-1000.4, 1.0, 7.3);
   const osg::Vec3d EnvironmentProcessRecordListTests::mTestPos2(11.4, -1.66, -4366.88);
   const osg::Vec3f EnvironmentProcessRecordListTests::mTestVec1(27.4f, -3.7f, 36.22f);
   const osg::Vec3f EnvironmentProcessRecordListTests::mTestVec2(19.22f, 11.6f, -0.0023f);

   const osg::Vec3f EnvironmentProcessRecordListTests::mTestOrient(-1.11, 3.77, 6.28f);

   const float EnvironmentProcessRecordListTests::mTestHeight = 1.88f;
   const float EnvironmentProcessRecordListTests::mTestRate = 8.73f;
}
