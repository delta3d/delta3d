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

#include <dtRender/oceanscene.h>
#include <dtRender/scenemanager.h>
#include <dtRender/scenegroup.h>
#include <dtRender/multipassscene.h>
#include <dtRender/uniformactcomp.h>

#include <dtRender/ppuscene.h>

#include <dtActors/engineactorregistry.h>

#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamvec4.h>

#include <dtGame/gamemanager.h> //to create water grid actor
#include <dtABC/application.h> //to get camera
#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>
#include <osg/Group>
#include <osgPPU/UnitDepthbufferBypass.h>
#include <osg/StateSet>


namespace dtRender
{

   const dtCore::RefPtr<SceneType> OceanScene::OCEAN_SCENE(new SceneType("Ocean Scene", "Scene", "Creates water meshes, and allows for underwater effects."));

   const dtUtil::RefString OceanScene::UNIFORM_WATER_HEIGHT("WaterHeight");
   const dtUtil::RefString OceanScene::UNIFORM_WATER_COLOR("WaterColor");
   const dtUtil::RefString OceanScene::UNIFORM_UNDERWATER_VIEW_DISTANCE("UnderWaterViewDistance");

   class OceanUniforms
   {
   public:
      OceanUniforms()
      {

      }
      ~OceanUniforms()
      {

      }

      dtCore::RefPtr<dtCore::ShaderParamFloat> mWaterHeight;
      dtCore::RefPtr<dtCore::ShaderParamFloat> mUnderWaterViewDistance;

      dtCore::RefPtr<dtCore::ShaderParamVec4> mWaterColor;
   };


   class OceanResizeCallback: public MultipassScene::ResizeCallback
   {
      public:
         OceanResizeCallback(OceanScene* ocean)
            : mOceanScene(ocean)
         {

         }

         void OnResize(MultipassScene& mps, int width, int height)
         {
            mOceanScene->OnResize(mps, width, height);
         }

      private:
         dtCore::ObserverPtr<OceanScene> mOceanScene;
   };



   class OceanSceneImpl
   {
   public:
      OceanSceneImpl()
         : mUseMultipassWater(true)
         , mUniforms(new OceanUniforms())
      {


      }

      ~OceanSceneImpl()
      {
         mRootNode = NULL;

         delete mUniforms;
         mUniforms = NULL;
      }

      bool mUseMultipassWater;
      OceanUniforms* mUniforms;
      dtCore::RefPtr<osg::Group> mRootNode;
      dtCore::ObserverPtr<dtActors::WaterGridActor> mWaterActor;
   };


   OceanScene::OceanScene()
   : BaseClass(*OCEAN_SCENE, SceneEnum::TRANSPARENT_OBJECTS)
   , mNumRows(200)
   , mNumColumns(200)
   , mWaveDirection(0.0f)
   , mAmplitudeModifier(1.0f)
   , mWavelengthModifier(1.0)
   , mSpeedModifier(1.0)
   , mUnderWaterViewDistance(15.0)
   , mWaterColor(0.117187, 0.3125, 0.58593, 1.0)
   , mReflectionMapResolution(512, 512)
   , mTexWaveTextureResolution(1024, 1024)
   , mTexWaveResolutionScalar(2.0)
   , mTexWaveAmpScalar(1.0)
   , mTexWaveSpreadScalar(1.15)
   , mTexWaveSteepness(2.0)
   , mSeaState(&dtActors::WaterGridActor::SeaState::SeaState_4)
   , mChop(&dtActors::WaterGridActor::ChopSettings::CHOP_FLAT)
   , mUseDebugKeys(false)
   , mImpl(new OceanSceneImpl())
   {
      SetName("OceanScene");
   }


   OceanScene::~OceanScene()
   {
      delete mImpl;
      mImpl = NULL;
   }

   void OceanScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      mImpl->mRootNode = new osg::Group();
      mImpl->mRootNode->setNodeMask(dtUtil::NodeMask::WATER);

