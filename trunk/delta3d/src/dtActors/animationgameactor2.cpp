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
#include <dtActors/animationgameactor2.h>
#include <dtDAL/groupactorproperty.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>
#include <dtDAL/functor.h>

#include <dtAnim/animnodebuilder.h>
#include <dtAnim/animationhelper.h>

#include <osg/MatrixTransform>
#include <osg/Geode>

namespace dtActors
{
   AnimationGameActor2::AnimationGameActor2(dtGame::GameActorProxy &proxy)
      : dtGame::GameActor(proxy)
      , mHelper(new dtAnim::AnimationHelper())
   {

   }

   AnimationGameActor2::~AnimationGameActor2()
   {
   }

   dtAnim::AnimationHelper* AnimationGameActor2::GetHelper()
   {
      return mHelper.get();
   }

   const dtAnim::AnimationHelper* AnimationGameActor2::GetHelper() const
   {
      return mHelper.get();
   }


   void AnimationGameActor2::SetModel(const std::string &modelFile)
   {
      mHelper->LoadModel(modelFile);
      
      dtCore::RefPtr<osg::Geode> geode = mHelper->GetGeode();
      GetMatrixNode()->addChild(geode.get());
   }

   AnimationGameActorProxy2::AnimationGameActorProxy2()
   {
      SetClassName("dtActors::AnimationGameActor2");
   }

   AnimationGameActorProxy2::~AnimationGameActorProxy2()
   {
   }

   void AnimationGameActorProxy2::BuildPropertyMap()
   {
      dtGame::GameActorProxy::BuildPropertyMap();

      typedef std::vector<dtCore::RefPtr<dtDAL::ActorProperty> > APVector;
      APVector pFillVector;

      AnimationGameActor2 &actor = static_cast<AnimationGameActor2&>(GetGameActor());

      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::SKELETAL_MESH,
         "Skeletal Mesh", "Skeletal Mesh", dtDAL::MakeFunctor(actor, &AnimationGameActor2::SetModel),
         "The model resource that defines the skeletal mesh", "AnimationBase"));

   }

   const dtDAL::ActorProxy::RenderMode& AnimationGameActorProxy2::GetRenderMode()
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

   dtDAL::ActorProxyIcon* AnimationGameActorProxy2::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::STATICMESH);   
      }

      return mBillBoardIcon.get();
   }

   void AnimationGameActorProxy2::CreateActor()
   {
      SetActor(*new AnimationGameActor2(*this));   
   }

}
