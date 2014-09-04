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

#include <dtRender/scenemanager.h>
#include <dtRender/scenegroup.h>
#include <dtRender/multipassscene.h>

#include <dtRender/simplescene.h>
#include <dtRender/shadowscene.h>
#include <dtRender/uniformactcomp.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <osg/MatrixTransform>
#include <osg/ClampColor>

#include <stack>
#include <dtUtil/nodemask.h>
#include <dtABC/application.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>

#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/shaderparamvec4.h>

//for frame and elapsed time
#include <dtCore/system.h>

namespace dtRender
{

   class SMDefaultUniforms
   {
   public:
      SMDefaultUniforms()
      {

      }
      ~SMDefaultUniforms()
      {

      }

      dtCore::RefPtr<dtCore::ShaderParamInt> mScreenWidth;
      dtCore::RefPtr<dtCore::ShaderParamInt> mScreenHeight;

      dtCore::RefPtr<dtCore::ShaderParamFloat> mNearPlane;
      dtCore::RefPtr<dtCore::ShaderParamFloat> mFarPlane;

      dtCore::RefPtr<dtCore::ShaderParamFloat> mFrameTime;
      dtCore::RefPtr<dtCore::ShaderParamFloat> mElapsedTime;

      dtCore::RefPtr<dtCore::ShaderParamFloat> mGamma;
      dtCore::RefPtr<dtCore::ShaderParamFloat> mBrightness;

      dtCore::RefPtr<dtCore::ShaderParamFloat> mSceneLuminance;
      dtCore::RefPtr<dtCore::ShaderParamFloat> mSceneAmbience;

      dtCore::RefPtr<dtCore::ShaderParamVec4> mMainCameraPos;
      dtCore::RefPtr<dtCore::ShaderParamVec4> mMainCameraHPR;

      //need a mat4 param
      dtCore::RefPtr<osg::Uniform> mMainCameraInverseViewMatrix;
      dtCore::RefPtr<osg::Uniform> mMainCameraInverseModelViewProjectionMatrix;
   };


   class SceneManagerImpl
   {
   public:

      SceneManagerImpl()
         : mCreateDefaultScene(true)
         , mCreateMultipassScene(true)
         , mEnableHDR(false)
         , mResize(false)
         , mGraphicsQuality(&GraphicsQuality::DEFAULT)
         , mMultipassScene(NULL)
         , mSceneCamera(NULL)
         , mUniforms(NULL)
         , mChildren()
         , mSceneStack()
      {
         mUniforms = new SMDefaultUniforms();
      }
      ~SceneManagerImpl()
      {
         delete mUniforms;

         //effectively clears without popping all the elements
         mSceneStack = std::stack<dtCore::ObserverPtr<SceneBase> >();
         
         mChildren.clear();

         mMultipassScene = NULL;
         mSceneCamera = NULL;
      }

      bool mCreateDefaultScene;
      bool mCreateMultipassScene;
      bool mEnableHDR;
      bool mResize;
      const GraphicsQuality* mGraphicsQuality;
      typedef std::vector<dtCore::RefPtr<SceneGroup> > SceneGroupArray;
      dtCore::RefPtr<MultipassScene> mMultipassScene;
      dtCore::RefPtr<dtCore::Camera> mSceneCamera;

      SMDefaultUniforms* mUniforms;

      SceneManagerImpl::SceneGroupArray mChildren;
      std::stack<dtCore::ObserverPtr<SceneBase> > mSceneStack;

   };


   const dtUtil::RefString SceneManager::UNIFORM_MAIN_CAMERA_POS("d3d_CameraPos");
   const dtUtil::RefString SceneManager::UNIFORM_MAIN_CAMERA_HPR("d3d_CameraHPR");
   const dtUtil::RefString SceneManager::UNIFORM_MAIN_CAMERA_INVERSE_VIEW("d3d_InverseViewMatrix");
   const dtUtil::RefString SceneManager::UNIFORM_MAIN_CAMERA_INVERSE_MODELVIEWPROJECTION("d3d_InverseModelViewProjectionMatrix");

