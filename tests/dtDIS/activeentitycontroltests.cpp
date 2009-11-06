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

#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/uniqueid.h>
#include <DIS/EntityID.h>
#include <dtDIS/activeentitycontrol.h>

namespace dtDIS
{
   class ActiveControlTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ActiveControlTests );
      CPPUNIT_TEST( TestAdding );
      CPPUNIT_TEST( TestRemoving );
      CPPUNIT_TEST( EntityIDCompare );
      CPPUNIT_TEST_SUITE_END();

      void setup();
      void teardown();

      void TestAdding();
      void TestRemoving();
      void EntityIDCompare();
   };

   ////////////////////////////////////////////////////////////////////////////////
   void ActiveControlTests::setup()
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActiveControlTests::teardown()
   {

   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActiveControlTests::TestAdding()
   {
      dtDIS::ActiveEntityControl control;

      DIS::EntityID eid;
      eid.setSite(23);
      eid.setApplication(24);
      eid.setEntity(25);

      const dtCore::UniqueId uid;
      bool result = control.AddEntity(eid, uid);
      CPPUNIT_ASSERT_MESSAGE("Couldn't add a DIS entity to UniqueID mapping",
                              result == true);

      const dtCore::UniqueId* returnUID = control.GetActor(eid);
      CPPUNIT_ASSERT_MESSAGE("Returned UniqueID was NULL", returnUID != NULL);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned UniqueID isn't what was expected",
                                    uid, *returnUID);

      const DIS::EntityID* returnEID =  control.GetEntity(uid);
      CPPUNIT_ASSERT_MESSAGE("Returned EntityID was NULL", returnEID != NULL);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned EntityID isn't what was expected",
                             eid.getApplication(), returnEID->getApplication());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned EntityID isn't what was expected",
                             eid.getSite(), returnEID->getSite());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned EntityID isn't what was expected",
                             eid.getEntity(), returnEID->getEntity());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActiveControlTests::TestRemoving()
   {
      dtDIS::ActiveEntityControl control;

      DIS::EntityID eid1;
      eid1.setSite(23);
      eid1.setApplication(24);
      eid1.setEntity(25);
      const dtCore::UniqueId uid1;
      control.AddEntity(eid1, uid1);

      DIS::EntityID eid2;
      eid2.setSite(13);
      eid2.setApplication(14);
      eid2.setEntity(15);
      const dtCore::UniqueId uid2;
      control.AddEntity(eid2, uid2);

      //remove 2nd one
      bool result = control.RemoveEntity(eid2, uid2);
      CPPUNIT_ASSERT_MESSAGE("Couldn't remove a DIS entity to UniqueID mapping",
                             result == true);

      const dtCore::UniqueId* returnUID = control.GetActor(eid2);
      CPPUNIT_ASSERT_MESSAGE("Actor didn't get removed",
                              returnUID == NULL);

      //ensure first one is still there
      CPPUNIT_ASSERT_MESSAGE("First Actor should not have been removed",
                              control.GetActor(eid1) != NULL);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActiveControlTests::EntityIDCompare()
   {
      DIS::EntityID left;
      left.setApplication(1);
      left.setSite(10);
      left.setEntity(5);

      DIS::EntityID right;
      right.setApplication(10);
      right.setSite(1);
      right.setEntity(50);

      dtDIS::details::EntityIDCompare compare;

      bool lrResult = compare(left, right);
      bool rlResult = compare(right, left);

      CPPUNIT_ASSERT_MESSAGE("dtDIS::EntityIDCompare failed the 'Strict Weak Ordering'",
                             lrResult != rlResult);
   }

   CPPUNIT_TEST_SUITE_REGISTRATION( ActiveControlTests );
}
