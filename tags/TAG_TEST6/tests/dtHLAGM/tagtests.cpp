/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
#include <dtHLAGM/distypes.h>

class TagTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(TagTests);
  
   CPPUNIT_TEST(TestGetSet);
   CPPUNIT_TEST(TestAssign);
   CPPUNIT_TEST(TestEncodeDecode);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();
      void TestGetSet();
      void TestAssign();
      void TestEncodeDecode();
   
   private:
      dtHLAGM::Tag mTag;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TagTests);

void TagTests::setUp()
{
}

void TagTests::tearDown()
{
}

void TagTests::TestGetSet()
{
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The tag value should default to 0.0.", 0.0, mTag.GetTag());
   double testVal = 637.288;
   mTag.SetTag(testVal);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The tag value should default to the test value.", testVal, mTag.GetTag());
}

void TagTests::TestAssign()
{
   double testVal = 637.288;
   mTag.SetTag(testVal);
   dtHLAGM::Tag tempTag;
   tempTag = mTag;
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The tag value should have been copied.", mTag.GetTag(), tempTag.GetTag());
}

void TagTests::TestEncodeDecode()
{
   char buffer[17];
   mTag.Encode(buffer);
   
   dtHLAGM::Tag tempTag;
   tempTag.Decode(buffer);
   
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The tag value should be the same when encoded and decoded.", mTag.GetTag(), tempTag.GetTag());
   CPPUNIT_ASSERT(tempTag.EncodedLength() == 17);
}