   const dtUtil::RefString SceneManager::UNIFORM_SCREEN_WIDTH("d3d_ScreenWidth");
   const dtUtil::RefString SceneManager::UNIFORM_SCREEN_HEIGHT("d3d_ScreenHeight");

   const dtUtil::RefString SceneManager::UNIFORM_NEAR_PLANE("d3d_NearPlane");
   const dtUtil::RefString SceneManager::UNIFORM_FAR_PLANE("d3d_FarPlane");

   const dtUtil::RefString SceneManager::UNIFORM_FRAME_TIME("d3d_FrameTime");
   const dtUtil::RefString SceneManager::UNIFORM_ELAPSED_TIME("d3d_ElapsedTime");

   const dtUtil::RefString SceneManager::UNIFORM_GAMMA("d3d_Gamma");
   const dtUtil::RefString SceneManager::UNIFORM_BRIGHTNESS("d3d_Brightness");

   const dtUtil::RefString SceneManager::UNIFORM_SCENE_LUMINANCE("d3d_SceneLuminance");
   const dtUtil::RefString SceneManager::UNIFORM_SCENE_AMBIENCE("d3d_SceneAmbience");

   SceneManager::SceneManager( dtGame::GameActorProxy& parent )
   : BaseClass(parent)
   , mImpl(new SceneManagerImpl())
   {
      SetName("SceneManager");

   }


   SceneManager::~SceneManager()
   {
      RemoveAllActors();
      delete mImpl;
   }

   void SceneManager::CreateScene()
   {
      //clear old scene first
      RemoveAllActors();

      //create a scene for each enumeration
      mImpl->mChildren.resize(SceneEnum::NUM_SCENES.GetSceneNumber());
      int numScenes = SceneEnum::NUM_SCENES.GetSceneNumber();
      for(int i = 0; i < numScenes; ++i)
      {
         dtCore::RefPtr<SceneGroup> newNode = new SceneGroup();
         SceneEnum& se = SceneEnum::FindSceneByNumber(i);
         newNode->SetSceneEnum(se);

         SetNodeMask(se, *newNode->GetOSGNode());

         newNode->CreateScene(*this, *mImpl->mGraphicsQuality);
         newNode->GetOSGNode()->getOrCreateStateSet()->setBinNumber(i);

         //this relies on the fact that children added are always pushed back
         dtCore::Transformable::AddChild(newNode.get());
         mImpl->mChildren[i] = newNode;
      }

      //create a default scene to accept DeltaDrawables
      if(mImpl->mCreateDefaultScene)
      {
         CreateDefaultScene();
      }

      //create a default scene which enables render to texture 
      //and multipass effects
      if(mImpl->mCreateMultipassScene)
      {
         CreateDefaultMultipassScene();
      }
      
   }

   void SceneManager::CreateDefaultScene()
   {
      //create a default scene to accept DeltaDrawables
      dtCore::RefPtr<SimpleScene> ss = new SimpleScene();
      ss->CreateScene(*this, *mImpl->mGraphicsQuality);
      ss->SetSceneEnum(SceneEnum::DEFAULT_SCENE);

      AddScene(*ss);
   }

   void SceneManager::CreateDefaultMultipassScene()
   {    
      dtCore::Camera* sceneCam = GetSceneCamera();
      if(sceneCam != NULL)
      {
         dtCore::RefPtr<MultipassScene> mps = new MultipassScene();

         AddScene(*mps);
      }
      else
      {
         LOG_ERROR("Cannot create multipass scene without main scene camera.");
      }
   }

   void SceneManager::AddScene(SceneBase& sb)
   {
      const int sceneRenderOrder = sb.GetSceneEnum().GetSceneNumber();
      
      SceneGroup* childAsGroup = mImpl->mChildren[sceneRenderOrder]->GetAsSceneGroup();

      //if the current child is a group, we add it to that
      if(childAsGroup != NULL)
      {
         //Make the create scene call, this initializes any multi pass rendering 
         //effects and cameras
         sb.CreateScene(*this, *mImpl->mGraphicsQuality);
         
         bool addedToScene = childAsGroup->AddChild(&sb);
         if(!addedToScene)
         {
            LOG_ERROR("Error trying to add Scene to SceneManager");
         }
      }
      else
      {
         LOG_ERROR("Scene not initialized properly, all SceneManager children must be SceneGroups");
      }
      
   }

