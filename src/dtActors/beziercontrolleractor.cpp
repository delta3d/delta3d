/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* Bradley Anderegg
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtActors/beziercontrolleractor.h>
#include <dtABC/beziercontroller.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/functor.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>

#include <sstream>

using namespace dtABC;
using namespace dtCore;


namespace dtActors
{
////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
////////////////////////////////////////////////////////////////////////////////
int BezierControllerActor::mNumControllers = 0;

const dtUtil::RefString BezierControllerActor::CLASS_NAME("dtABC::BezierController");

const dtUtil::RefString BezierControllerActor::INVOKABLE_PROCESS_GAME_EVENT("ProcessGameEvent");

const dtUtil::RefString BezierControllerActor::PROPERTY_START_NODE("Start Node");
const dtUtil::RefString BezierControllerActor::PROPERTY_RENDER_PATH("Render Path");
const dtUtil::RefString BezierControllerActor::PROPERTY_FOLLOW_PATH("Follow Path");
const dtUtil::RefString BezierControllerActor::PROPERTY_SET_LOOPING("Set Looping");

//const dtCore::BaseActorObject::RenderMode& BezierControllerActor::GetRenderMode()
//{
//   return dtCore::BaseActorObject::RenderMode::DRAW_AUTO;
//}

BezierControllerActor::BezierControllerActor()
{
   SetClassName(CLASS_NAME);
}

BezierControllerActor::~BezierControllerActor()
{}

bool BezierControllerActor::IsPlaceable() const
{
   return false;
}

void BezierControllerActor::CreateDrawable()
{
   SetDrawable(*new BezierController());
   static_cast<BezierController*>(GetDrawable())->RenderProxyNode(true);

   std::ostringstream ss;
   ss << "Controller" << mNumControllers++;
   SetName(ss.str());
}

void BezierControllerActor::BuildInvokables()
{
   BaseClass::BuildInvokables();

   AddInvokable(*new dtGame::Invokable(INVOKABLE_PROCESS_GAME_EVENT,
      dtUtil::MakeFunctor(&BezierControllerActor::ProcessGameEvent, this)));
}

void BezierControllerActor::BuildPropertyMap()
{
   BaseClass::BuildPropertyMap();

   using namespace dtUtil; // for RefString

   BezierController* bc = NULL;
   GetDrawable(bc);

   RefString GROUP("Controller");

   AddProperty(new dtCore::ActorActorProperty(*this,
      PROPERTY_START_NODE,
      PROPERTY_START_NODE,
      ActorActorProperty::SetFuncType(this, &BezierControllerActor::SetActorStartNode),
      ActorActorProperty::GetFuncType(this, &BezierControllerActor::GetActorStartNode),
      RefString("dtABC::BezierNode"),
      RefString("Sets the start node to be used for this path"),
      GROUP));

   AddProperty(new BooleanActorProperty(
      PROPERTY_RENDER_PATH,
      PROPERTY_RENDER_PATH,
      BooleanActorProperty::SetFuncType(bc, &BezierController::RenderProxyNode),
      BooleanActorProperty::GetFuncType(bc, &BezierController::GetRenderProxyNode),
      RefString("Toggles Rendering of the Path."),
      GROUP));

   AddProperty(new BooleanActorProperty(
      PROPERTY_FOLLOW_PATH,
      PROPERTY_FOLLOW_PATH,
      BooleanActorProperty::SetFuncType(bc, &BezierController::SetFollowPath),
      BooleanActorProperty::GetFuncType(bc, &BezierController::GetFollowPath),
      RefString("Toggles path following oriented against start node."),
      GROUP));

   AddProperty(new BooleanActorProperty(
      PROPERTY_SET_LOOPING,
      PROPERTY_SET_LOOPING,
      BooleanActorProperty::SetFuncType(bc, &BezierController::SetLooping),
      BooleanActorProperty::GetFuncType(bc, &BezierController::GetLooping),
      RefString("Toggles The looping of the Path."),
      GROUP));
}

void BezierControllerActor::BuildActorComponents()
{
   BaseClass::BuildActorComponents();
   
   mComp = new BezierControllerActorComponent;
   AddComponent(*mComp);
}

void BezierControllerActor::OnEnteredWorld()
{
   BaseClass::OnEnteredWorld();

   RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT, INVOKABLE_PROCESS_GAME_EVENT);
}

dtCore::DeltaDrawable* BezierControllerActor::GetActorStartNode()
{
   BezierController* bc = static_cast<BezierController*>(GetDrawable());

   return bc->GetStartNode();
}

void BezierControllerActor::SetActorStartNode(dtCore::BaseActorObject* node)
{
   BezierNode* bNode = NULL;
   if (node)
   {
      bNode = static_cast<BezierNode*>(node->GetDrawable());
   }

   dtABC::BezierController* bc = NULL;
   GetDrawable(bc);

   bc->SetStartNode(bNode);
   bc->CreatePath();
}

void BezierControllerActor::ProcessGameEvent(const dtGame::GameEventMessage& gameEventMessage)
{
   mComp->ProcessGameEvent(gameEventMessage);
}


}//namespace dtActors


