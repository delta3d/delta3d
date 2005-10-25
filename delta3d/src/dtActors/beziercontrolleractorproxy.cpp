#include <dtActors/beziercontrolleractorproxy.h>
#include <dtABC/beziercontroller.h>
#include <dtDAL/functor.h>
#include <dtDAL/enginepropertytypes.h>

using namespace dtABC;
using namespace dtDAL;


namespace dtActors
{

const ActorProxy::RenderMode& BezierControllerActorProxy::GetRenderMode()
{
   return ActorProxy::RenderMode::DRAW_ACTOR;
}

void BezierControllerActorProxy::CreateActor()
{
   mActor = new dtABC::BezierController();
   dynamic_cast<dtABC::BezierController*>(mActor.get())->RenderProxyNode(true);
}


void BezierControllerActorProxy::BuildPropertyMap()
{
   MotionActionActorProxy::BuildPropertyMap();

   dtABC::BezierController* bc = dynamic_cast<dtABC::BezierController*>(mActor.get());
   const std::string GROUPNAME = "dtABC_BezierController";

   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierController");
   }

   AddProperty(new dtDAL::ActorActorProperty( "Start Node", "Start Node",
      MakeFunctor(*this, &BezierControllerActorProxy::SetActorStartNode),
      MakeFunctorRet(*this, &BezierControllerActorProxy::GetActorStartNode),
      "Sets the start node to be used for this path",
      GROUPNAME));

}


dtCore::DeltaDrawable* BezierControllerActorProxy::GetActorStartNode()
{
   dtABC::BezierController* bc = dynamic_cast<dtABC::BezierController*>(mActor.get());

   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierController");
   }

   return (dtCore::DeltaDrawable*)bc->GetStartNode();
}


void BezierControllerActorProxy::SetActorStartNode(dtCore::DeltaDrawable* pBezierNode)
{
   BezierNode* bNode = dynamic_cast<BezierNode*>(pBezierNode);
   dtABC::BezierController* bc = dynamic_cast<dtABC::BezierController*>(mActor.get());

   if(!bNode)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Invalid property type to set start node of BezierController");
   }
   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor of invalid type, should be a BezierController");
   }

   bc->SetStartNode(bNode);
}


}//namespace dtActors


