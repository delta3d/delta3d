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
 * @class ExceptionTests
 * @brief Unit tests for the exception module
 */
class ExceptionTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE( ExceptionTests );
   CPPUNIT_TEST( TestThrow );
   CPPUNIT_TEST( TestLogAndPrintFunctions );
   CPPUNIT_TEST( TestEnum );
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests throwing and catching a dtUtil::Exception
       */
      void TestThrow();    

      /**
       * Tests all the LogException functions along with Print(), What(), File(), and Line()
       */
      void TestLogAndPrintFunctions();

      /**
       * Tests that the enumeration in the instance is what was given during construction
       */
      void TestEnum();
   
   private:
      dtUtil::Log* mLogger;      

   public:
      class TestEnumType : public dtUtil::Enumeration
      {
         DECLARE_ENUM(TestEnumType);
      public:
         static TestEnumType ENUM;         

      private:
         TestEnumType(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };
};

IMPLEMENT_ENUM(ExceptionTests::TestEnumType);
ExceptionTests::TestEnumType
ExceptionTests::TestEnumType::ENUM("ENUM");


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ExceptionTests );

///////////////////////////////////////////////////////////////////////////////
void ExceptionTests::setUp() 
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
void ExceptionTests::tearDown() 
{
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void ExceptionTests::TestThrow()
{	
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Throwing Exception.\n");
      throw dtUtil::Exception(TestEnumType::ENUM, "Test throw", "enumerationtests.cpp", __LINE__);
      
   }
   catch (const dtUtil::Exception& e)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Caught Exception!.\n");
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }

}

///////////////////////////////////////////////////////////////////////////////
void ExceptionTests::TestLogAndPrintFunctions()
{
   try
   {
      dtUtil::Exception testException(TestEnumType::ENUM, "test exception", "enumerationtests.cpp", __LINE__);
      testException.LogException(dtUtil::Log::LOG_INFO);
      testException.LogException(dtUtil::Log::LOG_INFO, "__+default+__");
      testException.LogException(dtUtil::Log::LOG_INFO, *mLogger);
      testException.Print();
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "What is %s", testException.What().c_str());
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "File is %s", testException.File().c_str());
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Line is %d", testException.Line());
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
void ExceptionTests::TestEnum()
{
   try
   {
      dtUtil::Exception testException(TestEnumType::ENUM, "test exception", "enumerationtests.cpp", __LINE__);
      CPPUNIT_ASSERT(TestEnumType::ENUM == testException.TypeEnum()); 
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
