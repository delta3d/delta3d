/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*
*  Portions of this code were taken from the osgPPU examples
*     please see-     http://projects.tevs.eu/osgppu/ 
*
*/

#include <dtRender/ssaoscene.h>
#include <dtRender/multipassscene.h>

#include <dtUtil/nodemask.h>

#include <dtCore/propertymacros.h>

#include <dtCore/observerptr.h>

#include <osg/Camera>
#include <osg/Texture2D>
#include <osg/ClampColor>

#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>
#include <osgPPU/UnitCamera.h>
#include <osgPPU/UnitDepthbufferBypass.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitCameraAttachmentBypass.h>
#include <osgPPU/UnitInOut.h>
#include <osgPPU/UnitOut.h>
#include <osgPPU/UnitInMipmapOut.h>
#include <osgPPU/UnitInResampleOut.h>
#include <osgPPU/ShaderAttribute.h>

//needed to set the scene camera
#include <dtRender/scenemanager.h>
#include <dtGame/gamemanager.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>


namespace dtRender
{


   const dtCore::RefPtr<SceneType> SSAOScene::SSAO_SCENE(new SceneType("SSAO Scene", "Scene", "Creates an osgPPU Screen Space Ambient Occlusion Scene."));

   SSAOScene::SSAOScene()
   : BaseClass(*SSAO_SCENE, SceneEnum::MULTIPASS)
   , mBlurSigma(40.0f)
   , mBlurRadius(15.0f)
   , mIntensity(2.0f)
   , mShowOnlyAOMap(false)
   {
      SetName("SSAOScene");  
   }


   SSAOScene::~SSAOScene()
   {
   }


   void SSAOScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      
      MultipassScene* mps = dynamic_cast<MultipassScene*>(sm.FindSceneByType(*MultipassScene::MULTIPASS_SCENE));
     
      //try to create default multipass scene
      if(mps == NULL)
      {
         sm.CreateDefaultMultipassScene();
         mps = dynamic_cast<MultipassScene*>(sm.FindSceneByType(*MultipassScene::MULTIPASS_SCENE));

      }

