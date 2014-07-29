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

#include <dtRender/simplescene.h>

#include <dtUtil/log.h>

#include <osg/Group>

#include <osg/StateSet>


namespace dtRender
{

   const dtCore::RefPtr<SceneType> SimpleScene::SIMPLE_SCENE(new SceneType("Scene", "Scene", "The bare minimum scene."));


   SimpleScene::SimpleScene()
   : BaseClass(*SIMPLE_SCENE, SceneEnum::DEFAULT_SCENE)
   , mNode(new osg::Group())
   {
      SetName("SimpleScene");

      {
         osg::StateSet* pSS = mNode->getOrCreateStateSet();

         // required by osg 320, cheers.
         pSS->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      }
   }


   SimpleScene::~SimpleScene()
   {
      mNode = NULL;
   }

   void SimpleScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      
   }

   osg::Group* SimpleScene::GetSceneNode()
   {
      return mNode.get();
   }

   const osg::Group* SimpleScene::GetSceneNode() const
   {
      return mNode.get();
   }

   SimpleSceneProxy::SimpleSceneProxy()
   {
   }

   SimpleSceneProxy::~SimpleSceneProxy()
   {
   }

   void SimpleSceneProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void SimpleSceneProxy::CreateDrawable()
   {
      dtCore::RefPtr<SimpleScene> es = new SimpleScene();
      SetDrawable(*es);
   }

   bool SimpleSceneProxy::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender