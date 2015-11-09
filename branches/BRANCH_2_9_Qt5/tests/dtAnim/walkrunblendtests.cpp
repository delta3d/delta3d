/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2014, David Guthrie
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

#include <dtAnim/animatable.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/walkrunblend.h>

#include <dtCore/project.h>
#include <dtCore/refptr.h>
#include <dtUtil/datapathutils.h>

#include <osg/Math>
#include <sstream>

#include <string>

namespace dtAnim
{
   class WalkRunBlendTests : public AnimModelLoadingTestFixture
   {

      CPPUNIT_TEST_SUITE( WalkRunBlendTests );
      CPPUNIT_TEST( TestCalculateWeights );
      CPPUNIT_TEST( TestEdgeCases );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp() override;
         void tearDown() override;

         void TestCalculateWeights();
         void TestEdgeCases();

      private:

         dtCore::RefPtr<dtAnim::AnimationHelper> mAnimationAC;
         dtCore::RefPtr<const dtAnim::Animatable> mAnimStand;
         dtCore::RefPtr<const dtAnim::Animatable> mAnimWalk;
         dtCore::RefPtr<const dtAnim::Animatable> mAnimRun;

         std::string mAnimNameStand;
         std::string mAnimNameWalk;
         std::string mAnimNameRun;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( WalkRunBlendTests );

   /////////////////////////////////////////////////////////////////////////////
   void WalkRunBlendTests::setUp()
   {
      AnimModelLoadingTestFixture::setUp();

      mAnimationAC = new AnimationHelper();
      Connect(mAnimationAC);

      dtCore::ResourceDescriptor modelPath("SkeletalMeshes:Marine:marine_test.xml");

      LoadModel(mAnimationAC, modelPath);

      mAnimNameStand = "Idle";
      mAnimNameWalk = "Walk";
      mAnimNameRun = "Run";

      SequenceMixer& mixer = mAnimationAC->GetSequenceMixer();

      mAnimStand = mixer.GetRegisteredAnimation(mAnimNameStand);
      CPPUNIT_ASSERT(mAnimStand.valid());
      mAnimWalk = mixer.GetRegisteredAnimation(mAnimNameWalk);
      CPPUNIT_ASSERT(mAnimWalk.valid());
      mAnimRun = mixer.GetRegisteredAnimation(mAnimNameRun);
      CPPUNIT_ASSERT(mAnimRun.valid());

   }

   /////////////////////////////////////////////////////////////////////////////
   void WalkRunBlendTests::tearDown()
   {
      mAnimationAC = NULL;
      AnimModelLoadingTestFixture::tearDown();
   }

   class TestVelocityInterface: public osg::Referenced, public dtCore::VelocityInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE;
      /*override*/ osg::Vec3 GetVelocity() const
      {
         return mVelocity;
      }

      void SetVelocity(const osg::Vec3& vel)
      {
         mVelocity = vel;
      }
   private:
      osg::Vec3 mVelocity;
   };

   /////////////////////////////////////////////////////////////////////////////
   void WalkRunBlendTests::TestCalculateWeights()
   {

      dtCore::RefPtr<TestVelocityInterface> velocityInterface = new TestVelocityInterface;

      dtCore::RefPtr<WalkRunBlend> pChildSeq = new WalkRunBlend(*velocityInterface);
      CPPUNIT_ASSERT(pChildSeq.get() == &pChildSeq->GetController()->GetParent());
      std::string childSeqName("WalkRunBlend");

      CPPUNIT_ASSERT_EQUAL(size_t(0), pChildSeq->GetChildAnimations().size());

      // This doesn't work, but it shouldn't crash.
      pChildSeq->Setup(1.3f, 2.5f);

      pChildSeq->SetAnimations(mAnimStand->Clone(mAnimationAC->GetModelWrapper()),
            mAnimWalk->Clone(mAnimationAC->GetModelWrapper()),
            mAnimRun->Clone(mAnimationAC->GetModelWrapper()) );
      pChildSeq->SetName(childSeqName);

      CPPUNIT_ASSERT_EQUAL(size_t(3), pChildSeq->GetChildAnimations().size());

      pChildSeq->Setup(1.3f, 2.5f);

      dtCore::RefPtr<AnimationSequence> clonedSeq = static_cast<AnimationSequence*>(pChildSeq->Clone(mAnimationAC->GetModelWrapper()).get());
      CPPUNIT_ASSERT_EQUAL(size_t(3), clonedSeq->GetChildAnimations().size());
      dtCore::RefPtr<WalkRunBlend> clonedWRB = dynamic_cast<WalkRunBlend*>(clonedSeq.get());
      CPPUNIT_ASSERT(clonedWRB.valid());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The cloned WR controller should have 4 entries.", 4U, clonedWRB->GetWalkRunController().GetAnimationCount());
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The stand animation speed should be cloned properly.", 0.0f, clonedWRB->GetWalkRunController().GetAnimationInherentSpeed(0), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The slow walk animation speed should be cloned properly.", 1.3f * 0.618f, clonedWRB->GetWalkRunController().GetAnimationInherentSpeed(1), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The normal walk animation speed should be cloned properly.", 1.3f, clonedWRB->GetWalkRunController().GetAnimationInherentSpeed(2), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The run animation speed should be cloned properly.", 2.5f, clonedWRB->GetWalkRunController().GetAnimationInherentSpeed(3), 0.001f);

