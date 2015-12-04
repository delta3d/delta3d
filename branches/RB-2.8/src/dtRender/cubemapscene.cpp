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

#include <dtRender/cubemapscene.h>
#include <dtRender/multipassscene.h>
#include <dtRender/ephemerisscene.h>

#include <dtUtil/nodemask.h>
#include <dtUtil/cullmask.h>


#include <dtCore/transform.h>
#include <dtCore/observerptr.h>
#include <dtCore/propertymacros.h>

#include <osg/Camera>
#include <osg/Texture2D>
#include <osg/ClampColor>
#include <osg/TextureCubeMap>
#include <osg/Depth>

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
#include <dtCore/camera.h>


namespace dtRender
{
   typedef std::vector< dtCore::RefPtr<osg::Camera> >  CameraArray;

   class UpdateCubeMapCameraCallback : public osg::NodeCallback
   {
   public:

      UpdateCubeMapCameraCallback(CubeMapScene& scene, SceneManager& sm, dtCore::Transformable& reflector, CameraArray& Cameras)
         : mLightDir(0.0, 0.0, 0.0)
         , mCubeMapScene(&scene)
         , mEphemerisScene(NULL)
         , mSceneManager(&sm)
         , mReflector(&reflector)
         , mNearPlane(1.0f)
         , mFarPlane(1000.0f)
         , mCameras(Cameras)
      {
      }

      void SetNearPlane(float np)
      {
         mNearPlane = np;
      }

      float GetNearPlane() const
      {
         return mNearPlane;
      }

      void SetFarPlane(float fp)
      {
         mFarPlane = fp;
      }

      float GetFarPlane() const
      {
         return mFarPlane;
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         // first update subgraph to make sure objects are all moved into position
         traverse(node,nv);

         int traversalNumber = nv->getTraversalNumber();

         if(!mEphemerisScene.valid())
         {
            mEphemerisScene = dynamic_cast<EphemerisScene*>(mSceneManager->FindSceneByType(*EphemerisScene::EPHEMERIS_SCENE));
         }

         if(mEphemerisScene.valid())
         {
            osg::Vec3 sunPos = mEphemerisScene->GetSunPosition();

            osg::Vec3 lightVector = sunPos;
            lightVector.normalize();

            float diff = mLightDir * lightVector;

            if(diff < 0.98)
            {
               mCubeMapScene->SetLightChanged(true);
               mLightDir = lightVector;
            }
            else
            {
               mCubeMapScene->SetLightChanged(false);
            }
         }

         // compute the position of the center of the reflector subgraph
         dtCore::Transform xform;
         mReflector->GetTransform(xform);
         osg::Matrixd worldToLocal;
         
         osg::BoundingSphere bs = mReflector->GetOSGNode()->getBound();
         osg::Vec3 position = xform.GetTranslation();// bs.center();

         const osg::NodePathList& nodePathList = mReflector->GetOSGNode()->getParentalNodePaths();

         if(!nodePathList.empty())
         {
            const osg::NodePath& nodePath = nodePathList[0];

            worldToLocal.set(osg::computeWorldToLocal(nodePath));
         }

         typedef std::pair<osg::Vec3, osg::Vec3> ImageData;
         const ImageData id[] =
         {
            ImageData( osg::Vec3( 1,  0,  0), osg::Vec3( 0, -1,  0) ), // +X
            ImageData( osg::Vec3(-1,  0,  0), osg::Vec3( 0, -1,  0) ), // -X
            ImageData( osg::Vec3( 0,  1,  0), osg::Vec3( 0,  0,  1) ), // +Y
            ImageData( osg::Vec3( 0, -1,  0), osg::Vec3( 0,  0, -1) ), // -Y
            ImageData( osg::Vec3( 0,  0,  1), osg::Vec3( 0, -1,  0) ), // +Z
            ImageData( osg::Vec3( 0,  0, -1), osg::Vec3( 0, -1,  0) )  // -Z
         };



         for(unsigned int i=0; i < 6 && i<mCameras.size();++i)
         {
            osg::Matrix localOffset;
            localOffset.makeLookAt(position,position+id[i].first,id[i].second);

            osg::Matrix viewMatrix = /*worldToLocal */localOffset;

            mCameras[i]->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
            mCameras[i]->setProjectionMatrixAsFrustum(-1.0,1.0,-1.0,1.0,mNearPlane,mFarPlane);
            mCameras[i]->setViewMatrix(viewMatrix);
         }

         mCubeMapScene->SetTraversal(traversalNumber);
      }

