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
#include <dtAI/steeringutility.h>
#include <dtAI/steeringbehavior.h>
#include <dtCore/refptr.h>

namespace dtAI
{



   /**
    *   To test the SteeringBehavoir API we create a simple derived class.
    */
   class TestSteeringBehavoir: public SteeringBehavior<KinematicGoal, Kinematic, SteeringOutput>
   {
   public:
      typedef SteeringBehavior<KinematicGoal, Kinematic, SteeringOutput> BaseClass;


      /*virtual*/ void Think(float testNumber, BaseClass::ConstKinematicGoalParam current_goal, BaseClass::ConstKinematicParam current_state, BaseClass::SteeringOutByRefParam result)
      {
         result.mLinearVelocity.set(testNumber, 0.0f, 0.0f);
      }

      /*virtual*/ bool GoalAchieved(BaseClass::ConstKinematicGoalParam current_goal, BaseClass::ConstKinematicParam current_state)
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
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestKinematicGoal();
      void TestSteeringBehavoirBaseClass();

   private:
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(AISteeringTests);

   void AISteeringTests::setUp()
   {
   }

   void AISteeringTests::tearDown()
   {
   }

   void AISteeringTests::TestSteeringBehavoirBaseClass()
   {

      TestSteeringBehavoir tb;
      KinematicGoal kg;
      Kinematic k;
      SteeringOutput steer;

      const float TestNumber = 1.0f;
      kg.SetLinearVelocity(osg::Vec3(TestNumber, 0.0f, 0.0f));
      
      tb.Think(TestNumber, kg, k, steer);
      
      CPPUNIT_ASSERT(steer.mLinearVelocity[0] == TestNumber);
      CPPUNIT_ASSERT(tb.GoalAchieved(kg, k));
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
}
