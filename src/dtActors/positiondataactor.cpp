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

#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////
   PositionDataActorProxy::PositionDataActorProxy() :
   dtCore::TransformableActorProxy(),
   mChangeTranslation(true),
   mChangeRotation(true)
   {    
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::CreateDrawable()
   { 
      SetDrawable(*new dtCore::Transformable()); 
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::BuildPropertyMap()
   {
      TransformableActorProxy::BuildPropertyMap();

      AddProperty( new dtCore::BooleanActorProperty("changeTranslation", "Change Position / Translation",
         dtCore::BooleanActorProperty::SetFuncType(this, &PositionDataActorProxy::SetChangeTranslation),
         dtCore::BooleanActorProperty::GetFuncType(this, &PositionDataActorProxy::GetChangeTranslation),
         "", "ModelView"));

      AddProperty( new dtCore::BooleanActorProperty("changeRotation", "Change Orientation / Rotation",
         dtCore::BooleanActorProperty::SetFuncType(this, &PositionDataActorProxy::SetChangeRotation),
         dtCore::BooleanActorProperty::GetFuncType(this, &PositionDataActorProxy::GetChangeRotation),
         "", "ModelView"));      
   }

   //////////////////////////////////////////////////////////////////////////
   void PositionDataActorProxy::ApplyDataTo(dtCore::BaseActorObject* actorProxy) const
   {
      ApplyDataTo(actorProxy->GetDrawable());
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

      GetDrawable<dtCore::Transformable>()->GetTransform(transformSource);
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
   dtCore::ActorProxyIcon* PositionDataActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_POSITION);
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& PositionDataActorProxy::GetRenderMode()
   {
      return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
   }
}
