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

#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>

#include <dtCore/light.h>
#include <osg/LightSource>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowTechnique>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/ShadowMap>

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

   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_HIGH("SR_HIGH");
   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_MEDIUM("SR_MEDIUM");
   ShadowScene::ShadowResolution ShadowScene::ShadowResolution::SR_LOW("SR_LOW");

   ShadowScene::ShadowResolution::ShadowResolution(const std::string &name) 
      : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }




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
         dtCore::RefPtr<dtCore::Light>  mLightSource;
   };

   ShadowScene::ShadowScene()
   : BaseClass(*SHADOW_SCENE, SceneEnum::NON_TRANSPARENT_OBJECTS)   
   , mShadowMapType(&ShadowMapType::BASIC_SHADOW_MAP)
   , mShadowResolution(&ShadowResolution::SR_MEDIUM)
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
      dtGame::GameManager* gm = sm.GetGameManager();
      if(gm != NULL)
      {
         mImpl->mNode = new osgShadow::ShadowedScene();

         mImpl->mNode->getOrCreateStateSet()->setGlobalDefaults();

         //use light zero for sunlight
         mImpl->mLightSource = gm->GetScene().GetLight(0);

         mImpl->mNode->setReceivesShadowTraversalMask(dtUtil::NodeMask::SHADOW_RECEIVE);
         mImpl->mNode->setCastsShadowTraversalMask(dtUtil::NodeMask::SHADOW_CAST);

         SetShadowsEnabled(true);

         //shadow scene becomes the new default scene
         sm.PushScene(*this);
      }
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
      if (mShadowResolution.get() == &ShadowScene::ShadowResolution::SR_HIGH)        { shadowRes = 4096; }
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
      shadowMap->setTextureSize(osg::Vec2s(shadowRes, shadowRes));
      shadowMap->setLight(mImpl->mLightSource->GetLightSource());
      //shadowMap->addShader(fragShader);
      //shadowMap->setAmbientBias(osg::Vec2(1.0f, 10.1f));

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
      unsigned int shadowTexUnit = 1;

      shadowMap->setMinLightMargin(mImpl->mMinLightMargin);
      shadowMap->setMaxFarPlane(mImpl->mMaxFarPlane);
      shadowMap->setTextureSize(osg::Vec2s(shadowRes, shadowRes));
      shadowMap->setShadowTextureCoordIndex(shadowTexUnit);
      shadowMap->setShadowTextureUnit(shadowTexUnit);
      shadowMap->setBaseTextureCoordIndex(baseTexUnit);
      shadowMap->setBaseTextureUnit(baseTexUnit);
      //shadowMap->setShadowVertexShader(shadowVertShader);
      //shadowMap->setMainVertexShader(vertShader);
      //shadowMap->setMainFragmentShader(fragShader);

      return shadowMap;
   }

   osgShadow::ShadowTechnique* ShadowScene::GetSoftShadowMap(int shadowRes)
   {
      osgShadow::SoftShadowMap* shadowMap = new osgShadow::SoftShadowMap;
      shadowMap->setTextureSize(osg::Vec2s(2048, 2048));
      shadowMap->setSoftnessWidth(1.0f);

      return shadowMap;
   }

   osgShadow::ShadowTechnique* ShadowScene::GetParallelSplitShadowMap(int shadowRes)
   {
      osgShadow::ParallelSplitShadowMap* shadowMap = new osgShadow::ParallelSplitShadowMap(NULL, 3);
      shadowMap->setTextureResolution(1024);
      shadowMap->setMinNearDistanceForSplits(0.25);
      shadowMap->setMaxFarDistance(1024.0);
      shadowMap->setPolygonOffset(osg::Vec2(10.0f, 20.0f));

      return shadowMap;
   }

   osg::LightSource* ShadowScene::GetLightSource()
   {
      return mImpl->mLightSource->GetLightSource();
   }

   bool ShadowScene::AddChild(DeltaDrawable* child)
   {
      return BaseClass::AddChild(child);
   }

   void ShadowScene::RemoveChild(DeltaDrawable* child)
   {
      BaseClass::RemoveChild(child);
   }
   ShadowSceneProxy::ShadowSceneProxy()
   {
   }

   ShadowSceneProxy::~ShadowSceneProxy()
   {
   }

   void ShadowSceneProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("ShadowScene");
      typedef dtCore::PropertyRegHelper<ShadowSceneProxy&, ShadowScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<ShadowScene>(), group);


      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(ShadowMapType, "ShadowMapType", "Shadow Map Type",
         "Determines the technique to render the shadows.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(ShadowResolution, "ShadowResolution", "Shadow Resolution",
         "Determines the resolution of the shadow map.",
         PropRegHelperType, propRegHelper);

   }

   void ShadowSceneProxy::CreateDrawable()
   {
      dtCore::RefPtr<ShadowScene> es = new ShadowScene();
      SetDrawable(*es);
   }

   bool ShadowSceneProxy::IsPlaceable() const
   {
      return false;
   }



   
}//namespace dtRender
