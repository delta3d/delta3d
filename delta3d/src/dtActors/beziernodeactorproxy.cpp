#include <dtActors/beziernodeactorproxy.h>
#include <dtABC/beziernode.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>

using namespace dtABC;
using namespace dtDAL;

namespace dtActors
{
   

void BezierNodeActorProxy::CreateActor()
{
   mActor = new dtABC::BezierNode();
}


void BezierNodeActorProxy::BuildPropertyMap()
{
   CurveNodeActorProxy::BuildPropertyMap();

   dtABC::BezierNode* bn = dynamic_cast<dtABC::BezierNode*>(mActor.get());

   const std::string GROUPNAME = "dtABC_BezierNode";

   if(!bn)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
   }

   AddProperty(new dtDAL::ActorActorProperty( "Entry Control", "Exit Control",
      MakeFunctor(*this, &BezierNodeActorProxy::SetActorEntry),
      MakeFunctorRet(*this, &BezierNodeActorProxy::GetActorEntry),
      "Sets the entry control point which effects the curve upon entry",
      GROUPNAME));

   AddProperty(new dtDAL::ActorActorProperty( "Exit Control", "Exit Control",
      MakeFunctor(*this, &BezierNodeActorProxy::SetActorExit),
      MakeFunctorRet(*this, &BezierNodeActorProxy::GetActorExit),
      "Sets the exit control point which effects the curve upon exit",
      GROUPNAME));

}


void BezierNodeActorProxy::SetActorEntry(dtCore::DeltaDrawable* pNode)
{
   dtABC::BezierNode* bNode = dynamic_cast<dtABC::BezierNode*>(mActor.get());
   dtABC::BezierControlPoint* bControl = dynamic_cast<dtABC::BezierControlPoint*>(pNode);

   if(!bNode)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
   }
   if(!bControl)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Trying to set BezierNode's entry control point with invalid type");
   }

   bNode->SetEntry(bControl);

}

dtCore::DeltaDrawable* BezierNodeActorProxy::GetActorEntry()
{
   dtABC::BezierNode* bNode = dynamic_cast<dtABC::BezierNode*>(mActor.get());
   
   if(!bNode)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
   }

   return (dtCore::DeltaDrawable*) bNode->GetEntry();
}


void BezierNodeActorProxy::SetActorExit(dtCore::DeltaDrawable* pNode)
{

   dtABC::BezierNode* bNode = dynamic_cast<dtABC::BezierNode*>(mActor.get());
   dtABC::BezierControlPoint* bControl = dynamic_cast<dtABC::BezierControlPoint*>(pNode);

   if(!bNode)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
   }
   if(!bControl)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Trying to set BezierNode's exit control point with invalid type");
   }

   bNode->SetExit(bControl);


}


dtCore::DeltaDrawable* BezierNodeActorProxy::GetActorExit()
{

   dtABC::BezierNode* bNode = dynamic_cast<dtABC::BezierNode*>(mActor.get());

   if(!bNode)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
   }

   return (dtCore::DeltaDrawable*) bNode->GetExit();
}


}//namespace dtActors

