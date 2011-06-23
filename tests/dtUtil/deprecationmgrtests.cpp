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
#include <dtUtil/deprecationmgr.h>
#include <cppunit/extensions/HelperMacros.h>


/**
 * @class DeprecationMgrTests
 * @brief Unit tests for the deprecation manager
 */
class DeprecationMgrTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(DeprecationMgrTests);
   CPPUNIT_TEST(TestAddDeprecatedFunction);
   CPPUNIT_TEST(TestCallDeprecatedFunction);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests adding a deprecated function to the manager
       */
      void TestAddDeprecatedFunction();

      /**
       * Tests calling a deprecated function that has been added to the manager
       */
      void TestCallDeprecatedFunction();

   private:
      /**
       * Function to deprecate (no args)
       */
      void funcToDeprecate1();

      /**
       * Function to deprecate (args)
       */
      void funcToDeprecate2(int x, const char* y);

      /**
       * The replacement function for funcToDeprecate1()
       */
      void correctFunction1();

      /**
       * The replacement function for funcToDeprecate2(int x, char* y)
       */
      void correctFunction2(int x, std::string& y);

      dtUtil::Log* mLogger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DeprecationMgrTests);

///////////////////////////////////////////////////////////////////////////////
void DeprecationMgrTests::setUp()
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
void DeprecationMgrTests::tearDown()
{
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void DeprecationMgrTests::TestAddDeprecatedFunction()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Adding a deprecated function.\n");
      DEPRECATE("", "");
      DEPRECATE("void DeprecationMgrTests::functionToDeprecate1()", "DeprecationMgrTests::correctFunction1()");
      DEPRECATE("void DeprecationMgrTests::functionToDeprecate2(int x, char* y)", "DeprecationMgrTests::correctFunction2(int x, std::string& y)");
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
void DeprecationMgrTests::TestCallDeprecatedFunction()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Calling a deprecated function.\n");
      funcToDeprecate1();
      funcToDeprecate2(1, "test");
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

void DeprecationMgrTests::correctFunction1(){}
void DeprecationMgrTests::correctFunction2(int x, std::string& y){}
void DeprecationMgrTests::funcToDeprecate1(){}
void DeprecationMgrTests::funcToDeprecate2(int x, const char* y){}
