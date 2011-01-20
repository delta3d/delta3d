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

#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/submesh.h>

#include <dtDAL/project.h>
#include <dtUtil/datapathutils.h>

#include <dtABC/application.h>
#include <dtCore/refptr.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <osg/Geode>
#include <osg/Drawable>
#include <osgViewer/Viewer>

#include <sstream>

#include <string>

extern dtABC::Application& GetGlobalApplication();

namespace dtAnim
{
   class AnimNodeBuilderTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(AnimNodeBuilderTests);
         CPPUNIT_TEST(TestBuildSoftware);
         CPPUNIT_TEST(TestBuildHardware);
      CPPUNIT_TEST_SUITE_END();

   public:
      AnimNodeBuilderTests()
      {
      }

      void setUp()
      {
         dtCore::System::GetInstance().Config();

         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mHelper = new AnimationHelper();
         dtDAL::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data/demoMap");

         mModelPath = dtUtil::FindFileInPathList("SkeletalMeshes/marine_test.xml");
         CPPUNIT_ASSERT(!mModelPath.empty());
      }

      void tearDown()
      {
         mHelper = NULL;
         mModelPath.clear();

         dtCore::System::GetInstance().Stop();
      }


      //////////////////////////////////////////////////////////////////////////
      class TestDrawable : public dtCore::DeltaDrawable
      {
      public:
         TestDrawable(osg::Node& node):
            mNode(&node)
            {
            }

            virtual osg::Node* GetOSGNode() {return mNode.get();}
            virtual const osg::Node* GetOSGNode() const {return mNode.get();}

      protected:
         virtual ~TestDrawable()
         {
         }

      private:
         osg::ref_ptr<osg::Node> mNode;
      };


      //////////////////////////////////////////////////////////////////////////
      void TestBuildHardware()
      {
         AnimNodeBuilder& nodeBuilder = Cal3DDatabase::GetInstance().GetNodeBuilder();

         //see if we can even do hardware building...
         if (nodeBuilder.SupportsHardware() == false)
         {
            return;
         }

         nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateHardware));
         mHelper->LoadModel(mModelPath);

         osg::Node* node = mHelper->GetNode();
         dtCore::RefPtr<TestDrawable> drawable = new TestDrawable(*node);
         GetGlobalApplication().GetScene()->AddChild(drawable.get());

         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();

         const osg::Group* group = node->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group != NULL);

         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder group node doesn't have any children",
                                 group->getNumChildren() > 0);

         const osg::Geode* geode = dynamic_cast<const osg::Geode*>(group->getChild(0));
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder's first child isn't a Geode",
                                 geode != NULL);

         CheckGeode(geode, true);
         Cal3DModelData* modelData = Cal3DDatabase::GetInstance().GetModelData(*mHelper->GetModelWrapper());
         CPPUNIT_ASSERT(modelData->GetVertexBufferObject() != 0);
         CPPUNIT_ASSERT(modelData->GetElementBufferObject() != 0);

         dtCore::RefPtr<dtAnim::AnimationHelper> secondHelper = new AnimationHelper();
         secondHelper->LoadModel(mModelPath);
         Cal3DModelData* secondModelData = Cal3DDatabase::GetInstance().GetModelData(*secondHelper->GetModelWrapper());

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The first and second hardware meshes should share VBO's",
               secondModelData->GetVertexBufferObject(), modelData->GetVertexBufferObject());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The first and second hardware meshes should share VBO's",
               secondModelData->GetElementBufferObject(), modelData->GetElementBufferObject());

         GetGlobalApplication().GetScene()->RemoveChild(drawable.get());
      }

      ///////////////////////////////////////////////////////////////////////
      void TestBuildSoftware()
      {
         AnimNodeBuilder& nodeBuilder = Cal3DDatabase::GetInstance().GetNodeBuilder();

         if (nodeBuilder.SupportsSoftware() == false)
         {
            return;
         }

         nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateSoftware));
         mHelper->LoadModel(mModelPath);
         dtCore::RefPtr<osg::Node> node = mHelper->GetNode();

         dtCore::RefPtr<TestDrawable> drawable = new TestDrawable(*node);
         GetGlobalApplication().GetScene()->AddChild(drawable.get());
         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();

         const osg::Group* group = node->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group != NULL);

         const osg::Geode* geode = dynamic_cast<const osg::Geode*>(group->getChild(0));
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder's first child isn't a Geode",
                                 geode != NULL);

         CheckGeode(geode, false);

         GetGlobalApplication().GetScene()->RemoveChild(drawable.get());
      }

   private:
      void CheckGeode(const osg::Geode* toCheck, bool hardware)
      {
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBUilder didn't generate a valid node",
                                 toCheck != NULL);
         CPPUNIT_ASSERT(toCheck->getNumDrawables() > 0);

         bool hasSubmesh = false;

         for (unsigned i = 0; i < toCheck->getNumDrawables(); ++i)
         {
            const osg::Drawable* draw = toCheck->getDrawable(i);

            if (hardware)
            {
               hasSubmesh = dynamic_cast<const dtAnim::HardwareSubmeshDrawable*>(draw) != NULL;
            }
            else
            {
               hasSubmesh = dynamic_cast<const dtAnim::SubmeshDrawable*>(draw) != NULL;
            }

            if (hasSubmesh)
            {
               break;
            }
         }

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The Geode didn't have any of the anticipated Drawables",
                                      true, hasSubmesh);


         const AnimNodeBuilder::Cal3DBoundingSphereCalculator* sphereCallback =
            dynamic_cast<const AnimNodeBuilder::Cal3DBoundingSphereCalculator*>(toCheck->getComputeBoundingSphereCallback());
         CPPUNIT_ASSERT(sphereCallback != NULL);
      }


      std::string mModelPath;
      dtCore::RefPtr<dtAnim::AnimationHelper> mHelper;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(AnimNodeBuilderTests);
}
