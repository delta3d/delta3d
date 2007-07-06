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
#include <dtAnim/skeletaldrawable.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>
#include <dtCore/scene.h>
#include <dtGame/basemessages.h>
#include <dtCore/system.h>
#include <dtGame/invokable.h>
#include <dtDAL/functor.h>

#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Material>
#include <osg/PolygonMode>
#include <dtUtil/bits.h>

#include <cstddef>  // for NULL

namespace dtActors
{

   const std::string AnimationGameActor::PropertyNames::ANIMATION_GROUP("ANIMATION_GROUP");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_GROUP_LABEL("Animation Group");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_GROUP("ANIMATION_BLEND_GROUP_");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_WEIGHT("ANIMATION_BLEND_WEIGHT_");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_ID("ANIMATION_BLEND_ID_");
   const std::string AnimationGameActor::PropertyNames::ANIMATION_BLEND_DELAY("ANIMATION_BLEND_DELAY_");
   const std::string AnimationGameActor::PropertyNames::RENDER_MODE("RENDER_MODE");
   const std::string AnimationGameActor::PropertyNames::RENDER_MODE_LABEL("Render mode");

   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN ACTOR //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::AnimationGameActor(dtGame::GameActorProxy &proxy)
      : dtGame::GameActor(proxy)
      , mModelGeode(new osg::Geode)
      , mSkeletalGeode(new osg::Geode)
      , mModelLoader(new dtAnim::Cal3DLoader)
      , mAnimator( NULL )
      , mRenderModeBits( RENDER_MODE_SKIN )
   {
      mSkeletalGeode->setName("AnimationGameActor_mSkeletalGeode");
      mSkeletalGeode->setNodeMask( 0x0 );  // will not be drawn
   }

   ///////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::~AnimationGameActor()
   {
   }

   void AnimationGameActor::SetModel(const std::string &modelFile)
   {
      dtCore::RefPtr<dtAnim::Cal3DModelWrapper> newModel = mModelLoader->Load(modelFile);    

      // If we successfully loaded the model, give it to the animator
      if (newModel.valid())
      {         
         mAnimator = new dtAnim::Cal3DAnimator(newModel.get());   

         // support to draw the skeleton
         mSkeletalGeode->addDrawable(new dtAnim::SkeletalDrawable(newModel.get()));

         // support to draw the mesh
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

   void AnimationGameActor::SetRenderMode(int bits)
   {
      mRenderModeBits = bits;

      // parse the bits
      if( RENDER_MODE_NONE == mRenderModeBits )
      {
         mModelGeode->setNodeMask( 0x0 );  // will not be drawn
         mSkeletalGeode->setNodeMask( 0x0 );  // will not be drawn
      }
      else
      {
         mModelGeode->setNodeMask( 0x0 );  // will not be drawn
         mSkeletalGeode->setNodeMask( 0x0 );  // will not be drawn

         if( dtUtil::Bits::Has(mRenderModeBits,RENDER_MODE_SKIN) )
         {
            mModelGeode->setNodeMask( 0xffffffff );  // will be drawn
         }
         if( dtUtil::Bits::Has(mRenderModeBits,RENDER_MODE_BONES) )
         {
            mSkeletalGeode->setNodeMask( 0xffffffff );  // will be drawn
         }
      }
   }

   int AnimationGameActor::GetRenderMode() const
   {
      return mRenderModeBits;
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActor::AddedToScene(dtCore::Scene* scene)
   {
      dtGame::GameActor::AddedToScene(scene);     
      GetMatrixNode()->addChild(mModelGeode.get());
      GetMatrixNode()->addChild(mSkeletalGeode.get());
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

      ///\todo make a UChar actor property and use it here.
      AddProperty(new dtDAL::IntActorProperty(AnimationGameActor::PropertyNames::RENDER_MODE,
                                              AnimationGameActor::PropertyNames::RENDER_MODE_LABEL,
                                              dtDAL::MakeFunctor(myActor, &AnimationGameActor::SetRenderMode),
                                              dtDAL::MakeFunctorRet(myActor, &AnimationGameActor::GetRenderMode),
                                              "Bits to control what is rendered.",
                                              "No idea what is meant by _group name_"));
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildInvokables()
   {
      dtCore::RefPtr<dtGame::Invokable> invokable_tick_local = new dtGame::Invokable(dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE, dtDAL::MakeFunctor(GetGameActor(),&dtGame::GameActor::TickLocal) );
      AddInvokable( *invokable_tick_local );
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
      ///\todo add all animations currently being blended
      return group;
   }

   dtAnim::Cal3DAnimator* AnimationGameActor::GetAnimator()
   {
      return mAnimator.get();
   }

   const dtAnim::Cal3DAnimator* AnimationGameActor::GetAnimator() const
   {
      return mAnimator.get();
   }

   void AnimationGameActor::TickLocal(const dtGame::Message& msg)
   {
      const dtGame::TickMessage& tickmsg = static_cast<const dtGame::TickMessage&>( msg );
      float dt = tickmsg.GetDeltaSimTime();
      
      if (mAnimator.valid())
         mAnimator->Update(dt);

      //static double doIt = 0;
      //doIt += dt;
      //if (doIt > 1.0)
      //{
      //   doIt = 0.0;

      //   std::vector<CalAnimation*> animVec = mAnimator->GetWrapper()->GetCalModel()->getMixer()->getAnimationVector();

      //   std::vector<CalAnimation*>::iterator animItr = animVec.begin();
      //   while (animItr != animVec.end())
      //   {
      //      CalAnimation *anim = *(animItr);
      //      if (anim!=NULL)
      //      {
      //         LOG_DEBUG("Anim: " + anim->getCoreAnimation()->getName() + " Weight: " + dtUtil::ToString(anim->getWeight()));
      //      }
      //      ++animItr;
      //   }
      //}
   
   }

   void AnimationGameActor::OnEnteredWorld()
   {
      dtGame::GameActorProxy& gap = GetGameActorProxy();
      gap.RegisterForMessages( dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE );
   }

}
