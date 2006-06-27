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

#define MAX_LENGTH 2048

/**
 * @class LogTests
 * @brief Unit tests for the log module
 */
class LogTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE( LogTests );
   CPPUNIT_TEST( TestLogMessage1 );
   CPPUNIT_TEST( TestLogMessage2 );
   CPPUNIT_TEST( TestLogMessage3 );
   CPPUNIT_TEST( TestIsLevelEnabled );
   CPPUNIT_TEST( TestLogFilename );
   CPPUNIT_TEST( TestOutputStream );
   CPPUNIT_TEST_SUITE_END();

   public:
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
CPPUNIT_TEST_SUITE_REGISTRATION( LogTests );

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
   mMsgStr.empty();
   mSource.empty();
   //turn of logging when done.
   //this is actually bad, it should save the log level and reset it.
   mLogger->SetLogLevel(dtUtil::Log::LOG_ERROR);
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void LogTests::TestLogMessage1()
{	
   //void LogMessage(const std::string &source, int line, const std::string &msg,
   //                     LogMessageType msgType = LOG_INFO);
   try
   {
      dtUtil::Log::LogMessageType logType;
      for(int type=0; type <=4; type++)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         mMsgStr.empty();
         mSource.empty();
         mMsgStr = "s";
         mSource = "f";

         //test with empty strings
         mLogger->LogMessage("", 1, "");
         mLogger->LogMessage("", 1, "", logType);

         //test with one character
         mLogger->LogMessage(mSource, 1, mMsgStr);		
         mLogger->LogMessage(mSource, 1, mMsgStr, logType);

         //test with max characters
         mMsgStr.empty();
         mSource.empty();
         for(int i=0; i<=MAX_LENGTH-1; i++)
         {
            mMsgStr += "s";
            mSource += "f";
         }				
         mLogger->LogMessage(mSource, 1, mMsgStr);		
         mLogger->LogMessage(mSource, 1, mMsgStr, logType);
      }
      mLogger->LogHorizRule();
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
void LogTests::TestLogMessage2()
{	
   //void LogMessage(LogMessageType msgType, const std::string &source, int line,
   //                         const char *msg, ...);
   try
   {
      dtUtil::Log::LogMessageType logType;
      for(int type=0; type <=4; type++)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         mMsgStr.empty();
         mSource.empty();
         mMsgStr = "g";
         mSource = "h";
         
         //test with empty strings
         mLogger->LogMessage(logType, "", 1, "");
         mLogger->LogMessage(logType, "", 1, "a %d %s %f %lf", 1, "c", 1.0, 1.1);

         //test with one character
         mLogger->LogMessage(logType, mSource, 1, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, 1, "a %d %s %f %lf", 1, "c", 1.0, 1.1);
         
         //test with max characters
         mMsgStr.empty();
         mSource.empty();
         
         for(int i=0; i<=MAX_LENGTH-1; i++)
         {
            mMsgStr += "g";
            mSource += "h";         
         }         
         mLogger->LogMessage(logType, mSource, 1, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, 1, "a %d %s %f %lf", 1, "c", 1.0, 1.1);
      }
      mLogger->LogHorizRule();
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
void LogTests::TestLogMessage3()
{	
   //void LogMessage(LogMessageType msgType, const std::string &source,
   //                            const char *msg, ...);
   try
   {
      dtUtil::Log::LogMessageType logType;
      for(int type=0; type <=4; type++)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         mMsgStr.empty();
         mSource.empty();
         mMsgStr = "i";
         mSource = "j";
         
         //test with empty strings
         mLogger->LogMessage(logType, "", "");
         mLogger->LogMessage(logType, "", "a %d %s %f %lf", 1, "c", 1.0, 1.1);

         //test with one character
         mLogger->LogMessage(logType, mSource, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, "a %d %s %f %lf", 1, "c", 1.0, 1.1);
         
         //test with max characters
         mMsgStr.empty();
         mSource.empty();
         
         for(int i=0; i<=MAX_LENGTH-1; i++)
         {
            mMsgStr += "i";
            mSource += "j";         
         }         
         mLogger->LogMessage(logType, mSource, mMsgStr.c_str());
         mLogger->LogMessage(logType, mSource, "a %d %s %f %lf", 1, "c", 1.0, 1.1);
      }
      mLogger->LogHorizRule();
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
void LogTests::TestIsLevelEnabled()
{
   //bool IsLevelEnabled(LogMessageType msgType) const { return msgType >= mLevel; };
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, "TestIsLevelEnabled", "Running TestIsLevelEnabled");
      dtUtil::Log::LogMessageType logType;
      for(int type=0; type <=4; type++)
      {
         logType = (dtUtil::Log::LogMessageType)type;
         mLogger->SetLogLevel(logType);
         CPPUNIT_ASSERT(mLogger->IsLevelEnabled(logType));
      }
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

//////////////////////////////////////////////////////////////////////////
void LogTests::TestLogFilename()
{
   const std::string newFileName("logtest.html");
   dtUtil::LogFile::SetFileName(newFileName);

   LOG_ALWAYS("Filename test");

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Filename should be: " + newFileName +
      " but returned: " + dtUtil::LogFile::GetFileName(),
      newFileName, dtUtil::LogFile::GetFileName() );

   //hmm, not much of a test.  Does the file exist?
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
