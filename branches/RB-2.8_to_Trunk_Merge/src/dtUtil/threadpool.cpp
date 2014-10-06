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

#include <prefix/dtutilprefix.h>
#include <dtUtil/threadpool.h>
#include <dtUtil/log.h>

#include <dtUtil/mswinmacros.h>
#include <dtUtil/mathdefines.h>

#include <OpenThreads/Thread>
#include <OpenThreads/Atomic>
#include <OpenThreads/Block>
#include <OpenThreads/Mutex>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <climits>

namespace dtUtil
{

   class TaskThread;

template<class _Ty,
   class _Container = std::vector<_Ty>,
   class _Pr = std::less<typename _Container::value_type> >
   class checked_priority_queue : public std::priority_queue<_Ty, _Container, _Pr>
   {
   public:
      typedef std::priority_queue<_Ty, _Container, _Pr> BaseClass;
      bool checkme()
      {
         for (unsigned i = 1; i < BaseClass::c.size(); ++i)
         {
            // c[0] should have the highest priority.
            if (BaseClass::c[0] < BaseClass::c[i])
               return false;
         }
         return true;
      }
   };


   class DT_UTIL_EXPORT TaskQueue : public osg::Referenced
   {
   public:

      static const unsigned MAX_QUEUE_ID = 15;

      TaskQueue();

      /** Return true if the operation queue is empty. */
      bool Empty() const { return mTasks.empty(); }

      /** Return the num of pending tasks that are sitting in the TaskQueue.*/
      unsigned int GetNumTasksInQueue() const { return unsigned(mTasks.size()); }

      /** Add a task to end of TaskQueue, this will be
      * executed by the task thread once this operation gets to the head of the queue.*/
      void Add(ThreadPoolTask& task, unsigned queueId);

      //           /** Remove task from TaskQueue.*/
      //           void Remove(ThreadPoolTask& task);
      //
      //           /** Remove named task from TaskQueue.*/
      //           void Remove(const dtUtil::RefString& name);

      /** Remove all tasks from TaskQueue.*/
      void RemoveAllTasks();

      /** Run all the tasks with the given priority. and optionally wait until all threads complete their tasks for this queue as well.
      */
      void ExecuteTasks(bool waitForAllTasksToBeCompleted = true, unsigned maxQueueId = 0);

      /**
      * Run one task
      * @param blockIfEmpty if the queue is empty at the start, then block until a task is queued or the block
      *                     is otherwise released.
      * @param maxQueueId execute only tasks with a queue id less than equal to the one passed it.
      * @return true if a task was executed.
      */
      bool ExecuteSingleTask(bool blockIfEmpty = true, unsigned maxQueueId = INT_MAX);

      /** Release tasks block that is used to block threads that are waiting on an empty tasks queue.*/
      void ReleaseTasksBlock();

      typedef std::set<TaskThread*> TaskThreads;

      /** Get the set of TaskThreads that are sharing this TaskQueue. */
      const TaskThreads& getTaskThreads() const { return mTaskThreads; }

   protected:

      virtual ~TaskQueue();

      friend class TaskThread;

      void AddTaskThread(TaskThread* thread);
      void RemoveTaskThread(TaskThread* thread);

      struct TaskQueueItem
      {
         dtCore::RefPtr<ThreadPoolTask> mTask;
         unsigned mQueueId;
         // We want low to high, not high to low.
         bool operator < (const TaskQueueItem& item) const { return mQueueId > item.mQueueId; }
      };

      typedef checked_priority_queue<TaskQueueItem> Tasks;

      OpenThreads::Mutex     mTasksMutex;
      OpenThreads::Block     mTasksBlock;
      Tasks                  mTasks;

      TaskThreads            mTaskThreads;
      OpenThreads::Atomic    mInProcessTasks[MAX_QUEUE_ID + 1U];
   };

   TaskQueue::TaskQueue():
       osg::Referenced(true)
   {
   }

   TaskQueue::~TaskQueue()
   {
   }

