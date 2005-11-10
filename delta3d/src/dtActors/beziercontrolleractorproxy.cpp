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
   mActor = new BezierController();
   dynamic_cast<BezierController*>(mActor.get())->RenderProxyNode(true);
   
   std::ostringstream ss;
   ss << "Controller" << mNumControllers++;
   SetName(ss.str());
}


void BezierControllerActorProxy::BuildPropertyMap()
{
   MotionActionActorProxy::BuildPropertyMap();

   BezierController* bc = dynamic_cast<BezierController*>(mActor.get());

   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierController");
   }

   AddProperty(new dtDAL::ActorActorProperty(*this, "Start Node", "Start Node",
      MakeFunctor(*this, &BezierControllerActorProxy::SetActorStartNode),
      MakeFunctorRet(*this, &BezierControllerActorProxy::GetActorStartNode),
      "dtABC::BezierNode",
      "Sets the start node to be used for this path" ));


   AddProperty(new BooleanActorProperty("Render Path", "Render Path",
      MakeFunctor(*bc, &BezierController::RenderProxyNode),
      MakeFunctorRet(*bc, &BezierController::GetRenderProxyNode),
      "Toggles Rendering of the Path.", "Controller"));

}

dtCore::DeltaDrawable* BezierControllerActorProxy::GetActorStartNode()
{
   BezierController* bc = dynamic_cast<BezierController*>(mActor.get());

   if( bc == 0 )
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor of invalid type, should be a BezierController");
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

   BezierController* bc = dynamic_cast<BezierController*>(mActor.get());

   if(!bc)
   {
      EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor of invalid type, should be a BezierController");
   }

   bc->SetStartNode(bNode);

   bc->CreatePath();
}


}//namespace dtActors


