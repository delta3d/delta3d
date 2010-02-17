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
 * Roy Newton
 */

#include <prefix/dtgameprefix-src.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <cppunit/extensions/HelperMacros.h>

/**
 * @class LogTests
 * @brief Unit tests for the log module
 */
class LogTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(LogTests);
      CPPUNIT_TEST(TestLogMessage1);
      CPPUNIT_TEST(TestLogMessage2);
      CPPUNIT_TEST(TestLogMessage3);
      CPPUNIT_TEST(TestIsLevelEnabled);
      CPPUNIT_TEST(TestLogFilename);
      CPPUNIT_TEST(TestOutputStream);
   CPPUNIT_TEST_SUITE_END();

   public:
      static const unsigned int MAX_LENGTH = 512;

      void setUp();
      void tearDown();

      /**
       * Tests the string, int, string version of LogMessage
       */
      void TestLogMessage1();

      /**
       * Tests the type, string, int, char*, ... version of LogMessage
       */
      void TestLogMessage2();

      /**
       * Tests the type, string, char*, ... version of LogMessage
       */
      void TestLogMessage3();
      void TestIsLevelEnabled();

      void TestLogFilename();

      void TestOutputStream();

   private:
      std::string mMsgStr;
      std::string mSource;
      dtUtil::Log* mLogger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(LogTests);

///////////////////////////////////////////////////////////////////////////////
void LogTests::setUp()
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
void LogTests::tearDown()
{
   //turn of logging when done.
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void LogTests::TestLogMessage1()
{
   try
   {
      dtUtil::Log::LogMessageType logType;
      for (int type=0; type <=4; type++)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         mMsgStr = "s";
         mSource = "f";

         //test with empty strings
         mLogger->LogMessage("", 1, "");
         mLogger->LogMessage("", 1, "", logType);

         //test with one character
         mLogger->LogMessage(mSource, 1, mMsgStr);
         mLogger->LogMessage(mSource, 1, mMsgStr, logType);

         //test with max characters
         //for (unsigned i=0; i<=MAX_LENGTH-1; i++)
         {
            mMsgStr += "stuff";
            mSource += "foo";
         }
         mLogger->LogMessage(mSource, 1, mMsgStr);
         mLogger->LogMessage(mSource, 1, mMsgStr, logType);
      }
      mLogger->LogHorizRule();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.what());
   }

}

///////////////////////////////////////////////////////////////////////////////
void LogTests::TestLogMessage2()
{
   try
   {
      dtUtil::Log::LogMessageType logType;
      for (int type=0; type <=4; type++)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         mMsgStr = "g";
         mSource = "h";

         //test with empty strings
         mLogger->LogMessage(logType, "", 1, "");
         mLogger->LogMessage(logType, "", 1, "a %d %s %f %lf", 1, "c", 1.0, 1.1);

         //test with one character
         mLogger->LogMessage(logType, mSource, 1, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, 1, "a %d %s %f %lf", 1, "c", 1.0, 1.1);

         //test with max characters
         //for (unsigned i=0; i<=MAX_LENGTH-1; i++)
         {
            mMsgStr += "grand";
            mSource += "horse";
         }
         mLogger->LogMessage(logType, mSource, 1, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, 1, "a %d %s %f %lf", 1, "c", 1.0, 1.1);
      }
      mLogger->LogHorizRule();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.what());
   }
}

///////////////////////////////////////////////////////////////////////////////
void LogTests::TestLogMessage3()
{
   try
   {
      dtUtil::Log::LogMessageType logType;
      for (int type = 0; type <= 4; ++type)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);

         mMsgStr = "i";
         mSource = "j";

         //test with empty strings
         mLogger->LogMessage(logType, "", "");
         mLogger->LogMessage(logType, "", "a %d %s %f %lf", 1, "c", 1.0, 1.1);

         //test with one character
         mLogger->LogMessage(logType, mSource, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, "a %d %s %f %lf", 1, "c", 1.0, 1.1);

         //for (unsigned i=0; i<=MAX_LENGTH-1; i++)
         {
            mMsgStr += "igloo";
            mSource += "jack-o-lantern";
         }
         mLogger->LogMessage(logType, mSource, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, "a %d %s %f %lf", 1, "c", 1.0, 1.1);
      }
      mLogger->LogHorizRule();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.what());
   }
}

///////////////////////////////////////////////////////////////////////////////
void LogTests::TestIsLevelEnabled()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, "TestIsLevelEnabled", "Running TestIsLevelEnabled");
      dtUtil::Log::LogMessageType logType;
      for (int type = 0; type <= 4; ++type)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         CPPUNIT_ASSERT(mLogger->IsLevelEnabled(logType));
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Error: ") + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void LogTests::TestLogFilename()
{
   const std::string newFileName("logtest.html");
   //make sure the file is deleted first
   if (dtUtil::FileUtils::GetInstance().FileExists(newFileName))
      dtUtil::FileUtils::GetInstance().FileDelete(newFileName);

   dtUtil::LogFile::SetFileName(newFileName);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Filename should be: " + newFileName +
      " but returned: " + dtUtil::LogFile::GetFileName(),
      newFileName, dtUtil::LogFile::GetFileName() );

   LOG_ALWAYS("Filename test");

   bool exists = dtUtil::FileUtils::GetInstance().FileExists(newFileName);

   CPPUNIT_ASSERT(exists);
}

//////////////////////////////////////////////////////////////////////////
void LogTests::TestOutputStream()
{
   unsigned int option = dtUtil::Log::NO_OUTPUT;

   dtUtil::Log::GetInstance().SetOutputStreamBit( option );
   unsigned int newBit = dtUtil::Log::GetInstance().GetOutputStreamBit();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned bit doesn't match set bit",
                                option, newBit);

   option = dtUtil::Log::TO_CONSOLE | dtUtil::Log::TO_FILE;
   dtUtil::Log::GetInstance().SetOutputStreamBit( option );
   newBit = dtUtil::Log::GetInstance().GetOutputStreamBit();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned bit doesn't match set bit",
                                 option, newBit);
}
