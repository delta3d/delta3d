/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
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
#include <dtCore/globals.h>

#include <dtDAL/project.h>

#include <osg/Geode>

#include <string>

namespace dtAnim
{
   class Cal3DLoaderTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( Cal3DLoaderTests );
         CPPUNIT_TEST( TestLoadFile );
         CPPUNIT_TEST( TestModelData );
      CPPUNIT_TEST_SUITE_END();

      public:
         Cal3DLoaderTests()
         {
         }
         
         void setUp()
         {
            dtDAL::Project::GetInstance().SetContext(dtCore::GetDeltaRootPath() + "/examples/data/demoMap");
            AnimNodeBuilder& nodeBuilder = Cal3DDatabase::GetInstance().GetNodeBuilder();
            nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateSoftware));
            mHelper = new dtAnim::AnimationHelper;
         }

         void tearDown()
         {
            Cal3DDatabase::GetInstance().TruncateDatabase();
            mHelper = NULL;
         }

         void TestLoadFile()
         {
            std::string modelPath = dtCore::FindFileInPathList("SkeletalMeshes/marine_test.xml");
            CPPUNIT_ASSERT(!modelPath.empty());

            std::string animName = "Walk";

            TestEmptyHelper();

            mHelper->LoadModel(modelPath);

            SequenceMixer& mixer = mHelper->GetSequenceMixer();
            
            std::vector<const Animatable*> toFill;
            mixer.GetRegisteredAnimations(toFill);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The number of animatables loaded from the file is incorrect.", 
                  size_t(5U), toFill.size());

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

            const AnimationSequence* runWalkSequence = dynamic_cast<const AnimationSequence*>(runWalkAnim);

            std::vector<std::string> childNames;
            childNames.push_back("Run");
            childNames.push_back("Walk");

            TestLoadedAnimationSequence(runWalkSequence, childNames);

            //Test unloading.
            CPPUNIT_ASSERT_NO_THROW(mHelper->LoadModel(""));
            TestEmptyHelper();
         }

         void TestModelData()
         {
            std::string modelPath = dtCore::FindFileInPathList("SkeletalMeshes/marine_test.xml");
            CPPUNIT_ASSERT(!modelPath.empty());

            dtAnim::Cal3DDatabase& database = dtAnim::Cal3DDatabase::GetInstance();
            mHelper->LoadModel(modelPath);

            dtAnim::Cal3DModelWrapper* wrapper = mHelper->GetModelWrapper();
            CPPUNIT_ASSERT(wrapper != NULL);
            Cal3DModelData* modelData = database.GetModelData(*wrapper);
            CPPUNIT_ASSERT(modelData != NULL);

            CPPUNIT_ASSERT_EQUAL(modelData->GetFilename(), modelPath);

            CPPUNIT_ASSERT_EQUAL(0U, modelData->GetVertexVBO());
            modelData->SetVertexVBO(4U);
            CPPUNIT_ASSERT_EQUAL(4U, modelData->GetVertexVBO());

            CPPUNIT_ASSERT_EQUAL(0U, modelData->GetIndexVBO());
            modelData->SetIndexVBO(4U);
            CPPUNIT_ASSERT_EQUAL(4U, modelData->GetIndexVBO());


            CPPUNIT_ASSERT(modelData->GetCoreModel() == wrapper->GetCalModel()->getCoreModel()); 

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
         }
         
      private:
         
         void TestEmptyHelper()
         {
            CPPUNIT_ASSERT(mHelper->GetGeode() == NULL);
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
}
