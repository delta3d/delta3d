#include <dtActors/curvenodeactorproxy.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtABC/curvenode.h>
#include <dtDAL/functor.h>

using namespace dtABC;
using namespace dtDAL;

namespace dtActors
{


bool CurveNodeActorProxy::IsPlaceable()
{
   return true;
}

const dtDAL::ActorProxy::RenderMode& CurveNodeActorProxy::GetRenderMode()
{
   return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
}

void CurveNodeActorProxy::BuildPropertyMap()
{
   dtABC::CurveNode* cn = dynamic_cast<dtABC::CurveNode*>(mActor.get());
   const std::string GROUPNAME = "dtABC_CurveNode";

   if(!cn)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::CurveNode");
   }

   AddProperty(new dtDAL::FloatActorProperty("Time To Next Node", "Time To Next Node", 
               MakeFunctor(*cn, &CurveNode::SetTimeToNext),
               MakeFunctorRet(*cn, &CurveNode::GetTimeToNext),
               "Sets the amount of time it will take to get to the next node, in seconds",
                GROUPNAME));

   AddProperty(new dtDAL::FloatActorProperty("Time Step", "Time Step",
               MakeFunctor(*cn, &CurveNode::SetStep), 
               MakeFunctorRet(*cn, (&CurveNode::GetStep)),
               "Defines the time step to interpolate this segment by in seconds",
               GROUPNAME));


   AddProperty(new dtDAL::ActorActorProperty( "Next Node", "Next Node",
               MakeFunctor(*this, &CurveNodeActorProxy::SetActorNext),
               MakeFunctorRet(*this, &CurveNodeActorProxy::GetActorNext),
               "Sets the next node to be used for the curve",
               GROUPNAME));


   AddProperty(new dtDAL::ActorActorProperty( "Previous Node", "Previous Node",
               MakeFunctor(*this, &CurveNodeActorProxy::SetActorPrev),
               MakeFunctorRet(*this, &CurveNodeActorProxy::GetActorPrev),
               "Sets the previous node to be used for the curve",
               GROUPNAME));
}



void CurveNodeActorProxy::SetActorPrev(dtCore::DeltaDrawable* pDrawable)
{
   dtABC::CurveNode* cn = dynamic_cast<dtABC::CurveNode*>(mActor.get());
   dtABC::CurveNode* pPrev = dynamic_cast<dtABC::CurveNode*>(pDrawable);

   if(!cn)
   {
      EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::CurveNode");
   }
   if(!pPrev)
   {
      EXCEPT(ExceptionEnum::InvalidActorException, "Cannot set Previous Node that is not of type CurveNode");
   }
   cn->SetPrev(pPrev);
}

void CurveNodeActorProxy::SetActorNext(dtCore::DeltaDrawable* pDrawable)
{

   dtABC::CurveNode* cn = dynamic_cast<dtABC::CurveNode*>(mActor.get());
   dtABC::CurveNode* pNext = dynamic_cast<dtABC::CurveNode*>(pDrawable);

   if(!cn)
   {
      EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::CurveNode");
   }
   if(!pNext)
   {
      EXCEPT(ExceptionEnum::InvalidActorException, "Cannot set Next Node that is not of type CurveNode");
   }
   cn->SetNext(pNext);

}

dtCore::DeltaDrawable* CurveNodeActorProxy::GetActorPrev()
{
   dtABC::CurveNode* cn = dynamic_cast<dtABC::CurveNode*>(mActor.get());
   
   if(!cn)
   {
      EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::CurveNode");
   }

   return (dtCore::DeltaDrawable*) cn->GetPrev();
}

dtCore::DeltaDrawable* CurveNodeActorProxy::GetActorNext()
{
   dtABC::CurveNode* cn = dynamic_cast<dtABC::CurveNode*>(mActor.get());

   if(!cn)
   {
      EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtABC::CurveNode");
   }

   return (dtCore::DeltaDrawable*)cn->GetNext();
}



}//namespace dtActors

