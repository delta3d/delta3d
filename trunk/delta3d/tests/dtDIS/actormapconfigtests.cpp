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
#include <DIS/EntityType.h>
#include <dtDIS/sharedstate.h>

namespace dtDIS
{
   class ActorMapConfigTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ActorMapConfigTests );
      CPPUNIT_TEST( EntityTypeCompare );
      CPPUNIT_TEST_SUITE_END();

      void setup(){};
      void teardown(){};

      void EntityTypeCompare();
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

   CPPUNIT_TEST_SUITE_REGISTRATION( ActorMapConfigTests );

}
