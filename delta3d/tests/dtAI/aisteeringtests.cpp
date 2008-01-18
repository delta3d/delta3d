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
#include <dtCore/refptr.h>

namespace dtAI
{
   /// Math unit tests for dtUtil
   class AISteeringTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AISteeringTests);
      CPPUNIT_TEST(TestKinematicGoal);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestKinematicGoal();

   private:
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(AISteeringTests);

   void AISteeringTests::setUp()
   {
   }

   void AISteeringTests::tearDown()
   {
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
