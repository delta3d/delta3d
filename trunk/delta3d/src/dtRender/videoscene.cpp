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
*
*  Portions of this code were taken from the osgPPU examples
*     please see-     http://projects.tevs.eu/osgppu/ 
*
*/

#include <dtRender/videoscene.h>
#include <dtRender/multipassscene.h>

#include <dtUtil/nodemask.h>

#include <dtCore/observerptr.h>
#include <dtCore/propertymacros.h>
#include <dtCore/project.h>

#include <osg/Camera>
#include <osg/Texture2D>
#include <osg/ClampColor>
#include <osg/Image>
#include <osg/ImageStream>
#include <osg/Geode>
#include <osg/MatrixTransform>

#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>
#include <osgPPU/UnitTexture.h>
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
#include <dtCore/camera.h>


namespace dtRender
{


   //--------------------------------------------------------------------------
   // create a square with center at 0,0,0 and aligned along the XZ plan
   //--------------------------------------------------------------------------
   osg::Drawable* createSquare(float textureCoordMax=1.0f)
   {
      // set up the Geometry.
      osg::Geometry* geom = new osg::Geometry;

      osg::Vec3Array* coords = new osg::Vec3Array(4);
      (*coords)[0].set(-1.25f,0.0f,1.0f);
      (*coords)[1].set(-1.25f,0.0f,-1.0f);
      (*coords)[2].set(1.25f,0.0f,-1.0f);
      (*coords)[3].set(1.25f,0.0f,1.0f);
      geom->setVertexArray(coords);

      osg::Vec3Array* norms = new osg::Vec3Array(1);
      (*norms)[0].set(0.0f,-1.0f,0.0f);
      geom->setNormalArray(norms);
      geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

      osg::Vec2Array* tcoords = new osg::Vec2Array(4);
      (*tcoords)[0].set(0.0f,0.0f);
      (*tcoords)[1].set(0.0f,textureCoordMax);
      (*tcoords)[2].set(textureCoordMax,textureCoordMax);
      (*tcoords)[3].set(textureCoordMax,0.0f);
      geom->setTexCoordArray(0,tcoords);

      geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

      return geom;
   }



   class VideoSceneImpl
   {
   public:
      VideoSceneImpl()
      {

      }

      ~VideoSceneImpl()
      {
         mVideoScene = NULL;
         mVideoTexture = NULL;
      }

      dtCore::RefPtr<osg::Group> mVideoScene;
      dtCore::RefPtr<osg::Texture2D> mVideoTexture;
   };

   const dtCore::RefPtr<SceneType> VideoScene::VIDEO_SCENE(new SceneType("Video Scene", "Scene", "Creates a Scene to play a video to texture."));

   VideoScene::VideoScene()
   : BaseClass(*VIDEO_SCENE, SceneEnum::MULTIPASS)
   , mVideoFile("textures/videos/testVideo.avi")
   , mLooping(true)
   , mImpl(new VideoSceneImpl())
   {
      SetName("VideoScene");  
   }


   VideoScene::~VideoScene()
   {
      delete mImpl;
      mImpl = NULL;
   }


   void VideoScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
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
         
         std::string videoFile = dtCore::Project::GetInstance().GetResourcePath(GetVideoFile());
         if(videoFile.empty())
         {
            LOG_ERROR("Cannot create video scene, unable to find video file.");
            return;
         }

         dtCore::RefPtr<osg::Image> image = osgDB::readImageFile(videoFile);
         if (image == NULL)
         {
            LOG_ERROR("Cannot create video scene, unable to read video file, check for missing plugins.");
            return;
         }

         osg::ImageStream* videoStream = dynamic_cast<osg::ImageStream*>(image.get());
         if (videoStream)
         {
            if(mLooping)
            {
               videoStream->setLoopingMode(osg::ImageStream::LOOPING);
            }
            else
            {
               videoStream->setLoopingMode(osg::ImageStream::NO_LOOPING);
            }

            videoStream->play();
         

            // setup scene
            mImpl->mVideoScene = new osg::Group();         
            dtCore::RefPtr<osg::Geode> gQuad = new osg::Geode();
            gQuad->addDrawable(createSquare());
            mImpl->mVideoScene->addChild(gQuad);
            sm.GetOSGNode()->asGroup()->addChild(mImpl->mVideoScene);

            // setup texture which will hold the video file
            mImpl->mVideoTexture = new osg::Texture2D();
            mImpl->mVideoTexture->setResizeNonPowerOfTwoHint(false);
            mImpl->mVideoTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
            mImpl->mVideoTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
            mImpl->mVideoTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_EDGE);
            mImpl->mVideoTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_EDGE);
            mImpl->mVideoTexture->setImage(image.get());
            gQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0, mImpl->mVideoTexture);

            // create osgPPU's units and processor         
            osgPPU::UnitTexture* unitTexture = new osgPPU::UnitTexture(mImpl->mVideoTexture);
         

            //set default reflection uniform
            osg::StateSet* mainSceneSS = sm.GetOSGNode()->getOrCreateStateSet();
            osg::Uniform* uniform = mainSceneSS->getOrCreateUniform("d3d_VideoTexture", osg::Uniform::SAMPLER_2D);
            uniform->set(2);

            mainSceneSS->setTextureAttributeAndModes(2, mImpl->mVideoTexture, osg::StateAttribute::ON);


            BaseClass::SetFirstUnit(*unitTexture);
            BaseClass::SetLastUnit(*unitTexture);
         }
         else
         {
            LOG_ERROR("Invalid video stream, file was found and loaded but the video stream was NULL");
         }
      }
      else
      {
         LOG_ERROR("Must have a valid Multipass Scene to use Video Scene.");
      }
   }



   osg::Group* VideoScene::GetSceneNode()
   {
      return mImpl->mVideoScene;
   }

   const osg::Group* VideoScene::GetSceneNode() const
   {
      return mImpl->mVideoScene;
   }


   //proxy
   VideoSceneActor::VideoSceneActor()
   {
   }

   VideoSceneActor::~VideoSceneActor()
   {
   }

   void VideoSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("VideoScene");
      typedef dtCore::PropertyRegHelper<VideoSceneActor&, VideoScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<VideoScene>(), group);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(VideoFile, "VideoFile", "Video File",
         "The video resource to playback.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(Looping, "Looping", "Looping",
         "Whether or not to look the video.",
         PropRegHelperType, propRegHelper);

   }

   void VideoSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<VideoScene> es = new VideoScene();

      SetDrawable(*es);
   }

   bool VideoSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

