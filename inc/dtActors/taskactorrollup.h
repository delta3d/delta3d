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
#ifndef DELTA_TASKACTORROLLUP
#define DELTA_TASKACTORROLLUP

#include <dtActors/taskactor.h>

namespace dtActors
{
   /**
    * Where as all tasks can contain children, a rollup task actually provides functional
    * behavior centered around that property.  A rollup task contains child tasks that must
    * be completed before the rollup task itself can be considered complete.  The task's score
    * is a summation of each of its child tasks making it more or less reflect the cummulative
    * results of its child tasks.
    */
   class DT_PLUGIN_EXPORT TaskActorRollup : public TaskActor
   {
      public:

         /**
          * Constructs the rollup task actor.
          * @param proxy Proxy owning this actor.
          */
         TaskActorRollup(dtGame::GameActorProxy& parent);

      protected:

         /**
          * Destroys this task actor.
          */
         virtual ~TaskActorRollup();
   };

   /**
    * Proxy for the game event task actor.  It contains properties to track the
    * number of occurances of an event as well as determine how many times a game
    * event is listened for before completeness can occur.
    */
   class DT_PLUGIN_EXPORT TaskActorRollupProxy : public TaskActorProxy
   {
      public:

         /**
          * Constructs the rollup task actor proxy.
          */
         TaskActorRollupProxy();

         /**
          * Updates this tasks total score tracking based on the newly changed child task.
          * @param childTask The child task that issued the score changed notify.
          */
         virtual void NotifyScoreChanged(const TaskActorProxy &childTask);

      protected:

         /**
          * Destroys the proxy.
          */
         virtual ~TaskActorRollupProxy();

         /**
          * Create the underlying rollup task actor to be managed by this proxy.
          */
         virtual void CreateDrawable();
   };
}

#endif
