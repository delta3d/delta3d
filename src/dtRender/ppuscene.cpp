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

#include <dtRender/ppuscene.h>

#include <dtCore/observerptr.h>

#include <dtCore/camera.h>

#include <osg/Camera>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>

namespace dtRender
{

   const dtCore::RefPtr<SceneType> PPUScene::PPU_SCENE(new SceneType("PPU Scene", "Scene", "Supports osgPPU scene construction."));

   class PPUSceneImpl
   {
   public:
      PPUSceneImpl()
      {
         mPPUProcessor = new osgPPU::Processor();

      }
      
      ~PPUSceneImpl()
      {

      }

      dtCore::RefPtr<osgPPU::Processor> mPPUProcessor;

      dtCore::ObserverPtr<osgPPU::Unit> mFirstUnit;
      dtCore::ObserverPtr<osgPPU::Unit> mLastUnit;

   };

   PPUScene::PPUScene()
   : BaseClass(*PPU_SCENE, SceneEnum::PRE_RENDER)
   , mImpl(new PPUSceneImpl())
   {
      SetName("PPUScene");
      
   }

   PPUScene::PPUScene(const SceneType& sceneId, const SceneEnum& defaultScene)
      : BaseClass(sceneId, defaultScene)
      , mImpl(new PPUSceneImpl())
   {
      SetName("PPUScene");

   }


   PPUScene::~PPUScene()
   {
      delete mImpl;
   }


   void PPUScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      
   }

   osg::Group* PPUScene::GetSceneNode()
   {
      return mImpl->mPPUProcessor.get();
   }

   const osg::Group* PPUScene::GetSceneNode() const
   {
      return mImpl->mPPUProcessor.get();
   }

   osgPPU::Processor* PPUScene::GetPPUProcessor()
   {
      return mImpl->mPPUProcessor.get();
   }

   const osgPPU::Processor* PPUScene::GetPPUProcessor() const
   {
      return mImpl->mPPUProcessor.get();
   }

   osgPPU::Unit* PPUScene::GetFirstUnit()
   {
      return mImpl->mFirstUnit.get();
   }

   const osgPPU::Unit* PPUScene::GetFirstUnit() const
   {
      return mImpl->mFirstUnit.get();
   }

   osgPPU::Unit* PPUScene::GetLastUnit()
   {
      return mImpl->mLastUnit.get();
   }

   const osgPPU::Unit* PPUScene::GetLastUnit() const
   {
      return mImpl->mLastUnit.get();

   }

   osg::Camera* PPUScene::GetCamera()
   {
      return mImpl->mPPUProcessor->getCamera();
   }

   const osg::Camera* PPUScene::GetCamera() const
   {
      return mImpl->mPPUProcessor->getCamera();
   }

   void PPUScene::SetCamera( osg::Camera& cam)
   {
      mImpl->mPPUProcessor->setCamera(&cam);
      mImpl->mPPUProcessor->dirtyUnitSubgraph();

   }

   void PPUScene::SetFirstUnit( osgPPU::Unit& u)
   {
      mImpl->mFirstUnit = &u;
   }

   void PPUScene::SetLastUnit( osgPPU::Unit& u)
   {
      mImpl->mLastUnit = &u;
   }

   
}//namespace dtRender
