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
#include <dtAI/aiparticlemanager.h>
#include <dtCore/globals.h>
#include <dtCore/refptr.h>

using namespace dtAI;


namespace dtTest
{
   class AIParticleManagerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AIParticleManagerTests);
      CPPUNIT_TEST(TestAIParticleManager);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestAIParticleManager();

   private:

      AIParticleManager::AgentParticle CloneNPC(BaseNPC* pNPC);
      bool FilterNPC(BaseNPC* pNPC);

      dtCore::RefPtr<AIParticleManager> mManager;

   };


   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AIParticleManagerTests );



   void AIParticleManagerTests::setUp()
   {
      mManager = new AIParticleManager(AIParticleManager::CloneFunctor(this, & AIParticleManagerTests::CloneNPC), AIParticleManager::FilterFunctor(this, &AIParticleManagerTests::FilterNPC));
   }


   void AIParticleManagerTests::tearDown()
   {

   }

   AIParticleManager::AgentParticle AIParticleManagerTests::CloneNPC(BaseNPC* pNPC)
   {
      static float prob = 0.5f;

      BaseNPC* pNewNPC = new BaseNPC("ClonedNPC");
      pNewNPC->LoadNPCScript(dtCore::GetDeltaRootPath() + "/tests/dtAI/npcscript_test.txt");
      pNewNPC->SetWSTemplate(pNPC->GetWSTemplate());
      pNewNPC->InitNPC();
      pNewNPC->SpawnNPC();

      pNewNPC->GeneratePlan();

      return AIParticleManager::AgentParticle(prob, pNewNPC);

   }

   bool AIParticleManagerTests::FilterNPC(BaseNPC* pNPC)
   {
      return true;
   }

   void AIParticleManagerTests::TestAIParticleManager()
   {      
      BaseNPC* pTestNPC = new BaseNPC("TestNPC");
      pTestNPC->LoadNPCScript(dtCore::GetDeltaRootPath() + "/tests/dtAI/npcscript_test.txt");
      pTestNPC->InitNPC();
      pTestNPC->SpawnNPC();

      pTestNPC->GeneratePlan();
 
      mManager->AddAgent(pTestNPC, 1.0);

      mManager->CloneAgent(pTestNPC);

      CPPUNIT_ASSERT_EQUAL(size_t(2), mManager->GetParticleList().size());

      const BaseNPC* pFront = mManager->GetParticleList().front().second.get();
      //the particle manager sorts on probability so the front one should be of probability 1.0 which is named TestNPC
      std::string name("TestNPC");
      CPPUNIT_ASSERT_EQUAL(name, pFront->GetName());

      AIParticleManager::AgentList pList;
      pList.push_back(pTestNPC);
      mManager->RemoveAgents(pList);
      CPPUNIT_ASSERT_EQUAL(size_t(1), mManager->GetParticleList().size());

      //update should call filter which removes all agents where filter returns true
      //this means that for our filter function above all agents should be removed
      mManager->UpdateAgents(0.05f);
      CPPUNIT_ASSERT_EQUAL(size_t(0), mManager->GetParticleList().size());

   }


}
