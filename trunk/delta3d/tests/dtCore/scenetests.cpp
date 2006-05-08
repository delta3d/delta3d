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
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/light.h>
#include <dtUtil/stringutils.h>

#include <osgDB/DatabasePager>
#include <osgDB/Registry>

class CoreTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(CoreTests);

      CPPUNIT_TEST(TestScene);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();
      void TestScene();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CoreTests);

void CoreTests::setUp()
{

}

void CoreTests::tearDown()
{

}

void CoreTests::TestScene()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene; 

   // The 0 light is the InfiniteLight
   const dtCore::Light* light0 = scene->GetLight(0);
   CPPUNIT_ASSERT_MESSAGE( std::string("Light number 0 should have a valid pointer but it is ")+dtUtil::ToString(light0), light0 != 0 );

   // The rest of the lights should be 0
   for( int i = 1; i < dtCore::MAX_LIGHTS; ++i )
   {
      std::string msg(std::string("Light number ")+dtUtil::ToString(i)+" should have a 0 pointer but it is "+dtUtil::ToString(scene->GetLight(i)));
      CPPUNIT_ASSERT_MESSAGE( msg, scene->GetLight(i) == 0 );
   }

   CPPUNIT_ASSERT_MESSAGE("The scene should not having paging enabled by default", !scene->IsPagingEnabled());

   CPPUNIT_ASSERT_MESSAGE("There should not be any cameras", dtCore::Camera::GetInstanceCount() == 0);

   scene->EnablePaging();

   CPPUNIT_ASSERT_MESSAGE("EnablePaging was called, but there are no cameras so it should not be enabled", !scene->IsPagingEnabled());

   dtCore::RefPtr<dtCore::Camera> camera = new dtCore::Camera("TestCamera");

   camera->SetScene(scene.get());

   scene->EnablePaging();

   CPPUNIT_ASSERT_MESSAGE("A camera was added to the scene so calling EnablePaging should now set the variable to true", scene->IsPagingEnabled());

   osgDB::DatabasePager *dp = osgDB::Registry::instance()->getDatabasePager();

   CPPUNIT_ASSERT_MESSAGE("Paging is enabled, so the database pager should not be NULL", dp != NULL);

   camera = NULL;

   scene->DisablePaging();

   CPPUNIT_ASSERT_MESSAGE("Paging should be disabled", !scene->IsPagingEnabled());

   dp = osgDB::Registry::instance()->getDatabasePager();

   CPPUNIT_ASSERT_MESSAGE("The database pager should now be NULL", dp == NULL);

   scene->EnablePaging();

   CPPUNIT_ASSERT_MESSAGE("Paging should not be enabled, there are no cameras", !scene->IsPagingEnabled());

   camera = new dtCore::Camera("TestCamera");

   scene->EnablePaging();

   CPPUNIT_ASSERT_MESSAGE("A camera was instanced. Paging should be enabled", scene->IsPagingEnabled());

   dp = osgDB::Registry::instance()->getDatabasePager();

   CPPUNIT_ASSERT_MESSAGE("The database pager should again be valid", dp != NULL);

   scene->DisablePaging();

   CPPUNIT_ASSERT_MESSAGE("Paging should be disabled", !scene->IsPagingEnabled());

   dp = osgDB::Registry::instance()->getDatabasePager();

   CPPUNIT_ASSERT_MESSAGE("The database pager should again be NULL", dp == NULL);
}
