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
* @author David Guthrie
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <string>
#include <iostream>
#include <dtHLAGM/ddmutil.h>

class DDMUtilTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMUtilTests);
   
      CPPUNIT_TEST(TestLinear);
      CPPUNIT_TEST(TestEnumerated);
      CPPUNIT_TEST(TestPartitioned);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
      }
      
      void tearDown()
      {
      }
      
      void TestLinear()
      {
         unsigned long result = dtHLAGM::DDMUtil::MapLinear(15.0, 0.0, 15.0);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::HLAGM_MAX_EXTENT, result);
         
         result = dtHLAGM::DDMUtil::MapLinear(0.0, 0.0, 15.0);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::HLAGM_MIN_EXTENT, result);

         result = dtHLAGM::DDMUtil::MapLinear(5.0, 0.0, 15.0);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::HLAGM_MAX_EXTENT/3, result);
         
         result = dtHLAGM::DDMUtil::MapLinear(7.5, 0.0, 15.0);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::HLAGM_MAX_EXTENT/2, result);

         result = dtHLAGM::DDMUtil::MapLinear(10.0, 0.0, 15.0);
         CPPUNIT_ASSERT_EQUAL(2 * (dtHLAGM::DDMUtil::HLAGM_MAX_EXTENT/3), result);
      }
      
      void TestEnumerated()
      {
         unsigned long result = dtHLAGM::DDMUtil::MapEnumerated(15, 0, 15);
         CPPUNIT_ASSERT_EQUAL(4160749567UL, result);
         
         result = dtHLAGM::DDMUtil::MapEnumerated(0, 0, 15);
         CPPUNIT_ASSERT_EQUAL(134217727UL, result);
         
         result = dtHLAGM::DDMUtil::MapEnumerated(8, 0, 16);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::HLAGM_MAX_EXTENT/2, result);         
      }

      void TestPartitioned()
      {
         std::vector<double> partitionValues;
         for (unsigned i = 0; i < 17 ; ++i)
         {
            partitionValues.push_back(double(i));
         }
         
         unsigned long result = dtHLAGM::DDMUtil::MapPartitioned(15.9, partitionValues);
         CPPUNIT_ASSERT_EQUAL(4160749567UL, result);
         
         result = dtHLAGM::DDMUtil::MapPartitioned(0.0, partitionValues);
         CPPUNIT_ASSERT_EQUAL(134217727UL, result);
         
         partitionValues.push_back(17.0);

         result = dtHLAGM::DDMUtil::MapPartitioned(8.0, partitionValues);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::HLAGM_MAX_EXTENT/2, result);
      }
   
   private:
      
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMUtilTests);
