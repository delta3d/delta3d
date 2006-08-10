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
 * @author David Guthrie
 */

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestRunner.h>

#include <cppunit/TestListener.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <time.h>    // for clock()
#include <dtCore/timer.h>

#include <sstream>

static std::ostringstream mSlowTests;

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
         timeDelta = ((int)(timeDelta * 10000)) / 10000.0; // force data truncation
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
 

int main (int argc, char* argv[])
{
   dtCore::Timer testsClock;
   dtCore::Timer_t testsTimerStart = testsClock.Tick(); 

   CPPUNIT_NS::TestResult testResult;

   CPPUNIT_NS::TestResultCollector collectedResults;
   testResult.addListener(&collectedResults);

   TimingListener timelistener;
   testResult.addListener(&timelistener);

   CPPUNIT_NS::TestRunner testRunner;
   testRunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
   testRunner.run(testResult);

   CPPUNIT_NS::CompilerOutputter compilerOutputter(&collectedResults,std::cerr);
   compilerOutputter.write();

   // print out slow tests and total time.
   dtCore::Timer_t testsTimerStop = testsClock.Tick();
   double timeDelta = testsClock.DeltaSec(testsTimerStart, testsTimerStop);
   timeDelta = ((int)(timeDelta * 10000)) / 10000.0; // force data truncation
   if (mSlowTests.str() != "")
   {
      std::cerr << " <<< SLOW TEST RESULTS ::: START >>> " << std::endl << 
         mSlowTests.str() << " <<< SLOW TEST RESULTS ::: END ::: TotalTime[" << 
         timeDelta << "] >>> " << std::endl;
   }
   else 
      std::cerr << " <<< SLOW TEST RESULTS ::: ALL TESTS RAN FAST!!! WOOT! ::: TotalTime[" << timeDelta << "] >>> " << std::endl;

   return collectedResults.wasSuccessful () ? 0 : 1;
}

