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
#include "AnimModelLoadingTestFixture.h"
#include <dtAnim/animationhelper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/cal3dnodebuilder.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/modeldatabase.h>
#include <dtAnim/submesh.h>
#include <dtAnim/geometrybuilder.h>

#include <dtCore/project.h>
#include <dtCore/transform.h>
#include <dtUtil/datapathutils.h>

#include <dtABC/application.h>
#include <dtCore/refptr.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osgViewer/Viewer>

#include <sstream>

#include <string>

extern dtABC::Application& GetGlobalApplication();

namespace dtAnim
{
   class AnimNodeBuilderTests : public AnimModelLoadingTestFixture
   {
      CPPUNIT_TEST_SUITE(AnimNodeBuilderTests);
         CPPUNIT_TEST(TestBuildSoftware);
//The old hardware model crashes in many cases on OS X.  It's deprecated anyway.
#ifndef __APPLE__
         CPPUNIT_TEST(TestBuildHardware);
#endif  
         CPPUNIT_TEST(TestBuildOsgGeometry);
         CPPUNIT_TEST(TestBuildWithScale);
      CPPUNIT_TEST_SUITE_END();

   public:
      AnimNodeBuilderTests()
      {
      }

      void setUp() override
      {
         AnimModelLoadingTestFixture::setUp();
         mHelper = new AnimationHelper();
         Connect(mHelper);
         mModelPath = dtCore::ResourceDescriptor("SkeletalMeshes/Marine/marine_test.xml");

      }

      void tearDown() override
      {
         mHelper = NULL;
         AnimModelLoadingTestFixture::tearDown();
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


      GeometryBuilder gTestInstance;
      //////////////////////////////////////////////////////////////////////////
      void TestBuildOsgGeometry()
      {
         AnimNodeBuilder& nodeBuilder = *ModelDatabase::GetInstance().GetNodeBuilder();

         //see if we can even do hardware building...
         if (nodeBuilder.SupportsHardware() == false)
         {
            return;
         }

         nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&gTestInstance, &GeometryBuilder::CreateGeometry));
         LoadModel(mHelper, mModelPath);

         osg::Node* node = mHelper->GetNode();
         CPPUNIT_ASSERT(node != NULL);
         dtCore::RefPtr<TestDrawable> drawable = new TestDrawable(*node);
         GetGlobalApplication().GetScene()->AddChild(drawable.get());

         dtCore::System::GetInstance().Step(0.016f);
         dtCore::System::GetInstance().Step(0.016f);

