/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Delta3D
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
