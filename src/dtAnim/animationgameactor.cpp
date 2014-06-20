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
 * Bradley G Anderegg
 */

#include <dtAnim/animationgameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/actorproxyicon.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>
#include <dtUtil/functor.h>

#include <dtAnim/animnodebuilder.h>

#include <osg/MatrixTransform>
#include <osg/Geode>


namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::AnimationGameActor(dtGame::GameActorProxy& proxy)
      : dtGame::GameActor(proxy)
      , mHelper(new dtAnim::AnimationHelper())
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::~AnimationGameActor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::BoundingBox AnimationGameActor::GetBoundingBox()
   {
      Cal3DModelWrapper* wrapper = GetComponent<AnimationHelper>()->GetModelWrapper();
      if (wrapper != NULL)
      {
         return wrapper->GetBoundingBox();
      }
      return osg::BoundingBox();
   }

   /////////////////////////////////////////////////////////////////////////////
   AnimationGameActorProxy::AnimationGameActorProxy()
   {
      SetClassName("dtActors::AnimationGameActor");
   }

   /////////////////////////////////////////////////////////////////////////////
   AnimationGameActorProxy::~AnimationGameActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildPropertyMap()
   {
      dtGame::GameActorProxy::BuildPropertyMap();
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildActorComponents()
   {
      dtGame::GameActorProxy::BuildActorComponents();

      dtCore::RefPtr<AnimationHelper> animAC = new dtAnim::AnimationHelper();
      AddComponent(*animAC);
   }


   /////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& AnimationGameActorProxy::GetRenderMode()
   {
      AnimationHelper* animHelper = GetComponent<AnimationHelper>();
      dtCore::ResourceDescriptor resource;
      if (animHelper != NULL)
      {
         resource = animHelper->GetSkeletalMesh();
      }
      if ( ! resource.IsEmpty())
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
      }
      else
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* AnimationGameActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH);
      }

      return mBillBoardIcon.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::CreateDrawable()
   {
      SetDrawable(*new AnimationGameActor(*this));
   }

} // namespace dtAnim
