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
* David Guthrie
*/
#include <prefix/unittestprefix.h>

#include <dtCore/refptr.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/longactorproperty.h>

#include <dtHLAGM/ddmmultienumeratedcalculator.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/ddmutil.h>

#include <dtUtil/coordinates.h>

#include <cppunit/extensions/HelperMacros.h>

#include <string>

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
         TestProperty<dtCore::BooleanActorProperty, bool>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_ENABLED, false, true);
         CPPUNIT_ASSERT(mCalc->IsSecondDimensionEnabled());

         TestProperty<dtCore::BooleanActorProperty, bool>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_ENABLED, false, true);
         CPPUNIT_ASSERT(mCalc->IsThirdDimensionEnabled());
      }
         
      void TestMinMaxProperties()
      {   
         TestProperty<dtCore::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_MIN, 0, 5);
         CPPUNIT_ASSERT(mCalc->GetSecondDimensionMinimum());

         TestProperty<dtCore::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_MAX, 1, 12);
         CPPUNIT_ASSERT(mCalc->GetSecondDimensionMaximum());

         TestProperty<dtCore::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_MIN, 0, 5);
         CPPUNIT_ASSERT(mCalc->GetThirdDimensionMinimum());

         TestProperty<dtCore::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_MAX, 1, 12);
         CPPUNIT_ASSERT(mCalc->GetThirdDimensionMaximum());

         TestProperty<dtCore::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_SECOND_DIMENSION_VALUE, 0, 5);
         CPPUNIT_ASSERT(mCalc->GetSecondDimensionValue());

         TestProperty<dtCore::LongActorProperty, long>(dtHLAGM::DDMMultiEnumeratedCalculator::PROP_THIRD_DIMENSION_VALUE, 0, 12);
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
         unsigned int expectedMappedValue = dtHLAGM::DDMUtil::MapEnumerated(unsigned(mCalc->GetDefaultAppSpace()), 
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
