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
* @author Bradley Anderegg
*/

#include <dtActors/autotriggeractorproxy.h> 
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtABC/autotrigger.h>

using namespace dtABC;
using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
   void AutoTriggerActorProxy::CreateActor()
   {
      mActor = new AutoTrigger;

      SetName( "AutoTrigger" );
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AutoTriggerActorProxy::BuildPropertyMap()
   {
      AutoTrigger* trigger = dynamic_cast<AutoTrigger*>(mActor.get());
      if(!trigger)
      {
         EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::AutoTrigger");
      }

      AddProperty(new ActorActorProperty(*this, "Action","Action",
         MakeFunctor(*this ,&AutoTriggerActorProxy::SetAction),
         MakeFunctorRet(*this ,&AutoTriggerActorProxy::GetAction),
         "dtABC::Action","Sets the action which this Auto Trigger will start."));

      AddProperty(new FloatActorProperty("Time Delay","Time Delay",
         MakeFunctor(*trigger,&AutoTrigger::SetTimeDelay),
         MakeFunctorRet(*trigger,&AutoTrigger::GetTimeDelay),
         "After this trigger has been fired it will wait this amount of time before starting its action.", "dtABC::AutoTrigger"));

   }

   DeltaDrawable* AutoTriggerActorProxy::GetAction()
   {
      AutoTrigger* autoTrigger = dynamic_cast<AutoTrigger*>( mActor.get() );
      if( autoTrigger == 0 )
      {
         EXCEPT(ExceptionEnum::BaseException,"Expected an AutoTrigger actor.");
      }

      return autoTrigger->GetTrigger()->GetAction();  
   }

   void AutoTriggerActorProxy::SetAction( ActorProxy* action )
   {
      SetLinkedActor("Action", action);

      AutoTrigger* autoTrigger = dynamic_cast<AutoTrigger*>( mActor.get() );
      if( autoTrigger == 0 )
      {
         EXCEPT(ExceptionEnum::BaseException,"Expected an AutoTrigger actor.");
      }

      Action* a = NULL;
      if(action)
      {
         a = dynamic_cast<Action*>(action->GetActor());
      }

      autoTrigger->GetTrigger()->SetAction(a);      
   }
}
