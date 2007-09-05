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
 * @edited Matthew stokes 06/18/2007
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/nodecollector.h>
#include <dtCore/object.h>
#include <dtUtil/exception.h>
#include <osg/ref_ptr>
#include <dtCore/globals.h>

#include <osg/Group>
#include <osgSim/DOFTransform>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osg/Material>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>

///used to test the DeltaWin functionality
class  NodeCollectorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(NodeCollectorTests);
   CPPUNIT_TEST(TestModel);
   CPPUNIT_TEST(TestNodeRemoval);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestModel();
   void TestNodeRemoval();

private:
   dtCore::RefPtr<dtCore::NodeCollector>  mNodeCollector;
   dtCore::RefPtr<dtCore::NodeCollector>  mNodeCollector2;
   dtCore::RefPtr<dtCore::NodeCollector>  mNodeCollector3;
   dtCore::RefPtr<dtCore::NodeCollector>  mNodeCollector4;
   dtCore::RefPtr<dtCore::NodeCollector>  mNodeCollector5;
   
   dtCore::RefPtr<dtCore::Object>         mObj;
   dtCore::RefPtr<osg::Group>             mTestTree;
   dtCore::RefPtr<osg::Geode>             mGeo;
};

CPPUNIT_TEST_SUITE_REGISTRATION(NodeCollectorTests);

void NodeCollectorTests::setUp()
{
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////Load an Object that contains Nodes////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  
   dtCore::SetDataFilePathList(dtCore::GetDeltaRootPath() + "/tests/data/ProjectContext/");
   std::string loadFile = dtCore::FindFileInPathList("StaticMeshes/articulation_test.ive");
   CPPUNIT_ASSERT(!loadFile.empty());

   mObj = new dtCore::Object("Object");
   mObj->LoadFile(loadFile);
   mNodeCollector = new dtCore::NodeCollector(mObj->GetOSGNode(), dtCore::NodeCollector::AllNodeTypes);
   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////End Load an Object that contains Nodes/////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Creation of a Group, DOFTransform, MatrixTransform, and Switch////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
   mTestTree = new osg::Group();
   mTestTree->setName("group_01");

   osg::Geode* geo01 = new osg::Geode();
   geo01->setName("geo_01");
   mTestTree->addChild(geo01);

   osgSim::DOFTransform* trans01 = new osgSim::DOFTransform();
   trans01->setName("trans_01");
   mTestTree->addChild(trans01); 

   osg::Switch* switch01 = new osg::Switch();
   switch01->setName("switch_01");  
   mTestTree->addChild(switch01);

   osg::MatrixTransform* matrix01 = new osg::MatrixTransform();
   matrix01->setName("matrix_01");
   trans01->addChild(matrix01);

   osg::MatrixTransform* matrix02 = new osg::MatrixTransform();
   matrix02->setName("matrix_02");
   trans01->addChild(matrix02);

   osgSim::DOFTransform* trans02 = new osgSim::DOFTransform();
   trans02->setName("trans_02");
   switch01->addChild(trans02); 

   osg::Geode* geo02 = new osg::Geode();
   geo02->setName("geo_02");
   matrix01->addChild(geo02);

   osg::Group* group02 = new osg::Group();
   group02->setName("group_02");
   matrix02->addChild(group02);

   osg::MatrixTransform* matrix03 = new osg::MatrixTransform();
   matrix03->setName("matrix_03");
   matrix02->addChild(matrix03);

   osg::Switch* switch02 = new osg::Switch();
   switch02->setName("switch_02");
   group02->addChild(switch02);

   mNodeCollector2 = new dtCore::NodeCollector(mTestTree.get(), dtCore::NodeCollector::AllNodeTypes);
   mNodeCollector3 = new dtCore::NodeCollector(mTestTree.get(), dtCore::NodeCollector::GroupFlag | dtCore::NodeCollector::DOFTransformFlag);
   mNodeCollector4 = new dtCore::NodeCollector(mTestTree.get(), dtCore::NodeCollector::SwitchFlag | dtCore::NodeCollector::MatrixTransformFlag);
   mNodeCollector5 = new dtCore::NodeCollector(mTestTree.get(), dtCore::NodeCollector::GeodeFlag);

}

void NodeCollectorTests::tearDown()
{
}

void NodeCollectorTests::TestModel()
{
     CPPUNIT_ASSERT_MESSAGE("Hot spot did NOT find hotspot_01", mNodeCollector->GetMatrixTransform("hotspot_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("DOF List did NOT find dof_wheel_lt_02, should have been there!", mNodeCollector->GetDOFTransform("dof_wheel_lt_02") != NULL);

     ////////////////////////////////////////////

     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector2->GetGroupNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector2->GetTransformNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_03") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector2->GetMatrixTransformNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector2->GetSwitchNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector2->GetGeodeNodeMap().empty() == false);

     mNodeCollector2->ClearAll();

     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector2->GetGroupNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector2->GetTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector2->GetMatrixTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector2->GetSwitchNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector2->GetGeodeNodeMap().empty() == true);

     mNodeCollector2->CollectNodes(mTestTree.get(), dtCore::NodeCollector::AllNodeTypes);

     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector2->GetGroupNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector2->GetTransformNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_03") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector2->GetMatrixTransformNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector2->GetSwitchNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector2->GetGeodeNodeMap().empty() == false);

     mNodeCollector2->ClearAll();

     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector2->GetGroupNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector2->GetTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector2->GetMatrixTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector2->GetSwitchNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector2->GetGeodeNodeMap().empty() == true);

     ////////////////////////////////////////////

     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector3->GetGroup("group_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector3->GetGroup("group_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector3->GetGroupNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector3->GetDOFTransform("trans_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector3->GetDOFTransform("trans_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector3->GetTransformNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector3->GetMatrixTransform("matrix_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector3->GetMatrixTransform("matrix_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector3->GetMatrixTransform("matrix_03") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector3->GetMatrixTransformNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector3->GetSwitch("switch_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector3->GetSwitch("switch_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector3->GetSwitchNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector3->GetGeode("geo_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector3->GetGeode("geo_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector3->GetGeodeNodeMap().empty() == true);

     //////////////////////////////////////////////

     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector4->GetGroup("group_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector4->GetGroup("group_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector4->GetGroupNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector4->GetDOFTransform("trans_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector4->GetDOFTransform("trans_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector4->GetTransformNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector4->GetMatrixTransform("matrix_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector4->GetMatrixTransform("matrix_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector4->GetMatrixTransform("matrix_03") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector4->GetMatrixTransformNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector4->GetSwitch("switch_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector4->GetSwitch("switch_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector4->GetSwitchNodeMap().empty() == false);

     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector4->GetGeode("geo_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector4->GetGeode("geo_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector4->GetGeodeNodeMap().empty() == true);

     //////////////////////////////////////////////////

     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector5->GetGroup("group_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector5->GetGroup("group_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector5->GetGroupNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector5->GetDOFTransform("trans_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector5->GetDOFTransform("trans_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector5->GetTransformNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector5->GetMatrixTransform("matrix_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector5->GetMatrixTransform("matrix_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector5->GetMatrixTransform("matrix_03") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector5->GetMatrixTransformNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector5->GetSwitch("switch_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector5->GetSwitch("switch_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector5->GetSwitchNodeMap().empty() == true);

     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector5->GetGeode("geo_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector5->GetGeode("geo_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector5->GetGeodeNodeMap().empty() == false);
     
}

void NodeCollectorTests::TestNodeRemoval()
{
   // Try removing nodes from unrelated mappings to ensure no other mappings are compromised.
   std::string curNodeName("group_02");
   mNodeCollector2->RemoveDOFTransform(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetGroup(curNodeName) != NULL );
   mNodeCollector2->RemoveMatrixTransform(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetGroup(curNodeName) != NULL );
   mNodeCollector2->RemoveSwitch(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetGroup(curNodeName) != NULL );
   mNodeCollector2->RemoveGeode(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetGroup(curNodeName) != NULL );
   mNodeCollector2->RemoveGroup(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetGroup(curNodeName) == NULL );

   // Ensure all other nodes still exist
   CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_03") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_02") != NULL);

   // Test removal of other types of nodes
   curNodeName = "trans_02";
   mNodeCollector2->RemoveDOFTransform(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetDOFTransform(curNodeName) == NULL );

   curNodeName = "matrix_02";
   mNodeCollector2->RemoveMatrixTransform(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetMatrixTransform(curNodeName) == NULL );

   curNodeName = "switch_02";
   mNodeCollector2->RemoveSwitch(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetSwitch(curNodeName) == NULL );

   curNodeName = "geo_02";
   mNodeCollector2->RemoveGeode(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetGeode(curNodeName) == NULL );

   // Again, ensure all other nodes still exist
   CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_03") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_01") != NULL);
}
