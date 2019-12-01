/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* David Guthrie
*/
#include <prefix/unittestprefix.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/hotspotdefinition.h>
#include <dtUtil/mathdefines.h>

#include <dtAnim/animationhelper.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/modeldatabase.h>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include "AnimModelLoadingTestFixture.h"

#include <dtGame/gamemanager.h>
#include <dtGame/basemessages.h>

#include <dtCore/map.h>
#include <dtCore/project.h>

#include <dtAnim/animationgameactor.h>

#include <osg/Vec3>
#include <osg/Quat>

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

namespace dtAnim
{   
   class AttachmentControllerExtended : public AttachmentController
   {
      public:
         
         typedef AttachmentController BaseClass;
         
         AttachmentControllerExtended(): AttachmentController(), mUpdated(false)
         {
         }
         
         virtual void Update(dtAnim::BaseModelWrapper& model)
         {
            BaseClass::Update(model);
            mUpdated = true;
         }
         
         bool mUpdated;
   };
   
   class AttachmentControllerTests : public AnimModelLoadingTestFixture
   {
      CPPUNIT_TEST_SUITE(AttachmentControllerTests);
      CPPUNIT_TEST(TestUpdate);
      CPPUNIT_TEST(TestMotion);
      CPPUNIT_TEST(TestAddRemove);
      CPPUNIT_TEST_SUITE_END();
      
      public:
         void setUp() override
         {
            AnimModelLoadingTestFixture::setUp();
            mAnimHelper = new AnimationHelper();
            Connect(mAnimHelper);
            mAttach = new AttachmentControllerExtended();
            mAnimHelper->SetAttachmentController(mAttach);

            dtCore::ResourceDescriptor charModel("SkeletalMeshes:Marine:marine_test.xml");

            LoadModel(mAnimHelper, charModel);
            dtAnim::BaseModelWrapper* wrapper = mAnimHelper->GetModelWrapper();
            CPPUNIT_ASSERT(wrapper != NULL);
            float testScale = 0.77f;
            wrapper->GetModelData()->SetScale(testScale);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, mAnimHelper->GetModelWrapper()->GetScale(), 0.01f);
            mAnimHelper->GetModelWrapper()->SetScale(testScale);
         }

         void tearDown() override
         {
            dtAnim::ModelDatabase::GetInstance().TruncateDatabase();
            mAttach = NULL;
            mAnimHelper = NULL;
            AnimModelLoadingTestFixture::tearDown();
         }

         void TestUpdate()
         {
            CPPUNIT_ASSERT(mAnimHelper->GetAttachmentController() == mAttach.get());

            mAnimHelper->Update(50);
            CPPUNIT_ASSERT(mAttach->mUpdated);
            mAttach->mUpdated = false;
            
         }

         void TestAddRemove()
         {
            CPPUNIT_ASSERT(mAnimHelper->GetAttachmentController() == mAttach.get());
            mAnimHelper->GetAttachmentController()->Clear(); // clear out what is loaded from the file.

            dtCore::RefPtr<dtCore::Transformable> attachment = new dtCore::Transformable("TestAttachment");
            dtUtil::HotSpotDefinition spotDef;
            spotDef.mName = "TestName1";
            
            mAttach->AddAttachment(*attachment, spotDef);
            
            {
               const AttachmentController::AttachmentContainer& attachments = mAttach->GetAttachments();
               CPPUNIT_ASSERT_EQUAL(size_t(1), attachments.size());
               CPPUNIT_ASSERT_MESSAGE("the first item in the attachments list must contain the added transformable.",
                     attachments[0].first.get() == attachment.get());
               CPPUNIT_ASSERT_MESSAGE("the first item in the attachments list must contain the added hotspot definition.",
                     attachments[0].second.mName == spotDef.mName);
            }
            
            dtCore::RefPtr<dtCore::Transformable> attachment2 = new dtCore::Transformable("TestAttachment");
            dtUtil::HotSpotDefinition spotDef2;
            spotDef2.mName = "TestName2";
            
            mAttach->AddAttachment(*attachment2, spotDef2);
            
            {
               const AttachmentController::AttachmentContainer& attachments = mAttach->GetAttachments();
               CPPUNIT_ASSERT_EQUAL(size_t(2), attachments.size());
   
               CPPUNIT_ASSERT_MESSAGE("the first item in the attachments list must contain the first added transformable.",
                     attachments[0].first.get() == attachment.get());
               CPPUNIT_ASSERT_MESSAGE("the first item in the attachments list must contain the first added hotspot definition.",
                     attachments[0].second.mName == spotDef.mName);
               CPPUNIT_ASSERT_MESSAGE("the second item in the attachments list must contain the second added transformable.",
                     attachments[1].first.get() == attachment2.get());
               CPPUNIT_ASSERT_MESSAGE("the second item in the attachments list must contain the second added hotspot definition.",
                     attachments[1].second.mName == spotDef2.mName);
            }
            
            mAttach->RemoveAttachment(*attachment);
            
            {
               const AttachmentController::AttachmentContainer& attachments = mAttach->GetAttachments();
               CPPUNIT_ASSERT_EQUAL(size_t(1), attachments.size());
   
               CPPUNIT_ASSERT_MESSAGE("the first item in the attachments list must now contain the second added transformable.",
                     attachments[0].first.get() == attachment2.get());
               CPPUNIT_ASSERT_MESSAGE("the first item in the attachments list must now contain the second added hotspot definition.",
                     attachments[0].second.mName == spotDef2.mName);
            }

            mAttach->RemoveAttachment(*attachment2);
            CPPUNIT_ASSERT_EQUAL(size_t(0), mAttach->GetAttachments().size());
         }

         void TestMotion()
         {
            dtCore::RefPtr<dtCore::Transformable> attachment = new dtCore::Transformable("TestAttachment");
            dtUtil::HotSpotDefinition spotDef;

            osg::Vec3 attachPos(0.1f, 0.2f, 0.3f);
            osg::Quat attachRot(0.632455532034f, 0.707106f, 0.316227766f, 0.0f);
            spotDef.mLocalTranslation = attachPos;
            spotDef.mLocalRotation = attachRot;
            spotDef.mName = "jojo";
            
            std::vector<std::string> names;
            BaseModelWrapper* wrapper = mAnimHelper->GetModelWrapper();
            CPPUNIT_ASSERT(wrapper != NULL);

            dtAnim::BoneArray bones;
            wrapper->GetBones(bones);
            dtAnim::BoneInterface* bone = bones.front().get();
            spotDef.mParentName = bone->GetName();

            mAttach->AddAttachment(*attachment, spotDef);

            mAnimHelper->Update(50);
            osg::Quat boneRot = bone->GetAbsoluteRotation();
            osg::Vec3 boneTrans = bone->GetAbsoluteTranslation();

            osg::Quat expectedRot = spotDef.mLocalRotation * boneRot;
            osg::Vec3 expectedPos = boneTrans + (boneRot * spotDef.mLocalTranslation) * wrapper->GetScale();

            dtCore::Transform xform;
            attachment->GetTransform(xform, dtCore::Transformable::REL_CS);
            osg::Quat actualRot;
            xform.GetRotation(actualRot);

            osg::Vec3 trans;
            xform.GetTranslation(trans);
            std::ostringstream oss;
            oss << "expectedRot: " << expectedRot << " actualRot: " << actualRot;
            CPPUNIT_ASSERT_MESSAGE(oss.str(), dtUtil::Equivalent(expectedRot, actualRot, 4, osg::Quat::value_type(0.0003)));
            oss.str("");
            oss << "expectedPos: " << expectedPos << " trans: " << trans;
            CPPUNIT_ASSERT_MESSAGE(oss.str(), dtUtil::Equivalent(expectedPos, trans, osg::Vec3::value_type(0.0003)));
         }

      private:

         dtCore::RefPtr<AnimationHelper> mAnimHelper;
         dtCore::RefPtr<AttachmentControllerExtended> mAttach;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AttachmentControllerTests );

}//namespace dtAnim
