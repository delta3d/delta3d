/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
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

#include <dtUtil/templateutility.h>

#include <dtDAL/propertymacros.h>
#include <dtDAL/propertycontainer.h>

#include <dtCore/refptr.h>

#include <dtAI/sensor.h>
#include <dtAI/steeringutility.h>
#include <dtAI/steeringbehavior.h>
#include <dtAI/steeringpipeline.h>
#include <dtAI/controllable.h>

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


   struct TestState
   {
      TestState(){}
      ~TestState(){}

      DECLARE_PROPERTY_INLINE(osg::Vec3, Pos);
      DECLARE_PROPERTY_INLINE(osg::Vec3, Forward);
      DECLARE_PROPERTY_INLINE(osg::Vec3, Up);

      DECLARE_PROPERTY_INLINE(osg::Vec3, Vel);
      DECLARE_PROPERTY_INLINE(osg::Vec3, Accel);

      DECLARE_PROPERTY_INLINE(float, AngularVel);
      DECLARE_PROPERTY_INLINE(float, AngularAccel);

      DECLARE_PROPERTY_INLINE(float, Pitch);
      DECLARE_PROPERTY_INLINE(float, Roll);

      void RegisterProperties(dtDAL::PropertyContainer& pc, const std::string& group)
      {
      }
   };

   struct  TestGoalState: public TestState
   {
      TestGoalState(){}
      ~TestGoalState(){}

      DECLARE_PROPERTY_INLINE(float, DragCoef);
      DECLARE_PROPERTY_INLINE(float, AngularDragCoef);

      DECLARE_PROPERTY_INLINE(float, MaxVel);
      DECLARE_PROPERTY_INLINE(float, MaxAccel);

      DECLARE_PROPERTY_INLINE(float, MaxAngularVel);
      DECLARE_PROPERTY_INLINE(float, MaxAngularAccel);

      DECLARE_PROPERTY_INLINE(float, MaxPitch);
      DECLARE_PROPERTY_INLINE(float, MaxRoll);

      DECLARE_PROPERTY_INLINE(float, MinElevation);
      DECLARE_PROPERTY_INLINE(float, MaxElevation);

      void RegisterProperties(dtDAL::PropertyContainer& pc, const std::string& group)
      {
      }
   };

   struct TestControls
   {
      TestControls(){}
      ~TestControls(){}

      //these are the control inputs
      //all are floats from 1 to -1 
      //which represents percentage of maximum
      DECLARE_PROPERTY_INLINE(float, Thrust);
      DECLARE_PROPERTY_INLINE(float, Lift);
      DECLARE_PROPERTY_INLINE(float, Yaw);

      void RegisterProperties(dtDAL::PropertyContainer& pc, const std::string& group)
      {
      }
   };

   class TestTargeter: public dtAI::Targeter<TestState, TestGoalState>
   {
   public:
      typedef Targeter<TestState, TestGoalState> BaseClass;

      TestTargeter(){}
      virtual ~TestTargeter(){}

      /*virtual*/ bool GetGoal(const BaseClass::StateType& current_state, BaseClass::GoalStateType& result) const
      {
         result.SetPos(osg::Vec3(250.0f, 50.0f, 20.0f));
         return true;
      }

   private:
   };

   class TestDecomposer: public dtAI::Decomposer<TestState, TestGoalState>
   {
   public:
      typedef Decomposer<TestState, TestGoalState> BaseClass;

      TestDecomposer(){}
      ~TestDecomposer(){}

      /*virtual*/ void Decompose(const TestState& current_state, TestGoalState& result) const
      {
      }
   };

   class TestConstraint: public dtAI::Constraint<TestState, TestGoalState>
   {
   public:
      typedef Constraint<TestState, TestGoalState> BaseClass;

      TestConstraint(){}
      ~TestConstraint(){}

      /*virtual*/ bool WillViolate(const BaseClass::PathType& pathToFollow) const
      {
         return false;
      }
      /*virtual*/ void Suggest(const BaseClass::PathType& pathToFollow, const TestState& current_state, TestGoalState& result) const
      {
      }
   };


   class TestControllable: public dtAI::Controllable<TestState, TestGoalState, TestControls>
   {
   public:
      typedef Controllable<TestState, TestGoalState, TestControls> BaseClass;

      TestControllable(){}
      ~TestControllable(){}

      /*virtual*/ bool FindPath(const AIState& fromState, const AIGoal& goal, AIPath& resultingPath) const
      {
         resultingPath.push_back(goal);
         return true;
      }
      
      /*virtual*/ void OutputControl(const PathType& pathToFollow, const StateType& current_state, ControlType& result) const 
      {
         result = mDefaultControls;
      }
      
      /*virtual*/ void UpdateState(float dt, const AIControlState& steerData)
      {
         mCurrentState = mDefaultState;
      }

      /*virtual*/ void RegisterProperties(dtDAL::PropertyContainer& pc, const std::string& group)
      {
         BaseClass::RegisterProperties(pc, group);
      }

   };

   typedef SteeringPipeline<TestControllable> TestAISteeringModel;


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

      TestAISteeringModel mTestSteeringPipeline;
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
         CPPUNIT_ASSERT_MESSAGE("Invoking Error Handling loop, no errors should be triggered "
               "until the sensor is triggered",
               !mErrorHandler->GetValue());
      }

      mSteeringBehavoir->InvokeErrorHandling();
      CPPUNIT_ASSERT_MESSAGE("Error should have been triggered by sensors 10th iteration",
            mErrorHandler->GetValue());

      //remove the error handler and assert that the sensor does not trigger an error
      mErrorHandler->operator()(false);
      CPPUNIT_ASSERT_MESSAGE("Resetting error handler to test remove handler", !mErrorHandler->GetValue()); 
      mSteeringBehavoir->RemoveErrorHandler(mSensor.get(), mErrorHandler.get());

      mSteeringBehavoir->InvokeErrorHandling();
      CPPUNIT_ASSERT_MESSAGE("Invoking error handling should not trigger an error if the handler was succesfully removed", !mErrorHandler->GetValue()); 

   }
}
