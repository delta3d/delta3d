#include <dtActors/beziercontrolleractorproxy.h>
#include <dtABC/beziercontroller.h>
#include <dtDAL/functor.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <sstream>

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
   static_cast<BezierController*>(mActor.get())->RenderProxyNode(true);
   
   std::ostringstream ss;
   ss << "Controller" << mNumControllers++;
   SetName(ss.str());
}


void BezierControllerActorProxy::BuildPropertyMap()
{
   MotionActionActorProxy::BuildPropertyMap();

   BezierController* bc = static_cast<BezierController*>(mActor.get());

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
   BezierController* bc = static_cast<BezierController*>(mActor.get());

   return bc->GetStartNode();
}

void BezierControllerActorProxy::SetActorStartNode(ActorProxy* node)
{

   SetLinkedActor("Start Node", node);

   BezierNode* bNode = NULL;
   if(node)
   {
      bNode = static_cast<BezierNode*>(node->GetActor());
   }

   BezierController* bc = static_cast<BezierController*>(mActor.get());

   bc->SetStartNode(bNode);

   bc->CreatePath();
}


}//namespace dtActors


