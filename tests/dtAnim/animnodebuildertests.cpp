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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/submesh.h>

#include <dtDAL/project.h>

#include <dtABC/application.h>
#include <dtCore/globals.h>
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
      
      CPPUNIT_TEST_SUITE( AnimNodeBuilderTests );
      CPPUNIT_TEST( TestBuildSoftware );
      CPPUNIT_TEST( TestBuildHardware );
      CPPUNIT_TEST_SUITE_END();
      
      public:
         AnimNodeBuilderTests()
         {
         }

         void setUp()
         {
            
            dtABC::Application& app = GetGlobalApplication();
            mScene = app.GetScene();
            mCamera = app.GetCamera();
            mWin = app.GetWindow();
            
            dtCore::System::GetInstance().Config();

            dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
            dtCore::System::GetInstance().Start();

            mHelper = new AnimationHelper();
            dtDAL::Project::GetInstance().SetContext(dtCore::GetDeltaRootPath() + "/examples/data/demoMap");
            
            mModelPath = dtCore::FindFileInPathList("SkeletalMeshes/marine_test.xml");
            CPPUNIT_ASSERT(!mModelPath.empty());
         }
         
         void tearDown()
         {
            mHelper = NULL;
            mModelPath.clear();

            mScene = NULL;
            mCamera = NULL;
            mWin = NULL;

            dtCore::System::GetInstance().Stop();
         }
         
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
            CheckGeode(dynamic_cast<osg::Geode*>(node), true);
            Cal3DModelData* modelData = Cal3DDatabase::GetInstance().GetModelData(*mHelper->GetModelWrapper());
            CPPUNIT_ASSERT(modelData->GetVertexVBO() != 0);
            CPPUNIT_ASSERT(modelData->GetIndexVBO() != 0);
            CPPUNIT_ASSERT(modelData->GetIndexVBO() != modelData->GetVertexVBO());

            dtCore::RefPtr<dtAnim::AnimationHelper> secondHelper = new AnimationHelper();
            secondHelper->LoadModel(mModelPath);
            Cal3DModelData* secondModelData = Cal3DDatabase::GetInstance().GetModelData(*secondHelper->GetModelWrapper());

            CPPUNIT_ASSERT_EQUAL_MESSAGE("The first and second hardware meshes should share VBO's", 
                  secondModelData->GetVertexVBO(), modelData->GetVertexVBO());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The first and second hardware meshes should share VBO's", 
                  secondModelData->GetIndexVBO(), modelData->GetIndexVBO());
         }
         
         void TestBuildSoftware()
         {
            AnimNodeBuilder& nodeBuilder = Cal3DDatabase::GetInstance().GetNodeBuilder();
            nodeBuilder.SetCreate(AnimNodeBuilder::CreateFunc(&nodeBuilder, &AnimNodeBuilder::CreateSoftware));
            mHelper->LoadModel(mModelPath);

            CheckGeode(dynamic_cast<osg::Geode*>(mHelper->GetNode()), false);
            
         }
         
      private:
         
         void CheckGeode(osg::Geode* toCheck, bool hardware)
         {
            CPPUNIT_ASSERT(toCheck != NULL);
            CPPUNIT_ASSERT(toCheck->getNumDrawables() > 0);
            
            for (unsigned i = 0; i < toCheck->getNumDrawables(); ++i)
            {
               osg::Drawable* draw = toCheck->getDrawable(i);
               if (hardware)
               {
                  CPPUNIT_ASSERT_MESSAGE("the default geode should have only hardware submeshes", 
                        dynamic_cast<dtAnim::HardwareSubmeshDrawable*>(draw) != NULL);
               }
               else
               {
                  CPPUNIT_ASSERT_MESSAGE("the default geode should have only software submeshes", 
                        dynamic_cast<dtAnim::SubmeshDrawable*>(draw) != NULL);
               }
            }
            AnimNodeBuilder::Cal3DBoundingSphereCalculator* sphereCallback = 
               dynamic_cast<AnimNodeBuilder::Cal3DBoundingSphereCalculator*>(toCheck->getComputeBoundingSphereCallback());
            CPPUNIT_ASSERT(sphereCallback != NULL);
         }

         
         std::string mModelPath;
         dtCore::RefPtr<dtAnim::AnimationHelper> mHelper;

         dtCore::RefPtr<dtCore::Scene>          mScene;
         dtCore::RefPtr<dtCore::Camera>         mCamera;
         dtCore::RefPtr<dtCore::DeltaWin>       mWin;
         dtCore::RefPtr<dtCore::View>           mView;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( AnimNodeBuilderTests );
}
