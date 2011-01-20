/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * @author Roy Newton
 */

#include <prefix/unittestprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <cppunit/extensions/HelperMacros.h>



/**
 * @class EnumerationTests
 * @brief Unit tests for the enumeration module
 */
class EnumerationTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(EnumerationTests);
   CPPUNIT_TEST(TestBools);
   CPPUNIT_TEST(TestIterator);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests all the boolean operators in the Enumeration class
       */
      void TestBools();

      /**
       * Tests iterator through an Enumeration
       */
      void TestIterator();

   private:
      dtUtil::Log* mLogger;

   public:
      class TestEnumType1 : public dtUtil::Enumeration
      {
         DECLARE_ENUM(TestEnumType1);
      public:
         static TestEnumType1 ENUM1;
         static TestEnumType1 ENUM2;

      private:
         TestEnumType1(const std::string& name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      class TestEnumType2 : public dtUtil::Enumeration
      {
         DECLARE_ENUM(TestEnumType2);
      public:
         static TestEnumType2 ENUM1;
         static TestEnumType2 ENUM2;

      private:
         TestEnumType2(const std::string& name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };


};

IMPLEMENT_ENUM(EnumerationTests::TestEnumType1)
EnumerationTests::TestEnumType1
EnumerationTests::TestEnumType1::ENUM1("ENUM1");
EnumerationTests::TestEnumType1
EnumerationTests::TestEnumType1::ENUM2("ENUM2");

IMPLEMENT_ENUM(EnumerationTests::TestEnumType2)
EnumerationTests::TestEnumType2
EnumerationTests::TestEnumType2::ENUM1("ENUM1");
EnumerationTests::TestEnumType2
EnumerationTests::TestEnumType2::ENUM2("ENUM2");

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(EnumerationTests);

///////////////////////////////////////////////////////////////////////////////
void EnumerationTests::setUp()
{
   try
   {
      mLogger = &dtUtil::Log::GetInstance();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
   CPPUNIT_ASSERT(mLogger != NULL);
}

///////////////////////////////////////////////////////////////////////////////
void EnumerationTests::tearDown()
{
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void EnumerationTests::TestBools()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Boolean Operators.\n");
      CPPUNIT_ASSERT(TestEnumType1::ENUM1 == "ENUM1");
      CPPUNIT_ASSERT(TestEnumType1::ENUM2 == "ENUM2");
      CPPUNIT_ASSERT(TestEnumType2::ENUM1 == "ENUM1");
      CPPUNIT_ASSERT(TestEnumType2::ENUM2 == "ENUM2");

      CPPUNIT_ASSERT(TestEnumType1::ENUM1 != "GARBAGE");
      CPPUNIT_ASSERT(TestEnumType1::ENUM2 != "GARBAGE");
      CPPUNIT_ASSERT(TestEnumType2::ENUM1 != "GARBAGE");
      CPPUNIT_ASSERT(TestEnumType2::ENUM2 != "GARBAGE");

      CPPUNIT_ASSERT(TestEnumType1::ENUM1 < "ENUM3");
      CPPUNIT_ASSERT(TestEnumType2::ENUM1 < "ENUM3");
      CPPUNIT_ASSERT(TestEnumType1::ENUM1 < "ENUM3");
      CPPUNIT_ASSERT(TestEnumType2::ENUM1 < "ENUM3");

      CPPUNIT_ASSERT(TestEnumType1::ENUM1 > "EN1UMERATION");
      CPPUNIT_ASSERT(TestEnumType1::ENUM2 > "EN1UMERATION");
      CPPUNIT_ASSERT(TestEnumType2::ENUM1 > "EN1UMERATION");
      CPPUNIT_ASSERT(TestEnumType2::ENUM2 > "EN1UMERATION");

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }

}

///////////////////////////////////////////////////////////////////////////////
void EnumerationTests::TestIterator()
{
   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Enumeration Iterator.\n");
   int count1 = 0;
   for (std::vector<TestEnumType1*>::const_iterator i = TestEnumType1::EnumerateType().begin();
            i != TestEnumType1::EnumerateType().end(); ++i)
   {
      TestEnumType1& TET = **i;
      if (TET == TestEnumType1::ENUM1)
      {
         ++count1;
      }
      else if (TET == TestEnumType1::ENUM2)
      {
         ++count1;
      }
   }
   CPPUNIT_ASSERT(count1 == 2);
}


