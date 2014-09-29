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
#include <dtActors/beziercontrolleractorcomponent.h>
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
const dtGame::ActorComponent::ACType BezierControllerActorComponent::TYPE(new dtCore::ActorType("BezierController", "ActorComponents", "", dtGame::ActorComponent::BaseActorComponentType));

const dtUtil::RefString BezierControllerActorComponent::CLASS_NAME("dtActors.BezierControllerActorComponent");

const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_TO_START("Event To Start");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_TO_PAUSE("Event To Pause");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_TO_UNPAUSE("Event To Unpause");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_ON_START("Event On Start");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_ON_PAUSE("Event On Pause");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_ON_UNPAUSE("Event On Unpause");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_ON_NEXT_STEP("Event On Next Step");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_ON_RESTART("Event On Restart");
const dtUtil::RefString BezierControllerActorComponent::PROPERTY_EVENT_ON_END("Event On End");

//const dtCore::BaseActorObject::RenderMode& BezierControllerActorComponent::GetRenderMode()
//{
//   return dtCore::BaseActorObject::RenderMode::DRAW_AUTO;
//}

BezierControllerActorComponent::BezierControllerActorComponent(const ACType& type)
   : BaseClass(type)
{
   SetClassName(CLASS_NAME);
}

BezierControllerActorComponent::~BezierControllerActorComponent()
{}

dtABC::BezierController* BezierControllerActorComponent::GetController()
{
   dtABC::BezierController* controller = NULL;
   BaseActorObject* actor = NULL;
   GetOwner(actor);

   if (actor != NULL)
   {
      actor->GetDrawable(controller);
   }

   return controller;
}

void BezierControllerActorComponent::OnEnteredWorld()
{
   BaseClass::OnEnteredWorld();

   // Setup sig/Slots with the drawable.
   BezierController* bc = GetController();

   if (bc == NULL)
   {
      LOG_ERROR("Could not access controller.");
   }
   else
   {
      bc->SignalStarted.connect_slot(this, &BezierControllerActorComponent::OnStart);
      bc->SignalPaused.connect_slot(this, &BezierControllerActorComponent::OnPause);
      bc->SignalUnPaused.connect_slot(this, &BezierControllerActorComponent::OnUnPause);
      bc->SignalRestarted.connect_slot(this, &BezierControllerActorComponent::OnRestart);
      bc->SignalNextStep.connect_slot(this, &BezierControllerActorComponent::OnNextStep);
      bc->SignalEnded.connect_slot(this, &BezierControllerActorComponent::OnEnd);
   }
}

void BezierControllerActorComponent::BuildPropertyMap()
{
   BaseClass::BuildPropertyMap();

   using namespace dtUtil; // for RefString

   BezierController* bc = GetController();

   RefString GROUP("Controller");

   // GAME EVENT PROPERTIES
   // --- "TO" Events
   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_TO_START,
      PROPERTY_EVENT_TO_START,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventToStart),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventToStart),
      RefString("Event to start the controller"),
      GROUP));

   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_TO_PAUSE,
      PROPERTY_EVENT_TO_PAUSE,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventToPause),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventToPause),
      RefString("Event to pause the controller"),
      GROUP));

   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_TO_UNPAUSE,
      PROPERTY_EVENT_TO_UNPAUSE,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventToUnPause),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventToUnPause),
      RefString("Event to unpause the controller"),
      GROUP));

   // --- "ON" Events
   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_ON_START,
      PROPERTY_EVENT_ON_START,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventOnStart),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventOnStart),
      RefString("Event to send when the controller starts"),
      GROUP));
   
   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_ON_PAUSE,
      PROPERTY_EVENT_ON_PAUSE,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventOnPause),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventOnPause),
      RefString("Event to send when the controller pauses"),
      GROUP));
   
   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_ON_UNPAUSE,
      PROPERTY_EVENT_ON_UNPAUSE,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventOnUnPause),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventOnUnPause),
      RefString("Event to send when the controller unpauses"),
      GROUP));

   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_ON_NEXT_STEP,
      PROPERTY_EVENT_ON_NEXT_STEP,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventOnNextStep),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventOnNextStep),
      RefString("Event to send when the controller performs next step"),
      GROUP));

   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_ON_RESTART,
      PROPERTY_EVENT_ON_RESTART,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventOnRestart),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventOnRestart),
      RefString("Event to send when the controller restarts"),
      GROUP));

   AddProperty(new GameEventActorProperty(*this,
      PROPERTY_EVENT_ON_END,
      PROPERTY_EVENT_ON_END,
      GameEventActorProperty::SetFuncType(this, &BezierControllerActorComponent::SetEventOnEnd),
      GameEventActorProperty::GetFuncType(this, &BezierControllerActorComponent::GetEventOnEnd),
      RefString("Event to send when the controller ends"),
      GROUP));
}