         const osg::Group* group = node->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group != NULL);

         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder group node doesn't have any children",
                                 group->getNumChildren() > 0);

         const osg::Geode* geode = dynamic_cast<const osg::Geode*>(group->getChild(0));
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder's first child isn't a Geode",
                                 geode != NULL);

         CheckOsgGeode(geode);

         dtCore::RefPtr<dtAnim::AnimationHelper> secondHelper = new AnimationHelper();
         secondHelper->SetLoadModelAsynchronously(false);
         secondHelper->LoadModel(mModelPath);
         secondHelper->Update(0.016);

         osg::Node* node2 = secondHelper->GetNode();
         dtCore::RefPtr<TestDrawable> drawable2 = new TestDrawable(*node2);
         GetGlobalApplication().GetScene()->AddChild(drawable2.get());

         dtCore::System::GetInstance().Step(0.016f);
         dtCore::System::GetInstance().Step(0.016f);

         const osg::Group* group2 = node2->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group2 != NULL);

         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder group node doesn't have any children",
                                 group2->getNumChildren() > 0);

         const osg::Geode* geode2 = dynamic_cast<const osg::Geode*>(group2->getChild(0));
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder's first child isn't a Geode",
                                 geode2 != NULL);

         CPPUNIT_ASSERT(geode->getNumDrawables() > 0);
         CPPUNIT_ASSERT_EQUAL(geode->getNumDrawables(), geode2->getNumDrawables());

         dtCore::ObserverPtr<const osg::Referenced> testOb;
         dtCore::ObserverPtr<const osg::Referenced> mrOb = mHelper->GetModelWrapper();

         for (unsigned i = 0; i < geode->getNumDrawables(); ++i)
         {
            const osg::Drawable* draw = geode->getDrawable(i);
            const osg::Drawable* draw2 = geode2->getDrawable(i);

            CPPUNIT_ASSERT(draw != draw2);

            CPPUNIT_ASSERT_MESSAGE("The user data should be the same, showing it is cached.", draw->getUserData() == draw2->getUserData());
            const osg::Geometry* g = dynamic_cast<const osg::Geometry*>(draw->getUserData());
            const osg::Geometry* g2 = dynamic_cast<const osg::Geometry*>(draw2->getUserData());
            CPPUNIT_ASSERT(g != NULL);
            CPPUNIT_ASSERT(g2 != NULL);

            CPPUNIT_ASSERT(g2->getVertexArray() == g->getVertexArray());
            testOb = draw->getUserData();
         }


         GetGlobalApplication().GetScene()->RemoveChild(drawable.get());
         GetGlobalApplication().GetScene()->RemoveChild(drawable2.get());
         CPPUNIT_ASSERT_EQUAL(2, testOb->referenceCount());
         drawable = NULL;
         drawable2 = NULL;
         mHelper->UnloadModel();
         if (mrOb.valid())
         {
            CPPUNIT_ASSERT_EQUAL(0, mrOb->referenceCount());
         }
         CPPUNIT_ASSERT(!mrOb.valid());
         CPPUNIT_ASSERT_EQUAL(1, testOb->referenceCount());
         secondHelper->UnloadModel();
         if (testOb.valid())
         {
            CPPUNIT_ASSERT_EQUAL(0, testOb->referenceCount());
         }
         CPPUNIT_ASSERT(!testOb.valid());
      }

      //////////////////////////////////////////////////////////////////////////
      void TestBuildHardware()
      {
         AnimNodeBuilder* nodeBuilder = ModelDatabase::GetInstance().GetNodeBuilder();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder should be valid.", nodeBuilder != NULL);

         //see if we can even do hardware building...
         if (nodeBuilder->SupportsHardware() == false)
         {
            return;
         }

         nodeBuilder->SetCreate(AnimNodeBuilder::CreateFunc(nodeBuilder, &AnimNodeBuilder::CreateHardware));

         LoadModel(mHelper, mModelPath);

         osg::Node* node = mHelper->GetNode();
         CPPUNIT_ASSERT(node != NULL);
         dtCore::RefPtr<TestDrawable> drawable = new TestDrawable(*node);
         GetGlobalApplication().GetScene()->AddChild(drawable.get());

         dtCore::System::GetInstance().Step(0.016f);
         dtCore::System::GetInstance().Step(0.016f);

         const osg::Group* group = node->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group != NULL);

         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder group node doesn't have any children",
                                 group->getNumChildren() > 0);

         const osg::Geode* geode = dynamic_cast<const osg::Geode*>(group->getChild(0));
         CPPUNIT_ASSERT_MESSAGE(std::string("AnimNodeBuilder's first child isn't a Geode, it's a ") + group->getChild(0)->className(),
                                 geode != NULL);

         CheckGeode(geode, true);
         BaseModelData* modelData = mHelper->GetModelWrapper()->GetModelData();
         CPPUNIT_ASSERT(modelData->GetVertexBufferObject() != 0);
         CPPUNIT_ASSERT(modelData->GetElementBufferObject() != 0);

         dtCore::RefPtr<dtAnim::AnimationHelper> secondHelper = new AnimationHelper();
         secondHelper->LoadModel(mModelPath);
         BaseModelData* secondModelData = secondHelper->GetModelWrapper()->GetModelData();

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The first and second hardware meshes should share VBO's",
               secondModelData->GetVertexBufferObject(), modelData->GetVertexBufferObject());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The first and second hardware meshes should share VBO's",
               secondModelData->GetElementBufferObject(), modelData->GetElementBufferObject());

         GetGlobalApplication().GetScene()->RemoveChild(drawable.get());
      }

      ///////////////////////////////////////////////////////////////////////
      void TestBuildSoftware()
      {
         AnimNodeBuilder* nodeBuilder = ModelDatabase::GetInstance().GetNodeBuilder();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder should be valid.", nodeBuilder != NULL);

         if (nodeBuilder->SupportsSoftware() == false)
         {
            return;
         }

         nodeBuilder->SetCreate(AnimNodeBuilder::CreateFunc(nodeBuilder, &AnimNodeBuilder::CreateSoftware));
         LoadModel(mHelper, mModelPath);
         dtCore::RefPtr<osg::Node> node = mHelper->GetNode();
         CPPUNIT_ASSERT(node.valid());

         dtCore::RefPtr<TestDrawable> drawable = new TestDrawable(*node);
         GetGlobalApplication().GetScene()->AddChild(drawable.get());
         dtCore::System::GetInstance().Step(0.016f);
         dtCore::System::GetInstance().Step(0.016f);

         const osg::Group* group = node->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group != NULL);

         const osg::Geode* geode = dynamic_cast<const osg::Geode*>(group->getChild(0));
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder's first child isn't a Geode, which should be the case if the scale is unity.",
                                 geode != NULL);

         CheckGeode(geode, false);

         GetGlobalApplication().GetScene()->RemoveChild(drawable.get());
      }

      void TestBuildWithScale()
      {
         dtAnim::ModelDatabase& database = ModelDatabase::GetInstance();

         AnimNodeBuilder& nodeBuilder = *database.GetNodeBuilder();

         if (nodeBuilder.SupportsSoftware() == false)
         {
            return;
         }

         nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateSoftware));

         CPPUNIT_ASSERT(database.Load(mModelPath));
         dtCore::RefPtr<dtAnim::BaseModelData> modelData;
         modelData = database.Find(mModelPath);
         CPPUNIT_ASSERT(modelData.valid());

         dtCore::RefPtr<BaseModelWrapper> wrapper = database.CreateModelWrapper(*modelData);
         CPPUNIT_ASSERT(wrapper.valid());
         CPPUNIT_ASSERT(wrapper->GetModelData() == modelData.get());

         float testScale = 3.5f;
         modelData->SetScale(testScale);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(testScale, modelData->GetScale(), 0.1f);
         LoadModel(mHelper, mModelPath);
         dtCore::RefPtr<osg::Node> node = mHelper->GetNode();
         // The model wrapper should get the scale from the cached database.
         CPPUNIT_ASSERT_DOUBLES_EQUAL(testScale, mHelper->GetModelWrapper()->GetScale(), 0.1f);

         dtCore::RefPtr<TestDrawable> drawable = new TestDrawable(*node);
         GetGlobalApplication().GetScene()->AddChild(drawable.get());
         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();

         const osg::Group* group = node->asGroup();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid group node",
                                 group != NULL);
         CPPUNIT_ASSERT_MESSAGE("The first node should be a transform because of the scale.", node->asTransform() != NULL);
         const osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node.get());
         CPPUNIT_ASSERT_MESSAGE("The first node should be a matrix transform because of the scale.", mt != NULL);
         dtCore::Transform xform;
         xform.Set(mt->getMatrix());
         osg::Vec3 curRow;
         for (unsigned i = 0; i < 3; ++i)
         {
            xform.GetRow(i, curRow);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Each row of the matrix should have the magnitude of the scale.", testScale, curRow.length(), 0.1f);
         }

         const osg::Group* group2 = node->asGroup()->getChild(0)->asGroup();
         CPPUNIT_ASSERT_MESSAGE("First child should be a group when scaled.",
                                 group2 != NULL);
         CPPUNIT_ASSERT_EQUAL(1U, group2->getNumChildren());
         const osg::Geode* geode =  group2->getChild(0)->asGeode();
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder's child of a child isn't a Geode",
                                 geode != NULL);

         CheckGeode(geode, false);

         GetGlobalApplication().GetScene()->RemoveChild(drawable.get());
      }
   private:
      void CheckOsgGeode(const osg::Geode* toCheck)
      {
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid node",
                                 toCheck != NULL);
         CPPUNIT_ASSERT(toCheck->getNumDrawables() > 0);

         bool hasSubmesh = false;

         for (unsigned i = 0; i < toCheck->getNumDrawables(); ++i)
         {
            const osg::Drawable* draw = toCheck->getDrawable(i);

            hasSubmesh = dynamic_cast<const osg::Geometry*>(draw) != NULL;

            if (hasSubmesh)
            {
               break;
            }
         }

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The Geode didn't have any of the anticipated Drawables",
                                      true, hasSubmesh);

      }

      void CheckGeode(const osg::Geode* toCheck, bool hardware)
      {
         CPPUNIT_ASSERT_MESSAGE("AnimNodeBuilder didn't generate a valid node",
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


         const dtAnim::Cal3DBoundingSphereCalculator* sphereCallback =
            dynamic_cast<const dtAnim::Cal3DBoundingSphereCalculator*>(toCheck->getComputeBoundingSphereCallback());
         CPPUNIT_ASSERT(sphereCallback != NULL);
      }


      dtCore::ResourceDescriptor mModelPath;
      dtCore::RefPtr<dtAnim::AnimationHelper> mHelper;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(AnimNodeBuilderTests);
}
