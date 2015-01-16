/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, MOVES Institute
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
 * Michael Guerrero
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtCore/system.h>
#include <dtABC/application.h>

#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <sstream>

using namespace dtCore;

extern dtABC::Application& GetGlobalApplication();

class DeltaDrawableTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(DeltaDrawableTests);
      CPPUNIT_TEST(TestOrphanedDrawables);
      CPPUNIT_TEST(FailedGetChildByIndex);
      CPPUNIT_TEST(ValidGetChildByIndex);
      CPPUNIT_TEST(TestActive);
      CPPUNIT_TEST(TestDeactive);
      CPPUNIT_TEST(TestDeactiveThenAddedToScene);
      CPPUNIT_TEST(TestDeactiveAddedToSceneThenActive);
      CPPUNIT_TEST(TestAddedAndRemovedCallbacks);
      CPPUNIT_TEST(TestAddingItselfAsAChild);
      CPPUNIT_TEST(TestAddingChildWithExistingParent);
      CPPUNIT_TEST(TestShaderGroup);
   CPPUNIT_TEST_SUITE_END();

public:
   void TestOrphanedDrawables();
   void TestParentChildRelationships();
   void FailedGetChildByIndex();
   void ValidGetChildByIndex();
   void TestActive();
   void TestDeactive();
   void TestDeactiveThenAddedToScene();
   void TestDeactiveAddedToSceneThenActive();
   void TestAddedAndRemovedCallbacks();
   void TestAddingItselfAsAChild();
   void TestAddingChildWithExistingParent();
   void TestShaderGroup();

private:
   bool HasChild(dtCore::DeltaDrawable* parent, dtCore::DeltaDrawable* child);
};

class TestTransformable : public dtCore::Transformable
{
   ~TestTransformable()
   {
      if (GetParent())
      {
         GetParent()->RemoveChild(this);
      }
   };
};

class TestAddedAndRemovedDrawable : public dtCore::DeltaDrawable
{
public:
   TestAddedAndRemovedDrawable()
   : mNode(new osg::Group())
   , mAddedCalled(0)
   , mRemovedCalled(0)
   {

   }

   virtual void AddedToScene(Scene* scene)
   {
      dtCore::DeltaDrawable::AddedToScene(scene);
      if (scene == NULL)
      {
         CPPUNIT_ASSERT(GetSceneParent() == NULL);
      }
      ++mAddedCalled;
   }

   virtual void RemovedFromScene(Scene* scene)
   {
      dtCore::DeltaDrawable::RemovedFromScene(scene);
      CPPUNIT_ASSERT(GetSceneParent() == NULL);
      ++mRemovedCalled;
   }

   void ResetCalled()
   {
      mAddedCalled = 0;
      mRemovedCalled = 0;
   }

   const osg::Node* GetOSGNode(void) const {return mNode.get();}
   osg::Node* GetOSGNode(void) { return mNode.get(); }

   osg::ref_ptr<osg::Group> mNode;

   int mAddedCalled;
   int mRemovedCalled;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeltaDrawableTests);

bool DeltaDrawableTests::HasChild(dtCore::DeltaDrawable* parent, dtCore::DeltaDrawable* child)
{
   for (unsigned int i = 0; i < parent->GetNumChildren(); ++i)
   {
      if (parent->GetChild(i) == child)
      {
         return true;
      }
   }

   return false;
}


