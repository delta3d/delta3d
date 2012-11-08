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
 * @author Erik Johnson 05/16/2006
 * @edited Matthew stokes 06/18/2007
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/object.h>
#include <dtUtil/exception.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/nodecollector.h>
#include <osg/ref_ptr>

#include <osg/Group>
#include <osgSim/DOFTransform>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osg/Material>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/LOD>

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
   dtCore::RefPtr<dtUtil::NodeCollector>  mNodeCollector;
   dtCore::RefPtr<dtUtil::NodeCollector>  mNodeCollector2;
   dtCore::RefPtr<dtUtil::NodeCollector>  mNodeCollector3;
   dtCore::RefPtr<dtUtil::NodeCollector>  mNodeCollector4;
   dtCore::RefPtr<dtUtil::NodeCollector>  mNodeCollector5;

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

   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/tests/data/ProjectContext/");
   std::string loadFile = dtUtil::FindFileInPathList("StaticMeshes/articulation_test.ive");
   CPPUNIT_ASSERT(!loadFile.empty());

   mObj = new dtCore::Object("Object");
   mObj->LoadFile(loadFile);
   mNodeCollector = new dtUtil::NodeCollector(mObj->GetOSGNode(), dtUtil::NodeCollector::AllNodeTypes);

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

   osg::LOD* lod01 = new osg::LOD();
   lod01->setName("lod_01");
   switch01->addChild(lod01);

   osgSim::DOFTransform* trans02 = new osgSim::DOFTransform();
   trans02->setName("trans_02");
   lod01->addChild(trans02);

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

   osg::LOD* lod02 = new osg::LOD();
   lod02->setName("lod_02");
   group02->addChild(lod02);

   mNodeCollector2 = new dtUtil::NodeCollector(mTestTree.get(), dtUtil::NodeCollector::AllNodeTypes);
   mNodeCollector3 = new dtUtil::NodeCollector(mTestTree.get(), dtUtil::NodeCollector::GroupFlag | dtUtil::NodeCollector::DOFTransformFlag);
   mNodeCollector4 = new dtUtil::NodeCollector(mTestTree.get(), dtUtil::NodeCollector::SwitchFlag | dtUtil::NodeCollector::MatrixTransformFlag);
   mNodeCollector5 = new dtUtil::NodeCollector(mTestTree.get(), dtUtil::NodeCollector::GeodeFlag | dtUtil::NodeCollector::LODFlag);

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

     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector2->GetLODNodeMap().empty() == false);

     mNodeCollector2->ClearAll();

     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector2->GetGroupNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector2->GetTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector2->GetMatrixTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector2->GetSwitchNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector2->GetGeodeNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector2->GetLODNodeMap().empty() == true);

     mNodeCollector2->CollectNodes(mTestTree.get(), dtUtil::NodeCollector::AllNodeTypes);

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

     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector2->GetLODNodeMap().empty() == false);

     mNodeCollector2->ClearAll();

     CPPUNIT_ASSERT_MESSAGE("This is a Group MAP Problem", mNodeCollector2->GetGroupNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a DOF MAP Problem", mNodeCollector2->GetTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Matrix MAP Problem", mNodeCollector2->GetMatrixTransformNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Switch MAP Problem", mNodeCollector2->GetSwitchNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a Geode MAP Problem", mNodeCollector2->GetGeodeNodeMap().empty() == true);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector2->GetLODNodeMap().empty() == true);

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

     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector2->GetLODNodeMap().empty() == true);

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

     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_01") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_02") == NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector2->GetLODNodeMap().empty() == true);

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

     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector5->GetLOD("lod_01") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector5->GetLOD("lod_02") != NULL);
     CPPUNIT_ASSERT_MESSAGE("This is a LOD MAP Problem", mNodeCollector5->GetLODNodeMap().empty() == false);

}

void NodeCollectorTests::TestNodeRemoval()
{
   // Try removing nodes from unrelated mappings to ensure no other mappings are compromised.
   std::string curNodeName("group_02");
   mNodeCollector2->RemoveDOFTransform(curNodeName);
   CPPUNIT_ASSERT(mNodeCollector2->GetGroup(curNodeName) != NULL);
   mNodeCollector2->RemoveMatrixTransform(curNodeName);
   CPPUNIT_ASSERT(mNodeCollector2->GetGroup(curNodeName) != NULL);
   mNodeCollector2->RemoveSwitch(curNodeName);
   CPPUNIT_ASSERT(mNodeCollector2->GetGroup(curNodeName) != NULL);
   mNodeCollector2->RemoveGeode(curNodeName);
   CPPUNIT_ASSERT(mNodeCollector2->GetGroup(curNodeName) != NULL);
   mNodeCollector2->RemoveGroup(curNodeName);
   CPPUNIT_ASSERT(mNodeCollector2->GetGroup(curNodeName) == NULL);

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
   CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_02") != NULL);

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

   curNodeName = "lod_02";
   mNodeCollector2->RemoveLOD(curNodeName);
   CPPUNIT_ASSERT( mNodeCollector2->GetLOD(curNodeName) == NULL );

   // Again, ensure all other nodes still exist
   CPPUNIT_ASSERT_MESSAGE("This is a Group Problem", mNodeCollector2->GetGroup("group_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a DOF Problem", mNodeCollector2->GetDOFTransform("trans_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Matrix Problem", mNodeCollector2->GetMatrixTransform("matrix_03") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Switch Problem", mNodeCollector2->GetSwitch("switch_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a Geode Problem", mNodeCollector2->GetGeode("geo_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("This is a LOD Problem", mNodeCollector2->GetLOD("lod_01") != NULL);
}
