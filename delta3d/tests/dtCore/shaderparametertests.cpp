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
#include <dtUtil/fileutils.h>
#include <dtDAL/project.h>
#include <dtCore/texture2dshaderparameter.h>
#include <dtCore/floatshaderparameter.h>
#include <dtCore/integershaderparameter.h>
#include <dtCore/globals.h>

#include <osg/StateSet>
#include <osg/Texture2D>

const std::string TESTS_DIR = dtCore::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"tests";
const std::string projectContext = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtCore" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingProject";

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class ShaderParameterTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ShaderParameterTests);
      CPPUNIT_TEST(TestTexture2DParameter);
      CPPUNIT_TEST(TestFloatParameter);
      CPPUNIT_TEST(TestIntParameter);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

   protected:
      void TestTexture2DParameter();
      void TestFloatParameter();
      void TestIntParameter();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ShaderParameterTests);

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::setUp()
{
   dtCore::SetDataFilePathList( dtCore::GetDeltaDataPathList() );

   try
   {
      dtDAL::Project::GetInstance().SetContext(projectContext);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::tearDown()
{

}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestTexture2DParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::Texture2DShaderParameter> param =
         new dtCore::Texture2DShaderParameter("TestTexture2D");
      dtCore::RefPtr<osg::StateSet> stateSet = new osg::StateSet();

      CPPUNIT_ASSERT_MESSAGE("Parameter name should be TestTexture2D.",param->GetName() == "TestTexture2D");
      CPPUNIT_ASSERT_MESSAGE("Parameter type should be SAMPLER_2D.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);

      param->SetTexture("Textures/detailmap.png");
      param->SetTextureUnit(2);
      param->SetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S,
                            dtCore::TextureShaderParameter::AddressMode::MIRROR);
      param->SetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T,
                            dtCore::TextureShaderParameter::AddressMode::MIRROR);

      CPPUNIT_ASSERT_MESSAGE("Texture source was wrong.",param->GetTexture() == "Textures/detailmap.png");
      CPPUNIT_ASSERT_MESSAGE("Texture unit was wrong.",param->GetTextureUnit() == 2);
      CPPUNIT_ASSERT_MESSAGE("Texture S axis address mode was wrong.",
                             param->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S) ==
                                   dtCore::TextureShaderParameter::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture T axis address mode was wrong.",
                             param->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T) ==
                                   dtCore::TextureShaderParameter::AddressMode::MIRROR);

      //Bind the parameter to the render state and verify that the proper attributes were set.
      param->AttachToRenderState(*stateSet);
      osg::Texture2D *tex2D = dynamic_cast<osg::Texture2D*>(stateSet->getTextureAttribute(2,osg::StateAttribute::TEXTURE));
      osg::Uniform *texUniform = stateSet->getUniform(param->GetName());

      CPPUNIT_ASSERT_MESSAGE("There was no 2D texture attribute on the render state.",tex2D != NULL);
      CPPUNIT_ASSERT_MESSAGE("There was no texture uniform on the render state.",texUniform != NULL);
      CPPUNIT_ASSERT_MESSAGE("S axis texture addressing mode was wrong.",tex2D->getWrap(osg::Texture::WRAP_S) == osg::Texture::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("T axis texture addressing mode was wrong.",tex2D->getWrap(osg::Texture::WRAP_T) == osg::Texture::MIRROR);

      int value;
      texUniform->get(value);
      CPPUNIT_ASSERT_MESSAGE("Uniform should have an integer value of 2.",value == 2);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestFloatParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::FloatShaderParameter> param = new dtCore::FloatShaderParameter("test");
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

      param->SetValue(101.0f);
      CPPUNIT_ASSERT_EQUAL(std::string("test"),param->GetName());
      CPPUNIT_ASSERT_EQUAL(101.0f,param->GetValue());

      param->AttachToRenderState(*ss);
      osg::Uniform *uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::FLOAT,uniform->getType());

      float value;
      uniform->get(value);
      CPPUNIT_ASSERT_EQUAL(101.0f,value);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::IntegerShaderParameter> param = new dtCore::IntegerShaderParameter("test");
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

      param->SetValue(25);
      CPPUNIT_ASSERT_EQUAL(std::string("test"),param->GetName());
      CPPUNIT_ASSERT_EQUAL(25,param->GetValue());

      param->AttachToRenderState(*ss);
      osg::Uniform *uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::INT,uniform->getType());

      int value;
      uniform->get(value);
      CPPUNIT_ASSERT_EQUAL(25,value);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}
