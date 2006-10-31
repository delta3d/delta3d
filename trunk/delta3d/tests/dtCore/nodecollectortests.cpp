/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute
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
 * @author Erik Johnson 05/16/2006
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/nodecollector.h>
#include <dtCore/object.h>
#include <dtUtil/exception.h>
#include <osg/ref_ptr>
#include <dtCore/globals.h>

///used to test the DeltaWin functionality
class  NodeCollectorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(NodeCollectorTests);
      CPPUNIT_TEST(TestModel);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestModel();

private:
   dtCore::RefPtr<dtCore::NodeCollector>  mNodeCollector;
   dtCore::RefPtr<dtCore::Object>         mObj;
};

CPPUNIT_TEST_SUITE_REGISTRATION(NodeCollectorTests);

void NodeCollectorTests::setUp()
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaRootPath() + "/tests/data/ProjectContext/");
   std::string loadFile = dtCore::FindFileInPathList("StaticMeshes/articulation_test.ive");
   CPPUNIT_ASSERT(!loadFile.empty());

   mObj = new dtCore::Object("Object");
   mObj->LoadFile(loadFile);
   mNodeCollector = new dtCore::NodeCollector(mObj->GetOSGNode(), dtCore::NodeCollectorTypes::LOAD_ALL_NODE_TYPES);
}

void NodeCollectorTests::tearDown()
{
}

void NodeCollectorTests::TestModel()
{
   CPPUNIT_ASSERT_MESSAGE("DOF List didnt get filled in", mNodeCollector->GetDOFListSize() != 0);
   CPPUNIT_ASSERT_MESSAGE("Hot spot List didnt get filled in", mNodeCollector->GetHotSpotListSize() != 0);

   CPPUNIT_ASSERT_MESSAGE("DOF List didnt find sub 0", mNodeCollector->GetDOFAtPosition(0) != NULL);
   CPPUNIT_ASSERT_MESSAGE("Hot spot didnt find sub 0", mNodeCollector->GetHotSpotAtPosition(0) != NULL);
   
   CPPUNIT_ASSERT_MESSAGE("Hot spot didnt find hotspot_01", mNodeCollector->GetHotSpotByName("hotspot_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("DOF List didnt find dof_wheel_lt_02, should have been there!", mNodeCollector->GetDOFByName("dof_wheel_lt_02") != NULL);
}
