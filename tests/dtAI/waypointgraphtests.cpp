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
* Bradley Anderegg
*/
#include <cppunit/extensions/HelperMacros.h>
#include <dtAI/waypoint.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/waypointgraph.h>
#include <dtAI/waypointgraphbuilder.h>
#include <dtAI/waypointtypes.h>
#include <dtAI/waypointgraphastar.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtAI/aiactorregistry.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/project.h>

#include <dtCore/refptr.h>
#include <dtUtil/mathdefines.h>
#include <algorithm>

namespace dtAI
{
   class WaypointGraphTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(WaypointGraphTests);
      CPPUNIT_TEST(TestAddRemoveWaypoints);
      CPPUNIT_TEST(TestAddRemoveEdge);
      CPPUNIT_TEST(TestBuildGraph);
      CPPUNIT_TEST(TestPathfinding);
      CPPUNIT_TEST(TestCollections);
      CPPUNIT_TEST(TestCollectionBounds);
      CPPUNIT_TEST(TestLoadSave);
      CPPUNIT_TEST(TestClearMemory);
      CPPUNIT_TEST(TestAddDuplicates);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();      

      void init();
      void destroy();
      void TestAddRemoveWaypoints();
      void TestAddRemoveEdge();
      void TestBuildGraph();
      void TestPathfinding();
      void TestCollections();
      void TestClearMemory();
      void TestLoadSave();
      void TestCollectionBounds();
      void TestAddDuplicates();

   private:
      void CreateWaypoints();

      std::vector<WaypointID> wpArray;
      dtCore::RefPtr<WaypointGraph> mGraph;
      dtCore::RefPtr<AIPluginInterface> mAIInterface;  
   };


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( WaypointGraphTests );

void WaypointGraphTests::setUp()
{
   dtDAL::LibraryManager& libMan = dtDAL::LibraryManager::GetInstance();
   libMan.LoadActorRegistry("dtAI");
   dtCore::RefPtr<dtDAL::ActorProxy> proxy = libMan.CreateActorProxy(*AIActorRegistry::AI_INTERFACE_ACTOR_TYPE);

   mAIInterface = dynamic_cast<dtAI::AIInterfaceActorProxy*>(proxy.get())->GetAIInterface();

   mGraph = &mAIInterface->GetWaypointGraph();

}

void WaypointGraphTests::tearDown()
{
   mGraph = NULL;  
}


void WaypointGraphTests::init()
{
 
}

void WaypointGraphTests::destroy()
{
 
}

void WaypointGraphTests::TestAddRemoveWaypoints()
{
   Waypoint* wp1 = new Waypoint(osg::Vec3(1.0f, 1.0f, 1.0f));
   Waypoint* wp2 = new Waypoint(osg::Vec3(2.0f, 2.0f, 2.0f));
   Waypoint* wp3 = new Waypoint(osg::Vec3(3.0f, 3.0f, 3.0f));

   mGraph->InsertWaypoint(wp1);
   mGraph->InsertWaypoint(wp2);
   mGraph->InsertWaypoint(wp3);

   CPPUNIT_ASSERT(mGraph->Contains(wp1->GetID()));
   CPPUNIT_ASSERT(mGraph->Contains(wp2->GetID()));
   CPPUNIT_ASSERT(mGraph->Contains(wp3->GetID()));

   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp1->GetID()) != NULL);
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp2->GetID()) != NULL);
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp3->GetID()) != NULL);

   mGraph->RemoveWaypoint(wp1->GetID());
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp1->GetID()) == NULL);

   mGraph->RemoveWaypoint(wp2->GetID());
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp2->GetID()) == NULL);

   mGraph->Clear();
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp3->GetID()) == NULL);

   delete wp1;
   delete wp2;
   delete wp3;

}

