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
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/animationwrapper.h>
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
   class WalkRunBlendTests : public CPPUNIT_NS::TestFixture
   {

      CPPUNIT_TEST_SUITE( WalkRunBlendTests );
      CPPUNIT_TEST( TestCalculateWeights );
      CPPUNIT_TEST( TestEdgeCases );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

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
      mAnimationAC = new AnimationHelper();
      dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");

      std::string modelPath = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
      CPPUNIT_ASSERT(!modelPath.empty());

      mAnimationAC->LoadModel(modelPath);

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

      pChildSeq->SetAnimations(mAnimStand->Clone(mAnimationAC->GetModelWrapper()),
            mAnimWalk->Clone(mAnimationAC->GetModelWrapper()),
            mAnimRun->Clone(mAnimationAC->GetModelWrapper()) );
      pChildSeq->SetName(childSeqName);

      CPPUNIT_ASSERT_EQUAL(size_t(3), pChildSeq->GetChildAnimations().size());

      pChildSeq->Setup(1.0f, 2.0f);

      dtCore::RefPtr<AnimationSequence> clonedSeq = static_cast<AnimationSequence*>(pChildSeq->Clone(mAnimationAC->GetModelWrapper()).get());
      CPPUNIT_ASSERT_EQUAL(size_t(3), clonedSeq->GetChildAnimations().size());

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

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 1.0f, 0.0f));

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

      velocityInterface->SetVelocity(osg::Vec3(0.0f, 2.0f, 0.0f));

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


