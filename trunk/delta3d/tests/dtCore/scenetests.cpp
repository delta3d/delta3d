/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Eddie Johnson
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/light.h>
#include <dtCore/pointaxis.h>
#include <dtCore/environment.h>
#include <dtUtil/stringutils.h>

#include <osg/Group>
#include <osg/Geode>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>

class CoreTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(CoreTests);

      CPPUNIT_TEST(TestAssignToView);
      CPPUNIT_TEST(TestScene);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();
      void TestAssignToView();
      void TestScene();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CoreTests);

void CoreTests::setUp()
{

}

void CoreTests::tearDown()
{

}

class TestSceneForSceneTests : public dtCore::Scene
{
public:
   TestSceneForSceneTests(const std::string& name = ""): Scene(name) {};
   bool GetIsAssignedToView(dtCore::View& view) const { return IsAssignedToView(view); }
};


void CoreTests::TestAssignToView()
{
   dtCore::RefPtr<dtCore::View> view = new dtCore::View;
   dtCore::RefPtr<TestSceneForSceneTests> scene = new TestSceneForSceneTests;

   CPPUNIT_ASSERT(!scene->GetIsAssignedToView(*view));

   // Add two drawables to test so that setting the database pager will run the code to re-add them to the
   // pager.  I need a way to test if the drawables are actually added to the pager.
   scene->AddDrawable(new dtCore::PointAxis());
   scene->AddDrawable(new dtCore::Environment());

   view->SetScene(scene.get());

   CPPUNIT_ASSERT(scene->GetIsAssignedToView(*view));
}

void CoreTests::TestScene()
{
   const std::string sceneName("ChickenMan");
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene(sceneName);
   CPPUNIT_ASSERT_EQUAL(sceneName, scene->GetName());

   //test setting scene node
   dtCore::RefPtr<osg::Group> grp = new osg::Group();
   grp->addChild(new osg::Group());
   grp->addChild(new osg::Geode());

   scene->AddDrawable(new dtCore::PointAxis());
   scene->AddDrawable(new dtCore::Environment());

   unsigned numSceneChildBefore = scene->GetNumberOfAddedDrawable();
   unsigned numGrpChildBefore = grp->getNumChildren();
   dtCore::RefPtr<osg::Group> sceneNode = scene->GetSceneNode();
   unsigned numSceneNodeChildBefore = sceneNode->getNumChildren();

   scene->SetSceneNode(grp.get());

   CPPUNIT_ASSERT_MESSAGE("After replacing the scene node, we should maintain the same number of drawables", scene->GetNumberOfAddedDrawable() == numSceneChildBefore);
   CPPUNIT_ASSERT_MESSAGE("After replacing the scene node, we should have the same number of osg nodes", grp->getNumChildren() == (numGrpChildBefore + numSceneNodeChildBefore));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("After replacing the scene node, our old scene node should not have any children",
                                 0U, unsigned(sceneNode->getNumChildren()));


   // The 0 light is the InfiniteLight
   const dtCore::Light* light0 = scene->GetLight(0);
   CPPUNIT_ASSERT_MESSAGE( std::string("Light number 0 should have a valid pointer but it is ")+dtUtil::ToString(light0), light0 != 0 );

   // The rest of the lights should be 0
   for( int i = 1; i < dtCore::MAX_LIGHTS; ++i )
   {
      std::string msg(std::string("Light number ")+dtUtil::ToString(i)+" should have a 0 pointer but it is "+dtUtil::ToString(scene->GetLight(i)));
      CPPUNIT_ASSERT_MESSAGE( msg, scene->GetLight(i) == 0 );
   }
// TODO ledocc
//   CPPUNIT_ASSERT_MESSAGE("The scene should not having paging enabled by default", !scene->IsPagingEnabled());
//
//   CPPUNIT_ASSERT_MESSAGE("There should not be any cameras", dtCore::Camera::GetInstanceCount() == 0);
//
//   scene->EnablePaging();
//
//   CPPUNIT_ASSERT_MESSAGE("EnablePaging was called, but there are no view so it should not be enabled", !scene->IsPagingEnabled());
//
//   dtCore::RefPtr<dtCore::View> view = new dtCore::View("TestView");
//
//   view->SetScene(scene.get());
//
//   scene->EnablePaging();
//
//   CPPUNIT_ASSERT_MESSAGE("A view was added to the scene so calling EnablePaging should now set the variable to true", scene->IsPagingEnabled());
//
//   osgDB::DatabasePager *dp = view->GetOsgViewerView()->getDatabasePager();
//
//   CPPUNIT_ASSERT_MESSAGE("Paging is enabled, so the database pager should not be NULL", dp != NULL);
//
//   scene->DisablePaging();
//
//   CPPUNIT_ASSERT_MESSAGE("Paging should be disabled", !scene->IsPagingEnabled());
//
//   dp = view->GetOsgViewerView()->getDatabasePager();
//
//   CPPUNIT_ASSERT_MESSAGE("The database pager should now be NULL", dp == NULL);
//
//   scene->EnablePaging();
//
//   CPPUNIT_ASSERT_MESSAGE("Paging should not be enabled, there are no cameras", !scene->IsPagingEnabled());
//
//   view = new dtCore::Camera("TestCamera");
//
//   scene->EnablePaging();
//
//   CPPUNIT_ASSERT_MESSAGE("A camera was instanced. Paging should be enabled", scene->IsPagingEnabled());
//
//   dp = osgDB::Registry::instance()->getDatabasePager();
//
//   CPPUNIT_ASSERT_MESSAGE("The database pager should again be valid", dp != NULL);
//
//   scene->DisablePaging();
//
//   CPPUNIT_ASSERT_MESSAGE("Paging should be disabled", !scene->IsPagingEnabled());
//
//   dp = osgDB::Registry::instance()->getDatabasePager();
//
//   CPPUNIT_ASSERT_MESSAGE("The database pager should again be NULL", dp == NULL);
}