void WaypointGraphTests::TestAddRemoveEdge()
{
   Waypoint* wp1 = new Waypoint(osg::Vec3(1.0f, 1.0f, 1.0f));
   Waypoint* wp2 = new Waypoint(osg::Vec3(2.0f, 2.0f, 2.0f));
   Waypoint* wp3 = new Waypoint(osg::Vec3(3.0f, 3.0f, 3.0f));
   Waypoint* wp4 = new Waypoint(osg::Vec3(4.0f, 4.0f, 4.0f));

   mGraph->InsertWaypoint(wp1);
   mGraph->InsertWaypoint(wp2);
   mGraph->InsertWaypoint(wp3);
   mGraph->InsertWaypoint(wp4);

   mGraph->AddEdge(wp1->GetID(), wp2->GetID());
   mGraph->AddEdge(wp2->GetID(), wp1->GetID());

   mGraph->AddEdge(wp2->GetID(), wp3->GetID());
   mGraph->AddEdge(wp3->GetID(), wp2->GetID());

   mGraph->AddEdge(wp1->GetID(), wp4->GetID());
   mGraph->AddEdge(wp3->GetID(), wp4->GetID());

   //CPPUNIT_ASSERT(mGraph->HasPath(wp1->GetID(), wp2->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp2->GetID(), wp3->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp1->GetID(), wp3->GetID()));

   WaypointGraph::ConstWaypointArray waypointArray;
   mGraph->GetAllEdgesFromWaypoint(wp1->GetID(), waypointArray);
   CPPUNIT_ASSERT(waypointArray.size() == 2);
   CPPUNIT_ASSERT(std::find(waypointArray.begin(), waypointArray.end(), wp2) != waypointArray.end());
   CPPUNIT_ASSERT(std::find(waypointArray.begin(), waypointArray.end(), wp4) != waypointArray.end());


   Waypoint* wp5 = new Waypoint(osg::Vec3(5.0f, 5.0f, 5.0f));
   Waypoint* wp6 = new Waypoint(osg::Vec3(6.0f, 6.0f, 6.0f));
   Waypoint* wp7 = new Waypoint(osg::Vec3(7.0f, 7.0f, 7.0f));
   Waypoint* wp8 = new Waypoint(osg::Vec3(8.0f, 8.0f, 8.0f));

   mGraph->InsertWaypoint(wp5);
   mGraph->InsertWaypoint(wp6);
   mGraph->InsertWaypoint(wp7);
   mGraph->InsertWaypoint(wp8);
   
   mGraph->AddEdge(wp5->GetID(), wp6->GetID());
   mGraph->AddEdge(wp7->GetID(), wp8->GetID());
   
   mGraph->AddEdge(wp1->GetID(), wp5->GetID());
   mGraph->AddEdge(wp3->GetID(), wp6->GetID());
   mGraph->AddEdge(wp4->GetID(), wp8->GetID());

   //there should be paths between all nodes now
   //CPPUNIT_ASSERT(mGraph->HasPath(wp1->GetID(), wp2->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp2->GetID(), wp3->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp1->GetID(), wp5->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp3->GetID(), wp6->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp4->GetID(), wp8->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp5->GetID(), wp7->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp6->GetID(), wp8->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp4->GetID(), wp6->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp4->GetID(), wp8->GetID()));
   //CPPUNIT_ASSERT(mGraph->HasPath(wp4->GetID(), wp5->GetID()));

   mGraph->RemoveEdge(wp1->GetID(), wp2->GetID());
   mGraph->RemoveEdge(wp2->GetID(), wp1->GetID());
   mGraph->RemoveAllEdgesFromWaypoint(wp1->GetID());

   waypointArray.clear();
   mGraph->GetAllEdgesFromWaypoint(wp1->GetID(), waypointArray);
   CPPUNIT_ASSERT(waypointArray.empty());

   
   mGraph->Clear();

   delete wp1;
   delete wp2;
   delete wp3;
   delete wp4;
   delete wp5;
   delete wp6;
   delete wp7;
   delete wp8;
}

