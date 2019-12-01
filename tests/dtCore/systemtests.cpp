/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
 * @author Bradley Anderegg
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/system.h>

#include <dtCore/base.h>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/NodeCallback>
#include <dtABC/application.h>
#include <dtCore/scene.h>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <dtCore/transform.h>
#include <dtCore/camera.h>
#include <dtUtil/bits.h>
#include <dtUtil/mathdefines.h>

extern dtABC::Application& GetGlobalApplication();

using namespace dtCore;

class DummyCallback: public osg::NodeCallback
{
public:
   DummyCallback():mCallbackCalled(false){}

   void operator()(osg::Node*, osg::NodeVisitor* nv)
   {
      mCallbackCalled = true;
   }

   bool mCallbackCalled;
};

class DummyDrawable: public dtCore::DeltaDrawable
{
public:
   double m_TimeOne;
   double m_TimeTwo;

   int mCounter;
   bool mPreframeCalled;
   bool mPostFrameCalled;
   bool mFrameCalled;
   bool mEventTraversalCalled;
   bool mPostEventTraversalCalled;
   bool mFrameSynchCalled;
   bool mCameraSynchCalled;
   bool mPauseStartCalled;
   bool mPauseEndCalled;
   bool mPauseCalled;
   bool mExitCalled;
   bool mConfigCalled;

   int mPreframeOrder;
   int mPostFrameOrder;
   int mFrameOrder;
   int mEventTraversalOrder;
   int mPostEventTraversalOrder;
   int mFrameSynchOrder;
   int mCameraSynchOrder;
   int mPauseStartOrder;
   int mPauseEndOrder;
   int mPauseOrder;
   int mExitOrder;
   int mConfigOrder;

   DummyDrawable()
   {
      m_TimeOne = 0;
      m_TimeTwo = 0;

      ResetState();
      dtCore::System::GetInstance().TickSignal.connect_slot(this, &DummyDrawable::OnSystem);
   }

   ~DummyDrawable()
   {
   }

   const osg::Node* GetOSGNode() const
   {
      return 0;
   }

   osg::Node* GetOSGNode()
   {
      return 0;
   }

   void ResetState()
   {
      mPreframeCalled = false;
      mPostFrameCalled = false;
      mFrameCalled = false;
      mEventTraversalCalled = false;
      mPostEventTraversalCalled = false;
      mCameraSynchCalled = false;
      mFrameSynchCalled = false;
      mPauseStartCalled = false;
      mPauseEndCalled = false;
      mPauseCalled = false;
      mExitCalled = false;
      mConfigCalled = false;

      mPreframeOrder = 0;
      mPostFrameOrder = 0;
      mFrameOrder = 0;
      mEventTraversalOrder = 0;
      mPostEventTraversalOrder = 0;
      mCameraSynchOrder = 0;
      mFrameSynchOrder = 0;
      mPauseStartOrder = 0;
      mPauseEndOrder = 0;
      mPauseOrder = 0;
      mExitOrder = 0;
      mConfigOrder = 0;

      mCounter = 0;
   }

   void OnSystem(const dtUtil::RefString& str, double dtSim, double dtReal)

   {
      if (str == dtCore::System::MESSAGE_EVENT_TRAVERSAL)
      {
         mEventTraversalCalled = true;
         mEventTraversalOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
      {
         mPostEventTraversalCalled = true;
         mPostEventTraversalOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_PRE_FRAME)
      {
         mPreframeCalled = true;
         m_TimeOne = dtSim;
         m_TimeTwo = dtReal;

         mPreframeOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_CAMERA_SYNCH)
      {
         mCameraSynchCalled = true;
         mCameraSynchOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_FRAME_SYNCH)
      {
         mFrameSynchCalled = true;
         mFrameSynchOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_FRAME)
      {
         mFrameCalled = true;
         mFrameOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_POST_FRAME)
      {
         mPostFrameCalled = true;
         mPostFrameOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_PAUSE_START)
      {
         mPauseStartCalled = true;
         mPauseStartOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_PAUSE_END)
      {
         mPauseEndCalled = true;
         mPauseEndOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_PAUSE)
      {
         mPauseCalled = true;
         mPauseOrder = mCounter++;
      }
      else if (str == dtCore::System::MESSAGE_CONFIG)
      {
         mConfigCalled = true;
         mConfigOrder = mCounter++;
      }
   }
};


class DummyNode: public osg::ShapeDrawable
{
public:
   META_Object(osg::Drawable, DummyNode);

