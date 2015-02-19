/* -*-c++-*-
 * Using 'The MIT License'
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
#include <dtUtil/mathdefines.h>
#include <testGameActorLibrary/testgameactor.h>
#include <testGameActorLibrary/testgameactorlibrary.h>

using namespace dtCore;
 


typedef dtCore::RefPtr<dtGame::GameActorProxy> GameActorPtr;
typedef dtCore::ObserverPtr<dtGame::GameActorProxy> GameActorWeakPtr;

////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class ActorHierarchyTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(ActorHierarchyTests);
      CPPUNIT_TEST(TestActorLinking);
      CPPUNIT_TEST(TestDrawableLinking);
      CPPUNIT_TEST(TestActorTraversal);
      CPPUNIT_TEST(TestActorDeletes);
      CPPUNIT_TEST(TestMapSaveLoad);
   CPPUNIT_TEST_SUITE_END();
   
   public:
      static const std::string TEST_ACTOR_REGISTRY;

      void setUp();
      void tearDown();  

      void TestActorLinking();
      void TestDrawableLinking();
      void TestActorTraversal();
      void TestActorDeletes();
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
void ActorHierarchyTests::TestDrawableLinking()
{
   GameActorPtr actorA = CreateTestActor("A");
   GameActorPtr actorB = CreateTestActor("B");
   GameActorPtr actorC = CreateTestActor("C");
   GameActorPtr actorD = CreateTestActor("D");

   typedef dtCore::DeltaDrawable Drawable;
   Drawable* drawableA = actorA->GetDrawable();
   Drawable* drawableB = actorB->GetDrawable();
   Drawable* drawableC = actorC->GetDrawable();
   Drawable* drawableD = actorD->GetDrawable();

   CPPUNIT_ASSERT(drawableA != NULL);
   CPPUNIT_ASSERT(drawableB != NULL);
   CPPUNIT_ASSERT(drawableC != NULL);
   CPPUNIT_ASSERT(drawableD != NULL);

   actorB->SetParentActor(actorA.get());
   actorC->SetParentActor(actorA.get());
   actorD->SetParentActor(actorC.get());

   // Test the HAS method.
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableB));
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableC));
   CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableD));

   CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableA));
   CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableC));
   CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableD));

   CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableA));
   CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableB));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableD));

   CPPUNIT_ASSERT( ! drawableD->HasChild(*drawableA));
   CPPUNIT_ASSERT( ! drawableD->HasChild(*drawableB));
   CPPUNIT_ASSERT( ! drawableD->HasChild(*drawableC));


   // Test the GET method.
   CPPUNIT_ASSERT(drawableA->GetChild(0) == drawableB);
   CPPUNIT_ASSERT(drawableA->GetChild(1) == drawableC);
   CPPUNIT_ASSERT(drawableA->GetNumChildren() == 2);

   CPPUNIT_ASSERT(drawableB->GetNumChildren() == 0);
   
   CPPUNIT_ASSERT(drawableC->GetChild(0) == drawableD);
   CPPUNIT_ASSERT(drawableC->GetNumChildren() == 1);

   CPPUNIT_ASSERT(drawableD->GetNumChildren() == 0);


   // Test the GET LIST method.
   dtCore::DeltaDrawable::DrawableList drawableList;
   CPPUNIT_ASSERT(drawableA->GetChildren(drawableList) == 2);
   CPPUNIT_ASSERT(drawableList.size() == 2);
   dtCore::DeltaDrawable::DrawableList::iterator iter = drawableList.begin();
   CPPUNIT_ASSERT(iter->get() == drawableB);
   ++iter;
   CPPUNIT_ASSERT(iter->get() == drawableC);
   drawableList.clear();

   CPPUNIT_ASSERT(drawableB->GetChildren(drawableList) == 0);
   CPPUNIT_ASSERT(drawableList.size() == 0);

   CPPUNIT_ASSERT(drawableC->GetChildren(drawableList) == 1);
   CPPUNIT_ASSERT(drawableList.size() == 1);
   iter = drawableList.begin();
   CPPUNIT_ASSERT(iter->get() == drawableD);
   drawableList.clear();

   CPPUNIT_ASSERT(drawableD->GetChildren(drawableList) == 0);
   CPPUNIT_ASSERT(drawableList.size() == 0);


   // Test drawable detachment.
   actorC->SetParentActor(NULL);
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableD));
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableB));
   CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableC));

   actorD->SetParentActor(NULL);
   CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableD));

   actorB->SetParentActor(NULL);
   CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableD));
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
void ActorHierarchyTests::TestActorDeletes()
{
   GameActorPtr actorA = CreateTestActor("A");
   GameActorPtr actorB = CreateTestActor("B");
   GameActorPtr actorC = CreateTestActor("C");
   GameActorPtr actorD = CreateTestActor("D");
   GameActorPtr actorE = CreateTestActor("E");
   GameActorPtr actorF = CreateTestActor("F");
   GameActorPtr actorG = CreateTestActor("G");

   typedef dtCore::DeltaDrawable Drawable;
   typedef dtCore::ObserverPtr<Drawable> DrawablePtr;
   DrawablePtr drawableA = actorA->GetDrawable();
   DrawablePtr drawableB = actorB->GetDrawable();
   DrawablePtr drawableC = actorC->GetDrawable();
   DrawablePtr drawableD = actorD->GetDrawable();
   DrawablePtr drawableE = actorE->GetDrawable();
   DrawablePtr drawableF = actorF->GetDrawable();
   DrawablePtr drawableG = actorG->GetDrawable();

   // Setup the tree
   actorB->SetParentActor(actorA.get());
   actorC->SetParentActor(actorA.get());
   actorD->SetParentActor(actorB.get());
   actorE->SetParentActor(actorB.get());
   actorF->SetParentActor(actorC.get());
   actorG->SetParentActor(actorC.get());

   // Quick verification that the tree has expected references.
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableB));
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableC));
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableD));
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableE));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableF));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableG));


   // Test the detach method.
   CPPUNIT_ASSERT(actorA->DetachChildActors() == 2);
   CPPUNIT_ASSERT(actorB->GetParentActor() == NULL);
   CPPUNIT_ASSERT(actorC->GetParentActor() == NULL);
   CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableB));
   CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableC));

   // Verify that the other children were not affected.
   CPPUNIT_ASSERT(actorD->GetParentActor() == actorB.get());
   CPPUNIT_ASSERT(actorE->GetParentActor() == actorB.get());
   CPPUNIT_ASSERT(actorF->GetParentActor() == actorC.get());
   CPPUNIT_ASSERT(actorG->GetParentActor() == actorC.get());
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableD));
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableE));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableF));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableG));


   // Test delete C to make sure F & G and their drawables still exist.
   actorC = NULL;
   CPPUNIT_ASSERT(actorF->GetParentActor() == NULL);
   CPPUNIT_ASSERT(actorF->GetDrawable() != NULL);
   CPPUNIT_ASSERT(actorF->GetDrawable()->GetOSGNode() != NULL);
   CPPUNIT_ASSERT(actorG->GetParentActor() == NULL);
   CPPUNIT_ASSERT(actorG->GetDrawable() != NULL);
   CPPUNIT_ASSERT(actorG->GetDrawable()->GetOSGNode() != NULL);


   // Test whole tree delete.
   // --- Move children over to weak pointers so that the original pointers
   //     do not cause them to persist when the tree is deleted. The parent
   //     actor ought to keep them around with its own reference pointers.
   GameActorWeakPtr weakPtrD = actorD.get();
   GameActorWeakPtr weakPtrE = actorE.get();
   actorD = NULL;
   actorE = NULL;

   // Validate that the structure has been maintained before the delet test.
   CPPUNIT_ASSERT(weakPtrD.valid());
   CPPUNIT_ASSERT(weakPtrE.valid());
   CPPUNIT_ASSERT(weakPtrD->GetParentActor() == actorB.get());
   CPPUNIT_ASSERT(weakPtrE->GetParentActor() == actorB.get());
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableD));
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableE));

   // Test the tree delete.
   actorB = NULL;
   CPPUNIT_ASSERT( ! weakPtrD.valid());
   CPPUNIT_ASSERT( ! weakPtrE.valid());
}

//////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
bool CompareVec(const osg::Vec3& vecA, const osg::Vec3& vecB, float errorThreshold)
{
   return dtUtil::Abs((vecA - vecB).length()) <= errorThreshold;
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

   typedef dtCore::DeltaDrawable Drawable;
   Drawable* drawableA = actorA->GetDrawable();
   Drawable* drawableB = actorB->GetDrawable();
   Drawable* drawableC = actorC->GetDrawable();
   Drawable* drawableD = actorD->GetDrawable();
   Drawable* drawableE = actorE->GetDrawable();
   Drawable* drawableF = actorF->GetDrawable();
   Drawable* drawableG = actorG->GetDrawable();


   // Setup the test hierarchy.
   actorB->SetParentActor(actorA.get());
   actorC->SetParentActor(actorA.get());
   actorD->SetParentActor(actorB.get());
   actorE->SetParentActor(actorB.get());
   actorF->SetParentActor(actorC.get());
   actorG->SetParentActor(actorC.get());


   // Verify drawable hierarchy.
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableB));
   CPPUNIT_ASSERT(drawableA->HasChild(*drawableC));
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableD));
   CPPUNIT_ASSERT(drawableB->HasChild(*drawableE));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableF));
   CPPUNIT_ASSERT(drawableC->HasChild(*drawableG));


   // Set test trasforms.
   osg::Vec3 transA(0.0f, 0.0f, 5.0f);
   osg::Vec3 transB(1.0f, 1.0f, 1.0f);
   osg::Vec3 transC(-3.0f, -3.0f, 3.0f);
   osg::Vec3 transD(1.0f, -1.0f, -1.0f);
   osg::Vec3 transE(-3.0f, 3.0f, -3.0f);
   osg::Vec3 transF(5.0f, 5.0f, -5.0f);
   osg::Vec3 transG(-7.0f, -7.0f, -7.0f);

   actorA->SetTranslation(transA);
   actorB->SetTranslation(transB);
   actorC->SetTranslation(transC);
   actorD->SetTranslation(transD);
   actorE->SetTranslation(transE);
   actorF->SetTranslation(transF);
   actorG->SetTranslation(transG);


   // Setup the project for writing and reading a map file
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
      typedef std::vector<dtGame::GameActorProxy*> GameActorArray;
      GameActorArray gameActors;

      dtGame::GameActorProxy* gameActor = NULL;
      std::stringstream ss;
      dtGame::GameActorProxy::iterator iter = loadedActorTree->begin();
      dtGame::GameActorProxy::iterator endIter = loadedActorTree->end();
      for (; iter != endIter; ++iter)
      {
         gameActor = dynamic_cast<dtGame::GameActorProxy*>(iter->value);
         CPPUNIT_ASSERT(gameActor != NULL);

         gameActors.push_back(gameActor);
         ss << gameActor->GetName();
      }

      CPPUNIT_ASSERT(ss.str() == "ABDECFG");


      // Test drawable arrangement.
      actorA = gameActors[0];
      actorB = gameActors[1];
      actorD = gameActors[2];
      actorE = gameActors[3];
      actorC = gameActors[4];
      actorF = gameActors[5];
      actorG = gameActors[6];

      drawableA = actorA->GetDrawable();
      drawableB = actorB->GetDrawable();
      drawableC = actorC->GetDrawable();
      drawableD = actorD->GetDrawable();
      drawableE = actorE->GetDrawable();
      drawableF = actorF->GetDrawable();
      drawableG = actorG->GetDrawable();

      CPPUNIT_ASSERT(drawableA->HasChild(*drawableB));
      CPPUNIT_ASSERT(drawableA->HasChild(*drawableC));
      CPPUNIT_ASSERT(drawableB->HasChild(*drawableD));
      CPPUNIT_ASSERT(drawableB->HasChild(*drawableE));
      CPPUNIT_ASSERT(drawableC->HasChild(*drawableF));
      CPPUNIT_ASSERT(drawableC->HasChild(*drawableG));

      // Ensure that other references have not been set.
      CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableA));
      CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableD));
      CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableE));
      CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableF));
      CPPUNIT_ASSERT( ! drawableA->HasChild(*drawableG));

      CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableA));
      CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableB));
      CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableC));
      CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableF));
      CPPUNIT_ASSERT( ! drawableB->HasChild(*drawableG));

      CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableA));
      CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableB));
      CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableC));
      CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableD));
      CPPUNIT_ASSERT( ! drawableC->HasChild(*drawableE));


      // Ensure proper transforms.
      float errorThreshold = 0.0001f;
      osg::Vec3 trans;
      CPPUNIT_ASSERT(CompareVec(transA, actorA->GetTranslation(), errorThreshold));
      CPPUNIT_ASSERT(CompareVec(transB, actorB->GetTranslation(), errorThreshold));
      CPPUNIT_ASSERT(CompareVec(transC, actorC->GetTranslation(), errorThreshold));
      CPPUNIT_ASSERT(CompareVec(transD, actorD->GetTranslation(), errorThreshold));
      CPPUNIT_ASSERT(CompareVec(transE, actorE->GetTranslation(), errorThreshold));
      CPPUNIT_ASSERT(CompareVec(transF, actorF->GetTranslation(), errorThreshold));
      CPPUNIT_ASSERT(CompareVec(transG, actorG->GetTranslation(), errorThreshold));
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
