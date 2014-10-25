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

#include <dtRender/dofscene.h>
#include <dtRender/multipassscene.h>
#include <dtRender/scenemanager.h>

#include <dtCore/camera.h>
#include <dtCore/observerptr.h>
#include <dtCore/propertymacros.h>

#include <dtUtil/nodemask.h>

#include <osg/Camera>
#include <osg/Texture2D>
#include <osg/ClampColor>

#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>
#include <osgPPU/UnitCamera.h>
#include <osgPPU/UnitCameraAttachmentBypass.h>
#include <osgPPU/UnitDepthbufferBypass.h>
#include <osgPPU/UnitInOut.h>
#include <osgPPU/UnitOut.h>
#include <osgPPU/UnitInMipmapOut.h>
#include <osgPPU/UnitInResampleOut.h>
#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitText.h>


namespace dtRender
{

   const dtCore::RefPtr<SceneType> DOFScene::DOF_SCENE(new SceneType("DOF Scene", "Scene", "Creates an osgPPU Depth of Field Scene."));

   DOFScene::DOFScene()
   : BaseClass(*DOF_SCENE, SceneEnum::MULTIPASS)
   , mFocalLength(20.0f)
   , mFocalRange(100.0)
   , mZNear(1.0f)
   , mZFar(1000.0f)
   , mGaussSigma(1.5)
   , mGaussRadius(2.0)
   {
      SetName("DOFScene");  
   }


   DOFScene::~DOFScene()
   {
   }


   void DOFScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
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
         //set znear, zfar
         double vfov, asp, nearp, farp;
         sm.GetSceneCamera()->GetPerspectiveParams(vfov, asp, nearp, farp);

         SetZNear(nearp);
         SetZFar(farp);

         mps->DetachDefaultUnitOut();


         osgPPU::UnitDepthbufferBypass* depthbypass = mps->GetDepthBypass();

         if(depthbypass == NULL)
         {         
            // next unit will bypass the depth output of the camera
            depthbypass = new osgPPU::UnitDepthbufferBypass();
            depthbypass->setName("DepthBypass");
            mps->GetMultipassPPUCamera()->addChild(depthbypass);
         }


         CreateDOFPipeline(mps->GetColorBypass(), depthbypass, mps->GetResampleColor());

         GetLastUnit()->addChild(mps->GetUnitOut());

      }
      else
      {
         LOG_ERROR("Must have a valid Multipass Scene to use DOF Scene.");
      }
   }

   void DOFScene::OnAddedToPPUScene( MultipassScene& mps )
   {
      if(mps.GetResampleColor() != NULL)
      {
         mps.GetResampleColor()->addChild(GetSceneNode());
      }
      else
      {
         mps.GetColorBypass()->addChild(GetSceneNode());
      }
      
   }

   void DOFScene::CreateDOFPipeline(osgPPU::UnitBypass* bypass, osgPPU::UnitBypass* depthbypass, osgPPU::Unit* resampleLight)
   {
      osg::ref_ptr<osgDB::ReaderWriter::Options> fragmentOptions = new osgDB::ReaderWriter::Options("fragment");
      osg::ref_ptr<osgDB::ReaderWriter::Options> vertexOptions = new osgDB::ReaderWriter::Options("vertex");

      // we need to blur the output of the color texture to emulate
      // the depth of field. Therefor first we just resample
      if(resampleLight == NULL)
      {
         osgPPU::UnitInResampleOut* resample = new osgPPU::UnitInResampleOut();

         resample->setName("ResampleLight");
         resample->setFactorX(0.5);
         resample->setFactorY(0.5);

         SetFirstUnit(*resample);
         
         resampleLight = resample;
      }
      
      

      // helper shader class to perform gauss blur
      osgPPU::ShaderAttribute* gaussx = new osgPPU::ShaderAttribute();
      osgPPU::ShaderAttribute* gaussy = new osgPPU::ShaderAttribute();
      {
         // read shaders from file
         osg::Shader* vshader = osgDB::readShaderFile("shaders/hdr/gauss_convolution_vp.glsl", vertexOptions.get());
         osg::Shader* fhshader = osgDB::readShaderFile("shaders/hdr/gauss_convolution_1Dx_fp.glsl", fragmentOptions.get());
         osg::Shader* fvshader = osgDB::readShaderFile("shaders/hdr/gauss_convolution_1Dy_fp.glsl", fragmentOptions.get());

         // setup horizontal blur shaders
         gaussx->addShader(vshader);
         gaussx->addShader(fhshader);
         gaussx->setName("BlurHorizontalShader");

         gaussx->add("sigma", osg::Uniform::FLOAT);
         gaussx->add("radius", osg::Uniform::FLOAT);
         gaussx->add("texUnit0", osg::Uniform::SAMPLER_2D);

         gaussx->set("sigma", GetGaussSigma());
         gaussx->set("radius", GetGaussRadius());
         gaussx->set("texUnit0", 0);

         // setup vertical blur shaders
         gaussy->addShader(vshader);
         gaussy->addShader(fvshader);
         gaussy->setName("BlurVerticalShader");

         gaussy->add("sigma", osg::Uniform::FLOAT);
         gaussy->add("radius", osg::Uniform::FLOAT);
         gaussy->add("texUnit0", osg::Uniform::SAMPLER_2D);

         gaussy->set("sigma", GetGaussSigma());
         gaussy->set("radius", GetGaussRadius());
         gaussy->set("texUnit0", 0);
      }

      // now we perform a gauss blur on the downsampled data
      osgPPU::UnitInOut* blurxlight = new osgPPU::UnitInOut();
      osgPPU::UnitInOut* blurylight = new osgPPU::UnitInOut();
      {
         // set name and indicies
         blurxlight->setName("BlurHorizontalLight");
         blurylight->setName("BlurVerticalLight");

         //blurxlight->setShader(gaussx);
         //blurylight->setShader(gaussy);
         blurxlight->getOrCreateStateSet()->setAttributeAndModes(gaussx);
         blurylight->getOrCreateStateSet()->setAttributeAndModes(gaussy);
      }

      if(GetFirstUnit() == NULL)
      {
         SetFirstUnit(*blurxlight);
      }
      else
      {
         resampleLight->addChild(blurxlight);
      }

      blurxlight->addChild(blurylight);


      // and also a stronger blurred/resampled texture is required
      osgPPU::UnitInResampleOut* resampleStrong = new osgPPU::UnitInResampleOut();
      {
         resampleStrong->setName("ResampleStrong");
         resampleStrong->setFactorX(0.25);
         resampleStrong->setFactorY(0.25);
      }
      bypass->addChild(resampleStrong);

      // now we perform a gauss blur on the downsampled data
      osgPPU::UnitInOut* blurxstrong = new osgPPU::UnitInOut();
      osgPPU::UnitInOut* blurystrong = new osgPPU::UnitInOut();
      {
         // set name and indicies
         blurxstrong->setName("BlurHorizontalStrong");
         blurystrong->setName("BlurVerticalStrong");

         //blurxstrong->setShader(gaussx);
         //blurystrong->setShader(gaussy);
         blurxstrong->getOrCreateStateSet()->setAttributeAndModes(gaussx);
         blurystrong->getOrCreateStateSet()->setAttributeAndModes(gaussy);
      }
      resampleStrong->addChild(blurxstrong);
      blurxstrong->addChild(blurystrong);


      // And finally we add a ppu which do use all the computed results:
      osgPPU::Unit* dof = new osgPPU::UnitInOut();
      {
         // setup inputs, name and index
         dof->setName("DoF-Result");

         // setup shader
         osgPPU::ShaderAttribute* sh = new osgPPU::ShaderAttribute();
         sh->addShader(osgDB::readShaderFile("shaders/hdr/depth_of_field_fp.glsl", fragmentOptions.get()));
         sh->setName("DoFResultShader");

         sh->add("focalLength", osg::Uniform::FLOAT);
         sh->add("focalRange", osg::Uniform::FLOAT);
         sh->add("zNear", osg::Uniform::FLOAT);
         sh->add("zFar", osg::Uniform::FLOAT);

         sh->set("focalLength", GetFocalLength());
         sh->set("focalRange", GetFocalRange());
         sh->set("zNear", GetZNear());
         sh->set("zFar", GetZFar());

         //dof->setShader(sh);
         dof->getOrCreateStateSet()->setAttributeAndModes(sh);
         dof->setInputTextureIndexForViewportReference(0); // we want to setup viewport based on this input

         // add inputs as uniform parameters
         dof->setInputToUniform(bypass, "texColorMap", true);
         dof->setInputToUniform(blurylight, "texBlurredColorMap", true);
         dof->setInputToUniform(blurystrong, "texStrongBlurredColorMap", true);
         dof->setInputToUniform(depthbypass, "texDepthMap", true);
      }

      // this is the last unit
      
      SetLastUnit(*dof);
   }


   //proxy
   DOFSceneActor::DOFSceneActor()
   {
   }

   DOFSceneActor::~DOFSceneActor()
   {
   }

   void DOFSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("DOFScene");
      typedef dtCore::PropertyRegHelper<DOFSceneActor, DOFScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<DOFScene>(), group);


      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(FocalLength, "FocalLength", "FocalLength",
         "Determines the near focal length.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(FocalRange, "FocalRange", "FocalRange",
         "Determines the near focal range.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(GaussSigma, "GaussSigma", "GaussSigma",
         "Determines the amount of blur.",
         PropRegHelperType, propRegHelper);      

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(GaussRadius, "GaussRadius", "GaussRadius",
         "Determines the radius of blur.",
         PropRegHelperType, propRegHelper);
   }

   void DOFSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<DOFScene> es = new DOFScene();

      SetDrawable(*es);
   }

   bool DOFSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