   DummyNode()
      : ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 10.0f, 0.0f), 10.0f))
      , mGeode(new osg::Geode())
      , mDrawCalled(false)
   {
      setUseDisplayList(false);
      ResetState();
      mGeode->addDrawable(this);
   }

   ~DummyNode()
   {
   }

   DummyNode(const DummyNode& dn, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
   {
      ResetState();
   }

   void drawImplementation(osg::RenderInfo& renderInfo) const
   {
      osg::ShapeDrawable::drawImplementation(renderInfo);
      mDrawCalled = true;
   }

   const osg::Node* GetOSGNode() const
   {
      return mGeode.get();
   }

   osg::Node* GetOSGNode()
   {
      return mGeode.get();
   }

   void ResetState()
   {
      mDrawCalled = false;
   }

   dtCore::RefPtr<osg::Geode> mGeode;

   mutable bool mDrawCalled;
};


class SystemTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(SystemTests);

   CPPUNIT_TEST(TestSimMode);
   CPPUNIT_TEST(TestProperties);
   CPPUNIT_TEST(TestStepping);
   CPPUNIT_TEST(TestSystemStages);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   void TestSimMode();
   void TestProperties();
   void TestStepping();
   void TestSystemStages();
   void AssertStages(int stageMask);
   void TestStage(int stageMask);

private:
   dtCore::RefPtr<DummyNode> mDummyNode;
   dtCore::RefPtr<DummyCallback> mDummyCallback;
   dtCore::RefPtr<DummyDrawable> mDummyDrawable;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SystemTests);