      void SetEnableCameras(bool b)
      {
         for(unsigned int i=0; i < 6 && i<mCameras.size();++i)
         {
            mCameras[i]->setNodeMask(b ? ~0 : 0);
         }

      }

   protected:

      virtual ~UpdateCubeMapCameraCallback() {}
      
      osg::Vec3 mLightDir;
      dtCore::ObserverPtr<CubeMapScene> mCubeMapScene;
      dtCore::ObserverPtr<EphemerisScene> mEphemerisScene;
      dtCore::ObserverPtr<dtRender::SceneManager> mSceneManager;
      dtCore::ObserverPtr<dtCore::Transformable> mReflector;
      
      float mNearPlane;
      float mFarPlane;
      CameraArray mCameras;
   };


   class CubemapSceneImpl
   {
   public:
      CubemapSceneImpl()
      {

      }
      ~CubemapSceneImpl()
      {
         mRootNode = NULL;
         mCubeMapTexture = NULL;
         mCameraCallback = NULL;
      }

      dtCore::RefPtr<osg::Group> mRootNode;
      dtCore::RefPtr<osg::TextureCubeMap> mCubeMapTexture;
      dtCore::ObserverPtr<dtCore::Transformable> mTarget;
      dtCore::RefPtr<UpdateCubeMapCameraCallback> mCameraCallback;
   };

   const dtCore::RefPtr<SceneType> CubeMapScene::CUBEMAP_SCENE(new SceneType("Reflection Scene", "Scene", "Creates a Scene to render and project reflections."));

   CubeMapScene::CubeMapScene()
   : BaseClass(*CUBEMAP_SCENE, SceneEnum::MULTIPASS)
   , mTexWidth(512)
   , mTexHeight(512)
   , mNearPlane(1.0)
   , mFarPlane(25000.0)
   , mLODScale(1.0f)
   , mShowCubeMap(false)
   , mClearColor(0.0f, 0.0f, 0.0f, 0.0f)
   , mRenderEveryFrame(true)
   , mRenderOnLightChanged(true)
   , mTraversalMod(0)
   , mTraversalNumber(0)
   , mBypassTraversal(false)
   , mLightChanged(false)
   , mImpl(new CubemapSceneImpl())
   {
      SetName("CubeMapScene");  
   }


   CubeMapScene::~CubeMapScene()
   {
      delete mImpl;
      mImpl = NULL;
   }


   void CubeMapScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
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
         mImpl->mRootNode = new osg::Group;
         mImpl->mCubeMapTexture = new osg::TextureCubeMap;
         mImpl->mCubeMapTexture->setTextureSize(mTexWidth, mTexHeight);

         mImpl->mCubeMapTexture->setInternalFormat(GL_RGB);
         mImpl->mCubeMapTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
         mImpl->mCubeMapTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
         mImpl->mCubeMapTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
         mImpl->mCubeMapTexture->setFilter(osg::TextureCubeMap::MIN_FILTER,osg::TextureCubeMap::LINEAR);
         mImpl->mCubeMapTexture->setFilter(osg::TextureCubeMap::MAG_FILTER,osg::TextureCubeMap::LINEAR);


         // set up the render to texture cameras.
         CameraArray Cameras;
         for(unsigned int i=0; i<6; ++i)
         {
            // create the camera
            dtCore::RefPtr<osg::Camera> camera = new osg::Camera;

            camera->setLODScale(mLODScale);
            camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            camera->setClearColor(mClearColor);
            
            ////DEBUG- just renders cube maps on top of screen
            if(mShowCubeMap)
            {               
               static int startX = 0;
               // set viewport
               camera->setViewport(startX,0, mTexWidth, mTexHeight);
               startX += mTexWidth + 15;
               // set the camera to render before the main camera.
               camera->setRenderOrder(osg::Camera::POST_RENDER);

            }
            else
            {
               // set viewport
               camera->setViewport(0,0, mTexWidth, mTexHeight);
               // set the camera to render before the main camera.
               camera->setRenderOrder(osg::Camera::PRE_RENDER, 1000 + i);

               // tell the camera to use OpenGL frame buffer object where supported.
               camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

               // attach the texture and use it as the color buffer.
               camera->attach(osg::Camera::COLOR_BUFFER, mImpl->mCubeMapTexture, 0, i);

            }

            camera->setCullMask(dtUtil::CullMask::MULTIPASS_REFLECTION_MASK);
            // add subgraph to render
            camera->addChild(sm.GetOSGNode());

            //mImpl->mRootNode->addChild(camera);
            sm.GetSceneCamera()->GetOSGCamera()->addChild(camera);
            
            
            Cameras.push_back(camera);
         }


