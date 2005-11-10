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
* @author William E. Johnson II
* @author Chris Osborn
*/

#include <dtActors/triggeractorproxy.h> 
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtABC/trigger.h>

using namespace dtABC;
using namespace dtDAL;

namespace dtActors 
{
   void TriggerActorProxy::CreateActor()
   {
      mActor = new Trigger;

      static int mNumTriggers = 0;
      std::ostringstream ss;
      ss << "Trigger" << mNumTriggers++;
      SetName(ss.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TriggerActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "Trigger";

      Trigger *trigger = dynamic_cast<Trigger*>(mActor.get());
      if(!trigger)
      {
         EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::Trigger");
      }

      AddProperty(new BooleanActorProperty("Enable","Enabled",
         MakeFunctor(*trigger,&Trigger::SetEnabled),
         MakeFunctorRet(*trigger,&Trigger::GetEnabled),
         "Sets whether or not the trigger is enabled in the scene.", GROUPNAME));

      AddProperty(new DoubleActorProperty("Time Delay","Time Delay",
         MakeFunctor(*trigger,&Trigger::SetTimeDelay),
         MakeFunctorRet(*trigger,&Trigger::GetTimeDelay),
         "After this trigger has been fired it will wait this amount of time before starting its action.", GROUPNAME));

      AddProperty(new ActorActorProperty(*this, "Action","Action",
         MakeFunctor(*this,&TriggerActorProxy::SetAction),
         MakeFunctorRet(*this,&TriggerActorProxy::GetAction),
         "dtABC::Action", "Sets the action which this Trigger will start."));
   }

   void TriggerActorProxy::SetAction( ActorProxy* action )
   {
      SetLinkedActor("Action", action);

      Trigger* trigger = dynamic_cast<Trigger*>( mActor.get() );
      if( trigger == 0 )
      {
         EXCEPT(ExceptionEnum::BaseException,"Expected a Trigger actor.");
      }

      Action* a(0);
      
      if( action != 0 )
      {
         a = dynamic_cast<Action*>( action->GetActor() );
      }

      trigger->SetAction(a);      
   }

   dtCore::DeltaDrawable* TriggerActorProxy::GetAction()
   {
      Trigger* trigger = dynamic_cast<Trigger*>( mActor.get() );
      if( trigger == 0 )
      {
         EXCEPT(ExceptionEnum::BaseException,"Expected a Trigger actor.");
      }

      return trigger->GetAction();
   }
}
