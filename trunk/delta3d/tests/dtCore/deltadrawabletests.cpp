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

#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <ode/collision.h>
#include <sstream>

using namespace dtCore;

class DeltaDrawableTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DeltaDrawableTests);  
   CPPUNIT_TEST(TestOrphanedDrawables);
   CPPUNIT_TEST(FailedGetChildByIndex);
   CPPUNIT_TEST(ValidGetChildByIndex);
   CPPUNIT_TEST(TestActive);
   CPPUNIT_TEST_SUITE_END();

public:
   
   void TestOrphanedDrawables();
   void TestParentChildRelationships();
   void FailedGetChildByIndex();
   void ValidGetChildByIndex();
   void TestActive();

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

class TestDrawable : public dtCore::DeltaDrawable
{
public:
   TestDrawable():
      mNode(new osg::Group())
   {      
   }

   ~TestDrawable() {};

   const osg::Node *GetOSGNode(void) const {return mNode.get();}
   osg::Node *GetOSGNode(void) { return mNode.get(); }

private:
   osg::ref_ptr<osg::Group> mNode;
};


void DeltaDrawableTests::TestActive()
{
   using namespace dtCore;

   RefPtr<TestDrawable> draw = new TestDrawable();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should be enabled", true, draw->GetActive());
   CPPUNIT_ASSERT_MESSAGE("Node mask should not be 0x0", unsigned(0x0) != draw->GetOSGNode()->getNodeMask() );

   draw->SetActive(false);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("DeltaDrawable should be disabled", false, draw->GetActive());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Node mask should be 0x0", unsigned(0x0), draw->GetOSGNode()->getNodeMask() );


   //check if the node mask we set it to remains after toggling it off/on
   draw->SetActive(true);
   const unsigned int nodeMask = 0x00001111;
   draw->GetOSGNode()->setNodeMask(nodeMask);
   draw->SetActive(false);
   draw->SetActive(true);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Node mask should be what it was set to before being disabled",
                                 nodeMask, draw->GetOSGNode()->getNodeMask());
}
