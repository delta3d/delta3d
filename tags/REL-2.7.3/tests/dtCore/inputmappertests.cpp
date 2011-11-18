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
* @author John K. Grant
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/inputmapper.h>

#include <dtCore/logicalinputdevice.h>
#include <dtCore/keyboard.h>

namespace dtTest
{
   /// unit tests for dtCore::Axis
   class InputMapperTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( InputMapperTests );
      CPPUNIT_TEST( TestObservers );
      CPPUNIT_TEST( AddFeaturesToLogicalInputDevice );
      CPPUNIT_TEST( InvalidButtonGet );

      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         ///\todo callback triggering and data flow to observers
         void TestObservers();

         void AddFeaturesToLogicalInputDevice();

         void InvalidButtonGet();

      private:
   };

}


using namespace dtTest;
CPPUNIT_TEST_SUITE_REGISTRATION(InputMapperTests);

void InputMapperTests::setUp()
{
}

void InputMapperTests::tearDown()
{
}

void InputMapperTests::TestObservers()
{
}


void InputMapperTests::AddFeaturesToLogicalInputDevice()
{
   dtCore::RefPtr<dtCore::LogicalInputDevice> dev = new dtCore::LogicalInputDevice();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("should have zero features", 0, dev->GetFeatureCount());

   dtCore::RefPtr<dtCore::Keyboard> keyboard = new dtCore::Keyboard();

   dtCore::LogicalButton *butt1 = dev->AddButton(
                                                "action 1", 
                                                keyboard->GetButton('1'),
                                                0xAAAA
                                                );

   CPPUNIT_ASSERT_MESSAGE("1st LogicalButton wasn't added to the LogicalInputDevice correctly", butt1 != NULL);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("should have one feature", 1, dev->GetFeatureCount());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("should have one button", 1, dev->GetButtonCount());

   dtCore::LogicalButton *butt2 = dev->AddButton(
                                                "action 2", 
                                                keyboard->GetButton('2'),
                                                0xAAAB
                                                );

   CPPUNIT_ASSERT_MESSAGE("2nd LogicalButton wasn't added to the LogicalInputDevice correctly", butt2 != NULL);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("should have two features", 2, dev->GetFeatureCount());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("should have two buttons", 2, dev->GetButtonCount());
}


void InputMapperTests::InvalidButtonGet()
{
   dtCore::RefPtr<dtCore::InputDevice> dev = new dtCore::InputDevice();
   CPPUNIT_ASSERT_MESSAGE("Should be NULL", NULL == dev->GetButton(123) );
}
