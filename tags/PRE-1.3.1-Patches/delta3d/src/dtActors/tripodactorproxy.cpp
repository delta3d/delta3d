/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2005, MOVES Institute
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
* @author Chris Osborn
*/

#include <dtActors/tripodactorproxy.h>
#include <dtCore/camera.h>
#include <dtCore/transformable.h>
#include <dtCore/tripod.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <sstream>

using namespace dtActors;
using namespace dtCore;
using namespace dtDAL;

IMPLEMENT_ENUM(dtActors::TripodActorProxy::TetherModeEnum)
TripodActorProxy::TetherModeEnum TripodActorProxy::TetherModeEnum::TETHER_PARENT_REL("TETHER_PARENT_REL");
TripodActorProxy::TetherModeEnum TripodActorProxy::TetherModeEnum::TETHER_WORLD_REL("TETHER_WORLD_REL");

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::CreateActor()
{
   mActor = new Tripod;

   static int actorCount = 0;
   std::ostringstream ss;
   ss << "Tripod" << actorCount++;
   SetName( ss.str() );

   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::BuildPropertyMap()
{
   const std::string& GROUPNAME = "Tripod";

   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   AddProperty(new ActorActorProperty(*this, "Camera", "Camera",
      MakeFunctor(*this,&TripodActorProxy::SetCamera),
      MakeFunctorRet(*this,&TripodActorProxy::GetCamera),
      "dtCore::Camera", "Sets the camera which this tripod will offset."));
   
   AddProperty(new ActorActorProperty(*this, "Parent", "Parent",
      MakeFunctor(*this,&TripodActorProxy::SetAttachToTransformable),
      MakeFunctorRet(*this,&TripodActorProxy::GetAttachedTransformable),
      "dtCore::Transformable", "Sets the Transformable which this Tripod will follow."));
   
   AddProperty(new ActorActorProperty(*this, "Look-At Target", "Look-At Target",
      MakeFunctor(*this,&TripodActorProxy::SetLookAtTarget),
      MakeFunctorRet(*this,&TripodActorProxy::GetLookAtTarget),
      "dtCore::Transformable", "Sets the Transformable which this Tripod will point the Camera at."));
   
   AddProperty(new Vec3ActorProperty("Rotation Offset", "Rotation Offset",
      MakeFunctor(*this, &TripodActorProxy::SetRotationOffset),
      MakeFunctorRet(*this, &TripodActorProxy::GetRotationOffset),
      "Sets the amount of rotation to offset the camera from the attached transformable. Represented with heading, pitch, and roll.",
      GROUPNAME));

   AddProperty(new Vec3ActorProperty("Translation Offset", "Translation Offset",
      MakeFunctor(*this, &TripodActorProxy::SetTranslationOffset),
      MakeFunctorRet(*this, &TripodActorProxy::GetTranslationOffset),
      "Sets the amount of translation to offset the camera from the attached transformable.",
      GROUPNAME));

   AddProperty(new Vec3ActorProperty("Translation Scale Offset", "Translation Scale Offset",
      MakeFunctor(*this, &TripodActorProxy::SetTranslationScaleOffset),
      MakeFunctorRet(*this, &TripodActorProxy::GetTranslationScaleOffset),
      "Sets the scale on the translation offset.",GROUPNAME));
      
   AddProperty(new Vec3ActorProperty("Rotation Scale Offset", "Rotation Scale Offset",
      MakeFunctor(*this, &TripodActorProxy::SetRotationScaleOffset),
      MakeFunctorRet(*this, &TripodActorProxy::GetRotationScaleOffset),
      "Sets the scale on the rotation offset.",GROUPNAME));
   
   AddProperty(new EnumActorProperty<TetherModeEnum>("Tether Mode","Tether Mode",
      MakeFunctor(*this,&TripodActorProxy::SetTetherMode),
      MakeFunctorRet(*this,&TripodActorProxy::GetTetherMode),
      "Sets the tether mode for this tripod actor.", GROUPNAME));     
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetCamera( ActorProxy* cameraProxy )
{
   SetLinkedActor( "Camera", cameraProxy );

   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::BaseException,"Expected a Tripod actor." );
   }

   Camera* camera(0);

   if( cameraProxy != 0 )
   {
      camera = dynamic_cast< Camera* >( cameraProxy->GetActor() );
   }

   tripod->SetCamera(camera);      
}

