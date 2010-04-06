/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, MOVES Institute
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
 * John K. Grant
 * David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>
#include <osg/Vec2>
#include <osg/Vec2d>
#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osg/Vec4d>
#include <osg/Matrix>
#include <osg/io_utils>
#include <cmath>
#include <limits>
#include <sstream>

namespace dtUtil
{
   /// Math unit tests for dtUtil
   class MathTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(MathTests);
         CPPUNIT_TEST(TestFiniteAndNAN);
         CPPUNIT_TEST(TestVecFinite);
         CPPUNIT_TEST(TestNormalizer);
         CPPUNIT_TEST(TestEquivalentVec2);
         CPPUNIT_TEST(TestEquivalentVec3);
         CPPUNIT_TEST(TestEquivalentVecUsingEqualVecs);
         CPPUNIT_TEST(TestEquivalentVecUsingSlightlyDifferentVecs);
         CPPUNIT_TEST(TestEquivalentVecUsingVeryDifferentVecs);
         CPPUNIT_TEST(TestEquivalentVec4);
         CPPUNIT_TEST(TestMatrixEulerConversions);
         CPPUNIT_TEST(TestEquivalentReals);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestFiniteAndNAN();
      void TestVecFinite();
      void TestNormalizer();
      void TestEquivalentVec2();
      void TestEquivalentVec3();
      void TestEquivalentVecUsingEqualVecs();
      void TestEquivalentVecUsingSlightlyDifferentVecs();
      void TestEquivalentVecUsingVeryDifferentVecs();
      void TestEquivalentVec4();
      void TestEquivalentReals();
    void TestMatrixEulerConversions();

   private:
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(MathTests);

   void MathTests::setUp()
   {
   }

   void MathTests::tearDown()
   {
   }

   void MathTests::TestVecFinite()
   {
      osg::Vec3f vecf1;
      osg::Vec4f vecf2;

      CPPUNIT_ASSERT(dtUtil::IsFiniteVec(vecf1));
      CPPUNIT_ASSERT(dtUtil::IsFiniteVec(vecf2));

      vecf1[1] = std::numeric_limits<float>::infinity();
      vecf2[3] = std::numeric_limits<float>::infinity();

      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecf1));
      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecf2));

      vecf1[1] = std::numeric_limits<float>::signaling_NaN();
      vecf2[3] = std::numeric_limits<float>::signaling_NaN();

      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecf1));
      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecf2));

      osg::Vec3d vecd1;
      osg::Vec4d vecd2;

      CPPUNIT_ASSERT(dtUtil::IsFiniteVec(vecd1));
      CPPUNIT_ASSERT(dtUtil::IsFiniteVec(vecd2));

      vecd1[1] = std::numeric_limits<double>::infinity();
      vecd2[3] = std::numeric_limits<double>::infinity();

      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecd1));
      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecd2));

      vecd1[1] = std::numeric_limits<double>::signaling_NaN();
      vecd2[3] = std::numeric_limits<double>::signaling_NaN();

      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecd1));
      CPPUNIT_ASSERT(!dtUtil::IsFiniteVec(vecd2));
   }

   void MathTests::TestFiniteAndNAN()
   {
      float f1 = 5.1f;
      double d1 = 6.1;

      CPPUNIT_ASSERT(dtUtil::IsFinite(f1));
      CPPUNIT_ASSERT(dtUtil::IsFinite(d1));

      CPPUNIT_ASSERT(!dtUtil::IsNAN(f1));
      CPPUNIT_ASSERT(!dtUtil::IsNAN(d1));

      f1 = std::numeric_limits<float>::infinity();
      d1 = std::numeric_limits<double>::infinity();

      CPPUNIT_ASSERT(!dtUtil::IsFinite(f1));
      CPPUNIT_ASSERT(!dtUtil::IsFinite(d1));

      CPPUNIT_ASSERT(!dtUtil::IsNAN(f1));
      CPPUNIT_ASSERT(!dtUtil::IsNAN(d1));

      f1 = std::numeric_limits<float>::quiet_NaN();
      d1 = std::numeric_limits<double>::quiet_NaN();

      CPPUNIT_ASSERT(!dtUtil::IsFinite(f1));
      CPPUNIT_ASSERT(!dtUtil::IsFinite(d1));

      CPPUNIT_ASSERT(dtUtil::IsNAN(f1));
      CPPUNIT_ASSERT(dtUtil::IsNAN(d1));

      f1 = -std::numeric_limits<float>::infinity();
      d1 = -std::numeric_limits<double>::infinity();

      CPPUNIT_ASSERT(!dtUtil::IsFinite(f1));
      CPPUNIT_ASSERT(!dtUtil::IsFinite(d1));

      CPPUNIT_ASSERT(!dtUtil::IsNAN(f1));
      CPPUNIT_ASSERT(!dtUtil::IsNAN(d1));

      f1 = std::numeric_limits<float>::signaling_NaN();
      d1 = std::numeric_limits<double>::signaling_NaN();

      CPPUNIT_ASSERT(!dtUtil::IsFinite(f1));
      CPPUNIT_ASSERT(!dtUtil::IsFinite(d1));

      CPPUNIT_ASSERT(dtUtil::IsNAN(f1));
      CPPUNIT_ASSERT(dtUtil::IsNAN(d1));
   }

   void MathTests::TestNormalizer()
   {
      // set up
      double x(-0.5), xMin(-1.0), xMax(1.0);
      double yMin(-1.0), yMax(3.0);

      double minTest = Min<double>(xMin, xMax);
      CPPUNIT_ASSERT_EQUAL(xMin, minTest);

      double maxTest = Max<double>(xMin, xMax);
      CPPUNIT_ASSERT_EQUAL(xMax, maxTest);

      double randRange = RandRange(5, 10);
      CPPUNIT_ASSERT(randRange <= 10 && randRange >= 5);

      float randFloat = RandFloat(float(xMin), float(xMax));
      CPPUNIT_ASSERT(randFloat <= xMax && randFloat >= xMin);

      float randPercent = RandPercent();
      CPPUNIT_ASSERT(randPercent <= 1.0 && randPercent >= 0.0);

      double absTest = Abs<double>(x);
      CPPUNIT_ASSERT_EQUAL( 0.5, absTest);

      Clamp<double>(absTest, xMin, xMax);
      CPPUNIT_ASSERT_EQUAL( 0.5, absTest);

      double xLerp = Lerp<double>(xMin, xMax, 0.25);
      CPPUNIT_ASSERT_EQUAL( x, xLerp);

      double xN( CalculateNormal(x,xMin,xMax) );
      CPPUNIT_ASSERT_EQUAL( 0.25 , xN );

      double my( MapRangeValue(x,xMin,xMax,yMin,yMax) );
      CPPUNIT_ASSERT_EQUAL( 0.0 , my );
   }

   void MathTests::TestEquivalentVec2()
   {
      osg::Vec2 v2a(3.31f, 3.32f);
      osg::Vec2 v2b(3.32f, 3.33f);

      CPPUNIT_ASSERT(!Equivalent(v2a, v2b, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v2a, v2b, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v2a, v2b, 0.1f));

      osg::Vec2f v2fa(3.31f, 3.32f);
      osg::Vec2f v2fb(3.32f, 3.33f);

      CPPUNIT_ASSERT(!Equivalent(v2fa, v2fb, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v2fa, v2fb, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v2fa, v2fb, 0.1f));

      osg::Vec2d v2da(3.31, 3.32);
      osg::Vec2d v2db(3.32, 3.33);

      CPPUNIT_ASSERT(!Equivalent(v2da, v2db, 1e-6));
      CPPUNIT_ASSERT(!Equivalent(v2da, v2db, 0.001));
      CPPUNIT_ASSERT(Equivalent(v2da, v2db, 0.1));
   }

   void MathTests::TestEquivalentVec3()
   {
      osg::Vec3 v3a(3.31f, 3.32f, 3.33f);
      osg::Vec3 v3b(3.32f, 3.33f, 3.34f);

      CPPUNIT_ASSERT(!Equivalent(v3a, v3b, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v3a, v3b, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v3a, v3b, 0.1f));

      osg::Vec3f v3fa(3.31f, 3.32f, 3.33f);
      osg::Vec3f v3fb(3.32f, 3.33f, 3.34f);

      CPPUNIT_ASSERT(!Equivalent(v3fa, v3fb, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v3fa, v3fb, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v3fa, v3fb, 0.1f));

      osg::Vec3d v3da(3.31, 3.32, 3.33);
      osg::Vec3d v3db(3.32, 3.33, 3.34);

      CPPUNIT_ASSERT(!Equivalent(v3da, v3db, 1e-6));
      CPPUNIT_ASSERT(!Equivalent(v3da, v3db, 0.001));
      CPPUNIT_ASSERT(Equivalent(v3da, v3db, 0.1));
   }

   //////////////////////////////////////////////////////////////////////////
   void MathTests::TestEquivalentVecUsingEqualVecs()
   {
      const osg::Vec3 v1(1.f, 2.f, 3.f);

      // easy case, this *better* be true
      CPPUNIT_ASSERT_MESSAGE("dtUtil::Equivalent(Vec3,Vec3) did not correctly compare equal Vec3",
                              Equivalent(v1, v1) == true );
   }

   //////////////////////////////////////////////////////////////////////////
   void MathTests::TestEquivalentVecUsingSlightlyDifferentVecs()
   {
      const osg::Vec3 v1(1.f, 2.f, 3.f);
      const osg::Vec3 v2(1.0000001f, 2.0000001f, 3.0000001f);

      // numbers are really close, should be equivalent
      CPPUNIT_ASSERT_MESSAGE("dtUtil::Equivalent(Vec3,Vec3) did not correctly compare slightly different Vec3",
                               Equivalent(v1, v2) == true );
   }

   //////////////////////////////////////////////////////////////////////////
   void MathTests::TestEquivalentVecUsingVeryDifferentVecs()
   {
      const osg::Vec3 v1(1.f, 2.f, 3.f);
      const osg::Vec3 v2(10.f, 20.f, 30.f);

      // numbers are really different, should *not* be equivalent
      CPPUNIT_ASSERT_MESSAGE("dtUtil::Equivalent(Vec3,Vec3) did not correctly compare very different Vec3",
                             Equivalent(v1, v2) == false );
   }

   //////////////////////////////////////////////////////////////////////////
   void MathTests::TestEquivalentVec4()
   {
      osg::Vec4 v4a(3.31f, 3.32f, 3.33f, 3.34f);
      osg::Vec4 v4b(3.32f, 3.33f, 3.34f, 3.33f);

      CPPUNIT_ASSERT(!Equivalent(v4a, v4b, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v4a, v4b, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v4a, v4b, 0.1f));

      osg::Vec4f v4fa(3.31f, 3.32f, 3.33f, 3.34f);
      osg::Vec4f v4fb(3.32f, 3.33f, 3.34f, 3.33f);

      CPPUNIT_ASSERT(!Equivalent(v4fa, v4fb, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v4fa, v4fb, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v4fa, v4fb, 0.1f));

      osg::Vec4d v4da(3.31, 3.32, 3.33, 3.34);
      osg::Vec4d v4db(3.32, 3.33, 3.34, 3.33);

      CPPUNIT_ASSERT(!Equivalent(v4da, v4db, 1e-6));
      CPPUNIT_ASSERT(!Equivalent(v4da, v4db, 0.001));
      CPPUNIT_ASSERT(Equivalent(v4da, v4db, 0.1));
   }

   void MathTests::TestMatrixEulerConversions()
   {
      float test_magic_epsilon = 1e-1f;

      osg::Matrix testMatrix;
      osg::Vec3 hprResult;

      // Test special cases with 90 degree angles
      osg::Vec3f hprTest1(0.0f, 89.0f, -10.0f);
      osg::Vec3f hprTest2(90.0f, 45.0f, 90.0f);
      osg::Vec3f hprTest3(5.0f, 80.0f, -10.0f);
      osg::Vec3f hprTest4(15.0f, 89.0f, 0.0f);

      std::ostringstream ss;
      ss.precision(8);

      dtUtil::MatrixUtil::HprToMatrix(testMatrix, hprTest1);
      dtUtil::MatrixUtil::MatrixToHpr(hprResult, testMatrix);

      ss.str("");
      ss << "Expected \"" << hprTest1 << "\" but got \"" << hprResult << "\"";

      CPPUNIT_ASSERT_MESSAGE(ss.str(), Equivalent(hprResult, hprTest1, test_magic_epsilon));

      dtUtil::MatrixUtil::HprToMatrix(testMatrix, hprTest2);
      dtUtil::MatrixUtil::MatrixToHpr(hprResult, testMatrix);

      ss.str("");
      ss << "Expected \"" << hprTest2 << "\" but got \"" << hprResult << "\"";

      CPPUNIT_ASSERT_MESSAGE(ss.str(), Equivalent(hprResult, hprTest2, test_magic_epsilon));

      dtUtil::MatrixUtil::HprToMatrix(testMatrix, hprTest3);
      dtUtil::MatrixUtil::MatrixToHpr(hprResult, testMatrix);

      ss.str("");
      ss << "Expected \"" << hprTest3 << "\" but got \"" << hprResult << "\"";

      CPPUNIT_ASSERT_MESSAGE(ss.str(), Equivalent(hprResult, hprTest3, test_magic_epsilon));

      dtUtil::MatrixUtil::HprToMatrix(testMatrix, hprTest4);
      dtUtil::MatrixUtil::MatrixToHpr(hprResult, testMatrix);

      ss.str("");
      ss << "Expected \"" << hprTest4 << "\" but got \"" << hprResult << "\"";

      CPPUNIT_ASSERT_MESSAGE(ss.str(), Equivalent(hprResult, hprTest4, test_magic_epsilon));
   }

   void MathTests::TestEquivalentReals()
   {
      float float1, float2;

      // It seems wierd, but if you are doing lots of math, and your values are in the 10 Million range,
      // Then the next representable number is actually in increments of 1.  So, if you compare this, using
      // the normal checks, it will say they are not equal, but in fact, because of hte level of precision
      // at that point, they are equal. So, to be most accurate, the Equivalent() method allows
      // for a scaling epsilon value.  It's hard to explain, but you can see the article mentioned in the
      // Equivalent method description.
      float1 = 10000000.0f;
      float2 = 10000001.0f;

      bool wasEqual;
      wasEqual = (float1 == float2);
      CPPUNIT_ASSERT_MESSAGE("Using regular epsilon type check, they should not be equal.", !wasEqual);

      wasEqual = Equivalent(float1, float2);
      CPPUNIT_ASSERT_MESSAGE("Floats should be the same using the new Equivalent test", wasEqual);

      float2 += 1.0;
      wasEqual = Equivalent(float1, float2);
      CPPUNIT_ASSERT_MESSAGE("They differ by the relative scaled epsilon, so they should not be equiv", !wasEqual);

      // Note, this should check doubles also, but I don't know the limit values for this.
   }
} // namespace dtUtil
