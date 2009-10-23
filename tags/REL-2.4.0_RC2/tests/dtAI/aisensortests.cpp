/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
* Bradley Anderegg
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtAI/sensor.h>
#include <dtUtil/templateutility.h>
#include <dtCore/refptr.h>

namespace dtAI
{
   /**
    *	The structs below are used as the templated types to the dtAI::Sensor.
    * They act as first class functions in their ability to maintain state
    * while interfacing through the operator().
    */

   struct EvaluateIncrement
   {
      void operator()(int& i)
      {
         ++i;
      }

   };

   struct CompareInt
   {
      bool operator()(bool& result, int first, int second)
      {
         return result = (first == second);
      }
   };

   struct SensorStateModel
   {
      SensorStateModel(): mState(false){} 
      void operator()(bool r)
      {
         mState = r;
      }
      bool mState;
   };

   /// Math unit tests for dtUtil
   class AISensorTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AISensorTests);
      CPPUNIT_TEST(TestSensor);
      CPPUNIT_TEST_SUITE_END();
   
   public:
      typedef Sensor<int, int, EvaluateIncrement, dtUtil::DoNothing<void, int>, CompareInt, SensorStateModel*, bool> TestSensorType;

   public:
      void setUp();
      void tearDown();

      void TestSensor();

   private:
      SensorStateModel mState;
      dtCore::RefPtr<SensorBase<bool> > mSensor;
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(AISensorTests);

   void AISensorTests::setUp()
   {
      //Sensor<int, int, EvaluateIncrement, EvaluateIncrement, CompareInt, SensorStateModel, bool>;    
      mSensor = new TestSensorType(0, 10, EvaluateIncrement(), dtUtil::DoNothing<void, int>(), CompareInt(), &mState);
   }

   void AISensorTests::tearDown()
   {
   }

   
   void AISensorTests::TestSensor()
   {  
      for(int i = 0; i < 9; ++i)
      {
         CPPUNIT_ASSERT(!mSensor->Evaluate());
      }

      CPPUNIT_ASSERT(mSensor->Evaluate() && mState.mState);
   }
}
