/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Alion Science and Technology
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
* Michael Guerrero
*/
#include <dtActors/animationgameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtAnim/submesh.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>

//#include <osg/MatrixTransform>
#include <osg/Geode>


namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN ACTOR //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::AnimationGameActor(dtGame::GameActorProxy &proxy)
      : dtGame::GameActor(proxy)
      , mModelGeode(new osg::Geode)
      , mModelLoader(new dtAnim::Cal3DLoader)
   {
      
   }

   ///////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::~AnimationGameActor()
   {
   }
    
   void AnimationGameActor::SetModel(const std::string &modelFile)
   {
      dtAnim::Cal3DModelWrapper *newModel = mModelLoader->Load(modelFile).get();

      // If we successfully loaded the model, give it to the animator
      if (newModel)
      {
         mAnimator = new dtAnim::Cal3DAnimator(newModel);         

         //if(newModel->BeginRenderingQuery()) 
         //{
         //   int meshCount = newModel->GetMeshCount();

         //   for(int meshId = 0; meshId < meshCount; meshId++) 
         //   {
         //      int submeshCount = newModel->GetSubmeshCount(meshId);

         //      for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
         //      {
         //         dtAnim::SubMeshDrawable *submesh = new dtAnim::SubMeshDrawable(newModel, meshId, submeshId);
         //         mModelGeode->addDrawable(submesh);
         //      }
         //   }
         //   newModel->EndRenderingQuery();
         //}

         ///// Force generation of first mesh
         //newModel->Update(0);

         //GetMatrixNode()->addChild(mGeode.get()); 
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActor::AddedToScene(dtCore::Scene* scene)
   {
      dtGame::GameActor::AddedToScene(scene);
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////// END ACTOR ///////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////


   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN PROXY //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActorProxy::AnimationGameActorProxy()
   {
      SetClassName("dtActors::AnimationGameActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActorProxy::~AnimationGameActorProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "AnimationModel";

      dtGame::GameActorProxy::BuildPropertyMap();

      AnimationGameActor &myActor = static_cast<AnimationGameActor&>(GetGameActor());

      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::SKELETAL_MESH,
         "Skeletal Mesh", "Skeletal Mesh", dtDAL::MakeFunctor(myActor, &AnimationGameActor::SetModel),
         "The model resource that defines the skeletal mesh", GROUPNAME));     
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   const dtDAL::ActorProxy::RenderMode& AnimationGameActorProxy::GetRenderMode()
   {
      dtDAL::ResourceDescriptor *resource = GetResource("skeletal mesh");
      if (resource != NULL)
      {
         if (resource->GetResourceIdentifier().empty() || GetActor()->GetOSGNode() == NULL)
         {
            return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
         }
         else
         {
            return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR;
         }
      }
      else
      {
         return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
      }
   }

   dtDAL::ActorProxyIcon* AnimationGameActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::STATICMESH);   
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::CreateActor()
   {
      SetActor(*new AnimationGameActor(*this));   
   }
}