      for (unsigned i = 0; i < clonedWRB->GetWalkRunController().GetAnimationCount(); ++i)
      {
         Animatable* a = clonedWRB->GetWalkRunController().GetAnimation(i);
         Animatable* oldA = pChildSeq->GetWalkRunController().GetAnimation(i);
         if (a != NULL)
         {
            CPPUNIT_ASSERT(oldA != a);
            CPPUNIT_ASSERT(oldA != NULL);
            CPPUNIT_ASSERT_EQUAL(oldA->GetName(), a->GetName());
            CPPUNIT_ASSERT(clonedWRB->GetAnimation(a->GetName()) == a);
         }
         else
         {
            CPPUNIT_ASSERT(oldA == NULL);
         }
      }
      CPPUNIT_ASSERT_MESSAGE("The cloned WR controller animations should not have the same pointers.", pChildSeq->GetWalkRunController().GetAnimation(0) != clonedWRB->GetWalkRunController().GetAnimation(0));

      dtAnim::WalkRunBlend::WRController* controller = dynamic_cast<dtAnim::WalkRunBlend::WRController*>(pChildSeq->GetController());

      CPPUNIT_ASSERT(controller != NULL);
      CPPUNIT_ASSERT_EQUAL(controller->GetAnimation(0), pChildSeq->GetAnimation(mAnimNameStand));
      CPPUNIT_ASSERT_EQUAL(controller->GetAnimation(1), (dtAnim::Animatable*)(NULL));
      CPPUNIT_ASSERT_EQUAL(controller->GetAnimation(2), pChildSeq->GetAnimation(mAnimNameWalk));
      CPPUNIT_ASSERT_EQUAL(controller->GetAnimation(3), pChildSeq->GetAnimation(mAnimNameRun));


      const float updateDT = 0.0166667f;

      pChildSeq->Update(updateDT);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 0.5f, 0.0f));

      pChildSeq->Update(updateDT);
      // The blending should not change for 1 frame.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      pChildSeq->Update(updateDT);
      // The blending should change after a frame.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.382f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.618f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 1.2f, 0.0f));

      pChildSeq->Update(updateDT);
      // The blending should not change yet.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.382f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.618f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      pChildSeq->Update(updateDT);
      // The blending should change after a frame.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 2.5f, 0.0f));

      pChildSeq->Update(updateDT);
      // The blending should not change yet.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      pChildSeq->Update(updateDT);
      // The blending should change after a frame.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

   }

   void WalkRunBlendTests::TestEdgeCases()
   {
      dtCore::RefPtr<TestVelocityInterface> velocityInterface = new TestVelocityInterface;

      dtCore::RefPtr<WalkRunBlend> pChildSeq = new WalkRunBlend(*velocityInterface);
      CPPUNIT_ASSERT(pChildSeq.get() == &pChildSeq->GetController()->GetParent());
      std::string childSeqName("WalkRunBlend");

      pChildSeq->SetAnimations(mAnimStand->Clone(mAnimationAC->GetModelWrapper()),
            mAnimWalk->Clone(mAnimationAC->GetModelWrapper()),
            mAnimRun->Clone(mAnimationAC->GetModelWrapper()) );
      pChildSeq->SetName(childSeqName);

      pChildSeq->Setup(1.0f, 2.0f);

      const float updateDT = 0.0166667f;

      pChildSeq->Update(updateDT);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      // Should trigger a pick closest, but it's outside of the 38.2% range, so if the next one up was active, this would pick that one,
      // but it shouldn't
      velocityInterface->SetVelocity(osg::Vec3(0.0f, 1.4f, 0.0f));
      pChildSeq->Update(updateDT);
      pChildSeq->Update(updateDT);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 1.6f, 0.0f));
      pChildSeq->Update(updateDT);
      pChildSeq->Update(updateDT);

      // Should not change.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 1.8f, 0.0f));
      pChildSeq->Update(updateDT);
      pChildSeq->Update(updateDT);

      // Should change at this point.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 1.4f, 0.0f));
      pChildSeq->Update(updateDT);
      pChildSeq->Update(updateDT);

      // Should not change.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 0.9f, 0.0f));
      pChildSeq->Update(updateDT);
      pChildSeq->Update(updateDT);

      // Should change.
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameStand)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, pChildSeq->GetAnimation(mAnimNameWalk)->GetCurrentWeight(), 0.001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, pChildSeq->GetAnimation(mAnimNameRun)->GetCurrentWeight(), 0.001f);
   }

}