   void TaskQueue::Add(ThreadPoolTask& task, unsigned queueId)
   {
      dtUtil::Clamp(queueId, 0U, MAX_QUEUE_ID);

      // acquire the lock on the operations queue to prevent anyone else from modifying it at the same time
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

      TaskQueueItem newItem;
      newItem.mTask = &task;
      newItem.mQueueId = queueId;
      // add the operation to the end of the list
      mTasks.push(newItem);

      ++mInProcessTasks[queueId];

      mTasksBlock.release();
   }

//   void TaskQueue::Remove(ThreadPoolTask& task)
//   {
//       // acquire the lock on the operations queue to prevent anyone else for modifying it at the same time
//       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);
//
//       for(Tasks::iterator itr = mTasks.begin();
//           itr!=mTasks.end();)
//       {
//           if ((*itr)==&task)
//           {
//               bool needToResetCurrentIterator = (mCurrentTaskIterator == itr);
//
//               itr = mTasks.erase(itr);
//
//               if (needToResetCurrentIterator) mCurrentTaskIterator = itr;
//
//           }
//           else ++itr;
//       }
//
//       if (Empty())
//       {
//           mTasksBlock.reset();
//       }
//   }

//   void TaskQueue::Remove(const dtUtil::RefString& name)
//   {
//       // acquire the lock on the operations queue to prevent anyone else for modifying it at the same time
//       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);
//
//       // find the remove all operations with specified name
//       for(Tasks::iterator itr = mTasks.begin();
//           itr!=mTasks.end();)
//       {
//           if ((*itr)->GetName() == name)
//           {
//               bool needToResetCurrentIterator = (mCurrentTaskIterator == itr);
//
//               itr = mTasks.erase(itr);
//
//               if (needToResetCurrentIterator) mCurrentTaskIterator = itr;
//           }
//           else ++itr;
//       }
//
//       if (Empty())
//       {
//           mTasksBlock.reset();
//       }
//   }

   void TaskQueue::RemoveAllTasks()
   {
       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

       while (!Empty())
       {
          mTasks.pop();
       }

        mTasksBlock.reset();
   }

   bool TaskQueue::ExecuteSingleTask(bool blockIfEmpty, unsigned maxQueueId)
   {
      dtUtil::Clamp(maxQueueId, 0U, MAX_QUEUE_ID);

      bool wasEmpty = false;

      dtCore::RefPtr<ThreadPoolTask> currentTask = NULL;
      unsigned queueId = 0;
      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

         if (Empty())
         {
            wasEmpty = true;
         }
         else
         {
            //if (!mTasks.checkme())
            //{
            //   LOG_ERROR("Task Queue was not in the correct order");
            //}

            queueId = mTasks.top().mQueueId;

            if (queueId > maxQueueId)
            {
               return false;
            }

            currentTask = mTasks.top().mTask;

            mTasks.pop();

            if (Empty())
            {
               mTasksBlock.reset();
            }
         }
      }

      // if the code above decided that the queue was empty when inside the lock
      // and the caller specified that it should block...
      if (wasEmpty)
      {
         if (blockIfEmpty && mTasksBlock.block(1000))
         {
            // if the block was released without a timeout, execute again, but with no blocking
            // The reason for no blocking is that we don't want to keep re-blocking if we don't
            // get a task, that would be bad.
            return ExecuteSingleTask(false, maxQueueId);
         }
         else
         {
            return false;
         }
      }
      else
      {
         /// execute
         (*currentTask)();

         if (currentTask->GetKeep())
         {
            // re-add the task before decrementing the in process count so that code won't think all tasks are done
            Add(*currentTask, queueId);
         }
         else
         {
            currentTask->ReleaseWaitBlock();
         }

         --mInProcessTasks[queueId];
      }

