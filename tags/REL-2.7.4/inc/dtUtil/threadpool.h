/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <osg/Referenced>
#include <OpenThreads/Block>
#include <dtUtil/export.h>
#include <dtUtil/getsetmacros.h>
#include <dtUtil/refstring.h>

namespace dtUtil
{
   class DT_UTIL_EXPORT ThreadPoolTask : public osg::Referenced
   {
   public:
      ThreadPoolTask();

      /// Implement this method to actually do the work for the task
      virtual void operator()() = 0;

      /// the task name
      DT_DECLARE_ACCESSOR(dtUtil::RefString, Name);

      /// Whether this operation should go the back of the queue when it's done.
      DT_DECLARE_ACCESSOR(bool, Keep);

      /// Called by the threadpool, this will reset the wait until complete block.
      void ResetWaitBlock();
      /// Called by the threadpool, this will release the wait until complete block.
      void ReleaseWaitBlock();

      /// Will block the current thread until this task completes.
      bool WaitUntilComplete(int timeoutMS = -1);

   protected:
      virtual ~ThreadPoolTask();
   private:
      OpenThreads::Block mBlockUntilComplete;
   };

   /**
    * <p>
    * The Thread Pool exists to create worker threads to allow more efficient and easy to use threading of programming
    * tasks.  The basic use case is to initialize this with the default settings so that it will create one thread
    * for each CPU core minus 1 because 1 main execution thread already exists.
    * </p>
    * <p>
    * Then for large tasks, specifically ones with lots of data, you create tasks. You generally want break up the work
    * load into enough tasks so that you will have work for every CPU core to do.  If you break it up into enough tasks,
    * then as machines get more and more cores, the application will automatically scale up.
    * Once the tasks have been added, then call ExecuteTasks() on the calling thread so that it, too, will work on
    * the tasks so as to give the maximum performance.
    * </p>
    *
    * E.g. <br>
    * for (...) <br>
    *    ThreadPool::AddTask(*new CustomTask(dataBlocks[i])); <br>
    * <br>
    * ThreadPool::ExecuteTasks(); <br>
    * <br>
    *
    * <p>
    * Sometimes it makes sense to just run something in the background while other things run on the main cpu, such
    * I.O. intensive tasks that will block.
    * </p>
    * <p>
    * In this case, just call
    * </p>
    * <p>
    *  ThreadPool::AddTask(*new CustomTask(), ThreadPool::BACKGROUND);
    * </p>
    *
    * <p>
    * Background tasks will run whenever threads have no IMMEDIATE tasks to complete.  Also, if you init the thread
    * pool on a single core box or request 0 threads, then there will still be a  thread just for doing background tasks
    * so that things like IO specific tasks will still run in the background and not block the main thread.
    * </p>
    */
   class DT_UTIL_EXPORT ThreadPool
   {
   public:

      enum PoolQueue
      {
         IMMEDIATE, ///< Should run immediately
         BACKGROUND, ///< Runs as lower priority and only when IMMEDIATE tasks are not available.
         IO ///< Runs as lower priority and on an additional thread because these tasks block but aren't cpu intensive.
      };

      static bool IsInitialized();

      /**
       * Initializes the thread pool and starts the worker threads.
       * @param numThreads the number of threads to create, don't set it and this will create a thread per core - 1.
       */
      static void Init(int numThreads = -1);
      static void Shutdown();



      /**
       * Adds a task for the worker threads to execute.
       * @param task the task to execute
       * @param queue the queue to put the task in.
       */
      static void AddTask(ThreadPoolTask& task, PoolQueue queue = IMMEDIATE);

      /**
       * After adding tasks to the pool, you then call Execute tasks and it will run the set of tasks
       * on both this thread and the additional worker threads until they all complete.
       * Only IMMEDIATE tasks are affected by this method.
       */
      static void ExecuteTasks();

      /**
       * @return the number of worker threads that can work on immediate tasks including the main thread.
       */
      static unsigned GetNumImmediateWorkerThreads();

   private:
      // Hide all constructors and destructors
      ThreadPool();
      ThreadPool(ThreadPool&);
      ~ThreadPool();
      ThreadPool& operator=(ThreadPool&);
   };

}

#endif /* THREADPOOL_H_ */
