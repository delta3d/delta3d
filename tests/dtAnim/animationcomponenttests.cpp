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
#include "AnimModelLoadingTestFixture.h"

#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>

#include <dtABC/application.h>

#include <dtAnim/animationcomponent.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/animactorregistry.h>
#include <dtAnim/modeldatabase.h>

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

   class AnimationComponentTests : public AnimModelLoadingTestFixture
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

      void setUp() override;
      void tearDown() override;

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
      void LoadAnimationACModel();

   private:
      void OnModelLoaded(dtAnim::AnimationHelper*);
      void OnModelUnloaded(dtAnim::AnimationHelper*);

      void SimulateMapUnloaded()
      {
         dtGame::MessageFactory& msgFac = mGM->GetMessageFactory();

         dtCore::RefPtr<dtGame::MapMessage> mapMsg;
         msgFac.CreateMessage(dtGame::MessageType::INFO_MAP_UNLOADED, mapMsg);
         mGM->SendMessage(*mapMsg);
         dtCore::System::GetInstance().Step();
      }

      dtCore::RefPtr<AnimationComponent> mAnimComp;
      dtCore::RefPtr<dtGame::GameActorProxy> mTestGameActor;
      dtCore::RefPtr<AnimationHelper> mAnimAC;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(AnimationComponentTests);

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::setUp()
   {
      AnimModelLoadingTestFixture::setUp();
      try
      {
         AnimNodeBuilder* nodeBuilder = ModelDatabase::GetInstance().GetNodeBuilder();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder should be valid.", nodeBuilder != NULL);

         if (nodeBuilder->SupportsSoftware())
         {
            nodeBuilder->SetCreate(AnimNodeBuilder::CreateFunc(nodeBuilder, &AnimNodeBuilder::CreateSoftware));
         }
         else
         {
            nodeBuilder->SetCreate(AnimNodeBuilder::CreateFunc(nodeBuilder, &AnimNodeBuilder::CreateNULL));
         }

         mAnimComp = new AnimationComponent();
         mGM->AddComponent(*mAnimComp, dtGame::GameManager::ComponentPriority::NORMAL);

         mGM->CreateActor(*dtAnim::AnimActorRegistry::ANIMATION_ACTOR_TYPE, mTestGameActor);
         CPPUNIT_ASSERT(mTestGameActor.valid());

         mTestGameActor->GetComponent(mAnimAC);
         CPPUNIT_ASSERT(mAnimAC.valid());
         Connect(mAnimAC);
      }
      catch(dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
      catch(std::exception& e)
      {
         CPPUNIT_FAIL(e.what());
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::tearDown()
   {
      dtCore::GameEventManager& gem = dtCore::GameEventManager::GetInstance();
      gem.ClearAllEvents();

      mAnimAC = NULL;
      mTestGameActor = NULL;
      mAnimComp = NULL;
      ModelDatabase::GetInstance().TruncateDatabase();
      AnimModelLoadingTestFixture::tearDown();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::TestRegisterUnregister()
   {
      CPPUNIT_ASSERT(!mAnimComp->IsRegisteredActor(*mTestGameActor));
      mGM->AddActor(*mTestGameActor, false, false);
      CPPUNIT_ASSERT_MESSAGE("Adding the actor to the gm should auto register with the gm component",
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

      CPPUNIT_ASSERT_EQUAL(true, animAC->GetLoadModelAsynchronously());
      CPPUNIT_ASSERT_EQUAL(true, animAC->GetEnableAttachingNodeToDrawable());

      animAC->SetLoadModelAsynchronously(true);
      animAC->SetEnableAttachingNodeToDrawable(false);
      CPPUNIT_ASSERT_EQUAL(true, animAC->GetLoadModelAsynchronously());
      CPPUNIT_ASSERT_EQUAL(false, animAC->GetEnableAttachingNodeToDrawable());

      animAC->SetSkeletalMesh(dtCore::ResourceDescriptor("SkeletalMeshes:Marine:marine.xml"));
      CPPUNIT_ASSERT_MESSAGE("It should not load the character until it's added to the GM.", animAC->GetNode() == NULL);
      mGM->AddActor(*mTestGameActor, false, false);
      for (unsigned i = 0 ; i < 10 && animAC->IsLoadingAsynchronously(); ++i)
      {
         dtCore::AppSleep(50);
         animAC->CheckLoadingState();
      }
      animAC->Update(0.016f);

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
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Setting the resource to null with AttachingNodeToDrawable enabled should unparent the node.",
               0U, nodeBackup->getNumParents());
      animAC->SetSkeletalMesh(dtCore::ResourceDescriptor("SkeletalMeshes:Marine:marine.xml"));
      for (unsigned i = 0 ; i < 10 && animAC->IsLoadingAsynchronously(); ++i)
      {
         dtCore::AppSleep(50);
         animAC->CheckLoadingState();
      }
      animAC->Update(0.016f);
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
      dtCore::ActorPtrVector actors;
      dtCore::ActorPtrVector groundActor;

      //load map
      try
      {
         //uses example data for now
         std::string context = dtUtil::GetDeltaRootPath() + "/examples/data";
         dtCore::Project::GetInstance().SetContext(context, true);
         mGM->ChangeMap("AnimationPerformance");

         //step a few times to ensure the map loaded
         dtCore::System::GetInstance().Step(0.016);
         dtCore::System::GetInstance().Step(0.016);
         dtCore::System::GetInstance().Step(0.016);

         mGM->FindActorsByName("CharacterEntity", actors);
         mGM->FindActorsByName("groundActor", groundActor);

         // we only want 20 for the test, so delete the rest.
         int size = actors.size();
         int numToDelete = size - 20;
         CPPUNIT_ASSERT_MESSAGE("There should be at least 20 character actors in the map to pref test.",
                  numToDelete >= 0);

         for (int i = 0; i < numToDelete; ++i)
         {
            mGM->DeleteActor(*actors[i]);
         }
         //Make sure they are deleted.
         dtCore::System::GetInstance().Step();

         //re-fetch the actors.
         actors.clear();
         mGM->FindActorsByName("CharacterEntity", actors);
         CPPUNIT_ASSERT_MESSAGE("There should be at exactly 20 actors in the vector",
                  numToDelete >= 0);
      }
      catch (dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }

      mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, __FUNCTION__, __LINE__,
            "Performance testing AnimationComponent with " +
            dtUtil::ToString(actors.size()) + " animated entities.");

      //register animation actors
      dtCore::ActorPtrVector::iterator iter, end;
      for (iter = actors.begin(), end = actors.end(); iter != end; ++iter)
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
      for (iter = actors.begin(), end = actors.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* actor = dynamic_cast<dtGame::GameActorProxy*>(*iter);
         if (actor != nullptr)
         {
            dtAnim::AnimationHelper* animAC = actor->GetComponent<dtAnim::AnimationHelper>();
            for (unsigned i = 0 ; i < 10 && animAC->IsLoadingAsynchronously(); ++i)
            {
               dtCore::AppSleep(50);
               animAC->CheckLoadingState();
            }
            CPPUNIT_ASSERT(!animAC->IsLoadingAsynchronously());
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
         dtCore::ActorPtrVector::iterator iter = groundActor.begin();
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
      for (iter = actors.begin(), end = actors.end(); iter != end; ++iter)
      {
         dtGame::GameActorProxy* actor = dynamic_cast<dtGame::GameActorProxy*>((*iter));
         if (actor != nullptr)
         {
            dtAnim::AnimationHelper* animAC = actor->GetComponent<dtAnim::AnimationHelper>();
            animAC->ClearAnimation("Walk", 0.0);
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
   void AnimationComponentTests::LoadAnimationACModel()
   {
      dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");

      dtCore::ResourceDescriptor modelPath("SkeletalMeshes:Marine:marine_test.xml");
      LoadModel(mAnimAC, modelPath);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::OnModelLoaded(dtAnim::AnimationHelper*)
   {
      mModelLoaded = true;
   }
   /////////////////////////////////////////////////////////////////////////////
   void AnimationComponentTests::OnModelUnloaded(dtAnim::AnimationHelper*)
   {
      mModelUnloaded = true;
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
      LoadAnimationACModel();
      dtCore::RefPtr<AnimationHelper> animAC = mAnimAC;
      CPPUNIT_ASSERT( ! animAC->GetSendEventCallback().valid());
      mGM->AddActor(*mTestGameActor, false, false);
      // --- Ensure that the component has registered its
      //     send event method with the helper.
      CPPUNIT_ASSERT(animAC->GetSendEventCallback().valid());
      animAC->SetCommandCallbacksEnabled(true);

      // Set the speed on the animatables.
      dtAnim::BaseModelWrapper* wrapper = animAC->GetModelWrapper();
      CPPUNIT_ASSERT(wrapper != NULL);
      const dtAnim::BaseModelData* modelData = wrapper->GetModelData();
      CPPUNIT_ASSERT(modelData != NULL);

      typedef dtAnim::AnimatableArray AnimArray;
      const AnimArray& anims = modelData->GetAnimatables();
      AnimArray::const_iterator curIter = anims.begin();
      for (; curIter != anims.end(); ++curIter)
      {
         (*curIter)->SetSpeed(speed);
      }

      // Start playing the animation that contains those events.
      animAC->PlayAnimation("TestEventsAction");
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
