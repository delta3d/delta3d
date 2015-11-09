/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2015, Caper Holdings, LLC
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
*/

#include <dtRender/atmospherescene.h>
#include <dtRender/multipassscene.h>
#include <dtRender/scenemanager.h>

#include <dtCore/camera.h>
#include <dtCore/observerptr.h>
#include <dtCore/propertymacros.h>

#include <dtUtil/nodemask.h>
#include <dtUtil/mathdefines.h>

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

   const dtCore::RefPtr<SceneType> AtmosphereScene::ATMOSPHERE_SCENE(new SceneType("Atmosphere Scene", "Scene", "Creates an osgPPU scene with post process fog and scattering calculations."));

   AtmosphereScene::AtmosphereScene()
   : BaseClass(*ATMOSPHERE_SCENE, SceneEnum::MULTIPASS)
   , mFogEnabled(true)
   , mAutoComputeNearFar(true)
   , mFogDensity(0.001)   
   , mNearPlane(1.0f)
   , mFarPlane(1000.0f)
   , mVisibility(1000.0f)
   , mFogColor(0.84f, 0.87f, 1.0f, 1.0f)
   {
      SetName("AtmosphereScene");  
   }


   AtmosphereScene::~AtmosphereScene()
   {
   }


   void AtmosphereScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
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

         mNearPlane = nearp;
         mFarPlane = farp;

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

   void AtmosphereScene::OnAddedToPPUScene( MultipassScene& mps )
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

   void AtmosphereScene::CreateDOFPipeline(osgPPU::UnitBypass* bypass, osgPPU::UnitBypass* depthbypass, osgPPU::Unit* resampleLight)
   {
      osg::ref_ptr<osgDB::ReaderWriter::Options> fragmentOptions = new osgDB::ReaderWriter::Options("fragment");
      osg::ref_ptr<osgDB::ReaderWriter::Options> vertexOptions = new osgDB::ReaderWriter::Options("vertex");

      osgPPU::Unit* fog = new osgPPU::UnitInOut();
      {
         // setup inputs, name and index
         fog->setName("Fog-Result");

         // setup shader
         osgPPU::ShaderAttribute* sh = new osgPPU::ShaderAttribute();
         sh->addShader(osgDB::readShaderFile("shaders/base/fog_post_process.frag", fragmentOptions.get()));
         sh->setName("FogResultShader");

         sh->add("zNear", osg::Uniform::FLOAT);
         sh->add("zFar", osg::Uniform::FLOAT);
         sh->add("fogDensity", osg::Uniform::FLOAT);
         sh->add("fogColor", osg::Uniform::FLOAT_VEC4);

         sh->set("zNear", mNearPlane);
         sh->set("zFar", mFarPlane);
         sh->set("fogDensity", mFogDensity);
         sh->set("fogColor", mFogColor);

         
         fog->getOrCreateStateSet()->setAttributeAndModes(sh);
         fog->setInputTextureIndexForViewportReference(0); // we want to setup viewport based on this input

         // add inputs as uniform parameters
         fog->setInputToUniform(bypass, "texColorMap", true);
         fog->setInputToUniform(depthbypass, "texDepthMap", true);
      }

      SetFirstUnit(*fog);
      SetLastUnit(*fog);
   }

   bool AtmosphereScene::GetFogEnable() const
   {
      return mFogEnabled;
   }

   void AtmosphereScene::SetFogEnable(bool val)
   {
      mFogEnabled = val;
   }

   bool AtmosphereScene::GetAutoComputeNearFar() const
   {
      return mAutoComputeNearFar;
   }

   void AtmosphereScene::SetAutoComputeNearFar(bool val)
   {
      mAutoComputeNearFar = val;
   }

   const osg::Vec4& AtmosphereScene::GetFogColor() const
   {
      return mFogColor;
   }

   void AtmosphereScene::SetFogColor(const osg::Vec4& color)
   {
      mFogColor = color;
   }


   float AtmosphereScene::GetFogDensity() const
   {
      return mFogDensity;
   }


   void AtmosphereScene::SetFogDensity(float val)
   {
      mFogDensity = val;
   }


   void AtmosphereScene::SetVisibility(float distance)
   {
      mVisibility = distance;
      
      if (dtUtil::Equivalent(mVisibility, 0.0f))
      {
         SetFogEnable(false);
         return;
      }

      double sqrt_m_log01 = sqrt(-log(0.01));
      mFogDensity = sqrt_m_log01 / mVisibility;
   }

   float AtmosphereScene::GetVisibility() const
   {
      return mVisibility;
   }


   //proxy
   AtmosphereSceneActor::AtmosphereSceneActor()
   {
   }

   AtmosphereSceneActor::~AtmosphereSceneActor()
   {
   }

   void AtmosphereSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("AtmosphereScene");
      typedef dtCore::PropertyRegHelper<AtmosphereSceneActor, AtmosphereScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<AtmosphereScene>(), group);


   }

   void AtmosphereSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<AtmosphereScene> es = new AtmosphereScene();

      SetDrawable(*es);
   }

   bool AtmosphereSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

