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
*/

#include <dtVoxel/volumescene.h>
#include <dtUtil/log.h>
#include <dtRender/scenemanager.h>

#include <osg/Group>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgVolume/VolumeScene>


namespace dtVoxel
{

   const dtCore::RefPtr<dtRender::SceneType> VolumeScene::VOLUME_SCENE(new dtRender::SceneType("VolumeScene", "VolumeScene", "Use this scene to render OSG Volumes."));


   VolumeScene::VolumeScene()
   : BaseClass(*VOLUME_SCENE, dtRender::SceneEnum::DEFAULT_SCENE)
   , mNode(new osg::Group())
   {
      SetName("VolumeScene");

      {
         osg::StateSet* pSS = mNode->getOrCreateStateSet();

         // required by osg 320, cheers.
         pSS->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      }
   }


   VolumeScene::~VolumeScene()
   {
      mNode = NULL;
   }

   void VolumeScene::CreateScene(dtRender::SceneManager& sm, const dtRender::GraphicsQuality& g)
   {
      mNode = new osgVolume::VolumeScene();

      sm.PushScene(*this);
   }

   osg::Group* VolumeScene::GetSceneNode()
   {
      return mNode.get();
   }

   const osg::Group* VolumeScene::GetSceneNode() const
   {
      return mNode.get();
   }

   VolumeSceneActor::VolumeSceneActor()
   {
   }

   VolumeSceneActor::~VolumeSceneActor()
   {
   }

   void VolumeSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void VolumeSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<VolumeScene> es = new VolumeScene();
      SetDrawable(*es);
   }

   bool VolumeSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtVoxel
