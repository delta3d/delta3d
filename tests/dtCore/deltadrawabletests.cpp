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
   CPPUNIT_TEST_SUITE_END();

public:
   
   void TestOrphanedDrawables();
   void TestParentChildRelationships();

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
   for( unsigned i = 0; i < parent->GetNumChildren(); ++i )
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
