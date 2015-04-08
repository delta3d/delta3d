/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * @author Eddie Johnson
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/light.h>
#include <dtCore/pointaxis.h>
#include <dtCore/environment.h>
#include <dtCore/skybox.h>
#include <dtCore/system.h>
#include <dtCore/object.h>
#include <dtABC/application.h>
#include <dtUtil/stringutils.h>

#include <osg/Group>
#include <osg/Geode>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>

extern dtABC::Application& GetGlobalApplication();

class CoreTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(CoreTests);
      CPPUNIT_TEST(TestAssignToView);
      CPPUNIT_TEST(TestScene);
      CPPUNIT_TEST(TestHeightOfTerrainWithSkybox);
      CPPUNIT_TEST(TestGettingAllDeltaDrawablesInScene);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   void TestAssignToView();
   void TestScene();
   void TestHeightOfTerrainWithSkybox();
   void TestGettingAllDeltaDrawablesInScene();
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
   scene->AddChild(new dtCore::PointAxis());
   scene->AddChild(new dtCore::Environment());

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

   scene->AddChild(new dtCore::PointAxis());
   scene->AddChild(new dtCore::Environment());

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
   for (int i = 1; i < dtCore::MAX_LIGHTS; ++i)
   {
      std::string msg(std::string("Light number ") + dtUtil::ToString(i)+ " should have a 0 pointer but it is " + dtUtil::ToString(scene->GetLight(i)));
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

//////////////////////////////////////////////////////////////////////////
void CoreTests::TestHeightOfTerrainWithSkybox()
{
   dtCore::RefPtr<dtABC::Application> application = &GetGlobalApplication();
   dtCore::RefPtr<dtCore::Environment> env = new dtCore::Environment();
   dtCore::RefPtr<dtCore::SkyBox> skybox = new dtCore::SkyBox("skybox", dtCore::SkyBox::RP_FIXED_FUNCTION);
   env->AddEffect(skybox.get());

   application->AddDrawable(env.get());
   dtCore::System::GetInstance().Start();
   dtCore::System::GetInstance().Config();
   dtCore::System::GetInstance().Step();

   float hot = 0.0f;
   bool success = application->GetScene()->GetHeightOfTerrain(hot, 0.0f, 0.0f);

   CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The height of terrain of an empty scene should be zero.",
                                        0.0f, hot, 0.0001f);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Getting the height of the terrain in an empty scene should fail.",
      false, success);
}

//////////////////////////////////////////////////////////////////////////
void CoreTests::TestGettingAllDeltaDrawablesInScene()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();

   // the Scene automatically adds a light as a DeltaDrawble.  We'll remove it
   // so that it won't affect out test
   scene->UseSceneLight(false);
   scene->RemoveChild(scene->GetLight(0));

   dtCore::RefPtr<dtCore::Object> obj1 = new dtCore::Object("one");
   dtCore::RefPtr<dtCore::Object> obj2 = new dtCore::Object("two");
   dtCore::RefPtr<dtCore::Object> obj3 = new dtCore::Object("three");
   dtCore::RefPtr<dtCore::Object> obj4 = new dtCore::Object("four");

   obj1->AddChild(obj2.get());
   obj2->AddChild(obj3.get());
   obj2->AddChild(obj4.get());

   scene->AddChild(obj1.get());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene should just have one DeltaDrawable added",
                                (unsigned int)1, scene->GetNumberOfAddedDrawable());

   std::vector<dtCore::DeltaDrawable*> drawables = scene->GetAllDrawablesInTheScene();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene should have returned all of the DeltaDrawables in the Scene",
                                size_t(4), drawables.size());

   bool foundOne(false);
   bool foundTwo(false);
   bool foundThree(false);
   bool foundFour(false);

   std::vector<dtCore::DeltaDrawable*>::iterator itr = drawables.begin();
   while (itr != drawables.end())
   {
      if ((*itr) == obj1.get())  {foundOne = true; }
      if ((*itr) == obj2.get())  {foundTwo = true; }
      if ((*itr) == obj3.get())  {foundThree = true; }
      if ((*itr) == obj4.get())  {foundFour = true; }

      ++itr;
   }

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene did not return an Object in the Scene", true, foundOne);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene did not return an Object in the Scene", true, foundTwo);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene did not return an Object in the Scene", true, foundThree);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene did not return an Object in the Scene", true, foundFour);

   scene->RemoveAllDrawables();
}