///////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetCamera()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::BaseException,"Expected a Tripod actor." );
   }

   return tripod->GetCamera();
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetAttachToTransformable( ActorProxy* transformableProxy )
{
   SetLinkedActor( "Parent", transformableProxy );

   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::BaseException,"Expected a Tripod actor." );
   }

   Transformable* parent(0);

   if( transformableProxy != 0 )
   {
      parent = dynamic_cast< Transformable* >( transformableProxy->GetActor() );
   }

   tripod->SetAttachToTransformable(parent);         
}

///////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetAttachedTransformable()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::BaseException,"Expected a Tripod actor." );
   }

   return tripod->GetAttachedTransformable();
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetLookAtTarget( ActorProxy* targetProxy )
{
   SetLinkedActor( "Look-At Target", targetProxy );

   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::BaseException, "Expected a dtCore::Tripod actor." );
   }

   Transformable* target(0);

   if( targetProxy != 0 )
   {
      target = dynamic_cast< Transformable* >( targetProxy->GetActor() );
   }

   tripod->SetLookAtTarget(target);         
}

///////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetLookAtTarget()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );
   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::BaseException,"Expected a Tripod actor." );
   }

   return tripod->GetLookAtTarget();
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetRotationOffset( const osg::Vec3 &rotation )
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 hpr = rotation;

   //Normalize the rotation.
   if( hpr.x() < 0.0f )
   {
      hpr.x() += 360.0f;
   }
   if( hpr.x() > 360.0f )
   {
      hpr.x() -= 360.0f;
   }

   if( hpr.y() < 0.0f )
   {
      hpr.y() += 360.0f;
   }
   if( hpr.y() > 360.0f )
   {
      hpr.y() -= 360.0f;
   }

   if( hpr.z() < 0.0f )
   {
      hpr.z() += 360.0f;
   }
   if( hpr.z() > 360.0f )
   {
      hpr.z() -= 360.0f;
   }

   osg::Vec3 xyz;
   osg::Vec3 oldHPR;
   tripod->GetOffset( xyz, oldHPR );
   tripod->SetOffset( xyz, hpr );
}

///////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetRotationOffset()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetOffset( xyz, hpr );

   return hpr;
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetTranslationOffset( const osg::Vec3& translation )
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 oldXYZ;
   osg::Vec3 hpr;
   tripod->GetOffset( oldXYZ, hpr );
   tripod->SetOffset( translation, hpr );
}

///////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetTranslationOffset()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetOffset( xyz, hpr );

   return xyz;
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetTranslationScaleOffset( const osg::Vec3& scale )
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 oldXYZ;
   osg::Vec3 hpr;
   tripod->GetScale( oldXYZ, hpr );
   tripod->SetScale( scale, hpr );
}

///////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetTranslationScaleOffset()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetScale( xyz, hpr );

   return xyz;
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetRotationScaleOffset( const osg::Vec3& scale )
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 xyz;
   osg::Vec3 oldScale;
   tripod->GetScale( xyz, oldScale );
   tripod->SetScale( xyz, scale );
}

///////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetRotationScaleOffset()
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be dtCore::Tripod." );
   }

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetScale( xyz, hpr );

   return hpr;
}

///////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetTetherMode( TripodActorProxy::TetherModeEnum& mode )
{
   Tripod* tripod = dynamic_cast< Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT(ExceptionEnum::InvalidActorException,
         "Actor should be type dtCore::Tripod.");
   }

   if( mode == TetherModeEnum::TETHER_PARENT_REL )
   {
      tripod->SetTetherMode( Tripod::TETHER_PARENT_REL );
   }
   else if( mode == TetherModeEnum::TETHER_WORLD_REL )
   {
      tripod->SetTetherMode( Tripod::TETHER_WORLD_REL );
   }
}

///////////////////////////////////////////////////////////////////////////////
TripodActorProxy::TetherModeEnum& TripodActorProxy::GetTetherMode() const
{
   const Tripod* tripod = dynamic_cast< const Tripod* >( mActor.get() );

   if( tripod == 0 )
   {
      EXCEPT(ExceptionEnum::InvalidActorException,
         "Actor should be type dtCore::Tripod.");
   }

   if( tripod->GetTetherMode() == Tripod::TETHER_PARENT_REL )
   {
      return TetherModeEnum::TETHER_PARENT_REL;
   }
   else
   {
      return TetherModeEnum::TETHER_WORLD_REL;
   }
}
