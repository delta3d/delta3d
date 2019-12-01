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

#include <dtRender/guiscene.h>

#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>

#include <osg/Group>

#include <osg/StateSet>


namespace dtRender
{

   const dtCore::RefPtr<SceneType> GUIScene::GUI_SCENE(new SceneType("GUI Scene", "Scene", "An orthographic scene rendered on top of the main pass."));


   GUIScene::GUIScene()
   : BaseClass(*GUI_SCENE, SceneEnum::FOREGROUND)
   , mNode(new osg::Group())
   {
      SetName("GUIScene");
   }


   GUIScene::~GUIScene()
   {
      mNode = NULL;
   }

   void GUIScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      mNode->setNodeMask(dtUtil::NodeMask::FOREGROUND);
   }

   osg::Group* GUIScene::GetSceneNode()
   {
      return mNode.get();
   }

   const osg::Group* GUIScene::GetSceneNode() const
   {
      return mNode.get();
   }

   GUISceneActor::GUISceneActor()
   {
   }

   GUISceneActor::~GUISceneActor()
   {
   }

   void GUISceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void GUISceneActor::CreateDrawable()
   {
      dtCore::RefPtr<GUIScene> es = new GUIScene();
      SetDrawable(*es);
   }

   bool GUISceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender
