/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * John K. Grant
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
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
#include <sstream>

namespace dtUtil
{
   /// Math unit tests for dtUtil
   class MathTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(MathTests);
         CPPUNIT_TEST( TestNormalizer );
         CPPUNIT_TEST( TestEquivalentVec2 );
         CPPUNIT_TEST( TestEquivalentVec3);
         CPPUNIT_TEST( TestEquivalentVec4 );
		 CPPUNIT_TEST( TestMatrixEulerConversions );
       CPPUNIT_TEST( TestEquivalentReals );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestNormalizer();
         void TestEquivalentVec2();
         void TestEquivalentVec3();
         void TestEquivalentVec4();
         void TestEquivalentReals();
		 void TestMatrixEulerConversions();

      private:
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( MathTests );

   void MathTests::setUp()
   {
   }

   void MathTests::tearDown()
   {
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
}
