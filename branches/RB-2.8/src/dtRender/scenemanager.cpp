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

#include <dtCore/transformable.h>
#include <osg/MatrixTransform>
#include <osg/ClampColor>

#include <stack>
#include <dtUtil/nodemask.h>
#include <dtABC/application.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>

namespace dtRender
{
   class SceneManagerImpl
   {
   public:

      SceneManagerImpl()
         : mCreateDefaultScene(true)
         , mCreateMultipassScene(true)
         , mEnableHDR(false)
         , mGraphicsQuality(&GraphicsQuality::DEFAULT)
      {
      }
      ~SceneManagerImpl()
      {
         //effectively clears without popping all the elements
         mSceneStack = std::stack<dtCore::ObserverPtr<SceneBase> >();
         
         mChildren.clear();

         mMultipassScene = NULL;
         mSceneCamera = NULL;
      }

      bool mCreateDefaultScene;
      bool mCreateMultipassScene;
      bool mEnableHDR;
      const GraphicsQuality* mGraphicsQuality;
      typedef std::vector<dtCore::RefPtr<SceneGroup> > SceneManagerImpl::SceneGroupArray;
      dtCore::RefPtr<MultipassScene> mMultipassScene;
      dtCore::RefPtr<dtCore::Camera> mSceneCamera;
      SceneManagerImpl::SceneGroupArray mChildren;
      std::stack<dtCore::ObserverPtr<SceneBase> > mSceneStack;
   };


   const std::string SceneManager::UNIFORM_SCENE_LUMINANCE("d3d_SceneLuminance");
   const std::string SceneManager::UNIFORM_SCENE_AMBIENCE("d3d_SceneAmbience");

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
         Transformable::AddChild(newNode.get());

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
      
      //set the hdr preference
      SetEnableHDR(mImpl->mEnableHDR);
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

               Transformable::RemoveChild(&dd);
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
      return GetGameActorProxy().GetGameManager();
   }

   const dtGame::GameManager* SceneManager::GetGameManager() const
   {
      return GetGameActorProxy().GetGameManager();
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

   SceneBase* SceneManager::FindSceneForActor( DeltaDrawable& dd)
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
            return FindSceneForActor(*parentNode);
         }
      }
      return NULL;
   }

   const SceneBase* SceneManager::FindSceneForActor( DeltaDrawable& dd) const
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
            return FindSceneForActor(*parentNode);
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
         if (GetGameActorProxy().IsInGM())
         {
            mImpl->mSceneCamera = GetGameActorProxy().GetGameManager()->GetApplication().GetCamera();
         }
         else if (GetGameActorProxy().IsInSTAGE())
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

      if(mImpl->mEnableHDR)
      {
         osg::Uniform* l = ss->getOrCreateUniform(UNIFORM_SCENE_LUMINANCE, osg::Uniform::FLOAT);
         l->set(2.5f);

         osg::Uniform* a = ss->getOrCreateUniform(UNIFORM_SCENE_AMBIENCE, osg::Uniform::FLOAT);
         a->set(1.5f);

         // disable color clamping, because we want to work on real hdr values
         osg::ClampColor* clamp = new osg::ClampColor();
         clamp->setClampVertexColor(GL_FALSE);
         clamp->setClampFragmentColor(GL_FALSE);
         clamp->setClampReadColor(GL_FALSE);

         // make it protected and override, so that it is done for the whole rendering pipeline
         ss->setAttribute(clamp, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);     
      }
      else
      {

         osg::Uniform* l = ss->getOrCreateUniform(UNIFORM_SCENE_LUMINANCE, osg::Uniform::FLOAT);
         l->set(1.0f);

         osg::Uniform* a = ss->getOrCreateUniform(UNIFORM_SCENE_AMBIENCE, osg::Uniform::FLOAT);
         a->set(1.0f);

         osg::ClampColor* clamp = new osg::ClampColor();
         clamp->setClampVertexColor(GL_TRUE);
         clamp->setClampFragmentColor(GL_TRUE);
         clamp->setClampReadColor(GL_TRUE);

         ss->setAttribute(clamp, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);     
      }
      
   }

   bool SceneManager::GetEnableHDR() const
   {
      return mImpl->mEnableHDR;
   }



   /////////////////////////////////////////////////////////////
   //proxy
   SceneManagerProxy::SceneManagerProxy()
   {
   }

   SceneManagerProxy::~SceneManagerProxy()
   {
   }

   void SceneManagerProxy::BuildPropertyMap()
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

   void SceneManagerProxy::CreateDrawable()
   {
      dtCore::RefPtr<SceneManager> scene = new SceneManager(*this);
      SetDrawable(*scene);

      scene->CreateScene();

   }

}//namespace dtRender