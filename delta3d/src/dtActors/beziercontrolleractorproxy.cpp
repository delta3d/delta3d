#include <dtActors/beziercontrolleractorproxy.h>
#include <dtABC/beziercontroller.h>
#include <dtDAL/functor.h>
#include <dtDAL/enginepropertytypes.h>

using namespace dtABC;
using namespace dtDAL;


namespace dtActors
{

int BezierControllerActorProxy::mNumControllers = 0;

//const ActorProxy::RenderMode& BezierControllerActorProxy::GetRenderMode()
//{
//   return ActorProxy::RenderMode::DRAW_AUTO;
//}

void BezierControllerActorProxy::CreateActor()
{
   mActor = new dtABC::BezierController();
   dynamic_cast<dtABC::BezierController*>(mActor.get())->RenderProxyNode(true);
   
   std::ostringstream ss;
   ss << "Controller" << mNumControllers++;
   SetName(ss.str());
}


void BezierControllerActorProxy::BuildPropertyMap()
{
   MotionActionActorProxy::BuildPropertyMap();

   dtABC::BezierController* bc = dynamic_cast<dtABC::BezierController*>(mActor.get());

   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierController");
   }

   AddProperty(new dtDAL::ActorActorProperty(*this, "Start Node", "Start Node",
      MakeFunctor(*this, &BezierControllerActorProxy::SetActorStartNode),
      "dtABC::BezierNode",
      "Sets the start node to be used for this path" ));

}

const dtABC::BezierNode* BezierControllerActorProxy::GetActorStartNode() const
{
   const dtABC::BezierController *bc = dynamic_cast<const BezierController*> (mActor.get());
   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::MotionAction");
   }

   return bc->GetStartNode();
}


void BezierControllerActorProxy::SetActorStartNode(ActorProxy* node)
{

   SetLinkedActor("Start Node", node);

   BezierNode* bNode = NULL;
   if(node)
   {
      bNode = dynamic_cast<BezierNode*>(node->GetActor());
      if(!bNode)
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Invalid property type to set start node of BezierController");
      }
   }

   dtABC::BezierController* bc = dynamic_cast<dtABC::BezierController*>(mActor.get());

   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor of invalid type, should be a BezierController");
   }

   bc->SetStartNode(bNode);

   bc->CreatePath();
}


}//namespace dtActors


