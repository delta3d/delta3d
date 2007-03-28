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
 * @author Olen A. Bruce
 * @author David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <string>
#include <sstream>
#include <dtHLAGM/ddmregiondata.h>

class DDMRegionDataTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMRegionDataTests);
   
      CPPUNIT_TEST(TestDefaults);
      CPPUNIT_TEST(TestGetSet);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mTestDDMRegionData = new dtHLAGM::DDMRegionData();
      }
      
      void tearDown()
      {
         mTestDDMRegionData = NULL;
      }
      
      void TestDefaults()
      {
         CPPUNIT_ASSERT(mTestDDMRegionData->GetRegion() == NULL);
         CPPUNIT_ASSERT_EQUAL(0U, mTestDDMRegionData->GetNumberOfExtents());
         CPPUNIT_ASSERT(mTestDDMRegionData->GetDimensionValue(0) == NULL);
      }
      
      void TestGetSet()
      {
         dtHLAGM::DDMRegionData::DimensionValues d[3];
         d[0].mMax = 500;
         d[0].mMin = 0;
         d[0].mName = "subspace";
         d[1].mMax = 300;
         d[1].mMin = 200;
         d[1].mName = "one";
         d[2].mMax = 12;
         d[2].mMin = 11;
         d[2].mName = "two";

         mTestDDMRegionData->SetDimensionValue(0, d[0]);
         CheckDimensions(d, 1);
         
         mTestDDMRegionData->SetDimensionValue(1, d[1]);
         CheckDimensions(d, 2);

         mTestDDMRegionData->SetDimensionValue(2, d[2]);
         CheckDimensions(d, 3);
      }
   
   private:
      
      void CheckDimensions(const dtHLAGM::DDMRegionData::DimensionValues d[], unsigned count)
      {
         CPPUNIT_ASSERT_EQUAL(unsigned(count), mTestDDMRegionData->GetNumberOfExtents());
         for (unsigned i = 0; i != count; ++i)
         {
            std::ostringstream ss;
            ss << "DimensionValue " << i << " should not be NULL because there should be " << count << " items in the list.";
            CPPUNIT_ASSERT_MESSAGE(ss.str(), mTestDDMRegionData->GetDimensionValue(i) != NULL);
            const dtHLAGM::DDMRegionData::DimensionValues& dActual = *mTestDDMRegionData->GetDimensionValue(i);
            CPPUNIT_ASSERT_MESSAGE("The dimension values returned by the region data object should match the ones passed in.",
                 dActual == d[i]);
         }
         CPPUNIT_ASSERT_MESSAGE("Reading on past the end should return ", mTestDDMRegionData->GetDimensionValue(count) == NULL);
      }
      
      dtCore::RefPtr<dtHLAGM::DDMRegionData> mTestDDMRegionData;
   
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMRegionDataTests);
