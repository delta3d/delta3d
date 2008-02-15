#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/view.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>

namespace dtTest
{
   class ViewTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(ViewTests);
         CPPUNIT_TEST(TestCameraSceneOrder);
      CPPUNIT_TEST_SUITE_END();

   public:
      ViewTests() {};
      ~ViewTests() {};

      void TestCameraSceneOrder();
   	
   };


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ViewTests);

   void ViewTests::TestCameraSceneOrder()
   {
      dtCore::RefPtr<dtCore::View> view = new dtCore::View();

      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
      view->SetScene( scene.get() );

      dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera();
      view->SetCamera( cam.get() );

      //scene's node should be the child node of the camera

      CPPUNIT_ASSERT_MESSAGE("Camera doesn't have the Scene node to render",
         cam->GetOSGCamera()->containsNode( scene->GetSceneNode()) );
   }

}
