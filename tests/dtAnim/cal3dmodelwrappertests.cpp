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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animationhelper.h>

#include <dtDAL/project.h>

#include <dtCore/globals.h>
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
      CPPUNIT_TEST_SUITE( Cal3DModelWrapperTests );
      CPPUNIT_TEST( TestAttachDetachMesh );
      CPPUNIT_TEST( TestShowHideMesh );
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         dtDAL::Project::GetInstance().SetContext(dtCore::GetDeltaRootPath() + "/examples/data/demoMap");

         std::string modelPath = dtCore::FindFileInPathList("SkeletalMeshes/marine_test.xml");
         CPPUNIT_ASSERT_MESSAGE("Could not find \"SkeletalMeshes/marine_test.xml\"", !modelPath.empty());

         mAnimHelper = new dtAnim::AnimationHelper();
         mAnimHelper->LoadModel(modelPath);
         mModel = mAnimHelper->GetModelWrapper();
      }

      void tearDown()
      {
         mModel = NULL;
         mAnimHelper = NULL;
      }

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

      dtCore::RefPtr<AnimationHelper> mAnimHelper;
      dtCore::RefPtr<Cal3DModelWrapper> mModel;

   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( Cal3DModelWrapperTests );

}
