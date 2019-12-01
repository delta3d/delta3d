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
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/shaderparamtexture2d.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamvec4.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/shaderparamoscillator.h>
#include <dtCore/system.h>
#include <dtCore/timer.h>

#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/io_utils>

const std::string TESTS_DIR = dtUtil::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"tests";
const std::string projectContext = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "data" + dtUtil::FileUtils::PATH_SEPARATOR + "ProjectContext";

///////////////////////////////////////////////////////////////////////////////
// TEST Wrapper for float timer - so I can call privates.
class TestShaderParameterFloatTimer : public dtCore::ShaderParamOscillator
{
public:
   TestShaderParameterFloatTimer() : ShaderParamOscillator("test")
   {
   }

   void PassThroughDoUpdate(float deltaTime) { DoShaderUpdate(deltaTime); }
};

///////////////////////////////////////////////////////////////////////////////
// TEST Wrapper for textures - so I can call privates/protected.
class TestShaderParamTexture2D : public dtCore::ShaderParamTexture2D
{
public:
   TestShaderParamTexture2D() : ShaderParamTexture2D("test")
   {
   }

   osg::Texture* TestGetTextureObject() { return GetTextureObject(); }
};


///////////////////////////////////////////////////////////////////////////////


class ShaderParameterTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ShaderParameterTests);
      CPPUNIT_TEST(TestTexture2DParameter);
      CPPUNIT_TEST(TestTexture2DParameterReverseOrder);
      CPPUNIT_TEST(TestTexture2DParameterShared);
      CPPUNIT_TEST(TestFloatParameter);
      CPPUNIT_TEST(TestFloatParameterShared);
      CPPUNIT_TEST(TestVec4Parameter);
      CPPUNIT_TEST(TestVec4ParameterShared);
      CPPUNIT_TEST(TestIntParameter);
      CPPUNIT_TEST(TestIntParameterShared);
      CPPUNIT_TEST(TestTimerFloatParameter);
      CPPUNIT_TEST(TestTimerFloatParameterShared);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

   protected:
      void TestTexture2DParameter();
      void TestTexture2DParameterReverseOrder();
      void TestTexture2DParameterShared();
      void TestFloatParameter();
      void TestFloatParameterShared();
      void TestVec4Parameter();
      void TestVec4ParameterShared();
      void TestIntParameter();
      void TestIntParameterShared();
      void TestTimerFloatParameter();
      void TestTimerFloatParameterShared();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ShaderParameterTests);

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::setUp()
{
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList() + ";" + projectContext);

   dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
   dtCore::System::GetInstance().Start();
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
      dtCore::RefPtr<dtCore::ShaderParamTexture2D> param =
         new dtCore::ShaderParamTexture2D("TestTexture2D");
      dtCore::RefPtr<osg::StateSet> stateSet = new osg::StateSet();

      CPPUNIT_ASSERT_MESSAGE("ShaderParamFloat should have a default of 'true' for shared.",
         param->IsShared());
      param->SetShared(false);

      CPPUNIT_ASSERT_MESSAGE("Parameter name should be TestTexture2D.",param->GetName() == "TestTexture2D");
      CPPUNIT_ASSERT_MESSAGE("Parameter type should be SAMPLER_2D.",
                             param->GetType() == dtCore::ShaderParameter::ParamType::SAMPLER_2D);

      param->SetTexture("Textures/detailmap.png");
      param->SetTextureUnit(2);
      param->SetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S,
                            dtCore::ShaderParamTexture::AddressMode::MIRROR);
      param->SetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T,
                            dtCore::ShaderParamTexture::AddressMode::MIRROR);

      CPPUNIT_ASSERT_MESSAGE("Texture source was wrong.",param->GetTexture() == "Textures/detailmap.png");
      CPPUNIT_ASSERT_MESSAGE("Texture unit was wrong.",param->GetTextureUnit() == 2);
      CPPUNIT_ASSERT_MESSAGE("Texture S axis address mode was wrong.",
                             param->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S) ==
                                   dtCore::ShaderParamTexture::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture T axis address mode was wrong.",
                             param->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T) ==
                                   dtCore::ShaderParamTexture::AddressMode::MIRROR);

      //Bind the parameter to the render state and verify that the proper attributes were set.
      param->AttachToRenderState(*stateSet);
      osg::Texture2D* tex2D = dynamic_cast<osg::Texture2D*>(stateSet->getTextureAttribute(2,osg::StateAttribute::TEXTURE));
      osg::Uniform* texUniform = stateSet->getUniform(param->GetName());

      CPPUNIT_ASSERT_MESSAGE("There was no 2D texture attribute on the render state.",tex2D != NULL);
      CPPUNIT_ASSERT_MESSAGE("There was no texture uniform on the render state.",texUniform != NULL);
      CPPUNIT_ASSERT_MESSAGE("S axis texture addressing mode was wrong.",tex2D->getWrap(osg::Texture::WRAP_S) == osg::Texture::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("T axis texture addressing mode was wrong.",tex2D->getWrap(osg::Texture::WRAP_T) == osg::Texture::MIRROR);

      int value;
      texUniform->get(value);
      CPPUNIT_ASSERT_MESSAGE("Uniform should have an integer value of 2.",value == 2);

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamTexture2D> clonedParam =
         static_cast<dtCore::ShaderParamTexture2D*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be a different pointer.",
         clonedParam != param);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for texture unit should be the same.",
         clonedParam->GetTextureUnit(), param->GetTextureUnit());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for texture should be the same.",
         clonedParam->GetTexture(), param->GetTexture());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for address mode S should be the same.",
         clonedParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S),
         param->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for address mode T should be the same.",
         clonedParam->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T),
         param->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T));

      // Test Detach
      param->DetachFromRenderState(*stateSet);
      texUniform = stateSet->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Uniform should go away when detached from stateset.", texUniform == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestTexture2DParameterReverseOrder()
{
   try
   {
      // Basically the same test as above but this one checks that we can load our image
      // After we set our address modes and it should still work right.

      dtCore::RefPtr<dtCore::ShaderParamTexture2D> param =
         new dtCore::ShaderParamTexture2D("TestTexture2D");
      dtCore::RefPtr<osg::StateSet> stateSet = new osg::StateSet();

      param->SetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S,
         dtCore::ShaderParamTexture::AddressMode::MIRROR);
      param->SetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T,
         dtCore::ShaderParamTexture::AddressMode::MIRROR);
      param->SetTexture("Textures/detailmap.png");
      param->SetTextureUnit(2);

      CPPUNIT_ASSERT_MESSAGE("Texture S axis address mode was wrong.",
         param->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S) ==
         dtCore::ShaderParamTexture::AddressMode::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("Texture T axis address mode was wrong.",
         param->GetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T) ==
         dtCore::ShaderParamTexture::AddressMode::MIRROR);

      //Bind the parameter to the render state and verify that the proper attributes were set.
      param->AttachToRenderState(*stateSet);
      osg::Texture2D* tex2D = dynamic_cast<osg::Texture2D*>(stateSet->getTextureAttribute(2,osg::StateAttribute::TEXTURE));
      osg::Uniform* texUniform = stateSet->getUniform(param->GetName());

      CPPUNIT_ASSERT_MESSAGE("There was no 2D texture attribute on the render state.",tex2D != NULL);
      CPPUNIT_ASSERT_MESSAGE("There was no texture uniform on the render state.",texUniform != NULL);
      CPPUNIT_ASSERT_MESSAGE("S axis texture addressing mode was wrong.",tex2D->getWrap(osg::Texture::WRAP_S) == osg::Texture::MIRROR);
      CPPUNIT_ASSERT_MESSAGE("T axis texture addressing mode was wrong.",tex2D->getWrap(osg::Texture::WRAP_T) == osg::Texture::MIRROR);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestTexture2DParameterShared()
{
   try
   {
      dtCore::RefPtr<TestShaderParamTexture2D> param = new TestShaderParamTexture2D();
      dtCore::RefPtr<osg::StateSet> ss1 = new osg::StateSet();
      dtCore::RefPtr<osg::StateSet> ss2 = new osg::StateSet();

      // TEST SHARED BEHAVIOR
      param->SetTexture("Textures/detailmap.png");
      param->SetTextureUnit(2);
      param->SetAddressMode(dtCore::ShaderParamTexture::TextureAxis::S,
         dtCore::ShaderParamTexture::AddressMode::MIRROR);
      param->SetAddressMode(dtCore::ShaderParamTexture::TextureAxis::T,
         dtCore::ShaderParamTexture::AddressMode::MIRROR);
      param->SetShared(true);
      param->AttachToRenderState(*ss1);
      osg::Texture2D* tex2D1 = static_cast<osg::Texture2D*>(param->TestGetTextureObject());
      osg::Image* image1 = tex2D1->getImage();
      osg::Uniform* uniform1 = ss1->getUniform(param->GetName());

      // Test clone behavior
      dtCore::RefPtr<TestShaderParamTexture2D> clonedParam =
         static_cast<TestShaderParamTexture2D*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be the same when shared.",
         clonedParam == param);
      clonedParam->AttachToRenderState(*ss2);
      osg::Texture2D* tex2D2 = static_cast<osg::Texture2D*>(clonedParam->TestGetTextureObject());
      osg::Image* image2 = tex2D2->getImage();
      osg::Uniform* uniform2 = ss2->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Cloned uniforms should be the same when shared.",
         uniform2 == uniform1);
      // Want to make sure the image and textures did not get reloaded during the clone or 2nd attach
      CPPUNIT_ASSERT_MESSAGE("Cloned image should not have changed when shared.",
         image1 == image2);
      CPPUNIT_ASSERT_MESSAGE("Cloned textures should not have changed when shared.",
         tex2D1 == tex2D2);

      // Test Detach
      param->DetachFromRenderState(*ss1);
      osg::Texture2D* tex2D3 = static_cast<osg::Texture2D*>(clonedParam->TestGetTextureObject());
      osg::Image* image3 = tex2D3->getImage();
      uniform1 = ss1->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Uniform should go away when detached from stateset.", uniform1 == NULL);
      CPPUNIT_ASSERT_MESSAGE("Image should not have changed on a shared param when another guy is detached",
         image3 == image2);
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
      dtCore::RefPtr<dtCore::ShaderParamFloat> param = new dtCore::ShaderParamFloat("test");
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

      CPPUNIT_ASSERT_MESSAGE("ShaderParamFloat should have a default of 'false' for shared.",
         !param->IsShared());
      // test setting just once for all the params
      param->SetShared(true);
      CPPUNIT_ASSERT_EQUAL(true,param->IsShared());
      param->SetShared(false);

      param->SetValue(101.0f);
      CPPUNIT_ASSERT_EQUAL(std::string("test"),param->GetName());
      CPPUNIT_ASSERT_EQUAL(101.0f,param->GetValue());

      param->AttachToRenderState(*ss);
      osg::Uniform* uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::FLOAT,uniform->getType());

      float value;
      uniform->get(value);
      CPPUNIT_ASSERT_EQUAL(101.0f,value);

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamFloat> clonedParam =
         static_cast<dtCore::ShaderParamFloat*>(param->Clone());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetValue(), param->GetValue());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be a different pointer.",
         clonedParam != param);

      // Test Detach
      param->DetachFromRenderState(*ss);
      uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Uniform should go away when detached from stateset.", uniform == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestFloatParameterShared()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderParamFloat> param = new dtCore::ShaderParamFloat("test");
      dtCore::RefPtr<osg::StateSet> ss1 = new osg::StateSet();
      dtCore::RefPtr<osg::StateSet> ss2 = new osg::StateSet();

      // TEST SHARED BEHAVIOR
      param->SetShared(true);
      param->AttachToRenderState(*ss1);
      osg::Uniform* uniform1 = ss1->getUniform(param->GetName());

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamFloat> clonedParam =
         static_cast<dtCore::ShaderParamFloat*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be the same when shared.",
         clonedParam == param);
      clonedParam->AttachToRenderState(*ss2);
      osg::Uniform* uniform2 = ss2->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Cloned uniforms should be the same when shared.",
         uniform2 == uniform1);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestVec4Parameter()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderParamVec4> param = new dtCore::ShaderParamVec4("test");
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

      CPPUNIT_ASSERT_MESSAGE("ShaderParamVec4 should have a default of 'false' for shared.",
         !param->IsShared());
      // test setting just once for all the params
      param->SetShared(true);
      CPPUNIT_ASSERT_EQUAL(true, param->IsShared());
      param->SetShared(false);

      osg::Vec4 testVec4(2.3f, 338.9f, 83.8f, 93.9f);
      param->SetValue(testVec4);
      CPPUNIT_ASSERT_EQUAL(std::string("test"), param->GetName());
      CPPUNIT_ASSERT_EQUAL(testVec4, param->GetValue());

      param->AttachToRenderState(*ss);
      osg::Uniform* uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::FLOAT_VEC4,uniform->getType());

      osg::Vec4 value;
      uniform->get(value);
      CPPUNIT_ASSERT_EQUAL(testVec4,value);

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamVec4> clonedParam =
         static_cast<dtCore::ShaderParamVec4*>(param->Clone());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetValue(), param->GetValue());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be a different pointer.",
         clonedParam != param);

      // Test Detach
      param->DetachFromRenderState(*ss);
      uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Uniform should go away when detached from stateset.", uniform == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestVec4ParameterShared()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderParamVec4> param = new dtCore::ShaderParamVec4("test");
      dtCore::RefPtr<osg::StateSet> ss1 = new osg::StateSet();
      dtCore::RefPtr<osg::StateSet> ss2 = new osg::StateSet();

      // TEST SHARED BEHAVIOR
      param->SetShared(true);
      param->AttachToRenderState(*ss1);
      osg::Uniform* uniform1 = ss1->getUniform(param->GetName());

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamVec4> clonedParam =
         static_cast<dtCore::ShaderParamVec4*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be the same when shared.",
         clonedParam == param);
      clonedParam->AttachToRenderState(*ss2);
      osg::Uniform* uniform2 = ss2->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Cloned uniforms should be the same when shared.",
         uniform2 == uniform1);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestIntParameter()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderParamInt> param = new dtCore::ShaderParamInt("test");
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

      CPPUNIT_ASSERT_MESSAGE("ShaderParamInt should have a default of 'false' for shared.",
         !param->IsShared());

      param->SetValue(25);
      CPPUNIT_ASSERT_EQUAL(std::string("test"),param->GetName());
      CPPUNIT_ASSERT_EQUAL(25,param->GetValue());

      param->AttachToRenderState(*ss);
      osg::Uniform* uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::INT,uniform->getType());

      int value;
      uniform->get(value);
      CPPUNIT_ASSERT_EQUAL(25,value);

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamInt> clonedParam =
         static_cast<dtCore::ShaderParamInt*>(param->Clone());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetValue(), param->GetValue());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be a different pointer.",
         clonedParam != param);

      // Test Detach
      param->DetachFromRenderState(*ss);
      uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Uniform should go away when detached from stateset.", uniform == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestIntParameterShared()
{
   try
   {
      dtCore::RefPtr<dtCore::ShaderParamInt> param = new dtCore::ShaderParamInt("test");
      dtCore::RefPtr<osg::StateSet> ss1 = new osg::StateSet();
      dtCore::RefPtr<osg::StateSet> ss2 = new osg::StateSet();

      // TEST SHARED BEHAVIOR
      param->SetShared(true);
      param->AttachToRenderState(*ss1);
      osg::Uniform* uniform1 = ss1->getUniform(param->GetName());

      // Test clone behavior
      dtCore::RefPtr<dtCore::ShaderParamInt> clonedParam =
         static_cast<dtCore::ShaderParamInt*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be the same when shared.",
         clonedParam == param);
      clonedParam->AttachToRenderState(*ss2);
      osg::Uniform* uniform2 = ss2->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Cloned uniforms should be the same when shared.",
         uniform2 == uniform1);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}


