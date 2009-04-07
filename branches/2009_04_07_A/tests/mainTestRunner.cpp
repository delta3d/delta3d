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
#include <prefix/dtgameprefix-src.h>
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
#include <dtCore/globals.h>
#include <dtCore/scene.h>
#include <dtABC/application.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include "unittestapplication.h"

#include <osg/GraphicsContext>
#include <osg/Version>

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <ctime>   

static std::ostringstream mSlowTests;



class EmbeddedWindowSystemWrapper: public osg::GraphicsContext::WindowingSystemInterface
{
   public:
      EmbeddedWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface):
         mInterface(&oldInterface)
      {
      }
      
      virtual unsigned int getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier = 
         osg::GraphicsContext::ScreenIdentifier())
      {
         return mInterface->getNumScreens(screenIdentifier);
      }

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 8

      virtual void getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings & resolution)
      {
         mInterface->getScreenSettings(si, resolution);
      }

      virtual void enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList & rl) 
      {
         mInterface->enumerateScreenSettings(si, rl);
      }
#endif

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
         if (timeDelta > 0.7)
         {
            testResult << " *** SLOW TEST ***";
            mSlowTests << testResult.str() << std::endl;
         }
         std::cout << testResult.str() << std::endl;
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

   GlobalApplication = new UnitTestApplication();
   GlobalApplication->Config();

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

      CPPUNIT_NS::CompilerOutputter compilerOutputter(&collectedResults,std::cout);
      compilerOutputter.write();

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
      if(!mSlowTests.str().empty())
      {
         std::cout << " <<< SLOW TEST RESULTS ::: START >>> " << std::endl << 
            mSlowTests.str() << " <<< SLOW TEST RESULTS ::: END ::: TotalTime[" << 
            timeDelta << "] >>> " << std::endl;
      }
      else 
         std::cout << " <<< SLOW TEST RESULTS ::: ALL TESTS RAN FAST!!! WOOT! ::: TotalTime[" << timeDelta << "] >>> " << std::endl;
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

   return collectedResults.wasSuccessful () ? 0 : 1;
}

