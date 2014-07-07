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
#include <cppunit/extensions/HelperMacros.h>

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

   class Cal3DModelWrapperTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(Cal3DModelWrapperTests);
      CPPUNIT_TEST(TestAttachDetachMesh);
      CPPUNIT_TEST(TestShowHideMesh);
      CPPUNIT_TEST(TestCanUpdate);
      CPPUNIT_TEST_SUITE_END();

   public:

      //////////////////////////////////////////////////////////////////////////
      void setUp()
      {
         dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");

         std::string modelPath = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
         CPPUNIT_ASSERT_MESSAGE("Could not find \"SkeletalMeshes/Marine/marine_test.xml\"", !modelPath.empty());

         mAnimHelper = new dtAnim::AnimationHelper();
         mAnimHelper->LoadModel(modelPath);
         mModel = mAnimHelper->GetModelWrapper();
      }

      //////////////////////////////////////////////////////////////////////////
      void tearDown()
      {
         mModel = NULL;
         mAnimHelper = NULL;
      }

      //////////////////////////////////////////////////////////////////////////
      void TestAttachDetachMesh()
      {
         int meshCount = mModel->GetMeshCount();
         int remainingMeshes = meshCount;
         CPPUNIT_ASSERT(meshCount > 0);

         for (int i = 0; i < meshCount; ++i)
         {
            CPPUNIT_ASSERT(mModel->DetachMesh(i));
            CPPUNIT_ASSERT_EQUAL(remainingMeshes - 1, mModel->GetMeshCount());
            CPPUNIT_ASSERT(mModel->AttachMesh(i));
            CPPUNIT_ASSERT_EQUAL(remainingMeshes, mModel->GetMeshCount());
            CPPUNIT_ASSERT(mModel->DetachMesh(i));
            --remainingMeshes;
            CPPUNIT_ASSERT_EQUAL(remainingMeshes, mModel->GetMeshCount());
         }
      }

      //////////////////////////////////////////////////////////////////////////
      void TestShowHideMesh()
      {
         int meshCount = mModel->GetMeshCount();
         CPPUNIT_ASSERT(meshCount > 0);

         for (int i = 0; i < meshCount; ++i)
         {
            CPPUNIT_ASSERT_MESSAGE("All meshes should be visible initially.", mModel->IsMeshVisible(i));
            mModel->HideMesh(i);
            CPPUNIT_ASSERT_MESSAGE("Mesh should now not be visible.", !mModel->IsMeshVisible(i));
         }

         mModel->ShowMesh(1003000);
         CPPUNIT_ASSERT_MESSAGE("Showing an invalid mesh should not make IsMeshVisible return true.",
                  !mModel->IsMeshVisible(1003000));
      }

      //////////////////////////////////////////////////////////////////////////
      void TestCanUpdate()
      {
         // Declare variables needed for following tests.
         std::string animName("Run");
         const float TIME_STEP = 0.5f;
         std::vector<std::string> boneNameArray;
         mModel->GetCoreBoneNames(boneNameArray);
         CPPUNIT_ASSERT(boneNameArray.size() > 0);

         // --- Find a test bone id.
         int boneId = mModel->GetCoreBoneID(boneNameArray[0]);
         CPPUNIT_ASSERT(boneId != Cal3DModelWrapper::NULL_BONE);


         // Begin tests...

         // Test the global default
         CPPUNIT_ASSERT( ! Cal3DModelWrapper::GetAllowBindPose());
         Cal3DModelWrapper::SetAllowBindPose(true);
         CPPUNIT_ASSERT(Cal3DModelWrapper::GetAllowBindPose());

         // --- Get the bind pose transform.
         mAnimHelper->Update(TIME_STEP);
         osg::Vec3 bindPoseTrans = mModel->GetBoneAbsoluteTranslation(boneId);
         osg::Quat bindPoseRot = mModel->GetBoneAbsoluteRotation(boneId);

         // --- Ensure the model takes the global flag into account.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT(mModel->CanUpdate());

         // --- Set back the default for further test purposes.
         Cal3DModelWrapper::SetAllowBindPose(false);
         CPPUNIT_ASSERT( ! Cal3DModelWrapper::GetAllowBindPose());

         // --- Ensure the model still takes the global flag into account.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT( ! mModel->CanUpdate());


         // Test movement of a bone in an animation.
         mAnimHelper->PlayAnimation(animName);
         mAnimHelper->Update(TIME_STEP);

         // --- Get the transform of the moved bone and ensure it is not
         osg::Vec3 animTrans = mModel->GetBoneAbsoluteTranslation(boneId);
         osg::Quat animRot = mModel->GetBoneAbsoluteRotation(boneId);
         CPPUNIT_ASSERT(animTrans != bindPoseTrans);
         CPPUNIT_ASSERT(animRot != bindPoseRot);

         // --- Ensure the state of the animation.
         CPPUNIT_ASSERT(mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT(mModel->CanUpdate());


         // Test that clearing the animation causes the model to be left on the
         // last frame of animation that was updated, rather than reverting back
         // to the model's default bind pose.
         mAnimHelper->ClearAll(0.0f);
         mAnimHelper->Update(TIME_STEP);

         // --- Ensure that the animation has stopped and that model knows that
         //     it cannot update its skeleton.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT( ! mModel->CanUpdate());

         // --- Verify that the bone's transform matches that of the recent animation frame.
         osg::Vec3 endTrans = mModel->GetBoneAbsoluteTranslation(boneId);
         osg::Quat endRot = mModel->GetBoneAbsoluteRotation(boneId);
         CPPUNIT_ASSERT(endTrans != bindPoseTrans);
         CPPUNIT_ASSERT(endRot != bindPoseRot);
         CPPUNIT_ASSERT(endTrans == animTrans);
         CPPUNIT_ASSERT(endRot == animRot);


         // Test re-enabling bind pose with a subsequent call to Update.
         Cal3DModelWrapper::SetAllowBindPose(true);
         mAnimHelper->ClearAll(0.0f);
         mAnimHelper->Update(TIME_STEP);

         // --- Ensure that the model knows that it is allowed to update from
         //     global permission while ensuring there is no animation present.
         CPPUNIT_ASSERT( ! mAnimHelper->GetSequenceMixer().IsAnimationPlaying(animName));
         CPPUNIT_ASSERT(mModel->CanUpdate());

         // --- The resulting transform of the test bone should be the same as
         //     that of the bones original bind pose.
         endTrans = mModel->GetBoneAbsoluteTranslation(boneId);
         endRot = mModel->GetBoneAbsoluteRotation(boneId);
         CPPUNIT_ASSERT(endTrans == bindPoseTrans);
         CPPUNIT_ASSERT(endRot == bindPoseRot);
         CPPUNIT_ASSERT(endTrans != animTrans);
         CPPUNIT_ASSERT(endRot != animRot);
      }

      dtCore::RefPtr<AnimationHelper> mAnimHelper;
      dtCore::RefPtr<Cal3DModelWrapper> mModel;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( Cal3DModelWrapperTests );

}
