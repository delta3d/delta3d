/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author David Guthrie
 */
#include <prefix/dtgameprefix-src.h>

#include <dtUtil/fileutils.h>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtABC/application.h>

#include <cppunit/extensions/HelperMacros.h>

#include <string>

extern dtABC::Application& GetGlobalApplication();


class CameraTests : public CPPUNIT_NS::TestFixture
{
      CPPUNIT_TEST_SUITE(CameraTests);
   
         CPPUNIT_TEST(TestSaveScreenShot);
         CPPUNIT_TEST(TestPerspective);
         CPPUNIT_TEST(TestFrustum);
         CPPUNIT_TEST(TestEnabled);
         CPPUNIT_TEST(TestSettingTheCullingMode);
         CPPUNIT_TEST(TestSupplyingOSGCameraToConstructor);
   
      CPPUNIT_TEST_SUITE_END();
   
   public:

      void setUp() {}
      
      void tearDown()
      {      
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         
         if (fileUtils.DirExists(SCREEN_SHOT_DIR))
            fileUtils.DirDelete(SCREEN_SHOT_DIR, true);         
      }
      
      void TestSaveScreenShot();
      void TestPerspective();
      void TestFrustum();
      void TestSupplyingOSGCameraToConstructor();
      void TestEnabled();
      void TestSettingTheCullingMode();

   private:
      static const std::string SCREEN_SHOT_DIR;      
};

const std::string CameraTests::SCREEN_SHOT_DIR("TestScreenShot");

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CameraTests);


void CameraTests::TestSaveScreenShot()
{
   //uses the GlobalApplication.

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   fileUtils.MakeDirectory(SCREEN_SHOT_DIR);
   const std::string prefix = SCREEN_SHOT_DIR + "/Mojo";
   const std::string result = GetGlobalApplication().GetCamera()->TakeScreenShot(prefix);

   CPPUNIT_ASSERT_MESSAGE("The resulting string should be longer.", result.size() > prefix.size());

   CPPUNIT_ASSERT_MESSAGE("The result should begin with the prefix.", 
      prefix == result.substr(0,prefix.size()));

   CPPUNIT_ASSERT_MESSAGE(result + " should not exist yet.", !fileUtils.FileExists(result));

   const bool wasStarted = dtCore::System::GetInstance().IsRunning();
   if (wasStarted == false)
   {
      dtCore::System::GetInstance().Start();
   }

   dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE(result + " should exist.  Check for the jpeg osg plugin.", fileUtils.FileExists(result));

   fileUtils.DirDelete(SCREEN_SHOT_DIR, true);

   dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("A screenshot should not have been written again.", !fileUtils.FileExists(result));

   if (wasStarted == false)
   {
      dtCore::System::GetInstance().Stop();
   }
}

void CameraTests::TestPerspective()
{
   double vfovSet = 60.0;
   double aspectSet = 1.33;
   double nearSet = 1.0;
   double farSet = 10000.0;

   dtCore::RefPtr<dtCore::Camera> camera = new dtCore::Camera();
   camera->SetPerspectiveParams(vfovSet, aspectSet, nearSet, farSet);

   double vfov, aspectRatio, nearClip, farClip;
   camera->GetPerspectiveParams(vfov, aspectRatio, nearClip, farClip);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The vertical field of view should be the same as the one set", 
      vfovSet, vfov, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The aspect ratio should be the same as the one set", 
      aspectSet, aspectRatio, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The near plane should be the same as the one set", 
      nearSet, nearClip, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The far plane should be the same as the one set", 
      farSet, farClip, 0.01);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The aspect ratio method should return the one set", 
      aspectSet, camera->GetAspectRatio(), 0.01);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The vertical fov method should return the one set", 
      vfovSet, camera->GetVerticalFov(), 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The horizontal fov method should return vertical fov * aspectRatio.", 
      vfovSet * aspectSet, camera->GetHorizontalFov(), 0.01);
}

void CameraTests::TestFrustum()
{
   double leftSet = 0.8, rightSet = -0.8;
   double topSet = 0.8, bottomSet = -0.8;
   double nearSet = 1.0;
   double farSet = 10000.0;

   dtCore::RefPtr<dtCore::Camera> camera = new dtCore::Camera();
   camera->SetFrustum(leftSet, rightSet, bottomSet, topSet, nearSet, farSet);

   double left, right, bottom, top, nearClip, farClip;
   camera->GetFrustum(left, right, bottom, top, nearClip, farClip);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The left plane should be the same as the one set", 
      leftSet, left, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The right plane should be the same as the one set", 
      rightSet, right, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The top plane should be the same as the one set", 
      topSet, top, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The bottom plane should be the same as the one set", 
      bottomSet, bottom, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The near plane should be the same as the one set", 
      nearSet, nearClip, 0.01);
   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The far plane should be the same as the one set", 
      farSet, farClip, 0.01);
}

void CameraTests::TestSupplyingOSGCameraToConstructor()
{
   osg::ref_ptr<osg::Camera> osgCam = new osg::Camera();
   dtCore::RefPtr<dtCore::Camera> camera = new dtCore::Camera(*osgCam);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Camera didn't return back the supplied OSG Camera",
      osgCam.get(), camera->GetOSGCamera() );
}


void CameraTests::TestEnabled()
{
   using namespace dtCore;
   RefPtr<Camera> cam = new Camera();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Camera should be enabled", true, cam->GetEnabled());
   CPPUNIT_ASSERT_MESSAGE("Node mask should not be 0x0", unsigned(0x0) != cam->GetOSGCamera()->getNodeMask() );

   cam->SetEnabled(false);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Camera should be disabled", false, cam->GetEnabled());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Node mask should be 0x0", unsigned(0x0), cam->GetOSGCamera()->getNodeMask() );


   //check if the node mask we set it to remains after toggling it off/on
   cam->SetEnabled(true);
   const unsigned int nodeMask = 0x00001111;
   cam->GetOSGCamera()->setNodeMask(nodeMask);
   cam->SetEnabled(false);
   cam->SetEnabled(true);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Node mask should be what it was set to before being disabled",
      nodeMask, cam->GetOSGCamera()->getNodeMask());
}

void CameraTests::TestSettingTheCullingMode()
{
   dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera();
   osg::Camera *osgCam = cam->GetOSGCamera();

   cam->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("should be DO_NOT_COMPUTE_NEAR_FAR", 
      osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR, 
      osgCam->getComputeNearFarMode() );

   cam->SetNearFarCullingMode(dtCore::Camera::BOUNDING_VOLUME_NEAR_FAR);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("should be COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES", 
      osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES, 
      osgCam->getComputeNearFarMode() );

   cam->SetNearFarCullingMode(dtCore::Camera::PRIMITIVE_NEAR_FAR);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("should be COMPUTE_NEAR_FAR_USING_PRIMITIVES", 
      osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES, 
      osgCam->getComputeNearFarMode() );
}