void DeltaDrawableTests::TestOrphanedDrawables()
{
   dtCore::RefPtr<Transformable> childOne(new Transformable("ChildOne"));
   dtCore::RefPtr<Transformable> childTwo(new Transformable("ChildTwo"));

   // Create a new scope so that the parent will be
   // destroyed leaving orphaned children
   {
      dtCore::RefPtr<Transformable> parent(new Transformable("Parent"));

      parent->AddChild(childOne.get());
      parent->AddChild(childTwo.get());

      // Verify Delta3D hierarchy
      CPPUNIT_ASSERT(HasChild(parent.get(), childOne.get()));
      CPPUNIT_ASSERT(HasChild(parent.get(), childTwo.get()));
   }

   // Since the parent has been removed, the
   // childrens parent pointers should be null
   CPPUNIT_ASSERT(childOne->GetParent() == NULL);
   CPPUNIT_ASSERT(childTwo->GetParent() == NULL);

   // Test orphans by emancipation
   dtCore::RefPtr<Transformable> parent(new Transformable("Parent"));

   parent->AddChild(childOne.get());
   parent->AddChild(childTwo.get());

   // Verify Delta3D hierarchy
   CPPUNIT_ASSERT(HasChild(parent.get(), childOne.get()));
   CPPUNIT_ASSERT(HasChild(parent.get(), childTwo.get()));

   childOne->Emancipate();
   childTwo->Emancipate();

   // Since the parent has been removed, the
   // childrens parent pointers should be null
   CPPUNIT_ASSERT(childOne->GetParent() == NULL);
   CPPUNIT_ASSERT(childTwo->GetParent() == NULL);

   // Next test emancipating children who are already orphaned or parentless
   childOne->Emancipate();
   childTwo->Emancipate();

   // Without the OnOrphaned call, this will make the
   // transformable's destructor get called twice.
   // http://www.delta3d.org/forum/viewtopic.php?forum=13&showtopic=9055
   {
      dtCore::RefPtr<dtCore::Transformable> parent = new dtCore::Transformable;

      parent->AddChild(new TestTransformable());
   }   // parent goes out of scope here
}

void DeltaDrawableTests::FailedGetChildByIndex()
{
   dtCore::RefPtr<Transformable> parent(new Transformable("Parent"));

   CPPUNIT_ASSERT_MESSAGE("Should have returned NULL", NULL == parent->GetChild(0));
   CPPUNIT_ASSERT_MESSAGE("Should have returned NULL", NULL == parent->GetChild(1));

   dtCore::RefPtr<Transformable> childOne(new Transformable("ChildOne"));
   parent->AddChild(childOne.get());
   CPPUNIT_ASSERT_MESSAGE("Should have returned NULL", NULL == parent->GetChild(1));
}

void DeltaDrawableTests::ValidGetChildByIndex()
{
   dtCore::RefPtr<Transformable> parent(new Transformable("Parent"));
   dtCore::RefPtr<Transformable> childOne(new Transformable("ChildOne"));
   dtCore::RefPtr<Transformable> childTwo(new Transformable("ChildTwo"));

   parent->AddChild(childOne.get());
   parent->AddChild(childTwo.get());

   CPPUNIT_ASSERT_MESSAGE("Should be the first child", parent->GetChild(0) == childOne.get());
   CPPUNIT_ASSERT_MESSAGE("Should be the second child", parent->GetChild(1) == childTwo.get());
}

//////////////////////////////////////////////////////////////////////////
class TestDrawable : public dtCore::DeltaDrawable
{
public:
   class CullCallback : public osg::NodeCallback
   {
   public:
      CullCallback():
         mTraversed(false)
         {}

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         mTraversed = true;
         traverse(node,nv);
      }

