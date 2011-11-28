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
 * @class ExceptionTests
 * @brief Unit tests for the exception module
 */
class ExceptionTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ExceptionTests);
      CPPUNIT_TEST(TestThrow);
      CPPUNIT_TEST(TestLogAndPrintFunctions);
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

   private:
      dtUtil::Log* mLogger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ExceptionTests);

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
      throw dtUtil::Exception("Test throw", "enumerationtests.cpp", __LINE__);

   }
   catch (const dtUtil::Exception&)
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
      dtUtil::Exception testException("test exception", __FILE__, __LINE__);
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

