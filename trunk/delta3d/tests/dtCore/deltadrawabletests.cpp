/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2007 MOVES Institute 
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
* Michael Guerrero
*/ 
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtCore/system.h>
#include <dtABC/application.h>

#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <ode/collision.h>
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

private:

   bool HasChild( dtCore::DeltaDrawable* parent, dtCore::DeltaDrawable* child );
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

CPPUNIT_TEST_SUITE_REGISTRATION(DeltaDrawableTests);

bool DeltaDrawableTests::HasChild( dtCore::DeltaDrawable* parent, dtCore::DeltaDrawable* child )
{
   for( unsigned int i = 0; i < parent->GetNumChildren(); ++i )
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
   dtCore::RefPtr<Transformable> childOne( new Transformable("ChildOne") );
   dtCore::RefPtr<Transformable> childTwo( new Transformable("ChildTwo") );

   // Create a new scope so that the parent will be 
   // destroyed leaving orphaned children
   {
      dtCore::RefPtr<Transformable> parent( new Transformable("Parent") );

      parent->AddChild( childOne.get() );
      parent->AddChild( childTwo.get() );   

      // Verify Delta3D hierarchy    
      CPPUNIT_ASSERT( HasChild( parent.get(), childOne.get() ) );
      CPPUNIT_ASSERT( HasChild( parent.get(), childTwo.get() ) ); 
   }   

   // Since the parent has been removed, the 
   // childrens parent pointers should be null
   CPPUNIT_ASSERT( childOne->GetParent() == NULL );
   CPPUNIT_ASSERT( childTwo->GetParent() == NULL );

   // Test orphans by emancipation
   dtCore::RefPtr<Transformable> parent( new Transformable("Parent") );

   parent->AddChild( childOne.get() );
   parent->AddChild( childTwo.get() );   

   // Verify Delta3D hierarchy    
   CPPUNIT_ASSERT( HasChild( parent.get(), childOne.get() ) );
   CPPUNIT_ASSERT( HasChild( parent.get(), childTwo.get() ) ); 

   childOne->Emancipate();
   childTwo->Emancipate();      

   // Since the parent has been removed, the 
   // childrens parent pointers should be null
   CPPUNIT_ASSERT( childOne->GetParent() == NULL );
   CPPUNIT_ASSERT( childTwo->GetParent() == NULL );

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
   dtCore::RefPtr<Transformable> parent( new Transformable("Parent") );

   CPPUNIT_ASSERT_MESSAGE("Should have returned NULL", NULL == parent->GetChild(0));
   CPPUNIT_ASSERT_MESSAGE("Should have returned NULL", NULL == parent->GetChild(1));

   dtCore::RefPtr<Transformable> childOne( new Transformable("ChildOne") );
   parent->AddChild( childOne.get() );
   CPPUNIT_ASSERT_MESSAGE("Should have returned NULL", NULL == parent->GetChild(1));
}

void DeltaDrawableTests::ValidGetChildByIndex()
{
   dtCore::RefPtr<Transformable> parent( new Transformable("Parent") );
   dtCore::RefPtr<Transformable> childOne( new Transformable("ChildOne") );
   dtCore::RefPtr<Transformable> childTwo( new Transformable("ChildTwo") );
   
   parent->AddChild(childOne.get());
   parent->AddChild(childTwo.get());

   CPPUNIT_ASSERT_MESSAGE("Should be the first child", parent->GetChild(0) == childOne.get() );
   CPPUNIT_ASSERT_MESSAGE("Should be the second child", parent->GetChild(1) == childTwo.get() );
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

   const osg::Node *GetOSGNode(void) const {return mNode.get();}
   osg::Node *GetOSGNode(void) { return mNode.get(); }

   osg::ref_ptr<osg::Group> mNode;
   osg::ref_ptr<CullCallback> mCullCallback;
};

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestActive()
{
   //control test.  Verify defaults case, should render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should be enabled", true, draw->GetActive());

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddDrawable(draw.get());

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should have rendered", true, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveDrawable(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestDeactive()
{
   //Added to scene, deactivated.  Should not render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddDrawable(draw.get());
   
   draw->SetActive(false);

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should not have rendered", false, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveDrawable(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestDeactiveThenAddedToScene()
{
   //deactive, added to scene.  Should not render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   draw->SetActive(false);

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddDrawable(draw.get());

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should not have rendered", false, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveDrawable(draw.get());
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawableTests::TestDeactiveAddedToSceneThenActive()
{
   //deactived, added to scene, then activated. Should render
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   draw->SetActive(false);

   RefPtr<Scene> scene = GetGlobalApplication().GetScene();
   scene->AddDrawable(draw.get());

   draw->SetActive(true);

   System::GetInstance().Start();
   System::GetInstance().Step();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should have rendered", true, draw->mCullCallback->mTraversed);

   System::GetInstance().Stop();
   scene->RemoveDrawable(draw.get());
}
