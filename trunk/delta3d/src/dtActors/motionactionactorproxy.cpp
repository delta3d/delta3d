#include <dtActors/motionactionactorproxy.h>
#include <dtABC/motionaction.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>


using namespace dtABC;
using namespace dtDAL;

namespace dtActors
{

   bool MotionActionActorProxy::IsPlaceable()
   {
      return false;
   }

   const ActorProxy::RenderMode& MotionActionActorProxy::GetRenderMode()
   {
      return ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }


   void MotionActionActorProxy::BuildPropertyMap()
   {
      dtABC::MotionAction* mo = dynamic_cast<dtABC::MotionAction*>(mActor.get());
      const std::string GROUPNAME = "dtABC_MotionAction";

      if(!mo)
      {
          EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
      }

      AddProperty(new dtDAL::ActorActorProperty( "Target Object", "Target Object",
         MakeFunctor(*this, &MotionActionActorProxy::SetActorTargetObject),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetActorTargetObject),
         "Sets the object to be moved when this action is triggered",
         GROUPNAME));

   }

   void MotionActionActorProxy::SetActorTargetObject(dtCore::DeltaDrawable* pDrawable)
   {
      dtABC::MotionAction* ma = dynamic_cast<dtABC::MotionAction*>(mActor.get());
      dtCore::Transformable* trans = dynamic_cast<dtCore::Transformable*>(pDrawable);

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

   dtCore::DeltaDrawable* MotionActionActorProxy::GetActorTargetObject()
   {
      dtABC::MotionAction* ma = dynamic_cast<dtABC::MotionAction*>(mActor.get());
    
      if(!ma)
      {
         EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
      }

      return (dtCore::DeltaDrawable*)ma->GetTargetObject();   
   }


}//namespace dtActors


