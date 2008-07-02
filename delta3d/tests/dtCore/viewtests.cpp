#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/view.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <osg/Version>

namespace dtTest
{
   class ViewTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(ViewTests);
         CPPUNIT_TEST(TestCameraSceneOrder);
         CPPUNIT_TEST(TestPagerPropogation);
         CPPUNIT_TEST(TestPagedLODRegistration);
      CPPUNIT_TEST_SUITE_END();

   public:
      ViewTests() {};
      ~ViewTests() {};

      void TestCameraSceneOrder();
      void TestPagerPropogation();
      void TestPagedLODRegistration();
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

   class TestScene : public dtCore::Scene
   {
   public:
      TestScene() {};
      dtCore::DatabasePager* GetPager() {return GetDatabasePager();}
   };
   
   ////////////////////////////////////////////////////////////////////////
   void ViewTests::TestPagerPropogation()
   {
      //verify that the dtCore::DatabasePager gets passed to the Scene
      dtCore::RefPtr<dtCore::View> view = new dtCore::View();
      CPPUNIT_ASSERT_MESSAGE("View should have a default DatabasePager",
                              view->GetDatabasePager() != NULL);

      dtCore::RefPtr<TestScene> scene = new TestScene();
      CPPUNIT_ASSERT_MESSAGE("Scene should not have a DatabasePager until added to a View",
                              scene->GetPager() == NULL);

      view->SetScene( scene.get() );
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene should have View's DatabasePager when added to View",
                                    view->GetDatabasePager(),
                                    scene->GetPager());

      view->SetDatabasePager(NULL);
      CPPUNIT_ASSERT_MESSAGE("View shouldn't have a DatabasePager when set to NULL",
                             view->GetDatabasePager() == NULL);

      CPPUNIT_ASSERT_MESSAGE("Scene's DatabasePager should be NULL if the View's is",
                              scene->GetPager() == NULL);
   }


   ///custom DatabasePager to get access to the PagedLOD list
   class TestPager : public osgDB::DatabasePager
   {
   public:
      TestPager() {};

      osgDB::DatabasePager::PagedLODList GetPagedLODList() 
      {
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 4
         return _pagedLODList;
#else
         return _activePagedLODList;
#endif
      }
   };

   ///custom drawable which sets up a couple PagedLOD Nodes
   class TestDrawable : public dtCore::DeltaDrawable
   {
   public:
      TestDrawable() 
      {
         mNode = new osg::Group();
         mNode->addChild( new osg::PagedLOD() );
         mNode->addChild( new osg::PagedLOD() );
      }
      
      const osg::Node *GetOSGNode(void) const  {return mNode.get();}
      osg::Node *GetOSGNode(void) {return mNode.get(); }

   private :
      osg::ref_ptr<osg::Group> mNode;

   };
   

   //////////////////////////////////////////////////////////////////////////
   void ViewTests::TestPagedLODRegistration()
   {
      //Verify when a DeltaDrawable containing PagedLOD nodes gets 
      //added to the Scene, that the DatabasePager knows about the nodes.
      dtCore::RefPtr<dtCore::View> view = new dtCore::View();

      osg::ref_ptr<TestPager> testPager = new TestPager();

      dtCore::RefPtr<dtCore::DatabasePager> pager = new dtCore::DatabasePager( *testPager );
      view->SetDatabasePager( pager.get() );
   
      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
      view->SetScene( scene.get() );

      scene->AddDrawable( new TestDrawable() );

      CPPUNIT_ASSERT_EQUAL_MESSAGE("SHould be 2",
                                    size_t(2),
                                    testPager->GetPagedLODList().size() );
   }
}
