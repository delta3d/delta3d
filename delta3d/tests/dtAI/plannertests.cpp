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
* @author Bradley Anderegg
*/

#include <cppunit/extensions/HelperMacros.h>

#include "testplannerutils.h"

using namespace dtAI;


namespace dtTest
{
   class PlannerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(PlannerTests );
      CPPUNIT_TEST(TestCreatePlan);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestCreatePlan();

   private:

      MyNPC mNPC;
   };


   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( PlannerTests );



   void PlannerTests::setUp()
   {
      mNPC.Init();
   }


   void PlannerTests::tearDown()
   {
      
   }

   void PlannerTests::TestCreatePlan()
   {
      std::list<const NPCOperator*> pOperators = mNPC.GetPlanToEat();
      
      std::string callGrandma("CallGrandma");
      CPPUNIT_ASSERT_EQUAL(callGrandma, pOperators.front()->GetName());
      pOperators.pop_front();

      std::string goToStore("GoToStore");
      CPPUNIT_ASSERT_EQUAL(goToStore, pOperators.front()->GetName());
      pOperators.pop_front();

      std::string cook("Cook");
      CPPUNIT_ASSERT_EQUAL(cook, pOperators.front()->GetName());
      pOperators.pop_front();

      std::string eat("Eat");
      CPPUNIT_ASSERT_EQUAL(eat, pOperators.front()->GetName());
      pOperators.pop_front();

   }

}