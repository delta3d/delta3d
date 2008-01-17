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
* @author Matthew "w00by" Stokes June 21, 2007
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/nodecollector.h>
#include <dtCore/stateattributecollector.h>
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
#include <osg/Texture2D>

///used to test the state attribute collector
class  StateAttribCollectorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(StateAttribCollectorTests);
   CPPUNIT_TEST(TestModel);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestModel();

private:
   dtCore::RefPtr<dtCore::StateAttributeCollector> mStateCollector1;
   dtCore::RefPtr<dtCore::StateAttributeCollector> mStateCollector2;
   dtCore::RefPtr<dtCore::StateAttributeCollector> mStateCollector3;
   dtCore::RefPtr<dtCore::StateAttributeCollector> mStateCollector4;

   osg::Group* mGroup;
};

CPPUNIT_TEST_SUITE_REGISTRATION(StateAttribCollectorTests);

void StateAttribCollectorTests::setUp()
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////Load an Object that contains Nodes////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  

/*   dtCore::SetDataFilePathList(dtCore::GetDeltaRootPath() + "/tests/data/ProjectContext/");
   std::string loadFile = dtCore::FindFileInPathList("StaticMeshes/articulation_test.ive");
   CPPUNIT_ASSERT(!loadFile.empty());

   mObj = new dtCore::Object("Object");
   mObj->LoadFile(loadFile);
   mNodeCollector = new dtCore::NodeCollector(mObj->GetOSGNode(), dtCore::NodeCollector::AllNodeTypes); 
*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////End Load an Object that contains Nodes/////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

   mGroup = new osg::Group();
   mGroup->setName("group_01");
   
   osg::MatrixTransform* MaxTran01 = new osg::MatrixTransform();
   MaxTran01->setName("maxtran_01");

   osg::MatrixTransform* MaxTran02 = new osg::MatrixTransform();
   MaxTran02->setName("maxtran_02");

   osg::Geode* geode01 = new osg::Geode();
   geode01->setName("geode_01");

   osgSim::DOFTransform* trans01 = new osgSim::DOFTransform();
   trans01->setName("trans_01");

   osg::Switch* switch01 = new osg::Switch();
   switch01->setName("switch_01");

   osg::Geometry* geomet01 = new osg::Geometry();
   geomet01->setName("geomet_01");

   osg::StateSet* stateset01 = new osg::StateSet();
   stateset01->setName("stateset_01");

   osg::StateSet* stateset02 = new osg::StateSet();
   stateset02->setName("stateset_02");

   osg::StateSet* stateset03 = new osg::StateSet();
   stateset03->setName("stateset_03");

   osg::StateSet* stateset04 = new osg::StateSet();
   stateset04->setName("stateset_04");

   osg::Texture2D* text2d01 = new osg::Texture2D();
   text2d01->setName("text2d_01");

   osg::Texture2D* text2d02 = new osg::Texture2D();
   text2d02->setName("text2d_02");

   osg::Texture2D* text2d03 = new osg::Texture2D();
   text2d03->setName("text2d_03");

   osg::Program* program01 = new osg::Program();
   program01->setName("program_01");

   osg::Program* program02 = new osg::Program();
   program02->setName("program_02");

   osg::Material* material01 = new osg::Material();
   material01->setName("material_01");

   stateset01->setAttribute(program01, osg::StateAttribute::PROGRAM);
   stateset02->setAttribute(material01, osg::StateAttribute::MATERIAL);
   stateset03->setTextureAttribute(0, text2d01, osg::StateAttribute::TEXTURE);
   stateset03->setTextureAttribute(1, text2d02, osg::StateAttribute::TEXTURE);
   stateset03->setTextureAttribute(2, text2d03, osg::StateAttribute::TEXTURE);
   stateset04->setAttribute(program02, osg::StateAttribute::PROGRAM);


   mGroup->addChild(MaxTran01);
   MaxTran01->addChild(MaxTran02);
   MaxTran01->addChild(geode01);

   MaxTran02->addChild(trans01);
   MaxTran02->addChild(switch01);

   geode01->setStateSet(stateset04);
   geomet01->setStateSet(stateset03);
   geode01->addDrawable(geomet01);

   switch01->setStateSet(stateset02);
   trans01->setStateSet(stateset01);
   
   mStateCollector1 = new dtCore::StateAttributeCollector(mGroup, dtCore::StateAttributeCollector::MaterialFlag);
   mStateCollector2 = new dtCore::StateAttributeCollector(mGroup, dtCore::StateAttributeCollector::ProgramFlag);
   mStateCollector3 = new dtCore::StateAttributeCollector(mGroup, dtCore::StateAttributeCollector::TextureFlag);
   mStateCollector4 = new dtCore::StateAttributeCollector(mGroup, dtCore::StateAttributeCollector::AllAttributes);
}

void StateAttribCollectorTests::tearDown()
{
}

void StateAttribCollectorTests::TestModel()
{
   CPPUNIT_ASSERT_MESSAGE("material_01 was NOT found.", mStateCollector4->GetMaterial("material_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("Material was NOT Map Found", mStateCollector4->GetMaterialMap().empty() == false);

   CPPUNIT_ASSERT_MESSAGE("program_01 was NOT found.", mStateCollector4->GetProgram("program_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("program_02 was NOT found.", mStateCollector4->GetProgram("program_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("Program was NOT found.", mStateCollector4->GetProgramMap().empty() == false);

   CPPUNIT_ASSERT_MESSAGE("text2d_01 was NOT found.", mStateCollector4->GetTexture("text2d_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_02 was NOT found.", mStateCollector4->GetTexture("text2d_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_03 was NOT found.", mStateCollector4->GetTexture("text2d_03") != NULL);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was NOT found.", mStateCollector4->GetTextureMap().empty() == false);
//////
   CPPUNIT_ASSERT_MESSAGE("material_01 was found.", mStateCollector3->GetMaterial("material_01") == NULL);
   CPPUNIT_ASSERT_MESSAGE("Material Map was found.", mStateCollector3->GetMaterialMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("program_01 was found.", mStateCollector3->GetProgram("program_01") == NULL);
   CPPUNIT_ASSERT_MESSAGE("program_02 was found.", mStateCollector3->GetProgram("program_02") == NULL);
   CPPUNIT_ASSERT_MESSAGE("Program Map was found.", mStateCollector3->GetProgramMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("text2d_01 was NOT found.", mStateCollector3->GetTexture("text2d_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_02 was NOT found.", mStateCollector3->GetTexture("text2d_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_03 was NOT found.", mStateCollector3->GetTexture("text2d_03") != NULL);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was NOT found.", mStateCollector3->GetTextureMap().empty() == false);
/////
   CPPUNIT_ASSERT_MESSAGE("material_01 was found.", mStateCollector2->GetMaterial("material_01") == NULL);
   CPPUNIT_ASSERT_MESSAGE("Material Map was found.", mStateCollector2->GetMaterialMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("program_01 was NOT found.", mStateCollector2->GetProgram("program_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("program_02 was NOT found.", mStateCollector2->GetProgram("program_02") != NULL);
   CPPUNIT_ASSERT_MESSAGE("Program Map was NOT found.", mStateCollector2->GetProgramMap().empty() == false);

   CPPUNIT_ASSERT_MESSAGE("text2d_01 was found.", mStateCollector2->GetTexture("text2d_01") == NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_02 was found.", mStateCollector2->GetTexture("text2d_02") == NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_03 was found.", mStateCollector2->GetTexture("text2d_03") == NULL);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was found.", mStateCollector2->GetTextureMap().empty() == true);
////
   CPPUNIT_ASSERT_MESSAGE("material_01 was NOT found.", mStateCollector1->GetMaterial("material_01") != NULL);
   CPPUNIT_ASSERT_MESSAGE("Material Map was NOT found.", mStateCollector1->GetMaterialMap().empty() == false);

   CPPUNIT_ASSERT_MESSAGE("program_01 was found.", mStateCollector1->GetProgram("program_01") == NULL);
   CPPUNIT_ASSERT_MESSAGE("program_02 was found.", mStateCollector1->GetProgram("program_02") == NULL);
   CPPUNIT_ASSERT_MESSAGE("Program Map was found.", mStateCollector1->GetProgramMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("text2d_01 was found.", mStateCollector1->GetTexture("text2d_01") == NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_02 was found.", mStateCollector1->GetTexture("text2d_02") == NULL);
   CPPUNIT_ASSERT_MESSAGE("text2d_03 was found.", mStateCollector1->GetTexture("text2d_03") == NULL);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was found.", mStateCollector1->GetTextureMap().empty() == true);

   mStateCollector1->ClearAll(); 
   mStateCollector2->ClearAll(); 
   mStateCollector3->ClearAll(); 
   mStateCollector4->ClearAll(); 

   CPPUNIT_ASSERT_MESSAGE("Material Map was found.", mStateCollector1->GetMaterialMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Program Map was found.", mStateCollector1->GetProgramMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was found.", mStateCollector1->GetTextureMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("Material Map was found.", mStateCollector2->GetMaterialMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Program Map was found.", mStateCollector2->GetProgramMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was found.", mStateCollector2->GetTextureMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("Material Map was found.", mStateCollector3->GetMaterialMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Program Map was found.", mStateCollector3->GetProgramMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was found.", mStateCollector3->GetTextureMap().empty() == true);

   CPPUNIT_ASSERT_MESSAGE("Material Map was found.", mStateCollector4->GetMaterialMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Program Map was found.", mStateCollector4->GetProgramMap().empty() == true);
   CPPUNIT_ASSERT_MESSAGE("Texture Map was found.", mStateCollector4->GetTextureMap().empty() == true);
}
