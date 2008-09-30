/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2008, Alion Science and Technology Corporation
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
* David Guthrie
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/refstring.h>
#include <sstream>

namespace dtUtil
{
   /// Math unit tests for dtUtil
   class RefStringTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(RefStringTests);
         CPPUNIT_TEST( TestCopyConstructorAndAssignment );
         CPPUNIT_TEST( TestSamePointer );
         CPPUNIT_TEST( TestOperators );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp()
         {
            
         }

         void tearDown()
         {
            
         }

         void TestCopyConstructorAndAssignment()
         {
            const std::string testString("booga booga booga");
            const std::string testString2("booly booly");
            dtUtil::RefString r1(testString);
            dtUtil::RefString r2(r1);

            //Test conversion operator
            CPPUNIT_ASSERT_EQUAL(static_cast<std::string>(r2), testString);

            //Test pointers are equal
            CPPUNIT_ASSERT(&r1.Get() == &r2.Get());
            //Test == operator
            CPPUNIT_ASSERT(r1 == r2);
            //Test == and != string
            CPPUNIT_ASSERT(r2 == testString);
            CPPUNIT_ASSERT(r2 != testString2);

            r1 = testString2;
            //Test == and != string
            CPPUNIT_ASSERT(r1 == testString2);
            CPPUNIT_ASSERT(r1 != testString);
            //Test conversion operator
            CPPUNIT_ASSERT_EQUAL(static_cast<std::string>(r1), testString2);
            CPPUNIT_ASSERT(&r1.Get() != &r2.Get());
            //Test != operator
            CPPUNIT_ASSERT(r1 != r2);

            //Test assignment operator
            r2 = r1;
            CPPUNIT_ASSERT(&r1.Get() == &r2.Get());
            CPPUNIT_ASSERT(r1 == r2);
         }

         void TestSamePointer()
         {
            const std::string testString("booga booga");

            const std::string* savePointer = NULL;

            {
               dtUtil::RefString one(testString);
               dtUtil::RefString two(testString);
               dtUtil::RefString three(testString);
               dtUtil::RefString four(testString);

               CPPUNIT_ASSERT(&one.Get() == &two.Get());
               CPPUNIT_ASSERT(&two.Get() == &three.Get());
               CPPUNIT_ASSERT(&three.Get() == &four.Get());
               savePointer = &one.Get();
            }

            {
               dtUtil::RefString five(testString);
               CPPUNIT_ASSERT_MESSAGE("Even after all the string go out of scope, I should still get the same value.", 
                        &five.Get() == savePointer);
            }
         }

         void TestOperators()
         {
            const std::string testString("booya booya");
            dtUtil::RefString one(testString);
            CPPUNIT_ASSERT_EQUAL(testString.size(), one->size());

            CPPUNIT_ASSERT_EQUAL(one[0], 'b');

            std::ostringstream ss;
            //test auto conversion to string
            ss.str(one);

            CPPUNIT_ASSERT_EQUAL(testString, ss.str());

            ss.str("");
            ss << static_cast<std::string>(one);
            CPPUNIT_ASSERT_EQUAL(testString, ss.str());
         }

      private:
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( RefStringTests );
}