      if(mps != NULL)
      {
         
         CreateSSAOPipeline(mps->GetColorBypass(), mps->GetDepthBypass());

         mps->DetachDefaultUnitOut();

         GetLastUnit()->addChild(mps->GetUnitOut());
      }
      else
      {
         LOG_ERROR("Must have a valid Multipass Scene to use SSAO.");
      }
   }

   void SSAOScene::CreateSSAOPipeline(osgPPU::UnitBypass* colorBypass, osgPPU::UnitBypass* depthBypass)
   {

      //---------------------------------------------------------------------------------
      // Create units which will apply gaussian blur on the input textures
      //---------------------------------------------------------------------------------
      osgPPU::UnitInOut* blurx = new osgPPU::UnitInOut();
      osgPPU::UnitInOut* blury = new osgPPU::UnitInOut();
      {
         // set name and indicies
         blurx->setName("BlurHorizontal");
         blury->setName("BlurVertical");

         // read shaders from file
         osg::ref_ptr<osgDB::ReaderWriter::Options> fragmentOptions = new osgDB::ReaderWriter::Options("fragment");
         osg::ref_ptr<osgDB::ReaderWriter::Options> vertexOptions = new osgDB::ReaderWriter::Options("vertex");
         osg::Shader* vshader = osgDB::readShaderFile("shaders/hdr/gauss_convolution_vp.glsl", vertexOptions.get());
         osg::Shader* fhshader = osgDB::readShaderFile("shaders/hdr/gauss_convolution_1Dx_fp.glsl", fragmentOptions.get());
         osg::Shader* fvshader = osgDB::readShaderFile("shaders/hdr/gauss_convolution_1Dy_fp.glsl", fragmentOptions.get());

         // setup horizontal blur shaders
         osgPPU::ShaderAttribute* gaussx = new osgPPU::ShaderAttribute();
         gaussx->addShader(vshader);
         gaussx->addShader(fhshader);
         gaussx->setName("BlurHorizontalShader");

         gaussx->add("sigma", osg::Uniform::FLOAT);
         gaussx->add("radius", osg::Uniform::FLOAT);
         gaussx->add("texUnit0", osg::Uniform::SAMPLER_2D);

         gaussx->set("sigma", mBlurSigma);
         gaussx->set("radius", mBlurRadius);
         gaussx->set("texUnit0", 0);

         blurx->getOrCreateStateSet()->setAttributeAndModes(gaussx);

         // setup vertical blur shaders
         osgPPU::ShaderAttribute* gaussy = new osgPPU::ShaderAttribute();
         gaussy->addShader(vshader);
         gaussy->addShader(fvshader);
         gaussy->setName("BlurVerticalShader");

         gaussy->add("sigma", osg::Uniform::FLOAT);
         gaussy->add("radius", osg::Uniform::FLOAT);
         gaussy->add("texUnit0", osg::Uniform::SAMPLER_2D);

         gaussy->set("sigma", mBlurSigma);
         gaussy->set("radius", mBlurRadius);
         gaussy->set("texUnit0", 0);

         blury->getOrCreateStateSet()->setAttributeAndModes(gaussy);

         // connect the gaussian blur ppus
         SetFirstUnit(*blurx);
         blurx->addChild(blury);
    }

    //---------------------------------------------------------------------------------
    // Now we want to substract blurred from non-blurred depth and to compute the 
    // resulting AO image
    //---------------------------------------------------------------------------------
    osgPPU::UnitInOut* aoUnit = new osgPPU::UnitInOut;
    {
        osg::Shader* fpShader = new osg::Shader(osg::Shader::FRAGMENT);

        // create a shader which will process the depth values
        if (mShowOnlyAOMap == false)
        {
            fpShader->setShaderSource( 
                "uniform float intensity;\n"\
                "uniform sampler2D blurredDepthTexture;\n"\
                "uniform sampler2D originalDepthTexture;\n"\
                "uniform sampler2D colorTexture;\n"\
                "void main() {\n"\
                "   float blurred = texture2D(blurredDepthTexture, gl_TexCoord[0].xy).x;\n"\
                "   float original = texture2D(originalDepthTexture, gl_TexCoord[0].xy).x;\n"\
                "   vec4 color = texture2D(colorTexture, gl_TexCoord[0].xy);\n"\
                "   vec4 result = color - vec4(intensity * clamp((original - blurred), 0.0, 1.0));\n"\
                "   gl_FragData[0].xyzw = clamp(result, 0.0, 1.0);\n"\
                "}\n"
            );
        }else
        {
            fpShader->setShaderSource( 
                "uniform float intensity;\n"\
                "uniform sampler2D blurredDepthTexture;\n"\
                "uniform sampler2D originalDepthTexture;\n"\
                "uniform sampler2D colorTexture;\n"\
                "void main() {\n"\
                "   float blurred = texture2D(blurredDepthTexture, gl_TexCoord[0].xy).x;\n"\
                "   float original = texture2D(originalDepthTexture, gl_TexCoord[0].xy).x;\n"\
                "   vec4 color = texture2D(colorTexture, gl_TexCoord[0].xy);\n"\
                "   vec4 result = vec4(1.0 - intensity * clamp((original - blurred), 0.0, 1.0));\n"\
                "   gl_FragData[0].xyzw = clamp(result, 0.0, 1.0);\n"\
                "}\n"
            );
        }

        // create shader attribute and setup one input texture
        osgPPU::ShaderAttribute* shader = new osgPPU::ShaderAttribute;
        shader->addShader(fpShader);
        shader->add("blurredDepthTexture", osg::Uniform::SAMPLER_2D);
        shader->set("blurredDepthTexture", 0);
        shader->add("originalDepthTexture", osg::Uniform::SAMPLER_2D);
        shader->set("originalDepthTexture", 1);
        shader->add("colorTexture", osg::Uniform::SAMPLER_2D);
        shader->set("colorTexture", 2);
        shader->add("intensity", osg::Uniform::FLOAT);
        shader->set("intensity", mIntensity);

        // create the unit and attach the shader to it
        aoUnit->getOrCreateStateSet()->setAttributeAndModes(shader);
        blury->addChild(aoUnit);
        depthBypass->addChild(aoUnit);
        colorBypass->addChild(aoUnit);
        aoUnit->setName("ComputeAO");
    }
    SetLastUnit(*aoUnit);
}

void SSAOScene::OnAddedToPPUScene( MultipassScene& mps )
{
   if(mps.GetDepthBypass() != NULL)
   {
      mps.GetDepthBypass()->addChild(GetSceneNode());
   }
   else
   {
      mps.GetMultipassPPUCamera()->addChild(GetSceneNode());
   }
}


   //proxy
   SSAOSceneActor::SSAOSceneActor()
   {
   }

   SSAOSceneActor::~SSAOSceneActor()
   {
   }

   void SSAOSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("SSAOScene");
      typedef dtCore::PropertyRegHelper<SSAOSceneActor&, SSAOScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<SSAOScene>(), group);

      DT_REGISTER_PROPERTY_WITH_NAME(BlurSigma, "BlurSigma", "The sigma blur value to use for the SSAO.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(BlurRadius, "BlurRadius", "The radial blur value to use for the SSAO.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(Intensity, "Intensity", "The intensity to scale the effect of the SSAO.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(ShowOnlyAOMap, "ShowOnlyAOMap", "A debug value to view the results of the SSAO.", PropRegHelperType, propRegHelper);
   }

   void SSAOSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<SSAOScene> es = new SSAOScene();

      SetDrawable(*es);
   }

   bool SSAOSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

