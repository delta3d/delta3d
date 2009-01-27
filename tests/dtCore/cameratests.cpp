/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
* @author David Guthrie
*/
#include <prefix/dtgameprefix-src.h>

#include <dtUtil/fileutils.h>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/cameradrawcallback.h>
#include <dtCore/cameracallbackcontainer.h>
#include <dtCore/screenshotcallback.h>
#include <dtUtil/functor.h>
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
         CPPUNIT_TEST(TestCameraSyncCallbacks);
         CPPUNIT_TEST(TestSettingTheCullingMode);
         CPPUNIT_TEST(TestSupplyingOSGCameraToConstructor);
         CPPUNIT_TEST(TestOnScreen);
         CPPUNIT_TEST(TestInitialCallbackConditions);
         CPPUNIT_TEST(TestAddingTwoCallbacks);
         CPPUNIT_TEST(TestTriggeringCallback);

      CPPUNIT_TEST_SUITE_END();

   public:

      void setUp()
      {
         mCalledCallback = false;
         mCamerasThatCalled.clear();
         dtCore::System::GetInstance().Start();
      }

      void tearDown()
      {
         dtCore::System::GetInstance().Stop();
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

         if (fileUtils.DirExists(SCREEN_SHOT_DIR))
            fileUtils.DirDelete(SCREEN_SHOT_DIR, true);

         mCalledCallback = false;
         mCamerasThatCalled.clear();
      }

      void TestSaveScreenShot();
      void TestPerspective();
      void TestFrustum();
      void TestSupplyingOSGCameraToConstructor();
      void TestEnabled();
      void TestCameraSyncCallbacks();
      void TestSettingTheCullingMode();
      void TestInitialCallbackConditions();
      void TestAddingTwoCallbacks();
      void TestTriggeringCallback();

      void TestOnScreen()
      {
         dtCore::Camera& camera = *GetGlobalApplication().GetCamera();
         dtCore::Transform xform;
         xform.SetTranslation(osg::Vec3(0.0, 0.0, 0.0));
         camera.SetTransform(xform);
         osg::Vec3 testPos(0.0, 5.0, 0.0);

         osg::Vec3d screenPos;
         bool onScreen = camera.ConvertWorldCoordinateToScreenCoordinate(testPos, screenPos);
         CPPUNIT_ASSERT(onScreen);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, screenPos.x(), 0.001);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, screenPos.y(), 0.001);
         CPPUNIT_ASSERT(screenPos.z() > 0.0);

         testPos.set(0.0, -10.0, 0.0);
         onScreen = camera.ConvertWorldCoordinateToScreenCoordinate(testPos, screenPos);
         CPPUNIT_ASSERT(!onScreen);

         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, screenPos.x(), 0.001);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, screenPos.y(), 0.001);
         CPPUNIT_ASSERT(screenPos.z() < 0.0);

      }

   private:
      void TestCallback(dtCore::Camera& camera);
      static const std::string SCREEN_SHOT_DIR;
      bool mCalledCallback;
      std::vector<dtCore::Camera*> mCamerasThatCalled;
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

   cam->SetEnabled(false);
}

void CameraTests::TestCallback(dtCore::Camera& camera)
{
   mCalledCallback = true;
   mCamerasThatCalled.push_back(&camera);
}

