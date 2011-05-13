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

#include <dtActors/triggeractorproxy.h>

#include <dtABC/trigger.h>

#include <dtDAL/actoractorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/doubleactorproperty.h>

#include <dtDAL/functor.h>

#include <sstream>

using namespace dtABC;
using namespace dtDAL;

namespace dtActors
{
   void TriggerActorProxy::CreateActor()
   {
      SetActor(*new Trigger);

      static int mNumTriggers = 0;
      std::ostringstream ss;
      ss << "Trigger" << mNumTriggers++;
      SetName(ss.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TriggerActorProxy::BuildPropertyMap()
   {
      dtDAL::BaseActorObject::BuildPropertyMap();

      const std::string GROUPNAME = "Trigger";

      Trigger *trigger = static_cast<Trigger*>(GetActor());

      AddProperty(new BooleanActorProperty("Enable","Enabled",
         BooleanActorProperty::SetFuncType(trigger,&Trigger::SetEnabled),
         BooleanActorProperty::GetFuncType(trigger,&Trigger::GetEnabled),
         "Sets whether or not the trigger is enabled in the scene.", GROUPNAME));

      AddProperty(new DoubleActorProperty("Time Delay","Time Delay",
         DoubleActorProperty::SetFuncType(trigger,&Trigger::SetTimeDelay),
         DoubleActorProperty::GetFuncType(trigger,&Trigger::GetTimeDelay),
         "After this trigger has been fired it will wait this amount of time before starting its action.", GROUPNAME));

      AddProperty(new ActorActorProperty(*this, "Action","Action",
         ActorActorProperty::SetFuncType(this,&TriggerActorProxy::SetAction),
         ActorActorProperty::GetFuncType(this,&TriggerActorProxy::GetAction),
         "dtABC::Action", "Sets the action which this Trigger will start."));
   }

   void TriggerActorProxy::SetAction( BaseActorObject* action )
   {
      SetLinkedActor("Action", action);

      Trigger* trigger = static_cast<Trigger*>( GetActor() );

      Action* a(0);

      if ( action != 0 )
      {
         a = dynamic_cast<Action*>( action->GetActor() );
      }

      trigger->SetAction(a);
   }

   dtCore::DeltaDrawable* TriggerActorProxy::GetAction()
   {
      Trigger* trigger = static_cast<Trigger*>( GetActor() );

      return trigger->GetAction();
   }
}
