/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
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

#include <dtCore/view.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/refptr.h>
#include <osg/Version>

#include <dtCore/databasepager.h>
#include <osgDB/DatabasePager>

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

   class TestSceneForViewTests : public dtCore::Scene
   {
   public:
      TestSceneForViewTests() {};
      dtCore::DatabasePager* GetPager() {return GetDatabasePager();}
   };

   ////////////////////////////////////////////////////////////////////////
   void ViewTests::TestPagerPropogation()
   {
      //verify that the dtCore::DatabasePager gets passed to the Scene
      dtCore::RefPtr<dtCore::View> view = new dtCore::View();
      CPPUNIT_ASSERT_MESSAGE("View should have a default DatabasePager",
                              view->GetDatabasePager() != NULL);

      dtCore::RefPtr<TestSceneForViewTests> scene = new TestSceneForViewTests();
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
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION == 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 4 && OPENSCENEGRAPH_MINOR_VERSION <= 6
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

      dtCore::RefPtr<TestPager> testPager = new TestPager();

      dtCore::RefPtr<dtCore::DatabasePager> pager = new dtCore::DatabasePager( *testPager );
      view->SetDatabasePager( pager.get() );

      dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
      view->SetScene( scene.get() );

      scene->AddDrawable( new TestDrawable() );

      CPPUNIT_ASSERT_EQUAL_MESSAGE("SHould be 2",
                                    size_t(2),
                                    testPager->GetPagedLODList().size() );

      dtCore::RefPtr<TestPager> testPager2 = new TestPager();
      dtCore::RefPtr<dtCore::DatabasePager> pager2 = new dtCore::DatabasePager( *testPager2 );
      view->SetDatabasePager( pager2.get() );

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Should be 2, and should have worked when setting the pager AFTER setting the scene.",
                                    size_t(2),
                                    testPager2->GetPagedLODList().size() );
   }
}
