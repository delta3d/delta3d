/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 * Chris Osborn
 */

#include <dtActors/proximitytriggeractorproxy.h>

#include <dtABC/proximitytrigger.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/functor.h>

#include <sstream>

using namespace dtABC;
using namespace dtCore;

namespace dtActors
{
   ///////////////////////////////////////////////////////////////////////////////
   void ProximityTriggerActorProxy::CreateDrawable()
   {
      ProximityTrigger* trigger = new ProximityTrigger;
      SetDrawable(*trigger);

      InitializeTrigger( *trigger );
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ProximityTriggerActorProxy::InitializeTrigger(ProximityTrigger& trigger)
   {
      static int mNumTriggers = 0;
      std::ostringstream ss;
      ss << "ProximityTrigger" << mNumTriggers++;
      SetName(ss.str());

   }

   ///////////////////////////////////////////////////////////////////////////////
   void ProximityTriggerActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "ProximityTrigger";

      TransformableActorProxy::BuildPropertyMap();

      ProximityTrigger* trigger = static_cast<ProximityTrigger*>(GetDrawable());

      AddProperty(new ActorActorProperty(*this, "Action","Action",
         ActorActorProperty::SetFuncType(this ,&ProximityTriggerActorProxy::SetAction),
         ActorActorProperty::GetFuncType(this ,&ProximityTriggerActorProxy::GetAction),
         "dtABC::Action","Sets the action which this Proximity Trigger will start."));

      AddProperty(new FloatActorProperty("Time Delay","Time Delay",
         FloatActorProperty::SetFuncType(trigger,&ProximityTrigger::SetTimeDelay),
         FloatActorProperty::GetFuncType(trigger,&ProximityTrigger::GetTimeDelay),
         "After this trigger has been fired it will wait this amount of time before starting its action.", "dtABC::AutoTrigger"));
   }

   //////////////////////////////////////////////////////////////////////////
   ActorProxyIcon* ProximityTriggerActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         //a proximity trigger does not need orientation arrows,
         //this is how to get rid of them.
         dtCore::ActorProxyIcon::ActorProxyIconConfig config;
         config.mForwardVector = false;
         config.mUpVector = false;

         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_TRIGGER, config);
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   void ProximityTriggerActorProxy::SetAction( BaseActorObject* action )
   {
      ProximityTrigger* proximityTrigger;
      GetDrawable(proximityTrigger);

      Action* a = NULL;
      if (action != NULL)
      {
         action->GetDrawable(a);
      }

      proximityTrigger->GetTrigger()->SetAction(a);
   }

   DeltaDrawable* ProximityTriggerActorProxy::GetAction()
   {
      ProximityTrigger* proximityTrigger = static_cast<ProximityTrigger*>(GetDrawable());

      return proximityTrigger->GetTrigger()->GetAction();
   }
}
