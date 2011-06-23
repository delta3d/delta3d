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
 * Chris Osborn
 */

#include <dtActors/tripodactorproxy.h>

#include <dtCore/camera.h>
#include <dtCore/transformable.h>
#include <dtCore/tripod.h>

#include <dtDAL/actoractorproperty.h>
#include <dtDAL/enumactorproperty.h>

#include <dtDAL/functor.h>
#include <dtDAL/vectoractorproperties.h>

#include <sstream>

using namespace dtActors;
using namespace dtCore;
using namespace dtDAL;

IMPLEMENT_ENUM(dtActors::TripodActorProxy::TetherModeEnum)
TripodActorProxy::TetherModeEnum TripodActorProxy::TetherModeEnum::TETHER_PARENT_REL("TETHER_PARENT_REL");
TripodActorProxy::TetherModeEnum TripodActorProxy::TetherModeEnum::TETHER_WORLD_REL("TETHER_WORLD_REL");

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::CreateActor()
{
   SetActor(*new Tripod);

   static int actorCount = 0;
   std::ostringstream ss;
   ss << "Tripod" << actorCount++;
   SetName(ss.str());

   //Tripod* tripod = static_cast<Tripod*>(GetActor());
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::BuildPropertyMap()
{
   dtDAL::BaseActorObject::BuildPropertyMap();

   static const std::string GROUPNAME = "Tripod";

   //Tripod* tripod = static_cast<Tripod*>(GetActor());

   AddProperty(new ActorActorProperty(*this, "Child", "Child",
      ActorActorProperty::SetFuncType(this,&TripodActorProxy::SetChild),
      ActorActorProperty::GetFuncType(this,&TripodActorProxy::GetChild),
      "dtCore::Transformable", "Sets the child which this tripod will offset."));

   AddProperty(new ActorActorProperty(*this, "Parent", "Parent",
      ActorActorProperty::SetFuncType(this,&TripodActorProxy::SetAttachToTransformable),
      ActorActorProperty::GetFuncType(this,&TripodActorProxy::GetAttachedTransformable),
      "dtCore::Transformable", "Sets the Transformable which this Tripod will follow."));

   AddProperty(new ActorActorProperty(*this, "Look-At Target", "Look-At Target",
      ActorActorProperty::SetFuncType(this,&TripodActorProxy::SetLookAtTarget),
      ActorActorProperty::GetFuncType(this,&TripodActorProxy::GetLookAtTarget),
      "dtCore::Transformable", "Sets the Transformable which this Tripod will point the Camera at."));

   AddProperty(new Vec3ActorProperty("Rotation Offset", "Rotation Offset",
      Vec3ActorProperty::SetFuncType(this, &TripodActorProxy::SetRotationOffset),
      Vec3ActorProperty::GetFuncType(this, &TripodActorProxy::GetRotationOffset),
      "Sets the amount of rotation to offset the camera from the attached transformable. Represented with heading, pitch, and roll.",
      GROUPNAME));

   AddProperty(new Vec3ActorProperty("Translation Offset", "Translation Offset",
      Vec3ActorProperty::SetFuncType(this, &TripodActorProxy::SetTranslationOffset),
      Vec3ActorProperty::GetFuncType(this, &TripodActorProxy::GetTranslationOffset),
      "Sets the amount of translation to offset the camera from the attached transformable.",
      GROUPNAME));

   AddProperty(new Vec3ActorProperty("Translation Scale Offset", "Translation Scale Offset",
      Vec3ActorProperty::SetFuncType(this, &TripodActorProxy::SetTranslationScaleOffset),
      Vec3ActorProperty::GetFuncType(this, &TripodActorProxy::GetTranslationScaleOffset),
      "Sets the scale on the translation offset.",GROUPNAME));

   AddProperty(new Vec3ActorProperty("Rotation Scale Offset", "Rotation Scale Offset",
      Vec3ActorProperty::SetFuncType(this, &TripodActorProxy::SetRotationScaleOffset),
      Vec3ActorProperty::GetFuncType(this, &TripodActorProxy::GetRotationScaleOffset),
      "Sets the scale on the rotation offset.",GROUPNAME));

   AddProperty(new EnumActorProperty<TetherModeEnum>("Tether Mode","Tether Mode",
      EnumActorProperty<TetherModeEnum>::SetFuncType(this,&TripodActorProxy::SetTetherMode),
      EnumActorProperty<TetherModeEnum>::GetFuncType(this,&TripodActorProxy::GetTetherMode),
      "Sets the tether mode for this tripod actor.", GROUPNAME));
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetCamera(BaseActorObject* cameraProxy)
{
   SetChild(cameraProxy);
}

///////////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetCamera()
{
   return GetChild();
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetChild(BaseActorObject* childProxy)
{
   SetLinkedActor("Child", childProxy);

   Tripod* tripod = static_cast<Tripod*>(GetActor());
   Transformable* child = NULL;

   if (childProxy != NULL)
   {
      child = dynamic_cast<Transformable*>(childProxy->GetActor());
   }

   tripod->SetChild(child);
}

///////////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetChild()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   return tripod->GetCamera();
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetAttachToTransformable(BaseActorObject* transformableProxy)
{
   SetLinkedActor("Parent", transformableProxy);

   Tripod* tripod = static_cast<Tripod*>(GetActor());

   Transformable* parent(0);

   if (transformableProxy != 0)
   {
      parent = dynamic_cast<Transformable*>(transformableProxy->GetActor());
   }

   tripod->SetAttachToTransformable(parent);
}

///////////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetAttachedTransformable()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   return tripod->GetAttachedTransformable();
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetLookAtTarget(BaseActorObject* targetProxy)
{
   SetLinkedActor("Look-At Target", targetProxy);

   Tripod* tripod = static_cast<Tripod*>(GetActor());

   Transformable* target(0);

   if (targetProxy != 0)
   {
      target = dynamic_cast<Transformable*>(targetProxy->GetActor());
   }

   tripod->SetLookAtTarget(target);
}

///////////////////////////////////////////////////////////////////////////////////
DeltaDrawable* TripodActorProxy::GetLookAtTarget()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   return tripod->GetLookAtTarget();
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetRotationOffset(const osg::Vec3& rotation)
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 hpr = rotation;

   //Normalize the rotation.
   if (hpr.x() < 0.0f)
   {
      hpr.x() += 360.0f;
   }

   if (hpr.x() > 360.0f)
   {
      hpr.x() -= 360.0f;
   }

   if (hpr.y() < 0.0f)
   {
      hpr.y() += 360.0f;
   }
   if (hpr.y() > 360.0f)
   {
      hpr.y() -= 360.0f;
   }

   if (hpr.z() < 0.0f)
   {
      hpr.z() += 360.0f;
   }

   if (hpr.z() > 360.0f)
   {
      hpr.z() -= 360.0f;
   }

   osg::Vec3 xyz;
   osg::Vec3 oldHPR;
   tripod->GetOffset(xyz, oldHPR);
   tripod->SetOffset(xyz, hpr);
}

///////////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetRotationOffset()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetOffset(xyz, hpr);

   return hpr;
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetTranslationOffset(const osg::Vec3& translation)
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 oldXYZ;
   osg::Vec3 hpr;
   tripod->GetOffset(oldXYZ, hpr);
   tripod->SetOffset(translation, hpr);
}

///////////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetTranslationOffset()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetOffset(xyz, hpr);

   return xyz;
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetTranslationScaleOffset(const osg::Vec3& scale)
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 oldXYZ;
   osg::Vec3 hpr;
   tripod->GetScale(oldXYZ, hpr);
   tripod->SetScale(scale, hpr);
}

