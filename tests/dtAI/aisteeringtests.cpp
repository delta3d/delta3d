/*
* Delta3D Open Source Game and Simulation Engine
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
#include <dtUtil/templateutility.h>
#include <dtAI/sensor.h>
#include <dtAI/steeringutility.h>
#include <dtAI/steeringbehavior.h>
#include <dtCore/refptr.h>

namespace dtAI
{

   template <typename T>
   struct ErrorHandler: public osg::Referenced
   {
      ErrorHandler(){}
      ErrorHandler(typename dtUtil::TypeTraits<T>::param_type dataValue): mDataType(dataValue){}

      void operator()(typename dtUtil::TypeTraits<T>::param_type p)
      {
         mDataType = p;
      }

      typename dtUtil::TypeTraits<T>::param_type GetValue()
      {
         return mDataType;
      }

      typename dtUtil::TypeTraits<T>::const_param_type GetValue() const
      {
         return mDataType;
      }

      private:
         T mDataType;
   };

   struct EvaluateIncrement
   {
      void operator()(int& i)
      {
         ++i;
      }

   };

   struct SensorState
   {
      SensorState(): mState(false){} 
      void operator()(bool r)
      {
         mState = r;
      }
      bool mState;
   };


   struct CompareInt
   {
      bool operator()(bool& result, int first, int second)
      {
         //std::cerr << "CompareInt-  First:" << first << ", Second:" << second << std::endl << std::endl;
         return result = (first == second);
      }
   };

   struct GenericSensor
   {
      typedef Sensor<int, int, EvaluateIncrement, dtUtil::DoNothing<void, int>, CompareInt, SensorState*, bool> CompareSensor;
   };

   struct Steering 
   {
      typedef SteeringBehavior<KinematicGoal, Kinematic, SteeringOutput, GenericSensor::CompareSensor*, bool, ErrorHandler<bool>*> BehavoirBase;
   };

   /**
   *   To test the SteeringBehavoir API we create a simple derived class.
   */
   class TestSteeringBehavoir: public Steering::BehavoirBase
   {
   public:
      typedef Steering::BehavoirBase BaseClass;

      TestSteeringBehavoir(){}

      /*virtual*/ void Think(float testNumber, Steering::BehavoirBase::ConstKinematicGoalParam current_goal, Steering::BehavoirBase::ConstKinematicParam current_state, Steering::BehavoirBase::SteeringOutByRefParam result)
      {
         result.mLinearVelocity.set(testNumber, 0.0f, 0.0f);
      }

      /*virtual*/ bool GoalAchieved(Steering::BehavoirBase::ConstKinematicGoalParam current_goal, Steering::BehavoirBase::ConstKinematicParam current_state)
      {
         return current_goal.GetLinearVelocity()[1] == current_state.mLinearVelocity[0];
      }

   };


   /// Math unit tests for dtUtil
   class AISteeringTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AISteeringTests);
      CPPUNIT_TEST(TestKinematicGoal);
      CPPUNIT_TEST(TestSteeringBehavoirBaseClass);
      CPPUNIT_TEST(TestSteeringBehavoirErrorHandling);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestKinematicGoal();
      void TestSteeringBehavoirBaseClass();
      void TestSteeringBehavoirErrorHandling();

   private:

      SensorState mState;
      dtCore::RefPtr<GenericSensor::CompareSensor> mSensor;
      dtCore::RefPtr<TestSteeringBehavoir> mSteeringBehavoir;
      dtCore::RefPtr<ErrorHandler<bool> > mErrorHandler;
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(AISteeringTests);

   void AISteeringTests::setUp()
   {
      mSensor = new GenericSensor::CompareSensor(0, 10, EvaluateIncrement(), dtUtil::DoNothing<void, int>(), CompareInt(), &mState);
      mSteeringBehavoir = new TestSteeringBehavoir();
      
      mErrorHandler = new ErrorHandler<bool>(false);
      mSteeringBehavoir->AddErrorHandler(mSensor.get(), mErrorHandler.get());
   } 

   void AISteeringTests::tearDown()
   {
   }

   void AISteeringTests::TestSteeringBehavoirBaseClass()
   {

      KinematicGoal kg;
      Kinematic k;
      SteeringOutput steer;

      const float TestNumber = 1.0f;
      kg.SetLinearVelocity(osg::Vec3(TestNumber, 0.0f, 0.0f));
      
      mSteeringBehavoir->Think(TestNumber, kg, k, steer); 
      
      CPPUNIT_ASSERT(steer.mLinearVelocity[0] == TestNumber);
      CPPUNIT_ASSERT(mSteeringBehavoir->GoalAchieved(kg, k));
   }

   void AISteeringTests::TestKinematicGoal()
   {  
      KinematicGoal kg;

      CPPUNIT_ASSERT(!kg.HasPosition()); 
      CPPUNIT_ASSERT(!kg.HasRotation());
      CPPUNIT_ASSERT(!kg.HasLinearVelocity());
      CPPUNIT_ASSERT(!kg.HasAngularVelocity());

      kg.SetPosition(osg::Vec3(1.0, 1.0, 1.0));
      kg.SetRotation(osg::Vec3(1.0, 1.0, 1.0));
      kg.SetLinearVelocity(osg::Vec3(1.0, 1.0, 1.0));
      kg.SetAngularVelocity(osg::Vec3(1.0, 1.0, 1.0));

      CPPUNIT_ASSERT(kg.HasPosition());
      CPPUNIT_ASSERT(kg.HasRotation());
      CPPUNIT_ASSERT(kg.HasLinearVelocity());
      CPPUNIT_ASSERT(kg.HasAngularVelocity());
   }

   void AISteeringTests::TestSteeringBehavoirErrorHandling()
   {
      mErrorHandler->operator()(false);

      for(int i = 0; i < 9; ++i)
      {
         mSteeringBehavoir->InvokeErrorHandling();
         CPPUNIT_ASSERT_MESSAGE("Invoking Error Handling loop, no errors should be triggered until the sensor is triggered",!mErrorHandler->GetValue());
      }

      mSteeringBehavoir->InvokeErrorHandling();
      CPPUNIT_ASSERT_MESSAGE("Error should have been triggered by sensors 10th iteration", mErrorHandler->GetValue());

      //remove the error handler and assert that the sensor does not trigger an error
      mErrorHandler->operator()(false);
      CPPUNIT_ASSERT_MESSAGE("Resetting error handler to test remove handler", !mErrorHandler->GetValue()); 
      mSteeringBehavoir->RemoveErrorHandler(mSensor.get(), mErrorHandler.get());

      mSteeringBehavoir->InvokeErrorHandling();
      CPPUNIT_ASSERT_MESSAGE("Invoking error handling should not trigger an error if the handler was succesfully removed", !mErrorHandler->GetValue()); 

   }
}
