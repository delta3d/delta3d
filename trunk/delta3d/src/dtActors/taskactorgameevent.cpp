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
#include <dtDAL/enginepropertytypes.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtDAL/functor.h>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActorGameEvent::TaskActorGameEvent(dtGame::GameActorProxy &proxy) : TaskActor(proxy), mGameEvent(NULL)
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
   void TaskActorGameEvent::HandleGameEvent(const dtGame::Message &msg)
   {
      const dtGame::GameEventMessage &eventMsg = static_cast<const dtGame::GameEventMessage&>(msg);

      if(eventMsg.GetGameEvent() == NULL)
      {
         LOG_WARNING("HandleGameEvent: Game event message contained a NULL game event.");
         return;
      }

      if( ! mGameEvent.valid() && ! mGameEventFail.valid() )
      {
         LOG_WARNING("HandleGameEvent: Game event task actor has NULL game events.  Perhaps it was "
                    " assigned to the task before being added to the event manager.");
         return;
      }

      // Determine which event has been received.
      const dtCore::UniqueId& eventId = eventMsg.GetGameEvent()->GetUniqueId();
      bool isFailEvent = false;
      if( mGameEvent.valid() && eventId == mGameEvent->GetUniqueId() )
      {
         // DO NOTHING.
      }
      else if( mGameEventFail.valid() && eventId == mGameEventFail->GetUniqueId() )
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
      TaskActorProxy &proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());
      if(!IsComplete() && !IsFailed())
      {
         if(proxy.RequestScoreChange(proxy,proxy))
         {
            if( isFailEvent ) // Fail event
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
            proxy.NotifyScoreChanged(proxy);
            proxy.NotifyFullActorUpdate();
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
      TaskActorGameEvent &task = static_cast<TaskActorGameEvent&>(GetGameActor());

      // Game event property.
      AddProperty(new dtDAL::GameEventActorProperty(*this, 
         TaskActorGameEventProxy::PROPERTY_EVENT_COMPLETE,
         "Game Event",
         dtDAL::MakeFunctor(task, &TaskActorGameEvent::SetGameEvent),
         dtDAL::MakeFunctorRet<dtDAL::GameEvent* (TaskActorGameEvent::*)(), TaskActorGameEvent>
            (task, &TaskActorGameEvent::GetGameEvent),
         "Sets and gets the game event being tracked by the task.",GROUPNAME));

      // Fail Game Event
      AddProperty(new dtDAL::GameEventActorProperty(*this, 
         TaskActorGameEventProxy::PROPERTY_EVENT_FAIL,
         "Fail Game Event",
         dtDAL::MakeFunctor(task, &TaskActorGameEvent::SetFailGameEvent),
         dtDAL::MakeFunctorRet<dtDAL::GameEvent* (TaskActorGameEvent::*)(), TaskActorGameEvent>
            (task, &TaskActorGameEvent::GetFailGameEvent),
         "Sets and gets the game event that will cause this task to fail.",GROUPNAME));

      // Min Occurances...
      AddProperty(new dtDAL::IntActorProperty(
         TaskActorGameEventProxy::PROPERTY_MIN_OCCURANCES,
         "Minimum Occurances",
         dtDAL::MakeFunctor(task,&TaskActorGameEvent::SetMinOccurances),
         dtDAL::MakeFunctorRet(task,&TaskActorGameEvent::GetMinOccurances),
         "Sets/gets the minimum number of times the game event must be fired before "
            "this task is considered complete.",GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEventProxy::BuildInvokables()
   {
      TaskActorGameEvent &task = static_cast<TaskActorGameEvent&>(GetGameActor());
      TaskActorProxy::BuildInvokables();
      AddInvokable(*new dtGame::Invokable("HandleGameEvent",
         dtDAL::MakeFunctor(task,&TaskActorGameEvent::HandleGameEvent)));
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorGameEventProxy::CreateActor()
   {
      SetActor(*new TaskActorGameEvent(*this));
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
