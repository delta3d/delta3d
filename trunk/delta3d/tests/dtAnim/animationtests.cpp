/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
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
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/animationwrapper.h>
#include <dtAnim/animatable.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationhelper.h>
#include <dtCore/globals.h>
#include <dtCore/refptr.h>

#include <osg/Math>
#include <strstream>

#include <string>

namespace dtAnim
{

      class TestAnimatable: public Animatable
      {
      public:
         TestAnimatable(){}
         TestAnimatable(const TestAnimatable& anim):Animatable(anim){}
         TestAnimatable& operator=(const TestAnimatable& anim){Animatable::operator=(anim); return *this;}

         void Update(float dt){};
         void ForceFadeOut(float time){}; 
         void Recalculate(){}
         dtCore::RefPtr<Animatable> Clone()const{return new TestAnimatable(*this);}
         void Prune(){}

         void SetStartTime2(float animStart1){ SetStartTime(animStart1);}
         void SetEndTime2(float animEnd1){ SetEndTime(animEnd1);}
         void SetFadeIn2(float fadeIn1){ SetFadeIn(fadeIn1);}
         void SetFadeOut2(float fadeOut1){ SetFadeOut(fadeOut1);}
         void SetBaseWeight2(float base1){ SetBaseWeight(base1);}
         void SetName2(const std::string& name1){ SetName(name1);}
         void SetStartDelay2(float start_delay1){ SetStartDelay(start_delay1);}
         
         void SetElapsedTime2(float time){ SetElapsedTime(time);}
      };

      class TestSequence: public AnimationSequence
      {
      };


      class TestController: public AnimationSequence::AnimationController
      {
         public:
            TestController(AnimationSequence* pSeq): AnimationController(pSeq) {}
            float TestComputeWeight(dtCore::RefPtr<TestAnimatable> anim)
            {
               SetComputeWeight(anim.get());
               return anim->GetCurrentWeight();
            }
      };



   class AnimationTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( AnimationTests );
         CPPUNIT_TEST( TestAnimWrapper );
         CPPUNIT_TEST( UnitTestAnimatable );
         CPPUNIT_TEST( TestAnimChannel );
         CPPUNIT_TEST( TestAnimSequence );
         CPPUNIT_TEST( TestSequenceMixer );
         CPPUNIT_TEST( TestAnimHelper );
         CPPUNIT_TEST( TestAnimController );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestAnimWrapper(); 
         void UnitTestAnimatable(); 
         void TestAnimChannel();
         void TestAnimSequence();         
         void TestSequenceMixer();
         void TestAnimController();
         void TestAnimHelper();

      private:

         template <typename T>
         void SetExpectedString(std::ostringstream& strstream, T expected, T result)
         {
            strstream.str("");
            strstream << "Expected \"" << expected << "\" but got \"" << result << "\"";
         }

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

