/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2008, Alion Science and Technology Corporation
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
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/unittestprefix.h>
#include "AnimModelLoadingTestFixture.h"

#include <dtAnim/cal3danimator.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animationhelper.h>

#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/refptr.h>

#include <osg/Math>
#include <osg/io_utils>
#include <cmath>
#include <sstream>

#include <string>



namespace dtAnim
{

   class Cal3DModelWrapperTests : public AnimModelLoadingTestFixture
   {
      CPPUNIT_TEST_SUITE(Cal3DModelWrapperTests);
      CPPUNIT_TEST(TestAttachDetachMesh);
      CPPUNIT_TEST(TestShowHideMesh);
      CPPUNIT_TEST(TestIsUpdatable);
      CPPUNIT_TEST_SUITE_END();

   public:

      //////////////////////////////////////////////////////////////////////////
      void setUp() override
      {
         AnimModelLoadingTestFixture::setUp();

         dtCore::Project::GetInstance().SetContext("../examples/data");

         dtCore::ResourceDescriptor modelPath("SkeletalMeshes/Marine/marine_test.xml");

         mAnimHelper = new dtAnim::AnimationHelper();
         Connect(mAnimHelper);
         LoadModel(mAnimHelper, modelPath);
         mModel = mAnimHelper->GetModelWrapper();
         CPPUNIT_ASSERT(mModel.valid());
      }

      //////////////////////////////////////////////////////////////////////////
      void tearDown() override
      {
         mModel = NULL;
         mAnimHelper = NULL;
         AnimModelLoadingTestFixture::tearDown();
      }

      //////////////////////////////////////////////////////////////////////////
      void TestAttachDetachMesh()
      {
         Cal3DModelWrapper* calModelWrapper
            = dynamic_cast<Cal3DModelWrapper*>(mModel.get());
         CPPUNIT_ASSERT(calModelWrapper != NULL);
         int meshCount = calModelWrapper->GetMeshCount();
         int remainingMeshes = meshCount;
         CPPUNIT_ASSERT(meshCount > 0);

         for (int i = 0; i < meshCount; ++i)
         {
            CPPUNIT_ASSERT_EQUAL(remainingMeshes, calModelWrapper->GetAttachedMeshCount());
            CPPUNIT_ASSERT(calModelWrapper->DetachMesh(i));
            CPPUNIT_ASSERT_EQUAL(remainingMeshes - 1, calModelWrapper->GetAttachedMeshCount());
            CPPUNIT_ASSERT(calModelWrapper->AttachMesh(i));
            CPPUNIT_ASSERT_EQUAL(remainingMeshes, calModelWrapper->GetAttachedMeshCount());
            CPPUNIT_ASSERT(calModelWrapper->DetachMesh(i));
            --remainingMeshes;
            CPPUNIT_ASSERT_EQUAL(remainingMeshes, calModelWrapper->GetAttachedMeshCount());
         }
      }

      //////////////////////////////////////////////////////////////////////////
      void TestShowHideMesh()
      {
         dtAnim::MeshArray meshes;
         int meshCount = mModel->GetMeshes(meshes);
         CPPUNIT_ASSERT(meshCount > 0);
         CPPUNIT_ASSERT(meshCount == (int)(meshes.size()));
         CPPUNIT_ASSERT(meshCount == mModel->GetMeshCount());

         dtAnim::MeshInterface* mesh = NULL;
         dtAnim::MeshArray::iterator curIter = meshes.begin();
         dtAnim::MeshArray::iterator endIter = meshes.end();
         for (; curIter != endIter; ++curIter)
         {
            mesh = curIter->get();

            CPPUNIT_ASSERT(mesh != NULL);
            CPPUNIT_ASSERT_MESSAGE("All meshes should be visible initially.", mesh->IsVisible());
            mesh->SetVisible(false);
            CPPUNIT_ASSERT_MESSAGE("Mesh should now not be visible.", !mesh->IsVisible());
            mesh->SetVisible(true);
            CPPUNIT_ASSERT_MESSAGE("Mesh should now be visible again.", mesh->IsVisible());
         }
      }

