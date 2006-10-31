/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute
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
 * @author Erik Johnson 06/2/2006
 */

#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/infinitelight.h>
#include <dtCore/refptr.h>
#include <dtUtil/stringutils.h>
#include <osg/Math>

///used to test the Light functionality
class  LightTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(LightTests);
   CPPUNIT_TEST(TestInfiniteLight);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestInfiniteLight();

private:
   void CheckAzValues( float &az1, float &az2, float &eps );
   void CheckElValue( float &el1, float &el2, float &eps );
   void DoAzElTest(dtCore::InfiniteLight *inf, float az, float el );
};

CPPUNIT_TEST_SUITE_REGISTRATION(LightTests);

void LightTests::setUp()
{
}

void LightTests::tearDown()
{
}

void LightTests::CheckAzValues( float &az1, float &az2, float &eps )
{
   CPPUNIT_ASSERT_MESSAGE("Azimuth should be: " + dtUtil::ToString(az1) + 
      " but returned: " + dtUtil::ToString(az2),
      osg::equivalent(az1, az2, eps));
}

void LightTests::CheckElValue( float &el1, float &el2, float &eps )
{
   CPPUNIT_ASSERT_MESSAGE("Elevation should be: " + dtUtil::ToString(el1) + 
      " but returned: " + dtUtil::ToString(el2),
      osg::equivalent(el1, el2, eps));
}

void LightTests::DoAzElTest( dtCore::InfiniteLight *inf, float az, float el )
{
   float retAz = 0.f;
   float retEl = 0.f;
   float EPS = 0.0001f;

   inf->SetAzimuthElevation(az, el);
   inf->GetAzimuthElevation(retAz, retEl);

   CheckElValue(el, retEl, EPS);
   CheckAzValues(az, retAz, EPS);
}

void LightTests::TestInfiniteLight()
{
   dtCore::RefPtr<dtCore::InfiniteLight> inf = new dtCore::InfiniteLight(0);

   // test northern hemisphere (0<el<90)
   DoAzElTest(inf.get(), 30.1f,  30.1f);       // test NE quadrant
   DoAzElTest(inf.get(), 120.1f, 30.1f);       // test SE quadrant
   DoAzElTest(inf.get(), 210.1f, 30.1f);       // test SW quadrant
   DoAzElTest(inf.get(), 300.1f, 30.1f);       // test NW quadrant
   
   // test northern hemisphere (-90<el<0)
   DoAzElTest(inf.get(), 30.1f,  -30.1f);       // test NE quadrant
   DoAzElTest(inf.get(), 120.1f, -30.1f);       // test SE quadrant
   DoAzElTest(inf.get(), 210.1f, -30.1f);       // test SW quadrant
   DoAzElTest(inf.get(), 300.1f, -30.1f);       // test NW quadrant
}
