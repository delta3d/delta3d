/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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

#include <vector>
#include <string>
#include <iostream>
#include <dtHLAGM/spatial.h>
#include <dtUtil/mathdefines.h>

class SpatialTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(SpatialTests);

   CPPUNIT_TEST(TestGetSet);
   CPPUNIT_TEST(TestEncodeDecode1);
   CPPUNIT_TEST(TestEncodeDecode2);
   CPPUNIT_TEST(TestEncodeDecode3);
   CPPUNIT_TEST(TestEncodeDecode4);
   CPPUNIT_TEST(TestEncodeDecode5);
   CPPUNIT_TEST(TestEncodeDecode6);
   CPPUNIT_TEST(TestEncodeDecode7);
   CPPUNIT_TEST(TestEncodeDecode8);
   CPPUNIT_TEST(TestEncodeDecode9);
   CPPUNIT_TEST(TestHasVelocity);
   CPPUNIT_TEST(TestHasAcceleration);
   CPPUNIT_TEST(TestHasAngularVelocity);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp()
      {

      }

      void tearDown()
      {

      }

      void TestGetSet()
      {
         CPPUNIT_ASSERT_MESSAGE("Default DeadReckoningAlgorithm isn't 0",
                                 mSpatial.GetDeadReckoningAlgorithm() == 0);
         mSpatial.SetDeadReckoningAlgorithm(3);
         CPPUNIT_ASSERT(mSpatial.GetDeadReckoningAlgorithm() == 3);

         CPPUNIT_ASSERT_MESSAGE("Default IsFrozen() isn't false",
                                 mSpatial.IsFrozen() == false);
         mSpatial.SetFrozen(true);
         CPPUNIT_ASSERT(mSpatial.IsFrozen());

         osg::Vec3d zeroVec(0.0, 0.0, 0.0);

         //There is no reason to check setting these because other tests do that.
         CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3d>(mSpatial.GetWorldCoordinate(), zeroVec, 0.01));
         CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(mSpatial.GetOrientation(), zeroVec, 0.01f));
         CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(mSpatial.GetVelocity(), zeroVec, 0.01f));
         CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(mSpatial.GetAcceleration(), zeroVec, 0.01f));
         CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(mSpatial.GetAngularVelocity(), zeroVec, 0.01f));
      }

      void TestEncodeDecode1()
      {
         TestEncodeDecode(1);
      }

      void TestEncodeDecode2()
      {
         TestEncodeDecode(2);
      }

      void TestEncodeDecode3()
      {
         TestEncodeDecode(3);
      }

      void TestEncodeDecode4()
      {
         TestEncodeDecode(4);
      }

      void TestEncodeDecode5()
      {
         TestEncodeDecode(5);
      }

      void TestEncodeDecode6()
      {
         TestEncodeDecode(6);
      }

      void TestEncodeDecode7()
      {
         TestEncodeDecode(7);
      }

      void TestEncodeDecode8()
      {
         TestEncodeDecode(8);
      }

      void TestEncodeDecode9()
      {
         TestEncodeDecode(9);
      }

      void TestEncodeDecode(char drAlgo)
      {
         const size_t bufferSize = 256;
         char buffer[bufferSize];

         osg::Vec3d zeroVec(0.0, 0.0, 0.0);

         ///so it will be true some and false some.
         const bool frozen = drAlgo > 4 ? false: true;

         mSpatial.SetDeadReckoningAlgorithm(drAlgo);
         mSpatial.SetFrozen(frozen);
         mSpatial.GetWorldCoordinate().set(78.8, 736.2, 382.99);
         mSpatial.GetOrientation().set(3.14f, 0.03f, 4.223f);
         mSpatial.GetVelocity().set(1.0f, 1.3f, -2.1);
         mSpatial.GetAcceleration().set(-1.3f, 3.3f, 87.9f);
         mSpatial.GetAngularVelocity().set(-1.3f, 3.3f, 87.9f);

         size_t encodedSize = mSpatial.Encode(buffer, bufferSize);

         CPPUNIT_ASSERT(encodedSize > 0);

         dtHLAGM::Spatial decodedSpatial;
         for (size_t i = 0; i < encodedSize; ++i)
         {
            CPPUNIT_ASSERT_MESSAGE(
                     "decoding a buffer with any value less than the correct "
                     "encoded size, should return false",
                     decodedSpatial.Decode(buffer, encodedSize));
         }
         
         CPPUNIT_ASSERT(decodedSpatial.Decode(buffer, encodedSize));

         CPPUNIT_ASSERT_EQUAL(drAlgo, decodedSpatial.GetDeadReckoningAlgorithm());
         CPPUNIT_ASSERT_EQUAL(frozen, decodedSpatial.IsFrozen());
         CPPUNIT_ASSERT_EQUAL(mSpatial.HasVelocity(), decodedSpatial.HasVelocity());
         CPPUNIT_ASSERT_EQUAL(mSpatial.HasAcceleration(), decodedSpatial.HasAcceleration());
         CPPUNIT_ASSERT_EQUAL(mSpatial.HasAngularVelocity(), decodedSpatial.HasAngularVelocity());

         CPPUNIT_ASSERT(dtUtil::Equivalent(mSpatial.GetWorldCoordinate(),
                  decodedSpatial.GetWorldCoordinate(), 0.01));
         CPPUNIT_ASSERT(dtUtil::Equivalent(mSpatial.GetOrientation(),
                  decodedSpatial.GetOrientation(), 0.01f));

         if (mSpatial.HasVelocity())
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(mSpatial.GetVelocity(), 
                     decodedSpatial.GetVelocity(), 0.01f));
         }
         else
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(zeroVec, decodedSpatial.GetVelocity(), 0.01f));
         }

         if (mSpatial.HasAcceleration())
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(mSpatial.GetAcceleration(),
                     decodedSpatial.GetAcceleration(), 0.01f));
         }
         else
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(zeroVec, decodedSpatial.GetAcceleration(), 0.01f));
         }

         if (mSpatial.HasAngularVelocity())
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(mSpatial.GetAngularVelocity(),
                     decodedSpatial.GetAngularVelocity(), 0.01f));
         }
         else
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent<osg::Vec3f>(zeroVec, decodedSpatial.GetAngularVelocity(), 0.01f));
         }

         for (size_t i = 0; i < encodedSize; ++i)
         {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                     "encoding with a buffer of any size less than expected, should return 0",
                     size_t(0), mSpatial.Encode(buffer, i));
         }
      }

      void TestHasVelocity()
      {
         mSpatial.SetDeadReckoningAlgorithm(0);
         CPPUNIT_ASSERT(!mSpatial.HasVelocity());
         mSpatial.SetDeadReckoningAlgorithm(1);
         CPPUNIT_ASSERT(!mSpatial.HasVelocity());

         for (unsigned i = 2; i < 10; ++i)
         {
            mSpatial.SetDeadReckoningAlgorithm(i);
            CPPUNIT_ASSERT_MESSAGE("All algorithms other than 0 and 1 have a velocity vector.",
                     mSpatial.HasVelocity());
         }
      }

      void TestHasAcceleration()
      {
         mSpatial.SetDeadReckoningAlgorithm(0);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 0 should not have acceleration",
                  !mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(1);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 1 should not have acceleration",
                  !mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(2);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 2 should not have acceleration",
                  !mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(3);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 3 should not have acceleration",
                  !mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(6);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 6 should not have acceleration",
                  !mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(7);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 7 should not have acceleration",
                  !mSpatial.HasAcceleration());


         mSpatial.SetDeadReckoningAlgorithm(5);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 5 should have acceleration",
                  mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(9);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 9 should have acceleration",
                  mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(4);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 4 should have acceleration",
                  mSpatial.HasAcceleration());

         mSpatial.SetDeadReckoningAlgorithm(8);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 8 should have acceleration",
                  mSpatial.HasAcceleration());
      }

      void TestHasAngularVelocity()
      {
         mSpatial.SetDeadReckoningAlgorithm(0);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 0 should not have angular velocity",
                  !mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(1);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 1 should not have angular velocity",
                  !mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(2);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 2 should not have angular velocity",
                  !mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(5);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 3 should not have angular velocity",
                  !mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(6);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 6 should not have angular velocity",
                  !mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(9);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 7 should not have angular velocity",
                  !mSpatial.HasAngularVelocity());


         mSpatial.SetDeadReckoningAlgorithm(3);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 3 should have angular velocity",
                  mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(7);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 7 should have angular velocity",
                  mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(4);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 4 should have angular velocity",
                  mSpatial.HasAngularVelocity());

         mSpatial.SetDeadReckoningAlgorithm(8);
         CPPUNIT_ASSERT_MESSAGE("DR Algorithm 8 should have angular velocity",
                  mSpatial.HasAngularVelocity());
      }

   private:
      dtHLAGM::Spatial mSpatial;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SpatialTests);