      //////////////////////////////////////////////////////////////////////////
      void TestIsUpdatable()
      {
         // Declare variables needed for following tests.
         std::string animName("Run");
         const float TIME_STEP = 0.5f;

         // --- Find a test bone.
         dtAnim::BoneArray bones;
         mModel->GetBones(bones);
         CPPUNIT_ASSERT(bones.size() > 0);

         dtAnim::BoneInterface* bone = bones.front().get();
         CPPUNIT_ASSERT(bone != NULL);

         // Begin tests...

         // Test the global default
         CPPUNIT_ASSERT( ! Cal3DAnimator::IsBindPoseAllowed());
         Cal3DAnimator::SetBindPoseAllowed(true);
         CPPUNIT_ASSERT(Cal3DAnimator::IsBindPoseAllowed());

         // --- Get the bind pose transform.
         mAnimHelper->Update(TIME_STEP);
         osg::Vec3 bindPoseTrans = bone->GetAbsoluteTranslation();
         osg::Quat bindPoseRot = bone->GetAbsoluteRotation();

         // --- Ensure the model takes the global flag into account.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT(mModel->GetAnimator()->IsUpdatable());

         // --- Set back the default for further test purposes.
         Cal3DAnimator::SetBindPoseAllowed(false);
         CPPUNIT_ASSERT( ! Cal3DAnimator::IsBindPoseAllowed());

         // --- Ensure the model still takes the global flag into account.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT( ! mModel->GetAnimator()->IsUpdatable());


         // Test movement of a bone in an animation.
         mAnimHelper->PlayAnimation(animName);
         mAnimHelper->Update(TIME_STEP);

         // --- Get the transform of the moved bone and ensure it is not
         osg::Vec3 animTrans = bone->GetAbsoluteTranslation();
         osg::Quat animRot = bone->GetAbsoluteRotation();
         CPPUNIT_ASSERT(animTrans != bindPoseTrans);
         CPPUNIT_ASSERT(animRot != bindPoseRot);

         // --- Ensure the state of the animation.
         CPPUNIT_ASSERT(mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT(mModel->GetAnimator()->IsUpdatable());


         // Test that clearing the animation causes the model to be left on the
         // last frame of animation that was updated, rather than reverting back
         // to the model's default bind pose.
         mAnimHelper->ClearAll(0.0f);
         mAnimHelper->Update(TIME_STEP);

         // --- Ensure that the animation has stopped and that model knows that
         //     it cannot update its skeleton.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT( ! mModel->GetAnimator()->IsUpdatable());

         // --- Verify that the bone's transform matches that of the recent animation frame.
         osg::Vec3 endTrans = bone->GetAbsoluteTranslation();
         osg::Quat endRot = bone->GetAbsoluteRotation();
         CPPUNIT_ASSERT(endTrans != bindPoseTrans);
         CPPUNIT_ASSERT(endRot != bindPoseRot);
         CPPUNIT_ASSERT(endTrans == animTrans);
         CPPUNIT_ASSERT(endRot == animRot);


         // Test re-enabling bind pose with a subsequent call to Update.
         Cal3DAnimator::SetBindPoseAllowed(true);
         mAnimHelper->ClearAll(0.0f);
         mAnimHelper->Update(TIME_STEP);

         // --- Ensure that the model knows that it is allowed to update from
         //     global permission while ensuring there is no animation present.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT(mModel->GetAnimator()->IsUpdatable());

         // --- The resulting transform of the test bone should be the same as
         //     that of the bones original bind pose.
         endTrans = bone->GetAbsoluteTranslation();
         endRot = bone->GetAbsoluteRotation();
         CPPUNIT_ASSERT(endTrans == bindPoseTrans);
         CPPUNIT_ASSERT(endRot == bindPoseRot);
         CPPUNIT_ASSERT(endTrans != animTrans);
         CPPUNIT_ASSERT(endRot != animRot);
      }

      dtCore::RefPtr<AnimationHelper> mAnimHelper;
      dtCore::RefPtr<BaseModelWrapper> mModel;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( Cal3DModelWrapperTests );

}