void WaypointGraphTests::CreateWaypoints()
{
   mAIInterface->ClearMemory();
   wpArray.clear();
   wpArray.reserve(17);

   for(int i = 0; i < 17; ++i)
   {  
      Waypoint* wp = new Waypoint(osg::Vec3(i, i, i));
      mAIInterface->InsertWaypoint(wp);
      wpArray.push_back(wp->GetID());
   }


   //0
   mAIInterface->RemoveWaypoint(mAIInterface->GetWaypointById(wpArray[0]));
   //mAIInterface->AddEdge(wpArray[0], wpArray[1]);
   //mAIInterface->AddEdge(wpArray[1], wpArray[0]);

   //1
   mAIInterface->AddEdge(wpArray[1], wpArray[2]);
   mAIInterface->AddEdge(wpArray[2], wpArray[1]);

   mAIInterface->AddEdge(wpArray[1], wpArray[3]);
   mAIInterface->AddEdge(wpArray[3], wpArray[1]);

   mAIInterface->AddEdge(wpArray[1], wpArray[4]);
   mAIInterface->AddEdge(wpArray[4], wpArray[1]);

   //2
   mAIInterface->AddEdge(wpArray[2], wpArray[3]);
   mAIInterface->AddEdge(wpArray[3], wpArray[2]);

   mAIInterface->AddEdge(wpArray[2], wpArray[4]);
   mAIInterface->AddEdge(wpArray[4], wpArray[2]);

   mAIInterface->AddEdge(wpArray[2], wpArray[15]);
   mAIInterface->AddEdge(wpArray[15], wpArray[2]);

   //3
   mAIInterface->AddEdge(wpArray[3], wpArray[4]);
   mAIInterface->AddEdge(wpArray[4], wpArray[3]);

   mAIInterface->AddEdge(wpArray[3], wpArray[5]);
   mAIInterface->AddEdge(wpArray[5], wpArray[3]);

   mAIInterface->AddEdge(wpArray[3], wpArray[6]);
   mAIInterface->AddEdge(wpArray[6], wpArray[3]);

   //4
   mAIInterface->AddEdge(wpArray[4], wpArray[5]);
   mAIInterface->AddEdge(wpArray[5], wpArray[4]);

   mAIInterface->AddEdge(wpArray[4], wpArray[6]);
   mAIInterface->AddEdge(wpArray[6], wpArray[4]);

   //5
   mAIInterface->AddEdge(wpArray[5], wpArray[6]);
   mAIInterface->AddEdge(wpArray[6], wpArray[5]);

   //6
   mAIInterface->AddEdge(wpArray[6], wpArray[7]);
   mAIInterface->AddEdge(wpArray[7], wpArray[6]);

   //7
   mAIInterface->AddEdge(wpArray[7], wpArray[8]);
   mAIInterface->AddEdge(wpArray[8], wpArray[7]);

   //8
   mAIInterface->AddEdge(wpArray[8], wpArray[9]);
   mAIInterface->AddEdge(wpArray[9], wpArray[8]);

   //9
   mAIInterface->AddEdge(wpArray[9], wpArray[10]);
   mAIInterface->AddEdge(wpArray[10], wpArray[9]);

   mAIInterface->AddEdge(wpArray[9], wpArray[11]);
   mAIInterface->AddEdge(wpArray[11], wpArray[9]);

   mAIInterface->AddEdge(wpArray[9], wpArray[12]);
   mAIInterface->AddEdge(wpArray[12], wpArray[9]);

   //10
   mAIInterface->AddEdge(wpArray[10], wpArray[11]);
   mAIInterface->AddEdge(wpArray[11], wpArray[10]);

   mAIInterface->AddEdge(wpArray[10], wpArray[12]);
   mAIInterface->AddEdge(wpArray[12], wpArray[10]);

   //11
   mAIInterface->AddEdge(wpArray[11], wpArray[12]);
   mAIInterface->AddEdge(wpArray[12], wpArray[11]);

   mAIInterface->AddEdge(wpArray[11], wpArray[13]);
   mAIInterface->AddEdge(wpArray[13], wpArray[11]);

   mAIInterface->AddEdge(wpArray[11], wpArray[14]);
   mAIInterface->AddEdge(wpArray[14], wpArray[11]);

   //12
   mAIInterface->AddEdge(wpArray[12], wpArray[13]);
   mAIInterface->AddEdge(wpArray[13], wpArray[12]);

   mAIInterface->AddEdge(wpArray[12], wpArray[14]);
   mAIInterface->AddEdge(wpArray[14], wpArray[12]);

   //13
   mAIInterface->AddEdge(wpArray[13], wpArray[14]);
   mAIInterface->AddEdge(wpArray[14], wpArray[13]);

   mAIInterface->AddEdge(wpArray[13], wpArray[16]);
   mAIInterface->AddEdge(wpArray[16], wpArray[13]);

   //14
   //15
   mAIInterface->AddEdge(wpArray[15], wpArray[16]);
   mAIInterface->AddEdge(wpArray[16], wpArray[15]);

   dtCore::RefPtr<WaypointGraphBuilder> mBuilder = new WaypointGraphBuilder(*mAIInterface, mAIInterface->GetWaypointGraph());
   mGraph->CreateSearchGraph(mBuilder.get(), 10);
}

