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
 * David Guthrie, Curtiss Murphy
 */

#include <prefix/unittestprefix.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestRunner.h>

#include <cppunit/TestListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>

#include <dtCore/timer.h>
#include <dtCore/scene.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/system.h>
#include <dtABC/application.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/threadpool.h>

#include "unittestapplication.h"

#include <osgViewer/GraphicsWindow>
#include <osg/Version>

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <ctime>

static std::ostringstream mSlowTests;



class EmbeddedWindowSystemWrapper: public osg::GraphicsContext::WindowingSystemInterface
{
   public:
      EmbeddedWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface)
         : mInterface(&oldInterface)
      {
      }

      virtual unsigned int getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier =
         osg::GraphicsContext::ScreenIdentifier())
      {
         return mInterface->getNumScreens(screenIdentifier);
      }

      virtual void getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings& resolution)
      {
         mInterface->getScreenSettings(si, resolution);
      }

      virtual void enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList& rl)
      {
         mInterface->enumerateScreenSettings(si, rl);
      }

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION < 3
      virtual void getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
         unsigned int& width, unsigned int& height)
      {
         mInterface->getScreenResolution(screenIdentifier, width, height);
      }

      virtual bool setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int width, unsigned int height)
      {
         return mInterface->setScreenResolution(screenIdentifier, width, height);
      }

      virtual bool setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               double refreshRate)
      {
         return mInterface->setScreenRefreshRate(screenIdentifier, refreshRate);
      }

#else
      virtual bool setScreenSettings(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier, const osg::GraphicsContext::ScreenSettings& resolution)
      {
         return mInterface->setScreenSettings(screenIdentifier, resolution);
      }

#endif



      virtual osg::GraphicsContext* createGraphicsContext(osg::GraphicsContext::Traits* traits)
      {
         return new osgViewer::GraphicsWindowEmbedded(traits);
      }

   protected:
      virtual ~EmbeddedWindowSystemWrapper() {};
   private:
      dtCore::RefPtr<osg::GraphicsContext::WindowingSystemInterface> mInterface;
};


static dtCore::RefPtr<dtABC::Application> GlobalApplication;

dtABC::Application& GetGlobalApplication()
{
  GlobalApplication->GetScene()->RemoveAllDrawables();
  GlobalApplication->GetScene()->UseSceneLight(true);
  return *GlobalApplication;
}

std::string GetExamplesDataDir() { return dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "examples/data"; }
std::string GetTestsDir() { return dtUtil::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR + "tests"; };

class TimingListener : public CppUnit::TestListener
{
  public:
     void startTest(CppUnit::Test* test)
     {
        mFailure = false;
        mTestClockStart = mTestClock.Tick();
     }

      void endTest(CppUnit::Test* test)
      {
         // handle timing - for checking slow tests
         std::ostringstream testResult;
         dtCore::Timer_t testClockStop = mTestClock.Tick();
         double timeDelta = mTestClock.DeltaSec(mTestClockStart, testClockStop);
         timeDelta = (std::floor(timeDelta * 10000.0)) / 10000.0; // force data truncation
         testResult << test->getName()  << ((!mFailure) ? ": OK " : ": FAILURE ") <<
            ": time [" << timeDelta << "]";
         if (timeDelta > 0.7)
         {
            testResult << " *** SLOW TEST ***";
            mSlowTests << testResult.str() << std::endl;
         }
         std::cout << testResult.str() << std::endl;
      }

      void addFailure(const CppUnit::TestFailure& failure)
      {
         mFailure = true;
      }

    private:
       dtCore::Timer mTestClock;
       dtCore::Timer_t mTestClockStart;
       bool mFailure;
};

#ifndef TEST_ROOT
#define TEST_ROOT ../
#endif

#define _GET_PATH(testpath) #testpath
#define GET_PATH(testpath) _GET_PATH(testpath)


