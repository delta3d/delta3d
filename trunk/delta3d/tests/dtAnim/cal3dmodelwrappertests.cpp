/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008, Alion Science and Technology
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
            //These should return true.  I don't know how with the api to tell
            //if they were REALLY successful.
            CPPUNIT_ASSERT(mModel->AttachMesh(i));
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
