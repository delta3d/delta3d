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
 * @author John K. Grant
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/mathdefines.h>
#include <math.h>

namespace dtTest
{
   /// Math unit tests for dtUtil
   class MathTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( MathTests );
      CPPUNIT_TEST( TestNormalizer );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestNormalizer();

      private:
   };
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::MathTests );

using namespace dtTest;

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

   double minTest = dtUtil::Min<double>(xMin, xMax);
   CPPUNIT_ASSERT_EQUAL(xMin, minTest);   

   double maxTest = dtUtil::Max<double>(xMin, xMax);
   CPPUNIT_ASSERT_EQUAL(xMax, maxTest);   

   double randRange = dtUtil::RandRange(5, 10);
   CPPUNIT_ASSERT(randRange <= 10 && randRange >= 5);   

   float randFloat = dtUtil::RandFloat(float(xMin), float(xMax));
   CPPUNIT_ASSERT(randFloat <= xMax && randFloat >= xMin);   

   float randPercent = dtUtil::RandPercent();
   CPPUNIT_ASSERT(randPercent <= 1.0 && randPercent >= 0.0);

   double absTest = dtUtil::Abs<double>(x);
   CPPUNIT_ASSERT_EQUAL( 0.5, absTest);

   dtUtil::Clamp<double>(absTest, xMin, xMax);
   CPPUNIT_ASSERT_EQUAL( 0.5, absTest);

   double xLerp = dtUtil::Lerp<double>(xMin, xMax, 0.25);
   CPPUNIT_ASSERT_EQUAL( x, xLerp);

   double xN( dtUtil::CalculateNormal(x,xMin,xMax) );
   CPPUNIT_ASSERT_EQUAL( 0.25 , xN );

   double y( dtUtil::CalculateValueForRange(xN,yMin,yMax) );
   CPPUNIT_ASSERT_EQUAL( 0.0 , y );

   double my( dtUtil::MapRangeValue(x,xMin,xMax,yMin,yMax) );
   CPPUNIT_ASSERT_EQUAL( 0.0 , my );
}