///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestTimerFloatParameter()
{
   try
   {
      dtCore::RefPtr<TestShaderParameterFloatTimer> param = new TestShaderParameterFloatTimer();
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

      CPPUNIT_ASSERT_MESSAGE("ShaderParameterFloatTimer should have a default of 'false' for shared.",
         !param->IsShared());

      CPPUNIT_ASSERT_EQUAL(std::string("test"),param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Timer should be a timer.",
         param->GetType() == dtCore::ShaderParameter::ParamType::TIMER_FLOAT);

      float temp, offset, startValue, stopValue;

      temp = param->GetValue();
      param->SetValue(55.43f);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Basic set test (Temporary)", 55.43f, param->GetValue());

      offset = 11.0;
      param->SetOffset(offset);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Offset should have been set", offset, param->GetOffset());

      param->SetRangeMin(3.0);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Range Min should have been set", 3.0f, param->GetRangeMin());

      param->SetRangeMax(4.0);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Range Max should have been set", 4.0f, param->GetRangeMax());

      param->SetCycleTimeMin(1.0);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cycle Min should have been set", 1.0f, param->GetCycleTimeMin());

      param->SetCycleTimeMax(2.0);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cycle Max should have been set", 2.0f, param->GetCycleTimeMax());

      bool tempBool = param->GetUseRealTime();
      param->SetUseRealTime(!tempBool);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Use Real Time should have been set",
         param->GetUseRealTime(), !tempBool);
      param->SetUseRealTime(false); // we want to use sim time below

      param->SetOscillationType(dtCore::ShaderParamOscillator::OscillationType::DOWN);
      CPPUNIT_ASSERT_MESSAGE("Oscillation Type should have been set",
         param->GetOscillationType() == dtCore::ShaderParamOscillator::OscillationType::DOWN);


      param->AttachToRenderState(*ss);
      osg::Uniform* uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::FLOAT,uniform->getType());

      // CHECK INITIAL CONDITIONS FROM FIRST Update()

      CPPUNIT_ASSERT_MESSAGE("Current Oscillation Type should have been set",
         param->GetOscillationType() == dtCore::ShaderParamOscillator::OscillationType::DOWN);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cycle Direction should be down", -1.0f, param->GetCycleDirection());
      CPPUNIT_ASSERT_MESSAGE("Current Range should be valid",
         (param->GetCurrentRange() >= 3.0 && param->GetCurrentRange() <= 4.0));
      CPPUNIT_ASSERT_MESSAGE("Current Cycle Time should be valid",
         (param->GetCurrentCycleTime() >= 1.0 && param->GetCurrentCycleTime() <= 2.0));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Initial Value should be set",
         param->GetValue(), param->GetCurrentRange() + param->GetOffset());

      float value;
      uniform->get(value);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("uniform should = parameter", param->GetValue(),value);

      ////////////////////////////
      // OSCILLATE - DOWN
      ///////////////////////////

      startValue = param->GetValue();
      stopValue = startValue - param->GetCurrentRange();

      // TICK ONCE
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0); // halfway down
      temp = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Value should be between top and bottom",
         (temp < startValue && temp > stopValue));
      // TICK AGAIN
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("Value should be less than last tick but more than top ",
         (param->GetValue() < temp && param->GetValue() > stopValue));
      // FINAL - should push past threshold
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 2.8);
      CPPUNIT_ASSERT_MESSAGE("Value should be pretty close to the top",
         (param->GetValue() > temp && param->GetValue() < startValue));

      ////////////////////////////
      // OSCILLATE - UP
      ////////////////////////////

      param->SetOscillationType(dtCore::ShaderParamOscillator::OscillationType::UP);
      param->SetValue(param->GetOffset());
      startValue = param->GetValue();
      stopValue = param->GetOffset() + param->GetCurrentRange();

      // TICK ONCE
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      temp = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Value should be between top and bottom",
         (temp > startValue && temp < stopValue));
      // TICK AGAIN
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("Value should be more than last tick but less than top ",
         (param->GetValue() > temp && param->GetValue() < stopValue));
      // FINAL - should push past threshold
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 2.8);
      CPPUNIT_ASSERT_MESSAGE("Value should be pretty close to the bottom.",
         (param->GetValue() < temp && param->GetValue() > startValue));

      ////////////////////////////
      // OSCILLATE - UPANDDOWN
      ////////////////////////////

      param->SetOscillationType(dtCore::ShaderParamOscillator::OscillationType::UPANDDOWN);
      param->SetValue(param->GetOffset());
      startValue = param->GetValue();
      stopValue = startValue + param->GetCurrentRange();

      // TICK ONCE
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      temp = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("UPandDown - Value should be between top and bottom",
         (temp > startValue && temp < stopValue));
      // TICK AGAIN
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("UpAndDown - Value should be more than last tick but less than top",
         (param->GetValue() > temp && param->GetValue() < stopValue));
      // TOP TICK - should be near top but should be descending now
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 2.8);
      CPPUNIT_ASSERT_MESSAGE("UpAndDown - Value should be pretty close to the top but going down",
         (param->GetValue() > temp && param->GetValue() < stopValue));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("UpAndDown - Should be descending", -1.0f, param->GetCycleDirection());
      // TICK AGAIN - GOING DOWN
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("UpAndDown - Value should be near the bottom",
         (param->GetValue() < temp && param->GetValue() > startValue));
      // FINAL TICK - SHOULD REVERSE
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("UpAndDown - Value should be near the bottom",
         (param->GetValue() < temp && param->GetValue() > startValue));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("UpAndDown - Should be ascending",  1.0f, param->GetCycleDirection());


      ///////////////////////////
      // OSCILLATE - DOWNANDUP
      ///////////////////////////

      param->SetOscillationType(dtCore::ShaderParamOscillator::OscillationType::DOWNANDUP);
      param->SetValue(param->GetOffset() + param->GetCurrentRange());
      startValue = param->GetValue();
      stopValue = startValue - param->GetCurrentRange();

      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      temp = param->GetValue();
      CPPUNIT_ASSERT_MESSAGE("DownAndUp - Value should be between top and bottom",
         (temp < startValue && temp > stopValue));
      // TICK AGAIN
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("DownAndUp - Value should be less than last tick but more than top",
         (param->GetValue() < temp && param->GetValue() > stopValue));
      // TOP TICK - should be near bottom but should be ascending now
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 2.8);
      CPPUNIT_ASSERT_MESSAGE("DownAndUp - Value should be pretty close to the bottom but going up",
         (param->GetValue() < temp && param->GetValue() > stopValue));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("DownAndUp - Should be ascending", 1.0f, param->GetCycleDirection());
      // TICK AGAIN - GOING UP
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("DownAndUp - Value should be near the top",
         (param->GetValue() > temp && param->GetValue() < startValue));
      // FINAL TICK - SHOULD REVERSE
      param->PassThroughDoUpdate(param->GetCurrentCycleTime() / 3.0);
      CPPUNIT_ASSERT_MESSAGE("DownAndUp - Value should be near the top",
         (param->GetValue() > temp && param->GetValue() < startValue));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("DownAndUp - Should be descending", -1.0f, param->GetCycleDirection());


      // CHECK THAT SYSTEM STEP IS WIRED IN
      temp = param->GetValue();
      dtCore::AppSleep(2);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("System Step should cause value to change.", temp != param->GetValue());


      // CHECK ERROR CHECKING
      param->SetRangeMin(-3.0); // min is 0.0, converts to 1.0
      param->SetRangeMax(0.5); // max should be >= min
      param->SetCycleTimeMin(-8.0); // min is 0.0, converts to 1.0
      param->SetCycleTimeMax(0.25); // max should be >= min
      param->Update();
      CPPUNIT_ASSERT_MESSAGE("Current Range should be 1.0", 1.0 == param->GetCurrentRange());
      CPPUNIT_ASSERT_MESSAGE("Range Min should be 1.0", 1.0 == param->GetRangeMin());
      CPPUNIT_ASSERT_MESSAGE("Range Max should be 1.0", 1.0 == param->GetRangeMax());
      CPPUNIT_ASSERT_MESSAGE("Current Cycle timer should be 1.0", 1.0 == param->GetCurrentCycleTime());
      CPPUNIT_ASSERT_MESSAGE("Cycle timer min should be 1.0", 1.0 == param->GetCycleTimeMin());
      CPPUNIT_ASSERT_MESSAGE("Cycle timer max should be 1.0", 1.0 == param->GetCycleTimeMax());

      // TEST CLONE BEHAVIOR
      dtCore::RefPtr<TestShaderParameterFloatTimer> clonedParam =
         static_cast<TestShaderParameterFloatTimer*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be a different pointer.",
         clonedParam != param);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned offset should be the same.",
         clonedParam->GetOffset(), param->GetOffset());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetRangeMin(), param->GetRangeMin());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetRangeMax(), param->GetRangeMax());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetCycleTimeMin(), param->GetCycleTimeMin());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetCycleTimeMax(), param->GetCycleTimeMax());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetUseRealTime(), param->GetUseRealTime());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.",
         clonedParam->GetOscillationType(), param->GetOscillationType());

      // Test Detach
      param->DetachFromRenderState(*ss);
      uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Uniform should go away when detached from stateset.", uniform == NULL);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::TestTimerFloatParameterShared()
{
   try
   {
      dtCore::RefPtr<TestShaderParameterFloatTimer> param = new TestShaderParameterFloatTimer();
      dtCore::RefPtr<osg::StateSet> ss1 = new osg::StateSet();
      dtCore::RefPtr<osg::StateSet> ss2 = new osg::StateSet();

      // TEST SHARED BEHAVIOR
      param->SetShared(true);
      param->AttachToRenderState(*ss1);
      osg::Uniform* uniform1 = ss1->getUniform(param->GetName());

      // Test clone behavior
      dtCore::RefPtr<TestShaderParameterFloatTimer> clonedParam =
         static_cast<TestShaderParameterFloatTimer*>(param->Clone());
      CPPUNIT_ASSERT_MESSAGE("Cloned param should be the same when shared.",
         clonedParam == param);
      clonedParam->AttachToRenderState(*ss2);
      osg::Uniform* uniform2 = ss2->getUniform(param->GetName());
      CPPUNIT_ASSERT_MESSAGE("Cloned uniforms should be the same when shared.",
         uniform2 == uniform1);
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}