      bool mTraversed;
   };

   TestDrawable():
      mNode(new osg::Group()),
      mCullCallback(new CullCallback())
   {
      mNode->setCullCallback(mCullCallback.get());
   }

   ~TestDrawable() {};

   const osg::Node* GetOSGNode(void) const {return mNode.get();}
   osg::Node* GetOSGNode(void) { return mNode.get(); }

   osg::ref_ptr<osg::Group> mNode;
   osg::ref_ptr<CullCallback> mCullCallback;
};

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestActive()
{
   // control test.  Verify defaults case, should render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should be enabled", true, draw->GetActive());

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddChild(draw.get());

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should have rendered", true, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveChild(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestDeactive()
{
   //Added to scene, deactivated.  Should not render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddChild(draw.get());

   draw->SetActive(false);

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should not have rendered", false, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveChild(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestDeactiveThenAddedToScene()
{
   //deactive, added to scene.  Should not render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   draw->SetActive(false);

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddChild(draw.get());

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should not have rendered", false, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveChild(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestDeactiveAddedToSceneThenActive()
{
   //deactived, added to scene, then activated. Should render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   draw->SetActive(false);

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddChild(draw.get());

   draw->SetActive(true);

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should have rendered", true, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveChild(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestAddedAndRemovedCallbacks()
{
   dtCore::RefPtr<TestAddedAndRemovedDrawable> drawable = new TestAddedAndRemovedDrawable;

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddChild(drawable.get());

   CPPUNIT_ASSERT_EQUAL(1, drawable->mAddedCalled);
   CPPUNIT_ASSERT_EQUAL(0, drawable->mRemovedCalled);

   drawable->ResetCalled();
   scene->RemoveChild(drawable.get());

   CPPUNIT_ASSERT_EQUAL(1, drawable->mAddedCalled);
   CPPUNIT_ASSERT_EQUAL(1, drawable->mRemovedCalled);

   drawable->ResetCalled();
   scene->AddChild(drawable.get());

   dtCore::RefPtr<TestAddedAndRemovedDrawable> subDrawable = new TestAddedAndRemovedDrawable;
   drawable->AddChild(subDrawable.get());

   CPPUNIT_ASSERT_EQUAL(1, subDrawable->mAddedCalled);
   CPPUNIT_ASSERT_EQUAL(0, subDrawable->mRemovedCalled);

   subDrawable->ResetCalled();
   drawable->RemoveChild(subDrawable.get());

   CPPUNIT_ASSERT_EQUAL(1, subDrawable->mAddedCalled);
   CPPUNIT_ASSERT_EQUAL(1, subDrawable->mRemovedCalled);

   drawable->AddChild(subDrawable.get());

   drawable->ResetCalled();
   subDrawable->ResetCalled();

   scene->RemoveChild(drawable.get());

   CPPUNIT_ASSERT_EQUAL(1, drawable->mAddedCalled);
   CPPUNIT_ASSERT_EQUAL(1, drawable->mRemovedCalled);

   CPPUNIT_ASSERT_EQUAL(1, subDrawable->mAddedCalled);
   CPPUNIT_ASSERT_EQUAL(1, subDrawable->mRemovedCalled);
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestAddingItselfAsAChild()
{
   using namespace dtCore;
   RefPtr<Transformable> drawable1 = new Transformable();

   bool added = drawable1->AddChild(drawable1.get());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should not be able to add a DeltaDrawable as a child of itself", false, added);
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestAddingChildWithExistingParent()
{
   using namespace dtCore;
   RefPtr<Transformable> parent1 = new Transformable("parent1");
   RefPtr<Transformable> parent2 = new Transformable("parent2");
   RefPtr<Transformable> child = new Transformable("child");

   parent1->AddChild(child);

   bool result = parent2->AddChild(child);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should not be able to add a DeltaDrawable as a child if it already has a parent", false, result);
}

//////////////////////////////////////////////////////////////////////////
class TestShaderDrawable : public dtCore::DeltaDrawable
{
public:
   typedef dtCore::DeltaDrawable BaseClass;

   bool mShaderGroupChanged;

   TestShaderDrawable(const std::string& name)
      : BaseClass(name)
      , mShaderGroupChanged(false)
   {}

   virtual osg::Node* GetOSGNode() 
   {
      return NULL;
   }

   virtual const osg::Node* GetOSGNode() const
   {
      return NULL;
   }

   virtual void OnShaderGroupChanged()
   {
      mShaderGroupChanged = true;
   }

protected:
   virtual ~TestShaderDrawable() {}
};

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestShaderGroup()
{
   using namespace dtCore;
   RefPtr<TestShaderDrawable> obj = new TestShaderDrawable("TestObject");

   CPPUNIT_ASSERT( ! obj->mShaderGroupChanged);

   std::string shaderGroup("MultiMap");
   CPPUNIT_ASSERT(obj->GetShaderGroup().empty());
   obj->SetShaderGroup(shaderGroup);
   CPPUNIT_ASSERT(obj->GetShaderGroup() == shaderGroup);

   CPPUNIT_ASSERT(obj->mShaderGroupChanged);
}
