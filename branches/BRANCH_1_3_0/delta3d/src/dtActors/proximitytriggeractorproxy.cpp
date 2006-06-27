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

#include <dtActors/proximitytriggeractorproxy.h> 
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtABC/proximitytrigger.h>

using namespace dtABC;
using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
   void ProximityTriggerActorProxy::CreateActor()
   {
      ProximityTrigger* trigger = new ProximityTrigger;
      mActor = trigger;

      static int mNumTriggers = 0;
      std::ostringstream ss;
      ss << "ProximityTrigger" << mNumTriggers++;
      SetName(ss.str());

      // Find & set default collision shape and dimensions.
      Transformable::CollisionGeomType* type = trigger->GetCollisionGeomType();

      std::vector<float> dimensions;
      trigger->GetCollisionGeomDimensions( dimensions );

      if(         type == &Transformable::CollisionGeomType::SPHERE && 
                  dimensions.size() == 1 )
      {
            SetCollisionRadius( dimensions[0] );  
      }
      else if(    type == &Transformable::CollisionGeomType::CYLINDER && 
                  dimensions.size() == 2  )
      {
         SetCollisionRadius( dimensions[0] );
         SetCollisionLength( dimensions[1] );
      }
      else if(    type == &Transformable::CollisionGeomType::CUBE && 
                  dimensions.size() == 3  )
      {
         if(   (dimensions[0] == dimensions[1]) &&
               (dimensions[0] == dimensions[2]) &&
               (dimensions[1] == dimensions[2]) )
         {
            SetCollisionLength( dimensions[0] );
         }
      }

      // The collision type of this actor proxy must be set after the dimensions
      // have been set. Otherwise it'll always use a default of 0.0 for all of
      // them and attempt to auto-generate the collision geometry.
      SetCollisionType( *type );
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void ProximityTriggerActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "ProximityTrigger";

      TransformableActorProxy::BuildPropertyMap();

      ProximityTrigger *trigger = dynamic_cast<ProximityTrigger*>(mActor.get());
      if(!trigger)
      {
         EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::ProximityTrigger");
      }

      AddProperty(new ActorActorProperty(*this, "Action","Action",
         MakeFunctor(*this ,&ProximityTriggerActorProxy::SetAction),
         MakeFunctorRet(*this ,&ProximityTriggerActorProxy::GetAction),
         "dtABC::Action","Sets the action which this Proximity Trigger will start."));

      AddProperty(new FloatActorProperty("Time Delay","Time Delay",
         MakeFunctor(*trigger,&ProximityTrigger::SetTimeDelay),
         MakeFunctorRet(*trigger,&ProximityTrigger::GetTimeDelay),
         "After this trigger has been fired it will wait this amount of time before starting its action.", "dtABC::AutoTrigger"));

   }
   
   //////////////////////////////////////////////////////////////////////////
   ActorProxyIcon* ProximityTriggerActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new ActorProxyIcon(ActorProxyIcon::IconType::TRIGGER);
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   void ProximityTriggerActorProxy::SetAction( ActorProxy* action )
   {
      SetLinkedActor("Action", action);

      ProximityTrigger* proximityTrigger = dynamic_cast<ProximityTrigger*>( mActor.get() );
      if( proximityTrigger == 0 )
      {
         EXCEPT(ExceptionEnum::BaseException,"Expected a ProximityTrigger actor.");
      }
      
      Action* a = NULL;
      if(action)
      {
         a = dynamic_cast<Action*>(action->GetActor());
      }

      proximityTrigger->GetTrigger()->SetAction(a);      
   }

   DeltaDrawable* ProximityTriggerActorProxy::GetAction()
   {
      ProximityTrigger* proximityTrigger = dynamic_cast<ProximityTrigger*>( mActor.get() );
      if( proximityTrigger == 0 )
      {
         EXCEPT(ExceptionEnum::BaseException,"Expected a ProximityTrigger actor.");
      }

      return proximityTrigger->GetTrigger()->GetAction();
   }
}
