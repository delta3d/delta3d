/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2010, Alion Science and Technology Corporation
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
 * @author David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/threadpool.h>
#include <dtUtil/log.h>

class TestTask : public dtUtil::ThreadPoolTask
{
public:
   TestTask(int keepTimes)
   : mKeepTimes(keepTimes)
   , mRunTimes(0)
   , mHasRun(false)
   , mOkayToDelete(false)
   {
      if (keepTimes > 0)
      {
         SetKeep(true);
      }
   }

   virtual ~TestTask()
   {
      if (!mOkayToDelete)
      {
         LOGN_ERROR("threadpooltests.cpp", "The Current thread pool task should not be deleted yet.");
      }
   }

   /// Implement this method to actually do the work for the task
   virtual void operator()()
   {
      mHasRun = true;
      ++mRunTimes;
      if (mRunTimes > mKeepTimes)
      {
         SetKeep(false);
      }
   }

   DT_DECLARE_ACCESSOR_INLINE(int, KeepTimes);
   DT_DECLARE_ACCESSOR_INLINE(int, RunTimes);
   DT_DECLARE_ACCESSOR_INLINE(bool, HasRun);
   DT_DECLARE_ACCESSOR_INLINE(bool, OkayToDelete);
};

/**
 * @class ThreadPoolTests
 * @brief Unit tests for the string utils class
 */
class ThreadPoolTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE(ThreadPoolTests);
   CPPUNIT_TEST(TestImmediateTasks);
   CPPUNIT_TEST(TestBackgroundTasksWithBlock);
   CPPUNIT_TEST_SUITE_END();

   public:
   ///////////////////////////////////////////////////////////////////////////////
   void setUp()
   {
      mOldNumImmediateWorkerThreads = -1;
      if (dtUtil::ThreadPool::IsInitialized())
      {
         mOldNumImmediateWorkerThreads = dtUtil::ThreadPool::GetNumImmediateWorkerThreads();
         dtUtil::ThreadPool::Shutdown();
      }
      dtUtil::ThreadPool::Init();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void tearDown()
   {
      dtUtil::ThreadPool::Shutdown();
      dtUtil::ThreadPool::Init(mOldNumImmediateWorkerThreads);
   }

   void TestImmediateTasks()
   {
      std::vector<dtCore::RefPtr<TestTask> > testTasks;

      const unsigned numTasks = 50U;
      for (unsigned i = 0; i < numTasks; ++i)
      {
         testTasks.push_back(new TestTask(1));
         //this should not block since the task has never been added to a queue.
         testTasks.back()->WaitUntilComplete();
         dtUtil::ThreadPool::AddTask(*testTasks.back());
      }

      dtUtil::ThreadPool::ExecuteTasks();

      for (unsigned i = 0; i < numTasks; ++i)
      {
         CPPUNIT_ASSERT(testTasks[i]->GetHasRun());
         CPPUNIT_ASSERT(testTasks[i]->GetRunTimes() == 2);
         testTasks[i]->SetOkayToDelete(true);
      }
   }

   void TestBackgroundTasksWithBlock()
   {
      std::vector<dtCore::RefPtr<TestTask> > testTasks;

      const unsigned numTasks = 10U;
      for (unsigned i = 0; i < numTasks; ++i)
      {
         testTasks.push_back(new TestTask(1));
         dtUtil::ThreadPool::AddTask(*testTasks.back(), dtUtil::ThreadPool::BACKGROUND);
      }

      for (unsigned i = 0; i < numTasks; ++i)
      {
         CPPUNIT_ASSERT(testTasks[i]->WaitUntilComplete(1000));
         CPPUNIT_ASSERT(testTasks[i]->GetHasRun());
         CPPUNIT_ASSERT(testTasks[i]->GetRunTimes() == 2);
         testTasks[i]->SetOkayToDelete(true);
      }
   }

   private:
      unsigned mOldNumImmediateWorkerThreads;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ThreadPoolTests);


