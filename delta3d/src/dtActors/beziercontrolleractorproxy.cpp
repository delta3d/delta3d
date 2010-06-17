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
   SetActor(*new BezierController());
   static_cast<BezierController*>(GetActor())->RenderProxyNode(true);

   std::ostringstream ss;
   ss << "Controller" << mNumControllers++;
   SetName(ss.str());
}


void BezierControllerActorProxy::BuildPropertyMap()
{
   MotionActionActorProxy::BuildPropertyMap();

   BezierController* bc = static_cast<BezierController*>(GetActor());

   AddProperty(new dtDAL::ActorActorProperty(*this, "Start Node", "Start Node",
      ActorActorProperty::SetFuncType(this, &BezierControllerActorProxy::SetActorStartNode),
      ActorActorProperty::GetFuncType(this, &BezierControllerActorProxy::GetActorStartNode),
      "dtABC::BezierNode",
      "Sets the start node to be used for this path" ));

   AddProperty(new BooleanActorProperty("Render Path", "Render Path",
      BooleanActorProperty::SetFuncType(bc, &BezierController::RenderProxyNode),
      BooleanActorProperty::GetFuncType(bc, &BezierController::GetRenderProxyNode),
      "Toggles Rendering of the Path.", "Controller"));

   AddProperty(new BooleanActorProperty("Follow Path", "Follow Path",
      BooleanActorProperty::SetFuncType(bc, &BezierController::SetFollowPath),
      BooleanActorProperty::GetFuncType(bc, &BezierController::GetFollowPath),
      "Toggles path following oriented against start node.", "Controller"));

   AddProperty(new BooleanActorProperty("Set Looping", "Set Looping",
      BooleanActorProperty::SetFuncType(bc, &BezierController::SetLooping),
      BooleanActorProperty::GetFuncType(bc, &BezierController::GetLooping),
      "Toggles The looping of the Path.", "Controller"));
}

dtCore::DeltaDrawable* BezierControllerActorProxy::GetActorStartNode()
{
   BezierController* bc = static_cast<BezierController*>(GetActor());

   return bc->GetStartNode();
}

void BezierControllerActorProxy::SetActorStartNode(ActorProxy* node)
{

   SetLinkedActor("Start Node", node);

   BezierNode* bNode = NULL;
   if (node)
   {
      bNode = static_cast<BezierNode*>(node->GetActor());
   }

   BezierController* bc = static_cast<BezierController*>(GetActor());

   bc->SetStartNode(bNode);

   bc->CreatePath();
}


}//namespace dtActors


