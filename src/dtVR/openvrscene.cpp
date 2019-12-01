/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2016, Chipper Chickadee Studios, LLC
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
#include <dtCore/camera.h>
#include <dtABC/application.h>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/StateSet>

#include <osgViewer/Viewer>


namespace dtVR
{

   const dtCore::RefPtr<dtRender::SceneType> OpenVRScene::VOLUME_SCENE(new dtRender::SceneType("OpenVRScene", "OpenVRScene", "Use this scene to render OSG Volumes."));


   OpenVRScene::OpenVRScene()
   : BaseClass(*VOLUME_SCENE, dtRender::SceneEnum::DEFAULT_SCENE)
   , mOpenVRViewer(new osg::Group())
   {
      SetName("OpenVRScene");

      {
         osg::StateSet* pSS = mOpenVRViewer->getOrCreateStateSet();

         // required by osg 320, cheers.
         pSS->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      }
   }


   OpenVRScene::~OpenVRScene()
   {
      mOpenVRViewer = NULL;
   }

   void OpenVRScene::CreateScene(dtRender::SceneManager& sm, const dtRender::GraphicsQuality& g)
   {

      if (!OpenVRDevice::hmdPresent())
      {
         osg::notify(osg::FATAL) << "Error: No valid HMD present!" << std::endl;
         return 1;
      }

      // Open the HMD
      float nearClip = 0.01f;
      float farClip = 10000.0f;
      float worldUnitsPerMetre = 1.0f;
      int samples = 4;
      mVRDevice = new OpenVRDevice(nearClip, farClip, worldUnitsPerMetre, samples);

      // Exit if we fail to initialize the HMD device
      if (!mVRDevice->hmdInitialized())
      {
         // The reason for failure was reported by the constructor.
         return 1;
      }

      osg::ref_ptr<OpenVRRealizeOperation> openvrRealizeOperation = new OpenVRRealizeOperation(openvrDevice);
      
      osgViewer::GraphicsWindow* gc = GetGameManager()->GetApplication().GetCamera()->GetWindow()->GetOsgViewerGraphicsWindow();
      
      gc->setRealizeOperation(openvrRealizeOperation.get());
      
      mOpenVRViewer = new OpenVRViewer(gc, mVRDevice, openvrRealizeOperation);
      openvrViewer->addChild(loadedModel);

      sm.PushScene(*this);
   }

   osg::Group* OpenVRScene::GetSceneNode()
   {
      return mOpenVRViewer.get();
   }

   const osg::Group* OpenVRScene::GetSceneNode() const
   {
      return mOpenVRViewer.get();
   }

   OpenVRSceneActor::OpenVRSceneActor()
   {
   }

   OpenVRSceneActor::~OpenVRSceneActor()
   {
   }

   void OpenVRSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

   }

   void OpenVRSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<OpenVRScene> es = new OpenVRScene();
      SetDrawable(*es);
   }

   bool OpenVRSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtVR
