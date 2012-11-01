/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
* @author Olen A. Bruce
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <string>
#include <sstream>
#include <dtHLAGM/ddmregiondata.h>
#include <dtCore/refptr.h>

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
