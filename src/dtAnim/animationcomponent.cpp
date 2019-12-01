/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 05/11/2007
 */

#include <dtAnim/animationcomponent.h>
#include <dtCore/gameevent.h>
#include <dtCore/gameeventmanager.h>
#include <dtGame/basemessages.h>
#include <dtGame/defaultgroundclamper.h>
#include <dtGame/messagetype.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/gameactor.h>
#include <dtUtil/functor.h>
#include <dtUtil/log.h>

namespace dtAnim
{

/////////////////////////////////////////////////////////////////////////////////
AnimationComponent::AnimationComponent(dtCore::SystemComponentType& type)
: BaseClass(type)
, mGroundClamper(new dtGame::DefaultGroundClamper)
{
   mGroundClamper->SetHighResGroundClampingRange(0.01);
   mGroundClamper->SetLowResGroundClampingRange(0.1);
}

/////////////////////////////////////////////////////////////////////////////////
AnimationComponent::~AnimationComponent()
{
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::ProcessMessage(const dtGame::Message& message)
{
   BaseClass::ProcessMessage(message);

   if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
   {
      if (GetTerrainActor() != NULL && GetTerrainActor()->GetUniqueId() == message.GetAboutActorId())
      {
         SetTerrainActor(NULL);
      }
   }
   else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOADED)
   {
      SetTerrainActor(NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::TickLocal(float dt)
{
   BaseClass::TickLocal(dt);

   if (mGroundClamper->GetTerrainActor() != NULL)
   {
      ForEachActorComponent(dtUtil::MakeFunctor(&AnimationComponent::GroundClamp, this));
   }

   ForEachActorComponent(dtUtil::MakeFunctor(&AnimationComponent::ExecuteCommands, this));
}

/////////////////////////////////////////////////////////////////////////////////
bool AnimationComponent::RegisterActor(dtGame::GameActorProxy& actor, dtAnim::AnimationHelper& helper)
{
   bool result = BaseClass::RegisterActor(actor, helper);
   if (result)
   {
      // Register the event firing callbacks that the helper will call
      // when any animatable reaches a particular state.
      AnimEventCallback callback(this, &AnimationComponent::OnAnimationEvent);
      helper.SetSendEventCallback(callback);
   }
   return result;
}

/////////////////////////////////////////////////////////////////////////////////
bool AnimationComponent::UnregisterActor(const dtCore::UniqueId& actorId)
{
   AnimationHelper* actorComp = GetComponentForActor(actorId);
   if (actorComp != NULL)
   {
      actorComp->SetSendEventCallback(AnimEventCallback());
   }
   return BaseClass::UnregisterActor(actorId);
}


/////////////////////////////////////////////////////////////////////////////////
dtCore::Transformable* AnimationComponent::GetTerrainActor()
{
   return mGroundClamper->GetTerrainActor();
}

/////////////////////////////////////////////////////////////////////////////////
const dtCore::Transformable* AnimationComponent::GetTerrainActor() const
{
   return mGroundClamper->GetTerrainActor();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::SetTerrainActor(dtCore::Transformable* newTerrain)
{
   mGroundClamper->SetTerrainActor(newTerrain);
}

//////////////////////////////////////////////////////////////////////
dtCore::Transformable* AnimationComponent::GetEyePointActor()
{
   return mGroundClamper->GetEyePointActor();
}

//////////////////////////////////////////////////////////////////////
const dtCore::Transformable* AnimationComponent::GetEyePointActor() const
{
   return mGroundClamper->GetEyePointActor();
}

//////////////////////////////////////////////////////////////////////
void AnimationComponent::SetEyePointActor(dtCore::Transformable* newEyePointActor)
{
   mGroundClamper->SetEyePointActor(newEyePointActor);
}

//////////////////////////////////////////////////////////////////////
void AnimationComponent::SetGroundClamper(dtGame::BaseGroundClamper& clamper)
{
   mGroundClamper = &clamper;
}

//////////////////////////////////////////////////////////////////////
dtGame::BaseGroundClamper& AnimationComponent::GetGroundClamper()
{
   return *mGroundClamper;
}

//////////////////////////////////////////////////////////////////////
const dtGame::BaseGroundClamper& AnimationComponent::GetGroundClamper() const
{
   return *mGroundClamper;
}


/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::ExecuteCommands(BaseClass::ActorCompMapping& item)
{
   // Track the current actor that may be sending any game events.
   //mCurrentSendingActorId = item.first;

   // Get the associated helper and execute any of its commands that
   // it gathered for the current frame. Some of the commands may
   // fire game events and subsequently call OnAnimationEvent to fire
   // a Game Event Message.
   dtAnim::AnimationHelper* curHelper = item.second.mActorComp.get();
   curHelper->ExecuteCommands();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::GroundClamp(BaseClass::ActorCompMapping& item)
{
   dtGame::GameManager* gm = GetGameManager();

   dtGame::GroundClampingData gcData;
   gcData.SetAdjustRotationToGround(false);
   gcData.SetUseModelDimensions(false);

   dtGame::GameActorProxy* pProxy = gm->FindGameActorById(item.first);
   if (pProxy != NULL)
   {
      dtCore::Transform xform;
      pProxy->GetDrawable<dtCore::Transformable>()->GetTransform(xform, dtCore::Transformable::REL_CS);

      mGroundClamper->ClampToGround(dtGame::BaseGroundClamper::GroundClampRangeType::RANGED,
               0.0, xform, *pProxy, gcData, true);
   } // if
}

////////////////////////////////////////////////////////////////////////////////
void AnimationComponent::OnAnimationEvent(const std::string& eventName)
{
   dtGame::GameManager* gm = GetGameManager();
   if (gm != NULL)
   {
      // Find any current reference to the specified event name.
      dtCore::GameEvent* gameEvent = dtCore::GameEventManager::GetInstance().FindEvent(eventName);

      // If the event was not found, log a warning and create it to ensure
      // an event is still fired anyway.
      if (gameEvent == NULL)
      {
         // Log the warning first in case anything goes wrong with creating
         // and adding the new event (though nothing wrong should happen).
         std::ostringstream oss;
         oss << "Animation game event \"" << eventName
            << "\" was not found, but will now be created and sent.\n";
         LOG_WARNING(oss.str());

         // Create and add the new event.
         gameEvent = new dtCore::GameEvent(eventName);
         dtCore::GameEventManager::GetInstance().AddEvent(*gameEvent);
      }

      // DEBUG:
      //std::cout << "\n\tSending Anim Event: " << eventName << "\n\n";

      // Create the event message and send it.
      dtCore::RefPtr<dtGame::GameEventMessage> message;
      gm->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT, message);
      message->SetSendingActorId(mCurrentSendingActorId);
      message->SetGameEvent(*gameEvent);
      gm->SendMessage(*message);
   }
}

} // namespace dtGame
