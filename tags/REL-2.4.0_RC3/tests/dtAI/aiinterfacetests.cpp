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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>
#include <dtDAL/librarymanager.h>
#include <dtCore/refptr.h>
#include <osg/Vec3>

namespace dtAI
{
   class AIInterfaceTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AIInterfaceTests);
      CPPUNIT_TEST( TestAddRemoveWaypoints );
      CPPUNIT_TEST( TestAddRemoveEdge );
      CPPUNIT_TEST( TestPathfinding );
      CPPUNIT_TEST( TestLoadSave );
      CPPUNIT_TEST( TestWaypointPropertyContainer );
      CPPUNIT_TEST( TestMemory );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();
         
         void TestAddRemoveWaypoints();
         void TestAddRemoveEdge();
         void TestPathfinding();
         void TestLoadSave();
         void TestWaypointPropertyContainer();
         void TestMemory();

      private:
         dtCore::RefPtr<AIPluginInterface> mAIInterface;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AIInterfaceTests );

   void AIInterfaceTests::setUp()
   {
      dtDAL::LibraryManager& libMan = dtDAL::LibraryManager::GetInstance();
      libMan.LoadActorRegistry("dtAI");
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = libMan.CreateActorProxy(*AIActorRegistry::AI_INTERFACE_ACTOR_TYPE);

      mAIInterface = dynamic_cast<dtAI::AIInterfaceActorProxy*>(proxy.get())->GetAIInterface();

   }

   void AIInterfaceTests::tearDown()
   {
      
   }

   void AIInterfaceTests::TestAddRemoveWaypoints()
   {
      std::vector<dtCore::RefPtr<const dtDAL::ObjectType> > objectTypes;

      mAIInterface->GetSupportedWaypointTypes(objectTypes);
      CPPUNIT_ASSERT(!objectTypes.empty());
      CPPUNIT_ASSERT(mAIInterface->IsWaypointTypeSupported(objectTypes[0].get()));
      
      osg::Vec3 waypointPos(2.0f, 4.9f, 1.9f);
      WaypointInterface* wp01 = mAIInterface->CreateWaypoint(waypointPos, *(objectTypes[0]));
      CPPUNIT_ASSERT(wp01 != NULL);
      int numWaypoints = 1;

      //test no duplicate create
      osg::Vec3 waypointPos2(1.9f, 4.9f, 1.9f);
      WaypointInterface* wp02 = mAIInterface->CreateWaypointNoDuplicates(waypointPos2, 0.2f, *(objectTypes[0]));
      CPPUNIT_ASSERT(wp02 == wp01);

      //lets try again
      waypointPos2.set(4.0f, 4.9f, 1.9f);
      wp02 = mAIInterface->CreateWaypointNoDuplicates(waypointPos2, 0.2f, *(objectTypes[0]));
      CPPUNIT_ASSERT(wp02 != NULL);
      CPPUNIT_ASSERT(wp02 != wp01);
      ++numWaypoints;

      //lets make a couple more
      osg::Vec3 waypointPos3(6.0f, 4.9f, 1.9f);
      WaypointInterface* wp03 = mAIInterface->CreateWaypointNoDuplicates(waypointPos3, 0.2f, *(objectTypes[0]));
      CPPUNIT_ASSERT(wp03 != NULL);
      CPPUNIT_ASSERT(wp03 != wp01);
      CPPUNIT_ASSERT(wp03 != wp02);
      ++numWaypoints;

      osg::Vec3 waypointPos4(8.0f, 4.9f, 1.9f);
      WaypointInterface* wp04 = mAIInterface->CreateWaypointNoDuplicates(waypointPos4, 0.2f, *(objectTypes[0]));
      CPPUNIT_ASSERT(wp04 != NULL);
      CPPUNIT_ASSERT(wp04 != wp01);
      CPPUNIT_ASSERT(wp04 != wp02);
      CPPUNIT_ASSERT(wp04 != wp03);
      ++numWaypoints;


      //lets try to do some lookups
      AIPluginInterface::WaypointArray wpArray;
      mAIInterface->GetWaypoints(wpArray);
      CPPUNIT_ASSERT(wpArray.size() == numWaypoints);
      
      CPPUNIT_ASSERT(mAIInterface->GetWaypointById(wp01->GetID()) == wp01);
      CPPUNIT_ASSERT(mAIInterface->GetWaypointById(wp02->GetID()) == wp02);
      CPPUNIT_ASSERT(mAIInterface->GetWaypointById(wp03->GetID()) == wp03);
      CPPUNIT_ASSERT(mAIInterface->GetWaypointById(wp04->GetID()) == wp04);
      
      osg::Vec3 origin(0.0f, 0.0f, 0.0f);
      osg::Vec3 one_hundred(100.0f, 100.0f, 100.0f);
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 100.0f) == wp01);
      
      mAIInterface->MoveWaypoint(wp01, osg::Vec3(100.0f, 100.0f, 100.0f));
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 100.0f) == wp02);
      
      mAIInterface->MoveWaypoint(wp02, osg::Vec3(101.0f, 101.0f, 101.0f));
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 100.0f) == wp03);
      
      mAIInterface->MoveWaypoint(wp03, osg::Vec3(102.0f, 102.0f, 102.0f));
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 100.0f) == wp04);
      
      mAIInterface->MoveWaypoint(wp04, osg::Vec3(103.0f, 103.0f, 103.0f));
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(one_hundred, 1000.0f) == wp01);


      //try a few out of range
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 1.0f) == NULL);
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, -1.0f) == NULL);
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 0.0f) == NULL);
      CPPUNIT_ASSERT(mAIInterface->GetClosestWaypoint(origin, 75.0f) == NULL);


      //now search at radius
      wpArray.clear();
      bool found = mAIInterface->GetWaypointsAtRadius(one_hundred, 25.0f, wpArray);
      CPPUNIT_ASSERT(found);
      CPPUNIT_ASSERT(wpArray.size() == numWaypoints);

      wpArray.clear();
      found = mAIInterface->GetWaypointsAtRadius(origin, 250.0f, wpArray);
      CPPUNIT_ASSERT(found);
      CPPUNIT_ASSERT(wpArray.size() == numWaypoints);

      wpArray.clear();
      found = mAIInterface->GetWaypointsAtRadius(origin, 5.0f, wpArray);
      CPPUNIT_ASSERT(!found);
      CPPUNIT_ASSERT(wpArray.empty());


      
      //now try some silly stuff, see if any problems show up
      wpArray.clear();
      found = mAIInterface->GetWaypointsAtRadius(origin, 0.0f, wpArray);
      CPPUNIT_ASSERT(!found);

   }

   //most of these are currently being tested in waypointgraphtests.cpp,
   //and are placeholders here

   void AIInterfaceTests::TestAddRemoveEdge()
   {

   }

   void AIInterfaceTests::TestPathfinding()
   {

   }

   void AIInterfaceTests::TestLoadSave()
   {

   }

   void AIInterfaceTests::TestWaypointPropertyContainer()
   {

   }

   void AIInterfaceTests::TestMemory()
   {

   }
}
