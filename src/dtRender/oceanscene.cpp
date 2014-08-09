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

#include <dtActors/watergridactor.h>
#include <dtActors/engineactorregistry.h>

#include <dtGame/gamemanager.h> //to create water grid actor
#include <dtABC/application.h> //to get camera
#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>
#include <osg/Group>

#include <osg/StateSet>


namespace dtRender
{

   const dtCore::RefPtr<SceneType> OceanScene::OCEAN_SCENE(new SceneType("Ocean Scene", "Scene", "Creates water meshes, and allows for underwater effects."));


   OceanScene::OceanScene()
   : BaseClass(*OCEAN_SCENE, SceneEnum::TRANSPARENT_OBJECTS)
   , mNode(new osg::Group())
   {
      SetName("OceanScene");
      mNode->setNodeMask(dtUtil::NodeMask::WATER);
   }


   OceanScene::~OceanScene()
   {
      mNode = NULL;
   }

   void OceanScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
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
            SceneGroup* defaultScene = sm.GetSceneGroup(SceneEnum::BACKGROUND);

            if(defaultScene != NULL)
            {
               water->SetSceneCamera(gm->GetApplication().GetCamera());
               water->SetReflectionScene(sm.GetOSGNode());//defaultScene->GetOSGNode());
            }
            else
            {
               LOG_ERROR("No default scene! Was unable to set a reflection scene for the WaterGridActor.");
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
      return mNode.get();
   }

   const osg::Group* OceanScene::GetSceneNode() const
   {
      return mNode.get();
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