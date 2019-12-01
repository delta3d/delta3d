/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Matthew W. Campbell, Curtiss Murphy
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/exception.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>

#include <dtCore/timer.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shaderparamtexture2d.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamvec4.h>
#include <dtCore/shaderparamoscillator.h>
#include <osg/Geode>

const std::string TESTS_DIR = dtUtil::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"tests";
const std::string projectContext = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "data" + dtUtil::FileUtils::PATH_SEPARATOR + "ProjectContext";

class ShaderManagerTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ShaderManagerTests);
      CPPUNIT_TEST(TestShaderGroups);
      CPPUNIT_TEST(TestShader);
      CPPUNIT_TEST(TestShaderManager);
      CPPUNIT_TEST(TestAssignShader);
      CPPUNIT_TEST(TestPartialShaders);
      CPPUNIT_TEST(TestShaderInstancesAreUnique);
      CPPUNIT_TEST(TestXMLParsing);
      CPPUNIT_TEST(TestTexture2DXMLParam);
      CPPUNIT_TEST(TestIntXMLParam);
      CPPUNIT_TEST(TestFloatXMLParam);
      CPPUNIT_TEST(TestVec4XMLParam);
      CPPUNIT_TEST(TestFloatTimerXMLParam);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

   protected:
      void TestShaderGroups();
      void TestShader();
      void TestShaderManager();
      void TestAssignShader();
      void TestPartialShaders();
      void TestShaderInstancesAreUnique();
      void TestXMLParsing();
      void TestTexture2DXMLParam();
      void TestIntXMLParam();
      void TestFloatXMLParam();
      void TestVec4XMLParam();
      void TestFloatTimerXMLParam();

   private:
      dtCore::ShaderManager* mShaderMgr;
      dtCore::ShaderProgram* mTestShader;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ShaderManagerTests);

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::setUp()
{
   try
   {
      mShaderMgr =& dtCore::ShaderManager::GetInstance();
      //dtCore::Project::GetInstance().SetContext(projectContext);
      dtUtil::SetDataFilePathList(projectContext);
      mShaderMgr->LoadShaderDefinitions("Shaders/TestShaderDefinitions.xml", false);

      dtCore::ShaderGroup* testGroup = mShaderMgr->FindShaderGroupPrototype("ParamsGroup");
      CPPUNIT_ASSERT(testGroup != NULL);
      mTestShader = testGroup->FindShader("AllParamsShader");
      CPPUNIT_ASSERT(mTestShader != NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::tearDown()
{
   mShaderMgr->Clear();
   mShaderMgr = NULL;
   mTestShader = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestShaderGroups()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderGroup> newGroup = new dtCore::ShaderGroup("TestGroup");
      CPPUNIT_ASSERT_MESSAGE("Shader group name shoudl have been TestGroup", newGroup->GetName() == "TestGroup");
      CPPUNIT_ASSERT_MESSAGE("Should not be a default shader on a new group.", newGroup->GetDefaultShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader count should be 0 on new group.", newGroup->GetNumShaders() == 0);

      int i;
      dtCore::RefPtr<dtCore::ShaderProgram> shader = NULL;
      for (i=0; i<100; i++)
      {
         shader = new dtCore::ShaderProgram("Shader" + dtUtil::ToString(i));

         if (i == 10)
            newGroup->AddShader(*shader, true);
         else
            newGroup->AddShader(*shader);
      }

      CPPUNIT_ASSERT_MESSAGE("Should be 100 shaders in the shader group.", newGroup->GetNumShaders() == 100);
      shader = newGroup->FindShader("Shader10");
      CPPUNIT_ASSERT_MESSAGE("Could not find shader 10.", shader != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader10 should be the default shader.", shader.get() == newGroup->GetDefaultShader());

      newGroup->RemoveShader(*shader);
      newGroup->RemoveShader("Shader25");
      CPPUNIT_ASSERT_MESSAGE("Should be 98 shaders in the group.", newGroup->GetNumShaders() == 98);
      CPPUNIT_ASSERT_MESSAGE("The default shader should be the first shader.",
                             newGroup->GetDefaultShader()->GetName() == "Shader0");

      //Make sure we can't add shaders with the same name...
      try
      {
         shader = new dtCore::ShaderProgram("Shader5");
         newGroup->AddShader(*shader);
         CPPUNIT_FAIL("Should not be able to add shaders with duplicate names.");
      }
      catch (const dtUtil::Exception& ) { }

      std::vector<dtCore::RefPtr<dtCore::ShaderProgram> > shaderList;
      newGroup->GetAllShaders(shaderList);
      CPPUNIT_ASSERT_MESSAGE("Should be 98 shaders in the group's shader list.", shaderList.size() == 98);

      newGroup->RemoveAllShaders();
      CPPUNIT_ASSERT_MESSAGE("Should have no shaders in group after clear.", newGroup->GetNumShaders() == 0);
      CPPUNIT_ASSERT_MESSAGE("Default shader should be NULL after clear.", newGroup->GetDefaultShader() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestShader()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderProgram> newShader = new dtCore::ShaderProgram("shaderone");
      CPPUNIT_ASSERT_MESSAGE("Shader name should have been shaderone.", newShader->GetName() == "shaderone");
//      CPPUNIT_ASSERT_MESSAGE("Shader fragment program should be NULL.", newShader->GetFragmentShader() == NULL);
//      CPPUNIT_ASSERT_MESSAGE("Shader vertex program should be NULL.", newShader->GetVertexShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader program should be NULL.", newShader->GetShaderProgram() == NULL);

      newShader->AddGeometryShader("Shaders/pass_through_geom.glsl");
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/pass_through_geom.glsl"), *(newShader->GetGeometryShaders().begin()));

      newShader->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_vert.glsl"), *(newShader->GetVertexShaders().begin()));

      newShader->AddFragmentShader("Shaders/perpixel_lighting_detailmap_frag.glsl");
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_frag.glsl"), *(newShader->GetFragmentShaders().begin()));

      int i;
      dtCore::RefPtr<dtCore::ShaderParameter> shaderParam = NULL;
      for (i=0; i<100; i++)
      {
         shaderParam = new dtCore::ShaderParamTexture2D("Param" + dtUtil::ToString(i));
         newShader->AddParameter(*shaderParam);
      }

      //Make sure we can't add parameters with the same name...
      try
      {
         shaderParam = new dtCore::ShaderParamTexture2D("Param20");
         newShader->AddParameter(*shaderParam);
         CPPUNIT_FAIL("Should not be able to add shader parameters with duplicate names.");
      }
      catch (const dtUtil::Exception& ) { }

      CPPUNIT_ASSERT_MESSAGE("Should be 100 parameters on the shader.", newShader->GetNumParameters() == 100);
      shaderParam = newShader->FindParameter("Param10");
      CPPUNIT_ASSERT_MESSAGE("Could not find Param10.", shaderParam != NULL);

      std::vector<dtCore::RefPtr<dtCore::ShaderParameter> > params;
      newShader->GetParameterList(params);
      CPPUNIT_ASSERT_MESSAGE("Getting the list of parameters returned an incorrect list.", params.size() == 100);

      newShader->RemoveParameter(*shaderParam);
      newShader->RemoveParameter("Param50");
      CPPUNIT_ASSERT_MESSAGE("Should only have 98 parameters in the shader.", newShader->GetNumParameters() == 98);

      newShader->Reset();
      CPPUNIT_ASSERT_MESSAGE("Should have no parameters after a reset.", newShader->GetNumParameters() == 0);
      CPPUNIT_ASSERT_MESSAGE("Should have no geometry source after a reset.", newShader->GetGeometryShaders().empty());
      CPPUNIT_ASSERT_MESSAGE("Should have no vertex source after a reset.", newShader->GetVertexShaders().empty());
      CPPUNIT_ASSERT_MESSAGE("Should have no fragment source after a reset.", newShader->GetFragmentShaders().empty());
      CPPUNIT_ASSERT_MESSAGE("Shader name should have been shaderone after reset.", newShader->GetName() == "shaderone");
//      CPPUNIT_ASSERT_MESSAGE("Shader fragment program should be NULL after reset.", newShader->GetFragmentShader() == NULL);
//      CPPUNIT_ASSERT_MESSAGE("Shader vertex program should be NULL after reset.", newShader->GetVertexShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader program should be NULL after reset.", newShader->GetShaderProgram() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestShaderManager()
{
   try
   {
      int i, j;
      dtCore::RefPtr<dtCore::ShaderGroup> group;
      dtCore::RefPtr<dtCore::ShaderProgram> shader;

      mShaderMgr->Clear();

      //Create 20 groups each containing 3 shaders.
      for (i=0; i<20; i++)
      {
         group = new dtCore::ShaderGroup("Group" + dtUtil::ToString(i));
         for (j=0; j<3; j++)
         {
            shader = new dtCore::ShaderProgram("Shader" + dtUtil::ToString(j));
	        shader->AddGeometryShader("Shaders/pass_through_geom.glsl");
            shader->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");
            shader->AddFragmentShader("Shaders/perpixel_lighting_detailmap_frag.glsl");
            shader->AddParameter(*(new dtCore::ShaderParamTexture2D("Param0")));
            shader->AddParameter(*(new dtCore::ShaderParamTexture2D("Param1")));
            shader->AddParameter(*(new dtCore::ShaderParamTexture2D("Param2")));
            group->AddShader(*shader);
         }

         mShaderMgr->AddShaderGroupPrototype(*group);
      }

      //Since all of the shaders above used the same vertex and fragment sources,
      //there should only be one entry in the program cache and all of the shaders
      //should be using the same compiled programs..
      CPPUNIT_ASSERT_EQUAL((unsigned int)1, mShaderMgr->GetShaderCacheSize());
      CPPUNIT_ASSERT_EQUAL((unsigned int)20, mShaderMgr->GetNumShaderGroupPrototypes());
      CPPUNIT_ASSERT_EQUAL((unsigned int)60, mShaderMgr->GetNumShaderPrototypes());

      const dtCore::ShaderProgram* cShader = mShaderMgr->FindShaderPrototype("Shader0", "Group0");
      CPPUNIT_ASSERT_MESSAGE("Could not find shader 0 on group 0.", cShader != NULL);

//      const osg::Shader* vertexShader = cShader->GetVertexShader();
//      const osg::Shader* fragmentShader = cShader->GetFragmentShader();
      const osg::Program* program = cShader->GetShaderProgram();
      for (i=0; i<20; i++)
      {
         std::string name = "Group" + dtUtil::ToString(i);
         const dtCore::ShaderGroup* cgroup = mShaderMgr->FindShaderGroupPrototype(name);
         CPPUNIT_ASSERT_MESSAGE("Could not find shader group: " + name, cgroup != NULL);

         for (j=0; j<3; j++)
         {
            std::string shaderName = "Shader" + dtUtil::ToString(j);
            const dtCore::ShaderProgram* cShader = mShaderMgr->FindShaderPrototype(shaderName, name);
            CPPUNIT_ASSERT_MESSAGE("Could not find shader: " + shaderName + " in group: " + name, cShader != NULL);

//            CPPUNIT_ASSERT_MESSAGE("Vertex program should be shared.", cShader->GetVertexShader() == vertexShader);
//            CPPUNIT_ASSERT_MESSAGE("Fragment program should be shared.", cShader->GetFragmentShader() == fragmentShader);
            CPPUNIT_ASSERT_MESSAGE("GLSL program should be shared.", cShader->GetShaderProgram() == program);
         }
      }

      //Make sure we cannot add a group with a duplicate name.
      group = new dtCore::ShaderGroup("Group1");
      try
      {
         mShaderMgr->AddShaderGroupPrototype(*group);
         CPPUNIT_FAIL("Should not be allowed to add duplicate groups to the shader manager.");
      }
      catch (const dtUtil::Exception& ) { }

      //Tests the removal of shader groups..
      mShaderMgr->RemoveShaderGroupPrototype("Group10");
      mShaderMgr->RemoveShaderGroupPrototype(*group);
      CPPUNIT_ASSERT_MESSAGE("Should only be 18 groups after removal.", mShaderMgr->GetNumShaderGroupPrototypes() == 18);
      CPPUNIT_ASSERT_MESSAGE("Should be 54 shaders in the shader manager after removal.",
                             mShaderMgr->GetNumShaderPrototypes() == 54);

      //Add another shader here that does not use the same shader source files.  This should increate
      //shader compiled shader cache by 1.
      shader = new dtCore::ShaderProgram("TestShader");
      shader->AddVertexShader("Shaders/perpixel_lighting_one_directional_vert.glsl");
      shader->AddFragmentShader("Shaders/perpixel_lighting_one_directional_frag.glsl");

      group = new dtCore::ShaderGroup("TestShaderGroup");
      group->AddShader(*shader);
      mShaderMgr->AddShaderGroupPrototype(*group);
      CPPUNIT_ASSERT_MESSAGE("Should be two entries in the shader program cache.", mShaderMgr->GetShaderCacheSize() == 2);

      std::vector<dtCore::RefPtr<dtCore::ShaderGroup> > groupList;
      mShaderMgr->GetAllShaderGroupPrototypes(groupList);
      CPPUNIT_ASSERT_MESSAGE("Shader group list size should be 19.", groupList.size() == 19);

      mShaderMgr->Clear();
      CPPUNIT_ASSERT_MESSAGE("Should be no groups in the manager.", mShaderMgr->GetNumShaderGroupPrototypes() == 0);
      CPPUNIT_ASSERT_MESSAGE("Should be no shaders in the manager.", mShaderMgr->GetNumShaderPrototypes() == 0);
      CPPUNIT_ASSERT_MESSAGE("Compiled program cache should be clear.", mShaderMgr->GetShaderCacheSize() == 0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestAssignShader()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderProgram> shader = new dtCore::ShaderProgram("TestShader");
      shader->AddGeometryShader("Shaders/pass_through_geom.glsl");
      shader->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");
      shader->AddFragmentShader("Shaders/perpixel_lighting_detailmap_frag.glsl");

      //This code creates a shader parameter of each type and assigns the shader
      //to a node.  The shader parameter tests ensures that each parameter type
      //actually got properly bound to the node's render state.
      dtCore::RefPtr<dtCore::ShaderParamTexture2D> tex1Param =
            new dtCore::ShaderParamTexture2D("baseTexture");
      tex1Param->SetTexture("Textures/smoke.rgb");
      tex1Param->SetTextureUnit(0);
      shader->AddParameter(*tex1Param);

      dtCore::RefPtr<dtCore::ShaderParamTexture2D> tex2Param =
            new dtCore::ShaderParamTexture2D("detailTexture");
      tex2Param->SetTexture("Textures/detailmap.png");
      tex2Param->SetTextureUnit(1);
      shader->AddParameter(*tex2Param);

      //Add our test shader to a shader group and add the shader group to the manager.
      //This will cause the shader to get loaded.
      dtCore::ShaderGroup* group = new dtCore::ShaderGroup("TestGroup");
      group->AddShader(*shader);
      mShaderMgr->AddShaderGroupPrototype(*group);

      //Try assigning the shader to a scene graph node.
      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();
      mShaderMgr->AssignShaderFromPrototype(*shader, *geode);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestPartialShaders()
{
   // This method tests for shaders that may or may not have a vertex or fragment shader.
   try
   {
      dtCore::ShaderGroup* group = new dtCore::ShaderGroup("TestGroup");

      // CREATE THE SHADER WITHOUT A FRAGMENT SHADER

      dtCore::RefPtr<dtCore::ShaderProgram> shader1 = new dtCore::ShaderProgram("TestShader1");
      shader1->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");

      //This code creates a shader parameter of each type and assigns the shader
      //to a node.  The shader parameter tests ensures that each parameter type
      //actually got properly bound to the node's render state.
      dtCore::RefPtr<dtCore::ShaderParamTexture2D> tex1Param =
         new dtCore::ShaderParamTexture2D("baseTexture");
      tex1Param->SetTexture("Textures/smoke.rgb");
      tex1Param->SetTextureUnit(0);
      shader1->AddParameter(*tex1Param);
      //Add our test shader to a shader group
      group->AddShader(*shader1);

      // CREATE THE SHADER WITHOUT A VERTEX SHADER

      dtCore::RefPtr<dtCore::ShaderProgram> shader2 = new dtCore::ShaderProgram("TestShader2");
      //shader->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");
      shader2->AddFragmentShader("Shaders/perpixel_lighting_detailmap_frag.glsl");

      //This code creates a shader parameter of each type and assigns the shader
      //to a node.  The shader parameter tests ensures that each parameter type
      //actually got properly bound to the node's render state.
      dtCore::RefPtr<dtCore::ShaderParamTexture2D> tex2Param =
         new dtCore::ShaderParamTexture2D("baseTexture");
      tex2Param->SetTexture("Textures/smoke.rgb");
      tex2Param->SetTextureUnit(0);
      shader2->AddParameter(*tex2Param);
      //Add our test shader to a shader group
      group->AddShader(*shader2);

      // CREATE THE SHADER WITHOUT EITHER SHADER

      dtCore::RefPtr<dtCore::ShaderProgram> shader3 = new dtCore::ShaderProgram("TestShader3");
      //shader3->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");

      //This code creates a shader parameter of each type and assigns the shader
      //to a node.  The shader parameter tests ensures that each parameter type
      //actually got properly bound to the node's render state.
      dtCore::RefPtr<dtCore::ShaderParamTexture2D> tex3Param =
         new dtCore::ShaderParamTexture2D("baseTexture");
      tex3Param->SetTexture("Textures/smoke.rgb");
      tex3Param->SetTextureUnit(0);
      shader3->AddParameter(*tex3Param);
      //Add our test shader to a shader group
      group->AddShader(*shader3);

      // NOW TEST ALL 3.

      // Everything is setup, so add the group to the shader manager
      // This will cause the shader to get loaded.
      mShaderMgr->AddShaderGroupPrototype(*group);

      // TEST without the fragment by assigning the shader to a scene graph node.
      dtCore::RefPtr<osg::Geode> geode1 = new osg::Geode();
      mShaderMgr->AssignShaderFromPrototype(*shader1, *geode1);

      // TEST without the vertex by assigning the shader to a scene graph node.
      dtCore::RefPtr<osg::Geode> geode2 = new osg::Geode();
      mShaderMgr->AssignShaderFromPrototype(*shader2, *geode2);

      // TEST without EITHER by assigning the shader to a scene graph node.
      dtCore::RefPtr<osg::Geode> geode3 = new osg::Geode();
      mShaderMgr->AssignShaderFromPrototype(*shader3, *geode3);

      // Test that the programs are unique.
      CPPUNIT_ASSERT_MESSAGE("1With Vertex and without Vertex should be unique programs.",
         shader1->GetShaderProgram() != shader2->GetShaderProgram());
      CPPUNIT_ASSERT_MESSAGE("2With Vertex and without Vertex should be unique programs.",
         shader2->GetShaderProgram() != shader3->GetShaderProgram());
      CPPUNIT_ASSERT_MESSAGE("3With Vertex and without Vertex should be unique programs.",
         shader3->GetShaderProgram() != shader1->GetShaderProgram());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestShaderInstancesAreUnique()
{
   try
   {

      // This test makes sure that each node got their own instance of the shader
      // so that it can assign different parameter values to separate nodes

      dtCore::RefPtr<dtCore::ShaderProgram> shader = new dtCore::ShaderProgram("TestShader");
      shader->AddVertexShader("Shaders/perpixel_lighting_detailmap_vert.glsl");
      shader->AddFragmentShader("Shaders/perpixel_lighting_detailmap_frag.glsl");

      // We just use an int param for this test.  We aren't testing params in general, just that they
      // get their own copy.
      dtCore::RefPtr<dtCore::ShaderParamInt> intParam =
         new dtCore::ShaderParamInt("intTest");
      intParam->SetValue(29);
      shader->AddParameter(*intParam);

      //Add our test shader to a shader group and add the shader group to the manager.
      //This will cause the shader to get loaded.
      dtCore::ShaderGroup* group = new dtCore::ShaderGroup("TestGroup");
      group->AddShader(*shader);
      mShaderMgr->AddShaderGroupPrototype(*group);

      //Try assigning the shader to a scene graph node.
      dtCore::RefPtr<osg::Geode> geode1 = new osg::Geode();
      dtCore::RefPtr<osg::Geode> geode2 = new osg::Geode();
      //dtCore::RefPtr<osg::Geode> geode3 = new osg::Geode();

      dtCore::ShaderProgram* newShader1 = mShaderMgr->AssignShaderFromPrototype(*shader, *geode1);
      CPPUNIT_ASSERT_MESSAGE("Assign Shader should return the new shader instance", newShader1 != NULL);

      dtCore::ShaderProgram* newShader2 = mShaderMgr->AssignShaderFromPrototype(*shader, *geode2);
      CPPUNIT_ASSERT_MESSAGE("Assign Shader should return the new shader instance", newShader2 != NULL);

      dtCore::ShaderParameter* newParam1 = newShader1->FindParameter("intTest");
      CPPUNIT_ASSERT_MESSAGE("The new shader instance should have the int param", newParam1 != NULL);
      dtCore::ShaderParamInt* newIntParam1 = dynamic_cast<dtCore::ShaderParamInt* >(newParam1);
      CPPUNIT_ASSERT_MESSAGE("The new shader instance should be an int param", newIntParam1 != NULL);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("New int param should initially have the same value",
         29, newIntParam1->GetValue());
      newIntParam1->SetValue(-15);
      CPPUNIT_ASSERT_MESSAGE("New int param should NOT be the same as the template param",
         intParam->GetValue() != newIntParam1->GetValue());

      // Make sure the 2nd shader didn't get changed either.
      dtCore::ShaderParameter* newParam2 = newShader2->FindParameter("intTest");
      CPPUNIT_ASSERT_MESSAGE("The new shader instance should have the int param", newParam2 != NULL);
      dtCore::ShaderParamInt* newIntParam2 = dynamic_cast<dtCore::ShaderParamInt* >(newParam2);
      CPPUNIT_ASSERT_MESSAGE("The new shader instance should be an int param", newIntParam2 != NULL);
      CPPUNIT_ASSERT_MESSAGE("New int param should NOT be the same as the 2nd int param instance",
         newIntParam1->GetValue() != newIntParam2->GetValue());

      // Just to be safe, set the 2nd and make sure the 1st doesn't change
      newIntParam2->SetValue(99);
      CPPUNIT_ASSERT_MESSAGE("New int param 1 should be unaffected by setting the 2nd",
         newIntParam1->GetValue() == -15);
      CPPUNIT_ASSERT_MESSAGE("The original int param should be unaffected by setting the 2nd",
         intParam->GetValue() == 29);
      CPPUNIT_ASSERT_MESSAGE("The 2nd Int should have its new value",
         newIntParam2->GetValue() == 99);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestXMLParsing()
{
   try
   {
      CPPUNIT_ASSERT_EQUAL((unsigned int)4, mShaderMgr->GetNumShaderGroupPrototypes());
      CPPUNIT_ASSERT_EQUAL((unsigned int)6, mShaderMgr->GetNumShaderPrototypes());

      dtCore::ShaderGroup* group1 = mShaderMgr->FindShaderGroupPrototype("TestGroup1");
      CPPUNIT_ASSERT(group1 != NULL);

      dtCore::ShaderGroup* group2 = mShaderMgr->FindShaderGroupPrototype("TestGroup2");
      CPPUNIT_ASSERT(group2 != NULL);

	  dtCore::ShaderGroup* group3 = mShaderMgr->FindShaderGroupPrototype("TestGroup3");
	  CPPUNIT_ASSERT(group3 != NULL);

      CPPUNIT_ASSERT_EQUAL((unsigned int)2, group1->GetNumShaders());
	  CPPUNIT_ASSERT_EQUAL((unsigned int)2, group2->GetNumShaders());
	  CPPUNIT_ASSERT_EQUAL((unsigned int)1, group3->GetNumShaders());

      //CHECK GROUP ONE'S SHADERS...
      dtCore::ShaderProgram* shader1 = group1->FindShader("Default");
      dtCore::ShaderProgram* shader2 = group1->FindShader("ShaderTwo");

      //SHADER ONE...
      CPPUNIT_ASSERT(shader1 != NULL);
      CPPUNIT_ASSERT(shader2 != NULL);
      CPPUNIT_ASSERT(group1->GetDefaultShader() != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("Default"), shader1->GetName());
      CPPUNIT_ASSERT_EQUAL(1U, shader1->GetNumParameters());
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_vert.glsl"), *(shader1->GetVertexShaders().begin()));
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_frag.glsl"), *(shader1->GetFragmentShaders().begin()));

      //SHADER TWO...
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should be named ShaderTwo.", shader2->GetName() == "ShaderTwo");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should have one parameter.",
                             shader2->GetNumParameters() == 1);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 had wrong vertex shader source.",
                             *(shader2->GetVertexShaders().begin()) == "Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 had wrong fragment shader source.",
                             *(shader2->GetFragmentShaders().begin()) == "Shaders/perpixel_lighting_detailmap_frag.glsl");

      dtCore::ShaderParameter* param;
      dtCore::ShaderParamTexture2D* texParam;
      param = shader2->FindParameter("baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should have had a parameter named baseTexture.",
                             param != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should have had a 2D texture parameter.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);

      texParam = static_cast<dtCore::ShaderParamTexture2D*>(param);
      CPPUNIT_ASSERT_MESSAGE("Parameter should be named baseTexture.", texParam->GetName() == "baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Parameter source should have been image.",
                             texParam->GetTextureSourceType() == dtCore::ShaderParamTexture::TextureSourceType::IMAGE);
      CPPUNIT_ASSERT_MESSAGE("Parameter should have had image Textures/detailmap.png.",
                             texParam->GetTexture() == "Textures/detailmap.png");
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have repeat assigned to axis S.",
                             texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S) ==
                                   dtCore::ShaderParamTexture::AddressMode::REPEAT);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have repeat assigned to axis T.",
                             texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T) ==
                                   dtCore::ShaderParamTexture::AddressMode::REPEAT);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should be assigned to unit 1.",
                             texParam->GetTextureUnit() == 1);

      //CHECK GROUP TWO'S SHADERS...
      shader1 = group2->FindShader("ShaderOne");
      shader2 = group2->FindShader("ShaderTwo");

      //SHADER ONE...
      CPPUNIT_ASSERT_MESSAGE("Group 2 should have a shader named ShaderOne.", shader1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("Group 2 should have a shader named ShaderTwo.", shader2 != NULL);
      CPPUNIT_ASSERT_MESSAGE("Group 2 should not have a default shader.", group2->GetDefaultShader() == NULL);

      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should be named ShaderOne.", shader1->GetName() == "ShaderOne");
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should have one parameter.",
                             shader1->GetNumParameters() == 1);
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 had wrong vertex shader source.",
                            * (shader1->GetVertexShaders().begin()) == "Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 had wrong fragment shader source.",
                            * (shader1->GetFragmentShaders().begin()) == "Shaders/perpixel_lighting_detailmap_frag.glsl");

      param = shader1->FindParameter("baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should have had a parameter named baseTexture.",
                             param != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should have had a 2D texture parameter.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2, baseTexture should not be shared.", !param->IsShared());

      texParam = static_cast<dtCore::ShaderParamTexture2D*>(param);
      CPPUNIT_ASSERT_MESSAGE("Parameter should be named baseTexture.", texParam->GetName() == "baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Parameter source should have been auto.",
                             texParam->GetTextureSourceType() == dtCore::ShaderParamTexture::TextureSourceType::AUTO);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have clamp assigned to axis S.",
                             texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S) ==
                                   dtCore::ShaderParamTexture::AddressMode::CLAMP);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have clamp assigned to axis T.",
                             texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T) ==
                                   dtCore::ShaderParamTexture::AddressMode::CLAMP);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should be assigned to unit 1.",
                             texParam->GetTextureUnit() == 1);

      //SHADER TWO...
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should be named ShaderTwo.", shader2->GetName() == "ShaderTwo");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should have one parameter.",
                             shader2->GetNumParameters() == 1);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 had wrong vertex shader source.",
                             *(shader2->GetVertexShaders().begin()) == "Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 had wrong fragment shader source.",
                             *(shader2->GetFragmentShaders().begin()) == "Shaders/perpixel_lighting_detailmap_frag.glsl");

      param = shader2->FindParameter("baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should have had a parameter named baseTexture.",
                             param != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should have had a 2D texture parameter.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2, baseTexture should be shared.", param->IsShared());

      texParam = static_cast<dtCore::ShaderParamTexture2D*>(param);
      CPPUNIT_ASSERT_MESSAGE("Parameter should be named baseTexture.", texParam->GetName() == "baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Parameter source should have been image.",
                             texParam->GetTextureSourceType() == dtCore::ShaderParamTexture::TextureSourceType::IMAGE);
      CPPUNIT_ASSERT_MESSAGE("Parameter should have had image Textures/detailmap.png.",
                             texParam->GetTexture() == "Textures/detailmap.png");
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have mirror assigned to axis S.",
                             texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S) ==
                                   dtCore::ShaderParamTexture::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have mirror assigned to axis T.",
                             texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T) ==
                                   dtCore::ShaderParamTexture::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should be assigned to unit 2.",
                             texParam->GetTextureUnit() == 2);

	  //CHECK GROUP THREES'S SHADERS...
	  shader1 = group3->FindShader("ShaderOne");

	  //SHADER ONE...
	  CPPUNIT_ASSERT_MESSAGE("Group 3 should have a shader named ShaderOne.", shader1 != NULL);
	  CPPUNIT_ASSERT_MESSAGE("Group 3 should not have a default shader.", group3->GetDefaultShader() == NULL);

	  CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 3 should be named ShaderOne.", shader1->GetName() == "ShaderOne");

	  CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 3 had wrong geometry shader source.",
	     * (shader1->GetGeometryShaders().begin()) == "Shaders/pass_through_geom.glsl");
	  CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 3 had wrong vertex shader source.",
	     * (shader1->GetVertexShaders().begin()) == "Shaders/perpixel_lighting_detailmap_vert.glsl");
	  CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 3 had wrong fragment shader source.",
	     * (shader1->GetFragmentShaders().begin()) == "Shaders/perpixel_lighting_detailmap_frag.glsl");
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestTexture2DXMLParam()
{
   try
   {
      dtCore::ShaderParameter* param = mTestShader->FindParameter("Texture2DParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::SAMPLER_2D, param->GetType());

      dtCore::ShaderParamTexture2D* texParam = static_cast<dtCore::ShaderParamTexture2D*>(param);
      CPPUNIT_ASSERT(texParam != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("Texture2DParam"), texParam->GetName());
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParamTexture::TextureSourceType::IMAGE, texParam->GetTextureSourceType());
      CPPUNIT_ASSERT_EQUAL(std::string("Textures/detailmap.png"), texParam->GetTexture());
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParamTexture::AddressMode::MIRROR,
                           texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S));
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParamTexture::AddressMode::MIRROR,
                           texParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T));
      CPPUNIT_ASSERT_EQUAL((unsigned int)2, texParam->GetTextureUnit());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestIntXMLParam()
{
   try
   {
      dtCore::ShaderParameter* param = mTestShader->FindParameter("IntParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::INT, param->GetType());

      dtCore::ShaderParamInt* intParam = static_cast<dtCore::ShaderParamInt*>(param);
      CPPUNIT_ASSERT(intParam != NULL);
      CPPUNIT_ASSERT_EQUAL(25, intParam->GetValue());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestFloatXMLParam()
{
   try
   {
      dtCore::ShaderParameter* param = mTestShader->FindParameter("FloatParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::FLOAT, param->GetType());

      dtCore::ShaderParamFloat* floatParam = static_cast<dtCore::ShaderParamFloat*>(param);
      CPPUNIT_ASSERT(floatParam != NULL);
      CPPUNIT_ASSERT_EQUAL(10.0f, floatParam->GetValue());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestVec4XMLParam()
{
   try
   {
      dtCore::ShaderParameter* param = mTestShader->FindParameter("Vec4Param");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::FLOAT_VEC4, param->GetType());

      dtCore::ShaderParamVec4* vec4Param = static_cast<dtCore::ShaderParamVec4*>(param);
      CPPUNIT_ASSERT(vec4Param != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Vec4(10.0, 3.0, 5.0, 1.0), vec4Param->GetValue());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestFloatTimerXMLParam()
{
   try
   {
      dtCore::ShaderParameter* param = mTestShader->FindParameter("TimerFloatParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::TIMER_FLOAT, param->GetType());

      dtCore::ShaderParamOscillator* floatTimerParam = static_cast<dtCore::ShaderParamOscillator*>(param);
      CPPUNIT_ASSERT(floatTimerParam != NULL);
      CPPUNIT_ASSERT_EQUAL(4.0f, floatTimerParam->GetOffset());
      CPPUNIT_ASSERT_EQUAL(3.0f, floatTimerParam->GetRangeMin());
      CPPUNIT_ASSERT_EQUAL(4.0f, floatTimerParam->GetRangeMax());
      CPPUNIT_ASSERT_EQUAL(2.0f, floatTimerParam->GetCycleTimeMin());
      CPPUNIT_ASSERT_EQUAL(2.5f, floatTimerParam->GetCycleTimeMax());
      CPPUNIT_ASSERT_EQUAL(false, floatTimerParam->GetUseRealTime());
      CPPUNIT_ASSERT(dtCore::ShaderParamOscillator::OscillationType::UPANDDOWN == floatTimerParam->GetOscillationType());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


