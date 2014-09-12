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
*/

#include <dtRender/shadowscene.h>
#include <dtRender/ephemerisscene.h>
#include <dtRender/uniformactcomp.h>

#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>
#include <dtUtil/mathdefines.h>

#include <dtCore/light.h>
#include <dtCore/shaderparambool.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/shaderparamfloat.h>
#include <osg/LightSource>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowTechnique>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ShadowVolume>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/ShadowMap>

#include <dtRender/parallelsplitshadowmap.h>

//needed to get scene/ scene light
#include <dtCore/scene.h>
#include <dtCore/propertymacros.h>
#include <dtGame/gamemanager.h>
#include <dtRender/scenemanager.h>

namespace dtRender
{

   const dtCore::RefPtr<SceneType> ShadowScene::SHADOW_SCENE(new SceneType("Shadow Scene", "Scene", "A scene which computes shadows for it's children."));

   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShadowScene::ShadowMapType);

   ShadowScene::ShadowMapType ShadowScene::ShadowMapType::BASIC_SHADOW_MAP("BASIC_SHADOW_MAP");
   ShadowScene::ShadowMapType ShadowScene::ShadowMapType::LSPSM("LSPSM");
   ShadowScene::ShadowMapType ShadowScene::ShadowMapType::PSSM("PSSM");
   ShadowScene::ShadowMapType ShadowScene::ShadowMapType::SOFT_SHADOW_MAP("SOFT_SHADOW_MAP");
   
   ShadowScene::ShadowMapType::ShadowMapType(const std::string &name) 
      : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShadowScene::ShadowResolution);

   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_ULTRAHIGH("SR_ULTRAHIGH");
   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_HIGH("SR_HIGH");
   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_MEDIUM("SR_MEDIUM");
   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_LOW("SR_LOW");

   ShadowScene::ShadowResolution::ShadowResolution(const std::string &name) 
      : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   ////////

   const dtUtil::RefString ShadowScene::UNIFORM_RENDER_SHADOWS("d3d_RenderShadows");
   const dtUtil::RefString ShadowScene::UNIFORM_SHADOW_EFFECT_SCALAR("d3d_ShadowEffectScalar");
   const dtUtil::RefString ShadowScene::UNIFORM_SHADOW_TEXTURE_UNIT("d3d_ShadowTextureUnit");


   ////////

   class UpdateShadowLightCallback : public osg::NodeCallback
   {
   public:

      UpdateShadowLightCallback(SceneManager* sm, ShadowScene* ss)
         : mSceneManager(sm)
         , mEphemerisScene(NULL)
         , mShadowScene(ss)
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         // first update subgraph to make sure objects are all moved into postion
         traverse(node,nv);

         if(!mEphemerisScene.valid())
         {
            mEphemerisScene = dynamic_cast<EphemerisScene*>(mSceneManager->FindSceneByType(*EphemerisScene::EPHEMERIS_SCENE));
            mShadowScene->SetLightSource(mEphemerisScene->GetLightSource());
         }

         if(mEphemerisScene.valid())
         {
            osg::Vec3 up (0.0, 0.0, 1.0);
            float shadowScalar = 1.0f;

            osg::Vec3 lightDir =  mEphemerisScene->GetSunPosition();

            osg::Vec3 lightVector = lightDir;
            lightVector.normalize();

            float lightDotUp = lightVector * up;
            if (lightDotUp < 0.0)
            {
               lightDir = mEphemerisScene->GetMoonPosition();

               lightVector = lightDir;
               lightVector.normalize();
               lightDotUp = lightVector * up;
               
               if (lightDotUp < 0.2)
               {
                  lightDir = up * 1000000.0;
               
                  shadowScalar = dtUtil::MapRangeValue<float>(lightDotUp, 0.0f, 0.2f, 0.5f, 1.0f);
               }
               else
               {
                  shadowScalar = 0.5f;
               }
            }
            else if(lightDotUp < 0.2)
            {
               //this might be a problem if the light was never set?
               shadowScalar = 0.7f * dtUtil::Max(0.0f, dtUtil::MapRangeValue<float>(lightDotUp, 0.0f, 0.1f, 0.5f, 1.0f));
               
               if(mShadowScene->GetUseShadowEffectScalar())
               {
                  mShadowScene->SetShadowEffectsScalar(shadowScalar);
               }

               return;
            }
                        
            shadowScalar *= 0.7f;

            shadowScalar = dtUtil::Max(0.3f, shadowScalar);

            if(mShadowScene->GetUseShadowEffectScalar())
            {
               mShadowScene->SetShadowEffectsScalar(shadowScalar);
            }


            mShadowScene->GetLightSource()->getLight()->setPosition(osg::Vec4(lightDir, 0.0) );

            lightDir.normalize();
            lightDir *= -1.0;

            mShadowScene->GetLightSource()->getLight()->setDirection(lightDir);
         
            
         }

      }

   protected:

      virtual ~UpdateShadowLightCallback() {}

      dtCore::ObserverPtr<dtRender::SceneManager> mSceneManager;            
      dtCore::ObserverPtr<dtRender::EphemerisScene> mEphemerisScene;            
      dtCore::ObserverPtr<dtRender::ShadowScene> mShadowScene;            
   };



   class ShadowSceneImpl
   {
      public:

         ShadowSceneImpl()
            : mMaxFarPlane(1024.0f)
            , mMinLightMargin(10.0f)
         {
         }


         float mMaxFarPlane;
         float mMinLightMargin;

         dtCore::RefPtr<osgShadow::ShadowedScene> mNode;
         dtCore::RefPtr<osg::LightSource>  mLightSource;

         //shader parameters
         dtCore::RefPtr<dtCore::ShaderParamBool> mRenderShadows;
         dtCore::RefPtr<dtCore::ShaderParamInt> mShadowTextureUnit;
         dtCore::RefPtr<dtCore::ShaderParamFloat> mShadowEffectScalar;


   };

   ShadowScene::ShadowScene()
   : BaseClass(*SHADOW_SCENE, SceneEnum::NON_TRANSPARENT_OBJECTS)   
   , mShadowMapType(&ShadowMapType::PSSM)
   , mShadowResolution(&ShadowResolution::SR_MEDIUM)
   , mAmbientBias(0.4f, 0.6f)
   , mPolygonOffset(10.0f, 20.0f)
   , mTextureUnitOffset(5)
   , mNumPSSMSplits(3)
   , mMinNearDistance(1.0f)
   , mMaxFarDistance(1000.0f)
   , mUseShadowEffectScalar(true)
   , mImpl(new ShadowSceneImpl())
   {
      SetName("ShadowScene");
   }


   ShadowScene::~ShadowScene()
   {
      delete mImpl;
   }


   void ShadowScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      
         mImpl->mNode = new osgShadow::ShadowedScene();

         mImpl->mNode->getOrCreateStateSet()->setGlobalDefaults();

         mImpl->mNode->setReceivesShadowTraversalMask(dtUtil::NodeMask::SHADOW_RECEIVE);
         mImpl->mNode->setCastsShadowTraversalMask(dtUtil::NodeMask::SHADOW_CAST);

         //shadow scene becomes the new default scene
         sm.PushScene(*this);
      
         //create a uniform for the shadow texture unit
        
         UniformActComp* uniformActComp = sm.GetOwner()->GetComponent<UniformActComp>();
         if(uniformActComp != NULL)
         {
            mImpl->mRenderShadows = new dtCore::ShaderParamBool(UNIFORM_RENDER_SHADOWS);
            mImpl->mRenderShadows->SetValue(true);

            mImpl->mShadowTextureUnit = new dtCore::ShaderParamInt(UNIFORM_SHADOW_TEXTURE_UNIT);
            mImpl->mShadowTextureUnit->SetValue(mTextureUnitOffset);

            mImpl->mShadowEffectScalar = new dtCore::ShaderParamFloat(UNIFORM_SHADOW_EFFECT_SCALAR);
            mImpl->mShadowEffectScalar->SetValue(1.0f);

            uniformActComp->AddParameter(*mImpl->mRenderShadows);
            uniformActComp->AddParameter(*mImpl->mShadowTextureUnit);
            uniformActComp->AddParameter(*mImpl->mShadowEffectScalar);
         }

         UpdateShadowLightCallback* usc = new UpdateShadowLightCallback(&sm, this);
         GetSceneNode()->setUpdateCallback(usc);
            
         /*if(!mImpl->mLightSource.valid())
         {
            //use light zero for sunlight
            mImpl->mLightSource = gm->GetScene().GetLight(0)->GetLightSource();
         }*/
         

         //this creates the shadow technique
         SetShadowsEnabled(true);
   }

   osg::Group* ShadowScene::GetSceneNode()
   {
      return mImpl->mNode.get();
   }

   const osg::Group* ShadowScene::GetSceneNode() const
   {
      return mImpl->mNode.get();
   }

   void ShadowScene::SetShadowsEnabled(bool enabled)
   {
      if (enabled)
      {
         SetShadowMap();
      }
      else
      {
         mImpl->mNode->setShadowTechnique(NULL);
      }
   }

   bool ShadowScene::GetShadowsEnabled() const
   {
      const bool enabled = mImpl->mNode->getShadowTechnique() != NULL;
      return enabled;
   }

   void ShadowScene::SetShadowMap()
   {
      int shadowRes = 0;
      if (mShadowResolution.get() == &ShadowScene::ShadowResolution::SR_ULTRAHIGH)        { shadowRes = 8192; }
      else if (mShadowResolution.get() == &ShadowScene::ShadowResolution::SR_HIGH)        { shadowRes = 4096; }
      else if (mShadowResolution.get() == &ShadowScene::ShadowResolution::SR_MEDIUM) { shadowRes = 2048; }
      else if (mShadowResolution.get() == &ShadowScene::ShadowResolution::SR_LOW)    { shadowRes = 1024; }

      osg::ref_ptr<osgShadow::ShadowTechnique> shadowMap;

      if (mShadowMapType.get() == &ShadowMapType::BASIC_SHADOW_MAP)
      {
         /*osg::ref_ptr<osgShadow::ShadowMap>*/ shadowMap =
            GetBasicShadowMap(shadowRes);
      }
      else if (mShadowMapType.get() == &ShadowMapType::LSPSM)
      {
         /*osg::ref_ptr<osgShadow::LightSpacePerspectiveShadowMapCB>*/ shadowMap =
            GetLightSpacePerspectiveShadowMap(shadowRes);
      }
      else if (mShadowMapType.get() == &ShadowMapType::SOFT_SHADOW_MAP)
      {
         /*osg::ref_ptr<osgShadow::SoftShadowMap>*/ shadowMap =
            GetSoftShadowMap(shadowRes);
      }
      else if (mShadowMapType.get() == &ShadowMapType::PSSM)
      {
         /*osg::ref_ptr<osgShadow::ParallelSplitShadowMap>*/ shadowMap =
            GetParallelSplitShadowMap(shadowRes);
      }
      else
      {
         LOG_ERROR("Unrecognized Shadow Map Type.");
         return;
      }

      mImpl->mNode->setShadowTechnique(shadowMap.get());
   }

   osgShadow::ShadowTechnique* ShadowScene::GetBasicShadowMap(int shadowRes)
   {
      //osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT, fragShaderSource);

      // Setup the scene with shadows via shadow mapping
      osgShadow::ShadowMap* shadowMap = new osgShadow::ShadowMap;

      //osgShadow::ShadowMap* shadowMap = new osgShadow::ShadowMap;
      shadowMap->setTextureSize(osg::Vec2s(shadowRes, shadowRes));
      if(mImpl->mLightSource.valid())
      {
         shadowMap->setLight(mImpl->mLightSource->getLight());
      }
      shadowMap->setTextureUnit(mTextureUnitOffset);
      //shadowMap->addShader(fragShader);
      
      shadowMap->setAmbientBias(mAmbientBias);

      return shadowMap;
   }

   osgShadow::ShadowTechnique* ShadowScene::GetLightSpacePerspectiveShadowMap(int shadowRes)
   {
      /*osg::Shader* shadowVertShader = new osg::Shader(osg::Shader::VERTEX, shadowVertShaderSource);
      osg::Shader* vertShader = new osg::Shader(osg::Shader::VERTEX, mainVertShaderSource);
      osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT, fragShaderSource);*/

      osgShadow::LightSpacePerspectiveShadowMapDB* shadowMap =
         new osgShadow::LightSpacePerspectiveShadowMapDB;

      unsigned int baseTexUnit = 0;
      

      shadowMap->setMinLightMargin(mImpl->mMinLightMargin);
      shadowMap->setMaxFarPlane(mImpl->mMaxFarPlane);
      shadowMap->setTextureSize(osg::Vec2s(shadowRes, shadowRes));
      shadowMap->setShadowTextureCoordIndex(mTextureUnitOffset);
      shadowMap->setShadowTextureUnit(mTextureUnitOffset);
      shadowMap->setBaseTextureCoordIndex(baseTexUnit);
      shadowMap->setBaseTextureUnit(baseTexUnit);
      

      if(mImpl->mLightSource.valid())
      {
         shadowMap->setLight(mImpl->mLightSource->getLight());
      }

      //shadowMap->setShadowVertexShader(shadowVertShader);
      //shadowMap->setMainVertexShader(vertShader);
      //shadowMap->setMainFragmentShader(fragShader);

      return shadowMap;
   }

   osgShadow::ShadowTechnique* ShadowScene::GetSoftShadowMap(int shadowRes)
   {
      osgShadow::ShadowVolume* shadowMap = new osgShadow::ShadowVolume;
      //shadowMap->setTextureSize(osg::Vec2s(shadowRes, shadowRes));
      //shadowMap->setTextureUnit(mTextureUnitOffset);
      //shadowMap->setSoftnessWidth(1.0f);

      return shadowMap;
   }

   osgShadow::ShadowTechnique* ShadowScene::GetParallelSplitShadowMap(int shadowRes)
   {
      //custom modified version
      dtRender::ParallelSplitShadowMap* shadowMap = new dtRender::ParallelSplitShadowMap(NULL, mNumPSSMSplits, mTextureUnitOffset);
      shadowMap->setTextureResolution(shadowRes);  
      shadowMap->setMinNearDistanceForSplits(mMinNearDistance);
      shadowMap->setMaxFarDistance(mMaxFarDistance);
      shadowMap->setPolygonOffset(mPolygonOffset);
      shadowMap->setMoveVCamBehindRCamFactor(15.0);
      shadowMap->setAmbientBias(mAmbientBias);
      
      //cannot set texture unit offset??

      if(mImpl->mLightSource.valid())
      {
         shadowMap->setUserLight(mImpl->mLightSource->getLight());
      }

      return shadowMap;
   }

   osg::LightSource* ShadowScene::GetLightSource()
   {
      return mImpl->mLightSource;
   }

   bool ShadowScene::AddChild(DeltaDrawable* child)
   {
      return BaseClass::AddChild(child);
   }

   void ShadowScene::RemoveChild(DeltaDrawable* child)
   {
      BaseClass::RemoveChild(child);
   }

   void ShadowScene::SetLightSource( osg::LightSource* ls )
   {
      mImpl->mLightSource = ls;


      if (mShadowMapType.get() == &ShadowMapType::BASIC_SHADOW_MAP)
      {
         osgShadow::ShadowMap* shadowMap = dynamic_cast<osgShadow::ShadowMap*>(mImpl->mNode->getShadowTechnique());
         if(shadowMap != NULL)
         {
            shadowMap->setLight(ls->getLight());
         }
      }
      else if (mShadowMapType.get() == &ShadowMapType::LSPSM)
      {
         osgShadow::LightSpacePerspectiveShadowMap* shadowMap = dynamic_cast<osgShadow::LightSpacePerspectiveShadowMap*>(mImpl->mNode->getShadowTechnique());
         if(shadowMap != NULL)
         {
            shadowMap->setLight(ls->getLight());
         }
      }
      else if (mShadowMapType.get() == &ShadowMapType::SOFT_SHADOW_MAP)
      {
      }
      else if (mShadowMapType.get() == &ShadowMapType::PSSM)
      {
         dtRender::ParallelSplitShadowMap* shadowMap = dynamic_cast<dtRender::ParallelSplitShadowMap*>(mImpl->mNode->getShadowTechnique());
         if(shadowMap != NULL)
         {
            shadowMap->setUserLight(ls->getLight());
         }
      }
      else
      {
         LOG_ERROR("Unrecognized Shadow Map Type.");
         return;
      }
   }

   void ShadowScene::SetShadowEffectsScalar(float f)
   {
      if(mImpl->mShadowEffectScalar.valid())
      {
         mImpl->mShadowEffectScalar->SetValue(f);
         mImpl->mShadowEffectScalar->Update();
      }
   }

   float ShadowScene::GetShadowEffectsScalar() const
   {
      float result = 1.0;
      if(mImpl->mShadowEffectScalar.valid())
      {
         result = mImpl->mShadowEffectScalar->GetValue();
      }
      return result;
   }

   ShadowSceneActor::ShadowSceneActor()
   {
   }

   ShadowSceneActor::~ShadowSceneActor()
   {
   }

   void ShadowSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("ShadowScene");
      typedef dtCore::PropertyRegHelper<ShadowSceneActor&, ShadowScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<ShadowScene>(), group);


      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(ShadowMapType, "ShadowMapType", "Shadow Map Type",
         "Determines the technique to render the shadows.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(ShadowResolution, "ShadowResolution", "Shadow Resolution",
         "Determines the resolution of the shadow map.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TextureUnitOffset, "TextureUnitOffset", "Texture Unit Offset",
         "The texture unit to start the shadow maps with.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(NumPSSMSplits, "NumPSSMSplits", "Number of PSSM Splits",
         "Number of shadow cameras for PSSM shadow scene.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(AmbientBias, "AmbientBias", "Ambient Bias",
         "Softens the result of the shadow map.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(PolygonOffset, "PolygonOffset", "Polygon Offset",
         "Keeps self shadowing objects from z fighting.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(MinNearDistance, "MinNearDistance", "MinNearDistance",
         "The start distance for PSSM Splits.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(MaxFarDistance, "MaxFarDistance", "MaxFarDistance",
         "The end distance for PSSM Splits.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(UseShadowEffectScalar, "UseShadowEffectScalar", "UseShadowEffectScalar",
         "This option scales the effect of the shadow map based on the sun, moon, and how shallow the angle to surface is.",
         PropRegHelperType, propRegHelper);
   }

   void ShadowSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<ShadowScene> es = new ShadowScene();
      SetDrawable(*es);
   }

   bool ShadowSceneActor::IsPlaceable() const
   {
      return false;
   }



   
}//namespace dtRender
