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

#include <dtRender/ppuscene.h>

#include <dtActors/watergridactor.h>
#include <dtActors/engineactorregistry.h>

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

   //class DepthPassScene : public PPUScene
   //{
   //public:
   //   typedef PPUScene BaseClass;
   //   static const dtCore::RefPtr<SceneType> DEPTH_PASS_SCENE;

   //public:
   //   DepthPassScene();
   //   virtual ~DepthPassScene();

   //   virtual osg::Group* GetSceneNode();
   //   virtual const osg::Group* GetSceneNode() const;

   //   virtual void CreateScene(SceneManager&, const GraphicsQuality&);

   //private:
   //   dtCore::RefPtr<osg::Group> mRootNode;
   //};


   class OceanSceneImpl
   {
   public:
      OceanSceneImpl()
         : mUseMultipassWater(true)
      {


      }

      ~OceanSceneImpl()
      {
         mRootNode = NULL;
      }

      bool mUseMultipassWater;
      dtCore::RefPtr<osg::Group> mRootNode;
   };


   OceanScene::OceanScene()
   : BaseClass(*OCEAN_SCENE, SceneEnum::TRANSPARENT_OBJECTS)
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

                  osg::StateSet* ss = water->GetOSGNode()->getOrCreateStateSet();
                  ss->addUniform(depthTextureUniform);
                  ss->setTextureAttributeAndModes(MultipassScene::TEXTURE_UNIT_PREDEPTH, mps->GetPreDepthTexture(), osg::StateAttribute::ON);

               }
            }
            //currently not creating multipass reflection scene
            //else
            {
               water->SetReflectionScene(sm.GetOSGNode());
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


   OceanSceneProxy::OceanSceneProxy()
   {
   }

   OceanSceneProxy::~OceanSceneProxy()
   {
   }

   void OceanSceneProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void OceanSceneProxy::CreateDrawable()
   {
      dtCore::RefPtr<OceanScene> es = new OceanScene();
      SetDrawable(*es);
   }

   bool OceanSceneProxy::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender
