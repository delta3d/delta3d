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
* @author Bradley Anderegg
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include "testplannerutils.h"
#include <dtAI/basenpc.h>
#include <dtAI/npcparser.h>
#include <dtAI/aiparticlemanager.h>
#include <dtCore/refptr.h>
#include <dtUtil/datapathutils.h>

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

      AIParticleManager::AgentParticle CloneNPC(AIParticleManager::AgentParticle& pNPC);
      bool FilterNPC(AIParticleManager::AgentParticle& pNPC);

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

   AIParticleManager::AgentParticle AIParticleManagerTests::CloneNPC(AIParticleManager::AgentParticle& pNPC)
   {
      static float prob = 0.5f;

      BaseNPC* pNewNPC = new BaseNPC("ClonedNPC");
      CPPUNIT_ASSERT(pNewNPC->LoadNPCScript("./data/npcscript_test.txt"));
      pNewNPC->SetWSTemplate(pNPC.second->GetWSTemplate());
      pNewNPC->InitNPC();
      pNewNPC->SpawnNPC();

      pNewNPC->GeneratePlan();

      return AIParticleManager::AgentParticle(prob, pNewNPC);

   }

   bool AIParticleManagerTests::FilterNPC(AIParticleManager::AgentParticle& pNPC)
   {
      return true;
   }

   void AIParticleManagerTests::TestAIParticleManager()
   {      
      BaseNPC* pTestNPC = new BaseNPC("TestNPC");
      CPPUNIT_ASSERT(!pTestNPC->LoadNPCScript("gook.txt"));
      CPPUNIT_ASSERT(pTestNPC->LoadNPCScript("./data/npcscript_test.txt"));
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
