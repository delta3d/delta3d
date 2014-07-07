/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/animationwrapper.h>
#include <dtAnim/animatable.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3dmodelwrapper.h>

#include <dtCore/refptr.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/refstring.h>
#include <dtUtil/mathdefines.h>

#include <cal3d/model.h>

#include <osg/io_utils>

#include <osg/Geode>

#include <string>

namespace dtAnim
{
   class Cal3DLoaderTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(Cal3DLoaderTests);
         CPPUNIT_TEST(TestIsFileValid);
         CPPUNIT_TEST(TestLoadFile);
         CPPUNIT_TEST(TestLODOptions);
         CPPUNIT_TEST(TestModelData);
         CPPUNIT_TEST(TestModelDataFileRegistration);
      CPPUNIT_TEST_SUITE_END();

      public:
         Cal3DLoaderTests()
         {
         }

         void setUp()
         {
            dtCore::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data");
            AnimNodeBuilder& nodeBuilder = Cal3DDatabase::GetInstance().GetNodeBuilder();
            if (nodeBuilder.SupportsSoftware())
            {
               nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateSoftware));
            }

            mHelper = new dtAnim::AnimationHelper;
         }

         void tearDown()
         {
            Cal3DDatabase::GetInstance().TruncateDatabase();
            mHelper = NULL;
         }

         void TestIsFileValid()
         {
            std::string validFile = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
            std::string invalidFile = dtUtil::FindFileInPathList("maps/TestAnim.xml");

            dtAnim::Cal3DDatabase& database = dtAnim::Cal3DDatabase::GetInstance();
            CPPUNIT_ASSERT(database.IsFileValid(validFile));
            CPPUNIT_ASSERT(!database.IsFileValid(invalidFile));
         }

         void TestLoadFile()
         {
            std::string modelPath = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
            CPPUNIT_ASSERT(!modelPath.empty());

            std::string animName = "Walk";

            TestEmptyHelper();

            mHelper->LoadModel(modelPath);

            SequenceMixer& mixer = mHelper->GetSequenceMixer();

            std::vector<const Animatable*> toFill;
            mixer.GetRegisteredAnimations(toFill);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of animatables loaded from the file is incorrect.",
                  size_t(9U), toFill.size());

            const Animatable* runAnim = mixer.GetRegisteredAnimation("Run");
            TestLoadedAnimatable(runAnim, "Run", 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

            const AnimationChannel* runChannel = dynamic_cast<const AnimationChannel*>(runAnim);
            TestLoadedAnimationChannel(runChannel, 0.0f, false, true);

            const Animatable* walkAnim = mixer.GetRegisteredAnimation("Walk");
            TestLoadedAnimatable(walkAnim, "Walk", 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

            const AnimationChannel* walkChannel = dynamic_cast<const AnimationChannel*>(walkAnim);
            TestLoadedAnimationChannel(walkChannel, 0.0f, false, true);

            const Animatable* idleAnim = mixer.GetRegisteredAnimation("Idle");
            TestLoadedAnimatable(idleAnim, "Idle", 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

            const AnimationChannel* idleChannel = dynamic_cast<const AnimationChannel*>(idleAnim);
            TestLoadedAnimationChannel(idleChannel, 0.0f, false, true);

            const Animatable* idleActionAnim = mixer.GetRegisteredAnimation("IdleAction");
            TestLoadedAnimatable(idleActionAnim, "IdleAction", 0.1f, 0.2f, 0.3f, 1.1f, 0.9f);

            const AnimationChannel* idleActionChannel = dynamic_cast<const AnimationChannel*>(idleActionAnim);
            TestLoadedAnimationChannel(idleActionChannel, 4.5f, true, false);

            const Animatable* runWalkAnim = mixer.GetRegisteredAnimation("RunWalk");
            TestLoadedAnimatable(runWalkAnim, "RunWalk", 0.1f, 0.2f, 0.2f, 1.0f, 1.0f);

            const Animatable* actionSeqAnim = mixer.GetRegisteredAnimation("ActionSequence");
            TestLoadedAnimatable(actionSeqAnim, "ActionSequence", 0.1f, 0.2f, 0.2f, 1.0f, 1.0f);

            const AnimationSequence* runWalkSequence = dynamic_cast<const AnimationSequence*>(runWalkAnim);
            const AnimationSequence* actionSequence = dynamic_cast<const AnimationSequence*>(actionSeqAnim);

            std::vector<std::string> childNames;
            childNames.push_back("Run");
            childNames.push_back("Walk");

            TestLoadedAnimationSequence(runWalkSequence, childNames);

            childNames.clear();
            childNames.push_back("RunAction");
            childNames.push_back("IdleAction");
            TestLoadedAnimationSequence(actionSequence, childNames);

            const dtAnim::AttachmentController& attachCon = mHelper->GetAttachmentController();
            CPPUNIT_ASSERT_EQUAL(1U, unsigned(attachCon.GetAttachments().size()));


            const dtUtil::HotSpotDefinition& hotSpotDef = attachCon.GetAttachments()[0].second;
            CPPUNIT_ASSERT_EQUAL(std::string("attachment1"), hotSpotDef.mName);
            CPPUNIT_ASSERT_EQUAL(std::string("Bip02 R Hand"), hotSpotDef.mParentName);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.1f, hotSpotDef.mLocalTranslation.x(), 0.01f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1f, hotSpotDef.mLocalTranslation.y(), 0.01f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3f, hotSpotDef.mLocalTranslation.z(), 0.01f);

            osg::Quat q;
            q.makeRotate ( osg::DegreesToRadians(90.0), osg::Vec3(1.0, 0.0, 0.0),
                              osg::DegreesToRadians(-90.0), osg::Vec3(0.0, 1.0, 0.0),
                              osg::DegreesToRadians(0.01), osg::Vec3(0.0, 0.0, 1.0));

            std::ostringstream oss;
            oss << q << " - " << hotSpotDef.mLocalRotation << std::endl;

            CPPUNIT_ASSERT_MESSAGE(oss.str(), dtUtil::Equivalent(q, hotSpotDef.mLocalRotation, 4U, 0.1));

            //Test unloading.
            CPPUNIT_ASSERT_NO_THROW(mHelper->LoadModel(""));
            TestEmptyHelper();


         }

         void TestLODOptions()
         {
            dtAnim::LODOptions lodOptions;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetStartDistance(), 10.0, 0.01);
            lodOptions.SetStartDistance(3.0);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetStartDistance(), 3.0, 0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetEndDistance(), 500.0, 0.01);
            lodOptions.SetEndDistance(4.0);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetEndDistance(), 4.0, 0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetMaxVisibleDistance(), 1000.0, 0.01);
            lodOptions.SetMaxVisibleDistance(5.0);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetMaxVisibleDistance(), 5.0, 0.01);
         }

         void TestModelData()
         {
            std::string modelPath = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
            CPPUNIT_ASSERT(!modelPath.empty());

            dtAnim::Cal3DDatabase& database = dtAnim::Cal3DDatabase::GetInstance();
            mHelper->LoadModel(modelPath);

            dtAnim::Cal3DModelWrapper* wrapper = mHelper->GetModelWrapper();
            CPPUNIT_ASSERT(wrapper != NULL);
            Cal3DModelData* modelData = database.GetModelData(*wrapper);
            CPPUNIT_ASSERT(modelData != NULL);

            dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
            fileUtils.CleanupFileString(modelPath);
            CPPUNIT_ASSERT_EQUAL(modelData->GetFilename(), modelPath);

            CPPUNIT_ASSERT(modelData->GetVertexBufferObject() == NULL);
            CPPUNIT_ASSERT(modelData->GetElementBufferObject() == NULL);

            CPPUNIT_ASSERT(modelData->GetCoreModel() == wrapper->GetCalModel()->getCoreModel());

            const dtAnim::LODOptions& lodOptions = modelData->GetLODOptions();

            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetStartDistance(), 1.75, 0.001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetEndDistance(), 43.6, 0.01);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(lodOptions.GetMaxVisibleDistance(), 401.0, 0.1);

            std::string testString("abc");
            CPPUNIT_ASSERT_EQUAL(std::string("Default"), modelData->GetShaderName());
            modelData->SetShaderName(testString);
            CPPUNIT_ASSERT_EQUAL(testString, modelData->GetShaderName());

            CPPUNIT_ASSERT_EQUAL(std::string("HardwareSkinning"), modelData->GetShaderGroupName());
            modelData->SetShaderGroupName(testString);
            CPPUNIT_ASSERT_EQUAL(testString, modelData->GetShaderGroupName());

            CPPUNIT_ASSERT_EQUAL_MESSAGE("71 should have been loaded from the test xml file",
                  71U, modelData->GetShaderMaxBones());
            modelData->SetShaderMaxBones(72);
            CPPUNIT_ASSERT_EQUAL(72U, modelData->GetShaderMaxBones());

            float errorTolerance = 0.001f;
            CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, modelData->GetScale(), errorTolerance);
            modelData->SetScale(0.5f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5f, modelData->GetScale(), errorTolerance);
            modelData->SetScale(2.5f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(2.5f, modelData->GetScale(), errorTolerance);
            // --- Ensure setting 0 does not exactly result in 0, but rather a close-to-0 scale.
            modelData->SetScale(0.0f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0.001f, modelData->GetScale(), errorTolerance);
         }

         // Helper Method
         bool VectorHasValue(const dtAnim::Cal3DModelData::StrArray& v, const std::string& value) const
         {
            bool success = false;
            dtAnim::Cal3DModelData::StrArray::const_iterator curIter = v.begin();
            dtAnim::Cal3DModelData::StrArray::const_iterator endIter = v.end();
            for (; curIter != endIter; ++curIter)
            {
               if(*curIter == value)
               {
                  success = true;
                  break;
               }
            }

            return success;
         }

         void TestModelDataFileRegistration()
         {
            std::string modelPath = dtUtil::FindFileInPathList("SkeletalMeshes/Marine/marine_test.xml");
            CPPUNIT_ASSERT(!modelPath.empty());

            dtAnim::Cal3DDatabase& database = dtAnim::Cal3DDatabase::GetInstance();
            mHelper->LoadModel(modelPath);

            dtAnim::Cal3DModelWrapper* wrapper = mHelper->GetModelWrapper();
            CPPUNIT_ASSERT(wrapper != NULL);
            Cal3DModelData* modelData = database.GetModelData(*wrapper);
            CPPUNIT_ASSERT(modelData != NULL);

            // Test file extension classification.
            CPPUNIT_ASSERT(modelData->GetFileType("test.caf") == Cal3DModelData::ANIM_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.xaf") == Cal3DModelData::ANIM_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.crf") == Cal3DModelData::MAT_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.xrf") == Cal3DModelData::MAT_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.cmf") == Cal3DModelData::MESH_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.xmf") == Cal3DModelData::MESH_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.csf") == Cal3DModelData::SKEL_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.xsf") == Cal3DModelData::SKEL_FILE);
            CPPUNIT_ASSERT(modelData->GetFileType("test.zzz") == Cal3DModelData::NO_FILE);

            // Ensure that the actual files that were loaded are in the file mapping.
            dtAnim::Cal3DModelData::StrArray fileList;
            CPPUNIT_ASSERT(modelData->GetFileListForFileType(Cal3DModelData::SKEL_FILE, fileList) == 1);
            CPPUNIT_ASSERT(VectorHasValue(fileList, "Skeleton.csf"));
            fileList.clear();
            CPPUNIT_ASSERT(modelData->GetFileListForFileType(Cal3DModelData::ANIM_FILE, fileList) == 5);
            CPPUNIT_ASSERT(VectorHasValue(fileList, "LowWalk.caf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "LowWalk_Wpn.caf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "Breath.caf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "Run.caf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "Walk.caf"));
            fileList.clear();
            CPPUNIT_ASSERT(modelData->GetFileListForFileType(Cal3DModelData::MESH_FILE, fileList) == 4);
            CPPUNIT_ASSERT(VectorHasValue(fileList, "HEAD.cmf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "BODY.cmf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "Helmet.cmf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "M16m80.cmf"));
            fileList.clear();
            CPPUNIT_ASSERT(modelData->GetFileListForFileType(Cal3DModelData::MAT_FILE, fileList) == 4);
            CPPUNIT_ASSERT(VectorHasValue(fileList, "HEAD.crf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "BODY.crf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "Helmet.crf"));
            CPPUNIT_ASSERT(VectorHasValue(fileList, "M16m80.crf"));
            fileList.clear();

            // Test name list by file type
            dtAnim::Cal3DModelData::StrArray nameList;
            CPPUNIT_ASSERT(modelData->GetObjectNameListForFileType(Cal3DModelData::SKEL_FILE, nameList) == 1);
            CPPUNIT_ASSERT(modelData->GetFileCount(Cal3DModelData::SKEL_FILE) == 1);
            CPPUNIT_ASSERT(VectorHasValue(nameList, "skeleton"));
            nameList.clear();
            CPPUNIT_ASSERT(modelData->GetObjectNameListForFileType(Cal3DModelData::ANIM_FILE, nameList) == 5);
            CPPUNIT_ASSERT(modelData->GetFileCount(Cal3DModelData::ANIM_FILE) == 5);
            CPPUNIT_ASSERT(VectorHasValue(nameList, "LowWalk"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "LowWalk with weapon"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Idle"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Run"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Walk"));
            nameList.clear();
            CPPUNIT_ASSERT(modelData->GetObjectNameListForFileType(Cal3DModelData::MESH_FILE, nameList) == 4);
            CPPUNIT_ASSERT(modelData->GetFileCount(Cal3DModelData::MESH_FILE) == 4);
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Head"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Body"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Helmet"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "M16"));
            nameList.clear();
            CPPUNIT_ASSERT(modelData->GetObjectNameListForFileType(Cal3DModelData::MAT_FILE, nameList) == 4);
            CPPUNIT_ASSERT(modelData->GetFileCount(Cal3DModelData::MAT_FILE) == 4);
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Head Material"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Body Material"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "Helmet Material"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "M16 Material"));
            nameList.clear();

            // Test access to a file name via an object name and file type.
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(Cal3DModelData::SKEL_FILE, "skeleton") == "Skeleton.csf");
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(Cal3DModelData::MAT_FILE, "Helmet Material") == "Helmet.crf");
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(Cal3DModelData::MESH_FILE, "Head") == "HEAD.cmf");
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(Cal3DModelData::ANIM_FILE, "LowWalk with weapon") == "LowWalk_Wpn.caf");


            // Test mapping multiple object names to a single file.
            std::string testFile("test.xaf");
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "A"));
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "B"));
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "C"));
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "D"));

            CPPUNIT_ASSERT(modelData->GetObjectNameListForFile(testFile, nameList) == 4);
            CPPUNIT_ASSERT(VectorHasValue(nameList, "A"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "B"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "C"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "D"));
            nameList.clear();


            // Test Un-registering.
            // --- Test removal by incorrect type (nothing should happen).
            CPPUNIT_ASSERT(modelData->UnregisterObjectName("C", ".cmf", &fileList) == 0);
            CPPUNIT_ASSERT( ! VectorHasValue(fileList, testFile));
            CPPUNIT_ASSERT(fileList.empty());
            // --- Now actually remove it.
            CPPUNIT_ASSERT(modelData->UnregisterObjectName("C", ".caf", &fileList) == 1);
            CPPUNIT_ASSERT(VectorHasValue(fileList, testFile));
            fileList.clear();
            // --- Ensure it cannot be done again.
            CPPUNIT_ASSERT(modelData->UnregisterObjectName("C", ".caf", &fileList) == 0);
            CPPUNIT_ASSERT( ! VectorHasValue(fileList, testFile));
            CPPUNIT_ASSERT(fileList.empty());


            CPPUNIT_ASSERT(modelData->UnregisterFile(testFile, &nameList) == 3);
            CPPUNIT_ASSERT(VectorHasValue(nameList, "A"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "B"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "D"));
            nameList.clear();
            // --- Ensure it cannot be done again.
            CPPUNIT_ASSERT(modelData->UnregisterFile(testFile, &nameList) == 0);
            CPPUNIT_ASSERT( ! VectorHasValue(nameList, "A"));
            CPPUNIT_ASSERT( ! VectorHasValue(nameList, "B"));
            CPPUNIT_ASSERT( ! VectorHasValue(nameList, "D"));
            CPPUNIT_ASSERT(nameList.empty());


            // Re-register for more tests.
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "A"));
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "B"));
            CPPUNIT_ASSERT(modelData->RegisterFile(testFile, "D"));

            // Test changing an object name.
            dtAnim::Cal3DModelData::CalFileType fileType = dtAnim::Cal3DModelData::ANIM_FILE;
            CPPUNIT_ASSERT( ! modelData->ReplaceObjectName(fileType, "A", "B"));
            CPPUNIT_ASSERT(modelData->ReplaceObjectName(fileType, "A", "C"));
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(fileType, "C") == testFile);
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(fileType, "A").empty());

            CPPUNIT_ASSERT(modelData->GetObjectNameListForFile(testFile, nameList) == 3);
            CPPUNIT_ASSERT( ! VectorHasValue(nameList, "A"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "B"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "C"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "D"));
            nameList.clear();

            // Test changing a file that an object name is mapped to.
            std::string newTestFile("newTestFile.caf");
            CPPUNIT_ASSERT( ! modelData->SetFileForObjectName(fileType, "D", testFile));
            CPPUNIT_ASSERT(modelData->SetFileForObjectName(fileType, "D", newTestFile));
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(fileType, "D") != testFile);
            CPPUNIT_ASSERT(modelData->GetFileForObjectName(fileType, "D") == newTestFile);

            CPPUNIT_ASSERT(modelData->GetObjectNameListForFile(testFile, nameList) == 2);
            CPPUNIT_ASSERT( ! VectorHasValue(nameList, "A"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "B"));
            CPPUNIT_ASSERT(VectorHasValue(nameList, "C"));
            CPPUNIT_ASSERT( ! VectorHasValue(nameList, "D"));
            nameList.clear();
         }

      private:

         void TestEmptyHelper()
         {
            CPPUNIT_ASSERT(mHelper->GetNode() == NULL);
            CPPUNIT_ASSERT(mHelper->GetAnimator() == NULL);
            CPPUNIT_ASSERT(mHelper->GetModelWrapper() == NULL);
            std::vector<const Animatable*> toFill;
            mHelper->GetSequenceMixer().GetRegisteredAnimations(toFill);
            CPPUNIT_ASSERT_EQUAL(0U, unsigned(toFill.size()));
         }

         void TestLoadedAnimatable(const Animatable* anim,
               const std::string& name, float startDelay,
               float fadeIn, float fadeOut,
               float speed, float baseWeight)
         {
            CPPUNIT_ASSERT(anim != NULL);
            CPPUNIT_ASSERT_EQUAL(name, anim->GetName());
            CPPUNIT_ASSERT_DOUBLES_EQUAL(startDelay, anim->GetStartDelay(), 0.001f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fadeIn, anim->GetFadeIn(), 0.001f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fadeOut, anim->GetFadeOut(), 0.001f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(speed, anim->GetSpeed(), 0.001f);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(baseWeight, anim->GetBaseWeight(), 0.001f);
         }

         void TestLoadedAnimationChannel(const AnimationChannel* channel,
               float maxDuration, bool isAction, bool isLooping)
         {
            CPPUNIT_ASSERT(channel != NULL);
            CPPUNIT_ASSERT_EQUAL(maxDuration, channel->GetMaxDuration());
            CPPUNIT_ASSERT_EQUAL(isAction, channel->IsAction());
            CPPUNIT_ASSERT_EQUAL(isLooping, channel->IsLooping());
         }

         void TestLoadedAnimationSequence(const AnimationSequence* sequence,
               const std::vector<std::string>& childNames)
         {
            CPPUNIT_ASSERT(sequence != NULL);
            const AnimationSequence::AnimationContainer& children = sequence->GetChildAnimations();

            CPPUNIT_ASSERT_EQUAL(childNames.size(), children.size());

            AnimationSequence::AnimationContainer::const_iterator i = children.begin();
            std::vector<std::string>::const_iterator j = childNames.begin();
            for (;i != children.end(); ++i, ++j)
            {
               CPPUNIT_ASSERT_EQUAL(*j, (*i)->GetName());
            }
         }

         dtCore::RefPtr<dtAnim::AnimationHelper> mHelper;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( Cal3DLoaderTests );



   class SequenceLoadingTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(SequenceLoadingTests);
         CPPUNIT_TEST(TestFollowsFlag);
         CPPUNIT_TEST(TestFollowsFlag_Split);
         CPPUNIT_TEST(TestFollowsName);
         CPPUNIT_TEST(TestCrossFade);
      CPPUNIT_TEST_SUITE_END();

   public:
      typedef CharacterFileHandler::AnimationSequenceStruct SequenceInfo;
      typedef CharacterFileHandler::AnimatableOverrideStruct AnimOverride;
      typedef CharacterFileHandler::AnimatableOverrideStructArray AnimOverrideArray2;

      static const dtUtil::RefString NAME_1;
      static const dtUtil::RefString NAME_2;
      static const dtUtil::RefString NAME_3;
      static const dtUtil::RefString NAME_4;
      static const dtUtil::RefString NAME_5;
      static const float EPSILON;

      SequenceLoadingTests()
      {
      }

      void SetDefaults(AnimOverride& info, Animatable& anim,
         AnimationWrapper& wrapper, const std::string& name)
      {
         info.mName = name;
         info.mSpeed = 1.0f;

         wrapper.SetSpeed(1.0f);
         wrapper.SetDuration(1.0f);

         anim.SetName(name);
         anim.SetSpeed(1.0f);

         AnimationChannel* channel = dynamic_cast<AnimationChannel*>(&anim);
         if (channel != NULL)
         {
            channel->SetAnimation(&wrapper);
            channel->SetMaxDuration(channel->GetMaxDuration());
            channel->SetLooping(false);
         }
      }

      void setUp()
      {
         // Create the test objects.
         AnimOverrideArray2& animInfoArray = mSeqInfo.GetChildren();
         animInfoArray.reserve(5);
         AnimOverride tmp;
         for(int i = 0; i < 5; ++i)
         {
            animInfoArray.push_back(tmp);
         }

         mInfo1 = &animInfoArray[0];
         mInfo2 = &animInfoArray[1];
         mInfo3 = &animInfoArray[2];
         mInfo4 = &animInfoArray[3];
         mInfo5 = &animInfoArray[4];

         mAnim1 = new AnimationChannel();
         mAnim2 = new AnimationChannel();
         mAnim3 = new AnimationChannel();
         mAnim4 = new AnimationChannel();
         mAnim5 = new AnimationChannel();

         mAnimArray.push_back(mAnim1.get());
         mAnimArray.push_back(mAnim2.get());
         mAnimArray.push_back(mAnim3.get());
         mAnimArray.push_back(mAnim4.get());
         mAnimArray.push_back(mAnim5.get());

         mAnimWrapper1 = new AnimationWrapper(NAME_1, 0);
         mAnimWrapper2 = new AnimationWrapper(NAME_2, 1);
         mAnimWrapper3 = new AnimationWrapper(NAME_3, 2);
         mAnimWrapper4 = new AnimationWrapper(NAME_4, 3);
         mAnimWrapper5 = new AnimationWrapper(NAME_5, 4);

         // Setup default values.
         SetDefaults(*mInfo1, *mAnim1, *mAnimWrapper1, NAME_1);
         SetDefaults(*mInfo2, *mAnim2, *mAnimWrapper2, NAME_2);
         SetDefaults(*mInfo3, *mAnim3, *mAnimWrapper3, NAME_3);
         SetDefaults(*mInfo4, *mAnim4, *mAnimWrapper4, NAME_4);
         SetDefaults(*mInfo5, *mAnim5, *mAnimWrapper5, NAME_5);
      }

      void tearDown()
      {
         mSeqInfo.GetChildren().clear();
         mInfo1 = NULL;
         mInfo2 = NULL;
         mInfo3 = NULL;
         mInfo4 = NULL;
         mInfo5 = NULL;

         mAnimArray.clear();
         mAnim1 = NULL;
         mAnim2 = NULL;
         mAnim3 = NULL;
         mAnim4 = NULL;
         mAnim5 = NULL;

         mAnimWrapper1 = NULL;
         mAnimWrapper2 = NULL;
         mAnimWrapper3 = NULL;
         mAnimWrapper4 = NULL;
         mAnimWrapper5 = NULL;
      }

      void TestFollowsFlag()
      {
         // Setup test values.
         mInfo2->mFollowsPrevious = true;
         mInfo3->mFollowsPrevious = true;
         mInfo4->mFollowsPrevious = true;
         mInfo5->mFollowsPrevious = true;

         Cal3DLoader::FinalizeSequenceInfo(mSeqInfo, mAnimArray);

         CPPUNIT_ASSERT( ! mInfo1->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo2->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo3->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo4->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo5->mOverrideStartDelay);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, mInfo1->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, mInfo2->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, mInfo3->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, mInfo4->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, mInfo5->mStartDelay, EPSILON);
      }

      void TestFollowsFlag_Split()
      {
         // Setup test values.
         mInfo2->mFollowsPrevious = true;
         mInfo3->mFollowsPrevious = true;
         mInfo5->mFollowsPrevious = true;

         Cal3DLoader::FinalizeSequenceInfo(mSeqInfo, mAnimArray);

         CPPUNIT_ASSERT( ! mInfo1->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo2->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo3->mOverrideStartDelay);
         CPPUNIT_ASSERT( ! mInfo4->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo5->mOverrideStartDelay);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, mInfo1->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, mInfo2->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, mInfo3->mStartDelay, EPSILON);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, mInfo4->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, mInfo5->mStartDelay, EPSILON);
      }

      void TestFollowsName()
      {
         // Setup test values.
         mInfo1->mFollowAnimatableName = NAME_5;
         mInfo3->mFollowAnimatableName = NAME_1;
         mInfo2->mFollowAnimatableName = NAME_3;
         mInfo4->mFollowAnimatableName = NAME_2;

         Cal3DLoader::FinalizeSequenceInfo(mSeqInfo, mAnimArray);

         CPPUNIT_ASSERT( ! mInfo5->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo1->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo3->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo2->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo4->mOverrideStartDelay);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, mInfo5->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, mInfo1->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, mInfo3->mStartDelay, EPSILON);

         // For now the start delays are not as expected for the following
         // since some of the elements are not processed out of order...
         //CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, mInfo2->mStartDelay, EPSILON);
         //CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, mInfo4->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, mInfo2->mStartDelay, EPSILON); // 2 processed before 3 was.
         CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0f, mInfo4->mStartDelay, EPSILON); // 2 processed before 3 was so throws off results for 4.
      }

      void TestCrossFade()
      {
         // Setup test values.
         mInfo2->mFollowsPrevious = true;
         mInfo2->mCrossFade = 0.25f;

         mInfo3->mFollowsPrevious = true;
         mInfo3->mCrossFade = 0.0f;

         mInfo4->mFollowsPrevious = true;
         mInfo4->mCrossFade = 0.25f;

         mInfo5->mFollowsPrevious = true;
         mInfo5->mCrossFade = 0.0f;

         Cal3DLoader::FinalizeSequenceInfo(mSeqInfo, mAnimArray);

         CPPUNIT_ASSERT( ! mInfo1->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo2->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo3->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo4->mOverrideStartDelay);
         CPPUNIT_ASSERT(mInfo5->mOverrideStartDelay);

         CPPUNIT_ASSERT( ! mInfo1->mOverrideFadeIn);
         CPPUNIT_ASSERT(mInfo2->mOverrideFadeIn);
         CPPUNIT_ASSERT( ! mInfo3->mOverrideFadeIn);
         CPPUNIT_ASSERT(mInfo4->mOverrideFadeIn);
         CPPUNIT_ASSERT( ! mInfo5->mOverrideFadeIn);

         CPPUNIT_ASSERT(mInfo1->mOverrideFadeOut);
         CPPUNIT_ASSERT( ! mInfo2->mOverrideFadeOut);
         CPPUNIT_ASSERT(mInfo3->mOverrideFadeOut);
         CPPUNIT_ASSERT( ! mInfo4->mOverrideFadeOut);
         CPPUNIT_ASSERT( ! mInfo5->mOverrideFadeOut);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, mInfo1->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.75f, mInfo2->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.75f, mInfo3->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(2.5f, mInfo4->mStartDelay, EPSILON);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(3.5f, mInfo5->mStartDelay, EPSILON);
      }

   private:
      Cal3DModelData::AnimatableArray mAnimArray;
      dtCore::RefPtr<Animatable> mAnim1;
      dtCore::RefPtr<Animatable> mAnim2;
      dtCore::RefPtr<Animatable> mAnim3;
      dtCore::RefPtr<Animatable> mAnim4;
      dtCore::RefPtr<Animatable> mAnim5;

      dtCore::RefPtr<AnimationWrapper> mAnimWrapper1;
      dtCore::RefPtr<AnimationWrapper> mAnimWrapper2;
      dtCore::RefPtr<AnimationWrapper> mAnimWrapper3;
      dtCore::RefPtr<AnimationWrapper> mAnimWrapper4;
      dtCore::RefPtr<AnimationWrapper> mAnimWrapper5;

      SequenceInfo mSeqInfo;
      AnimOverride* mInfo1;
      AnimOverride* mInfo2;
      AnimOverride* mInfo3;
      AnimOverride* mInfo4;
      AnimOverride* mInfo5;
   };

   const dtUtil::RefString SequenceLoadingTests::NAME_1("Anim1");
   const dtUtil::RefString SequenceLoadingTests::NAME_2("Anim2");
   const dtUtil::RefString SequenceLoadingTests::NAME_3("Anim3");
   const dtUtil::RefString SequenceLoadingTests::NAME_4("Anim4");
   const dtUtil::RefString SequenceLoadingTests::NAME_5("Anim5");
   const float SequenceLoadingTests::EPSILON = 0.001f;

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( SequenceLoadingTests );
}
