/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
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
