/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 * MG
 */

#include <dtActors/triggervolumeactorproxy.h>
#include <dtActors/triggervolumeactor.h>

#include <dtCore/datatype.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/actorproxyicon.h>

using namespace dtActors;

const dtUtil::RefString TriggerVolumeActorProxy::CLASS_NAME("dtActors::TriggerVolumeActorProxy");
const dtUtil::RefString TriggerVolumeActorProxy::PROPERTY_MAX_TRIGGER_COUNT("MaxTriggerCount");

////////////////////////////////////////////////////////////////////////////////
TriggerVolumeActorProxy::TriggerVolumeActorProxy()
: dtGame::GameActorProxy()
{
   SetClassName(TriggerVolumeActorProxy::CLASS_NAME.Get());
}

////////////////////////////////////////////////////////////////////////////////
TriggerVolumeActorProxy::~TriggerVolumeActorProxy()
{
}

////////////////////////////////////////////////////////////////////////////////
void TriggerVolumeActorProxy::CreateDrawable()
{
   SetDrawable(*new TriggerVolumeActor(*this));
}

////////////////////////////////////////////////////////////////////////////////
void TriggerVolumeActorProxy::BuildPropertyMap()
{
   dtGame::GameActorProxy::BuildPropertyMap();

   // Get the actor's interface.
   TriggerVolumeActor* actor = NULL;
   GetDrawable(actor);

   const std::string GROUP_TRIGGER("Trigger");

   AddProperty(new dtCore::IntActorProperty(
      TriggerVolumeActorProxy::PROPERTY_MAX_TRIGGER_COUNT,
      TriggerVolumeActorProxy::PROPERTY_MAX_TRIGGER_COUNT,
      dtCore::IntActorProperty::SetFuncType(actor, &TriggerVolumeActor::SetMaxTriggerCount),
      dtCore::IntActorProperty::GetFuncType(actor, &TriggerVolumeActor::GetMaxTriggerCount),
      "Sets the maximum number of times the trigger can active.  0 means an infinite number.",
      GROUP_TRIGGER));
}

//////////////////////////////////////////////////////////////////////////
const dtCore::BaseActorObject::RenderMode& dtActors::TriggerVolumeActorProxy::GetRenderMode()
{
   if (IsInSTAGE())
   {
      return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   return dtGame::GameActorProxy::GetRenderMode();
}

//////////////////////////////////////////////////////////////////////////
dtCore::ActorProxyIcon* dtActors::TriggerVolumeActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      dtCore::ActorProxyIcon::ActorProxyIconConfig cfg(false,false,1.0f);
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_TRIGGER, cfg);
   }

   return mBillBoardIcon.get();
}
