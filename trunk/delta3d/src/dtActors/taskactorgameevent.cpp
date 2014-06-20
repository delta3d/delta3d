/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Matthew W. Campbell, Curtiss Murphy
 */

#include <dtActors/taskactorgameevent.h>

#include <dtCore/functor.h>
#include <dtCore/gameeventactorproperty.h>
#include <dtCore/intactorproperty.h>

#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActorGameEvent::TaskActorGameEvent(dtGame::GameActorProxy& parent) : TaskActor(parent), mGameEvent(NULL)
   {
      Reset();
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorGameEvent::~TaskActorGameEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEvent::Reset()
   {
      TaskActor::Reset();
      mMinOccurances = 1;
      mNumTimesEventFired = 0;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEvent::HandleGameEvent(const dtGame::GameEventMessage& eventMsg)
   {

      if (eventMsg.GetGameEvent() == NULL)
      {
         LOG_WARNING("HandleGameEvent: Game event message contained a NULL game event.");
         return;
      }

      if ( ! mGameEvent.valid() && ! mGameEventFail.valid() )
      {
         LOG_WARNING("HandleGameEvent: Game event task actor has NULL game events.  Perhaps it was "
                    " assigned to the task before being added to the event manager.");
         return;
      }

      // Determine which event has been received.
      const dtCore::UniqueId& eventId = eventMsg.GetGameEvent()->GetUniqueId();
      bool isFailEvent = false;
      if ( mGameEvent.valid() && eventId == mGameEvent->GetUniqueId() )
      {
         // DO NOTHING.
      }
      else if ( mGameEventFail.valid() && eventId == mGameEventFail->GetUniqueId() )
      {
         isFailEvent = true;
      }
      else
      {
         // Not an event to act upon.
         return;
      }

      //If we got here we have a game event we were looking for.  So, all we need to do
      //is track the number of times we got the event, and if it reaches the min occurances
      //attempt to mark ourselves complete.
      TaskActorProxy& owner = static_cast<TaskActorProxy&>(GetGameActorProxy());
      if (!IsComplete() && !IsFailed())
      {
         if (owner.RequestScoreChange(owner,owner))
         {
            if ( isFailEvent ) // Fail event
            {
               SetFailed( true );
            }
            else // Complete Event
            {
               mNumTimesEventFired++;
               float newScore = (float)mNumTimesEventFired / (float)mMinOccurances;
               if (newScore >= GetPassingScore())
                  SetComplete(true);

               SetScore(newScore);
            }

            // Notify the system that the task state has changed.
            owner.NotifyScoreChanged(owner);
            owner.NotifyFullActorUpdate();
         }
      }
   }

   ////////////////////////////END ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString TaskActorGameEventProxy::CLASS_NAME("dtActors::GameEventTaskActor");
   const dtUtil::RefString TaskActorGameEventProxy::PROPERTY_EVENT_COMPLETE("GameEvent");
   const dtUtil::RefString TaskActorGameEventProxy::PROPERTY_EVENT_FAIL("FailGameEvent");
   const dtUtil::RefString TaskActorGameEventProxy::PROPERTY_MIN_OCCURANCES("MinOccurances");

   //////////////////////////////////////////////////////////////////////////////
   TaskActorGameEventProxy::TaskActorGameEventProxy()
   {
      SetClassName(TaskActorGameEventProxy::CLASS_NAME);
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorGameEventProxy::~TaskActorGameEventProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEventProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "GameEvent";

      TaskActorProxy::BuildPropertyMap();
      TaskActorGameEvent* task = GetDrawable<TaskActorGameEvent>();

      // Game event property.
      AddProperty(new dtCore::GameEventActorProperty(*this,
         TaskActorGameEventProxy::PROPERTY_EVENT_COMPLETE,
         "Game Event",
         dtCore::GameEventActorProperty::SetFuncType(task, &TaskActorGameEvent::SetGameEvent),
         dtUtil::MakeFunctor<dtCore::GameEvent* (TaskActorGameEvent::*)(), TaskActorGameEvent>
            (&TaskActorGameEvent::GetGameEvent, task),
         "Sets and gets the game event being tracked by the task.",GROUPNAME));

      // Fail Game Event
      AddProperty(new dtCore::GameEventActorProperty(*this,
         TaskActorGameEventProxy::PROPERTY_EVENT_FAIL,
         "Fail Game Event",
         dtCore::GameEventActorProperty::SetFuncType(task, &TaskActorGameEvent::SetFailGameEvent),
         dtUtil::MakeFunctor<dtCore::GameEvent* (TaskActorGameEvent::*)(), TaskActorGameEvent>
            (&TaskActorGameEvent::GetFailGameEvent, task),
         "Sets and gets the game event that will cause this task to fail.",GROUPNAME));

      // Min Occurances...
      AddProperty(new dtCore::IntActorProperty(
         TaskActorGameEventProxy::PROPERTY_MIN_OCCURANCES,
         "Minimum Occurances",
         dtCore::IntActorProperty::SetFuncType(task,&TaskActorGameEvent::SetMinOccurances),
         dtCore::IntActorProperty::GetFuncType(task,&TaskActorGameEvent::GetMinOccurances),
         "Sets/gets the minimum number of times the game event must be fired before "
            "this task is considered complete.",GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEventProxy::BuildInvokables()
   {
      TaskActorGameEvent* task = GetDrawable<TaskActorGameEvent>();
      TaskActorProxy::BuildInvokables();
      AddInvokable(*new dtGame::Invokable("HandleGameEvent",
         dtUtil::MakeFunctor(&TaskActorGameEvent::HandleGameEvent, task)));
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEventProxy::CreateDrawable()
   {
      SetDrawable(*new TaskActorGameEvent(*this));
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEventProxy::OnEnteredWorld()
   {
      TaskActorProxy::OnEnteredWorld();

      if (!IsRemote())
      {
         GetGameManager()->RegisterForMessages(dtGame::MessageType::INFO_GAME_EVENT,
            *this,"HandleGameEvent");
      }
   }

}