void WaypointGraphTests::TestBuildGraph()
{   
   CreateWaypoints();

   
   CPPUNIT_ASSERT(mGraph->GetNumSearchLevels() == 4);   
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(0)->mNodes.size() == 16);   
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(1)->mNodes.size() == 6);   
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(2)->mNodes.size() == 3); 
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(3)->mNodes.size() == 1);


   //there should be paths between all nodes now
   for(int i = 1; i < 17; ++i)
   {
      for(int j = 1; j < 17; ++j)
      {
         if(i != j)
         {
            CPPUNIT_ASSERT(mGraph->HasPath(wpArray[i], wpArray[j]));
         }
      }
   }

   //cleanup
   mGraph->Clear();

   //delete wp1;
   //delete wp2;
   //delete wp3;
   //delete wp4;
   //delete wp5;
   //delete wp6;
   //delete wp7;
   //delete wp8;
}


void WaypointGraphTests::TestClearMemory()
{
   /*Waypoint* wp1 = new Waypoint(osg::Vec3(1.0f, 1.0f, 1.0f));
   Waypoint* wp2 = new Waypoint(osg::Vec3(2.0f, 2.0f, 2.0f));
   Waypoint* wp3 = new Waypoint(osg::Vec3(3.0f, 3.0f, 3.0f));
   Waypoint* wp4 = new Waypoint(osg::Vec3(4.0f, 4.0f, 4.0f));
   Waypoint* wp5 = new Waypoint(osg::Vec3(5.0f, 5.0f, 5.0f));
   Waypoint* wp6 = new Waypoint(osg::Vec3(6.0f, 6.0f, 6.0f));
   Waypoint* wp7 = new Waypoint(osg::Vec3(7.0f, 7.0f, 7.0f));
   Waypoint* wp8 = new Waypoint(osg::Vec3(8.0f, 8.0f, 8.0f));

   mGraph->InsertWaypoint(wp1);
   mGraph->InsertWaypoint(wp2);
   mGraph->InsertWaypoint(wp3);
   mGraph->InsertWaypoint(wp4);
   mGraph->InsertWaypoint(wp5);
   mGraph->InsertWaypoint(wp6);
   mGraph->InsertWaypoint(wp7);
   mGraph->InsertWaypoint(wp8);

   mGraph->Clear();



   mGraph->AddEdge(wp1->GetID(), wp2->GetID());
   mGraph->AddEdge(wp3->GetID(), wp4->GetID());
   mGraph->AddEdge(wp5->GetID(), wp6->GetID());
   mGraph->AddEdge(wp7->GetID(), wp8->GetID());

   mGraph->Clear();


   mGraph->AddEdge(wp1->GetID(), wp2->GetID());
   mGraph->AddEdge(wp3->GetID(), wp4->GetID());
   mGraph->AddEdge(wp5->GetID(), wp6->GetID());
   mGraph->AddEdge(wp7->GetID(), wp8->GetID());

   mGraph->Clear();


   mGraph->AddEdge(wp1->GetID(), wp2->GetID());
   mGraph->AddEdge(wp3->GetID(), wp4->GetID());

   mGraph->AddEdge(wp2->GetID(), wp3->GetID());

   mGraph->Clear();


   mGraph->AddEdge(wp1->GetID(), wp2->GetID());
   mGraph->AddEdge(wp3->GetID(), wp4->GetID());
   mGraph->AddEdge(wp5->GetID(), wp6->GetID());
   mGraph->AddEdge(wp7->GetID(), wp8->GetID());

   mGraph->AddEdge(wp2->GetID(), wp3->GetID());
   mGraph->AddEdge(wp4->GetID(), wp5->GetID());
   mGraph->AddEdge(wp6->GetID(), wp7->GetID());

   mGraph->Clear();


   delete wp1;
   delete wp2;
   delete wp3;
   delete wp4;
   delete wp5;
   delete wp6;
   delete wp7;
   delete wp8;*/
}


