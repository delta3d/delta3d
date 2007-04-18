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
#include <dtDAL/groupactorproperty.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtAnim/submesh.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>
#include <dtCore/scene.h>
#include <dtGame/basemessages.h>
#include <dtCore/system.h>

#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Material>
#include <osg/PolygonMode>

#include <cstddef>  // for NULL

namespace dtActors
{

   const std::string AnimationGameActor::PropertyNames::ANIMATION_GROUP("ANIMATION_GROUP");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_GROUP_LABEL("Animation Group");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_GROUP("ANIMATION_BLEND_GROUP_");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_WEIGHT("ANIMATION_BLEND_WEIGHT_");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_ID("ANIMATION_BLEND_ID_");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_DELAY("ANIMATION_BLEND_DELAY_");

   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN ACTOR //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::AnimationGameActor(dtGame::GameActorProxy &proxy)
      : dtGame::GameActor(proxy)
      , mModelGeode(new osg::Geode)
      , mModelLoader(new dtAnim::Cal3DLoader)
      , mAnimator( NULL )
   {
      AddSender(&dtCore::System::GetInstance());
   }

   ///////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::~AnimationGameActor()
   {
      RemoveSender(&dtCore::System::GetInstance()); 
   }

   void AnimationGameActor::SetModel(const std::string &modelFile)
   {
      dtCore::RefPtr<dtAnim::Cal3DModelWrapper> newModel = mModelLoader->Load(modelFile);    

      // If we successfully loaded the model, give it to the animator
      if (newModel.valid())
      {         
         mAnimator = new dtAnim::Cal3DAnimator(newModel.get());   

         if(newModel->BeginRenderingQuery()) 
         {
            int meshCount = newModel->GetMeshCount();

            for(int meshId = 0; meshId < meshCount; meshId++) 
            {
               int submeshCount = newModel->GetSubmeshCount(meshId);

               for(int submeshId = 0; submeshId < submeshCount; submeshId++) 
               {
                  dtAnim::SubMeshDrawable *submesh = new dtAnim::SubMeshDrawable(newModel.get(), meshId, submeshId);
                  mModelGeode->addDrawable(submesh);
               }
            }
            newModel->EndRenderingQuery();
         }

         /// Force generation of first mesh
         newModel->Update(0);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActor::AddedToScene(dtCore::Scene* scene)
   {
      dtGame::GameActor::AddedToScene(scene);     
      GetMatrixNode()->addChild(mModelGeode.get());
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

      AddProperty(new dtDAL::GroupActorProperty(AnimationGameActor::PropertyNames::ANIMATION_GROUP,
                                                AnimationGameActor::PropertyNames::ANIMATION_GROUP_LABEL,
                                                dtDAL::MakeFunctor(myActor, &AnimationGameActor::ApplyAnimationGroup),
                                                dtDAL::MakeFunctorRet(myActor, &AnimationGameActor::MakeAnimationGroup),
                                                "A pipe for processing animation requests",
                                                "Slot: animation parameter",
                                                "no thanks editor",
                                                false));
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   const dtDAL::ActorProxy::RenderMode& AnimationGameActorProxy::GetRenderMode()
   {
      dtDAL::ResourceDescriptor *resource = GetResource("Skeletal Mesh");
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

   void AnimationGameActor::ApplyAnimationGroup(const dtDAL::NamedGroupParameter& prop)
   {
      // unpack the creative method used to pack all the needed params into the group.
      unsigned int childcount = prop.GetParameterCount();
      for(unsigned int child=0; child<childcount; ++child)
      {
         if( const dtDAL::NamedParameter* childparam = prop.GetParameter( AnimationGameActor::PropertyNames::ANIMATION_BLEND_GROUP + dtUtil::ToString(child)) )
         {
            const dtDAL::NamedGroupParameter* childgroup = static_cast<const dtDAL::NamedGroupParameter*>(childparam);
            const dtDAL::NamedParameter* idchild = childgroup->GetParameter( AnimationGameActor::PropertyNames::ANIMATION_BLEND_ID + dtUtil::ToString(child));
            const dtDAL::NamedParameter* wchild = childgroup->GetParameter( AnimationGameActor::PropertyNames::ANIMATION_BLEND_WEIGHT + dtUtil::ToString(child));
            if( idchild && wchild )
            {
               const dtDAL::NamedUnsignedIntParameter* idparam = static_cast<const dtDAL::NamedUnsignedIntParameter*>( idchild );
               const dtDAL::NamedFloatParameter* wparam = static_cast<const dtDAL::NamedFloatParameter*>( wchild );

               float delay=0.f;
               const dtDAL::NamedParameter* dchild = childgroup->GetParameter( AnimationGameActor::PropertyNames::ANIMATION_BLEND_DELAY + dtUtil::ToString(child));
               if( dchild )
               {
                  const dtDAL::NamedFloatParameter* dparam = static_cast<const dtDAL::NamedFloatParameter*>( dchild );
                  delay = dparam->GetValue();
               }

               dtAnim::Cal3DModelWrapper* wrapper = mAnimator->GetWrapper();
               wrapper->BlendCycle( idparam->GetValue(), wparam->GetValue(), delay );
            }
         }
      }
   }

   dtCore::RefPtr<dtDAL::NamedGroupParameter> AnimationGameActor::MakeAnimationGroup()
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> group = new dtDAL::NamedGroupParameter(AnimationGameActor::PropertyNames::ANIMATION_GROUP);
      return group;
   }

   const dtAnim::Cal3DAnimator* AnimationGameActor::GetAnimator() const
   {
      return mAnimator.get();
   }

   //void AnimationGameActor::TickRemote(const dtGame::Message& msg)
   //{
   //   const dtGame::TickMessage& tickmsg = static_cast<const dtGame::TickMessage&>( msg );
   //   float dt = tickmsg.GetDeltaSimTime();
   //   mAnimator->Update(dt);
   //}

   //void AnimationGameActor::TickLocal(const dtGame::Message& msg)
   //{
   //   const dtGame::TickMessage& tickmsg = static_cast<const dtGame::TickMessage&>( msg );
   //   float dt = tickmsg.GetDeltaSimTime();
   //   mAnimator->Update(dt);
   //}

   void AnimationGameActor::OnMessage(dtCore::Base::MessageData* data)
   {
      if( data->message == "preframe" )
      {
         double dt = *static_cast<double*>(data->userData);      
         if (mAnimator.valid())
            mAnimator->Update(dt);
      }
   }
}