   void SceneManager::AddActor( dtCore::DeltaDrawable &dd )
   {
      SceneBase* scene = dynamic_cast<SceneBase*>(&dd);

      if(scene != NULL)
      {
         AddScene(*scene);
      }
      else
      {
         /**
         *  GetCurrentScene() uses the stack to determine where to add children
         */
         SceneBase* curScene = GetCurrentScene();
         if(curScene != NULL)
         {
            curScene->AddChild(&dd);
         }
         else //add to the default scene
         {
            bool added = mImpl->mChildren[SceneEnum::DEFAULT_SCENE.GetSceneNumber()]->AddChild(&dd);
            if(!added)
            {
               LOG_ERROR("Unable to add child to scene.");
            }
         }
      }
   }


   void SceneManager::RemoveActor( dtCore::DeltaDrawable &dd )
   {
      dtCore::DeltaDrawable* parent = dd.GetParent();
      if(parent != NULL)
      {
         if(parent != this)
         {
            parent->RemoveChild(&dd);
         }
         else
         {
            SceneBase* sb = dynamic_cast<SceneBase*>(&dd);
            //we should only have scene children
            if(sb != NULL)
            {
               //the remove must first traverse all scene children for cleanup
               sb->RemoveAllChildren();

               //clear the scene enum slot
               mImpl->mChildren[sb->GetSceneEnum().GetSceneNumber()] = NULL;

               RemoveChild(&dd);
            }
            else
            {
               LOG_ERROR("SceneManager should only have SceneBase's as children.")
            }

         }
      }
   }


   void SceneManager::RemoveAllActors()
   {
      while(GetNumChildren() > 0)
      {
         DeltaDrawable* dd = GetChild(0);
         RemoveChild(dd);
      }
   }


   bool SceneManager::ContainsActor( dtCore::DeltaDrawable &dd ) const
   {
      SceneManagerImpl::SceneGroupArray::iterator iter = mImpl->mChildren.begin();
      SceneManagerImpl::SceneGroupArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         SceneGroup* sg = (*iter).get();
         //if this is the child, or if the child scene owns it
         if(sg == &dd || sg->ContainsActor(dd) )
         {
            return true;
         }
      }

