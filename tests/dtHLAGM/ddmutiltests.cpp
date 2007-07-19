/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
