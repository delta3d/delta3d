/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2010, MOVES Institute & BMH Associates, Inc.
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
*/
#include <dtActors/positiondataactor.h>

#include <dtCore/transform.h>

#include <dtDAL/actorproxyicon.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/datatype.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////
   PositionDataActorProxy::PositionDataActorProxy() :
   dtDAL::TransformableActorProxy(),
   mChangeTranslation(true),
   mChangeRotation(true)
   {    
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::CreateActor()
   { 
      SetActor(*new dtCore::Transformable()); 
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::BuildPropertyMap()
   {
      TransformableActorProxy::BuildPropertyMap();

      AddProperty( new dtDAL::BooleanActorProperty("changeTranslation", "Change Position / Translation",
         dtDAL::BooleanActorProperty::SetFuncType(this, &PositionDataActorProxy::SetChangeTranslation),
         dtDAL::BooleanActorProperty::GetFuncType(this, &PositionDataActorProxy::GetChangeTranslation),
         "", "ModelView"));

      AddProperty( new dtDAL::BooleanActorProperty("changeRotation", "Change Orientation / Rotation",
         dtDAL::BooleanActorProperty::SetFuncType(this, &PositionDataActorProxy::SetChangeRotation),
         dtDAL::BooleanActorProperty::GetFuncType(this, &PositionDataActorProxy::GetChangeRotation),
         "", "ModelView"));      
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::ApplyDataTo(dtDAL::BaseActorObject* actorProxy) const
   {
      ApplyDataTo(actorProxy->GetActor());
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::ApplyDataTo(dtCore::DeltaDrawable* drawable) const
   {
      dtCore::Transformable* transformable = dynamic_cast<dtCore::Transformable*>(drawable);
      if(transformable)
      {
         ApplyDataTo(transformable);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::ApplyDataTo(dtCore::Transformable* transformable) const
   {
      dtCore::Transform transformSource;
      dtCore::Transform transformTarget;

      static_cast<const dtCore::Transformable*>(GetActor())->GetTransform(transformSource);
      transformable->GetTransform(transformTarget);

      if (mChangeTranslation)
      {
         transformTarget.SetTranslation(transformSource.GetTranslation());
      }

      if (mChangeRotation)
      {
         transformTarget.SetRotation(transformSource.GetRotation());
      }

      transformable->SetTransform(transformTarget);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon* PositionDataActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IMAGE_BILLBOARD_POSITION);
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtDAL::BaseActorObject::RenderMode& PositionDataActorProxy::GetRenderMode()
   {
      return dtDAL::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
   }
}
