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
 * Matthew W. Campbell
 */
#ifndef DELTA_TASKACTORGAMEEVENT
#define DELTA_TASKACTORGAMEEVENT

#include <dtActors/taskactor.h>
#include <dtCore/gameevent.h>
#include <dtGame/message.h>

namespace dtGame
{
   class GameEventMessage;
}

namespace dtActors
{
   /**
    * Game event tasks are tasks that are responsible for tracking game events.
    * Game events are simple atomic actions such as "Apple Found" or "Hostage Rescued".
    * This task actor is responsible for tracking and listening for one event.  When
    * that event is fired, the task attempts to mark itself complete.
    * @see dtCore::GameEvent
    */
   class DT_PLUGIN_EXPORT TaskActorGameEvent : public TaskActor
   {
      public:

         /**
          * Constructs the task game event actor.
          * @param proxy Proxy owning this actor.
          */
         TaskActorGameEvent(dtGame::GameActorProxy& parent);

         /**
          * Sets this task back to its default state.
          */
         virtual void Reset();

         /**
          * Sets the game event being tracked by this task.
          * @param event The game event to track.
          */
         void SetGameEvent(dtCore::GameEvent* event) { mGameEvent = event; }

         /**
          * Gets the game event currently tracked by this task.
          * @return The game event or NULL if this task is not currenlty responsible
          *   for an event.
          */
         dtCore::GameEvent* GetGameEvent() { return mGameEvent.get(); }

         /**
          * Gets the game event currently tracked by this task.
          * @return The game event or NULL if this task is not currenlty responsible
          *   for an event.
          */
         const dtCore::GameEvent* GetGameEvent() const { return mGameEvent.get(); }

         /**
          * Set an alternative event that can cause this task to fail.
          * @param gameEvent Event that causes this task to fail.
          */
         void SetFailGameEvent(dtCore::GameEvent* gameEvent) { mGameEventFail = gameEvent; }

         /**
          * @return Game event that will cause this task to fail; NULL if no event assigned.
          */
         dtCore::GameEvent* GetFailGameEvent() { return mGameEventFail.get(); }
         const dtCore::GameEvent* GetFailGameEvent() const { return mGameEventFail.get(); }

         /**
          * Sets the number of times the game event tracked by this task must be
          * fired before the task should be considered complete.
          * @param times The number of times.
          * @note By default, this value is set to 1.
          */
         void SetMinOccurances(int times) { mMinOccurances = times; }

         /**
          * Gets the number of times the game event tracked by this task must be
          * fired before the task should be considered complete.
          * @return The number of times.
          */
         int GetMinOccurances() const { return mMinOccurances; }

         /**
          * Called when the game manager receives an INFO_GAME_EVENT message.
          * This method checks the message for the game event.  If the game
          * event matches the game event currently tracked by this task,
          * the number of times fired is incremented.  If the number of times
          * fired is equal to the min occurances property, the task will
          * attempt to set itself complete.
          * @param eventMsg The message contained the game event.
          */
         void HandleGameEvent(const dtGame::GameEventMessage& eventMsg);

      protected:

         /**
          * Destroys this task actor.
          */
         virtual ~TaskActorGameEvent();

      private:
         dtCore::ObserverPtr<dtCore::GameEvent> mGameEvent;
         dtCore::ObserverPtr<dtCore::GameEvent> mGameEventFail;
         int mMinOccurances;
         int mNumTimesEventFired;
   };

   /**
    * Proxy for the game event task actor.  It contains properties to track the
    * number of occurances of an event as well as determine how many times a game
    * event is listened for before completeness can occur.
    */
   class DT_PLUGIN_EXPORT TaskActorGameEventProxy : public TaskActorProxy
   {
      public:
         static const dtUtil::RefString CLASS_NAME;
         static const dtUtil::RefString PROPERTY_EVENT_COMPLETE;
         static const dtUtil::RefString PROPERTY_EVENT_FAIL;
         static const dtUtil::RefString PROPERTY_MIN_OCCURANCES;

         /**
          * Constructs the task game event proxy.
          */
         TaskActorGameEventProxy();

         /**
          * Adds the properties custom to this task actor in addition to calling
          * the base task actor's build property map method.
          */
         virtual void BuildPropertyMap();

         /**
          * Builds the invokables necessary to trap game event messages coming from
          * the game manager.
          */
         virtual void BuildInvokables();

      protected:

         /**
          * Destroys the proxy.
          */
         virtual ~TaskActorGameEventProxy();

         /**
          * Create the underlying game event task actor to be managed by this proxy.
          */
         virtual void CreateDrawable();

         /**
          * Called just after the proxy has entered the world or added to the game manager.
          */
         virtual void OnEnteredWorld();
   };
}

#endif
