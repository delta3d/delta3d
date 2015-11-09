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

#include <dtAnim/animatable.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/basemodelwrapper.h>

#include <dtCore/project.h>
#include <dtCore/refptr.h>
#include <dtCore/timer.h>
#include <dtUtil/datapathutils.h>
#include "AnimModelLoadingTestFixture.h"

#include <osg/Math>
#include <sstream>

#include <string>

namespace dtAnim
{

   class TestAnimatable: public Animatable
   {
      public:
         TestAnimatable()
            : mDuration(0.0f) {}
         TestAnimatable(const TestAnimatable& anim):Animatable(anim){}
         TestAnimatable& operator=(const TestAnimatable& anim){Animatable::operator=(anim); return *this;}

         void Update(float dt){};
         void ForceFadeOut(float time){}; 
         void Recalculate(){}
         float CalculateDuration() const {return mDuration;}
         dtCore::RefPtr<Animatable> Clone(BaseModelWrapper* pWrapper)const{return new TestAnimatable(*this);}
         void Prune(){SetPrune(true);}

         void SetStartTime2(float animStart1){ SetStartTime(animStart1);}
         void SetEndTime2(float animEnd1){ SetEndTime(animEnd1);}
         void SetFadeIn2(float fadeIn1){ SetFadeIn(fadeIn1);}
         void SetFadeOut2(float fadeOut1){ SetFadeOut(fadeOut1);}
         void SetBaseWeight2(float base1){ SetBaseWeight(base1);}
         void SetName2(const std::string& name1){ SetName(name1);}
         void SetStartDelay2(float start_delay1){ SetStartDelay(start_delay1);}

         void SetElapsedTime2(float time){ SetElapsedTime(time);}

         void SetTestDuration(float seconds) {mDuration = seconds;}
         float GetTestDuration() const {return mDuration;}

         float mDuration;
   };

   class TestSequence: public AnimationSequence
   {
   };


   class TestController: public AnimationSequence::AnimationController
   {
      public:
         TestController(AnimationSequence& pSeq): AnimationController(pSeq) {}
         float TestComputeWeight(dtCore::RefPtr<TestAnimatable> anim)
         {
            SetComputeWeight(anim.get());
            return anim->GetCurrentWeight();
         }
   };



   class AnimationTests : public AnimModelLoadingTestFixture
   {
      
      CPPUNIT_TEST_SUITE( AnimationTests );
      CPPUNIT_TEST( UnitTestAnimatable );
      CPPUNIT_TEST( TestAnimatableEventNames );
      CPPUNIT_TEST( TestAnimChannel );
      CPPUNIT_TEST( TestAnimChannelRelativeTime );
      CPPUNIT_TEST( TestAnimSequence );
      CPPUNIT_TEST( TestAnimSequenceUpdate );
      CPPUNIT_TEST( TestAnimSequenceCalculateDuration );
      CPPUNIT_TEST( TestAnimInsert );
      CPPUNIT_TEST( TestAnimRelativeTime );
      CPPUNIT_TEST( TestSequenceMixer );
      CPPUNIT_TEST( TestAnimHelper );
      CPPUNIT_TEST( TestAnimHelperCustomCommands );
      CPPUNIT_TEST( TestAnimHelperEventCommands_Channel );
      CPPUNIT_TEST( TestAnimHelperEventCommands_Sequence );
      CPPUNIT_TEST( TestAnimController );
      CPPUNIT_TEST( TestAnimCallback );
      CPPUNIT_TEST_SUITE_END();
      
      public:
         void setUp() override;
         void tearDown() override;

         void UnitTestAnimatable(); 
         void TestAnimatableEventNames();
         void TestAnimChannel();
         void TestAnimChannelRelativeTime();
         void TestAnimSequence();
         void TestAnimSequenceUpdate();
         void TestAnimSequenceCalculateDuration();
         void TestAnimInsert();
         void TestAnimRelativeTime();
         void TestSequenceMixer();
         void TestAnimController();
         void TestAnimHelper();
         void TestAnimHelperCustomCommands();
         void TestAnimHelperEventCommands_Channel();
         void TestAnimHelperEventCommands_Sequence();
         void TestAnimCallback();

         void SubTestAnimHelperEventCommands(
            const std::string& testAnim, const std::string& eventPrefix);

         void OnAnimationCompleted(const dtAnim::Animatable& anim);

      private:

         template <typename T>
         void SetExpectedString(std::ostringstream& strstream, T expected, T result)
         {
            strstream.str("");
            strstream << "Expected \"" << expected << "\" but got \"" << result << "\"";
         }

         const dtAnim::Animatable* mLastAnimatableCompleted;

         float animStart1;
         float animEnd1;
         float fadeIn1;
         float fadeOut1;
         float base1;
         float start_delay1;
         std::string name1;
         dtCore::RefPtr<TestAnimatable> mAnimatable1;

         float animStart2;
         float animEnd2;
         float fadeIn2;
         float fadeOut2;
         float base2;
         float start_delay2;
         std::string name2;
         dtCore::RefPtr<TestAnimatable> mAnimatable2;

         dtCore::RefPtr<dtAnim::AnimationHelper> mHelper;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AnimationTests );

