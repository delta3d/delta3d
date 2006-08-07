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
 * @author Matthew W. Campbell
 */
#ifndef DELTA_COUNTERTASKACTOR
#define DELTA_COUNTERTASKACTOR

#include <dtActors/taskactor.h>
#include "export.h"

/**
 * This class is a simple subclass of the base task actor which is the crux of 
 * the task and objective tracking system in Delta3D.  The purpose of this task
 * actor is to simple track a count of something that occured in the system.
 * The testAAR example application uses this task to count the number of boxes
 * placed in the world by the player.
 */
class DT_EXAMPLE_EXPORT CounterTaskActor : public dtActors::TaskActor
{
   public:
   
      CounterTaskActor(dtGame::GameActorProxy &proxy);
      
   protected:
   
      virtual ~CounterTaskActor();
};   
 
/**
 * The game actor proxy for the counter task.  It has two properties, the counter 
 * which can be used to track the number of occurances that something happened,
 * and the target count which is used to determine completeness and final score.
 */
class DT_EXAMPLE_EXPORT CounterTaskActorProxy : public dtActors::TaskActorProxy
{
   public:
      
      /**
       * Constructs the proxy.  By default, the counter is initialized to zero
       * and the number of occurances is initialized to one.
       */
      CounterTaskActorProxy();
      
      /**
       * Creates the properties for the proxy.
       */
      void BuildPropertyMap();
      
      /**
       * Builds any invokables needed for this game actor proxy.
       */
      void BuildInvokables();   
      
      void SetCounter(int value);
      int GetCounter() const { return mCounter; }
      
      void SetTargetCount(int value);
      int GetTargetCount() const { return mTarget; }
      
   protected:
   
      virtual ~CounterTaskActorProxy();
      
   private:
      int mCounter;
      int mTarget;
};

#endif
