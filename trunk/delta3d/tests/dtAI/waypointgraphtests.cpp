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
#include <dtAI/waypointgraph.h>
#include <dtAI/waypoint.h>
#include <dtAI/waypointcollection.h>
#include <dtCore/refptr.h>
#include <dtUtil/mathdefines.h>
#include <algorithm>

namespace dtAI
{
   class WaypointGraphTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(WaypointGraphTests);
      CPPUNIT_TEST(TestAddRemoveWaypoints);
      CPPUNIT_TEST(TestAddEdge);
      CPPUNIT_TEST(TestPathfinding);
      CPPUNIT_TEST(TestCollectionBounds);
      CPPUNIT_TEST(TestClearMemory);
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void init();
      void destroy();
      void TestAddRemoveWaypoints();
      void TestAddEdge();
      void TestPathfinding();
      void TestClearMemory();
      void TestCollectionBounds();

   private:

      dtCore::RefPtr<WaypointGraph> mGraph;
  
   };


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( WaypointGraphTests );

void WaypointGraphTests::setUp()
{
   mGraph = new WaypointGraph();
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

   mGraph->RemoveWaypoint(wp1);
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp1->GetID()) == NULL);

   mGraph->RemoveWaypoint(wp2);
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp2->GetID()) == NULL);

   mGraph->Clear();
   CPPUNIT_ASSERT(mGraph->FindWaypoint(wp3->GetID()) == NULL);

   delete wp1;
   delete wp2;
   delete wp3;

}

void WaypointGraphTests::TestAddEdge()
{
   Waypoint* wp1 = new Waypoint(osg::Vec3(1.0f, 1.0f, 1.0f));
   Waypoint* wp2 = new Waypoint(osg::Vec3(2.0f, 2.0f, 2.0f));
   Waypoint* wp3 = new Waypoint(osg::Vec3(3.0f, 3.0f, 3.0f));
   Waypoint* wp4 = new Waypoint(osg::Vec3(4.0f, 4.0f, 4.0f));

   mGraph->AddEdge(wp1, wp2);
   mGraph->AddEdge(wp2, wp1);

   mGraph->AddEdge(wp2, wp3);
   mGraph->AddEdge(wp3, wp2);

   mGraph->AddEdge(wp1, wp4);
   mGraph->AddEdge(wp3, wp4);

   CPPUNIT_ASSERT(mGraph->HasPath(*wp1, *wp2));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp2, *wp3));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp1, *wp3));

   WaypointGraph::ConstWaypointArray waypointArray;
   mGraph->GetAllEdgesFromWaypoint(*wp1, waypointArray);
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
   
   mGraph->AddEdge(wp5, wp6);
   mGraph->AddEdge(wp7, wp8);
   
   mGraph->AddEdge(wp1, wp5);
   mGraph->AddEdge(wp3, wp6);
   mGraph->AddEdge(wp4, wp8);

   //there should be paths between all nodes now
   CPPUNIT_ASSERT(mGraph->HasPath(*wp1, *wp2));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp2, *wp3));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp1, *wp5));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp3, *wp6));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp4, *wp8));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp5, *wp7));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp6, *wp8));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp4, *wp6));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp4, *wp8));
   CPPUNIT_ASSERT(mGraph->HasPath(*wp4, *wp5));

   mGraph->RemoveEdge(wp1, wp2);
   mGraph->RemoveEdge(wp2, wp1);
   mGraph->RemoveAllEdgesFromWaypoint(wp1);

   waypointArray.clear();
   mGraph->GetAllEdgesFromWaypoint(*wp1, waypointArray);
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


void WaypointGraphTests::TestClearMemory()
{
   Waypoint* wp1 = new Waypoint(osg::Vec3(1.0f, 1.0f, 1.0f));
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



   mGraph->AddEdge(wp1, wp2);
   mGraph->AddEdge(wp3, wp4);
   mGraph->AddEdge(wp5, wp6);
   mGraph->AddEdge(wp7, wp8);

   mGraph->Clear();


   mGraph->AddEdge(wp1, wp2);
   mGraph->AddEdge(wp3, wp4);
   mGraph->AddEdge(wp5, wp6);
   mGraph->AddEdge(wp7, wp8);

   mGraph->Clear();


   mGraph->AddEdge(wp1, wp2);
   mGraph->AddEdge(wp3, wp4);

   mGraph->AddEdge(wp2, wp3);

   mGraph->Clear();


   mGraph->AddEdge(wp1, wp2);
   mGraph->AddEdge(wp3, wp4);
   mGraph->AddEdge(wp5, wp6);
   mGraph->AddEdge(wp7, wp8);

   mGraph->AddEdge(wp2, wp3);
   mGraph->AddEdge(wp4, wp5);
   mGraph->AddEdge(wp6, wp7);

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


void WaypointGraphTests::TestCollectionBounds()
{
   Waypoint* wp1 = new Waypoint(osg::Vec3(1.0f, 1.0f, 1.0f));
   Waypoint* wp2 = new Waypoint(osg::Vec3(2.0f, 2.0f, 2.0f));
   Waypoint* wp3 = new Waypoint(osg::Vec3(3.0f, 3.0f, 3.0f));

   mGraph->InsertWaypoint(wp1);
   WaypointCollection* wc = mGraph->FindCollection(wp1->GetID());
   CPPUNIT_ASSERT(wc != NULL);

   CPPUNIT_ASSERT(dtUtil::Equivalent(wp1->GetPosition(), wc->GetPosition()));
   CPPUNIT_ASSERT(wc->GetRadius() == 0.0f);

   //by connecting 1 with 2, 2 should be added to the collection for 1,
   //then taking an average of the position and recalculating the radius 
   mGraph->AddEdge(wp1, wp2);

   osg::Vec3 avg = (wp1->GetPosition() + wp2->GetPosition()) / 2.0f;
   CPPUNIT_ASSERT(dtUtil::Equivalent(avg, wc->GetPosition()));

   float dist = (wp1->GetPosition() - avg).length();
   CPPUNIT_ASSERT(dtUtil::Equivalent(wc->GetRadius(),dist));
}



void WaypointGraphTests::TestPathfinding()
{

}

}

