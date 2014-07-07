/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, MOVES Institute
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
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>

#include <dtABC/application.h>

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/animactorregistry.h>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/timer.h>

#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>
#include <dtCore/map.h>
#include <dtCore/project.h>

#include <dtAnim/animationgameactor.h>
#include <dtActors/engineactorregistry.h>

#include <osg/Geode>

#include <string>

extern dtABC::Application& GetGlobalApplication();

namespace dtAnim
{
   class TestAnimHelper: public AnimationHelper
   {
      bool mHasBeenUpdated;

   public:
      TestAnimHelper(): mHasBeenUpdated(false) {}

      bool HasBeenUpdated(){return mHasBeenUpdated;}

      void Update(float dt)
      {
         mHasBeenUpdated = true;
      }
   };

   class AnimationComponentTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AnimationComponentTests);
         CPPUNIT_TEST(TestAnimationComponent);
         CPPUNIT_TEST(TestAnimationPerformance);
         CPPUNIT_TEST(TestRegisterUnregister);
         CPPUNIT_TEST(TestRegisterMapUnload);
         CPPUNIT_TEST(TestAnimationActorComponentInit);
         CPPUNIT_TEST(TestAnimationEventFiring_FullSpeed);
         CPPUNIT_TEST(TestAnimationEventFiring_TwiceSpeed);
         CPPUNIT_TEST(TestAnimationEventFiring_HalfSpeed);
      CPPUNIT_TEST_SUITE_END();

   public:
      AnimationComponentTests()
      {
      }

      void setUp();
      void tearDown();

      void TestAnimationComponent();
      void TestAnimationPerformance();
      void TestRegisterUnregister();
      void TestRegisterMapUnload();
      void TestAnimationActorComponentInit();
      void TestAnimationEventFiring_FullSpeed();
      void TestAnimationEventFiring_TwiceSpeed();
      void TestAnimationEventFiring_HalfSpeed();

      void SubtestAnimationEventFiring(float speed);

      // Helper methods.
      dtCore::RefPtr<AnimationHelper> CreateRealAnimationHelper();

   private:
      void SimulateMapUnloaded()
      {
         dtGame::MessageFactory& msgFac = mGM->GetMessageFactory();

         dtCore::RefPtr<dtGame::MapMessage> mapMsg;
         msgFac.CreateMessage(dtGame::MessageType::INFO_MAP_UNLOADED, mapMsg);
         mGM->SendMessage(*mapMsg);
         dtCore::System::GetInstance().Step();
      }

      dtUtil::Log* mLogger;
      dtCore::RefPtr<dtGame::GameManager> mGM;
      dtCore::RefPtr<AnimationComponent> mAnimComp;
      dtCore::RefPtr<dtGame::GameActorProxy> mTestGameActor;
      dtCore::RefPtr<AnimationHelper> mHelper;

      dtCore::RefPtr<dtCore::Scene>          mScene;
      dtCore::RefPtr<dtCore::Camera>         mCamera;
      dtCore::RefPtr<dtCore::DeltaWin>       mWin;
      dtCore::RefPtr<dtCore::View>           mView;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(AnimationComponentTests);

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::setUp()
   {
      mLogger = &dtUtil::Log::GetInstance("animationcomponenttests.cpp");

      dtABC::Application& app = GetGlobalApplication();
      mScene = app.GetScene();
      mWin = app.GetWindow();
      mCamera = app.GetCamera();

      dtCore::System::GetInstance().Config();
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();

      AnimNodeBuilder& nodeBuilder = Cal3DDatabase::GetInstance().GetNodeBuilder();
      if (nodeBuilder.SupportsSoftware())
      {
         nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateSoftware));
      }
      else
      {
         nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateNULL));
      }

      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
      mGM = new dtGame::GameManager(*mScene);
      mGM->SetApplication(app);
      mAnimComp = new AnimationComponent();
      mGM->AddComponent(*mAnimComp, dtGame::GameManager::ComponentPriority::NORMAL);

      mGM->CreateActor(*dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE, mTestGameActor);
      CPPUNIT_ASSERT(mTestGameActor.valid());

      mTestGameActor->GetComponent(mHelper);
      CPPUNIT_ASSERT(mHelper.valid());
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::tearDown()
   {
      dtCore::GameEventManager& gem = dtCore::GameEventManager::GetInstance();
      gem.ClearAllEvents();

      dtCore::System::GetInstance().Stop();
      if (mGM.valid())
      {
         mHelper = NULL;
         mTestGameActor = NULL;
         mGM->DeleteAllActors(true);
         mGM = NULL;
      }
      mAnimComp = NULL;
      Cal3DDatabase::GetInstance().TruncateDatabase();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestRegisterUnregister()
   {
      CPPUNIT_ASSERT(!mAnimComp->IsRegisteredActor(*mTestGameActor));
      mGM->AddActor(*mTestGameActor, false, false);
      CPPUNIT_ASSERT_MESSAGE("Addding the actor to the gm should auto register with the gm component",
               mAnimComp->IsRegisteredActor(*mTestGameActor));

      mGM->DeleteActor(*mTestGameActor);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Removing the actor should auto unregister with the gm component",
               !mAnimComp->IsRegisteredActor(*mTestGameActor));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestRegisterMapUnload()
   {
      mGM->AddActor(*mTestGameActor, false, false);
      CPPUNIT_ASSERT(mAnimComp->IsRegisteredActor(*mTestGameActor));
      SimulateMapUnloaded();
      CPPUNIT_ASSERT(!mAnimComp->IsRegisteredActor(*mTestGameActor));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestAnimationActorComponentInit()
   {
      dtCore::ObserverPtr<dtAnim::AnimationHelper> animAC = mTestGameActor->GetComponent<dtAnim::AnimationHelper>();

      CPPUNIT_ASSERT_EQUAL(false, animAC->GetLoadModelAsynchronously());
      CPPUNIT_ASSERT_EQUAL(true, animAC->GetEnableAttachingNodeToDrawable());

      animAC->SetLoadModelAsynchronously(false);
      animAC->SetEnableAttachingNodeToDrawable(false);
      CPPUNIT_ASSERT_EQUAL(false, animAC->GetLoadModelAsynchronously());
      CPPUNIT_ASSERT_EQUAL(false, animAC->GetEnableAttachingNodeToDrawable());

      animAC->SetSkeletalMesh(dtCore::ResourceDescriptor("SkeletalMeshes:Marine:marine.xml"));
      CPPUNIT_ASSERT(animAC->GetNode() == NULL);
      mGM->AddActor(*mTestGameActor, false, false);
      CPPUNIT_ASSERT(animAC->GetNode() != NULL);
      CPPUNIT_ASSERT(animAC->GetNode()->getNumParents() == 0);
      animAC->AttachNodeToDrawable();
      CPPUNIT_ASSERT_EQUAL(1U, animAC->GetNode()->getNumParents());
      CPPUNIT_ASSERT(animAC->GetNode()->getParent(0) == mTestGameActor->GetDrawable()->GetOSGNode());
      animAC->DetachNodeFromDrawable();
      CPPUNIT_ASSERT(animAC->GetNode()->getNumParents() == 0);

      animAC->AttachNodeToDrawable();
      CPPUNIT_ASSERT_EQUAL(1U, animAC->GetNode()->getNumParents());
      animAC->SetEnableAttachingNodeToDrawable(true);

      dtCore::RefPtr<osg::Node> nodeBackup = animAC->GetNode();
      animAC->SetSkeletalMesh(dtCore::ResourceDescriptor());
      CPPUNIT_ASSERT(animAC->GetNode() == NULL);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Setting the resource to null with AttachingNoteToDrawable enabled should unparent the node.",
               0U, nodeBackup->getNumParents());
      animAC->SetSkeletalMesh(dtCore::ResourceDescriptor("SkeletalMeshes:Marine:marine.xml"));
      CPPUNIT_ASSERT_EQUAL(1U, animAC->GetNode()->getNumParents());
      CPPUNIT_ASSERT(animAC->GetNode()->getParent(0) == mTestGameActor->GetDrawable()->GetOSGNode());

   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestAnimationComponent()
   {
      dtCore::RefPtr<TestAnimHelper> mHelper = new TestAnimHelper();

      mAnimComp->RegisterActor(*mTestGameActor, *mHelper);

      CPPUNIT_ASSERT(mAnimComp->IsRegisteredActor(*mTestGameActor));

      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(mHelper->HasBeenUpdated());
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestAnimationPerformance()
   {
      typedef std::vector<dtCore::BaseActorObject*> ProxyContainer;
      ProxyContainer proxies;
      ProxyContainer groundActor;

      //load map
      try
      {
         //uses example data for now
         std::string context = dtUtil::GetDeltaRootPath() + "/examples/data";
         dtCore::Project::GetInstance().SetContext(context, true);
         mGM->ChangeMap("AnimationPerformance");

         //step a few times to ensure the map loaded
         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();

         mGM->FindActorsByName("CharacterEntity", proxies);
         mGM->FindActorsByName("groundActor", groundActor);

         // we only want 20 for the test, so delete the rest.
         int size = proxies.size();
         int numToDelete = size - 20;
         CPPUNIT_ASSERT_MESSAGE("There should be at least 20 character actors in the map to pref test.",
                  numToDelete >= 0);

         for (int i = 0; i < numToDelete; ++i)
         {
            mGM->DeleteActor(*proxies[i]);
         }
         //Make sure they are deleted.
         dtCore::System::GetInstance().Step();

         //re-fetch the actors.
         proxies.clear();
         mGM->FindActorsByName("CharacterEntity", proxies);
         CPPUNIT_ASSERT_MESSAGE("There should be at exactly 20 actors in the vector",
                  numToDelete >= 0);
      }
      catch (dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Performance testing AnimationComponent with " +
            dtUtil::ToString(proxies.size()) + " animated entities.");

      //register animation actors
      ProxyContainer::iterator iter, end;
      for (iter = proxies.begin(), end = proxies.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* actor = dynamic_cast<dtGame::GameActorProxy*>(*iter);
         if (actor)
         {
            actor->GetComponent<dtAnim::AnimationHelper>()->SetGroundClamp(true);
         }
      }

      // lets do some performance testing
      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
      "Testing performance of PlayAnimation on AnimationHelper");

      //////////////////////////////////////////////////////////////////////////
      dtCore::Timer timer;
      dtCore::Timer_t timerStart, timerEnd;

      timerStart = timer.Tick();
      for (iter = proxies.begin(), end = proxies.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* actor = dynamic_cast<dtGame::GameActorProxy*>(*iter);
         if (actor)
         {
            actor->GetComponent<dtAnim::AnimationHelper>()->PlayAnimation("Walk");
         }
      }
      timerEnd = timer.Tick();

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Time Results for Play Animation: "
            + dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      //////////////////////////////////////////////////////////////////////////

      //////////////////////////////////////////////////////////////////////////
      int numUpdates = 60;
      float updateTime = 1.0f / 60.0f;
      dtCore::RefPtr<dtGame::Message> message =
         mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      dtGame::TickMessage* tick = dynamic_cast<dtGame::TickMessage*>(message.get());
      if (!tick)
      {
         CPPUNIT_FAIL("Invalid message type");
      }
      tick->SetDeltaSimTime(updateTime);

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Testing performance of " + dtUtil::ToString(numUpdates) +
            " updates on AnimationComponent");

      timerStart = timer.Tick();
      for (int i = 0; i < numUpdates; ++i)
      {
         mAnimComp->ProcessMessage(*message.get());
      }
      timerEnd = timer.Tick();

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Time Results for Update: "
            + dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      //////////////////////////////////////////////////////////////////////////


      if (!groundActor.empty())
      {
         ProxyContainer::iterator iter = groundActor.begin();
         dtCore::BaseActorObject* proxy = dynamic_cast<dtCore::BaseActorObject*>(*iter);
         if (proxy)
         {
            dtCore::Transformable* transform
               = dynamic_cast<dtCore::Transformable*>(proxy->GetDrawable());
            if (transform)
            {
               mAnimComp->SetTerrainActor(transform);
            }
         }

         mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
               "Testing performance of " + dtUtil::ToString(numUpdates) +
               " updates on AnimationComponent with ground clamping");

         timerStart = timer.Tick();
         for (int i = 0; i < numUpdates; ++i)
         {
            mAnimComp->ProcessMessage(*message.get());
         }
         timerEnd = timer.Tick();

         mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
               "Time Results for Update with Ground Clamp: "
               + dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      }
      else
      {
         LOG_ERROR("Cannot find ground");
      }
      //////////////////////////////////////////////////////////////////////////


      //////////////////////////////////////////////////////////////////////////

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Testing performance of ClearAnimation()");

      timerStart = timer.Tick();
      for (iter = proxies.begin(), end = proxies.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* actor = dynamic_cast<dtGame::GameActorProxy*>((*iter));
         if (actor)
         {
            actor->GetComponent<dtAnim::AnimationHelper>()->ClearAnimation("Walk", 0.0);
         }
      }
      timerEnd = timer.Tick();

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Time Results for Clear Animation: " +
            dtUtil::ToString(timer.DeltaMil(timerStart, timerEnd)) + " ms");
      //////////////////////////////////////////////////////////////////////////


      //close map
      try
      {
         if (!mGM->GetCurrentMap().empty())
         {
            dtCore::Project::GetInstance().CloseMap(
                  dtCore::Project::GetInstance().GetMap(mGM->GetCurrentMap()), true);
         }
      }
      catch(dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper method
   dtCore::RefPtr<AnimationHelper> AnimationComponentTests::CreateRealAnimationHelper()
   {
      dtCore::RefPtr<AnimationHelper> helper = new AnimationHelper();
      dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");

      std::string modelPath = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
      CPPUNIT_ASSERT(!modelPath.empty());
      helper->LoadModel(modelPath);

      return helper;
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestAnimationEventFiring_FullSpeed()
   {
      SubtestAnimationEventFiring(1.0f);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestAnimationEventFiring_TwiceSpeed()
   {
      SubtestAnimationEventFiring(2.0f);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestAnimationEventFiring_HalfSpeed()
   {
      SubtestAnimationEventFiring(0.5f);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::SubtestAnimationEventFiring(float speed)
   {
      // Create a tick message used to tick the component.
      float timeStep = 0.2f / speed;
      dtCore::RefPtr<dtGame::TickMessage> tickMessage;
      mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL, tickMessage);
      tickMessage->SetDeltaSimTime(timeStep);

      // Declare the test event names.
      dtCore::GameEventManager& gem = dtCore::GameEventManager::GetInstance();
      const std::string eventStart1("startEvent1"); // time 0
      const std::string eventStart2("startEvent2"); // time 0
      const std::string eventStart3("startEvent3"); // time 0
      const std::string eventMid1("midEvent1"); // time 0.25
      const std::string eventMid2("midEvent2"); // time 0.25
      const std::string eventMid3("midEvent3"); // time 0.5

      // Ensure that the events do not currently exist.
      CPPUNIT_ASSERT(gem.FindEvent(eventStart1) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart2) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart3) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid1) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid2) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid3) == NULL);

      // Create the test animation helper
      dtCore::RefPtr<AnimationHelper> helper = CreateRealAnimationHelper();
      CPPUNIT_ASSERT( ! helper->GetSendEventCallback().valid());
      mAnimComp->RegisterActor(*mTestGameActor, *helper);
      // --- Ensure that the component has registered its
      //     send event method with the helper.
      CPPUNIT_ASSERT(helper->GetSendEventCallback().valid());
      helper->SetCommandCallbacksEnabled(true);

      // Set the speed on the animation wrappers.
      dtAnim::Cal3DModelWrapper* wrapper = helper->GetModelWrapper();
      const dtAnim::Cal3DModelData* modelData = dtAnim::Cal3DDatabase::GetInstance().GetModelData(*wrapper);

      typedef dtAnim::Cal3DModelData::AnimatableArray AnimArray;
      const AnimArray& anims = modelData->GetAnimatables();
      AnimArray::const_iterator curIter = anims.begin();
      for (; curIter != anims.end(); ++curIter)
      {
         (*curIter)->SetSpeed(speed);
      }

      // Start playing the animation that contains those events.
      helper->PlayAnimation("TestEventsAction");
      mAnimComp->ProcessMessage(*tickMessage); // Now at time 0.2
      // --- Ensure the start events were fired.
      CPPUNIT_ASSERT(gem.FindEvent(eventStart1) != NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart2) != NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart3) != NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid1) == NULL); // will not execute until time 0.25
      CPPUNIT_ASSERT(gem.FindEvent(eventMid2) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid3) == NULL);

      // --- Ensure some middle events were fired.
      gem.ClearAllEvents();
      mAnimComp->ProcessMessage(*tickMessage); // Now at time 0.4
      CPPUNIT_ASSERT(gem.FindEvent(eventStart1) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart2) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart3) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid1) != NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid2) != NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid3) == NULL); // will not execute until time 0.5

      // --- Ensure the last middle event was fired.
      gem.ClearAllEvents();
      mAnimComp->ProcessMessage(*tickMessage); // Now at time 0.6
      CPPUNIT_ASSERT(gem.FindEvent(eventStart1) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart2) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventStart3) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid1) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid2) == NULL);
      CPPUNIT_ASSERT(gem.FindEvent(eventMid3) != NULL);
   }

} // namespace dtAnim
