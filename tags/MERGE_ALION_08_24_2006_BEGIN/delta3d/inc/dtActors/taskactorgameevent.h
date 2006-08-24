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
 * 59 Temple Place, Suite 330, Boston, MA 021w11-1307 USA
 *
 * @author Matthew W. Campbelwl
 */
#ifndef DELTA_TASKACTORGAMEEVENT
#define DELTA_TASKACTORGAMEEVENT

#include "dtActors/taskactor.h"
#include <dtDAL/gameevent.h>
#include <dtGame/message.h>

namespace dtActors
{
   /**
    * Game event tasks are tasks that are responsible for tracking game events.
    * Game events are simple atomic actions such as "Apple Found" or "Hostage Rescued".
    * This task actor is responcible for tracking and listening for one event.  When
    * that event is fired, the task attempts to mark itself complete.
    * @see dtDAL::GameEvent
    */
   class DT_PLUGIN_EXPORT TaskActorGameEvent : public TaskActor
   {
      public:

         /**
          * Constructs the task game event actor.
          * @param proxy Proxy owning this actor.
          */
         TaskActorGameEvent(dtGame::GameActorProxy &proxy);

         /**
          * Sets this task back to its default state.
          */
         virtual void Reset();

         /**
          * Sets the game event being tracked by this task.
          * @param event The game event to track.
          */
         void SetGameEvent(dtDAL::GameEvent *event) { mGameEvent = event; }

         /**
          * Gets the game event currently tracked by this task.
          * @return The game event or NULL if this task is not currenlty responsible
          *   for an event.
          */
         dtDAL::GameEvent *GetGameEvent() const { return mGameEvent; }

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
          * @param msg The message contained the game event.
          */
         void HandleGameEvent(const dtGame::Message &msg);

      protected:

         /**
          * Destroys this task actor.
          */
         virtual ~TaskActorGameEvent();

      private:
         dtDAL::GameEvent *mGameEvent;
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
         virtual void CreateActor();

         /**
          * Called just after the proxy has entered the world or added to the game manager.
          */
         virtual void OnEnteredWorld();
   };
}

#endif
