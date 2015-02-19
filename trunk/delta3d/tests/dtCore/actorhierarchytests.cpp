/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <testGameActorLibrary/testgameactor.h>
#include <testGameActorLibrary/testgameactorlibrary.h>

using namespace dtCore;
 


typedef dtCore::RefPtr<dtGame::GameActorProxy> GameActorPtr;

////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class ActorHierarchyTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(ActorHierarchyTests);
      CPPUNIT_TEST(TestActorLinking);
      CPPUNIT_TEST(TestActorTraversal);
      CPPUNIT_TEST(TestMapSaveLoad);
   CPPUNIT_TEST_SUITE_END();
   
   public:
      static const std::string TEST_ACTOR_REGISTRY;

      void setUp();
      void tearDown();  

      void TestActorLinking();
      void TestActorTraversal();
      void TestMapSaveLoad();

      GameActorPtr CreateTestActor(const std::string& name) const;

      bool mMapSaved;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ActorHierarchyTests);

const std::string ActorHierarchyTests::TEST_ACTOR_REGISTRY("testGameActorLibrary");
const std::string TEST_PROJECT_DIR(dtUtil::GetDeltaRootPath() + "/examples/data/TestProject");
const std::string MAP_NAME("TestActorHierarchy");



/////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::setUp()
{
   mMapSaved = false;

   dtCore::ActorFactory::GetInstance().LoadActorRegistry(TEST_ACTOR_REGISTRY);
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::tearDown()
{
   dtCore::Project& proj = dtCore::Project::GetInstance();
   try
   {
      proj.CloseAllMaps();

      if (mMapSaved)
      {
         mMapSaved = false;
         proj.DeleteMap(MAP_NAME, true);
      }
   }
   catch (...)
   {
      CPPUNIT_FAIL("Failed cleaningup " + MAP_NAME
         + "in tests/" + TEST_PROJECT_DIR + "/Maps");
   }
   proj.ClearAllContexts();

   dtCore::ActorFactory::GetInstance().UnloadActorRegistry(TEST_ACTOR_REGISTRY);
}

//////////////////////////////////////////////////////////////////////////
GameActorPtr ActorHierarchyTests::CreateTestActor(const std::string& name) const
{
   dtCore::RefPtr<BaseActorObject> actor = dtCore::ActorFactory::GetInstance().CreateActor(*TestGameActorLibrary::TEST1_GAME_ACTOR_TYPE);
   actor->SetName(name);
   return dynamic_cast<dtGame::GameActorProxy*>(actor.get());
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestActorLinking()
{
   GameActorPtr actorA = CreateTestActor("A");
   GameActorPtr actorB = CreateTestActor("B");
   GameActorPtr actorC = CreateTestActor("C");
   GameActorPtr actorD = CreateTestActor("D");

   CPPUNIT_ASSERT(actorA.valid());
   CPPUNIT_ASSERT(actorB.valid());
   CPPUNIT_ASSERT(actorC.valid());
   CPPUNIT_ASSERT(actorD.valid());

   actorB->SetParentActor(actorA.get());
   actorC->SetParentActor(actorA.get());
   actorD->SetParentActor(actorC.get());

   CPPUNIT_ASSERT(actorA->GetParentActor() == NULL);
   CPPUNIT_ASSERT(actorB->GetParentActor() == actorA.get());
   CPPUNIT_ASSERT(actorC->GetParentActor() == actorA.get());
   CPPUNIT_ASSERT(actorD->GetParentActor() == actorC.get());

   // Test tree specific methods
   CPPUNIT_ASSERT( ! actorA->is_descendant_of(*actorA));
   CPPUNIT_ASSERT(actorB->is_descendant_of(*actorA));
   CPPUNIT_ASSERT(actorC->is_descendant_of(*actorA));
   CPPUNIT_ASSERT(actorD->is_descendant_of(*actorA));
   
   CPPUNIT_ASSERT( ! actorC->is_descendant_of(*actorB));
   CPPUNIT_ASSERT( ! actorD->is_descendant_of(*actorB));
   CPPUNIT_ASSERT(actorD->is_descendant_of(*actorC));

   CPPUNIT_ASSERT(actorA->is_root());
   CPPUNIT_ASSERT( ! actorB->is_root());
   CPPUNIT_ASSERT( ! actorC->is_root());
   CPPUNIT_ASSERT( ! actorD->is_root());

   CPPUNIT_ASSERT( ! actorA->is_leaf());
   CPPUNIT_ASSERT(actorB->is_leaf());
   CPPUNIT_ASSERT( ! actorC->is_leaf());
   CPPUNIT_ASSERT(actorD->is_leaf());

   CPPUNIT_ASSERT(actorB->next_sibling() == actorC.get());
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestActorTraversal()
{
   GameActorPtr actorA = CreateTestActor("A");
   GameActorPtr actorB = CreateTestActor("B");
   GameActorPtr actorC = CreateTestActor("C");
   GameActorPtr actorD = CreateTestActor("D");
   GameActorPtr actorE = CreateTestActor("E");
   GameActorPtr actorF = CreateTestActor("F");
   GameActorPtr actorG = CreateTestActor("G");

   actorB->SetParentActor(actorA.get());
   actorC->SetParentActor(actorA.get());
   actorD->SetParentActor(actorB.get());
   actorE->SetParentActor(actorB.get());
   actorF->SetParentActor(actorC.get());
   actorG->SetParentActor(actorC.get());

   std::stringstream ss;
   dtGame::GameActorProxy::iterator iter = actorA->begin();
   dtGame::GameActorProxy::iterator endIter = actorA->end();
   for (; iter != endIter; ++iter)
   {
      ss << iter->GetName();
   }

   CPPUNIT_ASSERT(ss.str() == "ABDECFG");
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestMapSaveLoad()
{
   GameActorPtr actorA = CreateTestActor("A");
   GameActorPtr actorB = CreateTestActor("B");
   GameActorPtr actorC = CreateTestActor("C");
   GameActorPtr actorD = CreateTestActor("D");
   GameActorPtr actorE = CreateTestActor("E");
   GameActorPtr actorF = CreateTestActor("F");
   GameActorPtr actorG = CreateTestActor("G");

   actorB->SetParentActor(actorA.get());
   actorC->SetParentActor(actorA.get());
   actorD->SetParentActor(actorB.get());
   actorE->SetParentActor(actorB.get());
   actorF->SetParentActor(actorC.get());
   actorG->SetParentActor(actorC.get());

   dtCore::Project& proj = dtCore::Project::GetInstance();
   dtCore::Map* loadedMap = NULL;

   try
   {
      // Prepare a temporary project and map for testing saving and loading
      // of an actor hierarchy to and from file.
      proj.ClearAllContexts();
      proj.SetReadOnly(false);
      proj.CreateContext(TEST_PROJECT_DIR, false);
      proj.AddContext(TEST_PROJECT_DIR);

      dtUtil::FileUtils::GetInstance().MakeDirectory(TEST_PROJECT_DIR + "/Maps");

      dtCore::Map* testMap = &proj.CreateMap(MAP_NAME, MAP_NAME+".dtmap");
      testMap->SetAuthor("Chris");
      testMap->SetDescription("Map for testing write and read of actor hierarchies.");

      // One complex actor added to the map should be saved entirely.
      // All child actors should be written upon save.
      testMap->AddProxy(*actorA);

      // Save, close and reload map.
      proj.SaveMap(*testMap);
      proj.CloseMap(*testMap);

      mMapSaved = true;

      loadedMap = &proj.GetMap(MAP_NAME);
      dtCore::ActorRefPtrVector actors;

      // One complex actor written should be loaded as a single complex actor.
      // All child actors should already be attached.
      loadedMap->GetAllProxies(actors);
      CPPUNIT_ASSERT(actors.size() == 1);

      GameActorPtr loadedActorTree = dynamic_cast<dtGame::GameActorProxy*>(actors[0].get());
      CPPUNIT_ASSERT(loadedActorTree.valid());

      // Verify that the actor structure and order has been maintained.
      std::stringstream ss;
      dtGame::GameActorProxy::iterator iter = loadedActorTree->begin();
      dtGame::GameActorProxy::iterator endIter = loadedActorTree->end();
      for (; iter != endIter; ++iter)
      {
         ss << iter->GetName();
      }

      CPPUNIT_ASSERT(ss.str() == "ABDECFG");
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   catch (...)
   {
      CPPUNIT_FAIL("Unknown exception hit.");
   }
}
