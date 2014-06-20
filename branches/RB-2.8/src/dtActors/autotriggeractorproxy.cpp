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
 * Bradley Anderegg
 */

#include <dtActors/autotriggeractorproxy.h>

#include <dtABC/autotrigger.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/floatactorproperty.h>

using namespace dtABC;
using namespace dtCore;
using namespace dtCore;

namespace dtActors
{
   void AutoTriggerActorProxy::CreateDrawable()
   {
      SetDrawable(*new AutoTrigger);

      SetName("AutoTrigger");
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AutoTriggerActorProxy::BuildPropertyMap()
   {
      dtCore::BaseActorObject::BuildPropertyMap();

      AutoTrigger* trigger = NULL;
      GetDrawable(trigger);

      AddProperty(new ActorActorProperty(*this, "Action","Action",
               ActorActorProperty::SetFuncType(this ,&AutoTriggerActorProxy::SetAction),
               ActorActorProperty::GetFuncType(this ,&AutoTriggerActorProxy::GetAction),
               "dtABC::Action","Sets the action which this Auto Trigger will start."));

      AddProperty(new FloatActorProperty("Time Delay","Time Delay",
               FloatActorProperty::SetFuncType(trigger,&AutoTrigger::SetTimeDelay),
               FloatActorProperty::GetFuncType(trigger,&AutoTrigger::GetTimeDelay),
               "After this trigger has been fired it will wait this amount of time before starting its action.", "dtABC::AutoTrigger"));

   }

   DeltaDrawable* AutoTriggerActorProxy::GetAction()
   {
      AutoTrigger* autoTrigger = static_cast<AutoTrigger*>(GetDrawable());

      return autoTrigger->GetTrigger()->GetAction();
   }

   void AutoTriggerActorProxy::SetAction(BaseActorObject* action)
   {
      AutoTrigger* autoTrigger = static_cast<AutoTrigger*>(GetDrawable());

      Action* a = NULL;
      if (action)
      {
         a = dynamic_cast<Action*>(action->GetDrawable());
      }

      autoTrigger->GetTrigger()->SetAction(a);
   }
}