///////////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetTranslationScaleOffset()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());
   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetScale(xyz, hpr);

   return xyz;
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetRotationScaleOffset(const osg::Vec3& scale)
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 xyz;
   osg::Vec3 oldScale;
   tripod->GetScale(xyz, oldScale);
   tripod->SetScale(xyz, scale);
}

///////////////////////////////////////////////////////////////////////////////////
osg::Vec3 TripodActorProxy::GetRotationScaleOffset()
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   osg::Vec3 xyz;
   osg::Vec3 hpr;
   tripod->GetScale(xyz, hpr);

   return hpr;
}

///////////////////////////////////////////////////////////////////////////////////
void TripodActorProxy::SetTetherMode(TripodActorProxy::TetherModeEnum& mode)
{
   Tripod* tripod = static_cast<Tripod*>(GetActor());

   if (mode == TetherModeEnum::TETHER_PARENT_REL)
   {
      tripod->SetTetherMode(Tripod::TETHER_PARENT_REL);
   }
   else if (mode == TetherModeEnum::TETHER_WORLD_REL)
   {
      tripod->SetTetherMode(Tripod::TETHER_WORLD_REL);
   }
}

///////////////////////////////////////////////////////////////////////////////////
TripodActorProxy::TetherModeEnum& TripodActorProxy::GetTetherMode() const
{
   const Tripod* tripod = static_cast<const Tripod*>(GetActor());

   if (tripod->GetTetherMode() == Tripod::TETHER_PARENT_REL)
   {
      return TetherModeEnum::TETHER_PARENT_REL;
   }
   else
   {
      return TetherModeEnum::TETHER_WORLD_REL;
   }
}

///////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtDAL::ActorProperty> TripodActorProxy::GetDeprecatedProperty(const std::string& name)
{
   dtCore::RefPtr<dtDAL::ActorProperty> result;
   if (name == "Camera")
   {
      result = new ActorActorProperty(*this, "Camera", "Camera",
         ActorActorProperty::SetFuncType(this, &TripodActorProxy::SetCamera),
         ActorActorProperty::GetFuncType(this, &TripodActorProxy::GetCamera),
         "dtCore::Camera", "DEPRECATED - USE Child INSTEAD.");
   }
   return result;
}