int main(int argc, char* argv[])
{
   const std::string executable = argv[0];
   bool changeDir = true;
   std::string singleSuiteName;
   std::string singleTestName;

   std::string currArg;
   for (int arg = 1; arg < argc; ++arg)
   {
      currArg = argv[arg];
      if (currArg == "-nochdir")
      {
         changeDir = false;
      }
      else if (singleSuiteName.empty())
      {
         singleSuiteName = currArg;
      }
      else if (singleSuiteName.empty())
      {
         singleTestName = currArg;
      }
      else
      {
         std::cerr << "Ignoring argument: " << currArg << std::endl;
      }
   }
   
   std::string testRoot = GET_PATH(TEST_ROOT);
   LOG_ALWAYS("The test root is: " + testRoot);

   if (changeDir)
   {
      LOG_ALWAYS(std::string("Changing to directory \"") + testRoot + dtUtil::FileUtils::PATH_SEPARATOR + "tests\".");

      try
      {
         dtUtil::FileUtils::GetInstance().ChangeDirectory(testRoot + dtUtil::FileUtils::PATH_SEPARATOR + "tests");
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
      }
   }
   
   // Must check this env var instead of GetDeltaRootPath because the function uses the app bundle path on osx and 
   // that will return a non-empty path.
   if (dtUtil::GetEnvironment("DELTA_ROOT").empty())
   {
      if (!testRoot.empty())
      {
         LOG_ALWAYS("DELTA_ROOT is not set, setting to " + testRoot);
         dtUtil::SetEnvironment("DELTA_ROOT", testRoot);
      }
      else
      {
         LOG_ERROR("DELTA_ROOT is not set, and the test root was not compiled in.  Aborting...");
         return -1;
      }
   }
   
   // Must check this env var instead of GetDeltaRootPath because the function uses the app bundle path on osx and 
   // that will return a non-empty path.
   if (dtUtil::GetEnvironment("DELTA_DATA").empty())
   {
      if (!testRoot.empty())
      {
         LOG_ALWAYS("DELTA_DATA is not set, setting to " + testRoot + "/data");
         dtUtil::SetEnvironment("DELTA_DATA", testRoot + "/data");
      }
      else
      {
         LOG_ERROR("DELTA_DATA is not set, and the test root was not compiled in.  Aborting...");
         return -1;
      }
   }
   
   LOG_ALWAYS("DELTA_ROOT is set to " + dtUtil::GetDeltaRootPath());
   LOG_ALWAYS("DELTA_DATA is set to " + dtUtil::GetDeltaDataPathList());

   //Set delta data.
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());

   try
   {
	   GlobalApplication = new UnitTestApplication();
   }
   catch (const dtCore::InvalidContextException& e)
   {
      LOG_ERROR("Could not create the unit test Application: "+e.ToString());
      return 1; //error
   }

   GlobalApplication->Config();
   dtCore::System::GetInstance().SetUseFixedTimeStep(false);

   ///Reset the windowing system for osg to use an embedded one.
   osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

   if (winSys != NULL)
   {
      osg::GraphicsContext::setWindowingSystemInterface(new EmbeddedWindowSystemWrapper(*winSys));
   }

   CPPUNIT_NS::TestResultCollector collectedResults;

   try
   {
      dtCore::Timer testsClock;
      dtCore::Timer_t testsTimerStart = testsClock.Tick();

      CPPUNIT_NS::TestResult testResult;
      testResult.addListener(&collectedResults);
      TimingListener timelistener;
      testResult.addListener(&timelistener);

      //This will print out the name of the test right before it runs.  Helpful
      //to track down a test that is seg faulting.
      CPPUNIT_NS::BriefTestProgressListener brief;
      testResult.addListener(&brief);

      // setup the test runner - does all the work
      CPPUNIT_NS::TestRunner testRunner;
      CPPUNIT_NS::Test* fullTestSuite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

      // Check for a specific test suite - if passed in, we only run the one suite
      if (singleSuiteName != "")
      {
         LOG_ALWAYS(std::string("Single suite name detected in arguments. Attempting to load a single test named [") + singleSuiteName + std::string("]."));
         CPPUNIT_NS::Test* suiteTest = fullTestSuite->findTest(singleSuiteName);
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
            CPPUNIT_NS::Test* individualTest = suiteTest->findTest(singleTestName);
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

      //write out an xml report file
      #ifdef _DEBUG
         std::string filename = "unittestreportd.xml";
      #else
         std::string filename = "unittestreport.xml";
      #endif

      std::ofstream reportFile;
      reportFile.open(filename.c_str());
      CPPUNIT_NS::XmlOutputter xmlout(&collectedResults, reportFile);
      xmlout.write();
      reportFile.close();

      // print out slow tests and total time.
      dtCore::Timer_t testsTimerStop = testsClock.Tick();
      double timeDelta = testsClock.DeltaSec(testsTimerStart, testsTimerStop);
      timeDelta = (floor(timeDelta * 10000.0)) / 10000.0; // force data truncation
      if (!mSlowTests.str().empty())
      {
         std::cout << "\n\n <<< SLOW TEST RESULTS ::: START >>> " << std::endl <<
            mSlowTests.str() << " <<< SLOW TEST RESULTS ::: END ::: TotalTime[" <<
            timeDelta << "] >>> \n" << std::endl;
      }
      else
      {
         std::cout << "TotalTime[" << timeDelta << "]" << std::endl;
      }

      CPPUNIT_NS::CompilerOutputter compilerOutputter(&collectedResults,std::cout);
      compilerOutputter.write();
   }
   catch (const std::invalid_argument& ie)
   {
      std::cerr << " <<< Invalid argument occurred. Likely, the suite name or test name are invalid or not found. " <<
         " For tests, be sure to include the class name like [MyClass::TestStuff]. Or, see cppunit.sourceforge.net for more info.  Error: [" <<
         ie.what() << "]. >>> " << std::endl;
   }
   catch (const std::exception& e)
   {
      std::cerr << " <<< Exception occurred. Error: [" << e.what() << "]. >>> " << std::endl;
   }
   catch (...)
   {
      std::cerr << " <<< Exception occurred while running main.cpp for this unit test. No other info available >>> " << std::endl;
   }

   GlobalApplication = NULL;

   return collectedResults.wasSuccessful () ? 0 : 1;
}

