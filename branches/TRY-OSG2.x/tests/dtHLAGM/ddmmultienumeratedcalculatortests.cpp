/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <dtCore/refptr.h>
#include <dtHLAGM/ddmmultienumeratedcalculator.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmutil.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/coordinates.h>

class DDMMultiEnumeratedCalculatorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMMultiEnumeratedCalculatorTests);
   
      CPPUNIT_TEST(TestEnabledProperties);
      CPPUNIT_TEST(TestMinMaxProperties);
      CPPUNIT_TEST(TestRegion1);
      CPPUNIT_TEST(TestRegion2);
      CPPUNIT_TEST(TestRegion3);
      CPPUNIT_TEST(TestRegion3No2);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mCalc = new dtHLAGM::DDMMultiEnumeratedCalculator;
      }
      
      void tearDown()
      {
         mCalc = NULL;
      }
                  
      void TestEnabledProperties()
      {   
         TestProperty<dtDAL::BooleanActorProperty, bool>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_ENABLED, false, true);
         CPPUNIT_ASSERT(mCalc->IsSecondDimensionEnabled());

         TestProperty<dtDAL::BooleanActorProperty, bool>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_ENABLED, false, true);
         CPPUNIT_ASSERT(mCalc->IsThirdDimensionEnabled());
      }
         
      void TestMinMaxProperties()
      {   
         TestProperty<dtDAL::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_MIN, 0, 5);
         CPPUNIT_ASSERT(mCalc->GetSecondDimensionMinimum());

         TestProperty<dtDAL::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_MAX, 1, 12);
         CPPUNIT_ASSERT(mCalc->GetSecondDimensionMaximum());

         TestProperty<dtDAL::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_MIN, 0, 5);
         CPPUNIT_ASSERT(mCalc->GetThirdDimensionMinimum());

         TestProperty<dtDAL::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_MAX, 1, 12);
         CPPUNIT_ASSERT(mCalc->GetThirdDimensionMaximum());

         TestProperty<dtDAL::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_VALUE, 0, 5);
         CPPUNIT_ASSERT(mCalc->GetSecondDimensionValue());

         TestProperty<dtDAL::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_VALUE, 0, 12);
         CPPUNIT_ASSERT(mCalc->GetThirdDimensionValue());
      }

      void TestRegions(unsigned dimCount)
      {
         std::vector<dtCore::RefPtr<dtHLAGM::DDMRegionData> > toFill;
         mCalc->CreateSubscriptionRegionData(toFill);
         CPPUNIT_ASSERT_EQUAL(1U, unsigned(toFill.size()));
         dtHLAGM::DDMRegionData& regData = *toFill[0];
         CPPUNIT_ASSERT(mCalc->UpdateRegionData(regData));
         CPPUNIT_ASSERT_EQUAL(dimCount, regData.GetNumberOfExtents());
         
         const dtHLAGM::DDMRegionData::DimensionValues* dv = regData.GetDimensionValue(0);
         CPPUNIT_ASSERT(dv != NULL);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Min and max values should be the same", dv->mMin, dv->mMax);
         unsigned long expectedMappedValue = dtHLAGM::DDMUtil::MapEnumerated(unsigned(mCalc->GetDefaultAppSpace()), 
               unsigned(mCalc->GetAppSpaceMinimum()), unsigned(mCalc->GetAppSpaceMaximum()));  
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Mapped value is not correct.", expectedMappedValue, dv->mMin);
                  
         if (dimCount > 1)
         {
            dv = regData.GetDimensionValue(1);
            CPPUNIT_ASSERT(dv != NULL);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Min and max values should be the same", dv->mMin, dv->mMax);
            expectedMappedValue = dtHLAGM::DDMUtil::MapEnumerated(unsigned(mCalc->GetSecondDimensionValue()), 
                  unsigned(mCalc->GetSecondDimensionMinimum()), unsigned(mCalc->GetSecondDimensionMaximum()));  
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Mapped value is not correct.", expectedMappedValue, dv->mMin);
            
            if (dimCount > 2)
            {
               dv = regData.GetDimensionValue(2);
               CPPUNIT_ASSERT(dv != NULL);
               CPPUNIT_ASSERT_EQUAL_MESSAGE("Min and max values should be the same", dv->mMin, dv->mMax);
               expectedMappedValue = dtHLAGM::DDMUtil::MapEnumerated(unsigned(mCalc->GetThirdDimensionValue()), 
                     unsigned(mCalc->GetThirdDimensionMinimum()), unsigned(mCalc->GetThirdDimensionMaximum()));  
               CPPUNIT_ASSERT_EQUAL_MESSAGE("Mapped value is not correct.", expectedMappedValue, dv->mMin);
               
            }
         }
      }
      
      void TestRegion1()
      {
         TestRegions(1);
      }
      
      void TestRegion2()
      {
         mCalc->SetSecondDimensionEnabled(true);
         TestRegions(2);
      }
      
      void TestRegion3()
      {
         mCalc->SetSecondDimensionEnabled(true);
         mCalc->SetThirdDimensionEnabled(true);
         TestRegions(3);
      }
      
      void TestRegion3No2()
      {
         mCalc->SetSecondDimensionEnabled(false);
         mCalc->SetThirdDimensionEnabled(true);
         TestRegions(1);
      }

   private:
      
      template <class PropType, typename PropDataType>
      void TestProperty(const std::string& name, PropDataType defaultValue, PropDataType testSetValue)
      {
         PropType* testProp = NULL;
         mCalc->GetProperty(name, testProp);
         CPPUNIT_ASSERT(testProp != NULL);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The property \"" + name + "\" does not have the correct default." , defaultValue, testProp->GetValue());
         testProp->SetValue(testSetValue);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The property \"" + name + "\" should have a new value." , testSetValue, testProp->GetValue());         
      }

      dtCore::RefPtr<dtHLAGM::DDMMultiEnumeratedCalculator> mCalc; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMMultiEnumeratedCalculatorTests);
