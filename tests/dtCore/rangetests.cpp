/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

/*#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/range.h>

using namespace dtCore;
 


class RangeTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(RangeTests);   
      CPPUNIT_TEST(TestConstructors);
      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestClamp);
   CPPUNIT_TEST_SUITE_END();
   
   public:
      void setUp();
      void tearDown();  

      void TestConstructors();
      void TestProperties();
      void TestClamp();
};

CPPUNIT_TEST_SUITE_REGISTRATION(RangeTests);


/////////////////////////////////////////////////////////////////////////
void RangeTests::setUp()
{
   // TODO: 
}

//////////////////////////////////////////////////////////////////////////
void RangeTests::tearDown()
{
   // TODO:
}

//////////////////////////////////////////////////////////////////////////
void RangeTests::TestConstructors()
{
   const float ZERO = Range::ZERO;

   Range r1;
   CPPUNIT_ASSERT(r1.GetMin() == ZERO);
   CPPUNIT_ASSERT(r1.GetMax() == ZERO);
   CPPUNIT_ASSERT(r1.GetValue() == ZERO);
   
   float testMax = 5.5f;
   Range r2(testMax);
   CPPUNIT_ASSERT(r2.GetMin() == ZERO);
   CPPUNIT_ASSERT(r2.GetMax() == testMax);
   CPPUNIT_ASSERT(r2.GetValue() == ZERO);

   float testMin = -3.5f;
   Range r3(testMin, testMax);
   CPPUNIT_ASSERT(r3.GetMin() == testMin);
   CPPUNIT_ASSERT(r3.GetMax() == testMax);
   CPPUNIT_ASSERT(r3.GetValue() == testMin);

   float testVal = 2.5f;
   Range r4(testMin, testMax, testVal);
   CPPUNIT_ASSERT(r4.GetMin() == testMin);
   CPPUNIT_ASSERT(r4.GetMax() == testMax);
   CPPUNIT_ASSERT(r4.GetValue() == testVal);

   Range r5(r4);
   CPPUNIT_ASSERT(r5.GetMin() == r4.GetMin());
   CPPUNIT_ASSERT(r5.GetMax() == r4.GetMax());
   CPPUNIT_ASSERT(r5.GetValue() == r4.GetValue());

   Range r6(testMin, testMax, testMax + 10.0f);
   CPPUNIT_ASSERT(r6.GetMin() == testMin);
   CPPUNIT_ASSERT(r6.GetMax() == testMax);
   CPPUNIT_ASSERT(r6.GetValue() == testMax);
}

//////////////////////////////////////////////////////////////////////////
void RangeTests::TestProperties()
{
   const float ZERO = Range::ZERO;

   Range r;
   CPPUNIT_ASSERT(r.GetMin() == ZERO);
   CPPUNIT_ASSERT(r.GetMax() == ZERO);
   CPPUNIT_ASSERT(r.GetValue() == ZERO);
   CPPUNIT_ASSERT(r.GetSpan() == ZERO);
   CPPUNIT_ASSERT(r.GetRatio() == ZERO);

   r.SetMin(-3.0f);
   r.SetMax(6.0f);
   r.SetValue(4.5f);

   CPPUNIT_ASSERT(r.GetMin() == -3.0f);
   CPPUNIT_ASSERT(r.GetMax() == 6.0f);
   CPPUNIT_ASSERT(r.GetValue() == 4.5f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, r.GetSpan(), 0.001f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5f/9.0f, r.GetRatio(), 0.001f);
}

//////////////////////////////////////////////////////////////////////////
void RangeTests::TestClamp()
{
   Range r(-2.0f, 3.0f);

   CPPUNIT_ASSERT(r.GetMin() == -2.0f);
   CPPUNIT_ASSERT(r.GetMax() == 3.0f);
   CPPUNIT_ASSERT(r.GetValue() == -2.0f);
   CPPUNIT_ASSERT(r.GetSpan() == 5.0f);
   CPPUNIT_ASSERT(r.GetRatio() == 0.0f);

   // Test non-member values.
   CPPUNIT_ASSERT(r.ClampValue(-10.0f) == -2.0f);
   CPPUNIT_ASSERT(r.ClampValue(10.0f) == 3.0f);
   CPPUNIT_ASSERT(r.GetRatio(-10.0f) == 0.0f);
   CPPUNIT_ASSERT(r.GetRatio(10.0f) == 1.0f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f/5.0f, r.GetRatio(1.0f), 0.001f);

   // Test member value.
   r.SetValue(10.0f);
   CPPUNIT_ASSERT(r.GetValue() == 3.0f);
   CPPUNIT_ASSERT(r.GetRatio() == 1.0f);

   r.SetValue(-10.0f);
   CPPUNIT_ASSERT(r.GetValue() == -2.0f);
   CPPUNIT_ASSERT(r.GetRatio() == 0.0f);

   r.SetValue(1.0f);
   CPPUNIT_ASSERT(r.GetValue() == 1.0f);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f/5.0f, r.GetRatio(), 0.001f);

   // Test clamping after adjusting the range limits.
   // --- Test shifting the range above the current value.
   r.SetMin(2.0f);
   r.SetMax(4.0f);
   CPPUNIT_ASSERT(r.GetMin() == 2.0f);
   CPPUNIT_ASSERT(r.GetMax() == 4.0f);
   CPPUNIT_ASSERT(r.GetValue() == 2.0f);
   CPPUNIT_ASSERT(r.GetRatio() == 0.0f);

   // --- Test shifting range below the current value.
   r.SetMin(-5.0f);
   r.SetMax(-2.0f);
   CPPUNIT_ASSERT(r.GetMin() == -5.0f);
   CPPUNIT_ASSERT(r.GetMax() == -2.0f);
   CPPUNIT_ASSERT(r.GetValue() == -2.0f);
   CPPUNIT_ASSERT(r.GetRatio() == 1.0f);
}*/