//////////////////////////////////////////////////////////////////////////
void SystemTests::setUp()
{
   System::GetInstance().SetSystemStages(System::STAGES_DEFAULT);
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::tearDown()
{
   dtCore::System::GetInstance().SetUseFixedTimeStep(false);
   dtCore::System::GetInstance().Stop();
   dtCore::System::GetInstance().SetPause(false);
   dtCore::System::GetInstance().SetFrameRate(60.0f);

   System::GetInstance().SetSystemStages(System::STAGES_DEFAULT);
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::TestStepping()
{
   dtCore::RefPtr<dtABC::Application> app = &GetGlobalApplication();

   mDummyDrawable = new DummyDrawable();
   mDummyNode = new DummyNode();
   mDummyCallback = new DummyCallback();

   app->GetScene()->AddChild(mDummyDrawable.get());
   app->GetScene()->GetSceneNode()->addChild(mDummyNode->GetOSGNode());
   mDummyNode->GetOSGNode()->setCullCallback(mDummyCallback.get());

   app->Config();

   dtCore::System& ourSystem = dtCore::System::GetInstance();
   ourSystem.SetFrameRate(1.0/0.0321);
   ourSystem.SetMaxTimeBetweenDraws(.01);
   ourSystem.SetUseFixedTimeStep(true);

   ourSystem.SetShutdownOnWindowClose(false);
   ourSystem.Start();

   CPPUNIT_ASSERT(ourSystem.GetSimulationTime() == 0.0);
   CPPUNIT_ASSERT(ourSystem.GetCorrectSimulationTime() == 0.0);

   ourSystem.SetPause(false);
   ourSystem.Step();

   ourSystem.SetSystemStages(System::STAGES_DEFAULT);

   ourSystem.SetTimeScale(1.0f);

   dtCore::AppSleep(200);
   ourSystem.Step();

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Time should be fixed",
            0.0321, mDummyDrawable->m_TimeOne, 0.001);

   ourSystem.SetFrameRate(1.0/0.0543);
   ourSystem.SetTimeScale(0.32);

   dtCore::AppSleep(100);
   ourSystem.Step();

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Time should be fixed, but scaled",
            0.0543 * 0.32, mDummyDrawable->m_TimeOne, 0.001);

   ourSystem.SetUseFixedTimeStep(false);
   ourSystem.SetTimeScale(1.0);
   ourSystem.SetFrameRate(60.0);
   ourSystem.SetMaxTimeBetweenDraws(0.03);
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::TestProperties()
{
   dtCore::System& ourSystem = dtCore::System::GetInstance();

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Frame Step default is wrong",
      ourSystem.GetFrameRate(), 60.0 , 0.0001);

   double aRandomFloat = dtUtil::RandFloat(10.0f, 50.0f);
   ourSystem.SetFrameRate(aRandomFloat);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Frame Step Property is broken",
      aRandomFloat, ourSystem.GetFrameRate(), 0.001);

   aRandomFloat = dtUtil::RandFloat(0.0f, 10.0f);
   ourSystem.SetMaxTimeBetweenDraws(aRandomFloat);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("GetMaxTimeBetweenDraws Property is broken",
      aRandomFloat, ourSystem.GetMaxTimeBetweenDraws(), 0.001);

   ourSystem.SetUseFixedTimeStep(true);

   CPPUNIT_ASSERT_MESSAGE("Use fixed time step is broken",
      true == ourSystem.GetUsesFixedTimeStep() );

   ourSystem.SetUseFixedTimeStep(false);

   CPPUNIT_ASSERT_MESSAGE("Use fixed time step is broken",
      false == ourSystem.GetUsesFixedTimeStep() );
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::TestSimMode()
{
   dtCore::RefPtr<dtABC::Application> app = &GetGlobalApplication();

   // adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.0f, -50.0f, 20.0f);
   osg::Vec3 lookAtXYZ (0.0f, 10.0f, 0.0f);
   osg::Vec3 upVec (0.0f, 0.0f, 1.0f );
   camPos.Set(camXYZ, lookAtXYZ, upVec);
   app->GetCamera()->SetTransform( camPos );

   mDummyDrawable = new DummyDrawable();
   mDummyNode = new DummyNode();
   mDummyCallback = new DummyCallback();

   app->GetScene()->AddChild(mDummyDrawable.get());
   app->GetScene()->GetSceneNode()->addChild(mDummyNode->GetOSGNode());
   mDummyNode->GetOSGNode()->setCullCallback(mDummyCallback.get());

   CPPUNIT_ASSERT_MESSAGE("System should be using default stages",
      dtUtil::Bits::Has((System::SystemStageFlags) System::STAGES_DEFAULT, System::GetInstance().GetSystemStages()) );

   CPPUNIT_ASSERT(!mDummyCallback->mCallbackCalled);
   CPPUNIT_ASSERT(!mDummyNode->mDrawCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPreframeCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPostFrameCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mFrameCalled);

   //app->Config();

   dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
   System::GetInstance().Start();

   /////////////////////////////////////////////////////////////
   System::SystemStageFlags currentStages = System::GetInstance().GetSystemStages();

   // turn off the STAGE_FRAME
   System::GetInstance().SetSystemStages( dtUtil::Bits::Remove(currentStages, System::STAGE_FRAME) );

   mDummyCallback->mCallbackCalled = false;
   mDummyNode->ResetState();
   mDummyDrawable->ResetState();

   dtCore::System::GetInstance().Step(0.016f);

   CPPUNIT_ASSERT(!mDummyCallback->mCallbackCalled);
   CPPUNIT_ASSERT(!mDummyNode->mDrawCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPreframeCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPostFrameCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mFrameCalled);

   ////////////////////////////////////////////////////////////
   mDummyCallback->mCallbackCalled = false;
   mDummyNode->ResetState();
   mDummyDrawable->ResetState();

   System::GetInstance().SetSystemStages(System::STAGES_DEFAULT);

   dtCore::System::GetInstance().Step(0.016f);

   CPPUNIT_ASSERT(mDummyCallback->mCallbackCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPreframeCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPostFrameCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mFrameCalled);
   CPPUNIT_ASSERT(mDummyNode->mDrawCalled);
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::TestSystemStages()
{
   dtCore::RefPtr<dtABC::Application> app = &GetGlobalApplication();

   mDummyDrawable = new DummyDrawable();

   app->GetScene()->AddChild(mDummyDrawable.get());

   dtCore::System& ourSystem = dtCore::System::GetInstance();
   ourSystem.SetShutdownOnWindowClose(false);
   ourSystem.SetUseFixedTimeStep(false);

   ourSystem.SetSystemStages(System::STAGE_CONFIG);

   mDummyDrawable->ResetState();
   AssertStages(0);
   app->Config();
   AssertStages(System::STAGE_CONFIG);
   mDummyDrawable->ResetState();

   ourSystem.SetSystemStages(System::STAGE_NONE);
   app->Config();
   AssertStages(System::STAGE_NONE);

   ourSystem.Start();

   // test each flag corresponds to the correct message
   TestStage(System::STAGE_EVENT_TRAVERSAL);
   TestStage(System::STAGE_POST_EVENT_TRAVERSAL);
   TestStage(System::STAGE_PREFRAME);
   TestStage(System::STAGE_CAMERA_SYNCH);
   TestStage(System::STAGE_FRAME_SYNCH);
   TestStage(System::STAGE_FRAME);
   TestStage(System::STAGE_POSTFRAME);

   // mix it up a bit, make sure it still works
   TestStage(System::STAGE_EVENT_TRAVERSAL | System::STAGE_POST_EVENT_TRAVERSAL);
   TestStage(System::STAGE_PREFRAME | System::STAGE_CAMERA_SYNCH);
   TestStage(System::STAGE_FRAME_SYNCH | System::STAGE_FRAME);
   TestStage(System::STAGE_POSTFRAME | System::STAGE_CAMERA_SYNCH);

   // test all the per frame messages and make sure the order is correct
   TestStage(System::STAGE_EVENT_TRAVERSAL | System::STAGE_POST_EVENT_TRAVERSAL |
             System::STAGE_PREFRAME | System::STAGE_CAMERA_SYNCH |
             System::STAGE_FRAME_SYNCH | System::STAGE_FRAME |
             System::STAGE_POSTFRAME | System::STAGE_CAMERA_SYNCH);

   // assert they were called in the proper order
   CPPUNIT_ASSERT(mDummyDrawable->mEventTraversalOrder == 0);
   CPPUNIT_ASSERT(mDummyDrawable->mPostEventTraversalOrder == 1);
   CPPUNIT_ASSERT(mDummyDrawable->mPreframeOrder == 2);
   CPPUNIT_ASSERT(mDummyDrawable->mCameraSynchOrder == 3);
   CPPUNIT_ASSERT(mDummyDrawable->mFrameSynchOrder == 4);
   CPPUNIT_ASSERT(mDummyDrawable->mFrameOrder == 5);
   CPPUNIT_ASSERT(mDummyDrawable->mPostFrameOrder == 6);


   // test pause
   mDummyDrawable->ResetState();
   ourSystem.SetSystemStages(System::STAGES_DEFAULT);
   ourSystem.Step();
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseStartCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseEndCalled);

   ourSystem.SetPause(true);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPauseStartCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseEndCalled);

   mDummyDrawable->ResetState();
   ourSystem.Step();
   CPPUNIT_ASSERT(mDummyDrawable->mPauseCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseStartCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseEndCalled);

   mDummyDrawable->ResetState();
   ourSystem.SetPause(false);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseCalled);
   CPPUNIT_ASSERT(!mDummyDrawable->mPauseStartCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPauseEndCalled);
}

