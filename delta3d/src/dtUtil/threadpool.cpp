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

#include <dtUtil/threadpool.h>

#include <OpenThreads/Thread>
#include <OpenThreads/Atomic>
#include <OpenThreads/Block>
#include <OpenThreads/Mutex>
#include <list>
#include <set>


namespace dtUtil
{

   class TaskThread;

   class DT_UTIL_EXPORT TaskQueue : public osg::Referenced
   {
       public:

           TaskQueue();

           /** Return true if the operation queue is empty. */
           bool Empty() const { return mTasks.empty(); }

           /** Return the num of pending tasks that are sitting in the TaskQueue.*/
           unsigned int GetNumTasksInQueue() const { return unsigned(mTasks.size()); }

           /** Add a task to end of TaskQueue, this will be
             * executed by the task thread once this operation gets to the head of the queue.*/
           void Add(ThreadPoolTask& task);

           /** Remove task from TaskQueue.*/
           void Remove(ThreadPoolTask& task);

           /** Remove named task from TaskQueue.*/
           void Remove(const dtUtil::RefString& name);

           /** Remove all tasks from TaskQueue.*/
           void RemoveAllTasks();

           /** Run the tasks. and optionally wait until all threads complete their tasks for this queue as well.
            */
           void ExecuteTasks(bool waitForAllTasksToBeCompleted = true);

           /**
            * Run one task
            * @param blockIfEmpty if the queue is empty at the start, then block until a task is queued or the block
            *                     is otherwise released.
            * @return true if a task was executed.
            */
           bool ExecuteSingleTask(bool blockIfEmpty = true);

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

           typedef std::list< dtCore::RefPtr<ThreadPoolTask> > Tasks;

           OpenThreads::Mutex     mTasksMutex;
           OpenThreads::Block     mTasksBlock;
           Tasks                  mTasks;
           Tasks::iterator        mCurrentTaskIterator;

           TaskThreads            mTaskThreads;
           OpenThreads::Atomic    mInProcessTasks;
   };

   TaskQueue::TaskQueue():
       osg::Referenced(true)
   {
       mCurrentTaskIterator = mTasks.begin();
   }

   TaskQueue::~TaskQueue()
   {
   }

   void TaskQueue::Add(ThreadPoolTask& task)
   {
       // acquire the lock on the operations queue to prevent anyone else for modifying it at the same time
       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

       // add the operation to the end of the list
       mTasks.push_back(&task);

       mTasksBlock.release();
   }

   void TaskQueue::Remove(ThreadPoolTask& task)
   {
       // acquire the lock on the operations queue to prevent anyone else for modifying it at the same time
       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

       for(Tasks::iterator itr = mTasks.begin();
           itr!=mTasks.end();)
       {
           if ((*itr)==&task)
           {
               bool needToResetCurrentIterator = (mCurrentTaskIterator == itr);

               itr = mTasks.erase(itr);

               if (needToResetCurrentIterator) mCurrentTaskIterator = itr;

           }
           else ++itr;
       }

       if (mTasks.empty())
       {
           mTasksBlock.reset();
       }
   }

   void TaskQueue::Remove(const dtUtil::RefString& name)
   {
       // acquire the lock on the operations queue to prevent anyone else for modifying it at the same time
       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

       // find the remove all operations with specified name
       for(Tasks::iterator itr = mTasks.begin();
           itr!=mTasks.end();)
       {
           if ((*itr)->GetName() == name)
           {
               bool needToResetCurrentIterator = (mCurrentTaskIterator == itr);

               itr = mTasks.erase(itr);

               if (needToResetCurrentIterator) mCurrentTaskIterator = itr;
           }
           else ++itr;
       }

       if (mTasks.empty())
       {
           mTasksBlock.reset();
       }
   }

   void TaskQueue::RemoveAllTasks()
   {
       OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

       mTasks.clear();

       // reset current operator.
       mCurrentTaskIterator = mTasks.begin();

       if (mTasks.empty())
       {
           mTasksBlock.reset();
       }
   }

