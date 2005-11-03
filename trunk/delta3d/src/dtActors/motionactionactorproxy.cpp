#include <dtActors/motionactionactorproxy.h>
#include <dtABC/motionaction.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>


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

      dtABC::MotionAction* mo = dynamic_cast<dtABC::MotionAction*>(mActor.get());

      if(!mo)
      {
          EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
      }

      AddProperty(new dtDAL::ActorActorProperty(*this, "Target Object", "Target Object",
         MakeFunctor(*this, &MotionActionActorProxy::SetActorTargetObject),
         "dtCore::Transformable",
         "Sets the object to be moved when this action is triggered"));

   }


   const dtCore::Transformable* MotionActionActorProxy::GetActorTargetObject() const
   {
      const MotionAction *ma = dynamic_cast<const MotionAction*> (mActor.get());
      if(!ma)
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
      }

      return ma->GetTargetObject();
   }

   void MotionActionActorProxy::SetActorTargetObject(ActorProxy* node)
   {
      SetLinkedActor("Target Object", node);

      dtABC::MotionAction* ma = dynamic_cast<dtABC::MotionAction*>(mActor.get());
      dtCore::Transformable* trans = dynamic_cast<dtCore::Transformable*>(node->GetActor());

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


