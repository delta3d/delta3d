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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include "testplannerutils.h"
#include <dtAI/basenpc.h>
#include <dtAI/npcparser.h>
#include <dtCore/globals.h>
#include <dtCore/refptr.h>

using namespace dtAI;


namespace dtTest
{
   class PlannerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(PlannerTests );
      CPPUNIT_TEST(TestCreatePlan);
      CPPUNIT_TEST(TestPlannerScript);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestCreatePlan();
      void TestPlannerScript();

   private:

      void VerifyPlan(std::list<const Operator*>& pPlan, bool pCallGrandma);

   };


   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( PlannerTests );



   void PlannerTests::setUp()
   {
      
   }


   void PlannerTests::tearDown()
   {
      
   }

   void PlannerTests::TestCreatePlan()
   {
      MyNPC mNPC;
      mNPC.InitNPC();
      mNPC.SpawnNPC();
      mNPC.GeneratePlan();
      std::list<const Operator*> pOperators = mNPC.GetPlan();
      VerifyPlan(pOperators, true);    

      //step through the plan
      while(!mNPC.GetPlan().empty())
      {
         mNPC.Update(0.05);
      }

      mNPC.MakeHungry();
      mNPC.GeneratePlan();
      pOperators = mNPC.GetPlan();
      VerifyPlan(pOperators, false);

      //step through the next plan
      while(!mNPC.GetPlan().empty())
      {
         mNPC.Update(0.05);
      }

   }

   void PlannerTests::TestPlannerScript()
   {
      NPCParser parser;            
      dtCore::RefPtr<BaseNPC> pTestNPC = new BaseNPC("TestNPC");
      pTestNPC->LoadNPCScript(dtCore::GetDeltaRootPath() + "/tests/dtAI/npcscript_test.txt");
      pTestNPC->InitNPC();
      pTestNPC->SpawnNPC();

      pTestNPC->GeneratePlan();
      std::list<const Operator*> pOperators = pTestNPC->GetPlan();
      VerifyPlan(pOperators, true);
   }

   void PlannerTests::VerifyPlan(std::list<const Operator*>& pOperators, bool pCallGrandma)
   {
      if(pCallGrandma)
      {
         std::string callGrandma("CallGrandma");
         CPPUNIT_ASSERT_EQUAL(callGrandma, pOperators.front()->GetName());
         pOperators.pop_front();
      }

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