      return false;
   }


   void SceneManager::GetAllActors( std::vector<dtCore::DeltaDrawable*> &vec )
   {
      SceneManagerImpl::SceneGroupArray::iterator iter = mImpl->mChildren.begin();
      SceneManagerImpl::SceneGroupArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         vec.push_back( (*iter).get() );
      }
   }


   unsigned int SceneManager::GetNumEnvironmentChildren() const
   {
      return mImpl->mChildren.size();
   }

   void SceneManager::PushScene( SceneBase& s)
   {
      //currently this is commented out because it does not allow a scene
      //to add actors to itself on CreateScene
      if(true)//ContainsActor(s))
      {
         mImpl->mSceneStack.push(&s);
      }
      else
      {
         LOG_ERROR("Cannot push non child scene onto scene stack.");
      }
   }

   void SceneManager::PopScene()
   {
      mImpl->mSceneStack.pop();
   }

   SceneBase* SceneManager::GetCurrentScene()
   {
      /***
      *  It is possible to remove a scene that is on the stack
      *     in this case the observer ptr should be NULL and 
      *     this (recursive) function will pop the stack until its empty
      *     looking for a valid scene.
      */
      if(!mImpl->mSceneStack.empty())
      {
         dtCore::ObserverPtr<SceneBase>& s = mImpl->mSceneStack.top();
         
         //NOTE: this does not seem to always work
         //the pointer is not always being set to /nqnull
         if(!s.valid())
         {
            PopScene();
            return GetCurrentScene();
         }
         else
         {
            return s.get();
         }
      }
   
      return NULL;
   }

   const GraphicsQuality& SceneManager::GetGraphicsQuality() const
   {
      return *(mImpl->mGraphicsQuality);
   }

   void SceneManager::SetGraphicsQuality( GraphicsQuality& gq)
   {
      mImpl->mGraphicsQuality = &gq;
   }

   SceneGroup* SceneManager::GetSceneGroup(SceneEnum& se)
   {
      return mImpl->mChildren[se.GetSceneNumber()];
   }

   const SceneGroup* SceneManager::GetSceneGroup( SceneEnum& se) const
   {
      return mImpl->mChildren[se.GetSceneNumber()];
   }

   dtGame::GameManager* SceneManager::GetGameManager()
   {
      return GetOwner()->GetGameManager();
   }

   const dtGame::GameManager* SceneManager::GetGameManager() const
   {
      return GetOwner()->GetGameManager();
   }

   SceneBase* SceneManager::FindSceneByType( SceneType& st)
   {

      SceneManagerImpl::SceneGroupArray::iterator iter = mImpl->mChildren.begin();
      SceneManagerImpl::SceneGroupArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         SceneGroup* sg = (*iter).get();
         if(sg->GetType() == st)
         {
            return sg;
         }
         else
         {
            SceneBase* sb = sg->FindSceneByType(st);
            if(sb != NULL)
            {
               return sb;
            }
         }
      }

      return NULL;
   }

   const SceneBase* SceneManager::FindSceneByType( SceneType& st) const
   {

      SceneManagerImpl::SceneGroupArray::const_iterator iter = mImpl->mChildren.begin();
      SceneManagerImpl::SceneGroupArray::const_iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         const SceneGroup* sg = (*iter).get();
         if(sg->GetType() == st)
         {
            return sg;
         }
         else
         {
            const SceneBase* sb = sg->FindSceneByType(st);
            if(sb != NULL)
            {
               return sb;
            }
         }
      }

      return NULL;
   }

   SceneBase* SceneManager::FindSceneForDrawable( DeltaDrawable& dd)
   {
      //get the parent and recurse up till we find a scene
      DeltaDrawable* parentNode = dd.GetParent();
      if(parentNode != NULL)
      {
         SceneBase* scene = dynamic_cast<SceneBase*>(parentNode);
         if(scene != NULL)
         {
            return scene;
         }
         else
         {
            return FindSceneForDrawable(*parentNode);
         }
      }
      return NULL;
   }

   const SceneBase* SceneManager::FindSceneForDrawable( DeltaDrawable& dd) const
   {
      //get the parent and recurse up till we find a scene
      DeltaDrawable* parentNode = dd.GetParent();
      if(parentNode != NULL)
      {
         const SceneBase* scene = dynamic_cast<const SceneBase*>(parentNode);
         if(scene != NULL)
         {
            return scene;
         }
         else
         {
            return FindSceneForDrawable(*parentNode);
         }
      }
      return NULL;
   }

   void SceneManager::GetAllScenesByType(SceneType& st, std::vector<SceneBase*>& toFill)
   {
      SceneManagerImpl::SceneGroupArray::iterator iter = mImpl->mChildren.begin();
      SceneManagerImpl::SceneGroupArray::iterator iterEnd = mImpl->mChildren.end();

      for (;iter != iterEnd; ++iter)
      {
         SceneGroup* sg = (*iter).get();
         if(sg->GetType() == st)
         {
            toFill.push_back(sg);
         }
         
         sg->GetAllScenesByType(st, toFill);
      }
   }

   dtCore::Camera* SceneManager::GetSceneCamera()
   {
      if(!mImpl->mSceneCamera.valid())
      {
         //setup default main scene camera      
         //TODO - WHY Doesnt this work??
         if (GetOwner()->IsInGM())
         {
            mImpl->mSceneCamera = GetGameManager()->GetApplication().GetCamera();
         }
         else if (GetOwner()->IsInSTAGE())
         {
            mImpl->mSceneCamera =  dtABC::Application::GetInstance("Application")->GetCamera();
         }
      }

      return mImpl->mSceneCamera.get();
   }

   const dtCore::Camera* SceneManager::GetSceneCamera() const
   {
      return mImpl->mSceneCamera.get();
   }

   void SceneManager::SetEnableMultipass( bool b )
   {
      mImpl->mCreateMultipassScene = b;
   }

   bool SceneManager::GetEnableMultipass() const
   {
      return mImpl->mCreateMultipassScene;
   }

   void SceneManager::SetSceneCamera( dtCore::Camera* cam)
   {
      mImpl->mSceneCamera = cam;
   }


   void SceneManager::SetNodeMask(const SceneEnum& se, osg::Node& n)
   {
      if(se == SceneEnum::PRE_RENDER)
      {
         n.setNodeMask(dtUtil::NodeMask::PRE_PROCESS);
      }
      else if(se == SceneEnum::POST_RENDER)
      {
         n.setNodeMask(dtUtil::NodeMask::POST_PROCESS);
      }
      else if(se == SceneEnum::BACKGROUND)
      {
         n.setNodeMask(dtUtil::NodeMask::BACKGROUND);
      }
      else if(se == SceneEnum::FOREGROUND)
      {
         n.setNodeMask(dtUtil::NodeMask::FOREGROUND);
      }
      else if(se == SceneEnum::NON_TRANSPARENT_OBJECTS)
      {
         n.setNodeMask(dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY);
      }
      else if(se == SceneEnum::DEFAULT_SCENE)
      {
         n.setNodeMask(dtUtil::NodeMask::DEFAULT_GEOMETRY);
      }
      else if(se == SceneEnum::TRANSPARENT_OBJECTS)
      {
         n.setNodeMask(dtUtil::NodeMask::TRANSPARENT_GEOMETRY);
      }
   }

   void SceneManager::SetEnableHDR( bool b)
   {
      mImpl->mEnableHDR = b;
      
      osg::StateSet* ss = GetOSGNode()->getOrCreateStateSet();

      osg::ClampColor* clamp = new osg::ClampColor();

      if(mImpl->mEnableHDR)
      {

         mImpl->mUniforms->mSceneLuminance->SetValue(2.0);
         mImpl->mUniforms->mSceneLuminance->Update();
         mImpl->mUniforms->mSceneAmbience->SetValue(1.25);
         mImpl->mUniforms->mSceneAmbience->Update();
         
         // disable color clamping, because we want to work on real hdr values
         clamp->setClampVertexColor(GL_FALSE);
         clamp->setClampFragmentColor(GL_FALSE);
         clamp->setClampReadColor(GL_FALSE);
      }
      else
      {
         mImpl->mUniforms->mSceneLuminance->SetValue(1.0);
         mImpl->mUniforms->mSceneLuminance->Update();
         mImpl->mUniforms->mSceneAmbience->SetValue(1.0);
         mImpl->mUniforms->mSceneAmbience->Update();

         osg::ClampColor* clamp = new osg::ClampColor();
         clamp->setClampVertexColor(GL_TRUE);
         clamp->setClampFragmentColor(GL_TRUE);
         clamp->setClampReadColor(GL_TRUE);
      }

      // make it protected and override, so that it is done for the whole rendering pipeline
      ss->setAttribute(clamp, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);     

   }

   bool SceneManager::GetEnableHDR() const
   {
      return mImpl->mEnableHDR;
   }

   void SceneManager::PostComponentInit()
   {
      //create all default scene uniforms
      InitUniforms();

      //set the hdr preference
      SetEnableHDR(mImpl->mEnableHDR);
   }

   float SceneManager::GetGamma() const
   {
      return mImpl->mUniforms->mGamma->GetValue();
   }

   void SceneManager::SetGamma( float g)
   {
      mImpl->mUniforms->mGamma->SetValue(g);
   }

   float SceneManager::GetBrightness() const
   {
      return mImpl->mUniforms->mBrightness->GetValue();
   }

   void SceneManager::SetBrightness( float b)
   {
      mImpl->mUniforms->mBrightness->SetValue(b);
   }

   float SceneManager::GetLuminance() const
   {
      return mImpl->mUniforms->mSceneLuminance->GetValue();
   }

   void SceneManager::SetLuminance( float l)
   {
      mImpl->mUniforms->mSceneLuminance->SetValue(l);
   }

   float SceneManager::GetAmbience() const
   {
      return mImpl->mUniforms->mSceneAmbience->GetValue();
   }

   void SceneManager::SetAmbience( float a)
   {
      mImpl->mUniforms->mSceneAmbience->SetValue(a);
   }

   void SceneManager::InitUniforms()
   {
   
      //without mat 4 params we have to bind a few ourselves
      osg::StateSet* ss = GetOSGNode()->getOrCreateStateSet();

      //add a camera update callback to set camera based uniforms
      dtCore::Camera::AddCameraSyncCallback(*this,
         dtCore::Camera::CameraSyncCallback(this, &SceneManager::UpdateUniforms));


      //create default shader parameters
      mImpl->mUniforms->mScreenWidth = new dtCore::ShaderParamInt(UNIFORM_SCREEN_WIDTH);
      mImpl->mUniforms->mScreenHeight = new dtCore::ShaderParamInt(UNIFORM_SCREEN_HEIGHT);

      mImpl->mUniforms->mNearPlane = new dtCore::ShaderParamFloat(UNIFORM_NEAR_PLANE);
      mImpl->mUniforms->mFarPlane = new dtCore::ShaderParamFloat(UNIFORM_FAR_PLANE);

      mImpl->mUniforms->mFrameTime = new dtCore::ShaderParamFloat(UNIFORM_FRAME_TIME);
      mImpl->mUniforms->mElapsedTime = new dtCore::ShaderParamFloat(UNIFORM_ELAPSED_TIME);

      mImpl->mUniforms->mGamma = new dtCore::ShaderParamFloat(UNIFORM_GAMMA);
      mImpl->mUniforms->mBrightness = new dtCore::ShaderParamFloat(UNIFORM_BRIGHTNESS);

      mImpl->mUniforms->mGamma->SetValue(1.0f);
      mImpl->mUniforms->mBrightness->SetValue(1.0f);

      mImpl->mUniforms->mSceneLuminance = new dtCore::ShaderParamFloat(UNIFORM_SCENE_LUMINANCE);
      mImpl->mUniforms->mSceneAmbience = new dtCore::ShaderParamFloat(UNIFORM_SCENE_AMBIENCE);

      mImpl->mUniforms->mSceneLuminance->SetValue(1.0f);
      mImpl->mUniforms->mSceneAmbience->SetValue(1.0f);

      mImpl->mUniforms->mMainCameraPos = new dtCore::ShaderParamVec4(UNIFORM_MAIN_CAMERA_POS);
      mImpl->mUniforms->mMainCameraHPR = new dtCore::ShaderParamVec4(UNIFORM_MAIN_CAMERA_HPR);
      
      //no mat4 parameter we have to add these ourselves
      mImpl->mUniforms->mMainCameraInverseViewMatrix = ss->getOrCreateUniform(UNIFORM_MAIN_CAMERA_INVERSE_VIEW, osg::Uniform::FLOAT_MAT4);
      mImpl->mUniforms->mMainCameraInverseModelViewProjectionMatrix = ss->getOrCreateUniform(UNIFORM_MAIN_CAMERA_INVERSE_MODELVIEWPROJECTION, osg::Uniform::FLOAT_MAT4);


      UniformActComp* actComp = GetOwner()->GetComponent<UniformActComp>();
      if(actComp != NULL)
      {
         actComp->AddParameter(*(mImpl->mUniforms->mScreenWidth));
         actComp->AddParameter(*(mImpl->mUniforms->mScreenHeight));
         actComp->AddParameter(*(mImpl->mUniforms->mNearPlane));
         actComp->AddParameter(*(mImpl->mUniforms->mFarPlane));
         actComp->AddParameter(*(mImpl->mUniforms->mFrameTime));
         actComp->AddParameter(*(mImpl->mUniforms->mElapsedTime));
         actComp->AddParameter(*(mImpl->mUniforms->mGamma));
         actComp->AddParameter(*(mImpl->mUniforms->mBrightness));
         actComp->AddParameter(*(mImpl->mUniforms->mSceneLuminance));
         actComp->AddParameter(*(mImpl->mUniforms->mSceneAmbience));
         actComp->AddParameter(*(mImpl->mUniforms->mMainCameraPos));
         actComp->AddParameter(*(mImpl->mUniforms->mMainCameraHPR));
      }
      else
      {
         LOG_ERROR("Error initializing uniforms, scene manager cannot find uniform actor component.");
      }
   }

   void SceneManager::UpdateUniforms(dtCore::Camera& pCamera)
   {
      osg::StateSet* ss = GetOSGNode()->getOrCreateStateSet();
      
      float frameTime = dtCore::System::GetInstance().GetSimulationTime();
      float elapsedTime = dtCore::System::GetInstance().GetSimTimeSinceStartup();

      //just something  better then 0
      float screenWidth = 1024;
      float screenHeight = 768;

      if(pCamera.GetOSGCamera()->getViewport() != NULL)
      {
         screenWidth = pCamera.GetOSGCamera()->getViewport()->width();
         screenHeight = pCamera.GetOSGCamera()->getViewport()->height();
      }

      double vfov, aspect, pnear, pfar;

      pCamera.GetPerspectiveParams(vfov, aspect, pnear, pfar);

      if(screenWidth != mImpl->mUniforms->mScreenWidth->GetValue())
      {
         mImpl->mUniforms->mScreenWidth->SetValue(screenWidth);
         mImpl->mUniforms->mScreenWidth->Update();
         mImpl->mResize = true;
      }

      if(screenHeight != mImpl->mUniforms->mScreenHeight->GetValue())
      {
         mImpl->mUniforms->mScreenHeight->SetValue(screenHeight);
         mImpl->mUniforms->mScreenHeight->Update();
         mImpl->mResize = true;
      }

      if(pnear != mImpl->mUniforms->mNearPlane->GetValue())
      {
         mImpl->mUniforms->mNearPlane->SetValue(pnear);
         mImpl->mUniforms->mNearPlane->Update();
      }

      if(pfar != mImpl->mUniforms->mFarPlane->GetValue())
      {
         mImpl->mUniforms->mFarPlane->SetValue(pfar);
         mImpl->mUniforms->mFarPlane->Update();
      }

      mImpl->mUniforms->mFrameTime->SetValue(frameTime);
      mImpl->mUniforms->mFrameTime->Update();

      mImpl->mUniforms->mElapsedTime->SetValue(elapsedTime);
      mImpl->mUniforms->mElapsedTime->Update();


      //camera based uniforms
      {
         osg::Matrix matWorld, matView, matViewInverse, matProj, matProjInverse, matViewProj, matViewProjInverse;
      
         matView.set(pCamera.GetOSGCamera()->getViewMatrix());

         matViewInverse.invert(matView);

         matProj.set(pCamera.GetOSGCamera()->getProjectionMatrix());
         matProjInverse.invert(matProj);

         matViewProj = matView * matProj;
         matViewProjInverse.invert(matViewProj);

         mImpl->mUniforms->mMainCameraInverseModelViewProjectionMatrix->set(matViewProjInverse);
         mImpl->mUniforms->mMainCameraInverseViewMatrix->set(matViewInverse);

         dtCore::Transform trans;
         pCamera.GetTransform(trans);

         osg::Vec3 hpr;
         trans.GetRotation(hpr);

         mImpl->mUniforms->mMainCameraHPR->SetValue(osg::Vec4(hpr[0], hpr[1], hpr[2], 0.0));
         mImpl->mUniforms->mMainCameraHPR->Update();
      }
   }


   /////////////////////////////////////////////////////////////
   // actor
   SceneManagerActor::SceneManagerActor()
   {
   }

   SceneManagerActor::~SceneManagerActor()
   {
   }

   void SceneManagerActor::BuildPropertyMap()
   {
      dtGame::GameActorProxy::BuildPropertyMap();

      //SceneManager *env;
      //GetDrawable(env);

      //todo- setup a default state set
      /*osg::Camera* cam = GetGameManager()->GetApplication().GetCamera()->GetOSGCamera();

      cam->setClearColor(osg::Vec4(0, 0, 0, 0));
      cam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
      cam->setCullingMode(osg::CullSettings::ENABLE_ALL_CULLING);
*/

   }

   void SceneManagerActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();
      AddComponent(*new UniformActComp());
      GetDrawable<SceneManager>()->PostComponentInit();
   }

   void SceneManagerActor::CreateDrawable()
   {
      dtCore::RefPtr<SceneManager> scene = new SceneManager(*this);
      SetDrawable(*scene);

      scene->CreateScene();

   }

}//namespace dtRender
