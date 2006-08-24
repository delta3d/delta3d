/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
//#include <dtDAL/project.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shader.h>
#include <dtCore/texture2dshaderparameter.h>
#include <dtCore/integershaderparameter.h>
#include <dtCore/floatshaderparameter.h>
#include <dtCore/globals.h>
#include <osg/Geode>

const std::string TESTS_DIR = dtCore::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"tests";
const std::string projectContext = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtCore" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingProject";
#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class ShaderManagerTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ShaderManagerTests);
      CPPUNIT_TEST(TestShaderGroups);
      CPPUNIT_TEST(TestShader);
      CPPUNIT_TEST(TestShaderManager);
      CPPUNIT_TEST(TestAssignShader);
      CPPUNIT_TEST(TestXMLParsing);
      CPPUNIT_TEST(TestTexture2DXMLParam);
      CPPUNIT_TEST(TestIntXMLParam);
      CPPUNIT_TEST(TestFloatXMLParam);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

   protected:
      void TestShaderGroups();
      void TestShader();
      void TestShaderManager();
      void TestAssignShader();
      void TestXMLParsing();
      void TestTexture2DXMLParam();
      void TestIntXMLParam();
      void TestFloatXMLParam();

   private:
      dtCore::ShaderManager *mShaderMgr;
      dtCore::Shader *mTestShader;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ShaderManagerTests);

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::setUp()
{
   try
   {
      mShaderMgr = &dtCore::ShaderManager::GetInstance();
      //dtDAL::Project::GetInstance().SetContext(projectContext);
      dtCore::SetDataFilePathList(projectContext);
      mShaderMgr->LoadShaderDefinitions("Shaders/TestShaderDefinitions.xml",false);

      dtCore::ShaderGroup *testGroup = mShaderMgr->FindShaderGroup("ParamsGroup");
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
      CPPUNIT_ASSERT_MESSAGE("Shader group name shoudl have been TestGroup",newGroup->GetName() == "TestGroup");
      CPPUNIT_ASSERT_MESSAGE("Should not be a default shader on a new group.",newGroup->GetDefaultShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader count should be 0 on new group.",newGroup->GetNumShaders() == 0);

      int i;
      dtCore::RefPtr<dtCore::Shader> shader = NULL;
      for (i=0; i<100; i++)
      {
         shader = new dtCore::Shader("Shader" + dtUtil::ToString(i));

         if (i == 10)
            newGroup->AddShader(*shader,true);
         else
            newGroup->AddShader(*shader);
      }

      CPPUNIT_ASSERT_MESSAGE("Should be 100 shaders in the shader group.",newGroup->GetNumShaders() == 100);
      shader = newGroup->FindShader("Shader10");
      CPPUNIT_ASSERT_MESSAGE("Could not find shader 10.",shader != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader10 should be the default shader.",shader.get() == newGroup->GetDefaultShader());

      newGroup->RemoveShader(*shader);
      newGroup->RemoveShader("Shader25");
      CPPUNIT_ASSERT_MESSAGE("Should be 98 shaders in the group.",newGroup->GetNumShaders() == 98);
      CPPUNIT_ASSERT_MESSAGE("The default shader should be the first shader.",
                             newGroup->GetDefaultShader()->GetName() == "Shader0");

      //Make sure we can't add shaders with the same name...
      try
      {
         shader = new dtCore::Shader("Shader5");
         newGroup->AddShader(*shader);
         CPPUNIT_FAIL("Should not be able to add shaders with duplicate names.");
      }
      catch (const dtUtil::Exception &) { }

      std::vector<dtCore::RefPtr<dtCore::Shader> > shaderList;
      newGroup->GetAllShaders(shaderList);
      CPPUNIT_ASSERT_MESSAGE("Should be 98 shaders in the group's shader list.",shaderList.size() == 98);

      newGroup->RemoveAllShaders();
      CPPUNIT_ASSERT_MESSAGE("Should have no shaders in group after clear.",newGroup->GetNumShaders() == 0);
      CPPUNIT_ASSERT_MESSAGE("Default shader should be NULL after clear.",newGroup->GetDefaultShader() == NULL);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestShader()
{
   try
   {
      dtCore::RefPtr<dtCore::Shader> newShader = new dtCore::Shader("shaderone");
      CPPUNIT_ASSERT_MESSAGE("Shader name should have been shaderone.", newShader->GetName() == "shaderone");
      CPPUNIT_ASSERT_MESSAGE("Shader fragment program should be NULL.",newShader->GetFragmentShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader vertex program should be NULL.",newShader->GetVertexShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader program should be NULL.",newShader->GetShaderProgram() == NULL);

      newShader->SetVertexShaderSource("Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_vert.glsl"),newShader->GetVertexShaderSource());

      newShader->SetFragmentShaderSource("Shaders/perpixel_lighting_detailmap_frag.glsl");
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_frag.glsl"),newShader->GetFragmentShaderSource());

      int i;
      dtCore::RefPtr<dtCore::ShaderParameter> shaderParam = NULL;
      for (i=0; i<100; i++)
      {
         shaderParam = new dtCore::Texture2DShaderParameter("Param" + dtUtil::ToString(i));
         newShader->AddParameter(*shaderParam);
      }

      //Make sure we can't add parameters with the same name...
      try
      {
         shaderParam = new dtCore::Texture2DShaderParameter("Param20");
         newShader->AddParameter(*shaderParam);
         CPPUNIT_FAIL("Should not be able to add shader parameters with duplicate names.");
      }
      catch (const dtUtil::Exception &) { }

      CPPUNIT_ASSERT_MESSAGE("Should be 100 parameters on the shader.",newShader->GetNumParameters() == 100);
      shaderParam = newShader->FindParameter("Param10");
      CPPUNIT_ASSERT_MESSAGE("Could not find Param10.",shaderParam != NULL);

      std::vector<dtCore::RefPtr<dtCore::ShaderParameter> > params;
      newShader->GetParameterList(params);
      CPPUNIT_ASSERT_MESSAGE("Getting the list of parameters returned an incorrect list.",params.size() == 100);

      newShader->RemoveParameter(*shaderParam);
      newShader->RemoveParameter("Param50");
      CPPUNIT_ASSERT_MESSAGE("Should only have 98 parameters in the shader.",newShader->GetNumParameters() == 98);

      newShader->Reset();
      CPPUNIT_ASSERT_MESSAGE("Should have no parameters after a reset.",newShader->GetNumParameters() == 0);
      CPPUNIT_ASSERT_MESSAGE("Should have no vertex source after a reset.",newShader->GetVertexShaderSource().empty());
      CPPUNIT_ASSERT_MESSAGE("Should have no fragment source after a reset.",newShader->GetFragmentShaderSource().empty());
      CPPUNIT_ASSERT_MESSAGE("Shader name should have been shaderone after reset.", newShader->GetName() == "shaderone");
      CPPUNIT_ASSERT_MESSAGE("Shader fragment program should be NULL after reset.",newShader->GetFragmentShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader vertex program should be NULL after reset.",newShader->GetVertexShader() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader program should be NULL after reset.",newShader->GetShaderProgram() == NULL);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestShaderManager()
{
   try
   {
      int i,j;
      dtCore::RefPtr<dtCore::ShaderGroup> group;
      dtCore::RefPtr<dtCore::Shader> shader;

      mShaderMgr->Clear();

      //Create 20 groups each containing 3 shaders.
      for (i=0; i<20; i++)
      {
         group = new dtCore::ShaderGroup("Group" + dtUtil::ToString(i));
         for (j=0; j<3; j++)
         {
            shader = new dtCore::Shader("Shader" + dtUtil::ToString(j));
            shader->SetVertexShaderSource("Shaders/perpixel_lighting_detailmap_vert.glsl");
            shader->SetFragmentShaderSource("Shaders/perpixel_lighting_detailmap_frag.glsl");
            shader->AddParameter(*(new dtCore::Texture2DShaderParameter("Param0")));
            shader->AddParameter(*(new dtCore::Texture2DShaderParameter("Param1")));
            shader->AddParameter(*(new dtCore::Texture2DShaderParameter("Param2")));
            group->AddShader(*shader);
         }

         mShaderMgr->AddShaderGroup(*group);
      }

      //Since all of the shaders above used the same vertex and fragment sources,
      //there should only be one entry in the program cache and all of the shaders
      //should be using the same compiled programs..
      CPPUNIT_ASSERT_EQUAL((unsigned int)1,mShaderMgr->GetShaderCacheSize());
      CPPUNIT_ASSERT_EQUAL((unsigned int)20,mShaderMgr->GetNumShaderGroups());
      CPPUNIT_ASSERT_EQUAL((unsigned int)60,mShaderMgr->GetNumShaders());

      shader = mShaderMgr->FindShader("Shader0","Group0");
      CPPUNIT_ASSERT_MESSAGE("Could not find shader 0 on group 0.",shader != NULL);

      const osg::Shader *vertexShader = shader->GetVertexShader();
      const osg::Shader *fragmentShader = shader->GetFragmentShader();
      const osg::Program *program = shader->GetShaderProgram();
      for (i=0; i<20; i++)
      {
         std::string name = "Group" + dtUtil::ToString(i);
         group = mShaderMgr->FindShaderGroup(name);
         CPPUNIT_ASSERT_MESSAGE("Could not find shader group: " + name,group != NULL);

         for (j=0; j<3; j++)
         {
            std::string shaderName = "Shader" + dtUtil::ToString(j);
            shader = mShaderMgr->FindShader(shaderName,name);
            CPPUNIT_ASSERT_MESSAGE("Could not find shader: " + shaderName + " in group: " + name,shader != NULL);

            CPPUNIT_ASSERT_MESSAGE("Vertex program should be shared.",shader->GetVertexShader() == vertexShader);
            CPPUNIT_ASSERT_MESSAGE("Fragment program should be shared.",shader->GetFragmentShader() == fragmentShader);
            CPPUNIT_ASSERT_MESSAGE("GLSL program should be shared.",shader->GetShaderProgram() == program);
         }
      }

      //Make sure we cannot add a group with a duplicate name.
      group = new dtCore::ShaderGroup("Group1");
      try
      {
         mShaderMgr->AddShaderGroup(*group);
         CPPUNIT_FAIL("Should not be allowed to add duplicate groups to the shader manager.");
      }
      catch (const dtUtil::Exception &) { }

      //Tests the removal of shader groups..
      mShaderMgr->RemoveShaderGroup("Group10");
      mShaderMgr->RemoveShaderGroup(*group);
      CPPUNIT_ASSERT_MESSAGE("Should only be 18 groups after removal.",mShaderMgr->GetNumShaderGroups() == 18);
      CPPUNIT_ASSERT_MESSAGE("Should be 54 shaders in the shader manager after removal.",
                             mShaderMgr->GetNumShaders() == 54);

      //Add another shader here that does not use the same shader source files.  This should increate
      //shader compiled shader cache by 1.
      shader = new dtCore::Shader("TestShader");
      shader->SetVertexShaderSource("Shaders/perpixel_lighting_one_directional_vert.glsl");
      shader->SetFragmentShaderSource("Shaders/perpixel_lighting_one_directional_frag.glsl");

      group = new dtCore::ShaderGroup("TestShaderGroup");
      group->AddShader(*shader);
      mShaderMgr->AddShaderGroup(*group);
      CPPUNIT_ASSERT_MESSAGE("Should be two entries in the shader program cache.",mShaderMgr->GetShaderCacheSize() == 2);

      std::vector<dtCore::RefPtr<dtCore::ShaderGroup> > groupList;
      mShaderMgr->GetAllShaderGroups(groupList);
      CPPUNIT_ASSERT_MESSAGE("Shader group list size should be 19.",groupList.size() == 19);

      mShaderMgr->Clear();
      CPPUNIT_ASSERT_MESSAGE("Should be no groups in the manager.",mShaderMgr->GetNumShaderGroups() == 0);
      CPPUNIT_ASSERT_MESSAGE("Should be no shaders in the manager.",mShaderMgr->GetNumShaders() == 0);
      CPPUNIT_ASSERT_MESSAGE("Compiled program cache should be clear.",mShaderMgr->GetShaderCacheSize() == 0);
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
      dtCore::RefPtr<dtCore::Shader> shader = new dtCore::Shader("TestShader");
      shader->SetVertexShaderSource("Shaders/perpixel_lighting_detailmap_vert.glsl");
      shader->SetFragmentShaderSource("Shaders/perpixel_lighting_detailmap_frag.glsl");

      //This code creates a shader parameter of each type and assigns the shader
      //to a node.  The shader parameter tests ensures that each parameter type
      //actually got properly bound to the node's render state.
      dtCore::RefPtr<dtCore::Texture2DShaderParameter> tex1Param =
            new dtCore::Texture2DShaderParameter("baseTexture");
      tex1Param->SetTexture("Textures/smoke.rgb");
      tex1Param->SetTextureUnit(0);
      shader->AddParameter(*tex1Param);

      dtCore::RefPtr<dtCore::Texture2DShaderParameter> tex2Param =
            new dtCore::Texture2DShaderParameter("detailTexture");
      tex2Param->SetTexture("Textures/detailmap.png");
      tex2Param->SetTextureUnit(1);
      shader->AddParameter(*tex2Param);

      //Add our test shader to a shader group and add the shader group to the manager.
      //This will cause the shader to get loaded.
      dtCore::ShaderGroup *group = new dtCore::ShaderGroup("TestGroup");
      group->AddShader(*shader);
      mShaderMgr->AddShaderGroup(*group);

      //Try assigning the shader to a scene graph node.
      dtCore::RefPtr<osg::Geode> geode = new osg::Geode();
      mShaderMgr->AssignShader(*shader,*geode);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestXMLParsing()
{
   try
   {
      CPPUNIT_ASSERT_EQUAL((unsigned int)3,mShaderMgr->GetNumShaderGroups());
      CPPUNIT_ASSERT_EQUAL((unsigned int)5,mShaderMgr->GetNumShaders());

      dtCore::ShaderGroup *group1 = mShaderMgr->FindShaderGroup("TestGroup1");
      CPPUNIT_ASSERT(group1 != NULL);

      dtCore::ShaderGroup *group2 = mShaderMgr->FindShaderGroup("TestGroup2");
      CPPUNIT_ASSERT(group2 != NULL);

      CPPUNIT_ASSERT_EQUAL((unsigned int)2,group1->GetNumShaders());
      CPPUNIT_ASSERT_EQUAL((unsigned int)2,group2->GetNumShaders());

      //CHECK GROUP ONE'S SHADERS...
      dtCore::Shader *shader1 = group1->FindShader("Default");
      dtCore::Shader *shader2 = group1->FindShader("ShaderTwo");

      //SHADER ONE...
      CPPUNIT_ASSERT(shader1 != NULL);
      CPPUNIT_ASSERT(shader2 != NULL);
      CPPUNIT_ASSERT(group1->GetDefaultShader() != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("Default"),shader1->GetName());
      CPPUNIT_ASSERT_EQUAL((unsigned int)1,shader1->GetNumParameters());
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_vert.glsl"),shader1->GetVertexShaderSource());
      CPPUNIT_ASSERT_EQUAL(std::string("Shaders/perpixel_lighting_detailmap_frag.glsl"),shader1->GetFragmentShaderSource());

      //SHADER TWO...
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should be named ShaderTwo.",shader2->GetName() == "ShaderTwo");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should have one parameter.",
                             shader2->GetNumParameters() == 1);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 had wrong vertex shader source.",
                             shader2->GetVertexShaderSource() == "Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 had wrong fragment shader source.",
                             shader2->GetFragmentShaderSource() == "Shaders/perpixel_lighting_detailmap_frag.glsl");

      dtCore::ShaderParameter *param;
      dtCore::Texture2DShaderParameter *texParam;
      param = shader2->FindParameter("baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should have had a parameter named baseTexture.",
                             param != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 1 should have had a 2D texture parameter.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);

      texParam = static_cast<dtCore::Texture2DShaderParameter*>(param);
      CPPUNIT_ASSERT_MESSAGE("Parameter should be named baseTexture.",texParam->GetName() == "baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Parameter source should have been image.",
                             texParam->GetTextureSourceType() == dtCore::TextureShaderParameter::TextureSourceType::IMAGE);
      CPPUNIT_ASSERT_MESSAGE("Parameter should have had image Textures/detailmap.png.",
                             texParam->GetTexture() == "Textures/detailmap.png");
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have repeat assigned to axis S.",
                             texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S) ==
                                   dtCore::TextureShaderParameter::AddressMode::REPEAT);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have repeat assigned to axis T.",
                             texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T) ==
                                   dtCore::TextureShaderParameter::AddressMode::REPEAT);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should be assigned to unit 1.",
                             texParam->GetTextureUnit() == 1);

      //CHECK GROUP TWO'S SHADERS...
      shader1 = group2->FindShader("ShaderOne");
      shader2 = group2->FindShader("ShaderTwo");

      //SHADER ONE...
      CPPUNIT_ASSERT_MESSAGE("Group 2 should have a shader named ShaderOne.",shader1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("Group 2 should have a shader named ShaderTwo.",shader2 != NULL);
      CPPUNIT_ASSERT_MESSAGE("Group 2 should not have a default shader.",group2->GetDefaultShader() == NULL);

      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should be named ShaderOne.",shader1->GetName() == "ShaderOne");
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should have one parameter.",
                             shader1->GetNumParameters() == 1);
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 had wrong vertex shader source.",
                             shader1->GetVertexShaderSource() == "Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 had wrong fragment shader source.",
                             shader1->GetFragmentShaderSource() == "Shaders/perpixel_lighting_detailmap_frag.glsl");

      param = shader1->FindParameter("baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should have had a parameter named baseTexture.",
                             param != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader 1 in Group 2 should have had a 2D texture parameter.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);

      texParam = static_cast<dtCore::Texture2DShaderParameter*>(param);
      CPPUNIT_ASSERT_MESSAGE("Parameter should be named baseTexture.",texParam->GetName() == "baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Parameter source should have been auto.",
                             texParam->GetTextureSourceType() == dtCore::TextureShaderParameter::TextureSourceType::AUTO);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have clamp assigned to axis S.",
                             texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S) ==
                                   dtCore::TextureShaderParameter::AddressMode::CLAMP);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have clamp assigned to axis T.",
                             texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T) ==
                                   dtCore::TextureShaderParameter::AddressMode::CLAMP);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should be assigned to unit 1.",
                             texParam->GetTextureUnit() == 1);

      //SHADER TWO...
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should be named ShaderTwo.",shader2->GetName() == "ShaderTwo");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should have one parameter.",
                             shader2->GetNumParameters() == 1);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 had wrong vertex shader source.",
                             shader2->GetVertexShaderSource() == "Shaders/perpixel_lighting_detailmap_vert.glsl");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 had wrong fragment shader source.",
                             shader2->GetFragmentShaderSource() == "Shaders/perpixel_lighting_detailmap_frag.glsl");

      param = shader2->FindParameter("baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should have had a parameter named baseTexture.",
                             param != NULL);
      CPPUNIT_ASSERT_MESSAGE("Shader 2 in Group 2 should have had a 2D texture parameter.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);

      texParam = static_cast<dtCore::Texture2DShaderParameter*>(param);
      CPPUNIT_ASSERT_MESSAGE("Parameter should be named baseTexture.",texParam->GetName() == "baseTexture");
      CPPUNIT_ASSERT_MESSAGE("Parameter source should have been image.",
                             texParam->GetTextureSourceType() == dtCore::TextureShaderParameter::TextureSourceType::IMAGE);
      CPPUNIT_ASSERT_MESSAGE("Parameter should have had image Textures/detailmap.png.",
                             texParam->GetTexture() == "Textures/detailmap.png");
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have mirror assigned to axis S.",
                             texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S) ==
                                   dtCore::TextureShaderParameter::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should have mirror assigned to axis T.",
                             texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T) ==
                                   dtCore::TextureShaderParameter::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture parameter should be assigned to unit 2.",
                             texParam->GetTextureUnit() == 2);
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
      dtCore::ShaderParameter *param = mTestShader->FindParameter("Texture2DParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::SAMPLER_2D,param->GetType());

      dtCore::Texture2DShaderParameter *texParam = static_cast<dtCore::Texture2DShaderParameter*>(param);
      CPPUNIT_ASSERT(texParam != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("Texture2DParam"),texParam->GetName());
      CPPUNIT_ASSERT_EQUAL(dtCore::TextureShaderParameter::TextureSourceType::IMAGE,texParam->GetTextureSourceType());
      CPPUNIT_ASSERT_EQUAL(std::string("Textures/detailmap.png"),texParam->GetTexture());
      CPPUNIT_ASSERT_EQUAL(dtCore::TextureShaderParameter::AddressMode::MIRROR,
                           texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S));
      CPPUNIT_ASSERT_EQUAL(dtCore::TextureShaderParameter::AddressMode::MIRROR,
                           texParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T));
      CPPUNIT_ASSERT_EQUAL((unsigned int)2,texParam->GetTextureUnit());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestIntXMLParam()
{
   try
   {
      dtCore::ShaderParameter *param = mTestShader->FindParameter("IntParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::INT,param->GetType());

      dtCore::IntegerShaderParameter *intParam = static_cast<dtCore::IntegerShaderParameter*>(param);
      CPPUNIT_ASSERT(intParam != NULL);
      CPPUNIT_ASSERT_EQUAL(25,intParam->GetValue());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderManagerTests::TestFloatXMLParam()
{
   try
   {
      dtCore::ShaderParameter *param = mTestShader->FindParameter("FloatParam");
      CPPUNIT_ASSERT(param != NULL);
      CPPUNIT_ASSERT_EQUAL(dtCore::ShaderParameter::ParamType::FLOAT,param->GetType());

      dtCore::FloatShaderParameter *floatParam = static_cast<dtCore::FloatShaderParameter*>(param);
      CPPUNIT_ASSERT(floatParam != NULL);
      CPPUNIT_ASSERT_EQUAL(10.0f,floatParam->GetValue());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


