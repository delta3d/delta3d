/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Roy Newton
 */

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <cppunit/extensions/HelperMacros.h>



/**
 * @class EnumerationTests
 * @brief Unit tests for the enumeration module
 */
class EnumerationTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE( EnumerationTests );
   CPPUNIT_TEST( TestBools );
   CPPUNIT_TEST( TestIterator );
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
         TestEnumType1(const std::string &name) : dtUtil::Enumeration(name)
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
         TestEnumType2(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

     
};

IMPLEMENT_ENUM(EnumerationTests::TestEnumType1);
EnumerationTests::TestEnumType1
EnumerationTests::TestEnumType1::ENUM1("ENUM1");
EnumerationTests::TestEnumType1
EnumerationTests::TestEnumType1::ENUM2("ENUM2");

IMPLEMENT_ENUM(EnumerationTests::TestEnumType2);
EnumerationTests::TestEnumType2
EnumerationTests::TestEnumType2::ENUM1("ENUM1");
EnumerationTests::TestEnumType2
EnumerationTests::TestEnumType2::ENUM2("ENUM2");

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EnumerationTests );

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
   for (std::vector<dtUtil::Enumeration*>::const_iterator i = TestEnumType1::Enumerate().begin();
            i != TestEnumType1::Enumerate().end(); ++i)
   {
      TestEnumType1& TET = *static_cast<TestEnumType1*>(*i);
      if(TET == TestEnumType1::ENUM1)
      {
         count1++;
      }
      else if(TET == TestEnumType1::ENUM2)
      {
         count1++;
      }           
   }
   CPPUNIT_ASSERT(count1 == 2);
}