   bool TaskQueue::ExecuteSingleTask(bool blockIfEmpty)
   {
      if (blockIfEmpty && mTasks.empty())
      {
          mTasksBlock.block();
      }

      dtCore::RefPtr<ThreadPoolTask> currentTask = NULL;
      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mTasksMutex);

         if (mTasks.empty())
         {
            return false;
         }

         if (mCurrentTaskIterator == mTasks.end())
         {
             // iterator at end of operations so reset to beginning.
             mCurrentTaskIterator = mTasks.begin();
         }

         currentTask = *mCurrentTaskIterator;

         mCurrentTaskIterator = mTasks.erase(mCurrentTaskIterator);
         if (mTasks.empty())
         {
            mTasksBlock.reset();
         }

         ++mInProcessTasks;
      }
//
//      if (OpenThreads::Thread::CurrentThread() != NULL)
//         printf("Running Thread %d\n", OpenThreads::Thread::CurrentThread()->getThreadId());
//      else
//         printf("Running Unknown Thread\n");

      /// execute
      (*currentTask)();

      if (currentTask->GetKeep())
      {
         // re-add the task before decrementing the in process count so that code won't think all tasks are done
         Add(*currentTask);
      }

      --mInProcessTasks;

      return true;
   }

   void TaskQueue::ExecuteTasks(bool waitForAllTasksToBeComplete)
   {
      // First empty the queue
      while (ExecuteSingleTask(false))
         ;

      if (waitForAllTasksToBeComplete)
      {
         // Then wait for all other threads to complete their tasks.
         while (unsigned(mInProcessTasks) > 0)
         {
            OpenThreads::Thread::YieldCurrentThread();
         }
      }
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
      TaskThread();

      /** Set the TaskQueue. */
      void AddTaskQueue(TaskQueue* q);
      /** Set the TaskQueue. */
      void RemoveTaskQueue(TaskQueue* q);

      /** Run does the operation thread run loop.*/
      virtual void run();

      bool GetDone() const { return mDone; }
      void SetDone(bool done);

      /** Cancel this thread.*/
      virtual int cancel();

   protected:

      virtual ~TaskThread();

      volatile bool mDone;

      OpenThreads::Mutex                       mThreadMutex;
      std::vector<dtCore::RefPtr<TaskQueue> >  mTaskQueues;
   };

   TaskThread::TaskThread()
   : osg::Referenced(true)
   , mDone(false)
   {
   }

   TaskThread::~TaskThread()
   {
      cancel();
   }

   void TaskThread::AddTaskQueue(TaskQueue* q)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mThreadMutex);

      if (std::find(mTaskQueues.begin(), mTaskQueues.end(), dtCore::RefPtr<TaskQueue>(q)) != mTaskQueues.end())
         return;

      mTaskQueues.push_back(q);

      //if (_operationQueue.valid()) _operationQueue->addTaskThread(this);
   }

   void TaskThread::RemoveTaskQueue(TaskQueue* q)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mThreadMutex);

      std::vector<dtCore::RefPtr<TaskQueue> >::iterator found = std::find(mTaskQueues.begin(), mTaskQueues.end(), dtCore::RefPtr<TaskQueue>(q));

      if (found != mTaskQueues.end())
      {
         mTaskQueues.erase(found);
         //if (_operationQueue.valid()) _operationQueue->removeTaskThread(this);
      }
   }

   void TaskThread::SetDone(bool done)
   {
      if (mDone==done) return;

      mDone = done;

      if (mDone)
      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mThreadMutex);

         std::vector<dtCore::RefPtr<TaskQueue> >::iterator i, iend;
         i = mTaskQueues.begin();
         iend = mTaskQueues.end();
         for (; i != iend; ++i)
         {
            TaskQueue& tq = **i;
            tq.ReleaseTasksBlock();
         }
      }
   }

   int TaskThread::cancel()
   {
      int result = 0;
      if (isRunning())
      {
         SetDone(true);

         // then wait for the the thread to stop running.
         while (isRunning())
         {
            OpenThreads::Thread::YieldCurrentThread();
         }
      }

      return result;
   }

   void TaskThread::run()
   {
      // Run Loop
      do
      {
         dtCore::RefPtr<ThreadPoolTask> task;
         dtCore::RefPtr<TaskQueue> queue;

         bool didSomething = false;

         for (unsigned i = 0; !mDone && i < mTaskQueues.size(); ++i)
         {
            {
               OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mThreadMutex);

               // I hope this doesn't amount to double check locking :-)
               if (mTaskQueues.size() > i)
               {
                  queue = mTaskQueues[i];
               }
               else
               {
                  // The loop is now invalid because the list of queues has changed
                  break;
               }
            }

            while (!mDone && queue->ExecuteSingleTask(false))
            {
               didSomething = true;
            }
         }

         if (!didSomething)
         {
            // yield when there was nothing to do for a full iteration over all the queues.
            // if there was something to do, there may be more to do in the earlier queues
            // by this time, so don't yield.
            OpenThreads::Thread::YieldCurrentThread();
         }

      } while (!mDone);

   }



   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   ThreadPoolTask::ThreadPoolTask()
   : mName("Task")
   , mKeep(false)
   {
   }

   IMPLEMENT_PROPERTY(ThreadPoolTask, dtUtil::RefString, Name);
   IMPLEMENT_PROPERTY(ThreadPoolTask, bool, Keep);


   //////////////////////////////////////////////////
   //////////////////////////////////////////////////

   class ThreadPoolImpl
   {
   public:
      ThreadPoolImpl()
      {
      }

      dtCore::RefPtr<TaskQueue> mImmediateQueue;
      dtCore::RefPtr<TaskQueue> mBackgroundQueue;

      std::vector<dtCore::RefPtr<TaskThread> > mTaskThreads;
   };

   static ThreadPoolImpl gThreadPoolImpl;

   //////////////////////////////////////////////////
   //////////////////////////////////////////////////
   void ThreadPool::Init(int numThreads)
   {
      if (numThreads < 0)
      {
         numThreads = OpenThreads::GetNumberOfProcessors() - 1;
      }

      bool threadForBackgroundOnly = false;

      if (numThreads <= 0)
      {
         // On a single core box, or if the user specifies 0 worker threads,
         // we still have to create one thread for background processes.
         // Immediate stuff will only be run when ExecuteTasks is called.
         numThreads = 1;
         threadForBackgroundOnly = true;
      }

      gThreadPoolImpl.mImmediateQueue = new TaskQueue;
      gThreadPoolImpl.mBackgroundQueue = new TaskQueue;

      for (int i = 0; i < numThreads; ++i)
      {
         dtCore::RefPtr<TaskThread> newThread = new TaskThread;
         if (!threadForBackgroundOnly)
         {
            newThread->AddTaskQueue(gThreadPoolImpl.mImmediateQueue);
         }
         newThread->AddTaskQueue(gThreadPoolImpl.mBackgroundQueue);
         gThreadPoolImpl.mTaskThreads.push_back(newThread);
         newThread->start();
      }
   }

   //////////////////////////////////////////////////
   void ThreadPool::Shutdown()
   {
      gThreadPoolImpl.mTaskThreads.clear();
      gThreadPoolImpl.mImmediateQueue = NULL;
      gThreadPoolImpl.mBackgroundQueue = NULL;
   }

   //////////////////////////////////////////////////
   void ThreadPool::AddTask(ThreadPoolTask& task, PoolQueue queue)
   {
      if (queue == IMMEDIATE)
      {
         gThreadPoolImpl.mImmediateQueue->Add(task);
      }
      else if (queue == BACKGROUND)
      {
         gThreadPoolImpl.mBackgroundQueue->Add(task);
      }
   }

   //////////////////////////////////////////////////
   void ThreadPool::ExecuteTasks()
   {
      gThreadPoolImpl.mImmediateQueue->ExecuteTasks(true);
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
