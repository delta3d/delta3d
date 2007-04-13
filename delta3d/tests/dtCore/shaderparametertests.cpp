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
 * Matthew W. Campbell, Curtiss Murphy
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtCore/texture2dshaderparameter.h>
#include <dtCore/floatshaderparameter.h>
#include <dtCore/integershaderparameter.h>
#include <dtCore/ShaderParameterFloatTimer.h>
#include <dtCore/globals.h>
#include <dtCore/system.h>

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

///////////////////////////////////////////////////////////////////////////////
// TEST Wrapper for float timer - so I can call privates.
class TestShaderParameterFloatTimer : public dtCore::ShaderParameterFloatTimer
{
public:
   TestShaderParameterFloatTimer::TestShaderParameterFloatTimer() : ShaderParameterFloatTimer("test")
   {
   }

   void PassThroughDoUpdate(float deltaTime) { DoShaderUpdate(deltaTime); }
};
///////////////////////////////////////////////////////////////////////////////


class ShaderParameterTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ShaderParameterTests);
      CPPUNIT_TEST(TestTexture2DParameter);
      CPPUNIT_TEST(TestTexture2DParameterReverseOrder);
      CPPUNIT_TEST(TestFloatParameter);
      CPPUNIT_TEST(TestIntParameter);
      CPPUNIT_TEST(TestTimerFloatParameter);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

   protected:
      void TestTexture2DParameter();
      void TestTexture2DParameterReverseOrder();
      void TestFloatParameter();
      void TestIntParameter();
      void TestTimerFloatParameter();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ShaderParameterTests);

///////////////////////////////////////////////////////////////////////////////
void ShaderParameterTests::setUp()
{
   dtCore::SetDataFilePathList(  dtCore::GetDeltaDataPathList() + ";" + 
                                 projectContext );

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

      // Test clone behavior
      dtCore::RefPtr<dtCore::Texture2DShaderParameter> clonedParam = 
         static_cast<dtCore::Texture2DShaderParameter *>(param->Clone());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for texture unit should be the same.", 
         clonedParam->GetTextureUnit(), param->GetTextureUnit());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for texture should be the same.", 
         clonedParam->GetTexture(), param->GetTexture());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for address mode S should be the same.", 
         clonedParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S), 
         param->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value for address mode T should be the same.", 
         clonedParam->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T), 
         param->GetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T));

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

      dtCore::RefPtr<dtCore::Texture2DShaderParameter> param =
         new dtCore::Texture2DShaderParameter("TestTexture2D");
      dtCore::RefPtr<osg::StateSet> stateSet = new osg::StateSet();

      param->SetAddressMode(dtCore::TextureShaderParameter::TextureAxis::S,
         dtCore::TextureShaderParameter::AddressMode::MIRROR);
      param->SetAddressMode(dtCore::TextureShaderParameter::TextureAxis::T,
         dtCore::TextureShaderParameter::AddressMode::MIRROR);
      param->SetTexture("Textures/detailmap.png");
      param->SetTextureUnit(2);

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

      // Test clone behavior
      dtCore::RefPtr<dtCore::FloatShaderParameter> clonedParam = 
         static_cast<dtCore::FloatShaderParameter *>(param->Clone());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.", 
         clonedParam->GetValue(), param->GetValue());

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

      // Test clone behavior
      dtCore::RefPtr<dtCore::IntegerShaderParameter> clonedParam = 
         static_cast<dtCore::IntegerShaderParameter *>(param->Clone());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Cloned value should be the same.", 
         clonedParam->GetValue(), param->GetValue());

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
void ShaderParameterTests::TestTimerFloatParameter()
{
   try
   {
      dtCore::RefPtr<TestShaderParameterFloatTimer> param = new TestShaderParameterFloatTimer();
      dtCore::RefPtr<osg::StateSet> ss = new osg::StateSet();

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

      param->SetOscillationType(dtCore::ShaderParameterFloatTimer::OscillationType::DOWN);
      CPPUNIT_ASSERT_MESSAGE("Oscillation Type should have been set", 
         param->GetOscillationType() == dtCore::ShaderParameterFloatTimer::OscillationType::DOWN);


      param->AttachToRenderState(*ss);
      osg::Uniform *uniform = ss->getUniform(param->GetName());
      CPPUNIT_ASSERT(uniform != NULL);
      CPPUNIT_ASSERT_EQUAL(osg::Uniform::FLOAT,uniform->getType());

      // CHECK INITIAL CONDITIONS FROM FIRST Update()

      CPPUNIT_ASSERT_MESSAGE("Current Oscillation Type should have been set", 
         param->GetOscillationType() == dtCore::ShaderParameterFloatTimer::OscillationType::DOWN);
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

      param->SetOscillationType(dtCore::ShaderParameterFloatTimer::OscillationType::UP);
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

      param->SetOscillationType(dtCore::ShaderParameterFloatTimer::OscillationType::UPANDDOWN);
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

      param->SetOscillationType(dtCore::ShaderParameterFloatTimer::OscillationType::DOWNANDUP);
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
      SLEEP(2);
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
      dtCore::RefPtr<dtCore::ShaderParameterFloatTimer> clonedParam = 
         static_cast<dtCore::ShaderParameterFloatTimer *>(param->Clone());
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
