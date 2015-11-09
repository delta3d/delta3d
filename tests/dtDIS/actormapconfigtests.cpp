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

#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>
#include <DIS/EntityType.h>
#include <dtDIS/entitymap.h>
#include <dtDIS/sharedstate.h>
#include <dtCore/actorfactory.h>

namespace dtDIS
{
   class ActorMapConfigTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ActorMapConfigTests );
      CPPUNIT_TEST( EntityTypeCompare );
      CPPUNIT_TEST(TestActorTypeAdding);
      CPPUNIT_TEST(TestActorTypeRemovingNoAddedResource);
      CPPUNIT_TEST(TestActorTypeRemovingWithAddedResource);
      CPPUNIT_TEST(TestResourceDescriptorAdding);
      CPPUNIT_TEST(TestResourceDescriptorRemovingNoAddedActorType);
      CPPUNIT_TEST(TestResourceDescriptorRemovingWithAddedActorType);
      CPPUNIT_TEST_SUITE_END();

      void setup(){};
      void teardown(){};

      void EntityTypeCompare();
      void TestActorTypeAdding();
      void TestActorTypeRemovingNoAddedResource();
      void TestActorTypeRemovingWithAddedResource();
      void TestResourceDescriptorAdding();
      void TestResourceDescriptorRemovingNoAddedActorType();
      void TestResourceDescriptorRemovingWithAddedActorType();
   };

   ////////////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::EntityTypeCompare()
   {
      DIS::EntityType left;
      left.setCategory(1);
      left.setCountry(2);
      left.setDomain(3);
      left.setEntityKind(4);
      left.setExtra(5);
      left.setSpecific(6);
      left.setSubcategory(7);

      DIS::EntityType right; //a little different
      right.setCategory(2);
      right.setCountry(1);
      right.setDomain(3);
      right.setEntityKind(4);
      right.setExtra(5);
      right.setSpecific(6);
      right.setSubcategory(7);

      dtDIS::details::EntityTypeCompare compare;
      bool lrResult = compare(left, right);
      bool rlResult = compare(right, left);

      CPPUNIT_ASSERT_MESSAGE("dtDIS::EntityTypeCompare failed the 'Strict Weak Ordering'",
                             lrResult != rlResult);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::TestActorTypeAdding()
   {
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setDomain(0);
      eid.setCountry(3);
      eid.setCategory(2);
      eid.setSubcategory(0);
      eid.setSpecific(0);

      dtCore::ActorTypeVec actorTypes;
      dtCore::ActorFactory::GetInstance().GetActorTypes(actorTypes);
      
      EntityMap mapping;
      mapping.SetEntityActorType(eid, actorTypes[0]);

      const dtCore::ActorType* foundType = mapping.GetMappedActorType(eid);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap didn't return back the correct ActorType",
                                   actorTypes[0], foundType);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::TestActorTypeRemovingNoAddedResource()
   {
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setDomain(0);
      eid.setCountry(3);
      eid.setCategory(2);
      eid.setSubcategory(0);
      eid.setSpecific(0);

      dtCore::ActorTypeVec actorTypes;
      dtCore::ActorFactory::GetInstance().GetActorTypes(actorTypes);

      EntityMap mapping;
      mapping.SetEntityActorType(eid, actorTypes[0]);
      mapping.RemoveEntityActorType(eid); //should have removed all reference to eid

      const dtCore::ActorType* actorType = mapping.GetMappedActorType(eid);
      const dtCore::ActorType* nullType = NULL;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap didn't remove the reference to the EntityType",
                                    nullType, actorType);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::TestActorTypeRemovingWithAddedResource()
   {
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setDomain(0);
      eid.setCountry(3);
      eid.setCategory(2);
      eid.setSubcategory(0);
      eid.setSpecific(0);

      dtCore::ActorTypeVec actorTypes;
      dtCore::ActorFactory::GetInstance().GetActorTypes(actorTypes);
      const dtCore::ResourceDescriptor rd("name", "description");

      EntityMap mapping;
      mapping.SetEntityActorType(eid, actorTypes[0]);
      mapping.SetEntityResource(eid, rd);
      mapping.RemoveEntityActorType(eid); //should have only removed the ActorType for this EntityType

      const dtCore::ActorType* actorType = mapping.GetMappedActorType(eid);
      const dtCore::ActorType* nullType = NULL;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap didn't remove the reference to the EntityType",
                                    nullType, actorType);

      const dtCore::ResourceDescriptor& returnedRD = mapping.GetMappedResource(eid);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap should not have removed the reference to the ResourceDescriptor",
                                   rd, returnedRD);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::TestResourceDescriptorAdding()
   {
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setDomain(0);
      eid.setCountry(3);
      eid.setCategory(2);
      eid.setSubcategory(0);
      eid.setSpecific(0);

      const dtCore::ResourceDescriptor rd("name", "description");

      EntityMap mapping;
      mapping.SetEntityResource(eid, rd);

      const dtCore::ResourceDescriptor& returnedRD = mapping.GetMappedResource(eid);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap didn't return back the correct ResourceDescriptor",
                                  rd, returnedRD);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::TestResourceDescriptorRemovingNoAddedActorType()
   {
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setDomain(0);
      eid.setCountry(3);
      eid.setCategory(2);
      eid.setSubcategory(0);
      eid.setSpecific(0);

      const dtCore::ResourceDescriptor rd("name", "description");

      EntityMap mapping;
      mapping.SetEntityResource(eid, rd);
      mapping.RemoveEntityResource(eid); //should have removed all reference to eid

      const dtCore::ResourceDescriptor& returnedRD = mapping.GetMappedResource(eid);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap didn't remove the reference to the EntityType",
                                   dtCore::ResourceDescriptor::NULL_RESOURCE, returnedRD);

   }

   //////////////////////////////////////////////////////////////////////////
   void ActorMapConfigTests::TestResourceDescriptorRemovingWithAddedActorType()
   {
      DIS::EntityType eid;
      eid.setEntityKind(1);
      eid.setDomain(0);
      eid.setCountry(3);
      eid.setCategory(2);
      eid.setSubcategory(0);
      eid.setSpecific(0);

      dtCore::ActorTypeVec actorTypes;
      dtCore::ActorFactory::GetInstance().GetActorTypes(actorTypes);
      const dtCore::ResourceDescriptor rd("name", "description");

      EntityMap mapping;
      mapping.SetEntityActorType(eid, actorTypes[0]);
      mapping.SetEntityResource(eid, rd);
      mapping.RemoveEntityResource(eid); //should have only removed the ResourceDescriptor for this EntityType

      const dtCore::ActorType* actorType = mapping.GetMappedActorType(eid);      
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap shouldn't remove the reference to the ActorType",
         actorTypes[0], actorType);

      const dtCore::ResourceDescriptor& returnedRD = mapping.GetMappedResource(eid);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("EntityMap should have removed the reference to the ResourceDescriptor",
                                    dtCore::ResourceDescriptor::NULL_RESOURCE, returnedRD);
   }

   CPPUNIT_TEST_SUITE_REGISTRATION( ActorMapConfigTests );

}