      return true;
   }

   void TaskQueue::ExecuteTasks(bool waitForAllTasksToBeComplete, unsigned maxQueueId)
   {
      dtUtil::Clamp(maxQueueId, 0U, MAX_QUEUE_ID);

      unsigned tasksInProcess = 0;

      do
      {
         tasksInProcess = 0;
         if (waitForAllTasksToBeComplete)
         {
            {
               for (unsigned i = 0; i <= maxQueueId; ++i)
               {
                  tasksInProcess += unsigned(mInProcessTasks[i]);
               }
            }

            if (tasksInProcess > 0)
            {
               OpenThreads::Thread::YieldCurrentThread();
            }
         }
      }
      while (ExecuteSingleTask(false, maxQueueId) || tasksInProcess > 0);
   }

   void TaskQueue::ReleaseTasksBlock()
   {
       mTasksBlock.release();
   }

   void TaskQueue::AddTaskThread(TaskThread* thread)
   {
       mTaskThreads.insert(thread);
   }

   void TaskQueue::RemoveTaskThread(TaskThread* thread)
   {
      mTaskThreads.erase(thread);
   }

   class  TaskThread : public osg::Referenced, public OpenThreads::Thread
   {
   public:
      TaskThread(TaskQueue& queue);

      /** Run does the operation thread run loop.*/
      virtual void run();

      /** Cancel this thread.*/
      virtual int cancel();

   protected:

      virtual ~TaskThread();

      OpenThreads::Mutex         mThreadMutex;
      dtCore::RefPtr<TaskQueue>  mTaskQueue;
      volatile bool mDone;
   };

   TaskThread::TaskThread(TaskQueue& queue)
   : osg::Referenced(true)
   , mTaskQueue(&queue)
   , mDone(false)
   {
   }

   TaskThread::~TaskThread()
   {
      cancel();
   }

   int TaskThread::cancel()
   {
      int result = 0;
      if (isRunning())
      {
         //result = OpenThreads::Thread::cancel();
         mDone = true;
         mTaskQueue->ReleaseTasksBlock();

         // then wait for the the thread to stop running.
         while (isRunning())
         {
            mTaskQueue->ReleaseTasksBlock();
            OpenThreads::Thread::YieldCurrentThread();
         }
      }

      return result;
   }

   void TaskThread::run()
   {
      bool firstTime = true;

      // Run Loop
      while (!mDone)
      {
         dtCore::RefPtr<ThreadPoolTask> task;
         dtCore::RefPtr<TaskQueue> queue;

         queue = mTaskQueue;

         //printf("Preparing To Run a task! %p \n", this);
         // execute any task and block if there are none
         if ((!queue->ExecuteSingleTask() && !mDone) || firstTime)
         {
            //printf("Yielding worker thread! %p \n", this);
            OpenThreads::Thread::YieldCurrentThread();
            firstTime = false;
         }


         testCancel();
      }

      //Probably not required. If we got here, we're already canceled.
      OpenThreads::Thread::cancel();
      mDone = true;

   }



   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   ThreadPoolTask::ThreadPoolTask()
   : osg::Referenced(true)
   , mName("Task")
   , mKeep(false)
   {
      //default it to released.
      mBlockUntilComplete.release();
   }

   ThreadPoolTask::~ThreadPoolTask()
   {
   }

   DT_IMPLEMENT_ACCESSOR(ThreadPoolTask, dtUtil::RefString, Name);
   DT_IMPLEMENT_ACCESSOR(ThreadPoolTask, bool, Keep);

   //////////////////////////////////////
   void ThreadPoolTask::ResetWaitBlock()
   {
      mBlockUntilComplete.reset();
   }

   //////////////////////////////////////
   void ThreadPoolTask::ReleaseWaitBlock()
   {
      mBlockUntilComplete.release();
   }

   //////////////////////////////////////
   bool ThreadPoolTask::WaitUntilComplete(int timeoutMS)
   {
      bool result = false;
      if (timeoutMS >= 0)
      {
         result = mBlockUntilComplete.block(timeoutMS);
      }
      else
      {
         result = mBlockUntilComplete.block();
      }
      return result;
   }

   //////////////////////////////////////////////////
   //////////////////////////////////////////////////

   class ThreadPoolImpl
   {
   public:
      ThreadPoolImpl()
      : mTaskThreadForBackgroundOnly(false)
      , mInitialized(false)
      {
      }

      dtCore::RefPtr<TaskQueue> mTaskQueue;
      dtCore::RefPtr<TaskQueue> mBackgroundQueue;
      dtCore::RefPtr<TaskQueue> mIOQueue;

      std::vector<dtCore::RefPtr<TaskThread> > mTaskThreads;
      bool mTaskThreadForBackgroundOnly;
      bool mInitialized;
   };

   static ThreadPoolImpl gThreadPoolImpl;

   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   bool ThreadPool::IsInitialized()
   {
      return gThreadPoolImpl.mInitialized;
   }

   //////////////////////////////////////////////////
   void ThreadPool::Init(int numThreads)
   {
      if (gThreadPoolImpl.mInitialized)
      {
         return;
      }

      if (numThreads < 0)
      {
         numThreads = OpenThreads::GetNumberOfProcessors() - 1;
      }

      gThreadPoolImpl.mTaskQueue = new TaskQueue;
      gThreadPoolImpl.mBackgroundQueue = gThreadPoolImpl.mTaskQueue;

      if (numThreads <= 0)
      {
         // On a single core box, or if the user specifies 0 worker threads,
         // we still have to create one thread for background processes.
         // Immediate stuff will only be run when ExecuteTasks is called.
         numThreads = 1;
         gThreadPoolImpl.mTaskThreadForBackgroundOnly = true;
         gThreadPoolImpl.mBackgroundQueue = new TaskQueue;
      }

      gThreadPoolImpl.mIOQueue = new TaskQueue;

      for (int i = 0; i < numThreads; ++i)
      {
         dtCore::RefPtr<TaskThread> newThread;
         // the background queue may also be the main task queue.
         newThread = new TaskThread(*gThreadPoolImpl.mBackgroundQueue);

         gThreadPoolImpl.mTaskThreads.push_back(newThread);
         newThread->start();
      }

      {
         dtCore::RefPtr<TaskThread> newThread;
         // the make a thread just for the io queue.
         newThread = new TaskThread(*gThreadPoolImpl.mIOQueue);

         gThreadPoolImpl.mTaskThreads.push_back(newThread);
         newThread->start();
      }

      gThreadPoolImpl.mInitialized = true;
   }

   //////////////////////////////////////////////////
   void ThreadPool::Shutdown()
   {
      gThreadPoolImpl.mTaskThreads.clear();
      gThreadPoolImpl.mTaskQueue = NULL;
      gThreadPoolImpl.mBackgroundQueue = NULL;
      gThreadPoolImpl.mIOQueue = NULL;
      gThreadPoolImpl.mInitialized = false;
   }

   //////////////////////////////////////////////////
   void ThreadPool::AddTask(ThreadPoolTask& task, PoolQueue queue)
   {
      task.ResetWaitBlock();
      if (queue == IMMEDIATE)
      {
         gThreadPoolImpl.mTaskQueue->Add(task, 0);
      }
      else if (queue == BACKGROUND)
      {
         // in cases where worker threads > 0, the background queue is the same pointer as the task queue
         gThreadPoolImpl.mBackgroundQueue->Add(task, 1);
      }
      else if (queue == IO)
      {
         gThreadPoolImpl.mIOQueue->Add(task, 1);
      }
   }

   //////////////////////////////////////////////////
   void ThreadPool::ExecuteTasks()
   {
      gThreadPoolImpl.mTaskQueue->ExecuteTasks(true, 0);
   }

   //////////////////////////////////////////////////
   unsigned ThreadPool::GetNumImmediateWorkerThreads()
   {
      if (gThreadPoolImpl.mTaskThreadForBackgroundOnly)
      {
         return 1U;
      }

      return gThreadPoolImpl.mTaskThreads.size();
   }

   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   ThreadPool::ThreadPool()
   {
   }

   //////////////////////////////////////////////////
   ThreadPool::ThreadPool(ThreadPool&)
   {
   }

   //////////////////////////////////////////////////
   ThreadPool::~ThreadPool()
   {
   }

   //////////////////////////////////////////////////
   ThreadPool& ThreadPool::operator=(ThreadPool&)
   {
      return *this;
   }

}
