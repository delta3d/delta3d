/* -*-c++-*-
* testGameActorLibrary - countertaskactor (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Matthew W. Campbell
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
   
      CounterTaskActor(dtGame::GameActorProxy& parent);
      
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