void CameraTests::TestCameraSyncCallbacks()
{
   dtCore::RefPtr<osg::Referenced> testKeyObject = new osg::Referenced;
   using namespace dtCore;
   RefPtr<Camera> cam = new Camera();
   Camera::AddCameraSyncCallback(*testKeyObject, Camera::CameraSyncCallback(this, &CameraTests::TestCallback));

   CPPUNIT_ASSERT(!mCalledCallback);
   dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT(mCalledCallback);
   CPPUNIT_ASSERT_MESSAGE("The new camera should have fired the callback.",
            std::find(mCamerasThatCalled.begin(), mCamerasThatCalled.end(), cam.get()) != mCamerasThatCalled.end());

   mCalledCallback = false;
   mCamerasThatCalled.clear();

   Camera::RemoveCameraSyncCallback(*testKeyObject);
   dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("The callback should have been removed", !mCalledCallback);

   mCalledCallback = false;
   mCamerasThatCalled.clear();
   Camera::AddCameraSyncCallback(*testKeyObject, Camera::CameraSyncCallback(this, &CameraTests::TestCallback));
   cam->SetEnabled(false);
   dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("The camera was disabled, it should not have fired the callback",
            std::find(mCamerasThatCalled.begin(), mCamerasThatCalled.end(), cam.get()) == mCamerasThatCalled.end());
   cam->SetEnabled(true);

   mCalledCallback = false;
   mCamerasThatCalled.clear();
   testKeyObject = NULL;
   dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("The callback should have not been called backs the key object was deleted.", !mCalledCallback);

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

//////////////////////////////////////////////////////////////////////////
void CameraTests::TestInitialCallbackConditions()
{
   dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera();
   
   osg::Camera::DrawCallback *osgCallback = cam->GetOSGCamera()->getPostDrawCallback();

   dtCore::CameraCallbackContainer *cont = dynamic_cast<dtCore::CameraCallbackContainer*>(osgCallback);

   CPPUNIT_ASSERT_MESSAGE("The Camera's postdrawcallback isn't set to the correct default type.",
                           cont != NULL);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("The Camera should have at least one initial callback for screenshots.",
                               false, cont->GetCallbacks().empty());

   dtCore::CameraDrawCallback* cb = cont->GetCallbacks()[0].get();

   dtCore::ScreenShotCallback* screenCB = dynamic_cast<dtCore::ScreenShotCallback*>(cb);
   CPPUNIT_ASSERT_MESSAGE("The first Camera Callback should be the screenshot callback.",
                          screenCB != NULL);
}

class TestCB : public dtCore::CameraDrawCallback
{
public:
   TestCB():
   mTriggered(false)
   {
   }

   virtual void operator () (const dtCore::Camera& camera, osg::RenderInfo& renderInfo) const
   {
      const_cast<TestCB*>(this)->mTriggered = true;
      //mTriggered = true;
   }

   bool mTriggered;

protected:
   virtual ~TestCB()
   {
   }
};


//////////////////////////////////////////////////////////////////////////
void CameraTests::TestAddingTwoCallbacks()
{
   dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera();

   dtCore::CameraCallbackContainer *cont = dynamic_cast<dtCore::CameraCallbackContainer*>(cam->GetOSGCamera()->getPostDrawCallback());
   if (cont == NULL)  { return; }

   const size_t numInitialCallbacks = cont->GetCallbacks().size();

   dtCore::RefPtr<TestCB> cb1 = new TestCB();
   dtCore::RefPtr<TestCB> cb2 = new TestCB();

   cam->AddPostDrawCallback(*cb1);
   cam->AddPostDrawCallback(*cb2);


   CPPUNIT_ASSERT_EQUAL_MESSAGE("The CameraCallbackContainer didn't get the callbacks added.",
                           numInitialCallbacks + 2, cont->GetCallbacks().size());


   cam->RemovePostDrawCallback(*cb1);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The CameraCallbackContainer didn't get the first callback removed.",
                                numInitialCallbacks + 1, cont->GetCallbacks().size());

   cam->RemovePostDrawCallback(*cb2);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("The CameraCallbackContainer didn't get the second callback removed.",
                                numInitialCallbacks, cont->GetCallbacks().size());
}

//////////////////////////////////////////////////////////////////////////
void CameraTests::TestTriggeringCallback()
{
   dtCore::Camera* cam = GetGlobalApplication().GetCamera();

   dtCore::RefPtr<TestCB> cb1 = new TestCB();
   dtCore::RefPtr<TestCB> cb2 = new TestCB();
   cam->AddPostDrawCallback(*cb1);
   cam->AddPostDrawCallback(*cb2);

   dtCore::System::GetInstance().Start();
   dtCore::System::GetInstance().Step();
   dtCore::System::GetInstance().Stop();
 
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The Camera postdraw callback didn't trigger.",
                                true, cb1->mTriggered);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The second Camera postdraw callback didn't trigger.",
                                true, cb2->mTriggered);

   cam->RemovePostDrawCallback(*cb1);
   cam->RemovePostDrawCallback(*cb1);
}
