#include <dtActors/motionactionactorproxy.h>
#include <dtABC/motionaction.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtDAL;

namespace dtActors
{

   IMPLEMENT_ENUM(MotionActionActorProxy::ParentRelationEnum);

   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::NO_RELATION("NO_RELATION");
   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::TRACK_PARENT("TRACK_PARENT");
   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::FOLLOW_PARENT("FOLLOW_PARENT");
   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::TRACK_AND_FOLLOW("TRACK_AND_FOLLOW");

   bool MotionActionActorProxy::IsPlaceable()
   {
      return false;
   }

   void MotionActionActorProxy::BuildPropertyMap()
   {
      ActionActorProxy::BuildPropertyMap();

      MotionAction* mo = dynamic_cast<MotionAction*>(mActor.get());

      if(!mo)
      {
          EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
      }

      AddProperty(new ActorActorProperty(*this, "Target Object", "Target Object",
         MakeFunctor(*this, &MotionActionActorProxy::SetActorTargetObject),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetActorTargetObject),
         "dtCore::Transformable",
         "Sets the object to be moved when this action is triggered"));

   }

   DeltaDrawable* MotionActionActorProxy::GetActorTargetObject()
   {
      MotionAction* mo = dynamic_cast< MotionAction* >( mActor.get() );

      if( mo == 0 )
      {
         EXCEPT( ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction" );
      }

      return mo->GetTargetObject();
   }

   void MotionActionActorProxy::SetActorTargetObject(ActorProxy* node)
   {
      SetLinkedActor("Target Object", node);

      MotionAction* ma = dynamic_cast<MotionAction*>(mActor.get());
      Transformable* trans = dynamic_cast<Transformable*>(node->GetActor());

      if(!ma)
      {
         EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
      }
      if(!trans)
      {
         EXCEPT(ExceptionEnum::InvalidActorException, "Cannot set MotionAction target to non-transformable");
      }

      ma->SetTargetObject(trans);
      
   }



}//namespace dtActors