   void AnimationTests::setUp()
   {      
      mHelper = new AnimationHelper();

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

   void AnimationTests::tearDown()
   {
      mAnimatable1 = NULL;
      mAnimatable2 = NULL;
      mHelper = NULL;
   }


   void AnimationTests::TestAnimWrapper()
   {
      std::string wrapperName("MyWrapper");
      int wrapperID = 101;
      float duration = 1000.0f;
      float speed = 2.0f;
      
      dtCore::RefPtr<AnimationWrapper> wrapper = new AnimationWrapper(wrapperName, wrapperID);

      wrapper->SetDuration(duration);
      wrapper->SetSpeed(speed);
       
      CPPUNIT_ASSERT_EQUAL(wrapperName, wrapper->GetName());
      CPPUNIT_ASSERT_EQUAL(wrapperID, wrapper->GetID());
      CPPUNIT_ASSERT_EQUAL(duration, wrapper->GetDuration());
      CPPUNIT_ASSERT_EQUAL(speed, wrapper->GetSpeed());
   }

   
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
      dtCore::RefPtr<Animatable> pClone = mAnimatable1->Clone();

      CPPUNIT_ASSERT_EQUAL(name1, pClone->GetName());
      CPPUNIT_ASSERT_EQUAL(animStart1, pClone->GetStartTime());
      CPPUNIT_ASSERT_EQUAL(animEnd1, pClone->GetEndTime());
      CPPUNIT_ASSERT_EQUAL(fadeIn1, pClone->GetFadeIn());
      CPPUNIT_ASSERT_EQUAL(fadeOut1, pClone->GetFadeOut());
      CPPUNIT_ASSERT_EQUAL(base1, pClone->GetBaseWeight());
      CPPUNIT_ASSERT(!pClone->IsActive());
   }

   
   void AnimationTests::TestAnimChannel()
   {

      
   }

   
   void AnimationTests::TestAnimSequence()
   {

      dtCore::RefPtr<AnimationSequence> pSeq = new AnimationSequence();
      
      CPPUNIT_ASSERT_EQUAL(size_t(0), pSeq->GetChildAnimations().size());
      
      pSeq->AddAnimation(mAnimatable1.get());
      CPPUNIT_ASSERT_EQUAL(size_t(1), pSeq->GetChildAnimations().size());

      pSeq->AddAnimation(mAnimatable2.get());
      CPPUNIT_ASSERT_EQUAL(size_t(2), pSeq->GetChildAnimations().size());

      CPPUNIT_ASSERT_EQUAL(pSeq->GetAnimation(name1), (Animatable*) mAnimatable1.get());
      CPPUNIT_ASSERT_EQUAL(pSeq->GetAnimation(name2), (Animatable*) mAnimatable2.get());
   }

   void AnimationTests::TestSequenceMixer()
   {

   }

   void AnimationTests::TestAnimController()
   {      
      std::ostringstream ss;

      //test fade in and out
      dtCore::RefPtr<TestSequence> seq = new TestSequence();
      dtCore::RefPtr<TestController> cont = new TestController(seq.get());
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
 
   void AnimationTests::TestAnimHelper()
   {
      std::string context = dtCore::GetDeltaRootPath() + "/examples/data/demoMap/SkeletalMeshes/";
      std::string filename = "marine.xml";
      std::string animName = "Walk";

      mHelper->LoadModel(context + filename); 
 
      SequenceMixer* mixer = mHelper->GetSequenceMixer();
      const Animatable* anim = mixer->GetRegisteredAnimation(animName);
      CPPUNIT_ASSERT(anim != NULL);
      
      mHelper->PlayAnimation(animName);

      Animatable* activeAnim = mixer->GetActiveAnimation(animName);

      CPPUNIT_ASSERT_EQUAL(animName, activeAnim->GetName());
      
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());
       
      activeAnim->SetStartDelay(3.0f);
      mixer->ForceRecalculate();
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      mHelper->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      mHelper->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(false, activeAnim->IsActive());

      mHelper->Update(1.0f);
      CPPUNIT_ASSERT_EQUAL(true, activeAnim->IsActive());

      CPPUNIT_ASSERT_EQUAL(3.0f, activeAnim->GetElapsedTime());
 
      dtCore::RefPtr<AnimationChannel> activeChannel = dynamic_cast<AnimationChannel*>(activeAnim);
      CPPUNIT_ASSERT(activeChannel.valid());

      activeChannel->SetMaxDuration(3.5f);
      mixer->ForceRecalculate();
      mHelper->Update(3.5f);
      
      CPPUNIT_ASSERT_EQUAL(false, activeChannel->IsActive());

      CPPUNIT_ASSERT(mixer->GetActiveAnimation(animName) == 0); 

      ////OK, so far so good, lets try testing looping vs non looping
      //mHelper->PlayAnimation(animName);
      //activeAnim = mixer->GetActiveAnimation(animName);
      //CPPUNIT_ASSERT(activeAnim);
      //activeChannel = dynamic_cast<AnimationChannel*>(activeAnim);
      //CPPUNIT_ASSERT(activeChannel.valid());

      //activeChannel->SetLooping(false);
   

   }
}
