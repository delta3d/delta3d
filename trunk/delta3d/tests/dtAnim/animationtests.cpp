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

#include <dtCore/refptr.h>

#include <string>

namespace dtAnim
{

      class MyAnimatable: public Animatable
      {
      public:
         std::string str;
         void Update(float dt){};
         void ForceFadeOut(float time){};
         const std::string& GetName() const{return str;}            
         void SetName(const std::string& s){str = s;};    
         void Recalculate(){}
         dtCore::RefPtr<Animatable> Clone()const{return new MyAnimatable();}
         void Prune(){}
      };




   class AnimationTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( AnimationTests );
         CPPUNIT_TEST( TestAnimWrapper );
         CPPUNIT_TEST( TestAnimatable );
         CPPUNIT_TEST( TestAnimChannel );
         CPPUNIT_TEST( TestAnimSequence );
         CPPUNIT_TEST( TestSequenceMixer );
         CPPUNIT_TEST( TestAnimHelper );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestAnimWrapper(); 
         void TestAnimatable(); 
         void TestAnimChannel();
         void TestAnimSequence();         
         void TestSequenceMixer();
         void TestAnimHelper();

      private:

         float animStart1;
         float animEnd1;
         float fadeIn1;
         float fadeOut1;
         float base1;
         float start_delay1;
         std::string name1;
         dtCore::RefPtr<MyAnimatable> mAnimatable1;

         float animStart2;
         float animEnd2;
         float fadeIn2;
         float fadeOut2;
         float base2;
         float start_delay2;
         std::string name2;
         dtCore::RefPtr<MyAnimatable> mAnimatable2;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AnimationTests );

   void AnimationTests::setUp()
   {
      animStart1 = 1.0f;
      animEnd1 = 2.0f;
      fadeIn1 = 0.5f;
      fadeOut1 = 1.0f;
      base1 = 1.5f;
      name1 = "Anim001";
      start_delay1 = 0.5f;
      mAnimatable1 = new MyAnimatable();

      mAnimatable1->SetStartTime(animStart1);
      mAnimatable1->SetEndTime(animEnd1);
      mAnimatable1->SetFadeIn(fadeIn1);
      mAnimatable1->SetFadeOut(fadeOut1);
      mAnimatable1->SetBaseWeight(base1);
      mAnimatable1->SetName(name1);
      mAnimatable1->SetStartDelay(start_delay1);

      animStart2 = 1.0f;
      animEnd2 = 2.0f;
      fadeIn2 = 0.5f;
      fadeOut2 = 1.0f;
      base2 = 1.5f;
      start_delay2 = 3.0f;
      name2 = "Anim002";

      mAnimatable2 = new MyAnimatable();

      mAnimatable2->SetStartTime(animStart2);
      mAnimatable2->SetEndTime(animEnd2);
      mAnimatable2->SetFadeIn(fadeIn2);
      mAnimatable2->SetFadeOut(fadeOut2);
      mAnimatable2->SetBaseWeight(base2);
      mAnimatable2->SetName(name2);
      mAnimatable2->SetStartDelay(start_delay2);

   }

   void AnimationTests::tearDown()
   {
      mAnimatable1 = 0;
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

   
   void AnimationTests::TestAnimatable()
   {

      CPPUNIT_ASSERT_EQUAL(animStart1, mAnimatable1->GetStartTime());
      CPPUNIT_ASSERT_EQUAL(animEnd1, mAnimatable1->GetEndTime());
      CPPUNIT_ASSERT_EQUAL(fadeIn1, mAnimatable1->GetFadeIn());
      CPPUNIT_ASSERT_EQUAL(fadeOut1, mAnimatable1->GetFadeOut());
      CPPUNIT_ASSERT_EQUAL(base1, mAnimatable1->GetBaseWeight());
      CPPUNIT_ASSERT(!mAnimatable1->IsActive());
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

   void AnimationTests::TestAnimHelper()
   {

   }
}
