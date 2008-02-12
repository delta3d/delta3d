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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestRunner.h>

#include <cppunit/TestListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include <dtCore/timer.h>
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>
#include <dtAudio/audiomanager.h>
#include <dtABC/application.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <ctime>   

static std::ostringstream mSlowTests;

static dtCore::RefPtr<dtABC::Application> GlobalApplication;

dtABC::Application& GetGlobalApplication() { return *GlobalApplication; }

class TimingListener : public CppUnit::TestListener
{
  public:
     void startTest( CppUnit::Test *test )
     {
        mFailure = false;
        mTestClockStart = mTestClock.Tick();
     }
 
      void endTest( CppUnit::Test *test )
      {
         // handle timing - for checking slow tests
         std::ostringstream testResult;
         dtCore::Timer_t testClockStop = mTestClock.Tick();
         double timeDelta = mTestClock.DeltaSec(mTestClockStart, testClockStop);
         timeDelta = (floor(timeDelta * 10000.0)) / 10000.0; // force data truncation
         testResult << test->getName()  << ((!mFailure) ? ": OK " : ": FAILURE ") <<
            ": time [" << timeDelta << "]";
         if (timeDelta > 2.0)
         {
            testResult << " *** SLOW TEST ***";
            mSlowTests << testResult.str() << std::endl;
         }
         std::cerr << testResult.str() << std::endl;
      }

      void addFailure( const CppUnit::TestFailure &failure )
      {
         mFailure = true;
      }

    private:
       dtCore::Timer mTestClock;
       dtCore::Timer_t mTestClockStart; 
       bool mFailure;
};
 

int main(int argc, char* argv[])
{
   const std::string executable = argv[0];
   std::string singleSuiteName;
   std::string singleTestName;

   // Get the test name from args.
   if (argc > 1)
      singleSuiteName = argv[1];
   if (argc > 2)
      singleTestName = argv[2];

   // We need to change our directory based on the executable
   dtUtil::FileInfo info = dtUtil::FileUtils::GetInstance().GetFileInfo(executable);
   if(info.fileType == dtUtil::FILE_NOT_FOUND)
   {
      LOG_ERROR(std::string("Unable to change to the directory of application \"")
         + executable + "\": file not found.");
   }
   else
   {
      std::string path = info.path;
      LOG_ALWAYS(std::string("Changing to directory \"") + info.path + "\".");

      try 
      {
         if(!info.path.empty())
            dtUtil::FileUtils::GetInstance().ChangeDirectory(info.path);
      } 
      catch(const dtUtil::Exception &ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
      }
   }
   
   //Set delta data.
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());

   dtAudio::AudioManager::Instantiate();
   dtAudio::AudioManager::GetInstance().Config(AudioConfigData(32));

   GlobalApplication = new dtABC::Application("config.xml");
   GlobalApplication->GetWindow()->SetPosition(0, 0, 50, 50);
   GlobalApplication->Config();

   CPPUNIT_NS::TestResultCollector collectedResults;

   try
   {
      dtCore::Timer testsClock;
      dtCore::Timer_t testsTimerStart = testsClock.Tick(); 

      CPPUNIT_NS::TestResult testResult;   
      testResult.addListener(&collectedResults);
      TimingListener timelistener;
      testResult.addListener(&timelistener);

      // setup the test runner - does all the work
      CPPUNIT_NS::TestRunner testRunner;
      CPPUNIT_NS::Test *fullTestSuite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

      // Check for a specific test suite - if passed in, we only run the one suite
      if (singleSuiteName != "")
      {
         LOG_ALWAYS(std::string("Single suite name detected in arguments. Attempting to load a single test named [") + singleSuiteName + std::string("]."));
         CPPUNIT_NS::Test *suiteTest = fullTestSuite->findTest(singleSuiteName);
         if (suiteTest == NULL)
         {
            std::cerr << " *** FAILED to find test suite named [" << singleSuiteName << 
               "]. Please check suite name. The name should match what was used in the registration line, " <<
               "\'CPPUNIT_TEST_SUITE(MyTests)\' would be \'MyTests\'. Aborting test." <<  std::endl;
         }
         // Check for a specific test within the specific suite
         else if (singleTestName != "")
         {
            LOG_ALWAYS(std::string("Individual test name detected in arguments. Attempting to load test [") + singleTestName + std::string("]."));
            CPPUNIT_NS::Test *individualTest = suiteTest->findTest(singleTestName);
            if (individualTest == NULL)
            {
               std::cerr << " *** FAILED to individual test [" << singleTestName << 
                  "] inside suite [" << singleSuiteName << "]. Please check suite name. " <<
                  "The name should match what was used in the registration line, " <<
                  "\'CPPUNIT_TEST(TestFunction)\' would be \'TestFunction\'. Aborting test." <<  std::endl;
            }
            else 
            {
               LOG_ALWAYS(std::string("   *** Found test suite and single test[ ") + 
                  singleTestName + std::string("].  Starting run."));
               testRunner.addTest(individualTest);
            }
         }
         // We found the single suite, no individual test
         else
         {
            LOG_ALWAYS(std::string("Found single test suite. Starting run."));
            testRunner.addTest(suiteTest);
         }
      }
      else 
      {
         LOG_ALWAYS(std::string("No arguments detected.  Running all tests!  Pass the suite name as 1st arg to run a single suite. For single test, pass test name as 2nd arg."));
         testRunner.addTest(fullTestSuite);
      }

      // Go to it!!!  Run this puppy!
      testRunner.run(testResult);

      CPPUNIT_NS::CompilerOutputter compilerOutputter(&collectedResults,std::cerr);
      compilerOutputter.write();

      // print out slow tests and total time.
      dtCore::Timer_t testsTimerStop = testsClock.Tick();
      double timeDelta = testsClock.DeltaSec(testsTimerStart, testsTimerStop);
      timeDelta = (floor(timeDelta * 10000.0)) / 10000.0; // force data truncation
      if(!mSlowTests.str().empty())
      {
         std::cerr << " <<< SLOW TEST RESULTS ::: START >>> " << std::endl << 
            mSlowTests.str() << " <<< SLOW TEST RESULTS ::: END ::: TotalTime[" << 
            timeDelta << "] >>> " << std::endl;
      }
      else 
         std::cerr << " <<< SLOW TEST RESULTS ::: ALL TESTS RAN FAST!!! WOOT! ::: TotalTime[" << timeDelta << "] >>> " << std::endl;
   }
   catch (const std::invalid_argument &ie)
   {
      std::cerr << " <<< Invalid argument occurred. Likely, the suite name or test name are invalid or not found. " << 
         " For tests, be sure to include the class name like [MyClass::TestStuff]. Or, see cppunit.sourceforge.net for more info.  Error: [" <<
         ie.what() << "]. >>> " << std::endl;
   }
   catch (const std::exception &e)
   {
      std::cerr << " <<< Exception occurred. Error: [" << e.what() << "]. >>> " << std::endl;
   }
   catch (...)
   {
      std::cerr << " <<< Exception occurred while running main.cpp for this unit test. No other info available >>> " << std::endl;
   }

   GlobalApplication = NULL;

   dtAudio::AudioManager::Destroy();

   //dtAudio::AudioManager::Destroy();
   return collectedResults.wasSuccessful () ? 0 : 1;
}

