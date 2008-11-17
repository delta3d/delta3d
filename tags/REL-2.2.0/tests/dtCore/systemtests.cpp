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
#include <prefix/dtgameprefix-src.h>
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
      bool mPreframeCalled;
      bool mPostFrameCalled;
      bool mFrameCalled;


      DummyDrawable()
      {      
         m_TimeOne = 0;
         m_TimeTwo = 0;

         ResetState();
         AddSender(&System::GetInstance());
      }

      ~DummyDrawable()
      {
         RemoveSender(&System::GetInstance());
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
      }

      void OnMessage(dtCore::Base::MessageData* data)
      {
         if(data->message == "preframe")
         {
            mPreframeCalled = true;
            double userData[2] = {0.0,0.0};
            memcpy(userData, data->userData, sizeof(double) * 2);
            m_TimeOne = userData[0];
            m_TimeTwo = userData[1];
         }
         else if(data->message == "frame")
         {
            mFrameCalled = true;
         }
         else if(data->message == "postframe")
         {
            mPostFrameCalled = true;
         }
      }

   public:
      double m_TimeOne;
      double m_TimeTwo;
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

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();
      void TestSimMode();
      void TestProperties();
      void TestStepping();

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
   dtCore::System::GetInstance().SetFrameRate(1.0f/60.0f);

   System::GetInstance().SetSystemStages(System::STAGES_DEFAULT);
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::TestStepping()
{
   dtCore::RefPtr<dtABC::Application> app = &GetGlobalApplication();

   mDummyDrawable = new DummyDrawable();
   mDummyNode = new DummyNode();
   mDummyCallback = new DummyCallback();

   app->GetScene()->AddDrawable(mDummyDrawable.get());
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
}

//////////////////////////////////////////////////////////////////////////
void SystemTests::TestProperties()
{
   dtCore::System& ourSystem = dtCore::System::GetInstance();

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Frame Step deflault is wrong",
      ourSystem.GetFrameRate(), 1.0/60.0 , 0.0001);

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

   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.0f, -50.0f, 20.0f);
   osg::Vec3 lookAtXYZ (0.0f, 10.f, 0.0f);
   osg::Vec3 upVec (0.0f, 0.0f, 1.0f );
   camPos.Set(camXYZ, lookAtXYZ, upVec);
   app->GetCamera()->SetTransform( camPos );

   mDummyDrawable = new DummyDrawable();
   mDummyNode = new DummyNode();
   mDummyCallback = new DummyCallback();

   app->GetScene()->AddDrawable(mDummyDrawable.get());
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

   //turn off the STAGE_FRAME
   System::GetInstance().SetSystemStages( dtUtil::Bits::Remove(currentStages, System::STAGE_FRAME) );

   mDummyCallback->mCallbackCalled = false;
   mDummyNode->ResetState();
   mDummyDrawable->ResetState();

   dtCore::System::GetInstance().Step();

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

   dtCore::System::GetInstance().Step();

   CPPUNIT_ASSERT(mDummyCallback->mCallbackCalled);
   CPPUNIT_ASSERT(mDummyNode->mDrawCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPreframeCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mPostFrameCalled);
   CPPUNIT_ASSERT(mDummyDrawable->mFrameCalled);
}