void SystemTests::TestStage(int stageMask)
{
   dtCore::System& ourSystem = dtCore::System::GetInstance();
   mDummyDrawable->ResetState();

   ourSystem.SetSystemStages(stageMask);
   ourSystem.Step();
   AssertStages(stageMask);
}

void SystemTests::AssertStages( int stageMask )
{
   // we pass in the flag containing which stages should be set to true
   if (stageMask & System::STAGE_CONFIG)
   {
      CPPUNIT_ASSERT(mDummyDrawable->mConfigCalled);
   }
   else
   {
      CPPUNIT_ASSERT(!mDummyDrawable->mConfigCalled);
   }

   if (stageMask & System::STAGE_EVENT_TRAVERSAL)
   {
      CPPUNIT_ASSERT(mDummyDrawable->mEventTraversalCalled);
   }
   else
   {
      CPPUNIT_ASSERT(!mDummyDrawable->mEventTraversalCalled);
   }

   (stageMask & System::STAGE_POST_EVENT_TRAVERSAL) ? CPPUNIT_ASSERT(mDummyDrawable->mPostEventTraversalCalled) : CPPUNIT_ASSERT(!mDummyDrawable->mPostEventTraversalCalled);

   (stageMask & System::STAGE_PREFRAME) ? CPPUNIT_ASSERT(mDummyDrawable->mPreframeCalled) : CPPUNIT_ASSERT(!mDummyDrawable->mPreframeCalled);

   (stageMask & System::STAGE_CAMERA_SYNCH) ? CPPUNIT_ASSERT(mDummyDrawable->mCameraSynchCalled) : CPPUNIT_ASSERT(!mDummyDrawable->mCameraSynchCalled);

   (stageMask & System::STAGE_FRAME_SYNCH) ? CPPUNIT_ASSERT(mDummyDrawable->mFrameSynchCalled) : CPPUNIT_ASSERT(!mDummyDrawable->mFrameSynchCalled);

   (stageMask & System::STAGE_FRAME) ? CPPUNIT_ASSERT(mDummyDrawable->mFrameCalled) : CPPUNIT_ASSERT(!mDummyDrawable->mFrameCalled);

   (stageMask & System::STAGE_POSTFRAME) ? CPPUNIT_ASSERT(mDummyDrawable->mPostFrameCalled) : CPPUNIT_ASSERT(!mDummyDrawable->mPostFrameCalled);
}
