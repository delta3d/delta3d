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

#include <dtRender/glowscene.h>
#include <dtRender/multipassscene.h>

#include <dtUtil/nodemask.h>

#include <dtCore/observerptr.h>

#include <osg/Camera>
#include <osg/Texture2D>
#include <osg/ClampColor>

#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>
#include <osgPPU/UnitCameraAttachmentBypass.h>
#include <osgPPU/UnitInOut.h>
#include <osgPPU/UnitOut.h>
#include <osgPPU/UnitInMipmapOut.h>
#include <osgPPU/UnitInResampleOut.h>
#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitText.h>

//needed to set the scene camera
#include <dtRender/scenemanager.h>
#include <dtGame/gamemanager.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>


namespace dtRender
{

 

   const dtCore::RefPtr<SceneType> GlowScene::GLOW_SCENE(new SceneType("Glow Scene", "Scene", "Creates an osgPPU Glow Scene."));

   GlowScene::GlowScene()
   : BaseClass(*GLOW_SCENE, SceneEnum::MULTIPASS)
   , mRootNode(new osg::Group())
   {
      SetName("GlowScene");  
   }


   GlowScene::~GlowScene()
   {
   }


   void GlowScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
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
         //sm.SetEnableHDR(true);

         //osgPPU::Unit* firstUnit = NULL;
         //osgPPU::Unit* lastUnit = NULL;

         //HDRRendering hdrBuilder;

         //hdrBuilder.createHDRPipeline(mps->GetColorBypass(), mps->GetResampleColor(), firstUnit, lastUnit);

         //BaseClass::SetFirstUnit(*firstUnit);
         //BaseClass::SetLastUnit(*lastUnit);

         ////the multipass unit connects its out to the last unit, 
         ////we need to undo this to to insert ourselves in the pipeline
         //dtCore::RefPtr<osgPPU::UnitOut> unitOut = mps->GetUnitOut();
         //if(unitOut->getParent(0) == mps->GetLastUnit() )
         //{
         //   mps->GetLastUnit()->removeChild(unitOut);
         //}

         //lastUnit->addChild(unitOut);
      }
      else
      {
         LOG_ERROR("Must have a valid Multipass Scene to use GlowScene.");
      }
   }

   osg::Group* GlowScene::GetSceneNode()
   {
      return mRootNode;
   }

   const osg::Group* GlowScene::GetSceneNode() const
   {
      return mRootNode;
   }

   GlowSceneActor::GlowSceneActor()
   {
   }

   GlowSceneActor::~GlowSceneActor()
   {
   }

   void GlowSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void GlowSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<GlowScene> es = new GlowScene();

      SetDrawable(*es);
   }

   bool GlowSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