   static const unsigned TOTAL_TEST_EVENTS = 18;

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::setUp()
   {
      AnimModelLoadingTestFixture::setUp();
      mHelper = new AnimationHelper();
      Connect(mHelper);
      
      dtCore::ResourceDescriptor modelPath = dtCore::ResourceDescriptor("SkeletalMeshes:Marine:marine_test.xml");

      mLastAnimatableCompleted = NULL;

      LoadModel(mHelper, modelPath);

      animStart1 = 1.0f;
      animEnd1 = 2.0f;
      fadeIn1 = 0.5f;
      fadeOut1 = 1.0f;
      base1 = 1.5f;
      name1 = "Anim001";
      start_delay1 = 0.5f;
      mAnimatable1 = new TestAnimatable();

      mAnimatable1->SetStartTime2(animStart1);
      mAnimatable1->SetEndTime2(animEnd1);
      mAnimatable1->SetFadeIn2(fadeIn1);
      mAnimatable1->SetFadeOut2(fadeOut1);
      mAnimatable1->SetBaseWeight2(base1);
      mAnimatable1->SetName2(name1);
      mAnimatable1->SetStartDelay2(start_delay1);
      
      animStart2 = 1.0f;
      animEnd2 = 2.0f;
      fadeIn2 = 0.5f;
      fadeOut2 = 1.0f;
      base2 = 1.5f;
      start_delay2 = 3.0f;
      name2 = "Anim002";

      mAnimatable2 = new TestAnimatable();

      mAnimatable2->SetStartTime2(animStart2);
      mAnimatable2->SetEndTime2(animEnd2);
      mAnimatable2->SetFadeIn2(fadeIn2);
      mAnimatable2->SetFadeOut2(fadeOut2);
      mAnimatable2->SetBaseWeight2(base2);
      mAnimatable2->SetName2(name2);
      mAnimatable2->SetStartDelay2(start_delay2);

   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::tearDown()
   {
      mAnimatable1 = NULL;
      mAnimatable2 = NULL;
      mHelper = NULL;
      mLastAnimatableCompleted = NULL;
      AnimModelLoadingTestFixture::tearDown();
   }


   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::UnitTestAnimatable()
   {
      CPPUNIT_ASSERT_EQUAL(name1, mAnimatable1->GetName());
      CPPUNIT_ASSERT_EQUAL(animStart1, mAnimatable1->GetStartTime());
      CPPUNIT_ASSERT_EQUAL(animEnd1, mAnimatable1->GetEndTime());
      CPPUNIT_ASSERT_EQUAL(fadeIn1, mAnimatable1->GetFadeIn());
      CPPUNIT_ASSERT_EQUAL(fadeOut1, mAnimatable1->GetFadeOut());
      CPPUNIT_ASSERT_EQUAL(base1, mAnimatable1->GetBaseWeight());
      CPPUNIT_ASSERT(!mAnimatable1->IsActive());

      //test operator =
      dtCore::RefPtr<TestAnimatable> pOpEqual = new TestAnimatable();
      pOpEqual = mAnimatable1;

      CPPUNIT_ASSERT_EQUAL(name1, pOpEqual->GetName());
      CPPUNIT_ASSERT_EQUAL(animStart1, pOpEqual->GetStartTime());
      CPPUNIT_ASSERT_EQUAL(animEnd1, pOpEqual->GetEndTime());
      CPPUNIT_ASSERT_EQUAL(fadeIn1, pOpEqual->GetFadeIn());
      CPPUNIT_ASSERT_EQUAL(fadeOut1, pOpEqual->GetFadeOut());
      CPPUNIT_ASSERT_EQUAL(base1, pOpEqual->GetBaseWeight());
      CPPUNIT_ASSERT(!pOpEqual->IsActive());

      //test copy operator
      dtCore::RefPtr<TestAnimatable> pCopyOp(mAnimatable1.get());

      CPPUNIT_ASSERT_EQUAL(name1, pCopyOp->GetName());
      CPPUNIT_ASSERT_EQUAL(animStart1, pCopyOp->GetStartTime());
      CPPUNIT_ASSERT_EQUAL(animEnd1, pCopyOp->GetEndTime());
      CPPUNIT_ASSERT_EQUAL(fadeIn1, pCopyOp->GetFadeIn());
      CPPUNIT_ASSERT_EQUAL(fadeOut1, pCopyOp->GetFadeOut());
      CPPUNIT_ASSERT_EQUAL(base1, pCopyOp->GetBaseWeight());
      CPPUNIT_ASSERT(!pCopyOp->IsActive());

      //test clone
      dtCore::RefPtr<Animatable> pClone = mAnimatable1->Clone(mHelper->GetModelWrapper());

      CPPUNIT_ASSERT_EQUAL(name1, pClone->GetName());
      CPPUNIT_ASSERT_EQUAL(animStart1, pClone->GetStartTime());
      CPPUNIT_ASSERT_EQUAL(animEnd1, pClone->GetEndTime());
      CPPUNIT_ASSERT_EQUAL(fadeIn1, pClone->GetFadeIn());
      CPPUNIT_ASSERT_EQUAL(fadeOut1, pClone->GetFadeOut());
      CPPUNIT_ASSERT_EQUAL(base1, pClone->GetBaseWeight());
      CPPUNIT_ASSERT(!pClone->IsActive());
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimatableEventNames()
   {
      dtCore::RefPtr<TestAnimatable> anim = new TestAnimatable();
      const TestAnimatable* constAnim = anim.get();

      anim->SetTestDuration(10.0f);

      const std::string EVENT_1("Event1");
      const std::string EVENT_2("Event2");
      const std::string EVENT_3("Event3");

      // Test inserting at start.
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().empty());
      CPPUNIT_ASSERT(anim->AddEventOnStart(EVENT_1));
      CPPUNIT_ASSERT(anim->AddEventOnStart(EVENT_2));
      CPPUNIT_ASSERT(anim->AddEventOnTime(EVENT_3, 0.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 3);

      // Test inserting at start again with similar method calls.
      // Inserting at the same time again should fail.
      CPPUNIT_ASSERT( ! anim->AddEventOnTime(EVENT_1, 0.0f));
      CPPUNIT_ASSERT( ! anim->AddEventOnTime(EVENT_2, 0.0f));
      CPPUNIT_ASSERT( ! anim->AddEventOnStart(EVENT_3));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 3);

      // Insert at an arbitrary time offset.
      CPPUNIT_ASSERT(anim->AddEventOnTime(EVENT_1, 5.0f));
      CPPUNIT_ASSERT(anim->AddEventOnTime(EVENT_2, 5.0f));
      CPPUNIT_ASSERT(anim->AddEventOnTime(EVENT_3, 5.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 6);

      // Insert at the end time.
      CPPUNIT_ASSERT(anim->AddEventOnEnd(EVENT_1));
      CPPUNIT_ASSERT(anim->AddEventOnEnd(EVENT_2));
      CPPUNIT_ASSERT(anim->AddEventOnTime(EVENT_3, -1.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 9);

      // Insert at the end time using a similar method.
      // Inserting at the same time again should fail.
      CPPUNIT_ASSERT( ! anim->AddEventOnTime(EVENT_1, -1.0f));
      CPPUNIT_ASSERT( ! anim->AddEventOnTime(EVENT_2, -1.0f));
      CPPUNIT_ASSERT( ! anim->AddEventOnEnd(EVENT_3));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 9);


      // Test accessing the time offsets.
      Animatable::TimeOffsetArray times;
      CPPUNIT_ASSERT(constAnim->GetEventTimeOffsets(EVENT_1, times) == 3);
      CPPUNIT_ASSERT(times.size() == 3);
      CPPUNIT_ASSERT(times[0] == 0.0f);
      CPPUNIT_ASSERT(times[1] == 5.0f);
      CPPUNIT_ASSERT(times[2] == 10.0f);
      times.clear();

      CPPUNIT_ASSERT(constAnim->GetEventTimeOffsets(EVENT_2, times) == 3);
      CPPUNIT_ASSERT(times.size() == 3);
      CPPUNIT_ASSERT(times[0] == 0.0f);
      CPPUNIT_ASSERT(times[1] == 5.0f);
      CPPUNIT_ASSERT(times[2] == 10.0f);
      times.clear();

      CPPUNIT_ASSERT(constAnim->GetEventTimeOffsets(EVENT_3, times) == 3);
      CPPUNIT_ASSERT(times.size() == 3);
      CPPUNIT_ASSERT(times[0] == 0.0f);
      CPPUNIT_ASSERT(times[1] == 5.0f);
      CPPUNIT_ASSERT(times[2] == 10.0f);
      times.clear();


      // Test getters for specific events.
      CPPUNIT_ASSERT(constAnim->HasEventOnTime(EVENT_2, 0.0f));
      CPPUNIT_ASSERT( ! constAnim->HasEventOnTime(EVENT_2, 2.5f));
      CPPUNIT_ASSERT(constAnim->HasEventOnTime(EVENT_2, 5.0f));
      CPPUNIT_ASSERT( ! constAnim->HasEventOnTime(EVENT_2, 7.5f));
      CPPUNIT_ASSERT(constAnim->HasEventOnTime(EVENT_2, 10.0f));
      CPPUNIT_ASSERT( ! constAnim->HasEventOnTime(EVENT_2, 12.5f));

      CPPUNIT_ASSERT(constAnim->HasEventForTimeRange(EVENT_2, 0.0f, 1.0f));
      CPPUNIT_ASSERT( ! constAnim->HasEventForTimeRange(EVENT_2, 2.0f, 3.0f));
      CPPUNIT_ASSERT(constAnim->HasEventForTimeRange(EVENT_2, 4.0f, 6.0f));
      CPPUNIT_ASSERT( ! constAnim->HasEventForTimeRange(EVENT_2, 7.0f, 8.0f));
      CPPUNIT_ASSERT(constAnim->HasEventForTimeRange(EVENT_2, 9.0f, 11.0f));


      // Test getting events within a time range.
      Animatable::EventNameArray names;
      CPPUNIT_ASSERT(constAnim->GetEventsForTimeRange(0.0f, 5.0f, names) == 6);
      CPPUNIT_ASSERT(names.size() == 6);
      names.clear();
      CPPUNIT_ASSERT(constAnim->GetEventsForTimeRange(2.5f, 7.5f, names) == 3);
      CPPUNIT_ASSERT(names.size() == 3);
      names.clear();
      CPPUNIT_ASSERT(constAnim->GetEventsForTimeRange(5.0f, 10.0f, names) == 6);
      CPPUNIT_ASSERT(names.size() == 6);
      names.clear();
      CPPUNIT_ASSERT(constAnim->GetEventsForTimeRange(7.5f, 10.0f, names) == 3);
      CPPUNIT_ASSERT(names.size() == 3);
      names.clear();

      // Test removal of a single event name and a specified time.
      CPPUNIT_ASSERT(anim->RemoveEventOnTime(EVENT_2, 5.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 8);
      CPPUNIT_ASSERT( ! anim->RemoveEventOnTime(EVENT_2, 5.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 8);

      // Test removal of a single event name.
      CPPUNIT_ASSERT(anim->RemoveEvent(EVENT_2, &times) == 2);
      CPPUNIT_ASSERT(times.size() == 2);
      CPPUNIT_ASSERT(times[0] == 0.0f);
      CPPUNIT_ASSERT(times[1] == 10.0f);
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 6);
      times.clear();
      CPPUNIT_ASSERT(constAnim->GetEventTimeOffsets(EVENT_2, times) == 0);
      CPPUNIT_ASSERT(times.empty());

      // Test getting events within a time range.
      CPPUNIT_ASSERT(anim->GetEventsForTimeRange(4.99f, 5.01f, names) == 2);
      CPPUNIT_ASSERT(names.size() == 2);
      CPPUNIT_ASSERT(names[0] == EVENT_1);
      CPPUNIT_ASSERT(names[1] == EVENT_3);
      names.clear();

      // Test removal of events within a time range.
      CPPUNIT_ASSERT(anim->RemoveEventsForTimeRange(4.99f, 5.01f, &names) == 2);
      CPPUNIT_ASSERT(names.size() == 2);
      CPPUNIT_ASSERT(names[0] == EVENT_1);
      CPPUNIT_ASSERT(names[1] == EVENT_3);
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 4);
      names.clear();
      CPPUNIT_ASSERT(anim->RemoveEventsForTimeRange(4.99f, 5.01f, &names) == 0);
      CPPUNIT_ASSERT(names.empty());


      // Test removing a single event in a specified time range.
      CPPUNIT_ASSERT( ! anim->RemoveEventForTimeRange(EVENT_3, 4.0f, 6.0f));
      CPPUNIT_ASSERT( ! anim->RemoveEventForTimeRange(EVENT_3, 1.0f, 4.0f));
      CPPUNIT_ASSERT(anim->RemoveEventForTimeRange(EVENT_3, 9.0f, 11.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 3);
      CPPUNIT_ASSERT( ! anim->RemoveEventForTimeRange(EVENT_3, 9.0f, 11.0f));
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().size() == 3);


      // Test removing all events.
      CPPUNIT_ASSERT(anim->ClearEvents() == 3);
      CPPUNIT_ASSERT(constAnim->GetTimeEventMap().empty());
      CPPUNIT_ASSERT(anim->ClearEvents() == 0);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimChannel()
   {
      dtAnim::BaseModelWrapper* model = mHelper->GetModelWrapper();
      CPPUNIT_ASSERT(model != NULL);
      dtCore::RefPtr<AnimationChannel> channel = new AnimationChannel(model);
      
      std::string name("ChickenWalk");
      CPPUNIT_ASSERT_MESSAGE("Channel should have an empty name by default.", channel->GetName().empty());
      channel->SetName(name);
      CPPUNIT_ASSERT_MESSAGE("Channel name should have been set.", channel->GetName() == name);

      std::string animName(name);
      CPPUNIT_ASSERT_MESSAGE("Channel animation name should be empty by default.", channel->GetAnimationName().empty());
      channel->SetAnimationName(name);
      CPPUNIT_ASSERT_MESSAGE("Channel animation name should have been set.", channel->GetAnimationName() == name);

      float speed = 2.0f;
      CPPUNIT_ASSERT_MESSAGE("Channel speed should be 1 by default.", channel->GetSpeed() == 1.0f);
      channel->SetSpeed(speed);
      CPPUNIT_ASSERT_MESSAGE("Channel speed should should have been set.", channel->GetSpeed() == speed);
      
      float duration = 39.6f;
      CPPUNIT_ASSERT_MESSAGE("Channel duration should be 0 by default.", channel->GetDuration() == 0.0f);
      channel->SetDuration(duration);
      CPPUNIT_ASSERT_MESSAGE("Channel duration should have been set.", channel->GetDuration() == duration);

      CPPUNIT_ASSERT_MESSAGE("Channel should be looping by default.", channel->IsLooping());
      CPPUNIT_ASSERT_MESSAGE("Channel should not have a definite end when looping.",
         ! channel->HasDefiniteEnd());
      CPPUNIT_ASSERT_MESSAGE("Channel should have infinite time limit when looping",
         channel->CalculateDuration() == Animatable::INFINITE_TIME);

      channel->SetLooping(false);
      CPPUNIT_ASSERT_MESSAGE("Channel should not be looping.", ! channel->IsLooping());
      CPPUNIT_ASSERT_MESSAGE("Channel should have a definite end when not.",
         channel->HasDefiniteEnd());
      CPPUNIT_ASSERT_MESSAGE("Channel should have a time limit when not looping",
         channel->CalculateDuration() == 39.6f);

      channel->SetMaxDuration(100.0f);
      CPPUNIT_ASSERT_MESSAGE("Channel should have same duration if the max duration is greater.",
         channel->CalculateDuration() == 39.6f);

      channel->SetMaxDuration(10.0f);
      CPPUNIT_ASSERT_MESSAGE("Channel should have a duration equal to max duration if the original duration is greater.",
         channel->CalculateDuration() == 10.0f);

      channel->SetMaxDuration(0.0f);
      CPPUNIT_ASSERT_MESSAGE("Channel should have the original duration if max duration is set to zero.",
         channel->CalculateDuration() == 39.6f);

      // Test access to the associated animation object.
      const AnimationChannel* channelConst = channel.get();
      CPPUNIT_ASSERT_MESSAGE("The model should not have an animation object with the test name.", model->GetAnimation(name) == NULL);
      CPPUNIT_ASSERT_MESSAGE("The channel should not have an animation object with the test name.", channelConst->GetAnimation() == NULL);

      // Use a name of an animation object contained in the model.
      animName = "";
      const dtAnim::AnimationInterface* animFromModel = model->GetAnimation(animName);
      channel->SetAnimationName(animName);
      CPPUNIT_ASSERT_MESSAGE("Channel name can differ from the referenced animation object's name.",
         channel->GetName() != channel->GetAnimationName());

      const dtAnim::AnimationInterface* anim = channelConst->GetAnimation();
      CPPUNIT_ASSERT_MESSAGE("If a channel has no animation name it cannot access its associated animation.", animFromModel == NULL);

      name = "Walk";
      channel->SetAnimationName(name);
      anim = channelConst->GetAnimation();
      animFromModel = model->GetAnimation(name);
      CPPUNIT_ASSERT_MESSAGE("An animation with the specified name should be returned from the model.", animFromModel != NULL);
      CPPUNIT_ASSERT_MESSAGE("Channel should return a valid animation object if it has a valid name.", anim != NULL);
      CPPUNIT_ASSERT_MESSAGE("Both the model and the channel should return the same animation object.", animFromModel == anim);
      CPPUNIT_ASSERT_MESSAGE("The animation object name should match.", name == anim->GetName());

      channel->SetModel(NULL);
      CPPUNIT_ASSERT_MESSAGE("The model should no longer be accessible from the animation channel.", channel->GetModel() == NULL);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimChannelRelativeTime()
   {
      float epsilon = 0.001f;
      float duration = 3.0f;
      dtCore::RefPtr<AnimationChannel> channel = new AnimationChannel();
      channel->SetStartTime(1.0f);
      channel->SetDuration(duration);

      // --- Test action mode.
      channel->SetAction(true);
      channel->SetLooping(false);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, channel->ConvertToRelativeTimeInAnimationScope(0.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, channel->ConvertToRelativeTimeInAnimationScope(0.5f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, channel->ConvertToRelativeTimeInAnimationScope(2.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, channel->ConvertToRelativeTimeInAnimationScope(3.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, channel->ConvertToRelativeTimeInAnimationScope(4.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, channel->ConvertToRelativeTimeInAnimationScope(5.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, channel->ConvertToRelativeTimeInAnimationScope(6.0f), epsilon);
      // --- Test looping mode.
      channel->SetAction(false);
      channel->SetLooping(true);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, channel->ConvertToRelativeTimeInAnimationScope(0.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, channel->ConvertToRelativeTimeInAnimationScope(0.5f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, channel->ConvertToRelativeTimeInAnimationScope(2.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, channel->ConvertToRelativeTimeInAnimationScope(3.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(2.999f, channel->ConvertToRelativeTimeInAnimationScope(3.999f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, channel->ConvertToRelativeTimeInAnimationScope(4.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, channel->ConvertToRelativeTimeInAnimationScope(5.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, channel->ConvertToRelativeTimeInAnimationScope(6.0f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(2.999f, channel->ConvertToRelativeTimeInAnimationScope(6.999f), epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, channel->ConvertToRelativeTimeInAnimationScope(7.0f), epsilon);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimSequence()
   {
      dtCore::RefPtr<AnimationSequence> pSeq = new AnimationSequence();

      std::string animName = "Walk";

      SequenceMixer& mixer = mHelper->GetSequenceMixer();
      const Animatable* anim = mixer.GetRegisteredAnimation(animName);
      CPPUNIT_ASSERT(anim != NULL);
      
      dtCore::RefPtr<AnimationSequence> pChildSeq = new AnimationSequence();
      CPPUNIT_ASSERT(pChildSeq.get() == &pChildSeq->GetController()->GetParent());
      std::string childSeqName("childSequence");
      pChildSeq->SetName(childSeqName);
      pChildSeq->AddAnimation(anim->Clone(NULL).get());
      
      CPPUNIT_ASSERT_EQUAL(size_t(0), pSeq->GetChildAnimations().size());

      pSeq->AddAnimation(anim->Clone(NULL).get());
      CPPUNIT_ASSERT_EQUAL(size_t(1), pSeq->GetChildAnimations().size());

      pSeq->AddAnimation(mAnimatable2.get());
      CPPUNIT_ASSERT_EQUAL(size_t(2), pSeq->GetChildAnimations().size());

      pSeq->AddAnimation(pChildSeq.get());
      CPPUNIT_ASSERT_EQUAL(size_t(3), pSeq->GetChildAnimations().size());
      
      // Had to cast the first parameter to const to make the underlying template work. 
      CPPUNIT_ASSERT(pSeq->GetAnimation(animName) != NULL);
      CPPUNIT_ASSERT_EQUAL(pSeq->GetAnimation(name2), static_cast<Animatable*>(mAnimatable2.get()));

      dtCore::RefPtr<AnimationSequence> clonedSeq = static_cast<AnimationSequence*>(pSeq->Clone(mHelper->GetModelWrapper()).get());
      
      CPPUNIT_ASSERT_EQUAL(pSeq->GetChildAnimations().size(), clonedSeq->GetChildAnimations().size());
      CPPUNIT_ASSERT(clonedSeq.get() == &clonedSeq->GetController()->GetParent());
      
      AnimationChannel* clonedAnim1 = dynamic_cast<AnimationChannel*>(clonedSeq->GetAnimation(animName));
      Animatable* clonedAnim2 = clonedSeq->GetAnimation(name2);
      AnimationSequence* clonedChildSeq = dynamic_cast<AnimationSequence*>(clonedSeq->GetAnimation(childSeqName));

      CPPUNIT_ASSERT(clonedAnim1 != NULL);
      CPPUNIT_ASSERT(clonedAnim2 != NULL);
      CPPUNIT_ASSERT(clonedChildSeq != NULL);

      CPPUNIT_ASSERT_EQUAL(clonedAnim1->GetModel(), mHelper->GetModelWrapper());

      /// The parent of the controller should be the sequence that owns it.
      CPPUNIT_ASSERT(clonedChildSeq == &clonedChildSeq->GetController()->GetParent());

      /// Test the child sequence to see if it was cloned correctly.
      CPPUNIT_ASSERT_EQUAL(pChildSeq->GetChildAnimations().size(), clonedChildSeq->GetChildAnimations().size());
      clonedAnim1 = dynamic_cast<AnimationChannel*>(clonedChildSeq->GetAnimation(animName));
      CPPUNIT_ASSERT(clonedAnim1 != NULL);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The Model wrapper should cascade throughout the cloned animation sequence.", clonedAnim1->GetModel(), mHelper->GetModelWrapper());

      float speed = 2.0f;
      CPPUNIT_ASSERT_MESSAGE("Sequence speed should default to 1.", pSeq->GetSpeed() == 1.0f);
      pSeq->SetSpeed(speed);
      CPPUNIT_ASSERT_MESSAGE("Sequence speed should have been set.", pSeq->GetSpeed() == speed);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimSequenceUpdate()
   {
      const AnimationSequence& rootSeq = mHelper->GetSequenceMixer().GetRootSequence();

      // Test an action.
      mHelper->PlayAnimation("TestEventsAction");
      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().size() == 1);
      float duration = rootSeq.GetChildAnimations().front()->CalculateDuration();
      float timeStep = duration * 0.2f;

      const float epsilon = 0.001f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f,            rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep,        rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 2.0f, rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 3.0f, rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 4.0f, rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep * 0.5f); // at 90% of duration
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 4.5f, rootSeq.GetElapsedTime(), epsilon);
      // --- Action should be pruned.
      mHelper->Update(timeStep * 0.5f); // at 100% of duration where all actions are removed.
      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().empty());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f,            rootSeq.GetElapsedTime(), epsilon);


      // Clear out the animation to text the next animation.
      mHelper->ClearAll(true);
      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().empty());


      // Test a looping animation.
      mHelper->PlayAnimation("Walk");

      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().size() == 1);
      const AnimationChannel* regAnim
         = dynamic_cast<const AnimationChannel*>
         (mHelper->GetSequenceMixer().GetRegisteredAnimation("Walk"));
      CPPUNIT_ASSERT(regAnim != NULL);
      duration = regAnim->GetDuration();
      timeStep = duration * 0.2f;

      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f,            rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep,        rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 2.0f, rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 3.0f, rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 4.0f, rootSeq.GetElapsedTime(), epsilon);
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 5.0f, rootSeq.GetElapsedTime(), epsilon);
      // --- The looping animation should still exist...
      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().size() == 1);
      mHelper->Update(timeStep);
      // --- ...but the sequence time should continue on.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep * 6.0f, rootSeq.GetElapsedTime(), epsilon);

      // Test that the root sequence time resets when the animation is cleared.
      mHelper->ClearAll(0.0f); // This will disable the looping animation.
      mHelper->Update(0.0001f); // This will force a prune of that child animation.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, rootSeq.GetElapsedTime(), epsilon);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimSequenceCalculateDuration()
   {
      float epsilon = 0.001f;

      dtCore::RefPtr<AnimationChannel> anim1 = new AnimationChannel;
      dtCore::RefPtr<AnimationChannel> anim2 = new AnimationChannel;

      anim1->SetName("Anim 1");
      anim1->SetLooping(false);
      anim1->SetDuration(10.0f);
      anim1->SetMaxDuration(0.0f);

      anim2->SetName("Anim 2");
      anim2->SetLooping(false);
      anim2->SetDuration(25.0f);
      anim2->SetMaxDuration(0.0f);
      anim2->SetStartDelay(anim1->GetDuration());

      dtCore::RefPtr<AnimationSequence> seq = new AnimationSequence();
      seq->AddAnimation(anim1.get());
      seq->AddAnimation(anim2.get());

      CPPUNIT_ASSERT(anim1->HasDefiniteEnd());
      CPPUNIT_ASSERT(anim2->HasDefiniteEnd());

      float duration = anim1->GetDuration() + anim2->GetDuration();
      CPPUNIT_ASSERT(seq->HasDefiniteEnd());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, seq->CalculateDuration(), epsilon);
      duration = anim1->CalculateDuration() + anim2->CalculateDuration();
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, seq->CalculateDuration(), epsilon);

      anim1->SetLooping(true);
      CPPUNIT_ASSERT( ! seq->HasDefiniteEnd());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Animatable::INFINITE_TIME, seq->CalculateDuration(), epsilon);

      anim1->SetMaxDuration(5.0f);
      // 2's start delay encompasses the duration of 1.
      duration = anim2->GetStartDelay() + anim2->GetDuration();
      CPPUNIT_ASSERT(seq->HasDefiniteEnd());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, seq->CalculateDuration(), epsilon);

      anim1->SetLooping(false);
      anim2->SetLooping(true);
      CPPUNIT_ASSERT( ! seq->HasDefiniteEnd());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(Animatable::INFINITE_TIME, seq->CalculateDuration(), epsilon);

      anim2->SetMaxDuration(5.0f);
      duration = anim1->GetDuration() + anim2->GetMaxDuration();
      CPPUNIT_ASSERT(seq->HasDefiniteEnd());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration, seq->CalculateDuration(), epsilon);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimInsert()
   {
      SequenceMixer& mixer = mHelper->GetSequenceMixer();
      const AnimationSequence& rootSeq = mixer.GetRootSequence();

      // Play a base animation so that the root sequence's
      // elapsed time is allowed to increment.
      CPPUNIT_ASSERT_EQUAL(size_t(0), rootSeq.GetChildAnimations().size());
      mHelper->PlayAnimation("Idle");
      CPPUNIT_ASSERT_EQUAL(size_t(1), rootSeq.GetChildAnimations().size());


      // Create the test animations.
      const Animatable* regChan = mixer.GetRegisteredAnimation("Walk");
      const Animatable* regSeq = mixer.GetRegisteredAnimation("RunWalk");
      CPPUNIT_ASSERT(regChan != NULL);
      CPPUNIT_ASSERT(regSeq != NULL);

      // Satisfy subsequent calls to Clone.
      BaseModelWrapper* modelWrapper = mHelper->GetModelWrapper();
      
      dtCore::RefPtr<AnimationChannel> chan
         = dynamic_cast<AnimationChannel*>(regChan->Clone(modelWrapper).get());
      dtCore::RefPtr<AnimationSequence> seq
         = dynamic_cast<AnimationSequence*>(regSeq->Clone(modelWrapper).get());

      CPPUNIT_ASSERT(chan.valid());
      CPPUNIT_ASSERT(seq.valid());


      // Declare and set time variables.
      double epsilon = 0.001;
      double timeStep = 3.5;

      float startDelay = 0.5f;
      seq->SetStartDelay(startDelay);
      chan->SetStartDelay(startDelay);


      // Elapse time to a non-zero value.
      double elapsedTime = timeStep;
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(elapsedTime, rootSeq.GetElapsedTime(), epsilon);

      // Ensure the insert time is set on an inserted animation sequence.
      CPPUNIT_ASSERT(chan->GetInsertTime() == 0.0f);
      CPPUNIT_ASSERT( ! chan->IsActive());
      mixer.PlayAnimation(chan.get());
      mixer.Update(0.0f);
      CPPUNIT_ASSERT_EQUAL(size_t(2), rootSeq.GetChildAnimations().size());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(elapsedTime, chan->GetInsertTime(), epsilon);
      CPPUNIT_ASSERT( ! chan->IsActive());


      // Elapse time to a non-zero value.
      elapsedTime += timeStep;
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(elapsedTime, rootSeq.GetElapsedTime(), epsilon);

      // Ensure the insert time is set on an inserted animation sequence.
      CPPUNIT_ASSERT(seq->GetInsertTime() == 0.0f);
      CPPUNIT_ASSERT( ! seq->IsActive());
      mixer.PlayAnimation(seq.get());
      mixer.Update(0.0f);
      CPPUNIT_ASSERT_EQUAL(size_t(3), rootSeq.GetChildAnimations().size());
      CPPUNIT_ASSERT_DOUBLES_EQUAL(elapsedTime, seq->GetInsertTime(), epsilon);
      CPPUNIT_ASSERT( ! seq->IsActive());
      CPPUNIT_ASSERT(chan->IsActive());


      // Elapse time to a non-zero value.
      elapsedTime += timeStep;
      mHelper->Update(timeStep);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(elapsedTime, rootSeq.GetElapsedTime(), epsilon);

      // Ensure that the channel and sequence are active.
      CPPUNIT_ASSERT(chan->IsActive());
      CPPUNIT_ASSERT(seq->IsActive());


      // Ensure that the animations remain active until pruned.
      mixer.ClearAnimation(chan->GetName(), timeStep);
      mixer.ClearAnimation(seq->GetName(), timeStep);
      CPPUNIT_ASSERT(chan->IsActive());
      CPPUNIT_ASSERT(seq->IsActive());

      double halfStep = timeStep * 0.5;
      mHelper->Update(halfStep);
      // TODO: Keep animatables around until they actually complete...
//      CPPUNIT_ASSERT(seq->IsActive());
//      CPPUNIT_ASSERT(chan->IsActive());
//      CPPUNIT_ASSERT(rootSeq.GetAnimation(chan->GetName()) == chan.get());
//      CPPUNIT_ASSERT(rootSeq.GetAnimation(seq->GetName()) == seq.get());
//      CPPUNIT_ASSERT_EQUAL(size_t(3), rootSeq.GetChildAnimations().size());

      // Ensure the animations are pruned.
      mHelper->Update(halfStep + 0.1);
      CPPUNIT_ASSERT( ! seq->IsActive());
      CPPUNIT_ASSERT( ! chan->IsActive());
      CPPUNIT_ASSERT(rootSeq.GetAnimation(chan->GetName()) == NULL);
      CPPUNIT_ASSERT(rootSeq.GetAnimation(seq->GetName()) == NULL);
      CPPUNIT_ASSERT_EQUAL(size_t(1), rootSeq.GetChildAnimations().size());
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimRelativeTime()
   {
      const AnimationSequence& rootSeq = mHelper->GetSequenceMixer().GetRootSequence();

      const float epsilon = 0.001f;


      // Test a channel (Action)
      const Animatable* regAnim = mHelper->GetSequenceMixer().GetRegisteredAnimation("RunAction");
      CPPUNIT_ASSERT(regAnim != NULL);

      mHelper->PlayAnimation("RunAction");

      dtCore::ObserverPtr<AnimationChannel> action
         = dynamic_cast<AnimationChannel*>(rootSeq.GetChildAnimations().front().get());
      CPPUNIT_ASSERT(action != NULL);
      CPPUNIT_ASSERT( ! action->IsLooping());
      float duration = action->GetAnimation()->GetDuration();
      CPPUNIT_ASSERT(duration > 0.0f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, action->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(0.1f); // now at 0.1 + start Time
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1f, action->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(duration - 0.11f); // now at (duration - 0.01) + start time
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration - 0.01f, action->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(0.2f);
      CPPUNIT_ASSERT( ! action.valid());


      // Test a channel (Looping)
      mHelper->ClearAll(0.0f);
      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().empty());
      regAnim = mHelper->GetSequenceMixer().GetRegisteredAnimation("Idle");
      CPPUNIT_ASSERT(regAnim != NULL);

      mHelper->PlayAnimation("Idle");

      AnimationChannel* loopAnim
         = dynamic_cast<AnimationChannel*>(rootSeq.GetChildAnimations().front().get());
      CPPUNIT_ASSERT(loopAnim != NULL);
      CPPUNIT_ASSERT(loopAnim->IsLooping());
      loopAnim->SetStartTime(0.1f);
      float startTime = loopAnim->GetStartTime();
      CPPUNIT_ASSERT(startTime > 0.0f);
      duration = loopAnim->GetAnimation()->GetDuration();
      CPPUNIT_ASSERT(duration > 0.0f);

      float timeStep = duration * 0.5f;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(startTime);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(timeStep); // now at duration * 0.5
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep, loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(timeStep); // now at duration * 1.0
      // --- There may be some rounding error on the boundary of duration-to-0.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(loopAnim->GetRelativeElapsedTimeInAnimationScope() > 0.0f ? duration : 0.0f,
         loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(timeStep); // now at duration * 1.5
      CPPUNIT_ASSERT_DOUBLES_EQUAL(timeStep, loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(timeStep); // now at duration * 2.0
      // --- There may be some rounding error on the boundary of duration-to-0.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(loopAnim->GetRelativeElapsedTimeInAnimationScope() > 0.0f ? duration : 0.0f,
         loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(duration * 0.75f); // now at duration * 2.75
      CPPUNIT_ASSERT_DOUBLES_EQUAL(duration * 0.75f, loopAnim->GetRelativeElapsedTimeInAnimationScope(), epsilon);


      // Test a sequence.
      mHelper->ClearAll(0.0f); // This will disable the preceding looping animation.
      mHelper->Update(0.0001f); // This will force a prune of that animation.
      CPPUNIT_ASSERT(rootSeq.GetChildAnimations().empty());
      regAnim = mHelper->GetSequenceMixer().GetRegisteredAnimation("RunWalk");
      mHelper->PlayAnimation("RunWalk");
      CPPUNIT_ASSERT(regAnim != NULL);
      Animatable* seq = rootSeq.GetChildAnimations().front();

      startTime = seq->GetStartTime();
      CPPUNIT_ASSERT(startTime > 0.0f);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, seq->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(0.05f); // now at 0.05
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, seq->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(0.05f); // now at 0.1
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, seq->GetRelativeElapsedTimeInAnimationScope(), epsilon);

      // Ensure that the sequence now updates its relative elapsed time.
      mHelper->Update(0.1f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1f, seq->GetRelativeElapsedTimeInAnimationScope(), epsilon);
      mHelper->Update(0.1f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2f, seq->GetRelativeElapsedTimeInAnimationScope(), epsilon);

      // Go to some far offset time to ensure the elapsed time continues on.
      mHelper->Update(10.0f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(10.2f, seq->GetRelativeElapsedTimeInAnimationScope(), epsilon);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestSequenceMixer()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimController()
   {      
      std::ostringstream ss;

      //test fade in and out
      dtCore::RefPtr<TestSequence> seq = new TestSequence();
      dtCore::RefPtr<TestController> cont = new TestController(*seq);
      CPPUNIT_ASSERT(&cont->GetParent() == seq.get());
      dtCore::RefPtr<TestAnimatable> anim = new TestAnimatable();

      //first pass, this one will be the easy base case
      seq->SetCurrentWeight(1.0f);

      anim->SetStartTime2(0.0f);
      anim->SetEndTime2(5.0f);
      anim->SetFadeIn2(1.0f);
      anim->SetFadeOut2(1.0f);
      anim->SetBaseWeight2(1.0f);

      //if we arent playing it should be 0.0
      anim->SetElapsedTime2(0.0f);
      float expected = 0.0f;
      float result = cont->TestComputeWeight(anim);

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      //with a one second fade in, the weight should be one half at 0.5 seconds
      anim->SetElapsedTime2(0.5f);
      result = cont->TestComputeWeight(anim);
      expected = 0.5f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));


      anim->SetElapsedTime2(0.75f);
      result = cont->TestComputeWeight(anim);
      expected = 0.75f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      //we should have completely faded in by now
      anim->SetElapsedTime2(2.0f);
      result = cont->TestComputeWeight(anim);
      expected = 1.0f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      anim->SetElapsedTime2(3.0);
      result = cont->TestComputeWeight(anim);
      expected = 1.0f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      //we should now start fading out
      anim->SetElapsedTime2(4.1f);
      result = cont->TestComputeWeight(anim);
      expected = 0.9f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      anim->SetElapsedTime2(4.5f);
      result = cont->TestComputeWeight(anim);
      expected = 0.5f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      //we should have just finished fading out by now
      anim->SetElapsedTime2(5.0f);
      result = cont->TestComputeWeight(anim);
      expected = 0.0f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      //lets try some bogus values
      anim->SetElapsedTime2(15.0f);
      result = cont->TestComputeWeight(anim);
      expected = 0.0f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));

      anim->SetElapsedTime2(-15.0f);
      result = cont->TestComputeWeight(anim);
      expected = 0.0f;

      SetExpectedString(ss, expected, result);
      CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expected, result));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimHelper()
   {
      std::string animName = "Walk";

      SequenceMixer& mixer = mHelper->GetSequenceMixer();
      const Animatable* anim = mixer.GetRegisteredAnimation(animName);
      CPPUNIT_ASSERT(anim != NULL);

      mHelper->PlayAnimation(animName);

      Animatable* activeAnim = mixer.GetActiveAnimation(animName);

      CPPUNIT_ASSERT_EQUAL(animName, activeAnim->GetName());

      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      activeAnim->SetStartDelay(3.0f);
      mixer.ForceRecalculate();
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      mHelper->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      mHelper->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      mHelper->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(true, activeAnim->IsActive());

      CPPUNIT_ASSERT_EQUAL(3.0, activeAnim->GetElapsedTime());

      dtCore::RefPtr<AnimationChannel> activeChannel = dynamic_cast<AnimationChannel*>(activeAnim);
      CPPUNIT_ASSERT(activeChannel.valid());

      activeChannel->SetMaxDuration(3.5f);
      mixer.ForceRecalculate();
      mHelper->Update(3.5f);

      CPPUNIT_ASSERT_EQUAL(false, activeChannel->IsActive());

      CPPUNIT_ASSERT(mixer.GetActiveAnimation(animName) == 0); 

      ////OK, so far so good, lets try testing looping vs non looping
      //mHelper->PlayAnimation(animName);
      //activeAnim = mixer.GetActiveAnimation(animName);
      //CPPUNIT_ASSERT(activeAnim);
      //activeChannel = dynamic_cast<AnimationChannel*>(activeAnim);
      //CPPUNIT_ASSERT(activeChannel.valid());

      //activeChannel->SetLooping(false);


   }

   /////////////////////////////////////////////////////////////////////////////
   // Test Helper Class
   struct CallbackTester 
   {
      typedef dtUtil::Functor<void,TYPELIST_0()> CallbackTypeA;
      typedef dtUtil::Functor<void,TYPELIST_1(int)> CallbackTypeB;
      typedef dtUtil::Functor<void,TYPELIST_2(int, int)> CallbackTypeC;
      typedef dtUtil::Functor<void,TYPELIST_1(Animatable*)> CallbackTypeD;

      typedef dtUtil::Command0<void> CommandTypeA;
      typedef dtUtil::Command1<void, int> CommandTypeB;
      typedef dtUtil::Command2<void, int, int> CommandTypeC;
      typedef dtAnim::AnimReferenceCommandCallback CommandTypeD;

      CallbackTester()
         : mCalledA(0)
         , mCalledB(0)
         , mCalledC(0)
         , mCalledD(0)
      {}

      void Reset()
      {
         mCalledA = 0;
         mCalledB = 0;
         mCalledC = 0;
         mCalledD = 0;
         mCalledAnims.clear();
         mCalledEventNames.clear();
      }

      bool HasBeenCalled() const
      {
         return mCalledA > 0
            || mCalledB > 0
            || mCalledC > 0
            || mCalledD > 0
            || ! mCalledEventNames.empty();
      }

      void OnCallA()
      {
         ++mCalledA;
      }

      void OnCallB(int arg1)
      {
         ++mCalledB;
      }

      void OnCallC(int arg1, int arg2)
      {
         ++mCalledC;
      }

      void OnCallD(Animatable* anim)
      {
         ++mCalledD;
         mCalledAnims.push_back(anim);
      }

      void SendEvent(const std::string& eventName)
      {
         mCalledEventNames.push_back(eventName);
      }

      int mCalledA;
      int mCalledB;
      int mCalledC;
      int mCalledD;
      std::vector<Animatable*> mCalledAnims;
      std::vector<std::string> mCalledEventNames;
   };

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimHelperCustomCommands()
   {
      // Ensure command execution is defaulted to off and is set enabled
      // for the subsequent tests.
      CPPUNIT_ASSERT( ! mHelper->GetCommandCallbacksEnabled());
      mHelper->SetCommandCallbacksEnabled(true);
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacksEnabled());

      // Create test variables
      const std::string testAnim("TestEventsAction");
      mHelper->PlayAnimation(testAnim);
      Animatable* activeAnim = mHelper->GetSequenceMixer().GetActiveAnimation(testAnim);
      CPPUNIT_ASSERT(activeAnim != NULL);
      // --- Tick the animation stuff slightly so that the animation
      //     that was set to play will become active.
      mHelper->Update(0.1f);

      CallbackTester cbt;
      CallbackTester::CallbackTypeA funcA(&cbt, &CallbackTester::OnCallA);
      CallbackTester::CallbackTypeB funcB(&cbt, &CallbackTester::OnCallB);
      CallbackTester::CallbackTypeC funcC(&cbt, &CallbackTester::OnCallC);
      CallbackTester::CallbackTypeD funcD(&cbt, &CallbackTester::OnCallD);
      
      dtCore::RefPtr<CallbackTester::CommandTypeA> cmdA
         = new CallbackTester::CommandTypeA(funcA);
      
      dtCore::RefPtr<CallbackTester::CommandTypeB> cmdB
         = new CallbackTester::CommandTypeB(funcB, 123);
      
      dtCore::RefPtr<CallbackTester::CommandTypeC> cmdC
         = new CallbackTester::CommandTypeC(funcC, -456, 678);

      dtCore::RefPtr<CallbackTester::CommandTypeD> cmdD
         = new CallbackTester::CommandTypeD(funcD, NULL);


      // Test setting the crucial event sending callback.
      AnimEventCallback eventFunc(&cbt, &CallbackTester::SendEvent);
      CPPUNIT_ASSERT( ! mHelper->GetSendEventCallback().valid());
      mHelper->SetSendEventCallback(eventFunc);
      CPPUNIT_ASSERT(mHelper->GetSendEventCallback().valid());


      // Test accessing the duration of the animation.
      float duration = mHelper->GetAnimationDuration(testAnim);
      CPPUNIT_ASSERT(duration > 0.0f);
      CPPUNIT_ASSERT(mHelper->GetAnimationDuration("") == 0.0f);


      // Remove objects that are not needed in this test.
      // --- Animation Event Callbacks will be tested later,
      //     in another test method.
      CPPUNIT_ASSERT(mHelper->ClearAnimationEventCallbacks() == TOTAL_TEST_EVENTS);
      CPPUNIT_ASSERT(mHelper->ClearAnimationEventCallbacks() == 0);


      // Test registering the commands.
      CPPUNIT_ASSERT(mHelper->RegisterCommandCallback(testAnim, 0.0f, *cmdC));
      CPPUNIT_ASSERT(mHelper->RegisterCommandCallback(testAnim, 0.5f, *cmdB));
      CPPUNIT_ASSERT(mHelper->RegisterCommandCallback(testAnim, duration, *cmdA));
      CPPUNIT_ASSERT(mHelper->RegisterCommandCallback(testAnim, -1.0f, *cmdC)); // Negative number sets to animation end.
      CPPUNIT_ASSERT(mHelper->RegisterCommandCallback(testAnim, duration, *cmdD));

      // Ensure registering the same commands for the same time offsets fails.
      CPPUNIT_ASSERT( ! mHelper->RegisterCommandCallback(testAnim, 0.0f, *cmdC));
      CPPUNIT_ASSERT( ! mHelper->RegisterCommandCallback(testAnim, 0.5f, *cmdB));
      CPPUNIT_ASSERT( ! mHelper->RegisterCommandCallback(testAnim, duration, *cmdA));
      CPPUNIT_ASSERT( ! mHelper->RegisterCommandCallback(testAnim, duration, *cmdC)); // Previous registration with a negative value inserted at the end (duration).
      CPPUNIT_ASSERT( ! mHelper->RegisterCommandCallback(testAnim, duration, *cmdD));

      // --- Ensure that nothing was triggered.
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());

      // Test accessing the commands.
      AnimCommandArray cmdArray;
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.0f, cmdArray) == 1);
      CPPUNIT_ASSERT(cmdArray.size() == 1);
      CPPUNIT_ASSERT(cmdArray[0] == cmdC);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.5f, cmdArray) == 1);
      CPPUNIT_ASSERT(cmdArray.size() == 1);
      CPPUNIT_ASSERT(cmdArray[0] == cmdB);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, duration, cmdArray) == 3);
      CPPUNIT_ASSERT(cmdArray.size() == 3);
      CPPUNIT_ASSERT(cmdArray[0] == cmdA);
      CPPUNIT_ASSERT(cmdArray[1] == cmdC);
      CPPUNIT_ASSERT(cmdArray[2] == cmdD);
      cmdArray.clear();

      // Ensure invalid times return none commands.
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, -1.0f, cmdArray) == 0);
      CPPUNIT_ASSERT(cmdArray.empty());

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.25f, cmdArray) == 0);
      CPPUNIT_ASSERT(cmdArray.empty());

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.75f, cmdArray) == 0);
      CPPUNIT_ASSERT(cmdArray.empty());

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 1.25f, cmdArray) == 0);
      CPPUNIT_ASSERT(cmdArray.empty());

      // --- Ensure that nothing was triggered.
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());


      // Test accessing a range of commands.
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.0f, duration, cmdArray) == 5);
      CPPUNIT_ASSERT(cmdArray.size() == 5);
      CPPUNIT_ASSERT(cmdArray[0] == cmdC);
      CPPUNIT_ASSERT(cmdArray[1] == cmdB);
      CPPUNIT_ASSERT(cmdArray[2] == cmdA);
      CPPUNIT_ASSERT(cmdArray[3] == cmdC);
      CPPUNIT_ASSERT(cmdArray[4] == cmdD);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, -0.1f, 0.1f, cmdArray) == 1);
      CPPUNIT_ASSERT(cmdArray.size() == 1);
      CPPUNIT_ASSERT(cmdArray[0] == cmdC);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.4f, 0.6f, cmdArray) == 1);
      CPPUNIT_ASSERT(cmdArray.size() == 1);
      CPPUNIT_ASSERT(cmdArray[0] == cmdB);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.9f, duration + 0.1f, cmdArray) == 3);
      CPPUNIT_ASSERT(cmdArray.size() == 3);
      CPPUNIT_ASSERT(cmdArray[0] == cmdA);
      CPPUNIT_ASSERT(cmdArray[1] == cmdC);
      CPPUNIT_ASSERT(cmdArray[2] == cmdD);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, -0.1f, 0.6f, cmdArray) == 2);
      CPPUNIT_ASSERT(cmdArray.size() == 2);
      CPPUNIT_ASSERT(cmdArray[0] == cmdC);
      CPPUNIT_ASSERT(cmdArray[1] == cmdB);
      cmdArray.clear();

      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.4f, duration + 0.1f, cmdArray) == 4);
      CPPUNIT_ASSERT(cmdArray.size() == 4);
      CPPUNIT_ASSERT(cmdArray[0] == cmdB);
      CPPUNIT_ASSERT(cmdArray[1] == cmdA);
      CPPUNIT_ASSERT(cmdArray[2] == cmdC);
      CPPUNIT_ASSERT(cmdArray[3] == cmdD);
      cmdArray.clear();

      // --- Ensure that nothing was triggered.
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());


      // Test queuing up commands.
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 5); // ALL
      // --- Ensure a another call does not build up the list unnecessarily.
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 5);
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 5);

      CPPUNIT_ASSERT(mHelper->CollectCommands(-0.1f, 0.1f) == 1); // C
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 1);

      CPPUNIT_ASSERT(mHelper->CollectCommands(0.4f, 0.6f) == 1); // B
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 1);

      CPPUNIT_ASSERT(mHelper->CollectCommands(0.9f, duration + 0.1f) == 3); // A, C & D
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 3);

      CPPUNIT_ASSERT(mHelper->CollectCommands(0.4f, duration + 0.1f) == 4); // B, A, C & D
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 4);

      CPPUNIT_ASSERT(mHelper->CollectCommands(-0.1f, 0.6f) == 2); // C & B
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 2);

      // --- Ensure that nothing was triggered.
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());


      // --- Test disabling command collecting.
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 2);
      
      mHelper->SetCommandCallbacksEnabled(false);
      // ------ Ensure disabling commands clears current command list.
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 0);
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 0); // ALL
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 0);
      
      mHelper->SetCommandCallbacksEnabled(true);
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 5); // ALL


      // Test triggering the commands.
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 5); // ALL
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 5);
       // The collected command list should be cleared after being executed.
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 0);
      CPPUNIT_ASSERT(cbt.HasBeenCalled());
      CPPUNIT_ASSERT(cbt.mCalledA == 1);
      CPPUNIT_ASSERT(cbt.mCalledB == 1);
      CPPUNIT_ASSERT(cbt.mCalledC == 2);
      CPPUNIT_ASSERT(cbt.mCalledD == 1);
      // --- Test special case D
      CPPUNIT_ASSERT(cbt.mCalledAnims.size() == 1);
      CPPUNIT_ASSERT(cbt.mCalledAnims[0] != NULL);
      CPPUNIT_ASSERT(cbt.mCalledAnims[0]->GetName() == testAnim);
      cbt.Reset();

      CPPUNIT_ASSERT(mHelper->CollectCommands(-0.1f, 0.1f) == 1); // C
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 1);
      CPPUNIT_ASSERT(cbt.mCalledA == 0);
      CPPUNIT_ASSERT(cbt.mCalledB == 0);
      CPPUNIT_ASSERT(cbt.mCalledC == 1);
      CPPUNIT_ASSERT(cbt.mCalledD == 0);
      // --- Test special case D
      CPPUNIT_ASSERT(cbt.mCalledAnims.empty());
      cbt.Reset();

      CPPUNIT_ASSERT(mHelper->CollectCommands(0.4f, 0.6f) == 1); // B
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 1);
      CPPUNIT_ASSERT(cbt.mCalledA == 0);
      CPPUNIT_ASSERT(cbt.mCalledB == 1);
      CPPUNIT_ASSERT(cbt.mCalledC == 0);
      CPPUNIT_ASSERT(cbt.mCalledD == 0);
      // --- Test special case D
      CPPUNIT_ASSERT(cbt.mCalledAnims.empty());
      cbt.Reset();

      CPPUNIT_ASSERT(mHelper->CollectCommands(0.9f, duration + 0.1f) == 3); // A, C & D
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 3);
      CPPUNIT_ASSERT(cbt.mCalledA == 1);
      CPPUNIT_ASSERT(cbt.mCalledB == 0);
      CPPUNIT_ASSERT(cbt.mCalledC == 1);
      CPPUNIT_ASSERT(cbt.mCalledD == 1);
      // --- Test special case D
      CPPUNIT_ASSERT(cbt.mCalledAnims.size() == 1);
      CPPUNIT_ASSERT(cbt.mCalledAnims[0] != NULL);
      CPPUNIT_ASSERT(cbt.mCalledAnims[0]->GetName() == testAnim);
      cbt.Reset();

      CPPUNIT_ASSERT(mHelper->CollectCommands(0.4f, duration + 0.1f) == 4); // B, A, C & D
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 4);
      CPPUNIT_ASSERT(cbt.mCalledA == 1);
      CPPUNIT_ASSERT(cbt.mCalledB == 1);
      CPPUNIT_ASSERT(cbt.mCalledC == 1);
      CPPUNIT_ASSERT(cbt.mCalledD == 1);
      // --- Test special case D
      CPPUNIT_ASSERT(cbt.mCalledAnims.size() == 1);
      CPPUNIT_ASSERT(cbt.mCalledAnims[0] != NULL);
      CPPUNIT_ASSERT(cbt.mCalledAnims[0]->GetName() == testAnim);
      cbt.Reset();

      CPPUNIT_ASSERT(mHelper->CollectCommands(-0.1f, 0.6f) == 2); // C & B
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 2);
      CPPUNIT_ASSERT(cbt.mCalledA == 0);
      CPPUNIT_ASSERT(cbt.mCalledB == 1);
      CPPUNIT_ASSERT(cbt.mCalledC == 1);
      CPPUNIT_ASSERT(cbt.mCalledD == 0);
      // --- Test special case D
      CPPUNIT_ASSERT(cbt.mCalledAnims.empty());
      cbt.Reset();


      // Test unregistering a specific command at a specific time for a specific animation.
      CPPUNIT_ASSERT(mHelper->UnregisterCommandCallback(testAnim, duration, cmdC.get()) == 1);
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, duration, cmdArray) == 2);
      CPPUNIT_ASSERT(cmdArray.size() == 2);
      CPPUNIT_ASSERT(cmdArray[0] == cmdA);
      CPPUNIT_ASSERT(cmdArray[1] == cmdD);
      cmdArray.clear();

      // --- Re-register it to test a different type of removal.
      CPPUNIT_ASSERT(mHelper->RegisterCommandCallback(testAnim, duration, *cmdC) == 1);
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, duration, cmdArray) == 3);
      CPPUNIT_ASSERT(cmdArray.size() == 3);
      CPPUNIT_ASSERT(cmdArray[0] == cmdA);
      CPPUNIT_ASSERT(cmdArray[1] == cmdD);
      CPPUNIT_ASSERT(cmdArray[2] == cmdC);
      cmdArray.clear();

      // --- Ensure the other commands are unaffected.
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.0f, 0.5f, cmdArray) == 2);
      CPPUNIT_ASSERT(cmdArray.size() == 2);
      CPPUNIT_ASSERT(cmdArray[0] == cmdC);
      CPPUNIT_ASSERT(cmdArray[1] == cmdB);
      cmdArray.clear();

      // Test unregistering all commands at a specific time for a specific animation.
      CPPUNIT_ASSERT(mHelper->UnregisterCommandCallback(testAnim, duration, NULL) == 3);
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, duration, cmdArray) == 0);
      CPPUNIT_ASSERT(cmdArray.empty());

      // --- Ensure that nothing was triggered.
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());

      // Test removing remaining commands for a specific animation.
      CPPUNIT_ASSERT(mHelper->UnregisterCommandCallbacks(testAnim) == 2); // C & B
      // --- Ensure all have been removed
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacks(testAnim, 0.0f, duration, cmdArray) == 0);
      // --- Ensure that nothing was triggered.
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimHelperEventCommands_Channel()
   {
      // Ensure command execution is enabled for the subsequent tests.
      mHelper->SetCommandCallbacksEnabled(true);
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacksEnabled());

      SubTestAnimHelperEventCommands("TestEventsAction", "");
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimHelperEventCommands_Sequence()
   {
      // Ensure command execution is enabled for the subsequent tests.
      mHelper->SetCommandCallbacksEnabled(true);
      CPPUNIT_ASSERT(mHelper->GetCommandCallbacksEnabled());

      SubTestAnimHelperEventCommands("ActionSequence2", "seq_");
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::SubTestAnimHelperEventCommands(const std::string& testAnim,
      const std::string& eventPrefix)
   {
      // Create test variables
      mHelper->PlayAnimation(testAnim);
      const Animatable* regAnim = mHelper->GetSequenceMixer().GetRegisteredAnimation(testAnim);
      Animatable* activeAnim = mHelper->GetSequenceMixer().GetActiveAnimation(testAnim);
      CPPUNIT_ASSERT(regAnim != NULL);
      CPPUNIT_ASSERT(activeAnim != NULL);

      CallbackTester cbt;

      // Maintain the names of the anticipated test events.
      const std::string eventStart1(eventPrefix+"startEvent1");
      const std::string eventStart2(eventPrefix+"startEvent2");
      const std::string eventStart3(eventPrefix+"startEvent3");
      const std::string eventMid1(eventPrefix+"midEvent1");
      const std::string eventMid2(eventPrefix+"midEvent2");
      const std::string eventMid3(eventPrefix+"midEvent3");
      const std::string eventEnd1(eventPrefix+"endEvent1");
      const std::string eventEnd2(eventPrefix+"endEvent2");
      const std::string eventEnd3(eventPrefix+"endEvent3");
      float duration = activeAnim->CalculateDuration();


      // Test setting the crucial event sending callback.
      AnimEventCallback eventFunc(&cbt, &CallbackTester::SendEvent);
      CPPUNIT_ASSERT( ! mHelper->GetSendEventCallback().valid());
      CPPUNIT_ASSERT(mHelper->SetSendEventCallback(eventFunc) == TOTAL_TEST_EVENTS); // TOTAL_TEST_EVENTS Event commands created.
      CPPUNIT_ASSERT(mHelper->GetSendEventCallback().valid());

      // Ensure the events for the animation have been loaded and set
      // on the registered animation and not the copied active animation.
      Animatable::EventNameArray eventArray;
      CPPUNIT_ASSERT_EQUAL(3U, regAnim->GetEvents(0.0f, eventArray));
      CPPUNIT_ASSERT_EQUAL(3U, unsigned(eventArray.size()));
      CPPUNIT_ASSERT(eventArray[0] == eventStart1);
      CPPUNIT_ASSERT(eventArray[1] == eventStart2);
      CPPUNIT_ASSERT(eventArray[2] == eventStart3);
      eventArray.clear();

      CPPUNIT_ASSERT_EQUAL(2U, regAnim->GetEvents(0.25f, eventArray));
      CPPUNIT_ASSERT_EQUAL(2U, unsigned(eventArray.size()));
      CPPUNIT_ASSERT(eventArray[0] == eventMid1);
      CPPUNIT_ASSERT(eventArray[1] == eventMid2);
      eventArray.clear();

      CPPUNIT_ASSERT_EQUAL(1U, regAnim->GetEvents(0.5f, eventArray));
      CPPUNIT_ASSERT_EQUAL(1U, unsigned(eventArray.size()));
      CPPUNIT_ASSERT(eventArray[0] == eventMid3);
      eventArray.clear();

      CPPUNIT_ASSERT_EQUAL(3U, regAnim->GetEvents(duration, eventArray));
      CPPUNIT_ASSERT_EQUAL(3U, unsigned(eventArray.size()));
      CPPUNIT_ASSERT(eventArray[0] == eventEnd1);
      CPPUNIT_ASSERT(eventArray[1] == eventEnd2);
      CPPUNIT_ASSERT(eventArray[2] == eventEnd3);
      eventArray.clear();

      // --- Ensure the active animation (copied from the registered animation)
      //     does not have copies of the event information, for the sake of memory.
      CPPUNIT_ASSERT(activeAnim->GetEvents(0.0f, eventArray) == 0);
      CPPUNIT_ASSERT(eventArray.empty());
      CPPUNIT_ASSERT(activeAnim->GetEvents(0.25f, eventArray) == 0);
      CPPUNIT_ASSERT(eventArray.empty());
      CPPUNIT_ASSERT(activeAnim->GetEvents(0.5f, eventArray) == 0);
      CPPUNIT_ASSERT(eventArray.empty());
      CPPUNIT_ASSERT(activeAnim->GetEvents(duration, eventArray) == 0);
      CPPUNIT_ASSERT(eventArray.empty());
      
      // --- Tick the animation slightly so that the animation
      //     that was set to play will become active.
      mHelper->Update(0.1f);

      // Determine if the events have been triggered.
      // --- Since the active animation was made active by ticking
      //     the helper at the beginning of this method, the start
      //     events should have not been triggered, but rather
      //     queued up as event commands.
      std::vector<std::string>& cbtEvents = cbt.mCalledEventNames;
      CPPUNIT_ASSERT( ! cbt.HasBeenCalled());
      CPPUNIT_ASSERT(cbtEvents.size() == 0); // Ensure the callback tester is not broken.

      // --- Now execute those stored commands for the start events.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 3); // 3 start events at time 0.0.
      CPPUNIT_ASSERT(cbtEvents.size() == 3);
      CPPUNIT_ASSERT(cbtEvents[0] == eventStart1);
      CPPUNIT_ASSERT(cbtEvents[1] == eventStart2);
      CPPUNIT_ASSERT(cbtEvents[2] == eventStart3);
      cbt.Reset();

      // --- Test the area of the time line where no events should execute.
      mHelper->Update(0.1f); // now at 0.2 time.
      // --- No event commands should have been queued.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 0);
      CPPUNIT_ASSERT(cbtEvents.empty());
      cbt.Reset();

      // --- Test the area of the time line where 2 events should execute.
      mHelper->Update(0.2f); // now at 0.4 time.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 2); // 2 mid events at time 0.25
      CPPUNIT_ASSERT(cbtEvents.size() == 2);
      CPPUNIT_ASSERT(cbtEvents[0] == eventMid1);
      CPPUNIT_ASSERT(cbtEvents[1] == eventMid2);
      cbt.Reset();

      // --- Test the area of the time line where 1 event should execute.
      //     Ending on the exact time should trigger the event
      mHelper->Update(0.1f); // now at 0.5 time.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 1); // 1 mid event at time 0.5
      CPPUNIT_ASSERT(cbtEvents.size() == 1);
      CPPUNIT_ASSERT(cbtEvents[0] == eventMid3);
      cbt.Reset();

      // --- Test the area of the time line where no events should execute.
      //     Starting on the precise non-zero time (when the event was fired
      //     last frame) should not trigger it again since the time step range
      //     start should be exclusive--which means an event's offset will
      //     trigger the event if: offset > startTimeStep && offset <= endTimeStep.
      mHelper->Update(0.2f); // now at 0.7 time.
      // --- No event commands should have been queued.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 0);
      CPPUNIT_ASSERT(cbtEvents.empty());
      cbt.Reset();

      // --- Test the end of the time line where 3 events should execute.
      //     If the event time offset is at the precise end of the animation
      //     for the current update, then it should be triggered before the
      //     animation is removed from the root sequence.
      mHelper->Update(duration - 0.7f); // now at duration time.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 3); // 3 end event at time 1.0
      CPPUNIT_ASSERT(cbtEvents.size() == 3);
      CPPUNIT_ASSERT(cbtEvents[0] == eventEnd1);
      CPPUNIT_ASSERT(cbtEvents[1] == eventEnd2);
      CPPUNIT_ASSERT(cbtEvents[2] == eventEnd3);
      cbt.Reset();

      // --- Test the area of the time line where no events should execute.
      mHelper->Update(0.2f); // now at duration + 0.2 time.
      // --- No event commands should have been queued.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 0);
      CPPUNIT_ASSERT(cbtEvents.empty());
      cbt.Reset();


      // Test re-starting the animation to ensure events will still fire again.
      mHelper->PlayAnimation(testAnim);
      mHelper->Update(0.2f); // now back at 0.2 time.
      // --- Ensure the start events fire again.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 3); // 3 start events at time 0.0.
      CPPUNIT_ASSERT(cbtEvents.size() == 3);
      CPPUNIT_ASSERT(cbtEvents[0] == eventStart1);
      CPPUNIT_ASSERT(cbtEvents[1] == eventStart2);
      CPPUNIT_ASSERT(cbtEvents[2] == eventStart3);
      cbt.Reset();


      // Test that disabling command callbacks will prevent events from firing.
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 9);
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 9);
      mHelper->SetCommandCallbacksEnabled(false);
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 0);
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 0);
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 0);
      mHelper->SetCommandCallbacksEnabled(true);
      CPPUNIT_ASSERT(mHelper->CollectCommands(0.0f, duration) == 9);
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 9);


      // Test clearing the event callbacks that were automatically generated
      // by the helper.
      CPPUNIT_ASSERT(mHelper->ClearAnimationEventCallbacks() == TOTAL_TEST_EVENTS);
      CPPUNIT_ASSERT(mHelper->GetCollectedCommandCount() == 0);
      CPPUNIT_ASSERT(mHelper->ClearAnimationEventCallbacks() == 0);


      // Finally, ensure no events are fired even though the event callbacks
      // have been declared to have been removed.
      mHelper->Update(0.4f); // now back at 0.6 time.
      CPPUNIT_ASSERT(mHelper->ExecuteCommands() == 0); // 1 mid event at time 0.5
      CPPUNIT_ASSERT(cbtEvents.empty());
      cbt.Reset();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::TestAnimCallback()
   {
      dtAnim::AnimationCallback animEndCallback(this, &AnimationTests::OnAnimationCompleted);
      mAnimatable1->SetEndCallback(animEndCallback);

      CPPUNIT_ASSERT(mLastAnimatableCompleted == NULL);
      mAnimatable1->Prune();
      CPPUNIT_ASSERT(mLastAnimatableCompleted == mAnimatable1.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   void AnimationTests::OnAnimationCompleted(const dtAnim::Animatable& anim)
   {
      mLastAnimatableCompleted = &anim;
   }

}
