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
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/mathdefines.h>
#include <osg/Vec2>
#include <osg/Vec2d>
#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osg/Vec4d>
#include <cmath>

namespace dtUtil
{
   /// Math unit tests for dtUtil
   class MathTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( MathTests );
         CPPUNIT_TEST( TestNormalizer );
         CPPUNIT_TEST( TestEquivalentVec2 );
         CPPUNIT_TEST( TestEquivalentVec3);
         CPPUNIT_TEST( TestEquivalentVec4 );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestNormalizer();
         void TestEquivalentVec2();
         void TestEquivalentVec3();
         void TestEquivalentVec4();

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

      double y( CalculateValueForRange(xN,yMin,yMax) );
      CPPUNIT_ASSERT_EQUAL( 0.0 , y );

      double my( MapRangeValue(x,xMin,xMax,yMin,yMax) );
      CPPUNIT_ASSERT_EQUAL( 0.0 , my );
   }

   void MathTests::TestEquivalentVec2()
   {
      osg::Vec2 v2a(3.31f, 3.32f);
      osg::Vec2 v2b(3.32f, 3.33f);
      
      CPPUNIT_ASSERT(!Equivalent(v2a, v2b, 2, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v2a, v2b, 2, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v2a, v2b, 2, 0.1f));

      osg::Vec2f v2fa(3.31f, 3.32f);
      osg::Vec2f v2fb(3.32f, 3.33f);
      
      CPPUNIT_ASSERT(!Equivalent(v2fa, v2fb, 2, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v2fa, v2fb, 2, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v2fa, v2fb, 2, 0.1f));

      osg::Vec2d v2da(3.31, 3.32);
      osg::Vec2d v2db(3.32, 3.33);
      
      CPPUNIT_ASSERT(!Equivalent(v2da, v2db, 2, 1e-6));
      CPPUNIT_ASSERT(!Equivalent(v2da, v2db, 2, 0.001));
      CPPUNIT_ASSERT(Equivalent(v2da, v2db, 2, 0.1));
   }

   void MathTests::TestEquivalentVec3()
   {
      osg::Vec3 v3a(3.31f, 3.32f, 3.33f);
      osg::Vec3 v3b(3.32f, 3.33f, 3.34f);
      
      CPPUNIT_ASSERT(!Equivalent(v3a, v3b, 3, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v3a, v3b, 3, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v3a, v3b, 3, 0.1f));

      osg::Vec3f v3fa(3.31f, 3.32f, 3.33f);
      osg::Vec3f v3fb(3.32f, 3.33f, 3.34f);
      
      CPPUNIT_ASSERT(!Equivalent(v3fa, v3fb, 3, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v3fa, v3fb, 3, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v3fa, v3fb, 3, 0.1f));

      osg::Vec3d v3da(3.31, 3.32, 3.33);
      osg::Vec3d v3db(3.32, 3.33, 3.34);
      
      CPPUNIT_ASSERT(!Equivalent(v3da, v3db, 3, 1e-6));
      CPPUNIT_ASSERT(!Equivalent(v3da, v3db, 3, 0.001));
      CPPUNIT_ASSERT(Equivalent(v3da, v3db, 3, 0.1));
   }

   void MathTests::TestEquivalentVec4()
   {
      osg::Vec4 v4a(3.31f, 3.32f, 3.33f, 3.34f);
      osg::Vec4 v4b(3.32f, 3.33f, 3.34f, 3.33f);
      
      CPPUNIT_ASSERT(!Equivalent(v4a, v4b, 4, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v4a, v4b, 4, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v4a, v4b, 4, 0.1f));

      osg::Vec4f v4fa(3.31f, 3.32f, 3.33f, 3.34f);
      osg::Vec4f v4fb(3.32f, 3.33f, 3.34f, 3.33f);
      
      CPPUNIT_ASSERT(!Equivalent(v4fa, v4fb, 4, 1e-6f));
      CPPUNIT_ASSERT(!Equivalent(v4fa, v4fb, 4, 0.001f));
      CPPUNIT_ASSERT(Equivalent(v4fa, v4fb, 4, 0.1f));

      osg::Vec4d v4da(3.31, 3.32, 3.33, 3.34);
      osg::Vec4d v4db(3.32, 3.33, 3.34, 3.33);
      
      CPPUNIT_ASSERT(!Equivalent(v4da, v4db, 4, 1e-6));
      CPPUNIT_ASSERT(!Equivalent(v4da, v4db, 4, 0.001));
      CPPUNIT_ASSERT(Equivalent(v4da, v4db, 4, 0.1));
   }
}