void BezierControllerActorComponent::SetEventToStart(dtCore::GameEvent* gameEvent)
{
   mEventToStart = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventToStart() const
{
   return mEventToStart.get();
}

void BezierControllerActorComponent::SetEventToPause(dtCore::GameEvent* gameEvent)
{
   mEventToPause = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventToPause() const
{
   return mEventToPause.get();
}

void BezierControllerActorComponent::SetEventToUnPause(dtCore::GameEvent* gameEvent)
{
   mEventToUnPause = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventToUnPause() const
{
   return mEventToUnPause.get();
}

void BezierControllerActorComponent::SetEventOnStart(dtCore::GameEvent* gameEvent)
{
   mEventOnStart = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventOnStart() const
{
   return mEventOnStart.get();
}

void BezierControllerActorComponent::SetEventOnPause(dtCore::GameEvent* gameEvent)
{
   mEventOnPause = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventOnPause() const
{
   return mEventOnPause.get();
}

void BezierControllerActorComponent::SetEventOnUnPause(dtCore::GameEvent* gameEvent)
{
   mEventOnUnPause = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventOnUnPause() const
{
   return mEventOnUnPause.get();
}

void BezierControllerActorComponent::SetEventOnRestart(dtCore::GameEvent* gameEvent)
{
   mEventOnRestart = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventOnRestart() const
{
   return mEventOnRestart.get();
}

void BezierControllerActorComponent::SetEventOnNextStep(dtCore::GameEvent* gameEvent)
{
   mEventOnNextStep = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventOnNextStep() const
{
   return mEventOnNextStep.get();
}

void BezierControllerActorComponent::SetEventOnEnd(dtCore::GameEvent* gameEvent)
{
   mEventOnEnd = gameEvent;
}

dtCore::GameEvent* BezierControllerActorComponent::GetEventOnEnd() const
{
   return mEventOnEnd.get();
}

void BezierControllerActorComponent::OnNextStep(dtABC::BezierController& controller)
{
   SendGameEvent(mEventOnNextStep.get());
}

void BezierControllerActorComponent::OnStart(dtABC::BezierController& controller)
{
   SendGameEvent(mEventOnStart.get());
}

void BezierControllerActorComponent::OnPause(dtABC::BezierController& controller)
{
   SendGameEvent(mEventOnPause.get());
}

void BezierControllerActorComponent::OnUnPause(dtABC::BezierController& controller)
{
   SendGameEvent(mEventOnUnPause.get());
}

void BezierControllerActorComponent::OnRestart(dtABC::BezierController& controller)
{
   SendGameEvent(mEventOnRestart.get());
}

void BezierControllerActorComponent::OnEnd(dtABC::BezierController& controller)
{
   SendGameEvent(mEventOnEnd.get());
}

void BezierControllerActorComponent::SendGameEvent(dtCore::GameEvent* gameEvent)
{
   if (gameEvent != NULL)
   {
      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      if (actor != NULL)
      {
         dtGame::GameManager* gm = actor->GetGameManager();

         if (gm != NULL)
         {
            dtCore::RefPtr<dtGame::GameEventMessage> message;
            gm->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, message);

            message->SetSendingActorId(actor->GetId());
            message->SetGameEvent(*gameEvent);

            gm->SendMessage(*message);
         }
      }
   }
}

void BezierControllerActorComponent::ProcessGameEvent(const dtGame::GameEventMessage& gameEventMessage)
{
   const dtCore::GameEvent* gameEvent = gameEventMessage.GetGameEvent();

   if (gameEvent != NULL)
   {
      dtABC::BezierController* bc = static_cast<dtABC::BezierController*>(GetDrawable());
      
      if (gameEvent == mEventToStart)
      {
         bc->Start();
      }
      else if(gameEvent == mEventToPause)
      {
         bc->Pause();
      }
      else if(gameEvent == mEventToUnPause)
      {
         bc->UnPause();
      }
   }
}


}//namespace dtActors


