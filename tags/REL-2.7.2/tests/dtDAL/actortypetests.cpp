/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
* @author Matthew W. Campbell
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/system.h>
#include <dtCore/actortype.h>
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
      dtCore::RefPtr<dtCore::ActorType> parentType = new dtCore::ActorType("ParentType","Tests");
      dtCore::RefPtr<dtCore::ActorType> childOne = new dtCore::ActorType("ChildOne","Tests","This is child one.",parentType.get());
      dtCore::RefPtr<dtCore::ActorType> childTwo = new dtCore::ActorType("ChildTwo","Tests","This is child two.",childOne.get());
      dtCore::RefPtr<dtCore::ActorType> childThree = new dtCore::ActorType("ChildThree","Tests","This is child three.",parentType.get());      
      
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
