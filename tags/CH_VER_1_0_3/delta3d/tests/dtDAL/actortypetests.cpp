/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/system.h>
#include <dtDAL/actortype.h>
#include <dtUtil/exception.h>
 
class ActorTypeTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(ActorTypeTests);   
      CPPUNIT_TEST(TestActorType);
   CPPUNIT_TEST_SUITE_END();
   
   public:
      void setUp();
      void tearDown();      
      void TestActorType();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ActorTypeTests);

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void ActorTypeTests::setUp()
{
   try 
   {
     
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL((std::string("Error: ") + ex.What()).c_str());
   }
      
}

//////////////////////////////////////////////////////////////////////////
void ActorTypeTests::tearDown()
{
   try 
   {
    
   } 
   catch (const dtUtil::Exception& e) 
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }   
}

//////////////////////////////////////////////////////////////////////////
void ActorTypeTests::TestActorType()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> parentType = new dtDAL::ActorType("ParentType","Tests");
      dtCore::RefPtr<dtDAL::ActorType> childOne = new dtDAL::ActorType("ChildOne","Tests","This is child one.",parentType.get());
      dtCore::RefPtr<dtDAL::ActorType> childTwo = new dtDAL::ActorType("ChildTwo","Tests","This is child two.",childOne.get());
      dtCore::RefPtr<dtDAL::ActorType> childThree = new dtDAL::ActorType("ChildThree","Tests","This is child three.",parentType.get());      
      
      CPPUNIT_ASSERT_MESSAGE("ParentActorType should not have had a parent.",
         parentType->GetParentActorType() == NULL);
      CPPUNIT_ASSERT_MESSAGE("ChildOne should have parent type as parent.",
         childOne->GetParentActorType() == parentType.get());
      CPPUNIT_ASSERT_MESSAGE("ChildTwo should have parent type as parent.",
         childTwo->GetParentActorType() == childOne.get());
      CPPUNIT_ASSERT_MESSAGE("ChildThree should have parent type as parent.",
         childThree->GetParentActorType() == parentType.get());
      
      //Check hierarchy..
      CPPUNIT_ASSERT_MESSAGE("Parent type should not have been an instance of childone.",
         parentType->InstanceOf(*childOne) == false);
      CPPUNIT_ASSERT_MESSAGE("Child one should have been an instance of parent type.",
         childOne->InstanceOf(*parentType) == true);
      CPPUNIT_ASSERT_MESSAGE("Child two should have been an instance of child one.",
         childTwo->InstanceOf(*childOne) == true);
      CPPUNIT_ASSERT_MESSAGE("Child two should have been an instance of parent type.",
         childTwo->InstanceOf(*parentType) == true);
      CPPUNIT_ASSERT_MESSAGE("Child three should have been an instance of parent type.",
         childThree->InstanceOf(*parentType) == true);
      CPPUNIT_ASSERT_MESSAGE("Child three should not have been an instance of childtwo.",
         childThree->InstanceOf(*childTwo) == false);
      
      //Check hierarchy using the string versions..
      CPPUNIT_ASSERT_MESSAGE("Parent type should not have been an instance of childone. (String version)",
         parentType->InstanceOf("Tests","ChildOne") == false);
      CPPUNIT_ASSERT_MESSAGE("Child one should have been an instance of parent type. (String version)" ,
         childOne->InstanceOf("Tests","ParentType") == true);
      CPPUNIT_ASSERT_MESSAGE("Child two should have been an instance of child one. (String version)",
         childTwo->InstanceOf("Tests","ChildOne") == true);
      CPPUNIT_ASSERT_MESSAGE("Child two should have been an instance of parent type. (String version)",
         childTwo->InstanceOf("Tests","ParentType") == true);
      CPPUNIT_ASSERT_MESSAGE("Child three should have been an instance of parent type. (String version)",
         childThree->InstanceOf("Tests","ParentType") == true);
      CPPUNIT_ASSERT_MESSAGE("Child three should not have been an instance of childtwo. (String version)",
         childThree->InstanceOf("Tests","ChildTwo") == false);
         
      //Make sure the actor type knows it is an instance of itself.
      CPPUNIT_ASSERT_MESSAGE("Child three should have been an instance of itself.",childThree->InstanceOf(*childThree));
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}