      dtGame::GameManager* gm = sm.GetGameManager();
      if(gm != NULL)
      {
         dtActors::WaterGridActor* water = NULL;
         dtCore::RefPtr<dtActors::WaterGridActorProxy> waterProxy;

         gm->CreateActor(*dtActors::EngineActorRegistry::WATER_GRID_ACTOR_TYPE, waterProxy);
         if (waterProxy.valid())
         {
            waterProxy->GetDrawable(water);
            mImpl->mWaterActor = water;
         
            //get reflection scene root node
            water->SetSceneCamera(gm->GetApplication().GetCamera());

            if(mImpl->mUseMultipassWater)
            {
               MultipassScene* mps = dynamic_cast<MultipassScene*>(sm.FindSceneByType(*MultipassScene::MULTIPASS_SCENE));

               //try to create default multipass scene
               if(mps == NULL)
               {
                  sm.CreateDefaultMultipassScene();
                  mps = dynamic_cast<MultipassScene*>(sm.FindSceneByType(*MultipassScene::MULTIPASS_SCENE));
               }

               if(mps != NULL && mps->GetEnablePreDepthPass())
               {
                  //bind the result of the pre depth texture to the scene
                  dtCore::RefPtr<osg::Uniform> depthTextureUniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, MultipassScene::UNIFORM_PREDEPTH_TEXTURE);
                  depthTextureUniform->setDataVariance(osg::Object::DYNAMIC);
                  depthTextureUniform->set(MultipassScene::TEXTURE_UNIT_PREDEPTH);

                  osg::StateSet* ss = /*water->*/GetOSGNode()->getOrCreateStateSet();
                  ss->addUniform(depthTextureUniform);
                  ss->setTextureAttributeAndModes(MultipassScene::TEXTURE_UNIT_PREDEPTH, mps->GetPreDepthTexture(), osg::StateAttribute::ON);

                  OceanResizeCallback* orc = new OceanResizeCallback(this);
                  mps->AddResizeCallback(*orc);
               }
            }
            //currently not creating multipass reflection scene
            //else
            {
               water->SetReflectionScene(sm.GetOSGNode());
            }

            //set properties
            {
               water->SetNumRows(mNumRows);
               water->SetNumColumns(mNumColumns);
               water->SetModForDirectionInDegrees(mWaveDirection);
               water->SetModForAmplitude(mAmplitudeModifier);
               water->SetModForWaveLength(mWavelengthModifier);
               water->SetModForSpeed(mSpeedModifier);
               water->SetUnderWaterViewDistance(mUnderWaterViewDistance);
               water->SetWaterColor(mWaterColor);
               water->SetReflectionMapResolution(mReflectionMapResolution);
               water->SetTexWaveTextureResolution(mTexWaveTextureResolution);
               water->SetTexWaveResolutionScalar(mTexWaveResolutionScalar);
               water->SetTexWaveAmpScalar(mTexWaveAmpScalar);
               water->SetTexWaveSpreadScalar(mTexWaveSpreadScalar);
               water->SetTexWaveSteepness(mTexWaveSteepness);
               water->SetChop(mChop);
               water->SetSeaState(mSeaState);
               water->SetUseDebugKeys(mUseDebugKeys);
            }

            //set scene uniforms for water things besides water will need

            UniformActComp* uniformActComp = sm.GetOwner()->GetComponent<UniformActComp>();
            if(uniformActComp != NULL)
            {
               mImpl->mUniforms->mWaterHeight = new dtCore::ShaderParamFloat(UNIFORM_WATER_HEIGHT);
               mImpl->mUniforms->mUnderWaterViewDistance = new dtCore::ShaderParamFloat(UNIFORM_UNDERWATER_VIEW_DISTANCE);
               mImpl->mUniforms->mWaterColor = new dtCore::ShaderParamVec4(UNIFORM_WATER_COLOR);

               mImpl->mUniforms->mWaterColor->SetValue(mWaterColor);
               mImpl->mUniforms->mWaterColor->Update();

               mImpl->mUniforms->mWaterHeight->SetValue(0.0f);
               mImpl->mUniforms->mWaterHeight->Update();

               mImpl->mUniforms->mUnderWaterViewDistance->SetValue(mUnderWaterViewDistance);
               mImpl->mUniforms->mUnderWaterViewDistance->Update();


               uniformActComp->AddParameter(*mImpl->mUniforms->mWaterHeight);
               uniformActComp->AddParameter(*mImpl->mUniforms->mUnderWaterViewDistance);
               uniformActComp->AddParameter(*mImpl->mUniforms->mWaterColor);
            }



            //this adds the actor to our scene
            sm.PushScene(*this);
         
            gm->AddActor(*waterProxy, false, false);
         
            sm.PopScene();
         }
      }
      else
      {
         LOG_ERROR("GM is NULL, cannot create water actor.");
      }

   }

   osg::Group* OceanScene::GetSceneNode()
   {
      return mImpl->mRootNode.get();
   }

   const osg::Group* OceanScene::GetSceneNode() const
   {
      return mImpl->mRootNode.get();
   }


   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, int, NumRows);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, int, NumColumns);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, bool, UseDebugKeys);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, WaveDirection);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, AmplitudeModifier);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, WavelengthModifier);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, SpeedModifier);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, UnderWaterViewDistance);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, TexWaveResolutionScalar);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, TexWaveAmpScalar);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, TexWaveSpreadScalar);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, float, TexWaveSteepness);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, osg::Vec4, WaterColor);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, dtUtil::EnumerationPointer<dtActors::WaterGridActor::SeaState>, SeaState);
   DT_IMPLEMENT_ACCESSOR_GETTER(OceanScene, dtUtil::EnumerationPointer<dtActors::WaterGridActor::ChopSettings>, Chop);

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetUseDebugKeys(bool b)
   {
      mUseDebugKeys = b;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetUseDebugKeys(b);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetTexWaveAmpScalar(float f)
   {
      mTexWaveAmpScalar = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetTexWaveAmpScalar(f);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetTexWaveSpreadScalar(float f)
   {
      mTexWaveSpreadScalar = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetTexWaveSpreadScalar(f);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetTexWaveSteepness(float f)
   {
      mTexWaveSteepness = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetTexWaveSteepness(f);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetTexWaveResolutionScalar(float f)
   {
      mTexWaveResolutionScalar = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetTexWaveResolutionScalar(f);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetUnderWaterViewDistance(float f)
   {
      mUnderWaterViewDistance = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetUnderWaterViewDistance(f);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetNumRows(int i)
   {
      mNumRows = i;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetNumRows(mNumRows);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetNumColumns(int i)
   {
      mNumColumns = i;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetNumColumns(mNumColumns);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetWaveDirection(float f)
   {
      mWaveDirection = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetModForDirectionInDegrees(mWaveDirection);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetWavelengthModifier(float f)
   {
      mWavelengthModifier = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetModForWaveLength(mWavelengthModifier);
      }      
   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetAmplitudeModifier(float f)
   {
      mAmplitudeModifier = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetModForAmplitude(mAmplitudeModifier);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetSpeedModifier(float f)
   {
      mSpeedModifier = f;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetModForSpeed(f);
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetWaterColor(const osg::Vec4& color_)
   {
      mWaterColor = color_;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetWaterColor(color_);
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetSeaState(dtActors::WaterGridActor::SeaState& seaState_)
   {
      mSeaState = seaState_;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetSeaState(seaState_);
      }
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void OceanScene::SetChop(dtActors::WaterGridActor::ChopSettings&  chop_)
   {
      mChop = chop_;
      if(mImpl->mWaterActor.valid())
      {
         mImpl->mWaterActor->SetChop(chop_);
      }
   }

   void OceanScene::OnResize( MultipassScene& mps, int width, int height )
   {
      if(mps.GetEnablePreDepthPass())
      {
         osg::StateSet* ss = /*water->*/GetOSGNode()->getOrCreateStateSet();

         //bind the result of the pre depth texture to the scene
         osg::Uniform* depthTextureUniform = ss->getOrCreateUniform(MultipassScene::UNIFORM_PREDEPTH_TEXTURE, osg::Uniform::SAMPLER_2D);
         depthTextureUniform->setDataVariance(osg::Object::DYNAMIC);
         depthTextureUniform->set(MultipassScene::TEXTURE_UNIT_PREDEPTH);

         
         ss->setTextureAttributeAndModes(MultipassScene::TEXTURE_UNIT_PREDEPTH, mps.GetPreDepthTexture(), osg::StateAttribute::ON);

      }
   }


   /////////////////////////////////////////////////////////////////////////////
   //actor
   OceanSceneActor::OceanSceneActor()
   {
   }

   OceanSceneActor::~OceanSceneActor()
   {
   }

   void OceanSceneActor::BuildPropertyMap()
   {
      const std::string GROUPNAME = "OceanScene";

      BaseClass::BuildPropertyMap();

      OceanScene* actor = GetDrawable<OceanScene>();

      typedef dtCore::PropertyRegHelper<dtCore::PropertyContainer, OceanScene> RegHelperType;
      RegHelperType propReg(*this, actor, GROUPNAME);

      DT_REGISTER_PROPERTY(NumRows, "The number of rows of tesselation for the water surface.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(NumColumns, "The number of columns of tesselation for the water surface.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(WaveDirection, "The direction of the waves.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(AmplitudeModifier, "A scalar for the amplitude.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(WavelengthModifier, "A scalar for the wave length.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(SpeedModifier, "A scalar for the speed.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(UnderWaterViewDistance, "How far you can see under water.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(SeaState, "A value representing the overall sea state based on the beaufort scale 1-12.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(Chop, "A value representing the overall chop of the waves.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(ReflectionMapResolution, "The resolution of the render to texture reflection map.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(TexWaveTextureResolution, "The resolution of the render to texture texture wave map.", RegHelperType, propReg);

      DT_REGISTER_PROPERTY(TexWaveResolutionScalar, "A value to scale the rendered resolution of the texture waves.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(TexWaveAmpScalar, "A value to scale amplitude of the texture waves.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(TexWaveSpreadScalar, "A value to scale the variation in angle of the texture waves.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(TexWaveSteepness, "A value to scale the steepness of the texture waves.", RegHelperType, propReg);


      DT_REGISTER_PROPERTY(UseDebugKeys, "These activate the debug keys for the water, keys- 1-9, TAB, Space, Return, Home, End, PG Up, and PG Down.", RegHelperType, propReg);


      AddProperty(new dtCore::ColorRgbaActorProperty("Water Color", "Water Color",
         dtCore::ColorRgbaActorProperty::SetFuncType(actor, &OceanScene::SetWaterColor),
         dtCore::ColorRgbaActorProperty::GetFuncType(actor,&OceanScene::GetWaterColor),
         "Sets the color of the water.", GROUPNAME));

   }

   void OceanSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<OceanScene> es = new OceanScene();
      SetDrawable(*es);
   }

   bool OceanSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender
