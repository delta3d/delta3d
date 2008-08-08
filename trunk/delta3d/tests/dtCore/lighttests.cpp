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
* @author Erik Johnson 06/2/2006
*/
#include <prefix/dtgameprefix-src.h>
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