         //if we dont have a target assigned use the main scene camera
         if(!mImpl->mTarget.valid())
         {
            mImpl->mTarget = sm.GetSceneCamera();
         }

         mImpl->mCameraCallback = new UpdateCubeMapCameraCallback(*this, sm, *mImpl->mTarget, Cameras);

         mImpl->mRootNode->setUpdateCallback(mImpl->mCameraCallback.get());

         //set default reflection uniform
         osg::StateSet* mainSceneSS = sm.GetOSGNode()->getOrCreateStateSet();
         osg::Uniform* uniform = mainSceneSS->getOrCreateUniform(MultipassScene::REFLECTION_TEXTURE_UNIFORM, osg::Uniform::SAMPLER_CUBE);
         uniform->set(MultipassScene::TEXTURE_UNIT_REFLECTION);

         mainSceneSS->setTextureAttributeAndModes(MultipassScene::TEXTURE_UNIT_REFLECTION, mImpl->mCubeMapTexture.get(), osg::StateAttribute::ON);

      }
      else
      {
         LOG_ERROR("Must have a valid Multipass Scene to use CubeMapScene.");
      }
   }

   osg::Group* CubeMapScene::GetSceneNode()
   {
      return mImpl->mRootNode;
   }

   const osg::Group* CubeMapScene::GetSceneNode() const
   {
      return mImpl->mRootNode;
   }

   void CubeMapScene::SetTarget( dtCore::Transformable& xform)
   {
      mImpl->mTarget = &xform;
   }

   const dtCore::Transformable& CubeMapScene::SetTarget() const
   {
      return *mImpl->mTarget;
   }

   void CubeMapScene::SetTraversal( int num )
   {
      if( mTraversalNumber == num)
         return;

      mTraversalNumber = num;

      if(!mRenderEveryFrame)
      {
         mBypassTraversal = true;

         if(mRenderOnLightChanged) 
         {
            mBypassTraversal = !mLightChanged;
         }

         //0 and 1 are special case for even and odd
         if(mTraversalMod == 0 || mTraversalMod == 1)
         {
            mBypassTraversal = (num % 2) == mTraversalMod;
         }
         else if(num % mTraversalMod == 0)
         {
            mBypassTraversal = false;
         }

         mImpl->mCameraCallback->SetEnableCameras(!mBypassTraversal);
      }
    }

   CubeMapSceneActor::CubeMapSceneActor()
   {
   }

   CubeMapSceneActor::~CubeMapSceneActor()
   {
   }

   void CubeMapSceneActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      std::string group("CubeMapScene");
      typedef dtCore::PropertyRegHelper<CubeMapSceneActor&, CubeMapScene> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, GetDrawable<CubeMapScene>(), group);

      DT_REGISTER_PROPERTY_WITH_NAME(TexWidth, "TexWidth", "The width of the render target.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(TexHeight, "TexHeight", "The height of the render target.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(NearPlane, "NearPlane", "The near plane to use for the cubemap cameras.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(FarPlane, "FarPlane", "The far plane to use for the cubemap cameras.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(LODScale, "LODScale", "The LOD scale can be used to enhance performance.", PropRegHelperType, propRegHelper);
      
      DT_REGISTER_PROPERTY_WITH_NAME(ShowCubeMap, "ShowCubeMap", "A debug setting for rendering the cubemap cameras on screen.", PropRegHelperType, propRegHelper);
      DT_REGISTER_PROPERTY_WITH_NAME(ClearColor, "ClearColor", "The clear color for the cubemap cameras.", PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(RenderEveryFrame, "RenderEveryFrame", "Render Every Frame",
         "Setting this to false allows the shadow rendering to only happen on traversal mod frames.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(RenderOnLightChanged, "RenderOnLightChanged", "Render On Light Changed",
         "If it is not rendering every frame, setting this to true forces the shadow rendering to happen when the time changes.",
         PropRegHelperType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(TraversalMod, "TraversalMod", "Traversal Mod",
         "Allows the shadow rendering to only happen on traversal mod frames, must set RenderEveryFrame to false.",
         PropRegHelperType, propRegHelper);

   }

   void CubeMapSceneActor::CreateDrawable()
   {
      dtCore::RefPtr<CubeMapScene> es = new CubeMapScene();

      SetDrawable(*es);
   }

   bool CubeMapSceneActor::IsPlaceable() const
   {
      return false;
   }

   
}//namespace dtRender