void WaypointGraphTests::TestCollectionBounds()
{
   CreateWaypoints();

   const WaypointInterface* wp1 = mGraph->FindWaypoint(wpArray[1]);

   WaypointCollection* wc = mGraph->FindCollection(wpArray[1]);
   CPPUNIT_ASSERT(wc != NULL);

   
   osg::Vec3 wp1Pos = wp1->GetPosition();
   osg::Vec3 vecToWp1 = wp1Pos - wc->GetPosition();
   float length = vecToWp1.length();
   CPPUNIT_ASSERT(length <= wc->GetRadius());

   //CPPUNIT_ASSERT(dtUtil::Equivalent(wp1->GetPosition(), wc->GetPosition()));
   //CPPUNIT_ASSERT(wc->GetRadius() == 0.0f);

   ////by connecting 1 with 2, 2 should be added to the collection for 1,
   ////then taking an average of the position and recalculating the radius 
   //mGraph->AddEdge(wp1->GetID(), wp2->GetID());

   //osg::Vec3 avg = (wp1->GetPosition() + wp2->GetPosition()) / 2.0f;
   //CPPUNIT_ASSERT(dtUtil::Equivalent(avg, wc->GetPosition()));

   //float dist = (wp1->GetPosition() - avg).length();
   //CPPUNIT_ASSERT(dtUtil::Equivalent(wc->GetRadius(),dist));
}

void WaypointGraphTests::TestPathfinding()
{
   CreateWaypoints();

   WaypointGraphAStar astar(*mGraph); 

   WaypointGraph::ConstWaypointArray path;
   PathFindResult result = astar.HierarchicalFindPath(wpArray[5], wpArray[14], path);
   CPPUNIT_ASSERT_EQUAL(result, PATH_FOUND);


   for(int i = 1; i < 17; ++i)
   {
      for(int j = 1; j < 17; ++j)
      {
         if(i != j)
         {
            path.clear();
            result = astar.HierarchicalFindPath(wpArray[i], wpArray[j], path);
            CPPUNIT_ASSERT_EQUAL(result, PATH_FOUND);
         }
      }
   }

}

void WaypointGraphTests::TestCollections()
{
   mAIInterface->ClearMemory();

   //lets create two sets of three waypoints with a bridge between them
   
   WaypointInterface* wp01_grp01 = mAIInterface->CreateWaypoint(osg::Vec3(-1.0f, 0.0f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);
   WaypointInterface* wp02_grp01 = mAIInterface->CreateWaypoint(osg::Vec3(-2.0f, 0.0f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);
   WaypointInterface* wp03_grp01 = mAIInterface->CreateWaypoint(osg::Vec3(-1.5f, 0.5f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);   

   mAIInterface->AddEdge(wp01_grp01->GetID(), wp02_grp01->GetID());
   mAIInterface->AddEdge(wp02_grp01->GetID(), wp01_grp01->GetID());

   mAIInterface->AddEdge(wp01_grp01->GetID(), wp03_grp01->GetID());
   mAIInterface->AddEdge(wp03_grp01->GetID(), wp01_grp01->GetID());

   mAIInterface->AddEdge(wp02_grp01->GetID(), wp03_grp01->GetID());
   mAIInterface->AddEdge(wp03_grp01->GetID(), wp02_grp01->GetID());

   //add the first set of waypoints to one collection
   dtCore::RefPtr<WaypointCollection> wc_grp01 = new WaypointCollection();
   mAIInterface->Assign(wp01_grp01->GetID(), wc_grp01.get());
   mAIInterface->Assign(wp02_grp01->GetID(), wc_grp01.get());
   mAIInterface->Assign(wp03_grp01->GetID(), wc_grp01.get());


   //now lets create a second group of waypoints
   WaypointInterface* wp01_grp02 = mAIInterface->CreateWaypoint(osg::Vec3(1.0f, 0.0f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);
   WaypointInterface* wp02_grp02 = mAIInterface->CreateWaypoint(osg::Vec3(2.0f, 0.0f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);
   WaypointInterface* wp03_grp02 = mAIInterface->CreateWaypoint(osg::Vec3(1.5f, 0.5f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);

   mAIInterface->AddEdge(wp01_grp02->GetID(), wp02_grp02->GetID());
   mAIInterface->AddEdge(wp02_grp02->GetID(), wp01_grp02->GetID());

   mAIInterface->AddEdge(wp01_grp02->GetID(), wp03_grp02->GetID());
   mAIInterface->AddEdge(wp03_grp02->GetID(), wp01_grp02->GetID());

   mAIInterface->AddEdge(wp02_grp02->GetID(), wp03_grp02->GetID());
   mAIInterface->AddEdge(wp03_grp02->GetID(), wp02_grp02->GetID());
   
   //and add them to a second waypoint collection
   dtCore::RefPtr<WaypointCollection> wc_grp02 = new WaypointCollection();
   mAIInterface->Assign(wp01_grp02->GetID(), wc_grp02.get());
   mAIInterface->Assign(wp02_grp02->GetID(), wc_grp02.get());
   mAIInterface->Assign(wp03_grp02->GetID(), wc_grp02.get());


   //now lets create a bridge between them, or point that is accessible from both groups
   WaypointInterface* wpBridge = mAIInterface->CreateWaypoint(osg::Vec3(0.0f, 0.0f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);

   mAIInterface->AddEdge(wpBridge->GetID(), wp01_grp01->GetID());
   mAIInterface->AddEdge(wp01_grp01->GetID(), wpBridge->GetID());

   mAIInterface->AddEdge(wpBridge->GetID(), wp01_grp02->GetID());
   mAIInterface->AddEdge(wp01_grp02->GetID(), wpBridge->GetID());

   //technically the bridge could be in either collection, or a new one
   //lets make it a new one for fun
   dtCore::RefPtr<WaypointCollection> wc_bridge = new WaypointCollection();
   mAIInterface->Assign(wpBridge->GetID(), wc_bridge.get());

   //now we currently have three groups of waypoints, but until they have a common parent
   //the HierarchicalFindPath() will not work on them, though the standard FindPath() will (on the AIPluginInterface)
   //since it just operates at the lowest level, so lets make a parent
   dtCore::RefPtr<WaypointCollection> rootNode = new WaypointCollection();
   mAIInterface->Assign(wc_grp01->GetID(), rootNode.get());
   mAIInterface->Assign(wc_grp02->GetID(), rootNode.get());
   mAIInterface->Assign(wc_bridge->GetID(), rootNode.get());

   //and since we created our own tree (I'll show you how to have it done automatically below)
   //we must have the waypoint graph create all absract edges necessary for pathing   
   mAIInterface->GetWaypointGraph().CreateAbstractEdges();

   //now we have a small hierarchical waypoint representation
   //how do we search on it- well thats the easy part :)
   WaypointGraph::ConstWaypointArray path;
   PathFindResult result = mAIInterface->HierarchicalFindPath(wp02_grp01->GetID(), wp02_grp02->GetID(), path);
   CPPUNIT_ASSERT_EQUAL(result, PATH_FOUND);

   //or what if we want to find a higher level path, thats quite simple as well
   //the standard FindPath() will operate at whichever level of the tree that the waypoints
   //its given are on, as long as they are both on the same search level.
   path.clear();
   result = mAIInterface->FindPath(wc_grp01->GetID(), wc_grp02->GetID(), path);
   CPPUNIT_ASSERT_EQUAL(result, PATH_FOUND);
}

void WaypointGraphTests::TestLoadSave()
{
   CreateWaypoints();

   //dtDAL::Project& proj = dtDAL::Project::GetInstance();
   //std::string filePath = proj.GetContext();
   std::string filename = "WaypointGraphTests_TestWaypointFile.ai";
   //std::string fullFilename = filePath + '/' + filename;
   std::string fullFilename = "./" + filename;

   bool result = mAIInterface->SaveWaypointFile(fullFilename);
   CPPUNIT_ASSERT_MESSAGE("Error saving waypoint file '" + fullFilename + "'.", result);

   mAIInterface->ClearMemory();

   CPPUNIT_ASSERT(mGraph->GetNumSearchLevels() == 0);   

   result = mAIInterface->LoadWaypointFile(fullFilename);
   CPPUNIT_ASSERT_MESSAGE("Error loading waypoint file '" + fullFilename + "'.", result);

   int numSearchLevels = mGraph->GetNumSearchLevels();
   CPPUNIT_ASSERT(numSearchLevels == 4);   
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(0)->mNodes.size() == 16);   
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(1)->mNodes.size() == 6);   
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(2)->mNodes.size() == 3); 
   CPPUNIT_ASSERT(mGraph->GetSearchLevel(3)->mNodes.size() == 1);


   //there should be paths between all nodes now
   for(int i = 1; i < 17; ++i)
   {
      for(int j = 1; j < 17; ++j)
      {
         if(i != j)
         {
            CPPUNIT_ASSERT(mGraph->HasPath(wpArray[i], wpArray[j]));
         }
      }
   }

   //try pathfinding on it after loading it from file
   WaypointGraphAStar astar(*mGraph); 

   WaypointGraph::ConstWaypointArray path;
   for(int i = 1; i < 17; ++i)
   {
      for(int j = 1; j < 17; ++j)
      {
         if(i != j)
         {
            path.clear();
            PathFindResult result = astar.HierarchicalFindPath(wpArray[i], wpArray[j], path);
            CPPUNIT_ASSERT_EQUAL(result, PATH_FOUND);
         }
      }
   }

}

void WaypointGraphTests::TestAddDuplicates()
{
   int i = 0;
   WaypointGraph::WaypointArray wps;

   mAIInterface->ClearMemory();
   
   //lets create two sets of three waypoints with a bridge between them

   WaypointInterface* wp01 = mAIInterface->CreateWaypoint(osg::Vec3(-1.0f, 0.0f, 0.0f), *WaypointTypes::DEFAULT_WAYPOINT);

   for(i = 0; i < 5; ++i)
   {
      mAIInterface->InsertWaypoint(wp01);
   }
   
   mAIInterface->GetWaypoints(wps);
   CPPUNIT_ASSERT(wps.size() == 1);

   //now lets test this on collections
   dtCore::RefPtr<WaypointCollection> wc = new WaypointCollection();

   for(i = 0; i < 5; ++i)
   {
      mAIInterface->InsertWaypoint(wc.get());
   }

   wps.clear();
   mAIInterface->GetWaypoints(wps);
   CPPUNIT_ASSERT(wps.size() == 2);

   for(i = 0; i < 5; ++i)
   {
      mAIInterface->Assign(wp01->GetID(), wc.get());
   }

   CPPUNIT_ASSERT(wc->degree() == 1);

   //now lets try assigning at the wrong level
   dtCore::RefPtr<WaypointCollection> rootNode = new WaypointCollection();
   mAIInterface->InsertWaypoint(rootNode.get());

   bool assignResult = mAIInterface->Assign(wc->GetID() , rootNode.get());

   CPPUNIT_ASSERT(assignResult);
   CPPUNIT_ASSERT(rootNode->degree() == 1);
   CPPUNIT_ASSERT(rootNode->level() == 0);
   CPPUNIT_ASSERT(wc->level() == 1);

   WaypointGraph& wpGraph = mAIInterface->GetWaypointGraph();
   CPPUNIT_ASSERT(wpGraph.GetSearchLevelNum(wp01->GetID()) == 0);
   CPPUNIT_ASSERT(wpGraph.GetSearchLevelNum(wc->GetID()) == 1);
   CPPUNIT_ASSERT(wpGraph.GetSearchLevelNum(rootNode->GetID()) == 2);

   wps.clear();
   mAIInterface->GetWaypoints(wps);
   CPPUNIT_ASSERT(wps.size() == 3);

}

}

